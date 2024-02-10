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
	LutElement::lutSize const getLutComponentSize (const LutElement::LutComponent component) {return m_lutComponentSize[component];}
	
	LutErrorCode::LutState LoadFile (std::ifstream& lutFile)
	{
		/* clear file stream status */
		lutFile.clear();
		/* cleanup internal objects before parsing */
		_cleanup();
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
	
	const std::string str_LutMarker  {"CSPLUTV100"};
	const std::string str_LutDimType {"3D"};
	const char symbNewLine        = '\n';
	const char symbCarriageReturn = '\r';
	const char symbSpace          = ' ';

	void _cleanup (void)
	{
		m_lutBody.clear();
		m_preLut_R_in.clear();
		m_preLut_R_out.clear();
		m_preLut_G_in.clear();
		m_preLut_G_out.clear();
		m_preLut_B_in.clear();
		m_preLut_B_out.clear();
		m_lutSize = 0u;
		m_lutComponentSize[0] = m_lutComponentSize[1] = m_lutComponentSize[2] = 0u;
		m_preLutR  = m_preLutG  = m_preLutB = 0u;
		m_error = LutErrorCode::LutState::NotInitialized;
		return;
	}

};

#endif /* __LUT_LIBRARY_LUT_CINE_SPACE_3D__ */