#include "S_Bullet.h"

S_Bullet::S_Bullet()
{
	ReSet();
}


S_Bullet::~S_Bullet()
{
}

void S_Bullet::ReSet()
{
	Type = ObjectsType::NONE;

	AnimTickCount = 0;;
}

float S_Bullet::GetPosX() const { return PositionX; }

float S_Bullet::GetPosY() const { return PositionY; }

float S_Bullet::GetRight() const { return PositionX + BULLET_SIZE; }

float S_Bullet::GetBottom() const { return PositionY + BULLET_SIZE; }

int S_Bullet::GetDamage() const { return Damage; }

int S_Bullet::GetType() const { return Type; }

void S_Bullet::SetType(int type) { Type = type; }

int S_Bullet::GetAnimationCount() const { return AnimationCount; }

void S_Bullet::BulletAnim()
{
	++AnimTickCount;

	if (AnimTickCount > 2)
	{
		AnimTickCount = 0;

		++AnimationCount;

		if (AnimationCount > 3) AnimationCount = 0;
	}
}

int S_Bullet::GetOwnerID() const { return OwnerID; }

void S_Bullet::PositionUpdate(float ticktime)
{
	PositionX = PositionX + Speed * DirectionX * ticktime;
	PositionY = PositionY + Speed * DirectionY * ticktime;

	if (PositionX < (0.0f - BULLET_SIZE)) Type = ObjectsType::NONE;
	else if (PositionX > WIN_WIDTH) Type = ObjectsType::NONE;
	if (PositionY < (0.0f - BULLET_SIZE)) Type = ObjectsType::NONE;
	else if (PositionY > WIN_HEIGHT) Type = ObjectsType::NONE;
}

// 발사체의 상태를 바꿈 (플레이어ID, PosX, PoxY, DirX, DirY) 
void S_Bullet::SetBulletState(int playerID, int playerType, float posX, float posY, float dirX, float dirY)
{
	PositionX = posX;
	PositionY = posY;
	AnimationCount = 0;
	OwnerID = playerID;
	DirectionX = dirX;
	DirectionY = dirY;

	switch (playerType)
	{
	case ObjectsType::BOMBER:
		Type = ObjectsType::BOOM;
		Speed = 300.0f;
		Damage = 6;
		break;
	case ObjectsType::GUNNER:
		Type = ObjectsType::BULLET;
		Speed = 300.0f;
		Damage = 4;
		break;
	case ObjectsType::MAGICIAN:
		Type = ObjectsType::FIRE;
		Speed = 300.0f;
		Damage = 2;
		break;
	default:
		printf("플레이어 Type 에러\n");
		break;
	}
	
}

// 발사체의 상태를 [발사체 정보 패킷]에 복사
void S_Bullet::SetBulletPacket(PACKET_Bullet* bulletpacket)
{
	bulletpacket->Position.X = (short)PositionX;
	bulletpacket->Position.Y = (short)PositionY;
	bulletpacket->Type = Type;
	bulletpacket->AnimationCount = AnimationCount;
}