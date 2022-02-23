/////////// Callback 예제 ///////////

#include <iostream>
#include <vector>
#include <string>

///////////// Server ///////////////

typedef void(*CALLBACK_FUNC)(int); // 함수 포인터 정의

CALLBACK_FUNC cbf = NULL; // 실제 콜백함수를 다루기 위한 전역 변수

void RegistCallback(CALLBACK_FUNC cb) // 콜백 등록 함수
{
	cbf = cb;
}

void StartCallback() // 콜백 테스트를 위한 함수
{
	if (cbf == NULL)
	{
		std::cout << "Callback Function is not Registed\n";
		return;
	}

	std::cout << "Server calls CB.\n";
	cbf(1); // 서버 측에 등록된 콜백함수 호출 / 인자로 1을 전달.

}

///////////// Client ///////////////

void UserCallback(int n)
{
	if (n == 1)
	{
		std::cout << "n == 1\n";

		
	}
	else
	{
		std::cout << "False\n";
	}
}

int main()
{
	int a = 5;
	int b = 0;
	int c = -1;

	try
	{
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

	///////////// Client ///////////////	
	RegistCallback(UserCallback); // Callback 등록


	///////////// Server ///////////////
	StartCallback();

	std::cout << "정상 종료\n";
	return 0;
}