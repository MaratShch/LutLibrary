#ifndef __C_HUFFMAN_STATIC_BLOCK_DECODER_IMPLEMENTATION__
#define __C_HUFFMAN_STATIC_BLOCK_DECODER_IMPLEMENTATION__


#include "IBlockDecoder.h"
#include "CDecoderConstants.h"
#include <array>

namespace HuffmanUtils
{

    class CStatBlockDecoder : public IBlockDecoder
    {
       public:
           virtual ~CStatBlockDecoder();

           bool decode (const std::vector<uint8_t>& in, std::vector<uint8_t>& out, CStreamPointer& inSp);
           uint8_t get_decoder_type(void) { return 0x01u; }
           
       private:

           static constexpr uint32_t codesArraySize = 512u;
           std::array<uint32_t, codesArraySize> mCodesLookUp;

           bool m_decoderIntegrityStatus = false;

           bool pre_decode (void);
           void createCodesTable   (void);

           uint32_t read_fixed_huffman_code(const std::vector<uint8_t>& in, CStreamPointer& sp);

    }; // class CStatBlockDecoder : public IBlockDecoder
	
}; // namespace HuffmanUtils

#endif // __C_HUFFMAN_STATIC_BLOCK_DECODER_IMPLEMENTATION__
