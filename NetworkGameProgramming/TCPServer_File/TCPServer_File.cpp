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

// 파일 기본 정보에 대한 구조체
struct DATAFILE
{
	char name[FIELNAMELENGTH];
	int bytes;
};

int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 수신에 사용 할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE];

	while (1)
	{
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);

		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		cout << endl << "[TCP 서버] 클라이언트 접속 : IP 주소 = " << inet_ntoa(clientaddr.sin_addr)
			<< ", 포트 번호 = " << ntohs(clientaddr.sin_port) << endl;

		// --- 클라이언트와 데이터 통신 시작 ---
		while (1)
		{
			// 파일 쓰기에 사용하는 변수
			ofstream dataFile;
			DATAFILE dataFileInfo;

			// 파일은 나누어 수신 받을 때 쓰이는 변수
			int slice;
			int count;
			int percent = 0;

			// 파일 기본 정보 수신
			retval = recvn(client_sock, (char*)&dataFileInfo, sizeof(dataFileInfo), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("파일 기본 정보 수신");
				exit(1);
			}

			// 파일 열기
			dataFile.open(dataFileInfo.name, ios::binary | ios::trunc);

			// 파일의 크기가 버퍼 크기보다 큰지 확인한다.
			slice = count = dataFileInfo.bytes / BUFSIZE;

			// 버퍼 크기 보다 큰 파일을 버퍼 크기 만큼 수신 받는다.
			while (count)
			{
				retval = recvn(client_sock, buf, BUFSIZE, 0);
				if (retval == SOCKET_ERROR)
				{
					err_quit("파일 수신");
					exit(1);
				}

				if (percent != ((slice - count) * 100 / slice))
				{
					system("cls");

					cout << "전송받는 파일 : " << dataFileInfo.name <<
						", 전송받는 파일 크기 : " << dataFileInfo.bytes << " Bytes" << endl;

					cout << endl << "진행도 : " << (slice - count) * 100 / slice << "% " << endl;
					percent = (slice - count) * 100 / slice;
				}

				// 파일 쓰기
				dataFile.write(buf, BUFSIZE);

				--count;
			}

			// 버퍼 크기 보다 작은 파일 나머지 수신
			count = dataFileInfo.bytes - ((dataFileInfo.bytes / BUFSIZE) * BUFSIZE);

			retval = recvn(client_sock, buf, count, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("파일 나머지 수신");
				exit(1);
			}

			system("cls");

			cout << "전송받는 파일 : " << dataFileInfo.name <<
				", 전송받는 파일 크기 : " << dataFileInfo.bytes << " Bytes" << endl;

			cout << endl << "진행도 : 100% " << endl;

			// 파일 쓰기
			dataFile.write(buf, count);

			// 파일 포인터 닫기
			dataFile.close();
		}
	}

	// closesocket()
	closesocket(client_sock);
	cout << "[TCP 서버] 클라이언트 종료 : IP 주소 = " << inet_ntoa(clientaddr.sin_addr)
		<< ", 포트 번호 = " << ntohs(clientaddr.sin_port) << endl;

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

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

// 사용자 정의 데이터 수신 함수
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