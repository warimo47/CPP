#pragma once
#include <vector>
#include "S_Obstacle.h"

enum DirectionState { DOWN_LEFT, LEFT, UP_LEFT, DOWN, UP, DOWN_RIGHT, RIGHT, UP_RIGHT };

// [캐릭터 정보 패킷]
struct PACKET_Character
{
	vector2 Position;
	short Life;
	BYTE Type;
	BYTE AnimationState;
	BYTE AnimationCount;
};

// [플레이어 상태 패킷]
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

	// [플레이어 상태 패킷]이 캐릭터의 상태를 바꿈
	void SetCharacterState(PACKET_PlayerState*);

	// 캐릭터의 상태를 [캐릭터 정보 패킷]에 복사
	void SetCharacterPacket(PACKET_Character*);

	void PositionUpdate(float);
	void BounceCollision(float);

	void PrintInfo() const;
};

