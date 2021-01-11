#include <Windows.h>
#include <stdio.h>
#include <iostream>

using namespace std;

int sum = 0;

DWORD WINAPI MyThread(LPVOID arg)
{
	int num = (int)arg;
	for (int i = 1; i <= num; ++i) sum += i;
	return 0;
}

int main(int argc, char* argv[])
{
	int num = 100;
	HANDLE hThread = CreateThread(NULL, 0, MyThread, (LPVOID)num, CREATE_SUSPENDED, NULL);
	if (hThread == NULL) return 1;

	cout << "������ ���� ��. ��� ��� = " << sum << endl;
	ResumeThread(hThread);

	WaitForSingleObject(hThread, INFINITE);
	
	cout << "������ ���� ��. ��� ��� = " << sum << endl;
	
	CloseHandle(hThread);

	return 0;
}