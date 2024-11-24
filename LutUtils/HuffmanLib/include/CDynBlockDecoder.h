#ifndef __C_HUFFMAN_DYNAMIC_BLOCK_DECODER_IMPLEMENTATION__
#define __C_HUFFMAN_DYNAMIC_BLOCK_DECODER_IMPLEMENTATION__

#include "IBlockDecoder.h"
#include "CHuffmanTree.h"
#include <array>

namespace HuffmanUtils
{
    // Alpahabet for build Code Lenghts For Code Lengths Tree
    constexpr std::array<uint32_t, 19> cl4cl_dictionary_idx =
    {
        // https://datatracker.ietf.org/doc/html/rfc1951
        16u, 17u, 18u, 0u, 8u, 7u, 9u, 6u, 10u, 5u, 11u, 4u, 12u, 3u, 13u, 2u, 14u, 1u, 15u
    };


    class CDynBlockDecoder : public IBlockDecoder
    {
       public:
           virtual ~CDynBlockDecoder(void);

           CStreamPointer decode (const uint8_t* in, uint8_t* out, size_t outSstorageSize, CStreamPointer& inSp);
           CStreamPointer decode (const std::vector<uint8_t>& in, std::vector<uint8_t>& out, CStreamPointer& inSp);
           uint8_t get_decoder_type(void) { return 0x02u; }

       private:
           
           uint32_t m_HCLEN = 0u;
           uint32_t m_HLIT  = 0u;
           uint32_t m_HDIST = 0u;
 
           std::shared_ptr<Node<uint32_t>> m_cl4cl_root = nullptr;    // code lenghts for code lengths tree
           std::shared_ptr<Node<uint32_t>> m_literal_root = nullptr;  // Huffman Literal tree
           std::shared_ptr<Node<uint32_t>> m_distance_root = nullptr; // Huffman Disatnce tree

           uint32_t get_HLIT (const std::vector<uint8_t>& in, CStreamPointer& sp);
           uint32_t get_HCLEN(const std::vector<uint8_t>& in, CStreamPointer& sp);
           uint32_t get_HDIST(const std::vector<uint8_t>& in, CStreamPointer& sp);

           void pre_decode (const std::vector<uint8_t>& in, CStreamPointer& sp);
           void build_code_lenghts_tree (const std::vector<uint8_t>& in, CStreamPointer& sp);
           void build_distance_tree(const std::vector<uint8_t>& in, CStreamPointer& sp);

    }; // class CDynBlockDecoder : public IBlockDecoder
	
}; // namespace HuffmanUtils



#endif // __C_HUFFMAN_DYNAMIC_BLOCK_DECODER_IMPLEMENTATION__
