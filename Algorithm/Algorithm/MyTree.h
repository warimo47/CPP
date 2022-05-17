#pragma once

#include <iostream>
#include <string>
#include <vector>

using NodeRef = std::shared_ptr<struct TNode>;

struct TNode
{
    TNode() {}
    TNode(const std::string& data) : data(data) {}
    std::string            data;
    std::vector<NodeRef>   children;
};

NodeRef CreateTree()
{
    NodeRef root = make_shared<TNode>("R1 개발실");
    {
        NodeRef node = make_shared<TNode>("디자인팀");
        root->children.push_back(node);
        {
            NodeRef leaf = make_shared<TNode>("전투");
            node->children.push_back(leaf);
        }
        {
            NodeRef leaf = make_shared<TNode>("경제");
            node->children.push_back(leaf);
        }
        {
            NodeRef leaf = make_shared<TNode>("스토리");
            node->children.push_back(leaf);
        }
    }
    {
        NodeRef node = make_shared<TNode>("프로그래밍팀");
        root->children.push_back(node);
        {
            NodeRef leaf = make_shared<TNode>("서버");
            node->children.push_back(leaf);
        }
        {
            NodeRef leaf = make_shared<TNode>("클라");
            node->children.push_back(leaf);
        }
        {
            NodeRef leaf = make_shared<TNode>("엔진");
            node->children.push_back(leaf);
        }
    }
    {
        NodeRef node = make_shared<TNode>("아트팀");
        root->children.push_back(node);
        {
            NodeRef leaf = make_shared<TNode>("배경");
            node->children.push_back(leaf);
        }
        {
            NodeRef leaf = make_shared<TNode>("캐릭터");
            node->children.push_back(leaf);
        }
    }

    return root;
}

void PrintTree(NodeRef root, int depth)
{
    for (int i = 0; i < depth; ++i)
        std::cout << "  - ";
    std::cout << root->data << std::endl;

    for (NodeRef& child : root->children)
        PrintTree(child, depth + 1);
}

// 깊이(depth) : 루트에서 어떤 노드에 도달하기 위해 거처야 하는 간선의 수 (aka. 몇 층?)
// 높이(height) : 가장 깊숙히 있는 노드의 깊이 (max(depth))
int GetHeight(NodeRef root)
{
    int height = 1;

    for (NodeRef& child : root->children)
        height = std::max(height, GetHeight(child) + 1);

    return height;
}