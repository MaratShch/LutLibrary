#ifndef __C_HUFFMAN_TREE_OBJECT__
#define __C_HUFFMAN_TREE_OBJECT__

#include <memory>

// Templated Node structure for the Huffman tree
template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>   
struct Node
{
    T symbol;   // Literal/Distance can be any integer type (int, char, etc.)
    T code;   	// Huffman code
    T code_size;// Huffman code size
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;

    Node() : symbol(T()), code(T()), code_size(T()), left(nullptr), right(nullptr) {}
};

using Node32  = Node<int32_t>;
using Node32u = Node<uint32_t>;

#endif // __C_HUFFMAN_TREE_OBJECT__
