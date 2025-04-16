#ifndef __HUFFMAN_BLOCK_DEFLATE_ALGO__
#define __HUFFMAN_BLOCK_DEFLATE_ALGO__

#include <cstdint>
#include <array>
#include <cmath>
#include "CHuffmanStreamPointer.h"
#include "IBlockDecoder.h"


namespace HuffmanUtils
{
    class CHuffmanBlock
    {
        public:
            virtual ~CHuffmanBlock();

            CHuffmanBlock (std::vector<uint8_t>& iData /* input */, std::vector<uint8_t>& oData /* output */, CStreamPointer& sp);
            CStreamPointer Decode (void);
            CStreamPointer GetStreamPointer(void) noexcept { return m_Sp; }
            
            bool isFinal (void) noexcept {return m_isFinal;}
 
            uint32_t GetDecoderType (void)
            {
                uint32_t bType = 0xFFu;
                if (nullptr != m_iBlockDecoder)
                    bType = m_iBlockDecoder->get_decoder_type();
                return bType;
            }

        private:
            IBlockDecoder* m_iBlockDecoder = nullptr;
            CStreamPointer m_Sp = 0ll;

            std::vector<uint8_t>& m_InData;
            std::vector<uint8_t>& m_OutData;

            bool m_isValid = false;
            bool m_isFinal = false;
 
            uint8_t m_BTYPE = 0u;
            void print_block_properties (void) noexcept;   
            bool parse_block_header (CStreamPointer& sp) noexcept;
            IBlockDecoder* create_decoder (const uint32_t& BTYPE);

    }; // class CHuffmanBlock

} // namespace HuffmanUtils

#endif // __HUFFMAN_BLOCK_DEFLATE_ALGO__
