#include "CHuffmanBlock.h"
#include "CHuffmanStream.h"
#include "CStatBlockDecoder.h"
#include "CDynBlockDecoder.h"


using namespace HuffmanUtils;


IBlockDecoder* CHuffmanBlock::create_decoder (const uint32_t& BTYPE)
{
    IBlockDecoder* iDecoder = nullptr;

    switch (BTYPE & 0x03u)
    {
        case 0x01u: // static Huffman Decoder
            iDecoder = static_cast<IBlockDecoder*>(new CStatBlockDecoder);
        break;

        case 0x02u: // dynamic Huffman Decoder
            iDecoder = static_cast<IBlockDecoder*>(new CDynBlockDecoder);
        break;

        default:    // broken stream - nothing to do
            iDecoder = nullptr;
        break;
    }

    return iDecoder;
}


void CHuffmanBlock::parse_block_header (CStreamPointer& sp) noexcept
{
    m_Sp = sp;

    m_CMF = m_BlockData[m_Sp.byte()];
    m_Sp.to_next_byte(); // forward stream pointer on next byte
    m_FLG = m_BlockData[m_Sp.byte()];
    m_Sp.to_next_byte(); // forward stream pointer on next byte

    uint32_t checkBit = static_cast<uint32_t>(m_CMF) * 256u + static_cast<uint32_t>(m_FLG);
    uint32_t checkRes = checkBit % 31u;
    const uint8_t CM = 0x08U & m_CMF;
    const uint8_t CINFO = (m_CMF >> 4) & 0x0Fu;

    if (0u == checkRes /* check bit correct */ && 0x08u == CM /* DEFLATE compression detected */)
    {
        m_WindowSize = static_cast<uint32_t>(std::pow(2.0, (static_cast<double>(CINFO) + 8.0)));

        m_FCHECK = m_FLG & 0x1Fu;	        // Check bits for CMF and FLG
        m_FDICT  = (m_FLG >> 5) & 0x01u;    // Preset Dictionary
        m_FLEVEL = (m_FLG >> 6) & 0x03u;    // Compression Level

        const uint32_t huffmanBlockHeader = readBits(m_BlockData, m_Sp, 3u);
        m_isFinal = static_cast<bool>(0x01u & huffmanBlockHeader);
        m_BTYPE   = static_cast<uint8_t>(0x3u & (huffmanBlockHeader >> 1));

        IBlockDecoder* iBlockDecoder = create_decoder(m_BTYPE);
        if (nullptr != iBlockDecoder)
        {
            m_iBlockDecoder = iBlockDecoder;
            m_isValid = (0u == m_FDICT ? true : false); // custom DICTIONARY is not supported yet
        }
    }

    return;
}


CHuffmanBlock::CHuffmanBlock (const uint8_t* pData, size_t inDataSize, CStreamPointer sp)
{
    if (nullptr != pData && 0ll < inDataSize)
        m_BlockData.assign(pData, pData + inDataSize);
    parse_block_header(sp);
    return;
}


CHuffmanBlock::CHuffmanBlock (const std::vector<uint8_t>&& pData, CStreamPointer sp) : m_BlockData(std::move(pData))
{
    parse_block_header(sp);
    return;
};
            
CHuffmanBlock::CHuffmanBlock (const std::vector<uint8_t>& pData, CStreamPointer sp) : m_BlockData(pData)
{
    parse_block_header(sp);
    return;
}

template <size_t N>
CHuffmanBlock::CHuffmanBlock(const std::array<uint8_t, N>& pData, CStreamPointer sp) : m_BlockData(pData.begin(), pData.end())
{
    parse_block_header(sp);
    return;
}


CHuffmanBlock::~CHuffmanBlock()
{
    if (nullptr != m_iBlockDecoder)
    {
        if (0x01u == m_BTYPE)
            delete static_cast<CStatBlockDecoder*>(m_iBlockDecoder);
        else if (0x02u == m_BTYPE)
            delete static_cast<CDynBlockDecoder*>(m_iBlockDecoder);

        m_iBlockDecoder = nullptr;
    }

    m_isFinal = m_isValid = false;
    m_Sp.reset();
    m_BlockData.clear();

    m_CMF = m_FLG = m_FCHECK = m_FDICT = m_FLEVEL = m_BTYPE = 0u;
    m_WindowSize = 0u;
    return;
}


std::vector<uint8_t> CHuffmanBlock::DecodeBlock(void)
{
    bool integrity = false;
    std::vector<uint8_t> decodedData;
    if (true == m_isValid)
        integrity = m_iBlockDecoder->decode (m_BlockData, decodedData, m_Sp);
    return decodedData;
}
