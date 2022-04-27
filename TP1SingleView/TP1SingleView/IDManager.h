#pragma once

#include <vector> // vector

#include "yolo_v2_class.hpp" // bbox_t

class IDManager
{
private:
	std::vector<bbox_t> results[10];

	int nowFrame; // 0-9



public:
	IDManager();
	~IDManager();

	void PushResult(std::vector<bbox_t>);
	void GrantID();
};