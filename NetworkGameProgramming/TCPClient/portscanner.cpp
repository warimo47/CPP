#pragma comment(lib, "ws2_32")

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#define SERVERIP "127.0.0.1"
#define SERVERPORT	9000
#define BUFSIZE		512

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
	cout << "[" << msg << "] " << lpMsgBuf;
	LocalFree(lpMsgBuf);
}

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, ptr, left, flags);

		if (received == SOCKET_ERROR) return SOCKET_ERROR;
		else if (received == 0) break;
		left -= received;
		ptr += received;
	}

	return (len - left);
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

	int minPortNum, maxPortNum;
	int connectionCount = 0;

	SOCKADDR_IN serveraddr;
	HOSTENT* ptr;
	int retval;

	// socket()
	SOCKET sock;
	
	while (1)
	{
		cout << "������ �Ǵ� IP �Է� : ";
		cin >> domainOrIPv4;

		cout << "�ּ� ��Ʈ ��ȣ �Է� : ";
		cin >> minPortNum;

		cout << "�ִ� ��Ʈ ��ȣ �Է� : ";
		cin >> maxPortNum;

		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET) err_display("socket()");

		// connect()
		ZeroMemory(&serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;

		// �������� �Է��ϸ� IP���� ���ڿ��� �ٲ���
		if ('0' <= domainOrIPv4[0] && domainOrIPv4[0] <= '9')
		{
			serveraddr.sin_addr.s_addr = inet_addr(domainOrIPv4);
			cout << "������ �̸� : " << domainOrIPv4 << endl;
		}
		else
		{
			ptr = gethostbyname(domainOrIPv4);
			// serveraddr.sin_addr.s_addr = ptr->h_addr);
			cout << "IPv4 �ּ� : " << ptr->h_addr_list[0] << endl;
		}

		for (int portNumCount = minPortNum; portNumCount <= maxPortNum; ++portNumCount)
		{
			serveraddr.sin_port = htons(portNumCount);
			retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
			if (retval == SOCKET_ERROR)
			{
				cout << portNumCount << "�� ��Ʈ�� ���� : UNLISTENING" << endl;
			}
			else
			{
				cout << portNumCount << "�� ��Ʈ�� ���� : LISTENING" << endl;
				++connectionCount;
				closesocket(sock);
			};
		}

		cout << "LISTENING ������ ��Ʈ ���� : " << connectionCount << endl << endl;
		connectionCount = 0;
	}

	// ���� ����
	WSACleanup();
	return 0;
}
