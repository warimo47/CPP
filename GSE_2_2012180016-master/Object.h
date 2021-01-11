#pragma once

#include <iostream>

using namespace std;

enum ObjectType { BACKGROUND, STATICOBJECT, MOVEABLEOBJECT, UNIT };

class Object
{
private:
	int ID;

	string Name;

	float PositionX, PositionY, PositionZ;
	float ColorRed, ColorGreen, ColorBlue, ColorAlpha;
	float Size;

	enum ObjectType Type;

	static int NextID;

public:
	Object();
	~Object();

	int const getID();

	string const getName();

	float const getPositionX();
	float const getPositionY();
	float const getPositionZ();

	float const getColorRed();
	float const getColorGreen();
	float const getColorBlue();
	float const getColorAlpha();

	float const getSize();

	ObjectType const getType();

	void setID(const int id);
	void setName(const string str);
	void setPosition(const float x, const float y, const float z);
	void setColor(const float r, const float g, const float b, const float a);
	void setSize(const float size);
	void setType(const ObjectType type);
};

