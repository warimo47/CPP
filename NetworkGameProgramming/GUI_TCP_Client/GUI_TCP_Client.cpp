#pragma comment(lib, "ws2_32")

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "resource.h"

using namespace std;

#define SERVERIP	"127.0.0.1"
#define SERVERPORT	9000
#define BUFSIZE		512

// ��ȭ���� ���ν���
bool CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
// ���� ��Ʈ�� ��� �Լ�
void DisplayText(char* fmt, ...);
// ���� ��� �Լ�
void err_quit(char* msg);
void err_display(char* msg);
// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char* buf, int len, int flags);
// ���� ��� ������ �Լ�
DWORD WINAPI ClientMain(LPVOID arg);

SOCKET sock;
char buf[BUFSIZE + 1];
HANDLE hReadEvent, hWriteEvent;
HWND hSendButton, hEndButton;
HWND hEdit1, hEdit2; // ���� ��Ʈ��

int WINAPI WinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	// �̺�Ʈ ����
	hReadEvent = CreateEvent(nullptr, false, true, nullptr);
	if (hReadEvent == nullptr) return 1;
	hWriteEvent = CreateEvent(nullptr, false, false, nullptr);
	if (hWriteEvent == nullptr) return 1;

	// ���� ��� ������ ����
//	CreateThread(nullptr, 0, ClientMain, nullptr, 0, nullptr);

	// ��ȭ���� ����
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, (DLGPROC)DlgProc);

	// �̺�Ʈ ����
	CloseHandle(hReadEvent);
	CloseHandle(hWriteEvent);

	return 0;
}

// ��ȭ���� ���ν���
bool CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
		hSendButton = GetDlgItem(hDlg, IDOK);
		hEndButton = GetDlgItem(hDlg, IDOK2);
		SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);
		return true;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EnableWindow(hSendButton, false);
			WaitForSingleObject(hReadEvent, INFINITE);
			GetDlgItemText(hDlg, IDC_EDIT1, buf, BUFSIZE + 1);
			SetEvent(hWriteEvent);
			SetFocus(hEdit1);
			SendMessage(hEdit1, EM_SETSEL, 0, -1);
			return true;
		case IDOK2:
			EndDialog(hDlg, IDOK2);
			return true;
		}
		return false;
	case WM_LBUTTONDOWN:
		MoveWindow(hDlg, 700, 20, 530, 330, true);
		return true;
	}
	return false;
}

// ���� ��Ʈ�� ��� �Լ�
void DisplayText(char* fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	char cbuf[BUFSIZE + 256];
	vsprintf_s(cbuf, fmt, arg);

	int nLength = GetWindowTextLength(hEdit2);

	SendMessage(hEdit2, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit2, EM_REPLACESEL, false, (LPARAM)cbuf);

	va_end(arg);
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

// TCP Ŭ���̾�Ʈ ���� �κ�
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;

	// ���� �ʱ�ȭ
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

	// ������ ������ ���
	while (1)
	{
		// ���� �Ϸ� ��ٸ���
		WaitForSingleObject(hWriteEvent, INFINITE);

		// ���ڿ� ���̰� 0�̸� ������ ����
		if (strlen(buf) == 0)
		{
			EnableWindow(hSendButton, true);
			SetEvent(hReadEvent);
			continue;
		}

		unsigned

		// ������ ������
		retval = send(sock, buf, strlen(buf), 0);

		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			break;
		}

		DisplayText("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\r\n", retval);

		// ������ �ޱ�
		retval = recvn(sock, buf, retval, 0);

		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		else if (retval == 0) break;

		// ���� ������ ���
		buf[retval] = '\0';
		DisplayText("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\r\n", retval);
		DisplayText("[���� ������] %s\r\n", buf);

		EnableWindow(hSendButton, true);
		SetEvent(hReadEvent);
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();

	return 0;
}