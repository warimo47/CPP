#pragma once

struct BSTNode
{
	BSTNode*	parents = nullptr;
	BSTNode*	left = nullptr;
	BSTNode*	right = nullptr;
	int			key = {};
};

class BinarySearchTree
{
public:
	void PrintTree();
	void PrintNode(int step, BSTNode* node);

	void Insert(int key);
	void Delete(int key);

private:
	BSTNode* _root = nullptr;

};

