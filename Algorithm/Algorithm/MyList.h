#pragma once

template<typename T>
class LNode
{
public:
    LNode() : _prev(nullptr), _next(nullptr), _data(T())
    {

    }

    LNode(const T& value) : _prev(nullptr), _next(nullptr), _data(value)
    {

    }

public:
    LNode* _prev;
    LNode* _next;
    T       _data;
};

template<typename T>
class Iterator
{
public:
    Iterator() : _node(nullptr)
    {

    }

    Iterator(LNode<T>* node) : _node(node)
    {

    }

    // ++it
    Iterator& operator++()
    {
        _node = _node->_next;
        return *this;
    }

    // it++
    Iterator operator++(int)
    {
        Iterator<T> temp = *this;
        _node = _node->_next;
        return temp;
    }

    // --it
    Iterator& operator--()
    {
        _node = _node->_prev;
        return *this;
    }

    // it--
    Iterator operator--(int)
    {
        Iterator<T> temp = *this;
        _node = _node->_prev;
        return temp;
    }

    // *it
    T& operator*()
    {
        return _node->_data;
    }

    bool operator==(const Iterator& other)
    {
        return _node == other._node;
    }

    bool operator!=(const Iterator& other)
    {
        return _node != other._node;
    }

public:
    LNode<T>* _node;
};

template<typename T>
class List
{
public:
    List() : _size(0)
    {
        _head = new LNode<T>();
        _tail = new LNode<T>();
        _head->_next = _tail;
        _tail->_prev = _head;
    }

    ~List()
    {
        while (_size > 0)
            pop_back();

        delete _head;
        delete _tail;
    }

    void push_back(const T& value)
    {
        AddNode(_tail, value);
    }

    void pop_back()
    {
        RemoveNode(_tail->_prev);
    }

private:
    LNode<T>* AddNode(LNode<T>* before, const T& value)
    {
        LNode<T>* newNode = new LNode<T>(value);
        LNode<T>* preNode = before->_prev;

        preNode->_next = newNode;
        newNode->_prev = preNode;

        newNode->_next = before;
        before->_prev = newNode;

        _size++;

        return newNode;
    }

    LNode<T>* RemoveNode(LNode<T>* node)
    {
        LNode<T>* preNode = node->_prev;
        LNode<T>* nextNode = node->_next;

        preNode->_next = nextNode;
        nextNode->_prev = preNode;

        delete node;

        _size--;

        return nextNode;
    }

public:
    using iterator = Iterator<T>;

    iterator begin() { return iterator(_head->_next); }
    iterator end() { return iterator(_tail); }

    // it '앞에' 추가
    iterator insert(iterator it, const T& value)
    {
        LNode<T>* node = AddNode(it._node, value);
        return iterator(node);
    }

    iterator erase(iterator it)
    {
        LNode<T>* node = RemoveNode(it._node);
        return iterator(node);
    }

private:
    LNode<T>* _head;
    LNode<T>* _tail;
    int         _size;
};
