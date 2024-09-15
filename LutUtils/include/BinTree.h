#ifndef __LUT_UTILS_BINARY_TREE__
#define __LUT_UTILS_BINARY_TREE__

template <typename T>
class UtilsBinTreeNode
{
private:
	T node{};
	T* left = nullptr;
	T* right= nullptr;

public:

	UtilsBinTreeNode () noexcept { ; }

	UtilsBinTreeNode (const T& val_node) noexcept
	{ 
		this->left  = nullptr;
		this->right = nullptr;
		this->node  = val_node;
	}

	UtilsBinTreeNode (const T& val_node, const T* l, const T* r) noexcept
	{
		this->left  = l;
		this->right = r;
		this->node  = val_node;
	}

	virtual ~UtilsBinTreeNode() { ; }

};


#endif /* __LUT_UTILS_BINARY_TREE__ */