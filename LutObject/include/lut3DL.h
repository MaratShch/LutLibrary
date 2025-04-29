#ifndef __LUT_LIBRARY_LUT_3DL__
#define __LUT_LIBRARY_LUT_3DL__

#include "lutElement.h"
#include "lutErrors.h"
#include "string_view.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <cmath>
#include <array>

template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr> 
class CLut3DL
{
public:	
    LutElement::lutFileName const getLutFileName (void) const {return m_lutName;}
    LutErrorCode::LutState getLastError(void) { return m_error; }
    LutElement::lutSize getLutSize(void) const { return m_lutSize; }
    LutElement::lutSize getLutComponentSize(const LutElement::LutComponent component) const { (void)component; return getLutSize(); }

    LutErrorCode::LutState LoadFile(std::ifstream& lutFile)
    {
        // clear file stream status
        lutFile.clear();
        // cleanup internal objects before start parse file content
        _cleanup();
        // move file pointer on start of file
        lutFile.seekg(static_cast<std::streampos>(0), std::ios_base::beg);

        /* check line separator used in 3D CUBE file */
        const char lineSeparator = getLineSeparator(lutFile);
        if ('\0' == lineSeparator)
            return LutErrorCode::LutState::CouldNotParseTableData;

        /* unconditional seek on start of file for start parsing */
        lutFile.seekg(static_cast<std::streampos>(0), std::ios_base::beg);

        LutErrorCode::LutState loadStatus = LutErrorCode::LutState::OK;
        bool bData = false;
        bool bGridLine = false;

        std::string stringBuffer, keyword;

        do {
            stringBuffer.clear();
            keyword.clear(); 
            if (LutErrorCode::LutState::OK == (loadStatus = ReadLine (lutFile, stringBuffer, lineSeparator)))
            {
                std::istringstream line(stringBuffer);
                line >> keyword;

                if (std::isdigit(static_cast<unsigned char>(keyword[0])) || '-' == keyword[0] || '.' == keyword[0])
                {
                    //  we read digits or numerical sign
                    if (false == bGridLine)
                    {
                        // check if we reed Grid Line or Lut Body
                        if (true == _containsMoreThanThreeNumbers(stringBuffer))
                        { 
                            fillGridLine (stringBuffer);
                            bGridLine = true;
                            continue; 
                        } 
                        else // possible GridLine not mandatory - so let's switch to parse LUT Body 
                        {
                            const std::array<T, 3> rowVal = ParseTableRow(stringBuffer);
                            std::copy(rowVal.begin(), rowVal.end(), std::back_inserter(m_lutBody));
                        }
                    } // if (false == bGridLine)
                    else
                    {
                        const std::array<T, 3> rowVal = ParseTableRow(stringBuffer);
                        std::copy(rowVal.begin(), rowVal.end(), std::back_inserter(m_lutBody));
                    }

                } // if (std::isdigit(static_cast<unsigned char>(keyword[0])) || keyword[0] == '-' || keyword[0] == '.')

            } // if (LutErrorCode::LutState::OK == (loadStatus = ReadLine (lutFile, stringBuffer, lineSeparator)))

        } while (loadStatus == LutErrorCode::LutState::OK);

        const size_t bodySize = m_lutBody.size();
        const size_t gridSize = m_gridLine.size();
        const size_t entries  = bodySize / 3;
        m_lutSize = static_cast<size_t>(std::cbrt(static_cast<T>(entries)));
        if (0ull != gridSize && gridSize != m_lutSize)
        { 
            std::cout << "Lut size not match to Lut Grid size!!!" << std::endl;
            std::cout << "Grid size = " << gridSize << " Lut size = " << m_lutSize << " [Body size = " << bodySize << "]" << std::endl;
            m_error = LutErrorCode::LutState::CouldNotParseTableData;
        } 
        else   
            m_error = LutErrorCode::LutState::OK;         
 
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


    LutErrorCode::LutState SaveFile(std::ofstream& outFile)
    {
        return LutErrorCode::LutState::NotInitialized;
    }


    LutErrorCode::LutState SaveFile(const string_view& fileName)
    {
        std::ofstream outFile(fileName, std::ios::out | std::ios::trunc);
        if (!outFile.good())
            return LutErrorCode::LutState::FileNotOpened;

        auto const err = SaveFile(outFile);
        outFile.close();
        return err;
    }


    LutErrorCode::LutState SaveFile(const char* fileName)
    {
        return (nullptr != fileName && '\0' != fileName[0]) ? SaveFile(string_view{ fileName }) : LutErrorCode::LutState::GenericError;
    }


    LutErrorCode::LutState SaveFile(const std::string& fileName)
    {
        return LutErrorCode::LutState::NotInitialized;
    }

    const LutElement::lutTable3DEx<T>& get_data(void) const noexcept { return m_lutBody; }


    const std::pair<T, T> get_inout_range(void) const { return std::make_pair(m_rangeIn, m_rangeOut); }


private:
    LutElement::lutTable3DEx<T> m_lutBody;
    LutElement::lutFileName     m_lutName;
    LutElement::lutTitle        m_title;
    LutElement::lutSize         m_lutSize;
    LutErrorCode::LutState      m_error = LutErrorCode::LutState::NotInitialized;

    std::vector<T> m_gridLine;
    std::string m_nativeComments;

    T m_rangeIn;
    T m_rangeOut;

    static const char symbNewLine        = '\n';
    static const char symbCarriageReturn = '\r';
    static const char symbCommentMarker  = '#';
    static const char symbQuote          = '"';
    static const char symbSpace          = ' ';


    void _cleanup (void)
    {
       m_lutBody.clear();
       m_title.clear();
       m_lutName.clear();
       m_lutSize = 0;
       m_gridLine.clear();
       m_nativeComments.clear();
       m_rangeIn = m_rangeOut = static_cast<T>(0);
       m_error = LutErrorCode::LutState::NotInitialized;
       return;
    }


    bool _containsMoreThanThreeNumbers (const std::string& line)
    {
        std::stringstream ss(line);
        std::string temp_token;
        int count = 0;

        // Count the numbers, stopping as soon as we find the 4th one.
        while (ss >> temp_token)
        {
            count++;
            if (count > 3)
                return true;
        }
        return false;
    }


    void fillGridLine (const std::string& line)
    {
        std::istringstream data_line (line);
        m_gridLine.clear();
        while (true)
        {
            T value{};
            data_line >> value;
            if (data_line.fail())
                break;
            m_gridLine.push_back(value);
        }

        return;
    }


    char getLineSeparator (std::ifstream& lutFile)
    {
        char lineSeparator{ '\0' };
        for (int32_t i = 0; i < 1024; i++)
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
            } // if (c == static_cast<decltype(c)>(symbCarriageReturn))

        }// for (int32_t i = 0; i < 1024; i++)

