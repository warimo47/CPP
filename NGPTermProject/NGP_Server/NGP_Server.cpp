#include "NGP_Server.h"

// 이벤트 핸들
HANDLE hEventReady[CLIENT_NUM];
HANDLE hEventCreateThread[CLIENT_NUM];
HANDLE hEventComStart;
HANDLE hEventGameEnd[CLIENT_NUM];

// 클라이언트 ID 변수
int ClientsCount;

// [씬 상태 패킷] 및 서버의 씬 상태 변수
PACKET_SceneState sceneState;

// 장애물 포인터 배열
S_Obstacle* obstacles[OBSTACLE_NUM];

// 캐릭터 포인터 배열
S_Character* characters[CLIENT_NUM];

// 발사체 포인터 배열
S_Bullet* bullets[BULLETS_MAX_NUM];

// 발사 시도 변수
AtteckDirection atteckDir[CLIENT_NUM];

int main(int argc, char* argv[])
{
	// 서버 초기화
	Initialize_Server();

	// 오류 확인 변수
	int retval;

	// 시간
	DWORD TickTime = 1;

	// 데이터 통신에 사용할 변수
	int client_count = 0;
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	while (1)
	{
		if (sceneState.SceneState == SceneState::IN_GAME)
		{
			TickTime = CheckTime(TickTime);

			TickTime = Update(TickTime);
		}
		else if (sceneState.SceneState == SceneState::SELECT_CHARACTER)
		{
			// 클라이언트가 [CLIENT_NUM] 개 미만이면
			if (ClientsCount < CLIENT_NUM)
			{
				// accept()
				addrlen = sizeof(clientaddr);
				client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
				if (client_sock == INVALID_SOCKET)
				{
					err_display("accept()");
					break;
				}

				// 스레드 생성
				hThread = CreateThread(NULL, 0, ComClient, (LPVOID)client_sock, 0, NULL);
				if (hThread == NULL) closesocket(client_sock);
				else CloseHandle(hThread);

				// 스레드에 ID 부여를 기다림
				WaitForSingleObject(hEventCreateThread[ClientsCount], INFINITE);
			}
			// 클라이언트가 [CLIENT_NUM] 개 이상이면
			else
			{
				// 모든 클라이언트가 준비하기를 기다림
				WaitForMultipleObjects(CLIENT_NUM, hEventReady, true, INFINITE);
				sceneState.SceneState = SceneState::IN_GAME;
				printf("ServerScene : IN_GAME\n");
			}
		}
		else
		{
			// 클라이언트에 [씬 상태 패킷]이 다 가기를 기다림
			WaitForMultipleObjects(CLIENT_NUM, hEventGameEnd, true, INFINITE);
			
			// 서버 리셋
			ResetServer();
		}
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

// 클라이언트와 데이터 통신 스레드
DWORD WINAPI ComClient(LPVOID arg)
{
	// 패킷 구조체
	PACKET_ClientID clientID;
	PACKET_ChoiceCharacter choiceChar;
	PACKET_Character characterPacket;
	PACKET_PlayerState playerState;
	PACKET_BulletNum bulletNum;
	PACKET_Bullet bullet;

	// 데이터 통신에 사용하는 변수
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;
	int addrlen;

	// 스레드에 ID를 부여
	clientID.ClientID = ClientsCount;
	++ClientsCount;

	// 스레드에 ID 부여함을 알림
	SetEvent(hEventCreateThread[clientID.ClientID]);

	// 오류 체크 변수
	int retval;

	// 패킷 구조체 변수 초기화
	bulletNum.BulletNum = 0;

	// 클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	// --- 클라이언트와 데이터 통신 시작 ---

	printf("%d번 클라이언트와 통신 시작\n", clientID.ClientID);

	// [ID 부여 패킷] 송신
	retval = send(client_sock, (char*)&clientID, sizeof(clientID), 0);
	if (retval == SOCKET_ERROR) err_quit("send() 클라이언트 ID 부여");

	while (1)
	{
		// [캐릭터 선택 패킷] 수신
		retval = recv(client_sock, (char*)&choiceChar, sizeof(choiceChar), 0);
		if (retval == SOCKET_ERROR) err_quit("recv() 캐릭터 선택");

		// 캐릭터 타입 설정
		characters[clientID.ClientID]->SetType(choiceChar.CharacterType);

		// 이 스레드가 [캐릭터 선택 패킷]을 받았다고 알림
		SetEvent(hEventReady[clientID.ClientID]);

		while (1)
		{
			// 클라이언트와 데이터 송수신 과정을 1초당 30번으로 제한하기 위한 이벤트
			WaitForSingleObject(hEventComStart, INFINITE);

			// [씬 상태 패킷] 송신
			retval = send(client_sock, (char*)&sceneState, sizeof(sceneState), 0);
			if (retval == SOCKET_ERROR) err_quit("send() [씬 상태 패킷]");

			// 인 게임 상태가 아니라면 캐릭터 선택으로 돌아감
			if (sceneState.SceneState > SceneState::IN_GAME)
			{
				// 이 스레드가 [씬 상태 패킷]을 보냈다고 알림
				SetEvent(hEventGameEnd[clientID.ClientID]);

				break;
			}

			// 캐릭터 정보 송신
			for (int i = 0; i < CLIENT_NUM; ++i)
			{
				// 서버에 있는 캐릭터 정보를 패킷으로 복사
				characters[i]->SetCharacterPacket(&characterPacket);

				// [캐릭터 정보 패킷] 송신
				retval = send(client_sock, (char*)&characterPacket, sizeof(characterPacket), 0);
				if (retval == SOCKET_ERROR) err_quit("send() [캐릭터 정보 패킷]");
			}

			// 발사체 갯수 확인
			bulletNum.BulletNum = 0;

			for (int i = 0; i < BULLETS_MAX_NUM; ++i)
			{
				if (bullets[i]->GetType() != ObjectsType::NONE)
				{
					++bulletNum.BulletNum;
				}
			}

			// [발사체 개수 패킷] 송신
			retval = send(client_sock, (char*)&bulletNum, sizeof(bulletNum), 0);
			if (retval == SOCKET_ERROR) err_quit("send() [발사체 갯수 패킷]");

			// 발사체 정보 송신
			for (int i = 0; i < BULLETS_MAX_NUM; ++i)
			{
				if (bullets[i]->GetType() != ObjectsType::NONE)
				{
					// 서버에 있는 발사체 정보를 패킷으로 복사
					bullets[i]->SetBulletPacket(&bullet);

					// [발사체 정보 패킷] 송신
					retval = send(client_sock, (char*)&bullet, sizeof(bullet), 0);
					if (retval == SOCKET_ERROR) err_quit("send() [발사체 갯수 패킷]");
				}
			}

			// [플레이어 상태 패킷] 수신
			retval = recv(client_sock, (char*)&playerState, sizeof(playerState), 0);
			if (retval == SOCKET_ERROR) err_quit("recv() [플레이어 상태 패킷]");

			// 플레이어 상태 갱신
			characters[clientID.ClientID]->SetCharacterState(&playerState);

			// 발사 시도 변수 갱신
			atteckDir[clientID.ClientID].X = playerState.AttackDirectionX;
			atteckDir[clientID.ClientID].Y = playerState.AttackDirectionY;
		}
	}

	// closesocket()
	closesocket(client_sock);

	printf("%d번 스레드 통신 종료\n", clientID.ClientID);

	return 0;
}

// 서버 초기화 함수
void Initialize_Server()
{

	// 클라이언트 ID 부여 변수 초기화
	ClientsCount = 0;

	// 서버의 씬 변수 초기화
	sceneState.SceneState = SceneState::SELECT_CHARACTER;

	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		// 자동 리셋, 비신호 시작
		hEventReady[i] = CreateEvent(nullptr, false, false, nullptr);

		// 자동 리셋, 비신호 시작
		hEventCreateThread[i] = CreateEvent(nullptr, false, false, nullptr);

		// 발사 시도 구조체 초기화
		atteckDir[i].X = 0.0f;
		atteckDir[i].Y = 0.0f;

		// 자동 리셋, 비신호 시작
		hEventGameEnd[i] = CreateEvent(nullptr, false, false, nullptr);
	}

	// 수동 리셋, 비신호 시작
	hEventComStart = CreateEvent(nullptr, true, false, nullptr);

	

	// 장애물 위치 지정
	obstacles[0] = new S_Obstacle(384.0f, 66.0f);
	obstacles[1] = new S_Obstacle(384.0f, 132.0f);
	obstacles[2] = new S_Obstacle(384.0f, 198.0f);
	obstacles[3] = new S_Obstacle(384.0f, 264.0f);
	obstacles[4] = new S_Obstacle(240.0f, 198.0f);
	obstacles[5] = new S_Obstacle(912.0f, 132.0f);
	obstacles[6] = new S_Obstacle(1032.0f, 198.0f);
	obstacles[7] = new S_Obstacle(864.0f, 462.0f);
	obstacles[8] = new S_Obstacle(912.0f, 462.0f);
	obstacles[9] = new S_Obstacle(960.0f, 462.0f);
	obstacles[10] = new S_Obstacle(1008.0f, 462.0f);
	obstacles[11] = new S_Obstacle(1056.0f, 462.0f);
	obstacles[12] = new S_Obstacle(1104.0f, 462.0f);
	obstacles[13] = new S_Obstacle(72.0f, 594.0f);
	obstacles[14] = new S_Obstacle(936.0f, 792.0f);

	// 캐릭터 생성
	characters[0] = new S_Character(96.0f, 132.0f);
	characters[1] = new S_Character(912.0f, 264.0f);
	if (CLIENT_NUM == 3)
	{
		characters[2] = new S_Character(576.0f, 858.0f);
	}

	// 발사체 배열 초기화
	for (int i = 0; i < BULLETS_MAX_NUM; ++i)
	{
		bullets[i] = new S_Bullet();
	}
}

// 서버 리셋 함수
void ResetServer()
{
	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		characters[i]->ReSet();
	}
	
	for (int i = 0; i < BULLETS_MAX_NUM; ++i)
	{
		bullets[i]->ReSet();
	}

	sceneState.SceneState = SceneState::SELECT_CHARACTER;
}

