#include "CDecoderConstants.h"
#include "CDynBlockDecoder.h"
#include "CHuffmanStream.h"
#include "CHuffmanTree.h"
#include <cassert>
#include <iomanip>	// for formatted output
#include <stdexcept>    // for std::runtime_error
#include <sstream>
#include <string>

using namespace HuffmanUtils;


CDynBlockDecoder::~CDynBlockDecoder(void)
{
    deleteTree<uint32_t>(m_distance_root);
    m_distance_root = nullptr;

    deleteTree<uint32_t>(m_literal_root);
    m_literal_root = nullptr;

    deleteTree<uint32_t>(m_cl4cl_root);
    m_cl4cl_root = nullptr;

    m_cl4cl.clear();

    m_HCLEN = m_HDIST = m_HLIT = 0u;
    return;
}

std::shared_ptr<Node<uint32_t>> CDynBlockDecoder::build_huffman_tree(const std::vector<uint8_t>& in, CStreamPointer& sp, uint32_t treeSize)
{
    std::vector<uint32_t> tmpVector(treeSize, 0);
    constexpr uint32_t lastCodeBad = 0xFFFFFFFFu;
    uint32_t lastCode = lastCodeBad;

    for (uint32_t i = 0u; i < treeSize; /* i incremented inside of loop */)
    {
        // read Huffman Codes from stream with parallel traversing of the Cl4Cl tree
        const std::shared_ptr<Node<uint32_t>> hTreeLeaf = readHuffmanBits<uint32_t>(in, sp, m_cl4cl_root);

        switch (hTreeLeaf->symbol)
        {
            case 16: // repeat code
            {
                if (lastCodeBad == lastCode)
                     return nullptr; // last code non exist
                constexpr uint32_t extraBits = 2u;
                const uint32_t repeatCount = 3u + readBits(in, sp, extraBits);
                std::fill(tmpVector.begin() + i, tmpVector.begin() + i + repeatCount, lastCode);
                i += repeatCount;
            }
            break;

            case 17: // zero code
            {
                constexpr uint32_t extraBits = 3u;
                const uint32_t zeroCount = 3u + readBits(in, sp, extraBits);
                std::fill(tmpVector.begin() + i, tmpVector.begin() + i + zeroCount, 0u);
                i += zeroCount;
            }
            break;

            case 18: // zero code
            {
                constexpr uint32_t extraBits = 7u;
                const uint32_t zeroCount = 11u + readBits(in, sp, extraBits);
                std::fill(tmpVector.begin() + i, tmpVector.begin() + i + zeroCount, 0u);
                i += zeroCount;
            }
            break;

            default: // rest of literal and distance codes
            {
                if (i >= tmpVector.size())
                     return nullptr; // Prevent overflow

                tmpVector[i] = lastCode = hTreeLeaf->symbol;
                i++;
            } 
            break;
            
        }
    }

    return buildHuffmanTreeFromLengths<uint32_t>(tmpVector);
}



// Build Literal tree based on the Cl4Cl4
bool CDynBlockDecoder::build_code_lenghts_tree (const std::vector<uint8_t>& in, CStreamPointer& sp)
{
    m_literal_root = build_huffman_tree (in, sp, m_HLIT);
    return (nullptr != m_literal_root ? true : false);
}


// Build Distance tree based on the Cl4Cl4
bool CDynBlockDecoder::build_distance_tree (const std::vector<uint8_t>& in, CStreamPointer& sp)
{
    m_distance_root = build_huffman_tree (in, sp, m_HDIST);
    return (nullptr != m_distance_root ? true : false);
}


// Parse Huffman stream, get HLIT, HDIST and HCLEN values, build Cl4Cl tree
bool CDynBlockDecoder::pre_decode (const std::vector<uint8_t>& in, CStreamPointer& sp)
{
    m_cl4cl.clear();

    // read Literal Lengths Codes (stream pointer incremented internally) 
    m_HLIT = get_HLIT(in, sp);
    if (m_HLIT < min_HLIT || m_HLIT > max_HLIT)
        return false;

    // read Distance Codes (stream pointer incremented internally)
    m_HDIST = get_HDIST(in, sp);
    if (m_HDIST < min_HDIST || m_HDIST > max_HDIST)
        return false;

    // read Codel Lengths forCode Lengths (stream pointer incremented internally)
    m_HCLEN = get_HCLEN(in, sp);
    if (m_HCLEN < min_HCLEN || m_HCLEN > max_HCLEN)
        return false;

    uint32_t cl4cl_sum = 0u;
    std::vector<uint32_t> cl4cl(m_HCLEN, 0u); // prepare storage for read from Huffman stream Code Lengths for Code Lengths alphabet
    for (uint32_t i = 0u; i < m_HCLEN; i++)
    {   // read Cl4Cl lengths in order defined into rfc1951 standard
        cl4cl[i] = readBits(in, sp, 3u); // read Code Lengths for Code Lengths 3 bits values
        cl4cl_sum += cl4cl[i];
    }
    // validate cl4cl
    if (0u == cl4cl_sum)
        return false; // all zero codes - at least one code should not be equal to zero.

    // Order Code Lengths for Code Lengths alphabet
    m_cl4cl.resize(cl4cl_dictionary_idx.size(), 0);
    for (uint32_t i = 0u; i < m_HCLEN; i++)
        m_cl4cl[cl4cl_dictionary_idx[i]] = cl4cl[i];

    // build Cl4Cl Huffman Tree
    m_cl4cl_root = buildHuffmanTreeFromLengths (m_cl4cl);

    // build Code Lengts and Literal Tree
    const bool literalValid = build_code_lenghts_tree(in, sp);
#ifdef _DEBUG
    const bool McMillanLiteral = validateKraftMcMillan (m_literal_root);
#else
    constexpr bool McMillanLiteral = true;
#endif

    // build Disatnce Codes Tree
    const bool distanceValid = build_distance_tree(in, sp);
#ifdef _DEBUG
    const bool McMillanDistance = validateKraftMcMillan (m_distance_root);
#else
    constexpr bool McMillanDistance = true;
#endif

    return (literalValid && McMillanLiteral && distanceValid && McMillanDistance);
}

