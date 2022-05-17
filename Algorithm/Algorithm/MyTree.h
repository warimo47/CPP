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
    NodeRef root = make_shared<TNode>("R1 ���߽�");
    {
        NodeRef node = make_shared<TNode>("��������");
        root->children.push_back(node);
        {
            NodeRef leaf = make_shared<TNode>("����");
            node->children.push_back(leaf);
        }
        {
            NodeRef leaf = make_shared<TNode>("����");
            node->children.push_back(leaf);
        }
        {
            NodeRef leaf = make_shared<TNode>("���丮");
            node->children.push_back(leaf);
        }
    }
    {
        NodeRef node = make_shared<TNode>("���α׷�����");
        root->children.push_back(node);
        {
            NodeRef leaf = make_shared<TNode>("����");
            node->children.push_back(leaf);
        }
        {
            NodeRef leaf = make_shared<TNode>("Ŭ��");
            node->children.push_back(leaf);
        }
        {
            NodeRef leaf = make_shared<TNode>("����");
            node->children.push_back(leaf);
        }
    }
    {
        NodeRef node = make_shared<TNode>("��Ʈ��");
        root->children.push_back(node);
        {
            NodeRef leaf = make_shared<TNode>("���");
            node->children.push_back(leaf);
        }
        {
            NodeRef leaf = make_shared<TNode>("ĳ����");
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

// ����(depth) : ��Ʈ���� � ��忡 �����ϱ� ���� ��ó�� �ϴ� ������ �� (aka. �� ��?)
// ����(height) : ���� ����� �ִ� ����� ���� (max(depth))
int GetHeight(NodeRef root)
{
    int height = 1;

    for (NodeRef& child : root->children)
        height = std::max(height, GetHeight(child) + 1);

    return height;
}