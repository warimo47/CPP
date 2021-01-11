#pragma once

#include <Windows.h>
#include <iostream>

using namespace std;

enum ObjectsType { BOMBER, GUNNER, MAGICIAN, DIE, BOOM, BULLET, FIRE, NONE };

struct vector2
{
	short X;
	short Y;
};

// [ 발사체 정보 패킷 ]
struct PACKET_Bullet
{
	vector2 Position;
	BYTE Type;
	BYTE AnimationCount;
};

class C_Bullet
{
	float PositionX;
	float PositionY;
	int Type;
	int AnimationCount;

public:
	C_Bullet();
	~C_Bullet();

	int GetType() const;
	float GetPosX() const;
	float GetPosY() const;
	int GetAnimCount() const;

	void SetTypeNone();

	void SetC_Bullet(PACKET_Bullet*);
};

