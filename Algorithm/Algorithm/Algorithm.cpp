#include <iostream>
#include <vector>
#include <list>
#include <stack>

#include "myVector.h"
#include "MyList.h"

using namespace std;

void MyVectorCheck();
void MyListCheck();

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

int main()
{
    Stack<int, list<int>> s;

    // 삽입
    s.push(1);
    s.push(2);
    s.push(3);

    // 비었나?
    while (s.empty() == false)
    {
        // 최상위 원소
        int data = s.top();

        // 최상위 원소 삭제
        s.pop();

        cout << data << endl;
    }

    int size = s.size();
}

void MyVectorCheck()
{
    // vector
    // - push_back O(1)
    // - push_front O(N)

    Vector<int> v;

    v.reserve(100);
    cout << v.size() << " " << v.capacity() << endl;

    for (int i = 0; i < 101; ++i)
    {
        v.push_back(i);
        cout << v[i] << " " << v.size() << " " << v.capacity() << endl;
    }

    v.resize(10);
    cout << v.size() << " " << v.capacity() << endl;

    v.clear();
    cout << v.size() << " " << v.capacity() << endl;
}

void MyListCheck()
{
    List<int> li;

    List<int>::iterator eraselt;

    for (int i = 0; i < 10; ++i)
    {
        if (i == 5)
        {
            eraselt = li.insert(li.end(), i);
        }
        else
        {
            li.push_back(i);
        }
    }

    li.pop_back();

    li.erase(eraselt);

    for (auto it = li.begin(); it != li.end(); it++)
    {
        cout << (*it) << endl;
    }
}