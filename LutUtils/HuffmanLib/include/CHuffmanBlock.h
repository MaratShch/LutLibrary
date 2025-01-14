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

            CHuffmanBlock (std::vector<uint8_t>& vData, CStreamPointer& sp);
            std::vector<uint8_t> Decode (void);
            CStreamPointer GetStreamPointer(void) noexcept { return m_Sp; }
            
            bool isFinal (void) noexcept {return m_isFinal;}
 
        private:
            IBlockDecoder* m_iBlockDecoder = nullptr;
            CStreamPointer m_Sp = 0ll;
            std::vector<uint8_t>& m_Data;

            bool m_isValid = false;
            bool m_isFinal = false;
 
            uint8_t m_CMF = 0u;
            uint8_t m_FLG = 0u;
            uint8_t m_FCHECK = 0u;
            uint8_t m_FDICT = 0u;
            uint8_t m_FLEVEL = 0u;
            uint8_t m_BTYPE = 0u;
            uint32_t m_WindowSize = 0u;

            void print_block_properties (void) noexcept;   
            bool parse_block_header (CStreamPointer& sp) noexcept;
            IBlockDecoder* create_decoder (const uint32_t& BTYPE);

    }; // class CHuffmanBlock

}; // namespace HuffmanUtils

#endif // __HUFFMAN_BLOCK_DEFLATE_ALGO__
