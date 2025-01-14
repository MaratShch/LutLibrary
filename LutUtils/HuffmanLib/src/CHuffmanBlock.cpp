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
    std::cout << "CMF    = " << static_cast<uint32_t>(m_CMF)    << std::endl;
    std::cout << "FLG    = " << static_cast<uint32_t>(m_FLG)    << std::endl;
    std::cout << "FCHECK = " << static_cast<uint32_t>(m_FCHECK) << std::endl;
    std::cout << "FDICT  = " << static_cast<uint32_t>(m_FDICT)  << std::endl;
    std::cout << "FLEVEL = " << static_cast<uint32_t>(m_FLEVEL) << std::endl;
    std::cout << "BTYPE  = " << static_cast<uint32_t>(m_BTYPE) << std::endl;
    std::cout << "WSize  = " << static_cast<uint32_t>(m_WindowSize) << std::endl;
    std::cout << std::endl;
    return;
}

bool CHuffmanBlock::parse_block_header (CStreamPointer& sp) noexcept
{
    m_Sp = sp;
    // read compression method/flags code [8 bits]
    m_CMF = readBits (m_Data, m_Sp, 8);
    // read additional flag/check bits [8 bits]
    m_FLG = readBits (m_Data, m_Sp, 8);

    uint32_t checkBit = static_cast<uint32_t>(m_CMF) * 256u + static_cast<uint32_t>(m_FLG);
    uint32_t checkRes = checkBit % 31u;
    const uint8_t CM = 0x08U & m_CMF;
    const uint8_t CINFO = (m_CMF >> 4) & 0x0Fu;

    if (0u == checkRes /* check bit correct */ && 0x08u == CM /* DEFLATE compression detected */)
    {
        m_WindowSize = 1u << (CINFO + 8);

        m_FCHECK = m_FLG & 0x1Fu;	        // Check bits for CMF and FLG
        m_FDICT = (m_FLG >> 5) & 0x01u;    // Preset Dictionary
        m_FLEVEL = (m_FLG >> 6) & 0x03u;    // Compression Level

        const uint32_t huffmanBlockHeader = readBits(m_Data, m_Sp, 3u);
        m_isFinal = static_cast<bool>(0x01u & huffmanBlockHeader);
        m_BTYPE = static_cast<uint8_t>(0x3u & (huffmanBlockHeader >> 1));

        IBlockDecoder* iBlockDecoder = create_decoder(m_BTYPE);
        if (max_WindowSize == m_WindowSize && nullptr != iBlockDecoder)
        {
            m_iBlockDecoder = iBlockDecoder;
            m_isValid = (0u == m_FDICT ? true : false); // custom DICTIONARY is not supported yet
        }
    }
    else
        m_isValid = false;


    return m_isValid;
}


CHuffmanBlock::CHuffmanBlock (std::vector<uint8_t>& pData, CStreamPointer& sp) : m_Data(pData)
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

    m_CMF = m_FLG = m_FCHECK = m_FDICT = m_FLEVEL = m_BTYPE = 0u;
    m_WindowSize = 0u;
    return;
}


std::vector<uint8_t> CHuffmanBlock::Decode (void)
{
    std::vector<uint8_t> decodedData{};
    if (true == m_isValid)
    {
        m_iBlockDecoder->decode(m_Data, decodedData, m_Sp);
    }
    return decodedData;
}

