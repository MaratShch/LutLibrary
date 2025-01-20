#ifndef __C_PNG_REVERSIBLE_FILTER_API__
#define __C_PNG_REVERSIBLE_FILTER_API__

#include <vector>
#include <iostream>
#include <cmath>
#include <limits>

namespace HuffmanUtils
{
    enum class eFILTER_T
    {
        FILTER_NONE = 0,
        FILTER_SUB,
        FILTER_UP,
        FILTER_AVERAGE,
        FILTER_PAETH,
        FILTER_INVALID
    };

    inline const eFILTER_T detect_filter (const uint8_t& fIdx)
    {
        eFILTER_T filterT = eFILTER_T::FILTER_INVALID;
          // Use switch or if-else to check possible values and convert data correctly
        switch (fIdx)
        {
            case 0:
                filterT = eFILTER_T::FILTER_NONE;
            break;
            case 1:
                filterT = eFILTER_T::FILTER_SUB;
            break;
            case 2:
                filterT = eFILTER_T::FILTER_UP;
            break;
            case 3:
                filterT = eFILTER_T::FILTER_AVERAGE;
            break;
            case 4:
                filterT = eFILTER_T::FILTER_PAETH;
            break;
            default:
                // Handle error case: invalid filter value
                std::cerr << "Error: Invalid filter value: " << static_cast<int>(fIdx) << std::endl;
            break;    
        }
        return (filterT);
    }

    
    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
    bool filter_line_reconstruct
    (
        const std::vector<uint8_t>& decoded,
              std::vector<T>& restored,
        const int32_t& inLineBytesSize,
        const int32_t& outLineSize,
        const int32_t& lineIdx,
        const int32_t& channels
    ) 
    {
        constexpr int32_t in_out_factor = sizeof(T) / sizeof(uint8_t);
        const int32_t inChannelIncrement = channels * in_out_factor;
        const int32_t lineIn  = inLineBytesSize  * lineIdx;
        const int32_t lineOut = outLineSize * lineIdx;

        int32_t i, j, o;

        switch (detect_filter(decoded[lineIn]))
        {
            case eFILTER_T::FILTER_NONE:
            {
                for (i = lineIn + 1, o = lineOut; o < outLineSize; o += channels, i += inChannelIncrement)
                {

                }
            }
            break;

            case eFILTER_T::FILTER_SUB:
            {
                for (i = lineIn + 1, o = lineOut; o < outLineSize; o += channels, i += inChannelIncrement)
                {

                }
            }
            break;

            case eFILTER_T::FILTER_UP:
            {
                for (i = lineIn + 1, o = lineOut; o < outLineSize; o += channels, i += inChannelIncrement)
                {

                }
            }
            break;

            case eFILTER_T::FILTER_AVERAGE:
            {
                for (i = lineIn + 1, o = lineOut; o < outLineSize; o += channels, i += inChannelIncrement)
                {

                }
            }
            break;

            case eFILTER_T::FILTER_PAETH:
            {
                for (i = lineIn + 1, o = lineOut; o < outLineSize; o += channels, i += inChannelIncrement)
                {

                }
            }
            break;

            default:
                std::runtime_error("Invalid filter type");
            break;

        } // switch (filterT)

        return true;
    }
	

    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
    const std::vector<T> filter_data_reconstruct
    (
        const std::vector<uint8_t>& decoded, // decoded data
        const int32_t sizeX,                 // horizontal image size in pixels
        const int32_t sizeY,                 // vertical image size in pixels
        const int32_t bpp,                   // bits per pixels (8, 16, 24, 32, 48, 64)
        const int32_t channels               // color channel (1-B/W, 3-RGB, 4-RGBA)    
    )
    {
        std::vector<T> out_data{};
        const int32_t colorChannelDept = bpp / channels;
        const int32_t inSizeBytes = 1 + sizeX * channels * (colorChannelDept / 8);
        const int32_t outSize = sizeX * channels;
        bool bResult = true;

        for (int32_t i = 0; i < sizeY && true == bResult; i++)
            bResult = filter_line_reconstruct(decoded, out_data, inSizeBytes, outSize, i, channels);

        return out_data;
    }
}; // namespace HuffmanUtils

#endif // __C_PNG_REVERSIBLE_FILTER_API__
