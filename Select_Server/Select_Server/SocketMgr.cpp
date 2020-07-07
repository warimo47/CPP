// Copyright 2020. Kangsan Bae all rights reserved.

#pragma comment ( lib, "ws2_32.lib" )

#define _WINSOCK_DEPRECATED_NO_WARNINGS

// Default Header
#include "SocketMgr.h"

// C++ Header
#include <iostream>
#include <ctime>

// C Header
#include <winsock2.h>

// Custeom Header
#include "Error.h"
#include "Protocol.h"

CSocketMgr::CSocketMgr()
{
	mTotalSockets = 0;

	for (int i = 0; i < FD_SETSIZE; ++i)
	{
		mSocketInfoArray[i] = nullptr;
	}

	Listen_Socket = SOCKET_ERROR;
}

CSocketMgr::~CSocketMgr() {}

void CSocketMgr::initialize()
{
	// ==================== Winsock initialize ====================

	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) Err_Msg("Error : Failed WSAStartup()", true);

	// ==================== Create socket ====================

	Listen_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // TCP 家南 积己

	if (Listen_Socket == SOCKET_ERROR || Listen_Socket == INVALID_SOCKET) Err_Msg("Error : Failed TCP socket create", true);

	// ==================== None blocking 家南栏肺 函券 ====================

	u_long sock_on = 1;
	int retval = ioctlsocket(Listen_Socket, FIONBIO, &sock_on);
	if (retval == SOCKET_ERROR) Err_Msg("Error : Failed ioctlsocket()", true);

	// ==================== 家南 可记 汲沥 ====================

	BOOL optReuse = TRUE;
	retval = setsockopt(Listen_Socket, SOL_SOCKET, SO_REUSEADDR, (char *)&optReuse, sizeof(optReuse));
	if (retval == SOCKET_ERROR) Err_Msg("Error : Failed setsockopt()", true);

	// ==================== Bind ====================

	SOCKADDR_IN Server_addr;
	ZeroMemory(&Server_addr, sizeof(Server_addr));
	Server_addr.sin_family = AF_INET;
	Server_addr.sin_addr.s_addr = INADDR_ANY;
	Server_addr.sin_port = htons(4211);

	retval = bind(Listen_Socket, (SOCKADDR*)&Server_addr, sizeof(Server_addr));
	if (retval == SOCKET_ERROR) Err_Msg("Error : Failed TCP socket binding", true);

	// ==================== Listen ====================

	retval = listen(Listen_Socket, SOMAXCONN);
	if (retval == SOCKET_ERROR) Err_Msg("Error : Failed listen()", true);

	std::cout << "Waiting for connencting from Client...\n";
}

void CSocketMgr::tick()
{
	FD_SET rset;
	FD_SET wset;

	SOCKET Client_Socket;
	SOCKADDR_IN Client_addr;
	int Client_addr_len = sizeof(Client_addr);

	int retval = 0;

	while (true)
	{
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		FD_SET(Listen_Socket, &rset);

		for (int i = 0; i < mTotalSockets; ++i)
		{
			FD_SET(mSocketInfoArray[i]->sock, &rset);
		}

		// ==================== Select ====================

		retval = select(NULL, &rset, &wset, NULL, NULL);
		if (retval == SOCKET_ERROR) Err_Msg("Error : Failed select()", false);

		if (FD_ISSET(Listen_Socket, &rset))
		{
			// ==================== Accept ====================

			Client_Socket = accept(Listen_Socket, (SOCKADDR*)&Client_addr, &Client_addr_len);
			if (Client_Socket == SOCKET_ERROR)
			{
				Err_Msg("Error : Failed accept() socket error", false);
			}
			else if (Client_Socket == INVALID_SOCKET)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK) Err_Msg("Error : Failed accept() invalid socket", false);
			}
			else
			{
				std::cout << "\n[Server] Client connet : IP Address : " << inet_ntoa(Client_addr.sin_addr) << "\tPort : " << ntohs(Client_addr.sin_port) << "\n";

				if (AddSocketInfo(Client_Socket) == FALSE)
				{
					closesocket(Client_Socket);
					std::cout << "\n[Server] Client Disconnet : IP Address : " << inet_ntoa(Client_addr.sin_addr) << "\tPort : " << ntohs(Client_addr.sin_port) << "\n";
				}
			}
		}

		// ==================== Recv ====================

		for (int i = 0; i < mTotalSockets; ++i)
		{
			SOCKETINFO* socketPtr = mSocketInfoArray[i];

			if (FD_ISSET(socketPtr->sock, &rset))
			{
				retval = recv(socketPtr->sock, (char*)socketPtr->recvBuf, sizeof(socketPtr->recvBuf), NULL);
				
				if (retval == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
				{
					Err_Msg("[Error : Failed recv]", false);
					RemoveSocketInfo(i);
					continue;
				}

				printPacket(socketPtr, socketPtr->recvBuf);
			}
		}
	}
}

BOOL CSocketMgr::AddSocketInfo(SOCKET sock)
{
	if (mTotalSockets >= (FD_SETSIZE - 1))
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
	ptr->PC_ID = mTotalSockets;

	mSocketInfoArray[mTotalSockets] = ptr;
	++mTotalSockets;

	return TRUE;
}

void CSocketMgr::RemoveSocketInfo(int nIndex)
{
	SOCKETINFO* ptr = mSocketInfoArray[nIndex];

	SOCKADDR_IN Client_addr;
	int Client_addr_len = sizeof(Client_addr);

	getpeername(ptr->sock, (SOCKADDR*)&Client_addr, &Client_addr_len);

	std::cout << "\n[Server] Client End : IP Address : "
		<< inet_ntoa(Client_addr.sin_addr) << "\t Port : "
		<< ntohs(Client_addr.sin_port) << "\n";

	closesocket(ptr->sock);
	delete ptr;

	for (int i = nIndex; i < mTotalSockets; ++i)
	{
		mSocketInfoArray[i] = mSocketInfoArray[i + 1];
	}

	mTotalSockets--;
}

void CSocketMgr::printPacket(SOCKETINFO* sock, unsigned char ptr[])
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