#include "CDecoderConstants.h"
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


std::shared_ptr<Node<uint32_t>> CDynBlockDecoder::build_huffman_tree(const std::vector<uint8_t>& in, CStreamPointer& sp, uint32_t treeSize)
{
    std::vector<uint32_t> tmpVector(treeSize, 0);
    uint32_t lastCode = 0u;

    for (uint32_t i = 0u; i < treeSize; /* i incremented inside of loop */)
    {
        // read Huffman Codes from stream with parallel traversing of the Cl4Cl tree
        const std::shared_ptr<Node<uint32_t>> hTreeLeaf = readHuffmanBits<uint32_t>(in, sp, m_cl4cl_root);

        switch (hTreeLeaf->symbol)
        {
            case 16: // repeat code
            {
                constexpr uint32_t extraBits = 2u;
                const uint32_t repeatCount = 3u + readBits(in, sp, extraBits);
                std::fill(tmpVector.begin() + i, tmpVector.begin() + i + repeatCount - 1u, lastCode);
                i += repeatCount;
            }
            break;

            case 17: // zero code
            {
                constexpr uint32_t extraBits = 3u;
                const uint32_t zeroCount = 3u + readBits(in, sp, extraBits);
                std::fill(tmpVector.begin() + i, tmpVector.begin() + i + zeroCount - 1u, 0u);
                i += zeroCount;
            }
            break;

            case 18: // zero code
            {
                constexpr uint32_t extraBits = 7u;
                const uint32_t zeroCount = 11u + readBits(in, sp, extraBits);
                std::fill(tmpVector.begin() + i, tmpVector.begin() + i + zeroCount - 1u, 0u);
                i += zeroCount;
            }
            break;

            default: // rest of literal and distance codes
                tmpVector[i] = lastCode = hTreeLeaf->symbol;
                i++;
            break;
        }
    }

    return buildHuffmanTreeFromLengths<uint32_t>(tmpVector);
}


// Build Literal tree based on the Cl4Cl4
void CDynBlockDecoder::build_code_lenghts_tree (const std::vector<uint8_t>& in, CStreamPointer& sp)
{
    m_literal_root = build_huffman_tree (in, sp, m_HLIT);
    return;
}


// Build Distance tree based on the Cl4Cl4
void CDynBlockDecoder::build_distance_tree (const std::vector<uint8_t>& in, CStreamPointer& sp)
{
    m_distance_root = build_huffman_tree (in, sp, m_HDIST);
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

    std::cout << std::endl;

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

    bool EndOfBlock = false;

    do {
        const std::shared_ptr<Node<uint32_t>> hLiteraLeaf = readHuffmanBits<uint32_t>(in, sp, m_literal_root);
        const uint32_t letter = hLiteraLeaf->symbol;
        if (letter > 255)
            EndOfBlock = true, std::cout << "Distance code " << letter << " detected. SP = " << sp << std::endl;
        else
            std::cout << "First character \'" << static_cast<char>(letter) << "\' decoded. SP = " << sp << std::endl;
    } while (EndOfBlock == false);


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

