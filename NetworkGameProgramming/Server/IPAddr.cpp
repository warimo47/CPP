#pragma comment(lib, "ws2_32")
#include <winSock2.h>
#include <ws2tcpip.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 원래의 IPv4 주소출력
	char* ipv4test = "147, 46, 114, 70";
	cout << "IPv4 주소(변환 전) = " << ipv4test << endl;

	// inet_addr() 함수연습
	PVOID adress = NULL;
	InetPton(AF_INET, ipv4test, adress);
	cout << "IPv4 주소(변환 후) = 0x" << hex << adress << endl;

	// inet_ntoa() 함수연습
	IN_ADDR ipv4num;
	ipv4num.s_addr = inet_addr(ipv4test);
//	cout << "IPv4 주소(다시 변환 후) = " << inet_ntoa(ipv4num) << endl << endl;

	// 원래의 IP

	return 0;
}