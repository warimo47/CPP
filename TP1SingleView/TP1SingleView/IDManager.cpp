#include "pch.h"
#include "IDManager.h"

IDManager::IDManager()
{
	nowFrame = 0;
}

IDManager::~IDManager()
{

}

void IDManager::PushResult(std::vector<bbox_t> _results)
{
	results[nowFrame] = _results;
	nowFrame++;
	nowFrame = nowFrame % 10;

	if (nowFrame == 9) GrantID();
}

void IDManager::GrantID()
{

}