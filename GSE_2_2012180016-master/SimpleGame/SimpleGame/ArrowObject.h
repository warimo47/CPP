#pragma once
#include "Object.h"
class ArrowObject : public Object
{
public:
	ArrowObject(const float x, const float y, const enum ObjectType E_Type, Object* ObjectPointer);
	~ArrowObject();
};

