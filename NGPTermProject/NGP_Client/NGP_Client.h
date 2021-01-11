#pragma once

#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")

#pragma comment(lib, "ws2_32")

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <stdlib.h>

#include "SceneManager.h"

#define SERVERPORT	9000
#define BUFSIZE		512

#define MAX_LOADSTRING	100
#define TILE_WIDTH		48
#define TILE_HEIGHT		66

#define SERVERPORT	9000
#define BUFSIZE		512

enum DirectionState { DOWN_LEFT, LEFT, UP_LEFT, DOWN, UP, DOWN_RIGHT, RIGHT, UP_RIGHT };

// [ID �ο� ��Ŷ]
struct PACKET_ClientID
{
	BYTE ClientID;
};

// [�߻�ü ���� ��Ŷ]
struct PACKET_BulletNum
{
	BYTE BulletNum;
};

// [ĳ���� ���� ��Ŷ]
struct PACKET_ChoiceCharacter
{
	BYTE CharacterType;
};

// [�÷��̾� ���� ��Ŷ]
struct PACKET_PlayerState
{
	BYTE DirectionState;
	BYTE IsMoving;
	float AttackDirectionX;
	float AttackDirectionY;
};

// ������ �޽��� �ݺ� �Լ�
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ���� �Լ� ���� ��� �� ����
void err_quit(char* msg);

// ���� �Լ� ���� ���
void err_display(char *msg);

// ������ ������ ��� ������
DWORD WINAPI ComServer(LPVOID);

// Ű �Է� ó�� �Լ�
void ProcessKeyDown(WPARAM);
void ProcessKeyUp(WPARAM);