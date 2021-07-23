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
			// 파일 읽기에 사용하는 변수
			ifstream dataFile;
			DATAFILE dataFileInfo;

			// 파일을 나누어 송신 할 때 쓰이는 변수
			int Slice;
			int count;
			int percent = 0;

			// 파일 이름을 여러번 받기 위한 반복문
			while (1)
			{
				// 파일 이름을 입력 받아 읽기
				cout << "파일 이름 입력 : ";
				cin.getline(dataFileInfo.name, 256);

				dataFile.open(dataFileInfo.name, ios::binary);

				if (!dataFile) cout << "파일 불러오기 에러" << endl << endl;
				else break;
			}

			// 파일의 크기를 얻기 위한 작업
			dataFile.seekg(0, ios::end);
			dataFileInfo.bytes = (int)dataFile.tellg();
			dataFile.seekg(0, ios::beg);

			// 파일 기본 정보 송신
			retval = send(client_sock, (char*)&dataFileInfo, sizeof(dataFileInfo), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("파일 기본 정보 전송");
				exit(1);
			}

			// 파일의 크기가 버퍼 크기보다 큰 경우 버퍼 크기로 나눈다.
			Slice = count = dataFileInfo.bytes / BUFSIZE;

			// 버퍼 크기 보다 큰 파일을 버퍼 크기 만큼 여러번 송신한다.
			while (count)
			{
				// 파일 읽어서 버퍼 크기 만큼 버퍼에 저장
				dataFile.read(buf, BUFSIZE);

				// 파일 송신
				retval = send(client_sock, buf, BUFSIZE, 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("파일 전송 while(count) 안");
					continue;
				}

				if (percent != ((Slice - count) * 100 / Slice))
				{
					system("cls");

					cout << "전송하는 파일 : " << dataFileInfo.name <<
						", 전송하는 파일 크기 : " << dataFileInfo.bytes << " Bytes" << endl;

					cout << endl << "진행도 : " << (Slice - count) * 100 / Slice << "% ";
					percent = (Slice - count) * 100 / Slice;
				}

				--count;
			}

			// 버퍼 크기 보다 작은 파일 나머지 송신

			system("cls");

			cout << "전송하는 파일 : " << dataFileInfo.name <<
				", 전송하는 파일 크기 : " << dataFileInfo.bytes << " Bytes" << endl;

			count = dataFileInfo.bytes - (Slice * BUFSIZE);
			dataFile.read(buf, count);

			retval = send(client_sock, buf, count, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("파일 전송 나머지");
				continue;
			}

			cout << endl << "진행도 : 100% " << endl << endl;

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

