#pragma comment(lib, "ws2_32")

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#define SERVERPORT      9000
#define BUFSIZE         524288	// 512MB
#define FIELNAMELENGTH   256

using namespace std;

void err_quit(char* msg);
void err_display(char *msg);
int recvn(SOCKET s, char* buf, int len, int flags);

// ���� �⺻ ������ ���� ����ü
struct DATAFILE
{
	char name[256];
	int bytes;
};

int main(int argc, char* argv[])
{
	char ipv4addr[16];
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// ������ �۽ſ� ��� �� ����
	char buf[BUFSIZE];

	cout << "ipv4 �ּ� �Է� : ";
	cin >> ipv4addr;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_display("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ipv4addr);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connet()");

	// --- ������ ������ ��� ���� ---

	while (1)
	{
		// ���� ���⿡ ����ϴ� ����
		ofstream dataFile;
		DATAFILE dataFileInfo;

		// ������ ������ ���� ���� �� ���̴� ����
		int slice;
		int count;
		int percent = 0;

		// ���� �⺻ ���� ����
		retval = recvn(sock, (char*)&dataFileInfo, sizeof(dataFileInfo), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("���� �⺻ ���� ����");
			exit(1);
		}

		// ���� ����
		dataFile.open(dataFileInfo.name, ios::binary | ios::trunc);

		// ������ ũ�Ⱑ ���� ũ�⺸�� ū�� Ȯ���Ѵ�.
		slice = count = dataFileInfo.bytes / BUFSIZE;

		// ���� ũ�� ���� ū ������ ���� ũ�� ��ŭ ���� �޴´�.
		while (count)
		{
			retval = recvn(sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR)
			{
				err_quit("���� ����");
				exit(1);
			}

			if (percent != ((slice - count) * 100 / slice))
			{
				system("cls");

				cout << "���۹޴� ���� : " << dataFileInfo.name <<
					", ���۹޴� ���� ũ�� : " << dataFileInfo.bytes << " Bytes" << endl;

				cout << endl << "���൵ : " << (slice - count) * 100 / slice << "% " << endl;
				percent = (slice - count) * 100 / slice;
			}

			// ���� ����
			dataFile.write(buf, BUFSIZE);

			--count;
		}

		// ���� ũ�� ���� ���� ���� ������ ����
		count = dataFileInfo.bytes - ((dataFileInfo.bytes / BUFSIZE) * BUFSIZE);

		retval = recvn(sock, buf, count, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("���� ������ ����");
			exit(1);
		}

		system("cls");

		cout << "���۹޴� ���� : " << dataFileInfo.name <<
			", ���۹޴� ���� ũ�� : " << dataFileInfo.bytes << " Bytes" << endl;

		cout << endl << "���൵ : 100% " << endl;

		// ���� ����
		dataFile.write(buf, count);

		// ���� ������ �ݱ�
		dataFile.close();
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}

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

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	int left = len;
	char* ptr = buf;

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