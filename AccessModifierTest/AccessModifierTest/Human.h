#pragma once
class Human
{
public:
	virtual void talk() = 0;
	virtual void read2();

private:
	virtual void read1();

	virtual void run(int a) = 0;
};

