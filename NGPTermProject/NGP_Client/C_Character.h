#pragma once

#include "C_Bullet.h"

// [ 캐릭터 정보 패킷 ]
struct PACKET_Character
{
	vector2 Position;
	short Life;
	BYTE Type;
	BYTE AnimationState;
	BYTE AnimationCount;
};

class C_Character
{
	float PositionX;
	float PositionY;
	short Life;
	int Type;
	BYTE AnimationState;
	BYTE AnimationCount;
	
public:
	C_Character();
	C_Character(int);
	~C_Character();

	float GetPosX();
	float GetPosY();
	short GetLife();
	int GetType();
	BYTE GetAnimState();
	BYTE GetAnimCount();

	void SetC_Character(PACKET_Character*);
};

