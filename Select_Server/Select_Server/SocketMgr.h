// Copyright 2020. Kangsan Bae all rights reserved.

#pragma once

// C Header
#include <winsock2.h>

// ���� ���� ������ ���� ����ü
struct SOCKETINFO
{
	SOCKET sock;
	BYTE recvBuf[100];
	BYTE sendBuf;
	int recvBytes;
	int sendBytes;
	int PC_ID;
};

class CSocketMgr
{
public:
	CSocketMgr();
	~CSocketMgr();
	void initialize();
	void tick();
	BOOL AddSocketInfo(SOCKET);
	void RemoveSocketInfo(int);

	void printPacket(SOCKETINFO*, unsigned char ptr[]);

protected:

private:

public:
	// ���� ���� ����
	int mTotalSockets;
	SOCKETINFO* mSocketInfoArray[FD_SETSIZE];
	SOCKET Listen_Socket;

protected:

private:
};

