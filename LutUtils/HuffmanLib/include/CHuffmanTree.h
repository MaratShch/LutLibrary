#ifndef __C_HUFFMAN_TREE_OBJECT__
#define __C_HUFFMAN_TREE_OBJECT__

#include <memory>
#include <vector>
#include <bitset>

namespace HuffmanUtils
{

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

    using Node32 = Node<int32_t>;
    using Node32u = Node<uint32_t>;


    // Function to build a Huffman tree from code lengths (templated)
    template<typename T>
    std::shared_ptr<Node<T>> buildHuffmanTreeFromLengths (const std::vector<T>& codeLengths)
    {
        std::shared_ptr<Node<T>> root = std::make_shared<Node<T>>();

        // Lambda for inserting a symbol into the Huffman tree
        auto insertCode = [&root](const std::string& code, T symbol) {
            std::shared_ptr<Node<T>> current = root;
            uint32_t val = 0x0u;

            for (char bit : code) {
                val = (val << 1) | (bit - '0'); // Shift and set the bit
                if (bit == '0') {
                    if (!current->left) current->left = std::make_shared<Node<T>>();
                    current = current->left;
                }
                else { // bit == '1'
                    if (!current->right) current->right = std::make_shared<Node<T>>();
                    current = current->right;
                }
            }

            current->symbol = symbol;
            current->code = val;
        };

        // Generate the binary Huffman codes for each symbol using the code lengths
        std::vector<std::string> codes(codeLengths.size());
        T nextCode{ 0 };

        // Build the code for each length in ascending order
        for (int32_t codeLength = 1; codeLength <= 15; ++codeLength)
        {
            for (T symbol = 0; symbol < static_cast<T>(codeLengths.size()); ++symbol)
            {
                if (codeLengths[symbol] == codeLength)
                {
                    // Convert nextCode to a binary string with codeLength bits
                    std::string binaryCode = std::bitset<16>(nextCode).to_string().substr(16 - codeLength);
                    codes[symbol] = binaryCode;
                    insertCode(binaryCode, symbol); // Call the lambda
                    ++nextCode;
                }
            }
            nextCode <<= 1; // Increment to the next code of the current length
        }

        return root;
    }



    template<typename T>
    void printHuffmanTree (const std::shared_ptr<Node<T>>& node, const std::string& prefix = "")
    {
        if (!node->left && !node->right)
        {
            std::cout << "Symbol: " << node->symbol << ", Code: " << prefix << std::endl;
        }
        else
        {
            if (node->left)
            {
                printHuffmanTree(node->left, prefix + "0");
            }
            if (node->right)
            {
                printHuffmanTree(node->right, prefix + "1");
            }
        }

        return;
    }

};


#endif // __C_HUFFMAN_TREE_OBJECT__
