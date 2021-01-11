#pragma once
#include <vector>
#include "S_Obstacle.h"

enum DirectionState { DOWN_LEFT, LEFT, UP_LEFT, DOWN, UP, DOWN_RIGHT, RIGHT, UP_RIGHT };

// [ĳ���� ���� ��Ŷ]
struct PACKET_Character
{
	vector2 Position;
	short Life;
	BYTE Type;
	BYTE AnimationState;
	BYTE AnimationCount;
};

// [�÷��̾� ���� ��Ŷ]
struct PACKET_PlayerState
{
	BYTE DirectionState;
	BYTE IsMoving;
	float AttackDirectionX;
	float AttackDirectionY;
};

class S_Character
{
	float StartPosX;
	float StartPosY;
	float PositionX;
	float PositionY;

	int MaxLife;
	int Life;
	int Type;

	BYTE AnimationState;
	int AnimTickCount;
	BYTE AnimationCount;
	
	float DirectionX;
	float DirectionY;
	float Speed;
	float MaxSpeed;

public:
	S_Character();
	S_Character(float, float);
	~S_Character();

	void ReSet();

	float GetPosX() const;
	float GetPosY() const;
	float GetRight() const;
	float GetBottom() const;
	
	int GetLife() const;
	void SetLife(int);

	int GetType() const;
	void SetType(int);

	BYTE GetAnimState() const;
	bool CheckAnimTickCount();
	BYTE GetAnimCount() const;
	void SetAnimCount(int);

	// [�÷��̾� ���� ��Ŷ]�� ĳ������ ���¸� �ٲ�
	void SetCharacterState(PACKET_PlayerState*);

	// ĳ������ ���¸� [ĳ���� ���� ��Ŷ]�� ����
	void SetCharacterPacket(PACKET_Character*);

	void PositionUpdate(float);
	void BounceCollision(float);

	void PrintInfo() const;
};

