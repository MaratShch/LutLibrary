#include "CHuffmanBlock.h"
#include "CHuffmanIo.h"
#include "CFakedBlockDecoder.h"
#include "CStatBlockDecoder.h"
#include "CDynBlockDecoder.h"
#include "CDecoderConstants.h"


using namespace HuffmanUtils;


IBlockDecoder* CHuffmanBlock::create_decoder (const uint32_t& BTYPE)
{
    IBlockDecoder* iDecoder = nullptr;

    switch (BTYPE & 0x03u)
    {
        case 0x00u: // not encoded (RAW) block
            iDecoder = static_cast<IBlockDecoder*>(new CFakedBlockDecoder);
        break;

        case 0x01u: // static Huffman Decoder
            iDecoder = static_cast<IBlockDecoder*>(new CStatBlockDecoder);
        break;

        case 0x02u: // dynamic Huffman Decoder
            iDecoder = static_cast<IBlockDecoder*>(new CDynBlockDecoder);
        break;

        default:    // broken stream or block header - nothing TODO
            iDecoder = nullptr;
            throw std::runtime_error("Invalid Huffman Block Type received (out of range 0 ... 2): " + std::to_string(BTYPE) + ".");

        break;
    }

    return iDecoder;
}

void CHuffmanBlock::print_block_properties(void) noexcept
{
    std::cout << "BLOCK PROPERTIES:" << std::endl;
    std::cout << "BTYPE  = " << static_cast<uint32_t>(m_BTYPE) << std::endl;
    std::cout << std::endl;
    return;
}

bool CHuffmanBlock::parse_block_header (CStreamPointer& sp) noexcept
{
    m_Sp = sp;
    const uint32_t huffmanBlockHeader = readBits(m_InData, m_Sp, 3u);
    m_isFinal = static_cast<bool>(0x01u & huffmanBlockHeader);
    m_BTYPE   = static_cast<uint8_t>(0x3u & (huffmanBlockHeader >> 1));

    IBlockDecoder* iBlockDecoder = create_decoder(m_BTYPE);
    if (nullptr != iBlockDecoder)
    {
        m_iBlockDecoder = iBlockDecoder;
        m_isValid = true;
    }
    else
        m_isValid = false;


    return m_isValid;
}


CHuffmanBlock::CHuffmanBlock (std::vector<uint8_t>& pData, std::vector<uint8_t>& dData, CStreamPointer& sp) : m_InData(pData), m_OutData(dData)
{
    parse_block_header(sp);
    return;
}


CHuffmanBlock::~CHuffmanBlock()
{
    if (nullptr != m_iBlockDecoder)
    {
        if (0x00u == m_BTYPE)
            delete static_cast<CFakedBlockDecoder*>(m_iBlockDecoder);
        else if (0x01u == m_BTYPE)
            delete static_cast<CStatBlockDecoder*>(m_iBlockDecoder);
        else if (0x02u == m_BTYPE)
            delete static_cast<CDynBlockDecoder*>(m_iBlockDecoder);

        m_iBlockDecoder = nullptr;
    }

    m_isFinal = m_isValid = false;
    m_Sp.reset();

    return;
}


CStreamPointer CHuffmanBlock::Decode (void)
{
    if (true == m_isValid)
    {
        m_iBlockDecoder->decode(m_InData, m_OutData, m_Sp);
    }
    return m_Sp;
}