#ifdef _DEBUG
void dbg_print_on_crash (const std::vector<uint8_t>& out)
{
   std::cout << "Print output stream before crash:" << std::endl;
   for (const auto& outElem : out)
     std::cout << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(outElem) << " ";
   std::cout << std::endl;
   return;
}
     
#else
inline void dbg_print_on_crash (const std::vector<uint8_t>& out) {return;}
#endif


bool CDynBlockDecoder::decode (const std::vector<uint8_t>& in, std::vector<uint8_t>& out, CStreamPointer& sp)
{
    // reset deocder integrity status
    m_decoderIntegrityStatus = false;

    // cleanup output vector
    out.clear();

#ifdef _DEBUG
    const CStreamPointer dbgSpCopy(sp);
#endif

    // Initialize and Build all Huffman Dynamic Decoder Infrastructures (Cl4Cl, Huffman trees, etc...)
    if (true == pre_decode(in, sp))
    {
        uint32_t symbol = 0u;

        do {
            const std::shared_ptr<Node<uint32_t>> hLiteraLeaf = readHuffmanBits<uint32_t>(in, sp, m_literal_root);
            symbol = hLiteraLeaf->symbol;

            if (symbol <= 255u)
                out.push_back(static_cast<uint8_t>(hLiteraLeaf->symbol));
            else if (symbol >= cLengthCodesMin && symbol <= cLengthCodesMax)
            {
                auto process_distance_sequence = [this]
                (
                    const std::vector<uint8_t>& in,
                    CStreamPointer& sp,
                    const uint32_t distanceCode
                    ) -> std::pair<int32_t, int32_t>
                {
                    // Get Length information
                    const int32_t LengtCodeArrayIdx = distanceCode - cLengthCodesMin;
                    const int32_t extraBitsInLen = cLengthGetExtra(LengtCodeArrayIdx);
                    const int32_t baseLength = cLengthGetBaseLen(LengtCodeArrayIdx);
                    const int32_t finalLength = baseLength + (extraBitsInLen > 0u ? readBits(in, sp, extraBitsInLen) : 0u);

                    // Read distance code
                    const std::shared_ptr<Node<uint32_t>> hTreeLeaf = readHuffmanBits<uint32_t>(in, sp, m_distance_root);
                    const int32_t DistanceCodeArrayIdx = hTreeLeaf->symbol - cDistanceCodesMin;
                    const int32_t extraBitsInDist = cDistanceGetExtra(DistanceCodeArrayIdx);
                    const int32_t baseDistance = cDistanceGetBaseLen(DistanceCodeArrayIdx);
                    const int32_t finalDistance = baseDistance + (extraBitsInDist > 0u ? readBits(in, sp, extraBitsInDist) : 0u);

                    return std::make_pair(finalLength, finalDistance);
                };

                const std::pair<int32_t, int32_t> pair_distance = process_distance_sequence(in, sp, symbol);
                auto const& size = pair_distance.first;
                auto const& distance = pair_distance.second;
                const int32_t outVectorSize = static_cast<int32_t>(out.size());

                if (outVectorSize < distance)
                {
                    dbg_print_on_crash(out);

                    std::ostringstream ex;
                    ex << "Distance exceeds output buffer size: negative offset computed. Distance = " << distance << " out size = " << outVectorSize <<
#ifdef _DEBUG
                        " SP before = " << dbgSpCopy << " SP after = " << sp; // 83.7 -> 86.2
#else
                        " SP = " << sp;
#endif
                    const std::string ex_as_string(ex.str());

                    throw std::runtime_error(ex_as_string);
                    return false;
                }
                else
                {
                    auto const pre = outVectorSize - distance;
                    for (int32_t i = 0; i < size; i++)
                        out.push_back(out[pre + i]);
                }

            }

        } while (symbol != EndOfBlock);

        auto convertEndian = [](uint32_t value) -> uint32_t
        {
            return ((value >> 24) & 0x000000FF) | // Move byte 3 to byte 0
                ((value >> 8) & 0x0000FF00) | // Move byte 2 to byte 1
                ((value << 8) & 0x00FF0000) | // Move byte 1 to byte 2
                ((value << 24) & 0xFF000000);  // Move byte 0 to byte 3
        };

        sp.align2byte(); // Skip any remaining padding bits until the byte boundary is reached

        const auto streamTail = in.size() - sp2byte(sp);
        if (5ull == streamTail) // remove zero padding bit
            sp.to_next_byte();

        // integirty check: read ADLER-32 checksum
        const uint32_t adler32Expected = convertEndian(readBits(in, sp, 32u));
        // integirty check: compute ADLER-32 checksum
        const uint32_t adler32Computed = computeAdler32(out);
        // validate checksums
        m_decoderIntegrityStatus = (adler32Expected == adler32Computed);
    }

  return m_decoderIntegrityStatus;
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

