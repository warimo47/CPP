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

// ���� �⺻ ������ ���� ����ü
struct DATAFILE
{
	char name[256];
	int bytes;
};

// ��ȭ���� ���ν���
bool CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
// ���� ��� �Լ�
void err_quit(char* msg);
void err_display(char* msg);
// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char* buf, int len, int flags);
// ���� ��� ������ �Լ�
DWORD WINAPI ClientMain(LPVOID arg);

SOCKET sock;
char buf[BUFSIZE + 1];
HWND hSendButton, hEndButton;
HWND hEdit; // ���� ��Ʈ��

char fileName[256];

int WINAPI WinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	// ��ȭ���� ����
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, (DLGPROC)DlgProc);

	return 0;
}

// ��ȭ���� ���ν���
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
			
			// ���� ��� ������ ����
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
		// ���� �б⿡ ����ϴ� ����
		ifstream dataFile;
		DATAFILE dataFileInfo;

		// ������ ������ �۽� �� �� ���̴� ����
		int Slice;
		int count;
		int percent = 0;

		// ���� �̸��� �Է� �޾� �б�
		strcpy_s(dataFileInfo.name, fileName);
		dataFile.open(dataFileInfo.name, ios::binary);

		if (!dataFile) cout << "���� �ҷ����� ����" << endl << endl;
		else break;

		// ������ ũ�⸦ ��� ���� �۾�
		dataFile.seekg(0, ios::end);
		dataFileInfo.bytes = (int)dataFile.tellg();
		dataFile.seekg(0, ios::beg);

		// ���� �⺻ ���� �۽�
		retval = send(sock, (char*)&dataFileInfo, sizeof(dataFileInfo), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("���� �⺻ ���� ����");
			exit(1);
		}

		// ������ ũ�Ⱑ ���� ũ�⺸�� ū ��� ���� ũ��� ������.
		Slice = count = dataFileInfo.bytes / BUFSIZE;

		// ���� ũ�� ���� ū ������ ���� ũ�� ��ŭ ������ �۽��Ѵ�.
		while (count)
		{
			// ���� �о ���� ũ�� ��ŭ ���ۿ� ����
			dataFile.read(buf, BUFSIZE);

			// ���� �۽�
			retval = send(sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("���� ���� while(count) ��");
				continue;
			}
			
			--count;
		}

		// ���� ũ�� ���� ���� ���� ������ �۽�

		count = dataFileInfo.bytes - (Slice * BUFSIZE);
		dataFile.read(buf, count);

		retval = send(sock, buf, count, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("���� ���� ������");
			continue;
		}

		// ���� ������ �ݱ�
		dataFile.close();
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();



	return 0;
}