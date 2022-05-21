#pragma once

struct BSTNode
{
	BSTNode*	parants = nullptr;
	BSTNode*	left = nullptr;
	BSTNode*	right = nullptr;
	int			value = 0;
};

class BinarySearchTree
{
public:
	void PrintAll();
	void PrintNode(int step, BSTNode* node);

	void Insert(int value);
	void Delete(int value);

private:
	BSTNode* root = nullptr;

};

