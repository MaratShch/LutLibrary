#ifndef __C_HUFFMAN_TREE_OBJECT__
#define __C_HUFFMAN_TREE_OBJECT__

#include <memory>
#include <vector>
#include <bitset>
#include <iostream>
#include <cmath>
#include <map>

namespace HuffmanUtils
{

    // Templated Node structure for the Huffman tree
    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
    struct Node
    {
        T symbol = static_cast<T>(-1);  // Sentinel value for uninitialized
        T code = static_cast<T>(0);     // Optional: debugging or additional info
        std::shared_ptr<Node<T>> left = nullptr;
        std::shared_ptr<Node<T>> right = nullptr;

        bool isLeaf() const {return left == nullptr && right == nullptr;}
    };

    using Node32 = Node<int32_t>;
    using Node32u = Node<uint32_t>;

    // Function to add a code to the Huffman tree
    template <typename T>
    void addCodeToTree(std::shared_ptr<Node<T>>& root, T code, int32_t length, T symbol)
    {
        auto current = root;
        for (int32_t i = length - 1; i >= 0; --i)
        {
            const T bit = static_cast<T>((code >> i) & 1);
            if (bit == 0)
            {
                if (!current->left) current->left = std::make_shared<Node<T>>(-1); // -1 for intermediate nodes
                current = current->left;
            }
            else
            {
                if (!current->right) current->right = std::make_shared<Node<T>>(-1);
                current = current->right;
            }
        }
        current->symbol = symbol; // Assign the symbol at the leaf node
        return;
    }


    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
    std::shared_ptr<Node<T>> buildHuffmanTreeFromLengths (const std::vector<T>& codeLengths, uint32_t maxLength = 15u)
    {
        if (codeLengths.empty())
            throw std::invalid_argument("Input codeLengths vector is empty.");

        auto root = std::make_shared<Node<T>>();
        T nextCode{ 0 };

        // Preprocess code lengths into buckets
        std::map<uint32_t, std::vector<T>> lengthBuckets;
        for (T symbol = 0; symbol < static_cast<T>(codeLengths.size()); ++symbol)
        {
            if (codeLengths[symbol] > 0)
            {
                if (codeLengths[symbol] > static_cast<decltype(codeLengths[symbol])>(maxLength))
                    throw std::invalid_argument("Code length exceeds maximum allowed length.");

                lengthBuckets[codeLengths[symbol]].push_back(symbol);
            }
        }

        // Build the Huffman tree
        for (uint32_t codeLength = 1u; codeLength <= maxLength; ++codeLength)
        {
            for (T symbol : lengthBuckets[codeLength])
            {
                T code{ nextCode };

                // Insert the code into the Huffman tree
                auto current{ root };
                for (int32_t bitIndex = static_cast<int32_t>(codeLength) - 1; bitIndex >= 0; --bitIndex)
                {
                    bool bit = (code >> bitIndex) & 0x1;
                    if (bit)
                    {
                        if (!current->right) current->right = std::make_shared<Node<T>>();
                        current = current->right;
                    }
                    else
                    {
                        if (!current->left) current->left = std::make_shared<Node<T>>();
                        current = current->left;
                    }
                }

                // Validate and assign the symbol
                if (current->isLeaf() && current->symbol != static_cast<T>(-1))
                    throw std::runtime_error("Huffman tree conflict: Node already contains a symbol.");

                current->symbol = symbol;
#ifdef _DEBUG
                current->code = code;
#endif
                ++nextCode;
            }

            nextCode <<= 1;
        }

        return root;
    }



    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
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


    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
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
    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
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
    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
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


    // Function to traverse the tree and collect code lengths
    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
    void collectCodeLengths (const std::shared_ptr<Node<T>>& node, std::vector<T>& codeLengths, T depth = static_cast<T>(0))
    {
        if (!node) return;

        // Leaf node condition
        if (!node->left && !node->right) {
            codeLengths.push_back(depth);
            return;
        }

        // Traverse left and right subtrees
        collectCodeLengths(node->left,  codeLengths, depth + static_cast<T>(1));
        collectCodeLengths(node->right, codeLengths, depth + static_cast<T>(1));
    }

    // Function to validate Huffman tree using Kraft-McMillan inequality
    template <typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
    bool validateKraftMcMillan (const std::shared_ptr<Node<T>>& root)
    {
        std::vector<T> codeLengths{};

        // Collect code lengths from the tree
        collectCodeLengths (root, codeLengths);

        // Apply Kraft-McMillan inequality
        double sum = 0.0;
        for (auto const& length : codeLengths)
            sum += std::pow(2.0, -(static_cast<int>(length)));

        // Check if the inequality holds [Allow for a small epsilon in floating-point comparison]
        constexpr double epsilon = 1e-6;
        return (sum <= 1.0 + epsilon);
    }


}; //namespace HuffmanUtils


#endif // __C_HUFFMAN_TREE_OBJECT__
