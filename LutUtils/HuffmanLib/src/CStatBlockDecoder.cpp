#include "CStatBlockDecoder.h"
#include "CDecoderConstants.h"
#include "CHuffmanIo.h"
#include <sstream>

using namespace HuffmanUtils;


CStatBlockDecoder::~CStatBlockDecoder(void)
{
   return;
}


void CStatBlockDecoder::createCodesTable (void)
{
    uint32_t i = 0u;
    // initialize Huffman Static Codes array with invalid code values
    mStaticCodes.fill(InvalidStaticCodeId);
    // fill by Literal/Length Codes (in range from 0...287) 
    for (i = 0u;   i <= 143u; ++i) mStaticCodes[i] = 0b00110000  + i;           // Literal/Length codes 0-143:   8-bit codes.
    for (        ; i <= 255u; ++i) mStaticCodes[i] = 0b110010000 + (i - 144u);  // Literal/Length codes 144-255: 9-bit codes.
    for (        ; i <= 279u; ++i) mStaticCodes[i] = i - 256u;                  // Literal/Length codes 256-279: 7-bit codes.
    for (        ; i <= 285u; ++i) mStaticCodes[i] = 0b11000000  + (i - 280u);  // Literal/Length codes 280-285: 8-bit codes.
    // Literal/length values 286-287 will never actually occur in the compressed data, but participate in the code construction.
    return;
}

void CStatBlockDecoder::createReverseTable (void)
{
    // initialize Huffman Static Codes array with invalid code values
    mCodesLookUp.fill(InvalidStaticCodeId);
    for (uint32_t i = 0u; i < 512u; i++)
        mCodesLookUp[mStaticCodes[i]] = i;

    return;
}


bool CStatBlockDecoder::pre_decode (void)
{
    createCodesTable();
    createReverseTable();

    return true;
}


uint32_t CStatBlockDecoder::read_fixed_huffman_code (const std::vector<uint8_t>& in, CStreamPointer& sp)
{
    uint32_t symbol = InvalidStaticCodeId;
    const uint32_t code7 = readStaticHuffmanBits (in, sp, 7);
    if (InvalidStaticCodeId == (symbol = mCodesLookUp[code7]))
    {
        // This code doesn't match 7 bits Static Huffman Codes values.
        // Let's try to identify this code as 8 bits Static Huffman Code
        const uint32_t code8 = readComplementarStaticHuffmanBits(in, sp, code7);
        if (InvalidStaticCodeId == (symbol = mCodesLookUp[code7]))
        {
            // Again, this code doen't match 8 bits Static Huffman Codes values.
            // Last chance to try identify this code as 9 bits Static Huffman Code
            const uint32_t code9 = readComplementarStaticHuffmanBits(in, sp, code8);
            symbol = mCodesLookUp[code9];
            if (InvalidStaticCodeId == symbol) // Invalid code received
                throw std::runtime_error("Unable to extract symbol from Fixed Huffman Code stream: <7>:" + std::to_string(code7) + " <8>:" + 
                    std::to_string(code8) + " <9>:" + std::to_string(code9) + ".");
        }
    }

   return symbol;
}



bool CStatBlockDecoder::decode (const std::vector<uint8_t>& in, std::vector<uint8_t>& out, CStreamPointer& sp)
{
    bool bResult = true;

    // Initialize and Build all Huffman Dynamic Decoder Infrastructures (Cl4Cl, Huffman trees, etc...)
    if (true == (bResult = pre_decode()))
    {
        auto process_distance_sequence = [this]
        (
            const std::vector<uint8_t>& in,
            CStreamPointer& sp,
            const uint32_t& distanceCode
        ) -> const std::pair<int32_t, int32_t>
        {
            // Get Length information
            const int32_t LengtCodeArrayIdx = distanceCode - cLengthCodesMin;
            const int32_t extraBitsInLen = cLengthGetExtra(LengtCodeArrayIdx);
            const int32_t baseLength = cLengthGetBaseLen(LengtCodeArrayIdx);
            const int32_t finalLength = baseLength + (extraBitsInLen > 0 ? readBits(in, sp, extraBitsInLen) : 0);

            if (finalLength > std::numeric_limits<uint32_t>::max() - (extraBitsInLen > 0 ? finalLength : 0u))
                throw std::runtime_error("Potential overflow of size.");

            // Read distance size
            const uint32_t distanceSizeCode = readBits(in, sp, 5);
            if (distanceSizeCode >= 0u && distanceSizeCode <= 29u) // let's ensure received distance code in range 0...29
            {
                const int32_t DistanceCodeArrayIdx = distanceSizeCode - cDistanceCodesMin;
                const int32_t extraBitsInDist = cDistanceGetExtra(DistanceCodeArrayIdx);
                const int32_t baseDistance = cDistanceGetBaseLen(DistanceCodeArrayIdx);
                const int32_t finalDistance = baseDistance + (extraBitsInDist > 0 ? readBits(in, sp, extraBitsInDist) : 0);

                return std::make_pair(finalLength, finalDistance);
            }
            return std::make_pair(0, 0);
        };

        uint32_t symbol = InvalidStaticCodeId;

        // decoding loop
        do {
            const CStreamPointer dbgSp{ sp };

            symbol = read_fixed_huffman_code(in, sp);

            if (symbol <= 255u)
                out.push_back(static_cast<uint8_t>(symbol)); // put literal code 
            else if (symbol >= static_cast<uint32_t>(cLengthCodesMin) && symbol <= static_cast<uint32_t>(cLengthCodesMax)) // process distance/length codes
            {
                const std::pair<int32_t, int32_t> pair_distance = process_distance_sequence(in, sp, symbol);
                auto const& size = pair_distance.first;
                auto const& distance = pair_distance.second;

                if (0 == distance || 0 == size) // nothing to copy
                    throw std::runtime_error("FIX: Distance or size equal to zero. Probably stream corrupted: distance = " + std::to_string(distance) + " size = " + std::to_string(size) + ".");

                const int32_t outVectorSize = static_cast<int32_t>(out.size());
                constexpr int32_t maxWinSize{ static_cast<int32_t>(max_WindowSize) };
                if (outVectorSize < distance || distance > maxWinSize)
                {
                    std::ostringstream ex;
                    ex << "FIX: Distance exceeds output buffer or max allowed window size " << maxWinSize << " bytes. Huffman Code = " << symbol
                        << ". Distance = " << distance << " bytes. Out size = " << outVectorSize << " bytes. SP(before) = " << dbgSp << " SP(after) = " << sp;

                    const std::string ex_as_string(ex.str());
                    throw std::runtime_error(ex_as_string);
                    return false;
                }

                auto const pre = outVectorSize - distance;

                // Preallocate space for the output vector
                out.reserve(out.size() + size);

                // Perform the copy operation
                for (int32_t i = 0; i < size; i++)
                    out.push_back(out[pre + i]);

            } // else if (symbol >= static_cast<uint32_t>(cLengthCodesMin) && symbol <= static_cast<uint32_t>(cLengthCodesMax))
            else if (symbol == InvalidStaticCodeId) // invalid code riched
                throw std::runtime_error("FIX: Invalid Fixed Huffman Code Detected: " + std::to_string(symbol) + ".");

        } while (symbol != EndOfBlock);
    
        bResult = true;
    } // if (true == pre_decode())
    else
        bResult = false;

   return bResult;
}

