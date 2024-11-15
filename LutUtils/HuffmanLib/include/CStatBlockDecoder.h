#ifndef __C_HUFFMAN_STATIC_BLOCK_DECODER_IMPLEMENTATION__
#define __C_HUFFMAN_STATIC_BLOCK_DECODER_IMPLEMENTATION__


#include "IBlockDecoder.h"

namespace HuffmanUtils
{
    class CStatBlockDecoder : public IBlockDecoder
    {
       public:
           CStreamPointer decode (const uint8_t* in, uint8_t* out, size_t outSstorageSize, CStreamPointer& inSp);
           CStreamPointer decode (const std::vector<uint8_t>& in, std::vector<uint8_t>& out, CStreamPointer& inSp);
           uint8_t get_decoder_type(void) { return 0x01u; }

       private:
  
    }; // class CStatBlockDecoder : public IBlockDecoder
	
}; // namespace HuffmanUtils

#endif // __C_HUFFMAN_STATIC_BLOCK_DECODER_IMPLEMENTATION__