// 소켓 함수 오류 출력 후 종료
void err_quit(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPTSTR)&lpMsgBuf, (LPCSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	cout << "[" << msg << "] " << lpMsgBuf << endl;
	LocalFree(lpMsgBuf);
}

DWORD Update(DWORD ticktime)
{
	DWORD oldtime = GetTickCount();
	float MiliSec = (float)ticktime / 1000.0f;

	// 캐릭터 위치 정보 업데이트
	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		if (characters[i]->GetType() == ObjectsType::DIE) continue;

		characters[i]->PositionUpdate(MiliSec);
	}

	// 발사체 위치 정보 업데이트
	for (int i = 0; i < BULLETS_MAX_NUM; ++i)
	{
		if (bullets[i]->GetType() == ObjectsType::NONE) continue;

		bullets[i]->PositionUpdate(MiliSec);
	}

	// 캐릭터 애니메이션
	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		if (characters[i]->CheckAnimTickCount())
		{
			characters[i]->SetAnimCount(characters[i]->GetAnimCount() + 1);

			if (characters[i]->GetAnimCount() == 3 ||
				characters[i]->GetAnimCount() == 5)
			{
				characters[i]->SetAnimCount(0);
			}
		}
	}

	// 발사체 애니메이션
	for (int i = 0; i < BULLETS_MAX_NUM; ++i)
	{
		bullets[i]->BulletAnim();
	}


	// 총알 발사 검사
	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		if ((atteckDir[i].X != 0.0f) || (atteckDir[i].Y != 0.0f))
		{
			characters[i]->SetAnimCount(3);

			for (int j = 0; j < BULLETS_MAX_NUM; ++j)
			{
				if (bullets[j]->GetType() == ObjectsType::NONE)
				{
					bullets[j]->SetBulletState(
						i, characters[i]->GetType(),
						characters[i]->GetPosX() + 12,
						characters[i]->GetPosY() + 21,
						atteckDir[i].X,
						atteckDir[i].Y
					);
					atteckDir[i].X = 0.0f;
					atteckDir[i].Y = 0.0f;
					break;
				}
			}
		}
	}

	// 캐릭터 <-> 장애물 충돌 검사
	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		for (int j = 0; j < OBSTACLE_NUM; ++j)
		{
			if (CC_Char_Obst(characters[i], obstacles[j]))
			{
				// 캐릭터 <-> 장애물 충돌 처리
				characters[i]->BounceCollision(MiliSec);
			}
		}
	}

	// 캐릭터 <-> 캐릭터 충돌 검사
	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		for (int j = i + 1; j < CLIENT_NUM; ++j)
		{
			if (CC_Char_Char(characters[i], characters[j]))
			{
				// 캐릭터 <-> 캐릭터 충돌 처리
				characters[i]->BounceCollision(MiliSec);
				characters[j]->BounceCollision(MiliSec);
			}
		}
	}

	// 캐릭터 <-> 발사체 충돌 검사
	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		for (int j = 0; j < BULLETS_MAX_NUM; ++j)
		{
			if (bullets[j]->GetType() == ObjectsType::NONE) continue;
			if (i == bullets[j]->GetOwnerID()) continue;

			if (CC_Char_Bullet(characters[i], bullets[j]))
			{
				// 캐릭터 <-> 발사체 충돌 처리
				characters[i]->SetLife(
					characters[i]->GetLife() - bullets[j]->GetDamage()
				);
				bullets[j]->SetType(ObjectsType::NONE);

				if (characters[i]->GetLife() < 1)
				{
					characters[i]->SetType(ObjectsType::DIE);
				}
			}
		}
	}

	// 게임 종료 판단
	int DieChar = 0;
	int leavePlayerID = 0;

	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		if (characters[i]->GetType() == ObjectsType::DIE)
		{
			++DieChar;
		}
		else
		{
			leavePlayerID = i;
		}
	}

	if (DieChar > CLIENT_NUM - 2)
	{
		sceneState.SceneState = leavePlayerID + 2;
	}

	// 클라이언트와 데이터 송수신 하라고 알림
	SetEvent(hEventComStart);
	ResetEvent(hEventComStart);

	DWORD nowtime = GetTickCount();
	return (nowtime - oldtime);
}

