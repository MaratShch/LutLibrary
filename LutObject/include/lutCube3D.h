#ifndef __LUT_LIBRARY_LUT_CUBE_3D__
#define __LUT_LIBRARY_LUT_CUBE_3D__

#include "lutElement.h"
#include "lutErrors.h"
#include "string_view.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <utility>
#include <cctype>

template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr> 
class CCubeLut3D
{
public:
	LutElement::lutFileName const getLutFileName (void) const {return m_lutName;}
	LutErrorCode::LutState getLastError(void) const { return m_error; }
	LutElement::lutSize getLutSize (void) const { return m_lutSize; }
	LutElement::lutSize getLutComponentSize (const LutElement::LutComponent component) const {(void)component; return getLutSize();}

    LutErrorCode::LutState LoadFile(std::ifstream& lutFile)
    {
        std::string stringBuffer, keyword;

        /* clear file stream status */
        lutFile.clear();
        /* cleanup internal objects before parsing */
        _cleanup();

        /* check line separator used in 3D CUBE file */
        const char lineSeparator = getLineSeparator(lutFile);
        if ('\0' == lineSeparator)
            return LutErrorCode::LutState::CouldNotParseTableData;

        /* unconditional seek on start of file for start parsing */
        lutFile.seekg(static_cast<std::streampos>(0), std::ios_base::beg);

        LutErrorCode::LutState loadStatus = LutErrorCode::LutState::OK;
        bool bData = false;

        /* IN FIRST READ KEYWORDS */
        do {
            stringBuffer.clear(); /* cleanup string before read line from file */
            const auto linePos = (lutFile.good() ? lutFile.tellg() : static_cast<std::streampos>(-1));
            if (LutErrorCode::LutState::OK == (loadStatus = ReadLine(lutFile, stringBuffer, lineSeparator)))
            {
                keyword.clear();
                std::istringstream line(stringBuffer);
                line >> keyword;

                if (std::isdigit(static_cast<unsigned char>(keyword[0])) || keyword[0] == '-' || keyword[0] == '.')
                {
                    /* LUT data itself starting */
                    lutFile.seekg(linePos, std::ios_base::beg);
                    bData = true;
                }
                else if ("TITLE" == keyword)
                    loadStatus = read_lut_title(line);
                else if ("LUT_3D_SIZE" == keyword)
                    loadStatus = set_lut_size(line);
                else if ("LUT_1D_SIZE" == keyword)
                    return LutErrorCode::LutState::IncorrectDimension; /* because 3D and 1D CUBE LUT looks very similar we need put protection here from incorrect read */
                else if ("DOMAIN_MIN" == keyword)
                    loadStatus = set_domain_min_value(line);
                else if ("DOMAIN_MAX" == keyword)
                    loadStatus = set_domain_max_value(line);
            } /* if (LutErrorCode::LutState::OK == (loadStatus = ReadLine(lutFile, stringBuffer, lineSeparator))) */
        } while (loadStatus == LutErrorCode::LutState::OK && false == bData);

        LutElement::lutSize r = 0u, g = 0u, b = 0u;

        // VALIDATE KEYWORDS AND READ LUT BODY
        if (LutErrorCode::LutState::OK == keywords_validation())
        {
            bool bNoBlob = true;
            const LutElement::lutSize lutLinesNumb = m_lutSize * m_lutSize * m_lutSize;
            for (LutElement::lutSize idx = 0; idx < lutLinesNumb; idx++)
            {
                // READ LUT DATA
                stringBuffer.clear();
                if (LutErrorCode::LutState::OK == (loadStatus = ReadLine(lutFile, stringBuffer, lineSeparator)))
                {
                    // skip #xxBLOB section
                    if (symbCommentMarker == stringBuffer[0])
                    {
                        bNoBlob = false;
                        continue;
                    }

                    const std::array<T, 3> lutLine = ParseTableRow(stringBuffer);
                    m_lutBody.insert(m_lutBody.end(), lutLine.cbegin(), lutLine.cend());
                }
            }

            loadStatus = lut_size_validation();
            m_error = loadStatus;
        }
        else
            loadStatus = LutErrorCode::LutState::GenericError;
        return loadStatus;
    }

	LutErrorCode::LutState LoadFile (const string_view& lutFileName)
	{
		LutErrorCode::LutState err = LutErrorCode::LutState::OK;
		if (!lutFileName.empty() && lutFileName != m_lutName)
		{
			std::ifstream cubeFile3D { lutFileName, std::ios::in | std::ios::binary };
			if (!cubeFile3D.good())
				return LutErrorCode::LutState::FileNotOpened;
			
			err = LoadFile (cubeFile3D);
			cubeFile3D.close();

			if (LutErrorCode::LutState::OK == err)
				m_lutName = lutFileName;
		}
		return err;
	}

