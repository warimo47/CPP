#include <Windows.h>
#include <iostream>

#define MAXCNT 100000000

using namespace std;

int g_count = 0;
CRITICAL_SECTION cs;

DWORD WINAPI MyThread1(LPVOID arg)
{
	EnterCriticalSection(&cs);
	for (int i = 0; i < MAXCNT; ++i)
	{
		
		g_count += 2;
		
	}
	LeaveCriticalSection(&cs);
	return 0;
}

DWORD WINAPI MyThread2(LPVOID arg)
{
	EnterCriticalSection(&cs);
	for (int i = 0; i < MAXCNT; ++i)
	{
		
		g_count -= 2;
		
	}
	LeaveCriticalSection(&cs);

	return 0;
}

int main(int argc, char* argv[])
{
	// 임계 영역 초기화
	InitializeCriticalSection(&cs);

	// 스레드 두 개 생성
	HANDLE hThread[2];

	hThread[0] = CreateThread(NULL, 0, MyThread1, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, MyThread2, NULL, 0, NULL);

	// 스레드 두 개 종료 대기
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

	// 임계 영역 삭제
	DeleteCriticalSection(&cs);

	// 결과 출력
	cout << "g_count = " << g_count << endl;

	return 0;
}