DWORD CheckTime(DWORD tick)
{
	// printf("tick : %d\n", tick);
	if (tick > 33) return tick;
	else
	{
		Sleep(33 - tick);
		return 33;
	}
}

// 캐릭터 <-> 장애물 충돌 검사
bool CC_Char_Obst(S_Character* char_a, S_Obstacle* obst_b)
{
	if (char_a->GetPosX() > obst_b->GetRight()) return false;
	if (char_a->GetPosY() > obst_b->GetBottom()) return false;

	if (char_a->GetRight() < obst_b->GetPosX()) return false;
	if (char_a->GetBottom() < obst_b->GetPosY()) return false;

	if (obst_b->GetPosX() > char_a->GetRight()) return false;
	if (obst_b->GetPosY() > char_a->GetBottom()) return false;

	if (obst_b->GetRight() < char_a->GetPosX()) return false;
	if (obst_b->GetBottom() < char_a->GetPosY()) return false;

	return true;
}

// 캐릭터 <-> 캐릭터 충돌 검사
bool CC_Char_Char(S_Character* char_a, S_Character* char_b)
{
	if (char_a->GetPosX() > char_b->GetRight()) return false;
	if (char_a->GetPosY() > char_b->GetBottom()) return false;

	if (char_a->GetRight() < char_b->GetPosX()) return false;
	if (char_a->GetBottom() < char_b->GetPosY()) return false;

	if (char_b->GetPosX() > char_a->GetRight()) return false;
	if (char_b->GetPosY() > char_a->GetBottom()) return false;

	if (char_b->GetRight() < char_a->GetPosX()) return false;
	if (char_b->GetBottom() < char_a->GetPosY()) return false;

	return true;
}

// 캐릭터 <-> 발사체 충돌 검사
bool CC_Char_Bullet(S_Character* character, S_Bullet* bullet)
{
	if (character->GetPosX() > bullet->GetRight()) return false;
	if (character->GetPosY() > bullet->GetBottom()) return false;

	if (character->GetRight() < bullet->GetPosX()) return false;
	if (character->GetBottom() < bullet->GetPosY()) return false;

	if (bullet->GetPosX() > character->GetRight()) return false;
	if (bullet->GetPosY() > character->GetBottom()) return false;

	if (bullet->GetRight() < character->GetPosX()) return false;
	if (bullet->GetBottom() < character->GetPosY()) return false;

	return true;
}