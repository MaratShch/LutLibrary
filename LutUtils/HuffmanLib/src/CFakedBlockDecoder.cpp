#include "CFakedBlockDecoder.h"
#include "CDecoderConstants.h"
#include "CHuffmanIo.h"
#include <sstream>

using namespace HuffmanUtils;


CFakedBlockDecoder::~CFakedBlockDecoder (void)
{
    m_LEN = m_NLEN = 0u;
    return;
}


bool CFakedBlockDecoder::decode (const std::vector<uint8_t>& in, std::vector<uint8_t>& out, CStreamPointer& sp)
{
    // rfc1951: Any bits of input up to the next byte boundary are ignored.
    sp.align2byte();

    // Read LEN (16-bits little-endian)
    m_LEN  = readBits (in, sp, 16);
    // Read NLEN (16-bit little-endian)
    m_NLEN = readBits (in, sp, 16);
 
    // Check if m_NLEN is a one's complement of m_LEN
    if (m_NLEN != static_cast<uint16_t>(~m_LEN))
    {
        std::ostringstream ex;
        ex << "RAW: Invalid NLEN for stored block: Data integrity check failed. LEN = " << m_LEN << " NLEN = " << m_NLEN << ". SP = " << sp << " total size = " << in.size() << " bytes" ;
        const std::string ex_as_string(ex.str());
        throw std::runtime_error(ex_as_string);
    }

    const size_t actualLen = static_cast<size_t>(m_LEN) + 1ull;
    // reserve memory for output vector     
    out.reserve (out.size() + actualLen);
    
    for (size_t i = 0ull; i < actualLen; i++)
        out.push_back (readBits (in, sp, 8)); 

    return true;
}

