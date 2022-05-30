#include "RedBlackTree.h"

#include <Windows.h>
#include <iostream>

using namespace std;

enum class ConsoleColor
{
	BLACK = 0,
	RED = FOREGROUND_RED,
	GREEN = FOREGROUND_GREEN,
	BLUE = FOREGROUND_BLUE,
	YELLOW = RED | GREEN,
	WHITE = RED | GREEN | BLUE
};

void SetCursorColor(ConsoleColor color)
{
	HANDLE output = ::GetStdHandle(STD_OUTPUT_HANDLE);
	::SetConsoleTextAttribute(output, static_cast<SHORT>(color));
}

RedBlackTree::RedBlackTree()
{
	_nil = new RBNode;
	_root = _nil;
}

RedBlackTree::~RedBlackTree()
{
	delete _nil;
}

void RedBlackTree::PrintTree()
{
	PrintNode(0, _root);
	SetCursorColor(ConsoleColor::WHITE);
}

void RedBlackTree::PrintNode(int step, RBNode* node)
{
	if (node == nullptr)
		return;

	PrintNode(step + 1, node->left);

	for (int i = 0; i < step; ++i)
	{
		std::cout.width(4);
		std::cout << " ";
	}

	if (node->color == Color::Black)
		SetCursorColor(ConsoleColor::BLUE);
	else
		SetCursorColor(ConsoleColor::RED);
	std::cout.width(4);
	std::cout << node->key << std::endl;

	PrintNode(step + 1, node->right);
}

RBNode* RedBlackTree::Search(RBNode* node, int key)
{
	if (node == _nil || key == node->key)
		return node;

	if (key < node->key)
		return Search(node->left, key);
	else
		return Search(node->right, key);
}

RBNode* RedBlackTree::Min(RBNode* node)
{
	while (node->left != _nil)
		node = node->left;

	return node;
}

RBNode* RedBlackTree::Max(RBNode* node)
{
	while (node->right != _nil)
		node = node->right;

	return node;
}

RBNode* RedBlackTree::Next(RBNode* node)
{
	if (node->right != _nil)
		return Min(node->right);

	RBNode* parents = node->parents;

	while (parents != _nil && node == parents->right)
		parents = node->parents;

	return parents;
}

RBNode* RedBlackTree::Search(int key)
{
	RBNode* node = _root;

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

void RedBlackTree::Insert(int key)
{
	RBNode* newNode = new RBNode();
	newNode->key = key;

	RBNode* node = _root;
	RBNode* parents = _nil;

	while (node != _nil)
	{
		parents = node;

		if (node->key == key)
			return;
		else if (node->key < key)
			node = node->right;
		else
			node = node->left;
	}

	newNode->parents = parents;

	if (parents == _nil)
		_root = newNode;
	else if (parents->key < key)
		parents->right = newNode;
	else
		parents->left = newNode;

	// 검사
	newNode->left = _nil;
	newNode->right = _nil;
	newNode->color = Color::Red;
}

void RedBlackTree::Delete(int key)
{
	RBNode* node = Search(key);
	if (node)
		Delete(node);
	else
		std::cout << "없는 노드입니다." << std::endl;
}

void RedBlackTree::Delete(RBNode* node)
{
	if (node->left == nullptr)
		Replace(node, node->right);
	else if (node->right == nullptr)
		Replace(node, node->left);
	else
	{
		RBNode* next = Next(node);

		node->key = next->key;

		Delete(next);
	}
}

void RedBlackTree::Replace(RBNode* b, RBNode* a)
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