	LutErrorCode::LutState LoadFile (const char* lutFileName)
	{
		return (nullptr != lutFileName && '\0' != lutFileName[0]) ? LoadFile (string_view{ lutFileName }) : LutErrorCode::LutState::GenericError;
	}


	LutErrorCode::LutState LoadFile (const std::string& lutFileName)
	{ 
		LutErrorCode::LutState err = LutErrorCode::LutState::OK;
		if (!lutFileName.empty() && lutFileName != m_lutName)
		{
			std::ifstream cubeFile3D { lutFileName, std::ios::in | std::ios::binary };
			if (!cubeFile3D.good())
				return LutErrorCode::LutState::FileNotOpened;
			
			err = LoadFile (cubeFile3D);
			cubeFile3D.close();

			if (LutErrorCode::LutState::OK == err)
				m_lutName = lutFileName;
		}
		return err;
	}


	LutErrorCode::LutState SaveFile (std::ofstream& outFile)
	{
		if (0 == m_lutSize)
			return LutErrorCode::LutState::NotInitialized;

		LutElement::lutSize r = 0, g = 0, b = 0;
		if (outFile.good())
		{
			outFile << symbCommentMarker << symbSpace << "This file created by LutLibrary" << std::endl;
			outFile << std::endl;
			if (m_title.size() > 0)
			{
				outFile << "TITLE" << symbSpace << symbQuote << m_title << symbQuote << std::endl;
				outFile << std::endl;
			}
			if (3 == m_domainMin.size() && 3 == m_domainMax.size())
			{
				outFile << "DOMAIN_MIN" << symbSpace << m_domainMin[0] << symbSpace << m_domainMin[1] << symbSpace << m_domainMin[2] << std::endl;
				outFile << "DOMAIN_MAX" << symbSpace << m_domainMax[0] << symbSpace << m_domainMax[1] << symbSpace << m_domainMax[2] << std::endl;
				outFile << std::endl;
			}
			outFile << "LUT_3D_SIZE" << symbSpace << m_lutSize << std::endl;
			outFile << std::endl;
			outFile.flush();

            const LutElement::lutSize totalElements = m_lutSize * m_lutSize * m_lutSize;
            const auto itBegin = m_lutBody.cbegin();
            const auto itEnd = m_lutBody.cend();
            auto it = itBegin;

            while (it + 2 < itEnd && outFile.good())
            {
                outFile << *it << symbSpace << *(it + 1) << symbSpace << *(it + 2) << std::endl;
                it += 3;
            }
            outFile << std::endl;
			outFile.flush(); /* flush file stream */
		}
		return (outFile.good() ? LutErrorCode::LutState::OK : LutErrorCode::LutState::WriteError);
	}

	LutErrorCode::LutState SaveFile (const string_view& fileName)
	{ 
		std::ofstream outFile (fileName, std::ios::out | std::ios::trunc);
		if (!outFile.good())
			return LutErrorCode::LutState::FileNotOpened;

		auto const err = SaveFile (outFile);
		outFile.close();
		return err;
	}


	LutErrorCode::LutState SaveFile (const char* fileName)
	{ 
		return (nullptr != fileName && '\0' != fileName[0]) ? SaveFile (string_view{ fileName }) : LutErrorCode::LutState::GenericError;
	}
	
	
	LutErrorCode::LutState SaveFile (const std::string& fileName)
	{
		std::ofstream outFile (fileName, std::ios::out | std::ios::trunc);
		if (!outFile.good())
			return LutErrorCode::LutState::FileNotOpened;

		auto const err = SaveFile (outFile);
		outFile.close();
		return err;
	}


   const LutElement::lutTable3D<T>& get_data(void) const noexcept { return m_lutBody; }
 
        
   const std::pair<LutElement::lutTableRaw<T>, LutElement::lutTableRaw<T>> getMinMaxDomain (void)
   {
      if (3 != m_domainMin.size())
      {
          m_domainMin.resize(3);
          m_domainMin[0] = m_domainMin[1] = m_domainMin[2] = static_cast<T>(0);
      }
      if (3 != m_domainMax.size())
      {
          m_domainMax.resize(3);
          m_domainMax[0] = m_domainMax[1] = m_domainMax[2] = static_cast<T>(1);
      }

      return std::make_pair(m_domainMin, m_domainMax); 
   }

private:
	LutElement::lutTableRaw<T>  m_domainMin;
	LutElement::lutTableRaw<T>  m_domainMax;
    LutElement::lutTable3D<T>   m_lutBody;
    LutElement::lutFileName     m_lutName;
	LutElement::lutTitle        m_title;
	LutElement::lutSize         m_lutSize;
	LutErrorCode::LutState      m_error = LutErrorCode::LutState::NotInitialized;

