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

BSTNode* BinarySearchTree::Min(BSTNode* node)
{
	while (node->left)
		node = node->left;

	return node;
}

BSTNode* BinarySearchTree::Max(BSTNode* node)
{
	while (node->right)
		node = node->right;

	return node;
}

BSTNode* BinarySearchTree::Next(BSTNode* node)
{
	if (node->right)
		return Min(node->right);

	BSTNode* parents = node->parents;

	while (parents && node == parents->right)
		parents = node->parents;

	return parents;
}

BSTNode* BinarySearchTree::Search(int key)
{
	BSTNode* node = _root;

	while (node)
	{
		if (node->key == key)
			break;
		else if (node->key < key)
			node = node->right;
		else
			node = node->left;
	}

	return node;
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
	BSTNode* node = Search(key);
	if (node)
		Delete(node);
	else
		std::cout << "없는 노드입니다." << std::endl;
}

void BinarySearchTree::Delete(BSTNode* node)
{
	if (node->left == nullptr)
		Replace(node, node->right);
	else if (node->right == nullptr)
		Replace(node, node->left);
	else
	{
		BSTNode* next = Next(node);

		node->key = next->key;

		Delete(next);
	}
}

void BinarySearchTree::Replace(BSTNode* b, BSTNode* a)
{
	if (b == nullptr)
	{
		std::cout << "대체하려는 대상 노드가 nullptr입니다." << std::endl;
		return;
	}

	if (b == b->parents->right)
		b->parents->right = a;
	else
		b->parents->left = a;

	if (a)
		a->parents = b->parents;

	delete b;
}
