#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include <queue>

#include "myVector.h"
#include "MyList.h"
#include "MyStack.h"
#include "MyQueue.h"
#include "DFSnBFS.h"
#include "Dijikstra.h"
#include "MyTree.h"
#include "MyPriorityQueue.h"

using namespace std;

void MyVectorCheck();
void MyListCheck();
void MyStackCheck();
void MyQueueCheck();
void CreateGraph_1();
void CreateGraph_2();
void CreateGraph_3();
void DfsCheck();
void BfsCheck();
void DijikstraCheck();
void TreeCheck();
void PriorityQueueCheck();

vector<int> numbers;

void BinarySearch(int N)
{
    int left = 0;
    int right = (int)numbers.size() - 1;

    while (left <= right)
    {
        cout << "탐색 범위: " << left << " - " << right << endl;

        int mid = (left + right) / 2;

        if (N < numbers[mid])
        {
            cout << N << " < " << numbers[mid] << endl;
            right = mid - 1;
        }
        else if (N > numbers[mid])
        {
            cout << N << " > " << numbers[mid] << endl;
            left = mid + 1;
        }
        else
        {
            cout << "찾음!" << endl;
            break;
        }
    }
}

int main()
{
    numbers = vector<int>{ 1, 8, 15, 23, 44, 56, 63, 81, 91 };
    BinarySearch(82);
    
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

void MyStackCheck()
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

void MyQueueCheck()
{
    ArrayQueue<int> q;

    for (int i = 0; i < 100; ++i)
        q.push(i);

    while (q.empty() == false)
    {
        int value = q.front();
        q.pop();
        cout << value << endl;
    }

    int size = q.size();
}

void CreateGraph_1()
{
    struct Vertex
    {
        vector<Vertex*> edges;
        // int data;
    };

    vector<Vertex> v;
    v.resize(6);

    v[0].edges.push_back(&v[1]);
    v[0].edges.push_back(&v[3]);
    v[1].edges.push_back(&v[0]);
    v[1].edges.push_back(&v[2]);
    v[1].edges.push_back(&v[3]);
    v[3].edges.push_back(&v[4]);
    v[5].edges.push_back(&v[4]);

    // Q) 0번 -> 3번 간선이 있나요?
    bool connected = false;
    for (Vertex* edge : v[0].edges)
    {
        if (edge == &v[3])
        {
            connected = true;
            break;
        }
    }
}

void CreateGraph_2()
{
    struct Vertex
    {
        // int data;
    };

    vector<Vertex> v;
    v.resize(6);

    // 연결된 목록을 따로 관리
    vector<vector<int>> adjacent(6);
    adjacent[0] = { 1, 3 };
    adjacent[1] = { 0, 2, 3 };
    adjacent[3] = { 4 };
    adjacent[5] = { 4 };

    // 정점이 100개
    // - 지하철 노선도 -> 서로 드문 드문 연결 (양옆, 환승역이라면 ++)
    // - 페이스북 친구 -> 서로 빽빽하게 연결

    // Q) 0번 -> 3번 간선이 있나요?
    bool connected = false;
    for (int vertex : adjacent[0])
    {
        if (vertex == 3)
        {
            connected = true;
            break;
        }
    }

    // STL
    vector<int>& adj = adjacent[0];
    bool connected2 = (std::find(adj.begin(), adj.end(), 3) != adj.end());
}

void CreateGraph_3()
{
    struct Vertex
    {
        // int data;
    };

    vector<Vertex> v;
    v.resize(6);

    // 연결된 목록을 따로 관리
    //    0 1 2 3 4 5
    //  0 X O X O X X
    //  1 O X O O X X
    //  2 X X X X X X
    //  3 X X X X O X
    //  4 X X X X X X
    //  5 X X X X O X

    // 읽는 방법 adjacent[from][to]
    // 행렬을 이용한 그래프 표현 (2차원 배열)
    // 메모리 소모가 심하지만, 빠른 접근이 가능하다
    // (간선이 많은 경우 이점이 있다)
    vector<vector<bool>> adjacent(6, vector<bool>(6, false));
    adjacent[0][1] = true;
    adjacent[0][3] = true;
    adjacent[1][0] = true;
    adjacent[1][2] = true;
    adjacent[1][3] = true;
    adjacent[3][4] = true;
    adjacent[5][4] = true;

    // Q) 0번 -> 3번 간선이 있나요?
    bool connected = adjacent[0][3];

    vector<vector<int>> adjacent2 =
    {
        vector<int> { -1, 15, -1, 35, -1, -1 },
        vector<int> { -5, -1, +5, 10, -1, -1 },
        vector<int> { -1, -1, -1, -1, -1, -1 },
        vector<int> { -1, -1, -1, -1, +5, -1 },
        vector<int> { -1, -1, -1, -1, -1, -1 },
        vector<int> { -1, -1, -1, -1, +5, -1 }
    };

    adjacent2[0][3];
}

void DfsCheck()
{
    vector<vector<int>> adjacent;
    CreateGraph(&adjacent);

    discovered = vector<bool>(6, false);

    DfsAll(&adjacent);
}

void BfsCheck()
{
    CreateGraph(&adjacent);

    discovered = vector<bool>(6, false);

    BfsAll(&adjacent);
}

void DijikstraCheck()
{
    CreateWeightsGraph();

    Dijikstra(0);
}

void TreeCheck()
{
    NodeRef root = CreateTree();

    PrintTree(root, 0);

    cout << "\nHeight : " << GetHeight(root) << endl;
}

void PriorityQueueCheck()
{
    PriorityQueue<int, vector<int>, std::greater<int>> pq;

    pq.push(100);
    pq.push(300);
    pq.push(200);
    pq.push(500);
    pq.push(400);

    while (pq.empty() == false)
    {
        int value = pq.top();
        pq.pop();

        cout << value << endl;
    }
}