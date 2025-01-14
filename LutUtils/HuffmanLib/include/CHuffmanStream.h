#ifndef __HUFFMAN_STREAM_API_DEFINITION__
#define __HUFFMAN_STREAM_API_DEFINITION__

#include <vector>
#include <array>
#include <utility>
#include <memory>
#include "CHuffmanStreamPointer.h"
#include "CHuffmanTree.h"

namespace HuffmanUtils
{
    using InStreamT  = std::vector<uint8_t>;
    using OutStreamT = std::vector<uint8_t>;

    class CHuffmanStream
    {
        public:
            CHuffmanStream (const uint8_t* pData, size_t inDataSize, CStreamPointer sp = 0ll);
            CHuffmanStream (const std::vector<uint8_t>& pData,  CStreamPointer sp = 0ll);
            CHuffmanStream (const std::vector<uint8_t>&& pData, CStreamPointer sp = 0ll);

            template <size_t N>
            CHuffmanStream(const std::array<uint8_t, N>& pData, CStreamPointer sp = 0ll);

            virtual ~CHuffmanStream();

            OutStreamT Decode (void);
            OutStreamT Encode (void);
            bool StreamIntegrityStatus(void) noexcept { return m_Integrity; }

        private:

            uint32_t m_blockCnt = 0u;
            bool m_Integrity = false;

            CStreamPointer m_Sp = 0ll;
            InStreamT m_StreamData;
            

    }; // class CHuffmanStream


}; // namespace HuffmanUtils

#endif // __HUFFMAN_STREAM_API_DEFINITION__
