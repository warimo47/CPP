#include <Windows.h>
#include <iostream>

#define BUFSIZE 10

HANDLE hReadEvent;
HANDLE hWriteEvent;
int buf[BUFSIZE];

DWORD WINAPI WriteThread(LPVOID arg)
{
	DWORD retval;

	for (int k = 1; k <= 500; ++k)
	{
		// �б� �Ϸ� ���
		retval = WaitForSingleObject(hReadEvent, INFINITE);

		if (retval != WAIT_OBJECT_0) break;


	}
}