#ifndef __LUT_LIBRARY_LUT_CINE_SPACE_3D__
#define __LUT_LIBRARY_LUT_CINE_SPACE_3D__

#include "lutElement.h"
#include "lutErrors.h"
#include "string_view.h"
#include <fstream>
#include <sstream>
#include <iostream>


template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr> 
class CCineSpaceLut3D
{
 public:
 	LutElement::lutFileName const getLutFileName (void) {return m_lutName;}
	LutErrorCode::LutState getLastError  (void)         { return m_error; }
	LutElement::lutSize const getLutSize (void)         { return m_lutSize; }
	LutElement::lutSize const getLutComponentSize (const LutElement::LutComponent component) {return m_lutComponentSize[static_cast<uint32_t>(component)];}
	
	LutErrorCode::LutState LoadFile (std::ifstream& lutFile)
	{
		/* clear file stream status */
		lutFile.clear();
		/* cleanup internal objects before parsing */
		_cleanup();
		/* check line separator used in 3D CUBE file */
		const char lineSeparator = getLineSeparator(lutFile);
		if ('\0' == lineSeparator)
			return LutErrorCode::LutState::CouldNotParseTableData;

		lutFile.seekg(static_cast<std::streampos>(0), std::ios_base::beg);
		LutErrorCode::LutState loadStatus = LutErrorCode::LutState::OK;
		std::string stringBuffer;
		bool bMarkerFound = false;
		/* read first line - "CSPLUTV100" value mandatory */
		if (LutErrorCode::LutState::OK == (loadStatus = ReadLine (lutFile, stringBuffer, lineSeparator)))
		{
			if ("CSPLUTV100" == stringBuffer) /* CSP LUT marker detected */
			{
				/* read LUT dimension "3D" value mandatory */
				stringBuffer.clear();
				if (LutErrorCode::LutState::OK == (loadStatus = ReadLine (lutFile, stringBuffer, lineSeparator)))
				{
					if ("3D" == stringBuffer)
						bMarkerFound = true;
					else if ("1D" == stringBuffer)
						loadStatus = LutErrorCode::LutState::IncorrectDimension;
					else
						loadStatus = LutErrorCode::LutState::UnknownOrRepeatedKeyword;	
				}	
			}
			else
				loadStatus = LutErrorCode::LutState::CouldNotParseTableData;
		}
		
		if (false == bMarkerFound) return loadStatus;
		/* CSP markers found - continue parse ... */
		bool bMetaData = false;
		stringBuffer.clear();
		loadStatus = ReadLine(lutFile, stringBuffer, lineSeparator);
		if (LutErrorCode::LutState::OK == loadStatus && "BEGIN METADATA" == stringBuffer)
		{
			bMetaData = true;
			/* we found metadata section. Just skeep it */
			do {
				stringBuffer.clear();
				loadStatus = ReadLine (lutFile, stringBuffer, lineSeparator);
			} while (LutErrorCode::LutState::OK == loadStatus && "END METADATA" != stringBuffer);
		}

		if (true == bMetaData)
		{
			stringBuffer.clear();
			loadStatus = ReadLine(lutFile, stringBuffer, lineSeparator);
		}
		/* check if pre-LUT data available in CSP file */
		if (LutErrorCode::LutState::OK == loadStatus && true == is_pre_lut(stringBuffer))
		{
			loadStatus = readPreLutData(lutFile, stringBuffer, lineSeparator);
			stringBuffer.clear();
			loadStatus = ReadLine(lutFile, stringBuffer, lineSeparator);
		}
		
		if (LutErrorCode::LutState::OK == loadStatus && 0u != stringBuffer.size())
		{
			/* read LUT body (channels dimension and LUT values itself */
			std::istringstream lutDim (stringBuffer);
			for (uint32_t i = 0u; i < 3u; i++)
				lutDim >> m_lutComponentSize[i];
		}

		LutElement::lutSize r = 0u, g = 0u, b = 0u;
		bool bValid = true;
		if (0u != m_lutComponentSize[0] && 0u != m_lutComponentSize[1] && 0u != m_lutComponentSize[2])
		{
			/* resize vectors holds LUT tables */
			set_lut_size();
			/* load LUT table from file */
			for (b = 0u; b < m_lutComponentSize[0] && true == bValid; b++)
				for (g = 0u; g < m_lutComponentSize[1] && true == bValid; g++)
					for (r = 0u; r < m_lutComponentSize[2] && true == bValid; r++)
					{
						stringBuffer.clear();
						if (LutErrorCode::LutState::OK == (loadStatus = ReadLine(lutFile, stringBuffer, lineSeparator)))
							m_lutBody[r][g][b] = ParseTableRow (stringBuffer);
						else
							bValid = false;
					}
		}

		auto const componentMask = (m_lutComponentSize[0] && m_lutComponentSize[1] && m_lutComponentSize[2]);

		return (true == componentMask && b == m_lutComponentSize[0] && g == m_lutComponentSize[1] && r == m_lutComponentSize[2]) ?
			LutErrorCode::LutState::OK : LutErrorCode::LutState::CouldNotParseTableData;
	}
	
