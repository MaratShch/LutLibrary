#ifndef __C_HUFFMAN_FAKED_BLOCK_DECODER_IMPLEMENTATION__
#define __C_HUFFMAN_FAKED_BLOCK_DECODER_IMPLEMENTATION__

#include "IBlockDecoder.h"
#include "CHuffmanStreamPointer.h"

namespace HuffmanUtils
{

    // This is not an actual decoder; instead, we'll create a 'faked' decoder to maintain a consistent interface 
    // across all Huffman block types.
    // This decoder will only be invoked for uncompressed data blocks (BLOCK=0) only."
    class CFakedBlockDecoder : public IBlockDecoder
    {
       public:
           virtual ~CFakedBlockDecoder(void);

           bool decode (const std::vector<uint8_t>& in, std::vector<uint8_t>& out, CStreamPointer& inSp);
           uint8_t get_decoder_type(void) { return 0x00u; }

       private:
           uint16_t m_LEN = static_cast<uint16_t>(0u);
           uint16_t m_NLEN= static_cast<uint16_t>(0u); 
    }; // class CFakedBlockDecoder : public IBlockDecoder
	
} // namespace HuffmanUtils



#endif // __C_HUFFMAN_DYNAMIC_BLOCK_DECODER_IMPLEMENTATION__
