#pragma once
#include "Human.h"
class Male : public Human
{
public:
	void talk();
	virtual void run();
	void read2() override;
	void read1() override;
	void run(int b) override;
};

