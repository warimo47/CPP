#pragma comment(lib, "ws2_32")

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "resource.h"

using namespace std;

#define SERVERIP	"127.0.0.1"
#define SERVERPORT	9000
#define BUFSIZE		512

// 파일 기본 정보에 대한 구조체
struct DATAFILE
{
	char name[256];
	int bytes;
};

// 대화상자 프로시저
bool CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
// 오류 출력 함수
void err_quit(char* msg);
void err_display(char* msg);
// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char* buf, int len, int flags);
// 소켓 통신 스레드 함수
DWORD WINAPI ClientMain(LPVOID arg);

SOCKET sock;
char buf[BUFSIZE + 1];
HWND hSendButton, hEndButton;
HWND hEdit; // 편집 컨트롤

char fileName[256];

int WINAPI WinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	// 대화상자 생성
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, (DLGPROC)DlgProc);

	return 0;
}

// 대화상자 프로시저
bool CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		hEdit = GetDlgItem(hDlg, IDC_EDIT1);
		hSendButton = GetDlgItem(hDlg, IDOK);
		hEndButton = GetDlgItem(hDlg, IDCANCEL);
		SendMessage(hEdit, EM_SETLIMITTEXT, BUFSIZE, 0);
		return true;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_EDIT1:
			switch (HIWORD(wParam)) {
			case EN_CHANGE:
				GetWindowText(hEdit, fileName, 255);
				return true;
			}
			return false;
		case IDOK:
			EnableWindow(hSendButton, false);
			
			// 소켓 통신 스레드 생성
			GetDlgItemText(hDlg, IDC_EDIT1, fileName, BUFSIZE + 1);
			CreateThread(nullptr, 0, ClientMain, nullptr, 0, nullptr);
			SetFocus(hEdit);
			SendMessage(hEdit, EM_SETSEL, 0, -1);
			return true;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return true;
		}
		return false;
	case WM_LBUTTONDOWN:
		MoveWindow(hDlg, 700, 20, 390, 155, true);
		return true;
	}
	return false;
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
	cout << "[" << msg << "] " << lpMsgBuf;
	LocalFree(lpMsgBuf);
}

// 사용자 정의 데이터 수신 함수
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

// TCP 클라이언트 시작 부분
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// 서버와 데이터 통신
	while (1)
	{
		// 파일 읽기에 사용하는 변수
		ifstream dataFile;
		DATAFILE dataFileInfo;

		// 파일을 나누어 송신 할 때 쓰이는 변수
		int Slice;
		int count;
		int percent = 0;

		// 파일 이름을 입력 받아 읽기
		strcpy_s(dataFileInfo.name, fileName);
		dataFile.open(dataFileInfo.name, ios::binary);

		if (!dataFile) cout << "파일 불러오기 에러" << endl << endl;
		else break;

		// 파일의 크기를 얻기 위한 작업
		dataFile.seekg(0, ios::end);
		dataFileInfo.bytes = (int)dataFile.tellg();
		dataFile.seekg(0, ios::beg);

		// 파일 기본 정보 송신
		retval = send(sock, (char*)&dataFileInfo, sizeof(dataFileInfo), 0);
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
			retval = send(sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("파일 전송 while(count) 안");
				continue;
			}
			
			--count;
		}

		// 버퍼 크기 보다 작은 파일 나머지 송신

		count = dataFileInfo.bytes - (Slice * BUFSIZE);
		dataFile.read(buf, count);

		retval = send(sock, buf, count, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("파일 전송 나머지");
			continue;
		}

		// 파일 포인터 닫기
		dataFile.close();
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();



	return 0;
}