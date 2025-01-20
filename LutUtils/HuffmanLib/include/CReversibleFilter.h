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

    inline uint16_t ref_none_filter (const uint8_t& filtMsb, const uint8_t& filtLsb)
    {
        return static_cast<uint16_t>(filtMsb) << 8 | static_cast<uint16_t>(filtLsb);
    }
    
    inline uint16_t rev_sub_filter (const uint8_t& filtMsb, const uint8_t& filtLsb, const uint16_t& reconLeft)
    {
	// Lambda expression for SUB filter
    	auto sub_filter = [&](int16_t filtered, int16_t left) -> uint8_t
    	{
	        return static_cast<uint8_t>((filtered + left) & 0x00FF);
    	};

        const uint8_t msb = sub_filter (filtMsb, 0x00FF & (reconLeft >> 8));
        const uint8_t lsb = sub_filter (filtLsb, 0x00FF & reconLeft);

        return (static_cast<uint16_t>(msb) << 8 | static_cast<uint16_t>(lsb));
    } 

    inline uint16_t rev_paeth_filter (const uint8_t& filtMsb, const uint8_t& filtLsb, const uint16_t& reconLeft, const uint16_t& reconUp, const uint16_t& reconUpLeft)
    {
        // Lambda expression for PAETH filter 
        auto paeth_filter = [&](int16_t filtered, int16_t left, int16_t above, int16_t upper_left) -> uint8_t
        {
            const int16_t p  = left + above - upper_left;
            const int16_t pa = std::abs(p - left);
            const int16_t pb = std::abs(p - above);
            const int16_t pc = std::abs(p - upper_left);

            const int16_t predictor = (pa <= pb && pa <= pc) ? left : ((pb <= pc) ? above : upper_left);
            return static_cast<uint8_t>((filtered + predictor) & 0xFFu);
        };

        const uint8_t msb = paeth_filter (filtMsb, 0x00FF & (reconLeft >> 8), 0x00FF & (reconUp >> 8), 0x00FF & (reconUpLeft >> 8));
        const uint8_t lsb = paeth_filter (filtLsb, 0x00FF & reconLeft, 0x00FF & reconUp, 0x00FF & reconUpLeft);

        return (static_cast<uint16_t>(msb) << 8 | static_cast<uint16_t>(lsb));
    }


    inline bool filter_line_reconstruct
    (
        const std::vector<uint8_t>&  decoded,
              std::vector<uint16_t>& restored,
        const int32_t& inLineBytesSize, // input line size 
        const int32_t& outLineSize,     // output line size
        const int32_t& lineIdx,         // line number in zero enumeration mode
        const int32_t& channels         // number of color channel (1-monochrome, 3-RGB, 4-RGBA)
    ) 
    {
        constexpr int32_t in_out_factor = sizeof(uint16_t) / sizeof(uint8_t);
        const int32_t inChannelIncrement = channels * in_out_factor;
        const int32_t lineIn  = inLineBytesSize  * lineIdx;
        const int32_t lineOut = outLineSize * lineIdx;
        const int32_t lineUp  = outLineSize * (lineIdx - 1); 

        int32_t i /* input pixel idx */, o /* output pixel idx */;
        int32_t idxFilt;       // current input pixel index
        int32_t idxRecon;      // current output pixel index
        int32_t idxReconLeft;  // left in current line input pixel index
        int32_t idxReconUp;    // up line input pixel index
        int32_t idxReconUpleft;// up line left input pixel index

        uint16_t lR,  lG,  lB; // left pixel color component value
        uint16_t uR,  uG,  uB; // up pixels color components value
        uint16_t ulR, ulG, ulB;// up and left pixel color components value
  
    	// Prepare output buffer
    	restored.resize(restored.size() + outLineSize);
        
        switch (detect_filter(decoded[lineIn]))
        {
            case eFILTER_T::FILTER_NONE:
            {
                for (i = lineIn + 1, o = 0; o < outLineSize; o += channels, i += inChannelIncrement)
                {
                    idxFilt  = lineIn  + i;
                    idxRecon = lineOut + o;  
		            restored[idxRecon + 0] = ref_none_filter(decoded[idxFilt + 0], decoded[idxFilt + 1]);
		            restored[idxRecon + 1] = ref_none_filter(decoded[idxFilt + 2], decoded[idxFilt + 3]);
		            restored[idxRecon + 2] = ref_none_filter(decoded[idxFilt + 4], decoded[idxFilt + 5]);
                }
            }
            break;

            case eFILTER_T::FILTER_SUB:
            {
                for (i = 1, o = 0; o < outLineSize; o += channels, i += inChannelIncrement)
                {
                    idxFilt  = lineIn  + i;
                    idxRecon = lineOut + o;  

                    if (o >= channels)
                    {
                        lR = restored[idxRecon - channels + 0];
                        lG = restored[idxRecon - channels + 1];
                        lB = restored[idxRecon - channels + 2];
                    }
                    else
                        lR = lG = lB = static_cast<uint16_t>(0u);

		            restored[idxRecon + 0] = rev_sub_filter(decoded[idxFilt + 0], decoded[idxFilt + 1], lR);
		            restored[idxRecon + 1] = rev_sub_filter(decoded[idxFilt + 2], decoded[idxFilt + 3], lG);
		            restored[idxRecon + 2] = rev_sub_filter(decoded[idxFilt + 4], decoded[idxFilt + 5], lB);
                 }
            }
            break;

            case eFILTER_T::FILTER_UP:
            {
                for (i = 1, o = 0; o < outLineSize; o += channels, i += inChannelIncrement)
                {

                }
            }
            break;

            case eFILTER_T::FILTER_AVERAGE:
            {
                for (i = 1, o = 0; o < outLineSize; o += channels, i += inChannelIncrement)
                {
                }
            }
            break;

            case eFILTER_T::FILTER_PAETH:
            {
                for (i = 1, o = 0; o < outLineSize; o += channels, i += inChannelIncrement)
                {
                    idxFilt        = lineIn   + i;
                    idxRecon       = lineOut  + o;
                    idxReconLeft   = idxRecon - channels;
                    idxReconUp     = lineUp   + o; 
                    idxReconUpleft = idxReconUp - channels;
  
                    if (o >= channels)
                    { // we have left pixels
                        lR = restored[idxRecon - channels + 0];
                        lG = restored[idxRecon - channels + 1];
                        lB = restored[idxRecon - channels + 2];
                        if (lineUp >= 0)
                        { // we have up line with left pixels too
                           ulR = restored[idxReconUpleft + 0];
                           ulG = restored[idxReconUpleft + 1];
                           ulB = restored[idxReconUpleft + 2];
                        }
                        else
                           ulR = ulG = ulB = static_cast<uint16_t>(0u); 
                    }
                    else
                        ulR = ulG = ulB = lR = lG = lB = static_cast<uint16_t>(0u);

 		            if (lineUp >= 0)
                    { // we have up line
                       uR = restored[idxReconUp + 0]; 
                       uG = restored[idxReconUp + 1]; 
                       uB = restored[idxReconUp + 2]; 
                    }
                    else
                       uR = uG = uB = static_cast<uint16_t>(0u);
 
		            restored[idxRecon + 0] = rev_paeth_filter(decoded[idxFilt + 0], decoded[idxFilt + 1], lR, uR, ulR);
		            restored[idxRecon + 1] = rev_paeth_filter(decoded[idxFilt + 2], decoded[idxFilt + 3], lG, uG, ulG);
		            restored[idxRecon + 2] = rev_paeth_filter(decoded[idxFilt + 4], decoded[idxFilt + 5], lB, uB, ulB);
                } // for (i = 1, o = 0; o < outLineSize; o += channels, i += inChannelIncrement)
            }
            break;

            default:
                std::runtime_error("Invalid filter type");
            break;

        } // switch (filterT)

        return true;
    }
	

    std::vector<uint16_t> filter_data_reconstruct
    (
        const std::vector<uint8_t>& decoded, // decoded data
        const int32_t sizeX,                 // horizontal image size in pixels
        const int32_t sizeY,                 // vertical image size in pixels
        const int32_t bpp,                   // bits per pixels (8, 16, 24, 32, 48, 64)
        const int32_t channels               // color channel (1-B/W, 3-RGB, 4-RGBA)    
    )
    {
        std::vector<uint16_t> out_data{};
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
