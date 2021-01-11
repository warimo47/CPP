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

// [ID 부여 패킷]
struct PACKET_ClientID
{
	BYTE ClientID;
};

// [발사체 개수 패킷]
struct PACKET_BulletNum
{
	BYTE BulletNum;
};

// [캐릭터 선택 패킷]
struct PACKET_ChoiceCharacter
{
	BYTE CharacterType;
};

// [플레이어 상태 패킷]
struct PACKET_PlayerState
{
	BYTE DirectionState;
	BYTE IsMoving;
	float AttackDirectionX;
	float AttackDirectionY;
};

// 윈도우 메시지 콜벡 함수
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 소켓 함수 오류 출력 후 종료
void err_quit(char* msg);

// 소켓 함수 오류 출력
void err_display(char *msg);

// 서버와 데이터 통신 스레드
DWORD WINAPI ComServer(LPVOID);

// 키 입력 처리 함수
void ProcessKeyDown(WPARAM);
void ProcessKeyUp(WPARAM);