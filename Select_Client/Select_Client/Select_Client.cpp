// Copyright 2020. Kangsan Bae all rights reserved.

#pragma comment ( lib, "ws2_32.lib" )

#define _WINSOCK_DEPRECATED_NO_WARNINGS

// C++ Header
#include <iostream>

// C Header
#include <winsock2.h>

// Custom Header
#include "../../Select_Server/Select_Server/Protocol.h"

// 소켓 함수 오류 출력 후 종료
void err_quit(const char * msg)
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

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char * msg)
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

	std::cout << "[" << msg << "] " << lpMsgBuf << "\n";
	LocalFree(lpMsgBuf);
}

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
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

int main()
{
	char ipAddrStr[20];
	int portNum, packetType;
	int objectType, isApproved, accidentRiskLevel;
	IF_SC100 sc100_Packet;
	IF_SC200 sc200_Packet;

	std::cout << "IP 주소를 입력하세요 : ";
	std::cin >> ipAddrStr;

	std::cout << "포트 번호를 입력하세요 : ";
	std::cin >> portNum;

	std::cout << "패킷 타입을 선택하세요(1 : IF_SC100, 2 : IF_SC200) : ";
	std::cin >> packetType;

	if (packetType == 1)
	{
		std::cout << "오브젝트 ID를 입력하세요 : ";
		std::cin >> sc100_Packet.object_ID;

		std::cout << "오브젝트 타입을 선택하세요(1 : 사람, 2 : 이동 장비, 3 : 미확인 객체) : ";
		std::cin >> objectType;
		sc100_Packet.object_Type = static_cast<unsigned char>(objectType);

		std::cout << "위도 값을 입력하세요(double) : ";
		std::cin >> sc100_Packet.latitude;

		std::cout << "경도 값을 입력하세요(double) : ";
		std::cin >> sc100_Packet.longitude;

		std::cout << "인가 여부를 입력하세요(0 : False, 1 : True) : ";
		std::cin >> isApproved;
		sc100_Packet.isApproved = static_cast<unsigned char>(isApproved);

		std::cout << "사고 위험 레벨을 입력하세요(int) : ";
		std::cin >> accidentRiskLevel;
		sc100_Packet.accidentRiskLevel = static_cast<unsigned char>(accidentRiskLevel);
	}
	else if (packetType == 2)
	{
		std::cout << "구역 이름을 입력하세요 : ";
		std::cin >> sc200_Packet.location_Name;

		std::cout << "사고 위험 분류를 입력하세요(0 : 화재, 1 : 위험물 누출) : ";
		std::cin >> sc200_Packet.accidentRiskType;
	}

	// ==================== Winsock initialize ====================

	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) err_quit("Error : Failed WSAStartup()");

	// ==================== Create socket ====================

	SOCKET mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // TCP 소켓 생성
	if (mySocket == SOCKET_ERROR || mySocket == INVALID_SOCKET) err_quit("Error : Failed TCP socket create");

	// ==================== Connect ====================

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ipAddrStr);
	serveraddr.sin_port = htons(portNum);

	int retval = connect(mySocket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("Error : Failed cennect()");

	// 보낼 데이터
	unsigned char sendBuff[100];
	int sendLen = 0;

	sendBuff[0] = 1;
	sendBuff[1] = packetType;
	if (packetType == 1)
	{
		memcpy_s(&sendBuff[2], sizeof(sendBuff) - 2, &sc100_Packet, sizeof(sc100_Packet));
	}
	else if (packetType == 2)
	{
		memcpy_s(&sendBuff[2], sizeof(sendBuff) - 2, &sc200_Packet, sizeof(sc200_Packet));
	}

	// 서버와 데이터 통신
	while (true)
	{
		// 데이터 입력

		// 데이터 보내기
		retval = send(mySocket, reinterpret_cast<char *>(&sendBuff), sizeof(sendBuff), NULL);
		if (retval == SOCKET_ERROR)
		{
			err_quit("Error : Failed send()");
			continue;
		}

		Sleep(100);
	}

	// closesocket()
	closesocket(mySocket);

	// 윈속 종료
	WSACleanup();
	return 0;
}

