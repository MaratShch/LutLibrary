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


uint32_t CStatBlockDecoder::read_fixed_huffman_code (const std::vector<uint8_t>& in, CStreamPointer& sp)
{
#if 0
    uint32_t code = 0;
    code = readBits(stream, sp, 7);
    
    auto symbol = table_lookup_7bit(code); // Implement it yourself based on RFC 1951
    if(symbol != undefined_symbol)
        return symbol;

    code = readBits(stream, sp, 1); //read 8 bit by adding one bit
    code = (code << 7) |  (code & 0x7F);

     symbol = table_lookup_8bit(code); // Implement it yourself based on RFC 1951
    if(symbol != undefined_symbol)
        return symbol;
    
    code = readBits(stream, sp, 1); // read 9 bit by adding another bit
    code = (code << 8) |  (code & 0xFF);

    symbol = table_lookup_9bit(code); // Implement it yourself based on RFC 1951
    if(symbol != undefined_symbol)
        return symbol;

   throw std::runtime_error("Unable to extract symbol from fixed Huffman code stream");
#endif
   return 0;
}



bool CStatBlockDecoder::decode (const std::vector<uint8_t>& in, std::vector<uint8_t>& out, CStreamPointer& sp)
{
    // Initialize and Build all Huffman Dynamic Decoder Infrastructures (Cl4Cl, Huffman trees, etc...)
    pre_decode ();

    uint32_t symbol = 0u;
    constexpr uint32_t readSize = 8u;

    // decoding loop
    do {
        uint32_t hCode = readStaticHuffmanBits (in, sp, readSize);

        auto it1 = m_reverseTable.find (hCode);
        if (it1 != m_reverseTable.end())
            symbol = it1->second; // Return the symbol associated with the code
        else
        {
            // complementar bit - make 8 bits code and search again
            hCode = readComplementarStaticHuffmanBits (in, sp, hCode);
            auto it2 = m_reverseTable.find(hCode);
            if (it2 != m_reverseTable.end())
                symbol = it2->second; // Return the symbol associated with the code
        }

        if (symbol != EndOfBlock)
            out.push_back(symbol);

    } while (symbol != EndOfBlock);

   return true;
}

