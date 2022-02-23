#include <iostream>
#include <vector>
#include <string>

int main()
{
	int a = 5;
	int b = 0;
	int c = -1;

	try
	{
		if (b == 0) throw(std::string("b is zero error"));
		// if (b == 0) throw("b is zero error");
		c = a / b;
		std::cout << "a / b = c\n";
		std::cout << a << " / " << b << " = " << c << "\n";
	}
	catch (std::exception e)
	{
		std::cout << e.what() << "\n";
	}
	catch (std::string s)
	{
		std::cout << s << "\n";
	}
	catch (...)
	{
		std::cout << "default exception\n";
	}

	std::cout << "정상 종료\n";
	return 0;
}