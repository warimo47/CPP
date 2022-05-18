#include "pch.h"
#include "Player.h"
#include "Board.h"

void Player::Init(Board* board)
{
	_board = board;
	_pos = _board->GetEnterPos();

	// RightHand();
	// Bfs();
	AStar();
}

void Player::Update(uint64 deltaTick)
{
	if (_pathIndex >= _path.size())
	{
		_board->GenerateMap();
		Init(_board);
		return;
	}

	_sumTick += deltaTick;

	if (_sumTick >= MOVE_TICK)
	{
		_sumTick = 0;

		_pos = _path[_pathIndex];
		_pathIndex++;
	}
}

bool Player::CanGo(Pos pos)
{
	TileType tileType = _board->GetTileType(pos);
	return tileType == TileType::EMPTY;
}

void Player::RightHand()
{
	Pos pos = _pos;

	_path.clear();
	_path.push_back(pos);

	// �������� �����ϱ� ������ ��� ����
	Pos dest = _board->GetExitPos();

	Pos front[4] =
	{
		Pos{ -1, 0 }, // UP
		Pos{ 0, -1 }, // LEFT
		Pos{ 1, 0 }, // DOWN
		Pos{ 0, 1 }, // RIGHT
	};

	while (pos != dest)
	{
		// 1. ���� �ٶ󺸴� ������ �������� ���������� �� �� �ִ��� Ȯ��.
		int32 newDir = (_dir - 1 + DIR_COUNT) % DIR_COUNT;
		if (CanGo(pos + front[newDir]))
		{
			// ������ �������� 90�� ȸ��.
			_dir = newDir;

			// ������ �� �� ����.
			pos += front[_dir];

			_path.push_back(pos);
		}
		// 2. ���� �ٶ󺸴� ������ �������� ���� �� �� �ִ��� Ȯ��.
		else if (CanGo(pos + front[_dir]))
		{
			// ������ �� �� ����.
			pos += front[_dir];

			_path.push_back(pos);
		}
		else
		{
			// ���� �������� ȸ��.
			_dir = (_dir + 1) % DIR_COUNT;
		}
	}

	stack<Pos> s;

	for (int i = 0; i < _path.size() - 1; ++i)
	{
		if (s.empty() == false && s.top() == _path[i + 1])
			s.pop();
		else
			s.push(_path[i]);
	}

	// ������ ����
	if (_path.empty() == false)
		s.push(_path.back());

	vector<Pos> path;
	while (s.empty() == false)
	{
		path.push_back(s.top());
		s.pop();
	}

	std::reverse(path.begin(), path.end());

	_path = path;
}

void Player::Bfs()
{
	Pos pos = _pos;

	// �������� �����ϱ� ������ ��� ����
	Pos dest = _board->GetExitPos();

	Pos front[4] =
	{
		Pos{ -1, 0 }, // UP
		Pos{ 0, -1 }, // LEFT
		Pos{ 1, 0 }, // DOWN
		Pos{ 0, 1 }, // RIGHT
	};

	const int32 size = _board->GetSize();
	vector<vector<bool>> discovered(size, vector<bool>(size, false));

	// vector<vector<Pos>> parent;
	// parent[A] = B; A�� B�� ���� �߰���.
	map<Pos, Pos> parent;

	queue<Pos> q;
	q.push(pos);
	discovered[pos.y][pos.x] = true;
	parent[pos] = pos;

	while (q.empty() == false)
	{
		pos = q.front();
		q.pop();

		// �湮!
		if (pos == dest)
			break;

		for (int32 dir = 0; dir < 4; ++dir)
		{
			Pos nextPos = pos + front[dir];
			// �� �� �ִ� ������ �´��� Ȯ��.

			if (CanGo(nextPos) == false)
				continue;

			// �̹� �߰��� �������� Ȯ��.
			if (discovered[nextPos.y][nextPos.x])
				continue;

			q.push(nextPos);
			discovered[nextPos.y][nextPos.x] = true;
			parent[nextPos] = pos;
		}
	}

	_path.clear();

	// �Ųٷ� �Ž��� �ö󰣴�
	pos = dest;

	while (true)
	{
		_path.push_back(pos);

		// �������� �ڽ��� �� �θ��̴�
		if (pos == parent[pos])
			break;

		pos = parent[pos];
	}

	reverse(_path.begin(), _path.end());
}

struct PQNode
{
	bool operator<(const PQNode& other) const { return f < other.f; }
	bool operator>(const PQNode& other) const { return f > other.f; }

