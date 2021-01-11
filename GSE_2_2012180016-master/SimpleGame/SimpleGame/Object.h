#pragma once

#define OBJECTNAMELENGTH 32

using namespace std;

struct Vector2
{
	float X;
	float Y;
};

class Object
{
protected:
	float PositionX, PositionY, PositionZ;
	float RGBA[4];
	float Size;
	Vector2 Direction;
	float Speed;

	float Life;
	float LifeTime;

	int AniState;
	int AniCount;

	int AniTickCount;

	enum ObjectType Type;

	Object* ParentsPointer;

public:
	Object();
	Object(const float, const float, const ObjectType, Object*);
	~Object();

	float getPositionX() const;
	float getPositionY() const;
	float getPositionZ() const;

	float getColorRed() const;
	float getColorGreen() const;
	float getColorBlue() const;
	float getColorAlpha() const;
	float* getColorNAlpha();

	float getSize() const;
	Vector2 getDirection() const;

	float getMaxX() const;
	float getMinX() const;
	float getMaxY() const;
	float getMinY() const;

	float getLife() const;
	float getLifeTime() const;

	int getAniState() const;
	int getAniCount() const;

	ObjectType getType() const;

	Object* getParents() const;

	void setPosition(const float x, const float y, const float z);
	void setColorNAlpha(const float r, const float g, const float b, const float a);
	void setSize(const float size);
	void setType(const ObjectType type);

	virtual void update(const float DeltaTime);
	void positionUpdate(const float DeltaTime);
	void decreaseLife(const float Damages);

	bool checkOutOfView();
};

