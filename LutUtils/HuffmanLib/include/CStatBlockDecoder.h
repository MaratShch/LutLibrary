#ifndef __C_HUFFMAN_STATIC_BLOCK_DECODER_IMPLEMENTATION__
#define __C_HUFFMAN_STATIC_BLOCK_DECODER_IMPLEMENTATION__


#include "IBlockDecoder.h"
#include <unordered_map>

namespace HuffmanUtils
{
    class CStatBlockDecoder : public IBlockDecoder
    {
       public:
           bool decode (const std::vector<uint8_t>& in, std::vector<uint8_t>& out, CStreamPointer& inSp);
           bool decode_blocks (const std::vector<uint8_t>& in, std::vector<uint8_t>& out, CStreamPointer& sp);

           uint8_t get_decoder_type(void) { return 0x01u; }

       private:
           std::unordered_map<int32_t,  uint32_t> m_Table;
           std::unordered_map<uint32_t, int32_t>  m_reverseTable;
           
           bool m_decoderIntegrityStatus = false;

           void pre_decode (void);
           void createCodesTable   (void);
           void createReverseTable(void);

    }; // class CStatBlockDecoder : public IBlockDecoder
	
}; // namespace HuffmanUtils

#endif // __C_HUFFMAN_STATIC_BLOCK_DECODER_IMPLEMENTATION__
