// Copyright 2021. Kangsan Bae all rights reserved. warimo47@naver.com

#pragma comment ( lib, "ws2_32.lib" )

// #define _WINSOCK_DEPRECATED_NO_WARNINGS

// C++ Header
#include <iostream>
#include <ctime>

// C Header
#include <winsock2.h>

// Structure for socket Information
struct SOCKETINFO
{
	SOCKET sock;
	BYTE recvBuf[100];
	BYTE sendBuf;
	int recvBytes;
	int sendBytes;
	int PC_ID;
};

// Global values
int gTotalSockets = 0;
SOCKETINFO* gSocketInfoArray[FD_SETSIZE];
SOCKET gListenSocket;

// Functions
void ErrMsg(const char* msg, bool option);
BOOL AddSocketInfo(SOCKET sock);
void RemoveSocketInfo(int nIndex);
void PrintPacket(SOCKETINFO* sock, unsigned char ptr[]);

int main()
{
	// ==================== Winsock initialize ====================

	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) ErrMsg("Error : Failed WSAStartup()", true);

	// ==================== Create socket ====================

	gListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (gListenSocket == SOCKET_ERROR || gListenSocket == INVALID_SOCKET) ErrMsg("Error : Failed TCP socket create", true);

	// ==================== Set none blocking socket option ====================

	u_long sock_on = 1;
	int retval = ioctlsocket(gListenSocket, FIONBIO, &sock_on);
	if (retval == SOCKET_ERROR) ErrMsg("Error : Failed ioctlsocket()", true);

	// ==================== Set socket options ====================

	BOOL optReuse = TRUE;
	retval = setsockopt(gListenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optReuse, sizeof(optReuse));
	if (retval == SOCKET_ERROR) ErrMsg("Error : Failed setsockopt()", true);

	// ==================== Bind ====================

	SOCKADDR_IN Server_addr;
	ZeroMemory(&Server_addr, sizeof(Server_addr));
	Server_addr.sin_family = AF_INET;
	Server_addr.sin_addr.s_addr = INADDR_ANY;
	Server_addr.sin_port = htons(4211);

	retval = bind(gListenSocket, (SOCKADDR*)&Server_addr, sizeof(Server_addr));
	if (retval == SOCKET_ERROR) ErrMsg("Error : Failed TCP socket binding", true);

	// ==================== Listen ====================

	retval = listen(gListenSocket, SOMAXCONN);
	if (retval == SOCKET_ERROR) ErrMsg("Error : Failed listen()", true);

	std::cout << "Waiting for connencting from Client...\n";

	// ==================== Main loop ====================

	FD_SET rset;
	FD_SET wset;

	SOCKET ClientSocket;
	SOCKADDR_IN ClientAddr;
	int ClientAddrLen = sizeof(ClientAddr);

	while (true)
	{
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		FD_SET(gListenSocket, &rset);

		for (int i = 0; i < gTotalSockets; ++i)
		{
			FD_SET(gSocketInfoArray[i]->sock, &rset);
		}

		// ==================== Select ====================

		retval = select(NULL, &rset, &wset, NULL, NULL);
		if (retval == SOCKET_ERROR) ErrMsg("Error : Failed select()", false);

		if (FD_ISSET(gListenSocket, &rset))
		{
			// ==================== Accept ====================

			ClientSocket = accept(gListenSocket, (SOCKADDR*)&ClientAddr, &ClientAddrLen);
			if (ClientSocket == SOCKET_ERROR)
			{
				ErrMsg("Error : Failed accept() socket error", false);
			}
			else if (ClientSocket == INVALID_SOCKET)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK) ErrMsg("Error : Failed accept() invalid socket", false);
			}
			else
			{
				std::cout << "\n[Server] Client connet : IP Address : " << inet_ntoa(ClientAddr.sin_addr) << "\tPort : " << ntohs(ClientAddr.sin_port) << "\n";

				if (AddSocketInfo(ClientSocket) == FALSE)
				{
					closesocket(ClientSocket);
					std::cout << "\n[Server] Client Disconnet : IP Address : " << inet_ntoa(ClientAddr.sin_addr) << "\tPort : " << ntohs(ClientAddr.sin_port) << "\n";
				}
			}
		}

		// ==================== Recv ====================

		for (int i = 0; i < gTotalSockets; ++i)
		{
			SOCKETINFO* socketPtr = gSocketInfoArray[i];

			if (FD_ISSET(socketPtr->sock, &rset))
			{
				retval = recv(socketPtr->sock, (char*)socketPtr->recvBuf, sizeof(socketPtr->recvBuf), NULL);

				if (retval == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
				{
					ErrMsg("[Error : Failed recv]", false);
					RemoveSocketInfo(i);
					continue;
				}

				PrintPacket(socketPtr, socketPtr->recvBuf);
			}
		}
	}

	return 0;
}

