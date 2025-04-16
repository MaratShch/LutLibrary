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
            bool StreamIntegrityStatus(void) const noexcept { return m_Integrity; }
            const CStreamPointer get_sp (void) const noexcept {return m_Sp;}

        private:

            bool read_stream_properties (CStreamPointer sp = 0ll);
          
            uint8_t m_CMF = 0u;
            uint8_t m_FLG = 0u;
            uint8_t m_FCHECK = 0u;
            uint8_t m_FDICT = 0u;
            uint8_t m_FLEVEL = 0u;
            uint32_t m_WindowSize = 0u;

            uint32_t m_blockCnt = 0u;
            bool m_Integrity = false;

            CStreamPointer m_Sp = 0ll;
            InStreamT m_StreamData;
            

    }; // class CHuffmanStream


} // namespace HuffmanUtils

#endif // __HUFFMAN_STREAM_API_DEFINITION__
