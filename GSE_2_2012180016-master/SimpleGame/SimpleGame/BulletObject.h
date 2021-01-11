#pragma once
#include "Object.h"
class BulletObject : public Object
{
	DWORD ParticleTime;
public:
	BulletObject(const float x, const float y, const enum ObjectType E_Type, Object* ObjectPointer);
	~BulletObject();

	DWORD GetParticleTime() const;
	void AddParticleTime(DWORD);
};

