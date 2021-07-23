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


// ���� �⺻ ������ ���� ����ü
struct DATAFILE
{
	char name[FIELNAMELENGTH];
	int bytes;
};

int main(int argc, char* argv[])
{
	int retval;

	// ���� �ʱ�ȭ
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

	// ������ ���ſ� ��� �� ����
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

		// ������ Ŭ���̾�Ʈ ���� ���
		cout << endl << "[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = " << inet_ntoa(clientaddr.sin_addr)
			<< ", ��Ʈ ��ȣ = " << ntohs(clientaddr.sin_port) << endl;

		// --- Ŭ���̾�Ʈ�� ������ ��� ���� ---
		while (1)
		{
			// ���� �б⿡ ����ϴ� ����
			ifstream dataFile;
			DATAFILE dataFileInfo;

			// ������ ������ �۽� �� �� ���̴� ����
			int Slice;
			int count;
			int percent = 0;

			// ���� �̸��� ������ �ޱ� ���� �ݺ���
			while (1)
			{
				// ���� �̸��� �Է� �޾� �б�
				cout << "���� �̸� �Է� : ";
				cin.getline(dataFileInfo.name, 256);

				dataFile.open(dataFileInfo.name, ios::binary);

				if (!dataFile) cout << "���� �ҷ����� ����" << endl << endl;
				else break;
			}

			// ������ ũ�⸦ ��� ���� �۾�
			dataFile.seekg(0, ios::end);
			dataFileInfo.bytes = (int)dataFile.tellg();
			dataFile.seekg(0, ios::beg);

			// ���� �⺻ ���� �۽�
			retval = send(client_sock, (char*)&dataFileInfo, sizeof(dataFileInfo), 0);
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
				retval = send(client_sock, buf, BUFSIZE, 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("���� ���� while(count) ��");
					continue;
				}

				if (percent != ((Slice - count) * 100 / Slice))
				{
					system("cls");

					cout << "�����ϴ� ���� : " << dataFileInfo.name <<
						", �����ϴ� ���� ũ�� : " << dataFileInfo.bytes << " Bytes" << endl;

					cout << endl << "���൵ : " << (Slice - count) * 100 / Slice << "% ";
					percent = (Slice - count) * 100 / Slice;
				}

				--count;
			}

			// ���� ũ�� ���� ���� ���� ������ �۽�

			system("cls");

			cout << "�����ϴ� ���� : " << dataFileInfo.name <<
				", �����ϴ� ���� ũ�� : " << dataFileInfo.bytes << " Bytes" << endl;

			count = dataFileInfo.bytes - (Slice * BUFSIZE);
			dataFile.read(buf, count);

			retval = send(client_sock, buf, count, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("���� ���� ������");
				continue;
			}

			cout << endl << "���൵ : 100% " << endl << endl;

			// ���� ������ �ݱ�
			dataFile.close();
		}
	}

	// closesocket()
	closesocket(client_sock);
	cout << "[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = " << inet_ntoa(clientaddr.sin_addr)
		<< ", ��Ʈ ��ȣ = " << ntohs(clientaddr.sin_port) << endl;

	// closesocket()
	closesocket(listen_sock);

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

