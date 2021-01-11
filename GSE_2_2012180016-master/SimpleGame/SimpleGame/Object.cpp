#include "stdafx.h"
#include "Object.h"

Object::Object()
{
	PositionX = 0.0f;
	PositionY = 0.0f;
	PositionZ = 0.0f;

	float randomRadian = (float)rand();

	Direction.X = cosf(randomRadian);
	Direction.Y = sinf(randomRadian);

	LifeTime = 1000000.0f;

	Type = ARROW_T1;

	ParentsPointer = nullptr;

	AniState = 0;
	AniCount = 0;

	AniTickCount = 0;
}

Object::Object(const float PosX, const float PosY, const ObjectType E_Type, Object* objectspointer)
{
	PositionX = PosX;
	PositionY = PosY;
	PositionZ = 0.0f;

	float randomRadian = (float)rand();

	Direction.X = cosf(randomRadian);
	Direction.Y = sinf(randomRadian);

	LifeTime = 1000000.0f;

	Type = E_Type;

	ParentsPointer = objectspointer;

	AniState = 0;
	AniCount = 0;

	AniTickCount = 0;
}

Object::~Object()
{
}

float Object::getPositionX() const
{
	return PositionX;
}

float Object::getPositionY() const
{
	return PositionY;
}

float Object::getPositionZ() const
{
	return PositionZ;
}

float Object::getSize() const
{
	return Size;
}

Vector2 Object::getDirection() const
{
	return Direction;
}

float Object::getMaxX() const
{
	return PositionX + (Size / 2.0f);
}

float Object::getMinX() const
{
	return PositionX - (Size / 2.0f);
}

float Object::getMaxY() const
{
	return PositionY + (Size / 2.0f);
}

float Object::getMinY() const
{
	return PositionY - (Size / 2.0f);
}

float Object::getColorRed() const
{
	return RGBA[0];		// RED
}

float Object::getColorGreen() const
{
	return RGBA[1];		// GREEN
}

float Object::getColorBlue() const
{
	return RGBA[2];		// BLUE
}

float Object::getColorAlpha() const
{
	return RGBA[3];		// ALPHA
}

float* Object::getColorNAlpha()
{
	return RGBA;
}

float Object::getLife() const
{
	return Life;
}

float Object::getLifeTime() const
{
	return LifeTime;
}

int Object::getAniState() const
{
	return AniState;
}

int Object::getAniCount() const
{
	return AniCount;
}

ObjectType Object::getType() const
{
	return Type;
}

Object* Object::getParents() const
{
	return ParentsPointer;
}

void Object::setPosition(const float x, const float y, const float z)
{
	PositionX = x;
	PositionY = y;
	PositionZ = z;
}

void Object::setColorNAlpha(const float r, const float g, const float b, const float a)
{
	RGBA[0] = r;
	RGBA[1] = g;
	RGBA[2] = b;
	RGBA[3] = a;
}

void Object::setSize(const float size)
{
	Size = size;
}

void Object::setType(const ObjectType type)
{
	Type = type;
}

void Object::update(const float DeltaTime)
{
	// Life = Life - 1.0f;
	LifeTime = LifeTime - DeltaTime; // Decrease LifeTime

	positionUpdate(DeltaTime);

	++AniTickCount;
	if (AniTickCount > 120)
	{
		AniTickCount = 0;
		++AniState;
		if (AniState > 3) AniState = 0;
	}
}

void Object::positionUpdate(const float DeltaTime)
{
	PositionX = PositionX + (Direction.X * Speed * DeltaTime);
	PositionY = PositionY + (Direction.Y * Speed * DeltaTime);
}

void Object::decreaseLife(const float Damages)
{
	Life = Life - Damages;
}

bool Object::checkOutOfView()
{
	if ((Type == BUILDING_T1) || (Type == BUILDING_T2) || (Type == CHARACTER_T1) || (Type == CHARACTER_T2))
	{
		if (PositionX < 0)
		{
			PositionX = 0.0f;
			Direction.X = Direction.X * -1.0f;
		}
		else if (PositionX > WINDOWWIDTH)
		{
			PositionX = (float)WINDOWWIDTH;
			Direction.X = Direction.X * -1.0f;
		}
		if (PositionY < 0)
		{
			PositionY = 0.0f;
			Direction.Y = Direction.Y * -1.0f;
		}
		else if (PositionY > WINDOWHEIGHT)
		{
			PositionY = (float)WINDOWHEIGHT;
			Direction.Y = Direction.Y * -1.0f;
		}

		return false;
	}
	if (PositionX < 0) return true;
	if (PositionX > WINDOWWIDTH) return true;
	if (PositionY < 0) return true;
	if (PositionY > WINDOWHEIGHT) return true;

	return false;
}