	static constexpr char symbNewLine        = '\n';
	static constexpr char symbCarriageReturn = '\r';
	static constexpr char symbCommentMarker  = '#';
	static constexpr char symbQuote          = '"';
	static constexpr char symbSpace          = ' ';

	void _cleanup (void)
	{
		m_domainMin.clear();
		m_domainMax.clear();
		m_lutBody.clear();
		m_lutName.clear();
		m_title.clear();
		m_lutSize = 0u;
		m_error = LutErrorCode::LutState::NotInitialized;
		return;
	}


	LutErrorCode::LutState lut_size_validation (void)
	{
		return ((0u == m_lutSize ? LutErrorCode::LutState::LutSizeInvalid : LutErrorCode::LutState::OK));
	}


	LutErrorCode::LutState keywords_validation (void)
	{
		/* validate LUT size */
		if (0u == m_lutSize)
			return LutErrorCode::LutState::LutSizeUnknown;

		/* validate DOMAIN_MIN and DOMAIN_MAX range */
		if (3 == m_domainMin.size() && 3 == m_domainMax.size())
			if (m_domainMin[0] > m_domainMax[0] || m_domainMin[1] > m_domainMax[1] || m_domainMin[2] > m_domainMax[2])
				return LutErrorCode::LutState::DomainBoundReversed;

		return LutErrorCode::LutState::OK;
	}


	LutErrorCode::LutState read_lut_title (std::istringstream& line)
	{
		LutErrorCode::LutState err = LutErrorCode::LutState::OK;
		char startOfTitle;
		line >> startOfTitle;
		if (symbQuote != startOfTitle)
		{
			err = LutErrorCode::LutState::TitleMissingQuote;
		}
		else
		{
			/* read till second quota character */
			std::getline(line, m_title, symbQuote);
		}

		return err;
	}


	LutErrorCode::LutState set_domain_min_value (std::istringstream& line)
	{
		m_domainMin.resize(3);
		line >> m_domainMin[0] >> m_domainMin[1] >> m_domainMin[2];
		return LutErrorCode::LutState::OK;
	}


	LutErrorCode::LutState set_domain_max_value (std::istringstream& line)
	{
		m_domainMax.resize(3);
		line >> m_domainMax[0] >> m_domainMax[1] >> m_domainMax[2];
		return LutErrorCode::LutState::OK;
	}


	LutErrorCode::LutState set_lut_size (std::istringstream& line)
	{
		int32_t lutSize = -1;
		line >> lutSize;
		if (lutSize >= 2 && lutSize <= 256)
		{
			m_lutSize = static_cast<decltype(m_lutSize)>(lutSize);
            const LutElement::lutSize vecMemSize = m_lutSize * m_lutSize * m_lutSize * static_cast<LutElement::lutSize>(3);
            m_lutBody.reserve(vecMemSize);
			return LutErrorCode::LutState::OK;
		}
		return LutErrorCode::LutState::LutSizeOutOfRange;
	}


	char getLineSeparator (std::ifstream& lutFile)
	{
		char lineSeparator { '\0' };
		for (int32_t i = 0; i < 256; i++)
		{
			auto const c = lutFile.get();
			if (c == static_cast<decltype(c)>(symbNewLine))
			{
				lineSeparator = symbNewLine;
				break;
			}

			if (c == static_cast<decltype(c)>(symbCarriageReturn))
			{
				if (symbCarriageReturn == lutFile.get())
					break;

				lineSeparator = symbCarriageReturn;
				std::cout << "This file uses non - complient line separator \\r(0x0D)" << std::endl;
				break;
			}
		}

		return lineSeparator;
	} /* char getLineSeparator (std::ifstream& lutFile) */


	LutErrorCode::LutState ReadLine (std::ifstream& lutFile, std::string& strBuffer, const char& lineSeparator)
	{
		while (0u == strBuffer.size() || symbCommentMarker == strBuffer[0])
		{
			if (lutFile.eof())
				return LutErrorCode::LutState::PrematureEndOfFile;

			std::getline(lutFile, strBuffer, lineSeparator);
			if (lutFile.fail())
				return LutErrorCode::LutState::ReadError;
		}
		return LutErrorCode::LutState::OK;
	} /* LutErrorCode::LutState ReadLine(std::ifstream& lutFile, std::string& strBuffer, const char& lineSeparator) */


    std::array<T, 3> ParseTableRow(const std::string& strBuffer)
    {
        constexpr size_t elementsInLine = 3ull;
        std::array<T, elementsInLine> lutRawData;
        std::istringstream data_line(strBuffer);

        for (int32_t i = 0; i < elementsInLine; i++)
        {
            data_line >> lutRawData[i];
            if (data_line.fail())
            {
                m_error = LutErrorCode::LutState::CouldNotParseTableData;
                break;
            }
        }

        return lutRawData;
    }

}; /* class CCubeLut3D */

#endif /* __LUT_LIBRARY_LUT_CUBE_3D__ */
