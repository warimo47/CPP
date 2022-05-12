#pragma once

#include <vector>

using namespace std;

template<typename T, typename Container = vector<T>>
class Stack
{
public:
    void push(const T& value)
    {
        _container.push_back(value);
    }

    /* 동작 성능과 예외 처리 관련 이슈가 있어 이렇게 설계하지 않음.
    T pop()
    {
        T ret = _data[_size - 1];
        _size--;
        return ret; // T(const T&)
    }
    */

    void pop()
    {
        _container.pop_back();
    }

    T& top()
    {
        return _container.back();
    }

    bool empty() { return _container.empty(); }
    int size() { return _container.size(); }

private:
    // vector<T> _container;
    Container _container;
};
