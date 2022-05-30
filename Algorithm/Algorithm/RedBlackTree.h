#pragma once

enum class Color
{
	Red = 0,
	Black = 1,
};

struct RBNode
{
	RBNode*		parents = nullptr;
	RBNode*		left = nullptr;
	RBNode*		right = nullptr;
	int			key = {};
	Color		color = Color::Black;
};

class RedBlackTree
{
public:
	RedBlackTree();
	~RedBlackTree();

	void		PrintTree();
	void		PrintNode(int step, RBNode* node);

	RBNode*		Search(RBNode* node, int key);

	RBNode*		Min(RBNode* node);
	RBNode*		Max(RBNode* node);
	RBNode*		Next(RBNode* node);
	RBNode*		Search(int key);

	void		Insert(int key);
	void		Delete(int key);
	void		Delete(RBNode* node);

	void		Replace(RBNode* b, RBNode* a); // before, after

private:
	RBNode*		_root = nullptr;
	RBNode*		_nil = nullptr;
};
