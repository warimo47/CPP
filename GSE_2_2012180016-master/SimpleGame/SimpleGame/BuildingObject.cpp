#include "stdafx.h"
#include "BuildingObject.h"

BuildingObject::BuildingObject(const float x, const float y, const ObjectType E_Type, Object* ObjectPointer) : Object(x, y, E_Type, ObjectPointer)
{
	Speed = 0.0f;

	Size = 100.0f;

	RGBA[0] = 1.0f;
	RGBA[1] = 1.0f;
	RGBA[2] = 1.0f;
	RGBA[3] = 1.0f;

	Life = BUILDING_MAX_LIFE;

	BulletRespawnTime = BULLET_RESPAWN_TIME;
}

BuildingObject::~BuildingObject()
{
}

bool BuildingObject::CheckBulletRespawnTime(DWORD dTime)
{
	BulletRespawnTime = BulletRespawnTime + dTime;

	if (BulletRespawnTime > BULLET_RESPAWN_TIME)
	{
		BulletRespawnTime = 0;
		return true;
	}

	return false;
}

void BuildingObject::Update(const float DeltaTime)
{
	LifeTime = LifeTime - DeltaTime; // Decrease LifeTime

	positionUpdate(DeltaTime);
}