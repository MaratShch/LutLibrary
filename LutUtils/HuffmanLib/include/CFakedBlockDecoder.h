#ifndef __C_HUFFMAN_FAKED_BLOCK_DECODER_IMPLEMENTATION__
#define __C_HUFFMAN_FAKED_BLOCK_DECODER_IMPLEMENTATION__

#include "IBlockDecoder.h"
#include "CHuffmanStreamPointer.h"

namespace HuffmanUtils
{

    // not actually decoder - let's create "Faked" decoder for keep same interface for all Huffman Block types
    class CFakedBlockDecoder : public IBlockDecoder
    {
       public:
           virtual ~CFakedBlockDecoder(void);

           bool decode (const std::vector<uint8_t>& in, std::vector<uint8_t>& out, CStreamPointer& inSp);
           uint8_t get_decoder_type(void) { return 0x00u; }

    }; // class CFakedBlockDecoder : public IBlockDecoder
	
}; // namespace HuffmanUtils



#endif // __C_HUFFMAN_DYNAMIC_BLOCK_DECODER_IMPLEMENTATION__
