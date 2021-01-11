#include "stdafx.h"
#include "CharacterObject.h"


CharacterObject::CharacterObject(const float x, const float y, const enum ObjectType E_Type, Object* ObjectPointer) : Object(x, y, E_Type, ObjectPointer)
{
	Speed = 300.0f;

	Size = 30.0f;

	if (E_Type == CHARACTER_T1)
	{
		RGBA[0] = 1.0f;
		RGBA[1] = 0.0f;
		RGBA[2] = 0.0f;
		RGBA[3] = 1.0f;
	}
	else if (E_Type == CHARACTER_T2)
	{
		RGBA[0] = 0.0f;
		RGBA[1] = 0.0f;
		RGBA[2] = 1.0f;
		RGBA[3] = 1.0f;
	}
	else
	{
		RGBA[0] = 1.0f;
		RGBA[1] = 1.0f;
		RGBA[2] = 1.0f;
		RGBA[3] = 1.0f;
	}

	Life = CHARACTER_MAX_LIFE;

	ArrowRespawnTime = 0;
}


CharacterObject::~CharacterObject()
{
}

bool CharacterObject::CheckArrowRespawnTime(DWORD dTime)
{
	ArrowRespawnTime = ArrowRespawnTime + dTime;

	if (ArrowRespawnTime > ARROW_RESPAWN_TIME)
	{
		ArrowRespawnTime = ArrowRespawnTime - ARROW_RESPAWN_TIME;
		return true;
	}

	return false;
}

void CharacterObject::Update(const float DeltaTime)
{
	LifeTime = LifeTime - DeltaTime; // Decrease LifeTime

	positionUpdate(DeltaTime);
}