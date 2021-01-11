#pragma once

#include "S_Bullet.h"

#define OBSTACLE_NUM 15

class S_Obstacle
{
	float PositionX;
	float PositionY;

public:
	S_Obstacle();
	S_Obstacle(float, float);
	~S_Obstacle();

	float GetPosX() const;
	float GetPosY() const;
	float GetRight() const;
	float GetBottom() const;
};

