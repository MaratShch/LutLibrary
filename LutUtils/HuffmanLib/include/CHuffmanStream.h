#ifndef __HUFFMAN_STREAM_API_DEFINITION__
#define __HUFFMAN_STREAM_API_DEFINITION__

#include <vector>
#include <utility>
#include <memory>
#include "CHuffmanStreamPointer.h"
#include "CHuffmanTree.h"

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

    // Read Huffman Codes from Huffman stream. Stream pointer will be automatically incremented
    template <typename T>
    inline const std::shared_ptr<Node<T>> readHuffmanBits
    (
        const std::vector<uint8_t>& stream,     // input Huffman Stream 
        CStreamPointer& streamOffset,           // stream pointer (bits offset) 
        const std::shared_ptr<Node<T>>& node    // tree for traversing and stop read when code will be find
    )
    {
        std::shared_ptr<Node<T>> huffmanNode = node;
        uint32_t huffmanCode = 0u;
        uint32_t position = 0u;

        do {
            const uint32_t huffmanBit = readBit(stream, streamOffset);;
            (huffmanCode <<= position) |= huffmanBit;
            streamOffset++, position++;
            huffmanNode = ((0u == huffmanBit) ? huffmanNode->left : huffmanNode->right);
        } while (nullptr != huffmanNode->left || nullptr != huffmanNode->right);

        return huffmanNode;
    }

}; // namespace HuffmanUtils

#endif // __HUFFMAN_STREAM_API_DEFINITION__