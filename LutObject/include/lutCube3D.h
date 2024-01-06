#ifndef __LUT_LIBRARY_LUT_CUBE_3D__
#define __LUT_LIBRARY_LUT_CUBE_3D__

#include "lutElement.h"
#include "lutErrors.h"
#include <fstream>
#include <sstream>
#include <iostream>

#if __cplusplus == 201703L /* C++17 only features */
#include <string_view>
#endif /* #if __cplusplus == 201703L */


template <typename T, typename = std::enable_if<std::is_floating_point<T>::value>::type>
class CCubeLut3D
{
public:
	CCubeLut3D(void) = default;
	virtual ~CCubeLut3D(void) = default;

	LutElement::lutFileName const getLutFileName (void) {return m_lutName;}
	size_t getLutSize (void) {return m_lutSize;}
	

	LutErrorCode::LutState LoadCubeFile (std::ifstream& lutFile)
	{
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
		std::string stringBuffer, keyword;
		constexpr T nonValidDomain = static_cast<T>(-1);
		bool bData = false;

		/* IN FIRST READ KEYWORDS */
		do {
			stringBuffer.clear(); /* cleanup string before read line from file */
			const std::streampos linePos = lutFile.tellg();
			if (LutErrorCode::LutState::OK == (loadStatus = ReadLine (lutFile, stringBuffer, lineSeparator)))
			{
				keyword.clear();
				std::istringstream line(stringBuffer);
				line >> keyword;

				if ("+" < keyword && ":" > keyword)
				{
					/* LUT data itself starting */
					lutFile.seekg(linePos, std::ios_base::beg);
					bData = true;
				}
				else if ("LUT_3D_SIZE" == keyword)
					loadStatus = set_lut_size(line);
				else if ("LUT_1D_SIZE" == keyword)
					return LutErrorCode::LutState::IncorrectDimension; /* because 3D and 1D CUBE LUT looks very similar we need protect here from incorrect read */
				else if ("DOMAIN_MIN" == keyword)
					loadStatus = set_domain_min_value(line);
				else if ("DOMAIN_MAX" == keyword)
					loadStatus = set_domain_max_value(line);
				else if ("TITLE" == keyword)
					loadStatus = read_lut_title(line);
				else
					loadStatus = LutErrorCode::LutState::UnknownOrRepeatedKeyword;
			} /* if (LutErrorCode::LutState::OK == (loadStatus = ReadLine(lutFile, stringBuffer, lineSeparator))) */
		} while (loadStatus == LutErrorCode::LutState::OK && false == bData);

		/* VALIDATE KEYWORDS */
		if (LutErrorCode::LutState::OK == keywords_validation())
		{
			/* READ LUT DATA */
		}
		return loadStatus;
	}


#if __cplusplus == 201703L /* C++17 only features */
	LutErrorCode::LutState LoadCubeFile (std::string_view lutFileName)
	{
		/* TODO */
	}
#endif /* #if __cplusplus == 201703L */


	LutErrorCode::LutState LoadCubeFile (const char* lutFileName)
	{
		return (nullptr != lutFileName && '\0' != lutFileName[0]) ? LoadCubeFile (std::string{ lutFileName }) : LutErrorCode::LutState::GenericError;
	}


	LutErrorCode::LutState LoadCubeFile (const std::string& lutFileName)
	{ 
		LutErrorCode::LutState err = LutErrorCode::LutState::OK;
		if (!lutFileName.empty() && lutFileName != m_lutName)
		{
			std::ifstream cubeFile3D { lutFileName };
			if (!cubeFile3D.good())
				return LutErrorCode::LutState::FileNotOpened;
			
			err = LoadCubeFile (cubeFile3D);
			cubeFile3D.close();

			if (LutErrorCode::LutState::OK == err)
				m_lutName = lutFileName;
		}
		return err;
	}


#if __cplusplus == 201703L /* C++17 only features */
	LutErrorCode::LutState SaveCubeFile (const std::string_view& fileName)
	{ 
		return LutErrorCode::LutState::NonImplemented; 
	}
#endif /* #if __cplusplus == 201703L */

	
	LutErrorCode::LutState SaveCubeFile (const std::string& fileName)
	{
		return LutErrorCode::LutState::NonImplemented;
	}

	LutErrorCode::LutState SaveCubeFile (std::ofstream& lutFile)
	{
		return LutErrorCode::LutState::NonImplemented;
	}


private:
	LutElement::lutTableRaw<T> m_domainMin;
	LutElement::lutTableRaw<T> m_domainMax;
	LutElement::lutTable3D<T>  m_lutBody;
	LutElement::lutFileName    m_lutName;
	LutElement::lutSize        m_lutSize;
	LutElement::lutTitle       m_title;

	const char symbNewLine        = '\n';
	const char symbCarriageReturn = '\r';
	const char symbCommentMarker  = '#';
	const char symbQuote          = '"';

	void _cleanup (void)
	{
		m_domainMin.clear();
		m_domainMax.clear();
		m_lutBody.clear();
		m_title.clear();
		m_lutSize = 0u;
		return;
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
		/* TODO */
		return LutErrorCode::LutState::OK;
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
			m_lutBody = std::move(LutElement::lutTable3D<T>(lutSize, LutElement::lutTable2D<T>(lutSize, LutElement::lutTable1D<T>(lutSize, LutElement::lutTableRaw<T>(3)))));
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


}; /* class CCubeLut3D */

#endif /* __LUT_LIBRARY_LUT_CUBE_3D__ */