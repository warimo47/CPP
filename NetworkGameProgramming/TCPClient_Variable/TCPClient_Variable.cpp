#pragma comment(lib, "ws2_32")

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#define SERVERPORT	9000
#define BUFSIZE		50

using namespace std;

// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	char domainOrIPv4[256];

	SOCKADDR_IN serveraddr;
	HOSTENT* ptr;
	int retval;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_display("socket()");

#pragma pack(1)
	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE];
	string textBuf;
	int len;

#pragma pack()

	while (1)
	{
		cout << "도메인 또는 IP 입력 : ";
		cin >> domainOrIPv4;

		// connect()
		ZeroMemory(&serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(SERVERPORT);
		// 도메인을 입력하면 IP형태 문자열로 바꿔줌
		if ('0' <= domainOrIPv4[0] && domainOrIPv4[0] <= '9')
		{
			serveraddr.sin_addr.s_addr = inet_addr(domainOrIPv4);
		}
		else
		{
			ptr = gethostbyname(domainOrIPv4);
			serveraddr.sin_addr.s_addr = inet_addr(ptr->h_addr);
		}

		retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
		if (retval == SOCKET_ERROR) err_quit("connet()");

		// 서버와 데이터 통신
		while (1)
		{
			// 데이터 입력
			getline(cin, textBuf, '\n');
			len = textBuf.length();
			strncpy_s(buf, sizeof(buf), textBuf.c_str(), len);
			buf[len++] = '\n';

			// 데이터 보내기
			retval = send(sock, buf, len, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send()");
				break;
			}

			cout << "[TCP 클라이언트] " << sizeof(int) << " + " << retval
				<< "바이트를 보냈습니다." << endl;
		}
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}