	LutErrorCode::LutState LoadFile (const string_view& lutFileName)
	{
		LutErrorCode::LutState err = LutErrorCode::LutState::OK;
		if (!lutFileName.empty() && lutFileName != m_lutName)
		{
			std::ifstream cspFile3D{ lutFileName };
			if (!cspFile3D.good())
				return LutErrorCode::LutState::FileNotOpened;

			err = LoadFile(cspFile3D);
			cspFile3D.close();

			if (LutErrorCode::LutState::OK == err)
				m_lutName = lutFileName;
		}
		m_error = err;
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
			std::ifstream cspFile3D{ lutFileName };
			if (!cspFile3D.good())
				return LutErrorCode::LutState::FileNotOpened;

			err = LoadFile(cspFile3D);
			cspFile3D.close();

			if (LutErrorCode::LutState::OK == err)
				m_lutName = lutFileName;
		}
		m_error = err;
		return err;
	}

	LutErrorCode::LutState SaveFile (std::ofstream& outFile)
	{
		return LutErrorCode::LutState::NonImplemented;
	}

	LutErrorCode::LutState SaveFile (const string_view& fileName)
	{ 
		return LutErrorCode::LutState::NonImplemented;
	}

	LutErrorCode::LutState SaveFile (const char* lutFileName)
	{
		return (nullptr != lutFileName && '\0' != lutFileName[0]) ? SaveFile (string_view{ lutFileName }) : LutErrorCode::LutState::GenericError;
	}

	LutErrorCode::LutState SaveFile (const std::string& fileName)
	{
		return LutErrorCode::LutState::NonImplemented;
	}

	
 private:
 	LutElement::lutTable3D<T>  m_lutBody;
 	LutElement::lutFileName    m_lutName;
	LutElement::lutSize        m_lutSize;
	LutElement::lutSize        m_lutComponentSize[3];
	LutErrorCode::LutState     m_error = LutErrorCode::LutState::NotInitialized;
	
	uint32_t m_preLutR;
	uint32_t m_preLutG;
	uint32_t m_preLutB;
	
	std::vector<T> m_preLut_R_in;
	std::vector<T> m_preLut_R_out;
	std::vector<T> m_preLut_G_in;
	std::vector<T> m_preLut_G_out;
	std::vector<T> m_preLut_B_in;
	std::vector<T> m_preLut_B_out;
	
	const std::string str_LutDimType {"3D"};
	static const char symbNewLine        = '\n';
	static const char symbCarriageReturn = '\r';
	static const char symbSpace          = ' ';

	void _cleanup (void)
	{
		m_lutBody.clear();
		m_preLut_R_in.clear();
		m_preLut_R_out.clear();
		m_preLut_G_in.clear();
		m_preLut_G_out.clear();
		m_preLut_B_in.clear();
		m_preLut_B_out.clear();
		m_lutSize = m_lutComponentSize[0] = m_lutComponentSize[1] = m_lutComponentSize[2] = 0u;
		m_preLutR  = m_preLutG  = m_preLutB = 0u;
		m_error = LutErrorCode::LutState::NotInitialized;
		return;
	}

	LutErrorCode::LutState readPreLutData
	(
		std::ifstream& lutFile, 
		std::string& strBuffer,
		uint32_t& preLutSize,
		std::vector<T>& in,
		std::vector<T>& out,
		const char& lineSeparator
	) noexcept
	{
		LutErrorCode::LutState err1 = LutErrorCode::LutState::ReadError;
		LutErrorCode::LutState err2 = LutErrorCode::LutState::ReadError;
		std::istringstream line_size (strBuffer);
		line_size >> preLutSize;
		in.resize (preLutSize);
		out.resize(preLutSize);
		strBuffer.clear();
		if (LutErrorCode::LutState::OK == (err1 = ReadLine(lutFile, strBuffer, lineSeparator)))
		{
			std::istringstream inStr(strBuffer);
			for (int32_t i = 0; i < preLutSize; i++)
				inStr >> in[i];
		}
		strBuffer.clear();
		if (LutErrorCode::LutState::OK == (err2 = ReadLine(lutFile, strBuffer, lineSeparator)))
		{
			std::istringstream outStr (strBuffer);
			for (int32_t i = 0; i < preLutSize; i++)
				outStr >> out[i];
		}
		return (LutErrorCode::LutState::OK == err1 && LutErrorCode::LutState::OK == err2 ? LutErrorCode::LutState::OK : LutErrorCode::LutState::ReadError);
	}

	LutErrorCode::LutState readPreLutData (std::ifstream& lutFile, std::string& strBuffer, const char& lineSeparator)
	{
		const auto rReadErr = readPreLutData (lutFile, strBuffer, m_preLutR, m_preLut_R_in, m_preLut_R_out, lineSeparator);
		strBuffer.clear();
		const auto rl1 = ReadLine (lutFile, strBuffer, lineSeparator);
		const auto gReadErr = readPreLutData (lutFile, strBuffer, m_preLutG, m_preLut_G_in, m_preLut_G_out, lineSeparator);
		strBuffer.clear();
		const auto rl2 = ReadLine (lutFile, strBuffer, lineSeparator);
		const auto bReadErr = readPreLutData (lutFile, strBuffer, m_preLutB, m_preLut_B_in, m_preLut_B_out, lineSeparator);

		return (LutErrorCode::LutState::OK == rReadErr && LutErrorCode::LutState::OK == gReadErr && LutErrorCode::LutState::OK == bReadErr &&
			LutErrorCode::LutState::OK == rl1 && LutErrorCode::LutState::OK == rl2 ? LutErrorCode::LutState::OK : LutErrorCode::LutState::ReadError);
	}


	bool is_pre_lut (const std::string& strBuffer)
	{
		bool one_digit = false;
		auto const strBufSize = strBuffer.size();
		for (auto i = 0; i < strBufSize; i++)
		{
			if ('+' < strBuffer[i] && ':' > strBuffer[i])
				one_digit = true;
			else
			{
				one_digit = false;
				break;
			}
		}
		return one_digit;
	}

	LutErrorCode::LutState ReadLine (std::ifstream& lutFile, std::string& strBuffer, const char& lineSeparator)
	{
		while (0u == strBuffer.size() || symbSpace == strBuffer[0])
		{
			if (lutFile.eof())
				return LutErrorCode::LutState::PrematureEndOfFile;

			std::getline(lutFile, strBuffer, lineSeparator);
			if (lutFile.fail())
				return LutErrorCode::LutState::ReadError;
		}
		return LutErrorCode::LutState::OK;
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

	LutElement::lutTableRaw<T> ParseTableRow (const std::string& strBuffer)
	{
		LutElement::lutTableRaw<T> lutRawData(3);
		std::istringstream data_line(strBuffer);

		for (int32_t i = 0; i < 3; i++)
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

	LutErrorCode::LutState set_lut_size (void)
	{
		if (m_lutComponentSize[0] >= 2 && m_lutComponentSize[0] <= 256 && m_lutComponentSize[1] >= 2 && m_lutComponentSize[1] && m_lutComponentSize[2] >= 2 && m_lutComponentSize[2])
		{
			/* let's set lutSize equal to minimal component size ??? */
			m_lutSize = std::min(m_lutComponentSize[0], std::min(m_lutComponentSize[1], m_lutComponentSize[2]));

			m_lutBody = std::move(LutElement::lutTable3D<T>(
				getLutComponentSize(LutElement::LutComponent::Blue),  LutElement::lutTable2D<T>(
				getLutComponentSize(LutElement::LutComponent::Green), LutElement::lutTable1D<T>(
				getLutComponentSize(LutElement::LutComponent::Red),   LutElement::lutTableRaw<T>(3))))
			);
			return LutErrorCode::LutState::OK;
		}
		return LutErrorCode::LutState::LutSizeOutOfRange;
	}


};

#endif /* __LUT_LIBRARY_LUT_CINE_SPACE_3D__ */
