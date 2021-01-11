#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <iostream>

using namespace std;

// ���� �Լ� ���� ��� �� ����
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

int main(int argc, char *argv[])
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(1, 1), &wsa) != 0)
		return 1;
	MessageBox(NULL, "���� �ʱ�ȭ ����", "�˸�", MB_OK);
	
	cout << "wVersion : \t" << (short)HIBYTE(wsa.wVersion) << "." << (short)LOBYTE(wsa.wVersion) << endl;
	cout << "wHighVersion : \t" << (short)HIBYTE(wsa.wHighVersion) << "." << (short)LOBYTE(wsa.wHighVersion) << endl;
	cout << "szDescription :\t" << wsa.szDescription << endl;
	cout << "szSystemStatus :" << wsa.szSystemStatus << endl;
	
	// socket()
	SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sock == INVALID_SOCKET) err_quit("socket()");
	// MessageBox(NULL, "TCP ���� ���� ����", "�˸�", MB_OK);

	// closesocket()
	closesocket(tcp_sock);

	// ���� ����
	WSACleanup();
	return 0;
}
