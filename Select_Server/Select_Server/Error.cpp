// Copyright 2020. Kangsan Bae all rights reserved.

// Default Header
#include "Error.h"

// C++ Header
#include <iostream>

// C Header
#include <winsock2.h>

void Err_Msg(const char* msg, bool option)
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