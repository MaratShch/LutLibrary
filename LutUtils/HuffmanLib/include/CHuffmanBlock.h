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

            CHuffmanBlock (const uint8_t* pData, size_t inDataSize, CStreamPointer sp = 0ll);
            CHuffmanBlock (const std::vector<uint8_t>& pData,  CStreamPointer sp = 0ll);
            CHuffmanBlock (const std::vector<uint8_t>&& pData, CStreamPointer sp = 0ll);

            template <size_t N>
            CHuffmanBlock(const std::array<uint8_t, N>& pData, CStreamPointer sp = 0ll);

            std::vector<uint8_t> DecodeBlock (void);
            CStreamPointer GetStreamPointer(void) noexcept { return m_Sp; }
            bool isBlockValid(void) noexcept { return m_isValid; }
            bool blockIntegrityStatus(void) noexcept { return m_Integrity; }

        private:
            IBlockDecoder* m_iBlockDecoder = nullptr;
            CStreamPointer m_Sp = 0ll;
            std::vector<uint8_t> m_BlockData;

            bool m_isValid = false;
            bool m_isFinal = false;
            bool m_Integrity = false;

            uint8_t m_CMF = 0u;
            uint8_t m_FLG = 0u;
            uint8_t m_FCHECK = 0u;
            uint8_t m_FDICT = 0u;
            uint8_t m_FLEVEL = 0u;
            uint8_t m_BTYPE = 0u;
            uint32_t m_WindowSize = 0u;

            void parse_block_header (CStreamPointer& sp) noexcept;
            IBlockDecoder* create_decoder (const uint32_t& BTYPE);

    }; // class CHuffmanBlock

}; // namespace HuffmanUtils

#endif // __HUFFMAN_BLOCK_DEFLATE_ALGO__
