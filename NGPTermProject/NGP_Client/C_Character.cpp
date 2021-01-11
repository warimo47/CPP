#include "C_Character.h"

C_Character::C_Character()
{
	PositionX = 0.0f;
	PositionY = 0.0f;
	Life = (short)46;
	AnimationState = (BYTE)0;
	AnimationCount = (BYTE)0;
}

C_Character::C_Character(int type)
{
	PositionX = 0.0f;
	PositionY = 0.0f;
	Life = (short)46;
	Type = type;
	AnimationState = (BYTE)0;
	AnimationCount = (BYTE)0;
}


C_Character::~C_Character()
{
}

float C_Character::GetPosX() { return PositionX; }

float C_Character::GetPosY() { return PositionY; }

short C_Character::GetLife() { return Life; }

int C_Character::GetType() { return Type; }

BYTE C_Character::GetAnimState() { return AnimationState; }

BYTE C_Character::GetAnimCount() { return AnimationCount; }

void C_Character::SetC_Character(PACKET_Character* charpacket)
{
	PositionX = charpacket->Position.X;
	PositionY = charpacket->Position.Y;
	Life = charpacket->Life;
	AnimationState = charpacket->AnimationState;
	AnimationCount = charpacket->AnimationCount;
	Type = charpacket->Type;
}