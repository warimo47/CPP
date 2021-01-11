#include "stdafx.h"
#include "Object.h"

int Object::NextID = 0;

Object::Object()
{
	ID = NextID;
	Name = "";
	PositionX = 0.0;
	PositionY = 0.0;
	PositionZ = 0.0;
	Size = 4.0;
	ColorRed = 1.0;
	ColorGreen = 1.0;
	ColorBlue = 1.0;
	ColorAlpha = 1.0;
	Type = UNIT;

	++NextID;
}

Object::~Object()
{
}

int const Object::getID()
{
	return ID;
}

string const Object::getName()
{
	return Name;
}

float const Object::getPositionX()
{
	return PositionX;
}

float const Object::getPositionY()
{
	return PositionY;
}

float const Object::getPositionZ()
{
	return PositionZ;
}

float const Object::getSize()
{
	return Size;
}

float const Object::getColorRed()
{
	return ColorRed;
}

float const Object::getColorGreen()
{
	return ColorGreen;
}

float const Object::getColorBlue()
{
	return ColorBlue;
}

float const Object::getColorAlpha()
{
	return ColorAlpha;
}

ObjectType const Object::getType()
{
	return Type;
}

void Object::setID(const int id)
{
	this->ID = id;
}

void Object::setName(const string str)
{
	this->Name = str;
}

void Object::setPosition(const float x, const float y, const float z)
{
	this->PositionX = x;
	this->PositionY = y;
	this->PositionZ = z;
}

void Object::setColor(const float r, const float g, const float b, const float a)
{
	this->ColorRed = r;
	this->ColorGreen = g;
	this->ColorBlue = b;
	this->ColorAlpha = a;
}

void Object::setSize(const float size)
{
	this->Size = size;
}

void Object::setType(const ObjectType type)
{
	this->Type = type;
}