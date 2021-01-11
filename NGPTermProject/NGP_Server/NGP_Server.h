#pragma once

#pragma comment(lib, "ws2_32")

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <stdlib.h>
#include "S_Character.h"

#define SERVERPORT 9000

#define CLIENT_NUM 2
#define BULLETS_MAX_NUM 100

enum SceneState { SELECT_CHARACTER, IN_GAME, PLAYER1WIN, PLAYER2WIN, PLAYER3WIN };

// [ID �ο� ��Ŷ]
struct PACKET_ClientID
{
	BYTE ClientID;
};

// [�� ���� ��Ŷ]
struct PACKET_SceneState
{
	BYTE SceneState;
};

// [ �߻�ü ���� ��Ŷ ]
struct PACKET_BulletNum
{
	BYTE BulletNum;
};

// [ĳ���� ���� ��Ŷ]
struct PACKET_ChoiceCharacter
{
	BYTE CharacterType;
};

// �߻� �õ� ����ü
struct AtteckDirection
{
	float X;
	float Y;
};

// Ŭ���̾�Ʈ�� ������ ��� ������
DWORD WINAPI ComClient(LPVOID arg);

// ���� �ʱ�ȭ �Լ�
void Initialize_Server();

// ���� ���� �Լ�
void ResetServer();

// ���� �Լ� ���� ��� �� ����
void err_quit(char* msg);

// ���� �Լ� ���� ���
void err_display(char *msg);

DWORD Update(DWORD ticktime);

DWORD CheckTime(DWORD tick);

// ĳ���� <-> ��ֹ� �浹 �˻�
bool CC_Char_Obst(S_Character*, S_Obstacle*);

// ĳ���� <-> ĳ���� �浹 �˻�
bool CC_Char_Char(S_Character*, S_Character*);

// ĳ���� <-> �߻�ü �浹 �˻�
bool CC_Char_Bullet(S_Character*, S_Bullet*);