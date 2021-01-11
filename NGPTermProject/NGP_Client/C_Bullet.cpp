#include "C_Bullet.h"

C_Bullet::C_Bullet()
{
	PositionX = 0;
	PositionY = 0;
	Type = ObjectsType::NONE;
	AnimationCount = 0;
}


C_Bullet::~C_Bullet()
{
}

int C_Bullet::GetType() const
{
	return Type;
}

float C_Bullet::GetPosX() const
{
	return PositionX;
}

float C_Bullet::GetPosY() const
{
	return PositionY;
}

int C_Bullet::GetAnimCount() const
{
	return AnimationCount;
}

void C_Bullet::SetTypeNone()
{
	Type = ObjectsType::NONE;
}

void C_Bullet::SetC_Bullet(PACKET_Bullet* packetBullet)
{
	PositionX = packetBullet->Position.X;
	PositionY = packetBullet->Position.Y;
	Type = packetBullet->Type;
	AnimationCount = packetBullet->AnimationCount;
}