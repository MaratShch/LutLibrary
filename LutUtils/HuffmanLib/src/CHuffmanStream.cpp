#include "CHuffmanStream.h"
#include "CHuffmanBlock.h"
#include "CHuffmanIo.h"
#include "CHuffmanStreamPointer.h"

using namespace HuffmanUtils;

CHuffmanStream::CHuffmanStream (const uint8_t* pData, size_t inDataSize, CStreamPointer sp)
{
    if (nullptr != pData && 0ll < inDataSize)
        m_StreamData.assign(pData, pData + inDataSize);

    m_blockCnt = 0u;
    // initial stream pointer
    m_Sp = sp;
    return;
}


CHuffmanStream::CHuffmanStream (const std::vector<uint8_t>&& pData, CStreamPointer sp) : m_StreamData(std::move(pData))
{
    m_blockCnt = 0u;
    // initial stream pointer
    m_Sp = sp;
    return;
};
            
CHuffmanStream::CHuffmanStream (const std::vector<uint8_t>& pData, CStreamPointer sp) : m_StreamData(pData)
{
    m_blockCnt = 0u;
    // initial stream pointer
    m_Sp = sp;
    return;
}

template <size_t N>
CHuffmanStream::CHuffmanStream(const std::array<uint8_t, N>& pData, CStreamPointer sp) : m_StreamData(pData.begin(), pData.end())
{
    m_blockCnt = 0u;
    // initial stream pointer
    m_Sp = sp;
    return;
}


CHuffmanStream::~CHuffmanStream (void)
{
    m_StreamData.clear();
    return;
}


OutStreamT Encode (void)
{
   OutStreamT encodedData{};
   return encodedData; // not implemented yet
}


OutStreamT CHuffmanStream::Decode (void)
{
    OutStreamT decodedData{};
    CStreamPointer lastBlockValidSp = 0ll;

    bool finalBlock = false;
   
    do {
        m_blockCnt++;
        CHuffmanBlock hBlock (m_StreamData, m_Sp);
        finalBlock = hBlock.isFinal();

        OutStreamT dataChunk = hBlock.Decode();
        m_Sp = hBlock.GetStreamPointer();

        if (0 != dataChunk.size())
        {
            lastBlockValidSp = m_Sp;

            // resize target vector and copy decoded data chunk into targe vector
            decodedData.resize(decodedData.size() + dataChunk.size());
            std::copy(dataChunk.begin(), dataChunk.end(), decodedData.end() - dataChunk.size());
        }
        else
        {
            finalBlock = true;
        }

    } while (false == finalBlock);

    auto convertEndian = [&](const uint32_t value) -> uint32_t
    {
        return ((value >> 24) & 0x000000FFu) | // Move byte 3 to byte 0
               ((value >> 8)  & 0x0000FF00u) | // Move byte 2 to byte 1
               ((value << 8)  & 0x00FF0000u) | // Move byte 1 to byte 2
               ((value << 24) & 0xFF000000u);  // Move byte 0 to byte 3
    };

    CStreamPointer adlerSp{ byte2sp(static_cast<uint32_t>(m_StreamData.size() - 4ull)) };
    uint32_t adler_provided = convertEndian(readBits(m_StreamData, adlerSp, 32));
    // check stream integrity
    uint32_t adler_expected = computeAdler32(decodedData);

    m_Integrity = (adler_provided == adler_expected);

    return decodedData;
}

