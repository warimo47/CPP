/////////// Callback ���� ///////////

#include <iostream>
#include <vector>
#include <string>

///////////// Server ///////////////

typedef void(*CALLBACK_FUNC)(int); // �Լ� ������ ����

CALLBACK_FUNC cbf = NULL; // ���� �ݹ��Լ��� �ٷ�� ���� ���� ����

void RegistCallback(CALLBACK_FUNC cb) // �ݹ� ��� �Լ�
{
	cbf = cb;
}

void StartCallback() // �ݹ� �׽�Ʈ�� ���� �Լ�
{
	if (cbf == NULL)
	{
		std::cout << "Callback Function is not Registed\n";
		return;
	}

	std::cout << "Server calls CB.\n";
	cbf(1); // ���� ���� ��ϵ� �ݹ��Լ� ȣ�� / ���ڷ� 1�� ����.

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
	RegistCallback(UserCallback); // Callback ���


	///////////// Server ///////////////
	StartCallback();

	std::cout << "���� ����\n";
	return 0;
}