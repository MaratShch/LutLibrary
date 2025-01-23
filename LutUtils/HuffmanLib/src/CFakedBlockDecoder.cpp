#include "CFakedBlockDecoder.h"
#include "CDecoderConstants.h"
#include "CHuffmanIo.h"

using namespace HuffmanUtils;


CFakedBlockDecoder::~CFakedBlockDecoder (void)
{
    m_LEN = m_NLEN = 0;
    return;
}


bool CFakedBlockDecoder::decode (const std::vector<uint8_t>& in, std::vector<uint8_t>& out, CStreamPointer& sp)
{
    // Read LEN (16-bits little-endian)
    m_LEN  = readBits (in, sp, 16);
    // Read NLEN (16-bit little-endian)
    m_NLEN = readBits (in, sp, 16);
 
    const uint16_t lenComplement = ~m_LEN & static_cast<uint16_t>(0xFFFFu); 
    // Check if m_NLEN is a one's complement of m_LEN
    if (m_NLEN != lenComplement)
        throw std::runtime_error("Invalid NLEN for stored block: Data integrity check failed"); 

    const size_t actualLen = static_cast<size_t>(m_LEN) + 1ull;
    // reserve memory for output vector     
    out.reserve (out.size() + actualLen);
    
    for (size_t i = 0ull; i < actualLen; i++)
        out.push_back (readBits (in, sp, 8)); 

    return true;
}

