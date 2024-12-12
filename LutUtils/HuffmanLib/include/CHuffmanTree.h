#ifndef __C_HUFFMAN_TREE_OBJECT__
#define __C_HUFFMAN_TREE_OBJECT__

#include <memory>
#include <vector>
#include <bitset>
#include <iostream>
#include <cmath>

namespace HuffmanUtils
{

    // Templated Node structure for the Huffman tree
    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
    struct Node
    {
        T symbol;   // Literal/Distance can be any integer type (int, char, etc.)
        T code;   	// Huffman code
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;

        Node() : symbol(T()), code(T()), left(nullptr), right(nullptr) {}
    };

    using Node32 = Node<int32_t>;
    using Node32u = Node<uint32_t>;

    template <typename T>
    std::shared_ptr<Node<T>> buildHuffmanTreeFromLengths (const std::vector<T>& codeLengths)
    {
        std::shared_ptr<Node<T>> root = std::make_shared<Node<T>>();

        T nextCode = 0; // Tracks the next code to assign
        constexpr uint32_t maxLength = 15u; // Maximum code length (per Deflate spec)

        // Build the Huffman tree
        for (uint32_t codeLength = 1u; codeLength <= maxLength; ++codeLength)
        {
            for (T symbol = 0; symbol < static_cast<T>(codeLengths.size()); ++symbol)
            {
                if (codeLengths[symbol] == codeLength)
                {
                    uint32_t code = nextCode;

                    // Insert the code into the Huffman tree
                    auto current = root;
                    for (int bitIndex = codeLength - 1; bitIndex >= 0; --bitIndex)
                    {
                        // Extract the current bit (MSB first)
                        bool bit = (code >> bitIndex) & 1;

                        if (bit)
                        {
                            // Create a right child if it doesn't exist
                            if (!current->right) current->right = std::make_shared<Node<T>>();
                            current = current->right;
                        }
                        else
                        {
                            // Create a left child if it doesn't exist
                            if (!current->left) current->left = std::make_shared<Node<T>>();
                            current = current->left;
                        }
                    }

                    // Assign the symbol to the leaf node
                    current->symbol = symbol;
                    current->code = code; // Optional for debugging
                    ++nextCode;
                }
            }

            // Prepare the next code by shifting to the next length
            nextCode <<= 1;
        }

        return root;
    }


    template<typename T>
    void printHuffmanTree (const std::shared_ptr<Node<T>>& node, const std::string& prefix = "")
    {
        if (!node->left && !node->right)
            std::cout << "Symbol: " << node->symbol << ", Code: " << prefix << std::endl;
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


    template<typename T>
    bool printHuffmanLeaf (const std::shared_ptr<Node<T>>& node, T targetSymbol, const std::string& prefix = "") 
    {
	if (!node) 
	    return false; // Base case: Empty node.

	    // Check if it's a leaf node and the symbol matches.
	    if (!node->left && !node->right) 
	    {
		if (node->symbol == targetSymbol) 
		{
		    std::cout << "Found Symbol: " << node->symbol << ", Code: " << prefix << std::endl;
		    return true;
		}
		return false; // Leaf node but symbol does not match.
	    }

	    // Recursively search the left subtree with added "0" to the prefix.
	    if (node->left && printHuffmanLeaf(node->left, targetSymbol, prefix + "0")) 
		return true;

	    // Recursively search the right subtree with added "1" to the prefix.
	    if (node->right && printHuffmanLeaf(node->right, targetSymbol, prefix + "1")) 
		return true;

        return false; // Symbol not found in this subtree.
    }

    // Recursive cleanup function
    template <typename T>
    void deleteTree (std::shared_ptr<Node<T>>& node)
    {
        if (!node) return;

        // Post-order traversal: clean left, right, then the current node
        deleteTree<T>(node->left);
        deleteTree<T>(node->right);

        // Explicitly reset the current node's children
        node->left.reset();
        node->right.reset();

        // Finally reset the current node itself
        node.reset();
    }


    // Function to compute the size (number of leaves) in the tree
    template <typename T>
    size_t computeTreeSize (const std::shared_ptr<Node<T>>& root)
    {
        if (!root)
        {
            return 0; // No nodes in an empty tree
        }

        // If it's a leaf node (no children), count it
        if (!root->left && !root->right)
        {
            return 1;
        }

        // Otherwise, count leaves in the left and right subtrees
        return computeTreeSize(root->left) + computeTreeSize(root->right);
    }

};


#endif // __C_HUFFMAN_TREE_OBJECT__
