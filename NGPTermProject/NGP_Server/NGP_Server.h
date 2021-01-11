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

// [ID 부여 패킷]
struct PACKET_ClientID
{
	BYTE ClientID;
};

// [씬 상태 패킷]
struct PACKET_SceneState
{
	BYTE SceneState;
};

// [ 발사체 개수 패킷 ]
struct PACKET_BulletNum
{
	BYTE BulletNum;
};

// [캐릭터 선택 패킷]
struct PACKET_ChoiceCharacter
{
	BYTE CharacterType;
};

// 발사 시도 구조체
struct AtteckDirection
{
	float X;
	float Y;
};

// 클라이언트와 데이터 통신 스레드
DWORD WINAPI ComClient(LPVOID arg);

// 서버 초기화 함수
void Initialize_Server();

// 서버 리셋 함수
void ResetServer();

// 소켓 함수 오류 출력 후 종료
void err_quit(char* msg);

// 소켓 함수 오류 출력
void err_display(char *msg);

DWORD Update(DWORD ticktime);

DWORD CheckTime(DWORD tick);

// 캐릭터 <-> 장애물 충돌 검사
bool CC_Char_Obst(S_Character*, S_Obstacle*);

// 캐릭터 <-> 캐릭터 충돌 검사
bool CC_Char_Char(S_Character*, S_Character*);

// 캐릭터 <-> 발사체 충돌 검사
bool CC_Char_Bullet(S_Character*, S_Bullet*);