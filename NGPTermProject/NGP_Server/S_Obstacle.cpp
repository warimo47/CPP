#include "S_Obstacle.h"

S_Obstacle::S_Obstacle()
{
	PositionX = -200.0f;
	PositionY = -200.0f;
}

S_Obstacle::S_Obstacle(float X, float Y)
{
	PositionX = X;
	PositionY = Y;
}

S_Obstacle::~S_Obstacle()
{
}

float S_Obstacle::GetPosX() const
{
	return PositionX;
}

float S_Obstacle::GetPosY() const
{
	return PositionY;
}

float S_Obstacle::GetRight() const
{
	return PositionX + CHAR_WIDTH;
}

float S_Obstacle::GetBottom() const
{
	return PositionY + CHAR_HEIGHT;
}
