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
	
	BSTNode* Min(BSTNode* node);
	BSTNode* Max(BSTNode* node);
	BSTNode* Next(BSTNode* node);
	BSTNode* Search(int key);

	void Insert(int key);
	void Delete(int key);
	void Delete(BSTNode* node);

	void Replace(BSTNode* b, BSTNode* a); // before, after

private:
	BSTNode* _root = nullptr;

};

