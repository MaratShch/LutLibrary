#include "CDynBlockDecoder.h"
#include "CHuffmanStream.h"

using namespace HuffmanUtils;


void CDynBlockDecoder::pre_decode (const std::vector<uint8_t>& in, CStreamPointer& sp)
{
    // read Literal Lengths Codes (stream pointer incremented internally) 
    m_HLIT = get_HLIT(in, sp);
    // read Distance Codes (stream pointer incremented internally)
    m_HDIST = get_HDIST(in, sp);
    // read Codel Lengths forCode Lengths (stream pointer incremented internally)
    m_HCLEN = get_HCLEN(in, sp);

    std::vector<uint32_t> cl4cl(m_HCLEN); // prepare storage for read from Huffman stream Code Lengths for Code Lengths alphabet
    for (uint32_t i = 0u; i < m_HCLEN; i++)
        cl4cl[i] = readBits(in, sp, 3u); // read Code Lengths for Code Lengths 3 bits values

    // generate Huffman Code Lengths for Code Legths codes
    std::vector<std::pair</* code */ uint32_t, /* length */ uint32_t>> cl4clCodes = generate_huffman_codes(cl4cl, m_HCLEN);
}


CStreamPointer CDynBlockDecoder::decode (const std::vector<uint8_t>& in, std::vector<uint8_t>& out, CStreamPointer& sp)
{
    pre_decode(in, sp);

  return sp;
}

CStreamPointer CDynBlockDecoder::decode (const uint8_t* in, uint8_t* out, size_t outSstorageSize, CStreamPointer& inSp)
{
    return{ 0 };
}

uint32_t CDynBlockDecoder::get_HLIT (const std::vector<uint8_t>& in, CStreamPointer& sp)
{
    const uint32_t HLIT = readBits (in, sp, 5u);
    return 257u + HLIT;
}

uint32_t CDynBlockDecoder::get_HCLEN (const std::vector<uint8_t>& in, CStreamPointer& sp)
{
    const uint32_t HCLEN = readBits (in, sp, 4u);
    return 4u + HCLEN;
}

uint32_t CDynBlockDecoder::get_HDIST (const std::vector<uint8_t>& in, CStreamPointer& sp)
{
    const uint32_t HDIST = readBits (in, sp, 5u);
    return 1u + HDIST;
}

