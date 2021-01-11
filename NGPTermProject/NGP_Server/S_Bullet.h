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

// [ �߻�ü ���� ��Ŷ ]
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

	// �߻�ü�� ���¸� �ٲ� (�÷��̾�ID, �÷��̾�Type, PosX, PoxY, DirX, DirY) 
	void SetBulletState(int, int, float, float, float, float);

	// �߻�ü�� ���¸� [�߻�ü ���� ��Ŷ]�� ����
	void SetBulletPacket(PACKET_Bullet*);
};

