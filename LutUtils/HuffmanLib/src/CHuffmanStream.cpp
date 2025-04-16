#include "CHuffmanStream.h"
#include "CHuffmanBlock.h"
#include "CHuffmanIo.h"
#include "CHuffmanStreamPointer.h"
#include "CDecoderConstants.h"

using namespace HuffmanUtils;

CHuffmanStream::CHuffmanStream (const uint8_t* pData, size_t inDataSize, CStreamPointer sp)
{
    if (nullptr != pData && 0ll < inDataSize)
        m_StreamData.assign(pData, pData + inDataSize);

    m_blockCnt = 0u;
    // read stream properties and initial stream pointer
    read_stream_properties (sp);
    return;
}


CHuffmanStream::CHuffmanStream (const std::vector<uint8_t>&& pData, CStreamPointer sp) : m_StreamData(std::move(pData))
{
    m_blockCnt = 0u;
    // read stream properties and initial stream pointer
    read_stream_properties (sp);
    return;
}
            
CHuffmanStream::CHuffmanStream (const std::vector<uint8_t>& pData, CStreamPointer sp) : m_StreamData(pData)
{
    m_blockCnt = 0u;
    // read stream properties and initial stream pointer
    read_stream_properties (sp);
    return;
}

template <size_t N>
CHuffmanStream::CHuffmanStream(const std::array<uint8_t, N>& pData, CStreamPointer sp) : m_StreamData(pData.begin(), pData.end())
{
    m_blockCnt = 0u;
    // read stream properties and initial stream pointer
    read_stream_properties (sp);
    return;
}


CHuffmanStream::~CHuffmanStream (void)
{
    m_StreamData.clear();
    return;
}

bool CHuffmanStream::read_stream_properties (CStreamPointer sp)
{
    bool streamValid = false;
    m_Sp = sp;
    // read compression method/flags code [8 bits]
    m_CMF = readBits (m_StreamData, m_Sp, 8);
    // read additional flag/check bits [8 bits]
    m_FLG = readBits (m_StreamData, m_Sp, 8);

    uint32_t checkBit = static_cast<uint32_t>(m_CMF) * 256u + static_cast<uint32_t>(m_FLG);
    uint32_t checkRes = checkBit % 31u;
    const uint8_t CM = 0x08U & m_CMF;
    const uint8_t CINFO = (m_CMF >> 4) & 0x0Fu;

    m_WindowSize = 1u << (CINFO + 8);

    if (
        0x00u == checkRes /* check bit correct */            && 
        0x08u == CM       /* DEFLATE compression detected */ &&
        max_WindowSize == m_WindowSize /* 32K sliding Windows detected */
       )
    {
        m_FCHECK = m_FLG & 0x1Fu;          // Check bits for CMF and FLG
        m_FDICT  = (m_FLG >> 5) & 0x01u;   // Preset Dictionary
        m_FLEVEL = (m_FLG >> 6) & 0x03u;   // Compression Level

        streamValid = (0u == m_FDICT ? true : false); // custom DICTIONARY is not supported yet
    } 
   
   return streamValid;
}


OutStreamT CHuffmanStream::Encode (void)
{
   OutStreamT encodedData{};
   return encodedData; // not implemented yet
}


OutStreamT CHuffmanStream::Decode (void)
{
    OutStreamT decodedData{};
    CStreamPointer lastBlockValidSp = 0ll;

    bool finalBlock = false;
    bool errDecode  = false;
    size_t decodedSizePrev = 0ull;
    size_t decodedSizeCurr = 0ull;

    do {
        m_blockCnt++;

        CHuffmanBlock hBlock (m_StreamData, decodedData, m_Sp);
        finalBlock = hBlock.isFinal();
        m_Sp = hBlock.Decode();

        decodedSizeCurr = decodedData.size();
        
        if (decodedSizeCurr > decodedSizePrev)
        {
            decodedSizePrev = decodedSizeCurr;
            lastBlockValidSp = m_Sp;
        }
        else
        {
            errDecode = (false == finalBlock ? true : false);
        }

    } while (false == finalBlock && false == errDecode);

    auto convertEndian = [&](const uint32_t value) -> uint32_t
    {
        return ((value >> 24) & 0x000000FFu) | // Move byte 3 to byte 0
               ((value >> 8)  & 0x0000FF00u) | // Move byte 2 to byte 1
               ((value << 8)  & 0x00FF0000u) | // Move byte 1 to byte 2
               ((value << 24) & 0xFF000000u);  // Move byte 0 to byte 3
    };

    CStreamPointer adlerSp{ byte2sp(static_cast<uint32_t>(m_StreamData.size() - 4ull)) };
#ifdef _DEBUG
    const CStreamPointer spFraction(adlerSp - m_Sp);
    const auto spbyte = spFraction.byte();
#endif
    uint32_t adler_provided = convertEndian(readBits(m_StreamData, adlerSp, 32));
    // check stream integrity
    uint32_t adler_expected = computeAdler32(decodedData);
    m_Integrity = (adler_provided == adler_expected);

    return decodedData;
}

