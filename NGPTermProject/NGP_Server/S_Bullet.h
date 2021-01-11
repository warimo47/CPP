#pragma once

#include <Windows.h>
#include <iostream>

using namespace std;

#define WIN_WIDTH 1200.0f
#define WIN_HEIGHT 990.0f
#define CHAR_WIDTH 48.0f
#define CHAR_HEIGHT 66.0f
#define BULLET_SIZE 24.0f

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

class S_Bullet
{
	float PositionX;
	float PositionY;
	int Type;
	int AnimTickCount;
	int AnimationCount;
	
	int OwnerID;
	float DirectionX;
	float DirectionY;
	float Speed;
	int Damage;

public:
	S_Bullet();
	~S_Bullet();

	void ReSet();

	float GetPosX() const;
	float GetPosY() const;
	float GetRight() const;
	float GetBottom() const;

	int GetDamage() const;

	int GetType() const;
	void SetType(int);

	int GetAnimationCount() const;
	void BulletAnim();

	int GetOwnerID() const;

	void PositionUpdate(float);

	// 발사체의 상태를 바꿈 (플레이어ID, 플레이어Type, PosX, PoxY, DirX, DirY) 
	void SetBulletState(int, int, float, float, float, float);

	// 발사체의 상태를 [발사체 정보 패킷]에 복사
	void SetBulletPacket(PACKET_Bullet*);
};

