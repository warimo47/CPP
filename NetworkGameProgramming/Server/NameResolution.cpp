#pragma comment(lib, "ws2_32")

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

using namespace std;

void IPNumToStr(char* str)
{
	IN_ADDR* string = (IN_ADDR*)str;

	cout << (USHORT)string->S_un.S_un_b.s_b1 << "."
		<< (USHORT)string->S_un.S_un_b.s_b2 << "."
		<< (USHORT)string->S_un.S_un_b.s_b3 << "."
		<< (USHORT)string->S_un.S_un_b.s_b4 << endl;
}

void CheckAddrType(short num)
{
	cout << "주소 타입 \t\t: ";
	if (num == AF_INET) cout << "AF_INET";
	else if (num == AF_INET6) cout << "AF_INET6";
	else cout << "Error";
	cout << endl;
}

void CheckAddrLength(short num)
{
	cout << "IP 주소 길이 \t\t: ";
	if (num == 4) cout << "4(IPv4)";
	else if (num == 16) cout << "16(IPv6)";
	else cout << "Error";
	cout << endl;
}

void PrintInfo(HOSTENT* ptr)
{
	//-----------IPv4---------//

	if (ptr == NULL) return;

	cout << endl << "공식 도메인 이름 \t: " << ptr->h_name << endl;
	
	CheckAddrType(ptr->h_addrtype);

	CheckAddrLength(ptr->h_length);
	
	cout << "IPv4 주소 \t\t: ";
	IPNumToStr(ptr->h_addr);
	cout << endl << endl;

	for (int i = 0; ptr->h_aliases[i] != NULL; ++i)
	{
		cout << "별명 리스트 [" << i << "] \t: " << ptr->h_aliases[i] << endl;
	}
	
	cout << endl;

	for (int i = 0; ptr->h_addr_list[i] != NULL; ++i)
	{
		cout << "IPv4 주소 리스트 [" << i << "] \t: ";
		IPNumToStr(ptr->h_addr_list[i]);
		cout << endl;
	}

	cout << endl;
}

void domainToIPv4(char* str, IN_ADDR* ipv4)
{
	char* tempStr;

	strtok_s(str, ".", &tempStr);

	ipv4->S_un.S_un_b.s_b1 = atoi(str);

	strtok_s(tempStr, ".", &str);

	ipv4->S_un.S_un_b.s_b2 = atoi(tempStr);

	strtok_s(str, ".", &tempStr);

	ipv4->S_un.S_un_b.s_b3 = atoi(str);

	strtok_s(tempStr, ".", &str);

	ipv4->S_un.S_un_b.s_b4 = atoi(tempStr);

	ipv4->S_un.S_un_w.s_w1 = ipv4->S_un.S_un_b.s_b2 * 256 + ipv4->S_un.S_un_b.s_b1;
	ipv4->S_un.S_un_w.s_w2 = ipv4->S_un.S_un_b.s_b4 * 256 + ipv4->S_un.S_un_b.s_b3;

	ipv4->S_un.S_addr = ipv4->S_un.S_un_w.s_w2 * (256 * 256) + ipv4->S_un.S_un_w.s_w1;
}

int main(int argc, char* argv[])
{
	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	char domainOrIPv4[256];
	IN_ADDR IPv4;
	HOSTENT* ptr;
	

	while (1)
	{
		cout << "도메인 이름 또는 IPv4 입력 : ";

		cin >> domainOrIPv4;

		if ('0' <= domainOrIPv4[0] && domainOrIPv4[0] <= '9')
		{
			domainToIPv4(domainOrIPv4, &IPv4);

			ptr = gethostbyaddr((char*)&IPv4, sizeof(IPv4), AF_INET);
		}
		else
		{
			ptr = gethostbyname(domainOrIPv4);
		}

		PrintInfo(ptr);
	}

	WSACleanup();

	return 0;
}