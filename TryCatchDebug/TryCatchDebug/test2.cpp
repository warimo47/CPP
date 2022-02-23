#include <iostream>
#include <vector>
#include <string>

int main()
{
	int a[5] = { 0, };
	int b = 10;

	try
	{
		a[b] = 55;
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