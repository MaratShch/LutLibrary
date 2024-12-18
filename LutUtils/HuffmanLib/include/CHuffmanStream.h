#ifndef __HUFFMAN_STREAM_API_DEFINITION__
#define __HUFFMAN_STREAM_API_DEFINITION__

#include <vector>
#include <utility>
#include <memory>
#include "CHuffmanStreamPointer.h"
#include "CHuffmanTree.h"

namespace HuffmanUtils
{

    // Read bit from specific position pointed by Stream Pointer.
    // Pay attention! This function isn't modify Stream Pointer after read.
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

    // Read Huffman Codes from Huffman stream. Stream pointer will be automatically incremented
    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
    inline const std::shared_ptr<Node<T>> readHuffmanBits
    (
        const std::vector<uint8_t>& stream,  // Input Huffman Stream 
        CStreamPointer& streamOffset,        // Stream pointer (bits offset), incremented internally
        const std::shared_ptr<Node<T>>& node // Root of the Huffman Tree for traversal
    )
    {
        auto huffmanNode{ node };

        // Traverse the Huffman Tree until a leaf node is found
        while (nullptr != huffmanNode && (huffmanNode->left || huffmanNode->right))
        {
            const uint32_t huffmanBit = readBit(stream, streamOffset); // Read one bit
            streamOffset++;                                            // Forward stream pointer to the next bit
            huffmanNode = (huffmanBit == 0u) ? huffmanNode->left : huffmanNode->right; // Move to the next node based on the bit value
        }

        // Ensure we reached a valid leaf node
        if (nullptr == huffmanNode)
            throw std::runtime_error("Huffman decoding error: Reached a nullptr node.");

        return huffmanNode; // Return the decoded leaf node
    }

    inline uint32_t readStaticHuffmanBits
    (
        const std::vector<uint8_t>& stream, // input Huffman Stream 
        CStreamPointer& streamOffset,       // stream pointer (bits offset), value incremented internally
        uint32_t bits                       // size of Huffman static code for read
    )
    {
        uint32_t huffmanCode = 0u;
        uint32_t shft = 0u;
        for (uint32_t i = 0u; i < bits; i++)
        {
            const uint32_t huffmanBit = readBit(stream, streamOffset);
            (huffmanCode <<= shft) |= huffmanBit;
            shft = 1u;
            streamOffset++;
        }
        return huffmanCode;
    }

    inline uint32_t readComplementarStaticHuffmanBits
    (
        const std::vector<uint8_t>& stream, // input Huffman Stream 
        CStreamPointer& streamOffset,       // stream pointer (bits offset), value incremented internally
        uint32_t code                       // size of Huffman static code for read
    )
    {
        const uint32_t huffmanCode = (code << 1) | readBit(stream, streamOffset);
        streamOffset++;
        return huffmanCode;
    }


    inline uint32_t computeAdler32 (const std::vector<uint8_t> data)
    {
        uint32_t A = 1u; // A starts with 1
        uint32_t B = 0u; // B starts with 0
        constexpr uint32_t MOD_ADLER = 65521u;

        for (const auto& element : data)
        {
            A = (A + element) % MOD_ADLER;
            B = (B + A) % MOD_ADLER;
        }

        return (B << 16) | A; // Combine B (high) and A (low) into a 32-bit checksum
    }

}; // namespace HuffmanUtils

#endif // __HUFFMAN_STREAM_API_DEFINITION__
