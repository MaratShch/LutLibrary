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

	LutErrorCode::LutState LoadFile (std::ifstream& lutFile)
	{
		return LutErrorCode::LutState::NonImplemented;
	}
	
	LutErrorCode::LutState LoadFile (const string_view& lutFileName)
	{
		return LutErrorCode::LutState::NonImplemented;
	}
	
	LutErrorCode::LutState LoadFile (const char* lutFileName)
	{
		return (nullptr != lutFileName && '\0' != lutFileName[0]) ? LoadFile (string_view{ lutFileName }) : LutErrorCode::LutState::GenericError;
	}

	LutErrorCode::LutState LoadFile (const std::string& lutFileName)
	{ 
		return LutErrorCode::LutState::NonImplemented;
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
	LutErrorCode::LutState     m_error = LutErrorCode::LutState::NotInitialized;
	
	const std::string str_LutMarker  {"CSPLUTV100"};
	const std::string str_LutDimType {"3D"};
	const char symbNewLine        = '\n';
	const char symbCarriageReturn = '\r';
	const char symbSpace          = ' ';

	void _cleanup (void)
	{
		m_lutBody.clear();
		m_lutSize = 0u;
		m_error = LutErrorCode::LutState::NotInitialized;
		return;
	}

};

#endif /* __LUT_LIBRARY_LUT_CINE_SPACE_3D__ */