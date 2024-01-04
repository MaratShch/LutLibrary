#ifndef __LUT_LIBRARY_LUT_CUBE_3D__
#define __LUT_LIBRARY_LUT_CUBE_3D__

#include "lutElement.h"
#include "lutErrors.h"
#include <fstream>
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
		/* check line separator used in CUBE file */
		const char lineSeparator = getLineSeparator(lutFile);

		/* unconditional seek on start of file for start parsing */
		lutFile.seekg(static_cast<std::streampos>(0), std::ios_base::beg);

		/* cleanup internal objects before parsing */
		_cleanup();

		return LutErrorCode::LutState::NonImplemented;
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
	LutElement::lutFileName    m_lutName{};

	std::string m_title{};
	size_t      m_lutSize = 0u;
	bool        m_blobIndicator = false;

	const char symbNewLine        = '\n';
	const char symbCarriageReturn = '\r';
	const char symbCommentMarker  = '#';

	void _cleanup (void)
	{
		m_domainMin.clear();
		m_domainMax.clear();
		m_lutBody.clear();
		m_title.clear();
		m_lutSize = 0u;
		m_blobIndicator = false;
		return;
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
	}

};

#endif /* __LUT_LIBRARY_LUT_CUBE_3D__ */