        return lineSeparator;
    } // char getLineSeparator (std::ifstream& lutFile)


    LutErrorCode::LutState ReadLine (std::ifstream& lutFile, std::string& strBuffer, const char& lineSeparator)
    {
        while (0u == strBuffer.size() || symbCommentMarker == strBuffer[0])
        {
            if (lutFile.eof())
                return LutErrorCode::LutState::PrematureEndOfFile;

            std::getline(lutFile, strBuffer, lineSeparator);
            if (lutFile.fail())
                return LutErrorCode::LutState::ReadError;

            if (symbCommentMarker == strBuffer[0])
            {
                m_nativeComments += strBuffer;
                m_nativeComments += symbNewLine;
            }
        }
        return LutErrorCode::LutState::OK;
    } // LutErrorCode::LutState ReadLine(std::ifstream& lutFile, std::string& strBuffer, const char& lineSeparator)


    const std::array<T, 3> ParseTableRow (const std::string& strBuffer)
    {
        std::array<T, 3> lutRawData;
        std::istringstream data_line (strBuffer);

        for (int32_t i = 0; i < 3; i++)
        {
            data_line >> lutRawData[i];
            if (data_line.fail())
            {
                m_error = LutErrorCode::LutState::CouldNotParseTableData;
                break;
            }
        } // for (int32_t i = 0; i < 3; i++)

        return lutRawData;
    } // std::array<T, 3> ParseTableRow (const std::string& strBuffer)

};


#endif // __LUT_LIBRARY_LUT_3DL__
