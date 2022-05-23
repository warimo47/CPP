#include "BinarySearchTree.h"

#include <iostream>

void BinarySearchTree::PrintTree()
{
	PrintNode(0, _root);
}

void BinarySearchTree::PrintNode(int step, BSTNode* node)
{
	if (node == nullptr)
		return;

	PrintNode(step + 1, node->left);

	for (int i = 0; i < step; ++i)
	{
		std::cout.width(4);
		std::cout << " ";
	}

	std::cout.width(4);
	std::cout << node->key << std::endl;

	PrintNode(step + 1, node->right);
}

void BinarySearchTree::Insert(int key)
{
	BSTNode* newNode = new BSTNode();
	newNode->key = key;

	if (_root == nullptr)
	{
		// newNode->parents = newNode;
		_root = newNode;
		return;
	}

	BSTNode* parents = nullptr;
	BSTNode* node = _root;

	while (node)
	{
		parents = node;

		if (node->key == key)
			break;
		else if (node->key < key)
			node = node->right;
		else
			node = node->left;
	}

	newNode->parents = parents;
	if (parents->key < key)
		parents->right = newNode;
	else
		parents->left = newNode;
}

void BinarySearchTree::Delete(int key)
{

}
