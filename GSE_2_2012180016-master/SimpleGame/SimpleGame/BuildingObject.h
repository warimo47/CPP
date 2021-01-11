#pragma once
#include "Object.h"

#define BULLET_RESPAWN_TIME 2000

#define BUILDING_MAX_LIFE 500.0f

class BuildingObject : public Object
{
private:
	DWORD BulletRespawnTime;

public:
	BuildingObject(const float x, const float y, const enum ObjectType E_Type, Object* ObjectPointer);
	~BuildingObject();

	bool CheckBulletRespawnTime(DWORD dTime);

	void Update(const float);
};

