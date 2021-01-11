#pragma comment(lib, "ws2_32")

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#define SERVERPORT	9000
#define BUFSIZE		50

using namespace std;

// ���� �Լ� ���� ��� �� ����
void err_quit(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPTSTR)&lpMsgBuf, (LPCSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	cout << "[" << msg << "] " << lpMsgBuf << endl;
	LocalFree(lpMsgBuf);
}

void domainToIPv4(char* str, IN_ADDR* ipv4)
{
	char* tempStr;

	strtok_s(str, ".", &tempStr);

	ipv4->S_un.S_un_b.s_b1 = atoi(str);

	strtok_s(tempStr, ".", &str);

	ipv4->S_un.S_un_b.s_b2 = atoi(tempStr);

	strtok_s(str, ".", &tempStr);

	ipv4->S_un.S_un_b.s_b3 = atoi(str);

	strtok_s(tempStr, ".", &str);

	ipv4->S_un.S_un_b.s_b4 = atoi(tempStr);

	ipv4->S_un.S_un_w.s_w1 = ipv4->S_un.S_un_b.s_b2 * 256 + ipv4->S_un.S_un_b.s_b1;
	ipv4->S_un.S_un_w.s_w2 = ipv4->S_un.S_un_b.s_b4 * 256 + ipv4->S_un.S_un_b.s_b3;

	ipv4->S_un.S_addr = ipv4->S_un.S_un_w.s_w2 * (256 * 256) + ipv4->S_un.S_un_w.s_w1;
}

int main(int argc, char* argv[])
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	char domainOrIPv4[256];

	// connect() ȣ�⿡ ��� �� ����
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serveraddr.sin_port = htons(SERVERPORT);
	
	HOSTENT* ptr;
	int retval;

#pragma pack(1)
	// ������ ��ſ� ����� ����
	char buf[BUFSIZE];
	char* testdata[] = {
		"�ȳ��ϼ���",
		"�ݰ�����",
		"���õ��� �� �̾߱Ⱑ ���� �� ���׿�",
		"���� �׷��׿�",
	};
	int len;

#pragma pack()

	// ������ ������ ���
	for(int i = 0; i < 4; ++i)
	{
		// socket()
		SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET) err_display("socket()");

		// connect()
		retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
		if (retval == SOCKET_ERROR) err_quit("connect()");

		// ������ �Է� ( �ùķ��̼� )
		len = strlen(testdata[i]);
		strncpy_s(buf, sizeof(buf), testdata[i], len);

		// ������ ������
		retval = send(sock, buf, len, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			break;
		}

		cout << "[TCP Ŭ���̾�Ʈ] " << sizeof(int) << " + " << retval
			<< "����Ʈ�� ���½��ϴ�." << endl;
	
		// closesocket()
		closesocket(sock);
	}

	// ���� ����
	WSACleanup();
	return 0;
}
