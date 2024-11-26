#ifndef __I_HUFFMAN_BLOCK_DECODER_INTERACE__
#define __I_HUFFMAN_BLOCK_DECODER_INTERACE__

#include <cstdint>
#include <utility>
#include <type_traits>
#include <vector>
#include "CHuffmanStreamPointer.h"


namespace HuffmanUtils
{
    class IBlockDecoder
    {
       public:
           virtual CStreamPointer decode (const std::vector<uint8_t>& in, std::vector<uint8_t>& out, CStreamPointer& inSp) = 0;
           virtual uint8_t get_decoder_type(void) = 0;

    }; // class IBlockDecoder
	
}; // namespace HuffmanUtils

#endif // __I_HUFFMAN_BLOCK_DECODER_INTERACE__
