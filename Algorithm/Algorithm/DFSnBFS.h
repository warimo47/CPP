#pragma once

#include <iostream>
#include <vector>
#include <queue>

using namespace std;

// DFS (Depth First Search) ���� �켱 Ž��
// BFS (Breadth First Search) �ʺ� �켱 Ž��

struct Vertex
{
    // int data;
};

vector<bool> discovered;

void CreateGraph(vector<vector<int>>* adjacent)
{
    vector<Vertex> vertices;
    vertices.resize(6);
    
    *adjacent = vector<vector<int>>(6);

    /* ���� ����Ʈ version
    adjacent[0].push_back(1);
    adjacent[0].push_back(3);
    adjacent[1].push_back(0);
    adjacent[1].push_back(2);
    adjacent[1].push_back(3);
    adjacent[3].push_back(4);
    adjacent[5].push_back(4);*/

    // ���� ��� version
    *adjacent = vector<vector<int>>
    {
        { 0, 1, 0, 1, 0, 0 },
        { 1, 0, 1, 1, 0, 0 },
        { 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 0 },
        { 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 0 }
    };
}

void Dfs(int here, vector<bool>* visited, vector<vector<int>>* adjacent)
{
    // �湮!
    (*visited)[here] = true;
    cout << "Visited : " << here << endl;

    /* ���� ����Ʈ version
    ��� ���� ������ ��ȸ�Ѵ�
    for (int i = 0; i < adjacent[here].size(); ++i)
    {
        int there = adjacent[here][i];

        if (visited[there] == false)
            Dfs(there);
    }*/

    // ���� ��� version
    for (int there = 0; there < 6; ++there)
    {
        if ((*adjacent)[here][there] == 0)
            continue;

        // ���� �湮���� ���� ���� ������ �湮�Ѵ�
        if ((*visited)[there] == false)
            Dfs(there, visited, adjacent);
    }
}

void DfsAll(vector<vector<int>>* adjacent)
{
    vector<bool> visited = vector<bool>(6, false);

    for (int i = 0; i < 6; ++i)
        if (visited[i] == false)
            Dfs(i, &visited, adjacent);
}

void Bfs(int here, vector<vector<int>>* adjacent)
{
    // ������ ���� �߰� �Ǿ�����?
    vector<int> parent(6, -1);
    // ���������� ��ŭ ������ �ִ���?
    vector<int> distance(6, -1);

    queue<int> q;
    q.push(here);
    discovered[here] = true;
    parent[here] = here;
    distance[here] = 0;

    /* ���� ����Ʈ version
    while (q.empty() == false)
    {
        here = q.front();
        q.pop();

        cout << "Visited : " << here << endl;

        for (int there : adjacent[here])
        {
            if (discovered[there])
                continue;

            q.push(there);
            discovered[there] = true;

            parent[there] = here;
            distance[there] = distance[here] + 1;
        }
    }*/

    // ���� ��� version
    while (q.empty() == false)
    {
        here = q.front();
        q.pop();

        cout << "Visited : " << here << endl;

        for (int there = 0; there < 6; ++there)
        {
            if ((*adjacent)[here][there] == 0)
                continue;

            if (discovered[there])
                continue;

            q.push(there);
            discovered[there] = true;

            parent[there] = here;
            distance[there] = distance[here] + 1;
        }
    }

    int a = 3;
}

void BfsAll(vector<vector<int>>* adjacent)
{
    for (int i = 0; i < 6; ++i)
    {
        if (discovered[i] == false)
            Bfs(i, adjacent);
    }
}
