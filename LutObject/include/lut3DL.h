#ifndef __LUT_LIBRARY_LUT_3DL__
#define __LUT_LIBRARY_LUT_3DL__

#include "lutElement.h"
#include "lutErrors.h"
#include "string_view.h"
#include <fstream>
#include <sstream>
#include <iostream>


template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr> 
class CLut3DL
{
public:	
    LutElement::lutFileName const getLutFileName (void) {return m_lutName;}
    LutErrorCode::LutState getLastError(void) { return m_error; }

    LutErrorCode::LutState LoadFile(std::ifstream& lutFile)
    {
        /* clear file stream status */
        lutFile.clear();
        /* cleanup internal objects before parsing */
        _cleanup();

        lutFile.seekg(static_cast<std::streampos>(0), std::ios_base::beg);

        return m_error;
    }

    LutErrorCode::LutState LoadFile(const string_view& lutFileName)
    {
        LutErrorCode::LutState err = LutErrorCode::LutState::OK;
        if (!lutFileName.empty() && lutFileName != m_lutName)
        {
            std::ifstream file3DL{ lutFileName };
            if (!file3DL.good())
                return LutErrorCode::LutState::FileNotOpened;

            err = LoadFile(file3DL);
            file3DL.close();

            if (LutErrorCode::LutState::OK == err)
            m_lutName = lutFileName;
         }
         m_error = err;
         return err;
    }

    LutErrorCode::LutState LoadFile (const char* lutFileName)
    {
        return (nullptr != lutFileName && '\0' != lutFileName[0]) ? LoadFile(string_view{ lutFileName }) : LutErrorCode::LutState::GenericError;
    }


    LutErrorCode::LutState LoadFile(const std::string& lutFileName)
    {
        LutErrorCode::LutState err = LutErrorCode::LutState::OK;
        if (!lutFileName.empty() && lutFileName != m_lutName)
        {
            std::ifstream file3DL{ lutFileName };
            if (!file3DL.good())
                return LutErrorCode::LutState::FileNotOpened;

            err = LoadFile(file3DL);
            file3DL.close();

            if (LutErrorCode::LutState::OK == err)
                m_lutName = lutFileName;
        }
        m_error = err;
        return err;
    }

private:
	LutElement::lutTable3D<T>  m_lutBody;
	LutElement::lutFileName    m_lutName;
	LutElement::lutTitle       m_title;
	LutElement::lutSize        m_lutSize;
	LutErrorCode::LutState     m_error = LutErrorCode::LutState::NotInitialized;

    void _cleanup (void)
    {
       m_lutSize = 0;  
       m_error = LutErrorCode::LutState::NotInitialized;
       return;
    }
};


#endif // __LUT_LIBRARY_LUT_3DL__
