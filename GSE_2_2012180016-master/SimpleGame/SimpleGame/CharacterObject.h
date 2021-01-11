#pragma once
#include "Object.h"

#define ARROW_RESPAWN_TIME 3000

#define CHARACTER_MAX_LIFE 100.0f

class CharacterObject : public Object
{
private:
	DWORD ArrowRespawnTime;
public:
	CharacterObject(const float, const float, const ObjectType, Object* ObjectPointer);
	~CharacterObject();

	bool CheckArrowRespawnTime(DWORD dTime);

	void Update(const float);
};

