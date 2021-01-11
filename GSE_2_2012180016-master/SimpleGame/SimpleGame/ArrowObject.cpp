#include "stdafx.h"
#include "ArrowObject.h"


ArrowObject::ArrowObject(const float x, const float y, const enum ObjectType E_Type, Object* ObjectPointer) : Object(x, y, E_Type, ObjectPointer)
{
	Speed = 100.0f;

	Size = 4.0f;

	if (E_Type == ARROW_T1)
	{
		RGBA[0] = 1.0f;
		RGBA[1] = 0.0f;
		RGBA[2] = 1.0f;
		RGBA[3] = 1.0f;
	}
	else if (E_Type == ARROW_T2)
	{
		RGBA[0] = 1.0f;
		RGBA[1] = 1.0f;
		RGBA[2] = 0.0f;
		RGBA[3] = 1.0f;
	}
	else
	{
		RGBA[0] = 1.0f;
		RGBA[1] = 1.0f;
		RGBA[2] = 1.0f;
		RGBA[3] = 1.0f;
	}

	Life = 10.0f;
}


ArrowObject::~ArrowObject()
{
}
