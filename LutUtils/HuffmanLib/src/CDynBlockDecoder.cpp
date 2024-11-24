#include "CDynBlockDecoder.h"
#include "CHuffmanStream.h"
#include "CHuffmanTree.h"

using namespace HuffmanUtils;


CDynBlockDecoder::~CDynBlockDecoder(void)
{
    deleteTree<uint32_t>(m_distance_root);
    m_distance_root = nullptr;

    deleteTree<uint32_t>(m_literal_root);
    m_literal_root = nullptr;

    deleteTree<uint32_t>(m_cl4cl_root);
    m_cl4cl_root = nullptr;

    m_HCLEN = m_HDIST = m_HLIT = 0u;
    return;
}


// Build Literal tree based on the Cl4Cl4
void CDynBlockDecoder::build_code_lenghts_tree (const std::vector<uint8_t>& in, CStreamPointer& sp)
{
    std::vector<uint32_t> litLens(m_HLIT, 0);
    uint32_t lastCode = 0u;

    // Build Literal and Distance Code Lengths Tree from Code Lengths for Code Lengths Tree
    for (uint32_t i = 0u; i < m_HLIT; i++)
    {
        // read Huffman Codes from stream with parallel traversing of the Cl4Cl tree
        const std::shared_ptr<Node<uint32_t>> hTreeLeaf = readHuffmanBits<uint32_t> (in, sp, m_cl4cl_root);

        switch (hTreeLeaf->symbol)
        {
            case 16: // repeat code
            {
                constexpr uint32_t extraBits = 2u;
                const uint32_t repeatCount = 3u + readBits (in, sp, extraBits) - 1u;
                std::fill (litLens.begin() + i, litLens.begin() + i + repeatCount, lastCode);
                i += repeatCount;
            }
            break;

            case 17: // zero code
            {
                constexpr uint32_t extraBits = 3u;
                const uint32_t zeroCount = 3u + readBits (in, sp, extraBits) - 1u;
                std::fill (litLens.begin() + i, litLens.begin() + i + zeroCount, 0u);
                i += zeroCount;
            }
            break;

            case 18: // zero code
            {
                constexpr uint32_t extraBits = 7u;
                const uint32_t zeroCount = 11u + readBits (in, sp, extraBits) - 1u;
                std::fill (litLens.begin() + i, litLens.begin() + i + zeroCount, 0u);
                i += zeroCount;
            }
            break;

            default: // rest of literal and distance codes
                litLens[i] = lastCode = hTreeLeaf->symbol;
            break;
        }
    }

    // build Literal Tree (Huffman alphabet)
    m_literal_root = buildHuffmanTreeFromLengths<uint32_t>(litLens);
#ifdef _DEBUG
    std::cout << "m_literal_root tree [size = " << computeTreeSize<uint32_t>(m_literal_root) << " leaves]:" << std::endl;
    printHuffmanTree(m_literal_root);
    std::cout << std::endl;
#endif

    return;
}


// Build Distance tree based on the Cl4Cl4
void CDynBlockDecoder::build_distance_tree (const std::vector<uint8_t>& in, CStreamPointer& sp)
{
    return;
}

// Parse Huffman stream, get HLIT, HDIST and HCLEN values, build Cl4Cl tree
void CDynBlockDecoder::pre_decode (const std::vector<uint8_t>& in, CStreamPointer& sp)
{
    // read Literal Lengths Codes (stream pointer incremented internally) 
    m_HLIT = get_HLIT(in, sp);
    // read Distance Codes (stream pointer incremented internally)
    m_HDIST = get_HDIST(in, sp);
    // read Codel Lengths forCode Lengths (stream pointer incremented internally)
    m_HCLEN = get_HCLEN(in, sp);

    std::vector<uint32_t> cl4cl(m_HCLEN, 0u); // prepare storage for read from Huffman stream Code Lengths for Code Lengths alphabet
    for (uint32_t i = 0u; i < m_HCLEN; i++)
        cl4cl[i] = readBits(in, sp, 3u); // read Code Lengths for Code Lengths 3 bits values

    // generate Huffman Code Lengths for Code Legths codes
    std::vector<std::pair</* code */ uint32_t, /* length */ uint32_t>> huffmanCodes = generate_huffman_codes(cl4cl, m_HCLEN);

    // re-order Cl4Cl alphabet
    std::vector<uint32_t> vecCodeLengths(cl4cl_dictionary_idx.size(), 0u);
    for (size_t i = 0ull; i < huffmanCodes.size(); i++)
        vecCodeLengths[cl4cl_dictionary_idx[i]] = huffmanCodes.at(i).second;

    // build Cl4Cl Huffman Tree
    m_cl4cl_root = buildHuffmanTreeFromLengths (vecCodeLengths);
#ifdef _DEBUG
    std::cout << "m_cl4cl_root tree [size = " << computeTreeSize<uint32_t>(m_cl4cl_root) << " leaves]:" << std::endl;
    printHuffmanTree(m_cl4cl_root);
    std::cout << std::endl;
#endif

    // build Code Lengts and Literal Tree
    build_code_lenghts_tree(in, sp);

    // build Disatnce Codes Tree
    build_distance_tree(in, sp);

    return;
}


CStreamPointer CDynBlockDecoder::decode (const std::vector<uint8_t>& in, std::vector<uint8_t>& out, CStreamPointer& sp)
{
    // cleanup output vector
    out.clear();

    // Initialize and Build all Huffman Dynamic Decoder Infrastructures (Cl4Cl, Huffman trees, etc...)
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