	int32	f; // f = g + h
	int32	g;
	Pos		pos;
};

void Player::AStar()
{
	// ���� �ű��
	// F = G + H
	// F = ���� ���� (���� ���� ����, ��ο� ���� �޶���)
	// G = ���������� �ش� ��ǥ���� �̵��ϴµ� ��� ��� (���� ���� ����, ��ο� ���� �޶���)
	// H = ���������� �󸶳� ������� (���� ���� ����, ����) (heuristic)

	Pos start = _pos;
	Pos dest = _board->GetExitPos();

	enum
	{
		DIR_COUNT = 8
	};

	Pos front[] =
	{
		Pos{-1,  0 },	// UP
		Pos{ 0, -1 },	// LEFT
		Pos{ 1,  0 },	// DOWN
		Pos{ 0,  1 },	// RIGHT
		Pos{-1, -1},
		Pos{ 1, -1},
		Pos{ 1,  1},
		Pos{-1,  1}
	};

	int32 cost[] =
	{
		10, // UP
		10, // LEFT
		10, // DOWN
		10, // RIGHT
		14,
		14,
		14,
		14
	};

	const int32 size = _board->GetSize();

	// CloseList
	// close[y][x] -> (y, x)�� �湮�� �ߴ��� ����
	vector<vector<bool>> closed(size, vector<bool>(size, false));

	// best[y][x] -> ���ݱ��� (y, x)�� ���� ���� ���� ��� (���� ���� ����)
	vector<vector<int32>> best(size, vector<int32>(size, INT32_MAX));

	// �θ� ���� �뵵
	map<Pos, Pos> parent;

	// OpenList
	priority_queue<PQNode, vector<PQNode>, greater<PQNode>> pq;

	// 1) ����(�߰�) �ý��� ����
	// - �̹� �� ���� ��θ� ã�Ҵٸ� ��ŵ
	// 2) �ڴʰ� �� ���� ��ΰ� �߰ߵ� �� ���� -> ���� ó�� �ʼ�
	// - openList���� ã�Ƽ� �����Ѵٰų�,
	// - pq���� pop�� ������ �����Ѵٰų�.

	// �ʱⰪ
	{
		int32 g = 0;
		int32 h = 10 * (abs(dest.y - start.y) + abs(dest.x - start.x));
		pq.push(PQNode{ g + h, g, start });
		best[start.y][start.x] = g + h;
		parent[start] = start;
	}

	while (pq.empty() == false)
	{
		// ���� ���� �ĺ��� ã�´�
		PQNode node = pq.top();
		pq.pop();

		// ������ ��ǥ�� ���� ��η� ã�Ƽ�,
		// �� ���� ��η� ���ؼ� �̹� �湮(closed)�� ��� ��ŵ
		
		// [����]
		if (closed[node.pos.y][node.pos.x])
			continue;
		//if (best[node.pos.y][node.pos.x] < node.f)
		//	continue;

		// �湮
		closed[node.pos.y][node.pos.x] = true;

		// �������� ���������� �ٷ� ����
		if (node.pos == dest)
			break;

		for (int32 dir = 0; dir < DIR_COUNT; ++dir)
		{
			Pos nextPos = node.pos + front[dir];

			// �� �� �ִ� ������ �´��� Ȯ��
			if (CanGo(nextPos) == false)
				continue;

			// [����] �̹� �湮�� ���̸� ��ŵ
			if (closed[nextPos.y][nextPos.x])
				continue;

			// ��� ���
			int32 g = node.g + cost[dir];
			int32 h = 10 * (abs(dest.y - nextPos.y) + abs(dest.x - nextPos.x));

			// �ٸ� ��ο��� �� ���� ���� ã������ ��ŵ
			if (best[nextPos.y][nextPos.x] <= g + h)
				continue;

			// ���� ����
			best[nextPos.y][nextPos.x] = g + h;
			pq.push(PQNode{ g + h, g, nextPos });
			parent[nextPos] = node.pos;
		}
	}
	
	// �Ųٷ� �Ž��� �ö󰣴�
	Pos pos = dest;

	_path.clear();
	_pathIndex = 0;

	while (true)
	{
		_path.push_back(pos);

		// �������� �ڽ��� �� �θ��̴�
		if (pos == parent[pos])
			break;

		pos = parent[pos];
	}

	reverse(_path.begin(), _path.end());
}
