#ifndef __C_HUFFMAN_DYNAMIC_BLOCK_DECODER_IMPLEMENTATION__
#define __C_HUFFMAN_DYNAMIC_BLOCK_DECODER_IMPLEMENTATION__

#include "IBlockDecoder.h"
#include "CHuffmanTree.h"
#include "CHuffmanStreamPointer.h"

namespace HuffmanUtils
{

    class CDynBlockDecoder : public IBlockDecoder
    {
       public:
           virtual ~CDynBlockDecoder(void);

           bool decode (const std::vector<uint8_t>& in, std::vector<uint8_t>& out, CStreamPointer& inSp);
           uint8_t get_decoder_type(void) { return 0x02u; }

       private:
           
           uint32_t m_HCLEN = 0u;
           uint32_t m_HLIT  = 0u;
           uint32_t m_HDIST = 0u;
 
           std::vector<uint32_t> m_cl4cl;

           std::shared_ptr<Node<uint32_t>> m_cl4cl_root = nullptr;    // code lenghts for code lengths tree
           std::shared_ptr<Node<uint32_t>> m_literal_root = nullptr;  // Huffman Literal tree
           std::shared_ptr<Node<uint32_t>> m_distance_root = nullptr; // Huffman Disatnce tree

           uint32_t get_HLIT (const std::vector<uint8_t>& in, CStreamPointer& sp);
           uint32_t get_HCLEN(const std::vector<uint8_t>& in, CStreamPointer& sp);
           uint32_t get_HDIST(const std::vector<uint8_t>& in, CStreamPointer& sp);

           bool pre_decode (const std::vector<uint8_t>& in, CStreamPointer& sp); // initialize Dynamic Huffman Decoder infrastruture
           bool build_code_lenghts_tree (const std::vector<uint8_t>& in, CStreamPointer& sp); // build Literal Tree
           bool build_distance_tree(const std::vector<uint8_t>& in, CStreamPointer& sp); // build Distance Tree

           std::shared_ptr<Node<uint32_t>> build_huffman_tree (const std::vector<uint8_t>& in, CStreamPointer& sp, uint32_t treeSize);

    }; // class CDynBlockDecoder : public IBlockDecoder
	
}; // namespace HuffmanUtils



#endif // __C_HUFFMAN_DYNAMIC_BLOCK_DECODER_IMPLEMENTATION__
