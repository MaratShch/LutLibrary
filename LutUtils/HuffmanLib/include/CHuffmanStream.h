#ifndef __HUFFMAN_STREAM_API_DEFINITION__
#define __HUFFMAN_STREAM_API_DEFINITION__

#include <vector>
#include <utility>
#include "CHuffmanStreamPointer.h"

namespace HuffmanUtils
{

    std::vector<std::pair<uint32_t, uint32_t>> generate_huffman_codes (const std::vector<uint32_t>& code_lengths, int32_t num_symbols);

    // Read bit from specific position pointed by Stream Pointer.
    // Pay attention! This function isn't modfify Stream Pointer after read.
    inline uint32_t readBit (const std::vector<uint8_t>& stream, const CStreamPointer& streamOffset) 
    {
#ifdef _DEBUG
        return (stream.at(streamOffset.byte()) >> streamOffset.bit()) & 0x01u;
#else
        return (stream[streamOffset.byte()] >> streamOffset.bit()) & 0x01u;
#endif
    }

    // Read number of regular bits from Huffman Stream pointed by Stream Pointer.
    // Stream Pointer will be incremented automatically.
    inline uint32_t readBits (const std::vector<uint8_t>& stream, CStreamPointer& streamOffset, uint32_t bitsRead = 1u) 
    {
        uint32_t value = 0u;
        for (uint32_t i = 0; i < bitsRead; i++)
        {
            value |= ((readBit(stream, streamOffset)) << i);
            streamOffset++;
        }
        return value;
    }

}; // namespace HuffmanUtils

#endif // __HUFFMAN_STREAM_API_DEFINITION__