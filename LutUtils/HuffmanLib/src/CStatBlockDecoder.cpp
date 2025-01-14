#include "CStatBlockDecoder.h"
#include "CDecoderConstants.h"
#include "CHuffmanIo.h"

using namespace HuffmanUtils;

CStatBlockDecoder::~CStatBlockDecoder(void)
{
   // nothing TODO
   return;
}

void CStatBlockDecoder::createCodesTable (void)
{
    uint32_t i = 0u;

    // Literal/Length Codes (0...287)
    for (i = 0u;   i <= 143u; ++i) m_Table[i] = 0b00110000 + i;            // Literal/Length codes 0-143:   8-bit codes.
    for (i = 144u; i <= 255u; ++i) m_Table[i] = 0b110010000 + (i - 144u);  // Literal/Length codes 144-255: 9-bit codes.
    for (i = 256u; i <= 279u; ++i) m_Table[i] = i - 256u;                  // Literal/Length codes 256-279: 7-bit codes.
    for (i = 280u; i <= 287u; ++i) m_Table[i] = 0b11000000 + (i - 280u);   // Literal/Length codes 280-287: 8-bit codes.

    return;
}

void CStatBlockDecoder::createReverseTable (void)
{
    for (const auto& pair : m_Table)
        m_reverseTable[pair.second] = pair.first;

    return;
}


void CStatBlockDecoder::pre_decode (void)
{
    createCodesTable();
    createReverseTable();

    return;
}


bool CStatBlockDecoder::decode (const std::vector<uint8_t>& in, std::vector<uint8_t>& out, CStreamPointer& sp)
{
    m_decoderIntegrityStatus = false;

    // cleanup output vector
    out.clear();

    // Initialize and Build all Huffman Dynamic Decoder Infrastructures (Cl4Cl, Huffman trees, etc...)
    pre_decode ();

    uint32_t symbol = 0u;
    uint32_t readSize = 8u; // initial read of 8 bits, because steam can't be started from distance code

    // decoding loop
    // Currently Length codes in range 257 - 287 isn't processed.
    // Need to understand how resolve ambiguity on read from stream between these codes and Literal 8/9 bits codes
    do {
        uint32_t hCode = readStaticHuffmanBits (in, sp, readSize);

        auto it1 = m_reverseTable.find (hCode);
        if (it1 != m_reverseTable.end())
            symbol = it1->second; // Return the symbol associated with the code
        else
        {
            // complementar bit - make 9 bits code and search again
            hCode = readComplementarStaticHuffmanBits (in, sp, hCode);
            auto it2 = m_reverseTable.find(hCode);
            if (it2 != m_reverseTable.end())
                symbol = it2->second; // Return the symbol associated with the code
        }

        if (symbol != EndOfBlock)
            out.push_back(symbol);

    } while (symbol != EndOfBlock);

    auto convertEndian = [](uint32_t value) -> uint32_t
    {
        return ((value >> 24) & 0x000000FF) | // Move byte 3 to byte 0
            ((value >> 8) & 0x0000FF00) | // Move byte 2 to byte 1
            ((value << 8) & 0x00FF0000) | // Move byte 1 to byte 2
            ((value << 24) & 0xFF000000);  // Move byte 0 to byte 3
    };


    sp.align2byte(); // Skip any remaining padding bits until the byte boundary is reached
                     // integirty check: read ADLER-32 checksum
    const uint32_t adler32Expected = convertEndian(readBits(in, sp, 32u));
    // integirty check: compute ADLER-32 checksum
    const uint32_t adler32Computed = computeAdler32(out);

    // validate checksums
    m_decoderIntegrityStatus = (adler32Expected == adler32Computed);

    return m_decoderIntegrityStatus;
}

