#include "pch.h"
#include "Player.h"
#include "Board.h"

void Player::Init(Board* board)
{
	_board = board;
	_pos = _board->GetEnterPos();

	Pos pos = _pos;

	_path.clear();
	_path.push_back(pos);

	// �������� �����ϱ� ������ ��� ����
	Pos dest = board->GetExitPos();

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
}

void Player::Update(uint64 deltaTick)
{
	if (_pathIndex >= _path.size())
		return;

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
