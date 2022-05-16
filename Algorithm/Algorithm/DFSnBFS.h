#pragma once

#include <iostream>
#include <vector>
#include <queue>

using namespace std;

// DFS (Depth First Search) 깊이 우선 탐색
// BFS (Breadth First Search) 너비 우선 탐색

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

    /* 인접 리스트 version
    adjacent[0].push_back(1);
    adjacent[0].push_back(3);
    adjacent[1].push_back(0);
    adjacent[1].push_back(2);
    adjacent[1].push_back(3);
    adjacent[3].push_back(4);
    adjacent[5].push_back(4);*/

    // 인접 행렬 version
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
    // 방문!
    (*visited)[here] = true;
    cout << "Visited : " << here << endl;

    /* 인접 리스트 version
    모든 인접 정점을 순회한다
    for (int i = 0; i < adjacent[here].size(); ++i)
    {
        int there = adjacent[here][i];

        if (visited[there] == false)
            Dfs(there);
    }*/

    // 인접 행렬 version
    for (int there = 0; there < 6; ++there)
    {
        if ((*adjacent)[here][there] == 0)
            continue;

        // 아직 방문하지 않은 곳이 있으면 방문한다
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
    // 누구에 의해 발견 되었는지?
    vector<int> parent(6, -1);
    // 시작점에서 얼만큼 떨어져 있는지?
    vector<int> distance(6, -1);

    queue<int> q;
    q.push(here);
    discovered[here] = true;
    parent[here] = here;
    distance[here] = 0;

    /* 인접 리스트 version
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

    // 인접 행렬 version
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
