#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <iostream>

using namespace std;

BOOL IsLittleEndian()
{
	unsigned short exBigShort = 0x1234;
	unsigned short temp = 0x3412;

	printf("\n%04x  ->  %04x\n", exBigShort, htons(exBigShort));

	if (temp == htons(exBigShort))
	{
		printf("\nLittleEndian : true \n");
		return true;
	}
	else
	{
		printf("\nLittleEndian : false \n");
		return false;
	}
}

BOOL IsBigEndian()
{
	unsigned short exBigShort = 0x1234;

	printf("\n%04x  ->  %04x\n", exBigShort, htons(exBigShort));

	if (exBigShort == htons(exBigShort))
	{
		printf("\nBigEndian : true \n\n");
		return true;
	}
	else
	{
		printf("\nBigEndian : false \n\n");
		return false;
	}
}

int main(int argc, char *argv[])
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	u_short x1 = 0x1234;
	u_long y1 = 0x12345678;
	u_short x2 = htons(x1);
	u_long y2 = htonl(y1);

	// ȣ��Ʈ����Ʈ -> ��Ʈ��ũ ����Ʈ
	printf("[ȣ��Ʈ ����Ʈ -> ��Ʈ��ũ ����Ʈ]\n");
	printf("0x%x -> 0x%x\n", x1, x2);
	printf("0x%x -> 0x%x\n", y1, y2);

	printf("0x%x -> 0x%x -> 0x%x\n", x2, htons(x2), htons(htons(x2)));
	printf("0x%x -> 0x%x -> 0x%x\n", y2, htonl(y2), htonl(htonl(y2)));

	// ��Ʈ��ũ ����Ʈ -> ȣ��Ʈ ����Ʈ
	printf("\n[��Ʈ��ũ ����Ʈ -> ȣ��Ʈ ����Ʈ]\n");
	printf("0x%x -> 0x%x\n", x2, ntohs(x2));
	printf("0x%x -> 0x%x\n", y2, ntohl(y2));

	printf("0x%x -> 0x%x -> 0x%x\n", x2, ntohs(x2), ntohs(ntohs(x2)));
	printf("0x%x -> 0x%x -> 0x%x\n", y2, ntohl(y2), ntohl(ntohl(y2)));

	// �߸��� ��� ��
	printf("\n[�߸��� ��� ��]\n");
	printf("0x%x -> 0x%x\n", x1, htonl(x1));

	IsLittleEndian();
	IsBigEndian();

	WSACleanup();

	return 0;
}