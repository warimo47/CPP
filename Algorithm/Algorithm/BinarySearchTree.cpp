#include "BinarySearchTree.h"

#include <iostream>

void BinarySearchTree::PrintAll()
{
	PrintNode(0, root);
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
	std::cout << node->value << std::endl;

	PrintNode(step + 1, node->right);
}

void BinarySearchTree::Insert(int value)
{
	BSTNode* newNode = new BSTNode();
	newNode->value = value;

	if (root == nullptr)
	{
		newNode->parants = newNode;
		root = newNode;
		return;
	}

	BSTNode* parants = root;
	BSTNode* node = root;

	while (node)
	{
		parants = node;

		if (node->value == value)
			break;
		else if (node->value < value)
			node = node->right;
		else
			node = node->left;
	}

	newNode->parants = parants;
	if (parants->value < value)
		parants->right = newNode;
	else
		parants->left = newNode;
}

void BinarySearchTree::Delete(int value)
{

}
