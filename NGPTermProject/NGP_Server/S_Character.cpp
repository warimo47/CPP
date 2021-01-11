#include "S_Character.h"

S_Character::S_Character()
{
	StartPosX = 0.0f;
	StartPosY = 0.0f;
	MaxLife = 46;
	Type = ObjectsType::DIE;

	MaxSpeed = 200.0f;

	ReSet();
}

S_Character::S_Character(float PosX, float PosY)
{
	StartPosX = PosX;
	StartPosY = PosY;
	MaxLife = 46;
	Type = ObjectsType::DIE;

	MaxSpeed = 200.0f;

	ReSet();
}


S_Character::~S_Character()
{
}

void S_Character::ReSet()
{
	PositionX = StartPosX;
	PositionY = StartPosY;
	Life = MaxLife;

	AnimationState = (BYTE)0;
	AnimTickCount = 0;
	AnimationCount = (BYTE)0;

	Speed = 0.0f;
	DirectionX = 0.0f;
	DirectionY = 0.0f;
}

// [플레이어 상태 패킷]이 캐릭터의 상태를 바꿈
void S_Character::SetCharacterState(PACKET_PlayerState* state)
{
	switch (state->DirectionState)
	{
	case DirectionState::DOWN:
		DirectionX = +0.0f;
		DirectionY = +1.0f;
		break;
	case DirectionState::DOWN_LEFT:
		DirectionX = -0.707106f;
		DirectionY = +0.707106f;
		break;
	case DirectionState::DOWN_RIGHT:
		DirectionX = +0.707106f;
		DirectionY = +0.707106f;
		break;
	case DirectionState::LEFT:
		DirectionX = -1.0f;
		DirectionY = +0.0f;
		break;
	case DirectionState::RIGHT:
		DirectionX = +1.0f;
		DirectionY = +0.0f;
		break;
	case DirectionState::UP:
		DirectionX = +0.0f;
		DirectionY = -1.0f;
		break;
	case DirectionState::UP_LEFT:
		DirectionX = -0.707106f;
		DirectionY = -0.707106f;
		break;
	case DirectionState::UP_RIGHT:
		DirectionX = +0.707106f;
		DirectionY = -0.707106f;
		break;
	default:
		printf("플레이어 상태 오류\n");
		break;
	}

	AnimationState = state->DirectionState;

	if (state->IsMoving == 1)
	{
		Speed = MaxSpeed;
	}
	else if (state->IsMoving == 0)
	{
		Speed = 0.0f;
	}
	else
	{
		printf("플레이어 움직임 오류\n");
	}
}

float S_Character::GetPosX() const { return PositionX; }

float S_Character::GetPosY() const { return PositionY; }

float S_Character::GetRight() const { return PositionX + CHAR_WIDTH; }

float S_Character::GetBottom() const { return PositionY + CHAR_HEIGHT; }

int S_Character::GetLife() const { return Life; }

void S_Character::SetLife(int life) { Life = life; }

int S_Character::GetType() const { return Type; }

void S_Character::SetType(int type) { Type = type; }

BYTE S_Character::GetAnimState() const { return AnimationState; }

bool S_Character::CheckAnimTickCount()
{
	if (Speed == MaxSpeed)
	{
		++AnimTickCount;

		if (AnimTickCount > 2)
		{
			AnimTickCount = 0;
		}

		return true;
	}

	return false;
}

BYTE S_Character::GetAnimCount() const { return AnimationCount; }

void S_Character::SetAnimCount(int count)
{
	AnimationCount = count;
}

// 캐릭터의 상태를 [캐릭터 정보 패킷]에 복사
void S_Character::SetCharacterPacket(PACKET_Character* charpacket)
{
	charpacket->Position.X = (short)PositionX;
	charpacket->Position.Y = (short)PositionY;
	charpacket->Life = (short)Life;
	charpacket->AnimationState = AnimationState;
	charpacket->AnimationCount = AnimationCount;
	charpacket->Type = Type;
}

void S_Character::PositionUpdate(float ticktime)
{
	PositionX = PositionX + Speed * DirectionX * ticktime;
	PositionY = PositionY + Speed * DirectionY * ticktime;

	if (PositionX < 48.0f)
		PositionX = 48.0f;
	else if (PositionX > 1104.0f)
		PositionX = 1104.0f;

	if (PositionY < 66.0f)
		PositionY = 66.0f;
	else if (PositionY > 858.0f)
		PositionY = 858.0f;
}

void S_Character::BounceCollision(float ticktime)
{
	PositionX = PositionX + Speed * DirectionX * -1.0f * ticktime;
	PositionY = PositionY + Speed * DirectionY * -1.0f * ticktime;
}

void S_Character::PrintInfo() const
{
	printf("Pos  : ( %f, %f)\n",
		PositionX, PositionY);
	printf("Life : %d\n", Life);
	printf("Type : %d\n", Type);
	printf("Anim : ( %d, %d)\n\n",
		AnimationState, AnimationCount
	);
}