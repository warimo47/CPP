#pragma comment(lib, "ws2_32")
#include <winSock2.h>
#include <ws2tcpip.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// ������ IPv4 �ּ����
	char* ipv4test = "147, 46, 114, 70";
	cout << "IPv4 �ּ�(��ȯ ��) = " << ipv4test << endl;

	// inet_addr() �Լ�����
	PVOID adress = NULL;
	InetPton(AF_INET, ipv4test, adress);
	cout << "IPv4 �ּ�(��ȯ ��) = 0x" << hex << adress << endl;

	// inet_ntoa() �Լ�����
	IN_ADDR ipv4num;
	ipv4num.s_addr = inet_addr(ipv4test);
//	cout << "IPv4 �ּ�(�ٽ� ��ȯ ��) = " << inet_ntoa(ipv4num) << endl << endl;

	// ������ IP

	return 0;
}