void ErrMsg(const char* msg, bool option)
{
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL);

	if (option)
	{
		MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
		LocalFree(lpMsgBuf);
		exit(-1);
		return;
	}
	else
	{
		std::cout << "[" << msg << "] " << lpMsgBuf << "\n";
		LocalFree(lpMsgBuf);
	}
}

BOOL AddSocketInfo(SOCKET sock)
{
	if (gTotalSockets >= (FD_SETSIZE - 1))
	{
		std::cout << "Error : Can not add socket information\n";
		return FALSE;
	}

	SOCKETINFO* ptr = new SOCKETINFO;
	if (ptr == nullptr)
	{
		std::cout << "Error : Fail new SOCKETINFO\n";
		return FALSE;
	}

	ptr->sock = sock;
	ptr->recvBytes = 0;
	ptr->sendBytes = 0;
	ptr->sendBuf = -1;
	ptr->PC_ID = gTotalSockets;

	gSocketInfoArray[gTotalSockets] = ptr;
	++gTotalSockets;

	return TRUE;
}

void RemoveSocketInfo(int nIndex)
{
	SOCKETINFO* ptr = gSocketInfoArray[nIndex];

	SOCKADDR_IN ClientAddr;
	int ClientAddrLen = sizeof(ClientAddr);

	getpeername(ptr->sock, (SOCKADDR*)&ClientAddr, &ClientAddrLen);

	std::cout << "\n[Server] Client End : IP Address : "
		<< inet_ntoa(ClientAddr.sin_addr) << "\t Port : "
		<< ntohs(ClientAddr.sin_port) << "\n";

	closesocket(ptr->sock);
	delete ptr;

	for (int i = nIndex; i < gTotalSockets; ++i)
	{
		gSocketInfoArray[i] = gSocketInfoArray[i + 1];
	}

	gTotalSockets--;
}

void PrintPacket(SOCKETINFO* sock, unsigned char ptr[])
{
	IF_SC100* pIF_SC100 = nullptr;
	IF_SC200* pIF_SC200 = nullptr;

	switch (static_cast<int>(ptr[1]))
	{
	case 1:
		pIF_SC100 = reinterpret_cast<IF_SC100*>(&ptr[2]);
		std::cout << "[" << sock->PC_ID << "] Object ID : " << pIF_SC100->object_ID << " | Object Type : " << static_cast<int>(pIF_SC100->object_Type) << " | Latitude : ";
		std::cout.precision(6);
		std::cout << std::fixed << pIF_SC100->latitude << " | Longitude : ";
		std::cout.precision(6);
		std::cout << std::fixed << pIF_SC100->longitude << " | " << static_cast<int>(pIF_SC100->isApproved) << " " << static_cast<int>(pIF_SC100->accidentRiskLevel) << "\n";
		break;
	case 2:
		pIF_SC200 = reinterpret_cast<IF_SC200*>(&ptr[2]);
		std::cout << "[" << sock->PC_ID << "] Location Name : " << pIF_SC200->location_Name << " | ART : " << pIF_SC200->accidentRiskType << "\n";
		break;
	default:
		break;
	}

}