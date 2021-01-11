#include "NGP_Server.h"

// �̺�Ʈ �ڵ�
HANDLE hEventReady[CLIENT_NUM];
HANDLE hEventCreateThread[CLIENT_NUM];
HANDLE hEventComStart;
HANDLE hEventGameEnd[CLIENT_NUM];

// Ŭ���̾�Ʈ ID ����
int ClientsCount;

// [�� ���� ��Ŷ] �� ������ �� ���� ����
PACKET_SceneState sceneState;

// ��ֹ� ������ �迭
S_Obstacle* obstacles[OBSTACLE_NUM];

// ĳ���� ������ �迭
S_Character* characters[CLIENT_NUM];

// �߻�ü ������ �迭
S_Bullet* bullets[BULLETS_MAX_NUM];

// �߻� �õ� ����
AtteckDirection atteckDir[CLIENT_NUM];

int main(int argc, char* argv[])
{
	// ���� �ʱ�ȭ
	Initialize_Server();

	// ���� Ȯ�� ����
	int retval;

	// �ð�
	DWORD TickTime = 1;

	// ������ ��ſ� ����� ����
	int client_count = 0;
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;

	// ���� �ʱ�ȭ
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
			// Ŭ���̾�Ʈ�� [CLIENT_NUM] �� �̸��̸�
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

				// ������ ����
				hThread = CreateThread(NULL, 0, ComClient, (LPVOID)client_sock, 0, NULL);
				if (hThread == NULL) closesocket(client_sock);
				else CloseHandle(hThread);

				// �����忡 ID �ο��� ��ٸ�
				WaitForSingleObject(hEventCreateThread[ClientsCount], INFINITE);
			}
			// Ŭ���̾�Ʈ�� [CLIENT_NUM] �� �̻��̸�
			else
			{
				// ��� Ŭ���̾�Ʈ�� �غ��ϱ⸦ ��ٸ�
				WaitForMultipleObjects(CLIENT_NUM, hEventReady, true, INFINITE);
				sceneState.SceneState = SceneState::IN_GAME;
				printf("ServerScene : IN_GAME\n");
			}
		}
		else
		{
			// Ŭ���̾�Ʈ�� [�� ���� ��Ŷ]�� �� ���⸦ ��ٸ�
			WaitForMultipleObjects(CLIENT_NUM, hEventGameEnd, true, INFINITE);
			
			// ���� ����
			ResetServer();
		}
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}

// Ŭ���̾�Ʈ�� ������ ��� ������
DWORD WINAPI ComClient(LPVOID arg)
{
	// ��Ŷ ����ü
	PACKET_ClientID clientID;
	PACKET_ChoiceCharacter choiceChar;
	PACKET_Character characterPacket;
	PACKET_PlayerState playerState;
	PACKET_BulletNum bulletNum;
	PACKET_Bullet bullet;

	// ������ ��ſ� ����ϴ� ����
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;
	int addrlen;

	// �����忡 ID�� �ο�
	clientID.ClientID = ClientsCount;
	++ClientsCount;

	// �����忡 ID �ο����� �˸�
	SetEvent(hEventCreateThread[clientID.ClientID]);

	// ���� üũ ����
	int retval;

	// ��Ŷ ����ü ���� �ʱ�ȭ
	bulletNum.BulletNum = 0;

	// Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	// --- Ŭ���̾�Ʈ�� ������ ��� ���� ---

	printf("%d�� Ŭ���̾�Ʈ�� ��� ����\n", clientID.ClientID);

	// [ID �ο� ��Ŷ] �۽�
	retval = send(client_sock, (char*)&clientID, sizeof(clientID), 0);
	if (retval == SOCKET_ERROR) err_quit("send() Ŭ���̾�Ʈ ID �ο�");

	while (1)
	{
		// [ĳ���� ���� ��Ŷ] ����
		retval = recv(client_sock, (char*)&choiceChar, sizeof(choiceChar), 0);
		if (retval == SOCKET_ERROR) err_quit("recv() ĳ���� ����");

		// ĳ���� Ÿ�� ����
		characters[clientID.ClientID]->SetType(choiceChar.CharacterType);

		// �� �����尡 [ĳ���� ���� ��Ŷ]�� �޾Ҵٰ� �˸�
		SetEvent(hEventReady[clientID.ClientID]);

		while (1)
		{
			// Ŭ���̾�Ʈ�� ������ �ۼ��� ������ 1�ʴ� 30������ �����ϱ� ���� �̺�Ʈ
			WaitForSingleObject(hEventComStart, INFINITE);

			// [�� ���� ��Ŷ] �۽�
			retval = send(client_sock, (char*)&sceneState, sizeof(sceneState), 0);
			if (retval == SOCKET_ERROR) err_quit("send() [�� ���� ��Ŷ]");

			// �� ���� ���°� �ƴ϶�� ĳ���� �������� ���ư�
			if (sceneState.SceneState > SceneState::IN_GAME)
			{
				// �� �����尡 [�� ���� ��Ŷ]�� ���´ٰ� �˸�
				SetEvent(hEventGameEnd[clientID.ClientID]);

				break;
			}

			// ĳ���� ���� �۽�
			for (int i = 0; i < CLIENT_NUM; ++i)
			{
				// ������ �ִ� ĳ���� ������ ��Ŷ���� ����
				characters[i]->SetCharacterPacket(&characterPacket);

				// [ĳ���� ���� ��Ŷ] �۽�
				retval = send(client_sock, (char*)&characterPacket, sizeof(characterPacket), 0);
				if (retval == SOCKET_ERROR) err_quit("send() [ĳ���� ���� ��Ŷ]");
			}

			// �߻�ü ���� Ȯ��
			bulletNum.BulletNum = 0;

			for (int i = 0; i < BULLETS_MAX_NUM; ++i)
			{
				if (bullets[i]->GetType() != ObjectsType::NONE)
				{
					++bulletNum.BulletNum;
				}
			}

			// [�߻�ü ���� ��Ŷ] �۽�
			retval = send(client_sock, (char*)&bulletNum, sizeof(bulletNum), 0);
			if (retval == SOCKET_ERROR) err_quit("send() [�߻�ü ���� ��Ŷ]");

			// �߻�ü ���� �۽�
			for (int i = 0; i < BULLETS_MAX_NUM; ++i)
			{
				if (bullets[i]->GetType() != ObjectsType::NONE)
				{
					// ������ �ִ� �߻�ü ������ ��Ŷ���� ����
					bullets[i]->SetBulletPacket(&bullet);

					// [�߻�ü ���� ��Ŷ] �۽�
					retval = send(client_sock, (char*)&bullet, sizeof(bullet), 0);
					if (retval == SOCKET_ERROR) err_quit("send() [�߻�ü ���� ��Ŷ]");
				}
			}

			// [�÷��̾� ���� ��Ŷ] ����
			retval = recv(client_sock, (char*)&playerState, sizeof(playerState), 0);
			if (retval == SOCKET_ERROR) err_quit("recv() [�÷��̾� ���� ��Ŷ]");

			// �÷��̾� ���� ����
			characters[clientID.ClientID]->SetCharacterState(&playerState);

			// �߻� �õ� ���� ����
			atteckDir[clientID.ClientID].X = playerState.AttackDirectionX;
			atteckDir[clientID.ClientID].Y = playerState.AttackDirectionY;
		}
	}

	// closesocket()
	closesocket(client_sock);

	printf("%d�� ������ ��� ����\n", clientID.ClientID);

	return 0;
}

// ���� �ʱ�ȭ �Լ�
void Initialize_Server()
{

	// Ŭ���̾�Ʈ ID �ο� ���� �ʱ�ȭ
	ClientsCount = 0;

	// ������ �� ���� �ʱ�ȭ
	sceneState.SceneState = SceneState::SELECT_CHARACTER;

	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		// �ڵ� ����, ���ȣ ����
		hEventReady[i] = CreateEvent(nullptr, false, false, nullptr);

		// �ڵ� ����, ���ȣ ����
		hEventCreateThread[i] = CreateEvent(nullptr, false, false, nullptr);

		// �߻� �õ� ����ü �ʱ�ȭ
		atteckDir[i].X = 0.0f;
		atteckDir[i].Y = 0.0f;

		// �ڵ� ����, ���ȣ ����
		hEventGameEnd[i] = CreateEvent(nullptr, false, false, nullptr);
	}

	// ���� ����, ���ȣ ����
	hEventComStart = CreateEvent(nullptr, true, false, nullptr);

	

	// ��ֹ� ��ġ ����
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

	// ĳ���� ����
	characters[0] = new S_Character(96.0f, 132.0f);
	characters[1] = new S_Character(912.0f, 264.0f);
	if (CLIENT_NUM == 3)
	{
		characters[2] = new S_Character(576.0f, 858.0f);
	}

	// �߻�ü �迭 �ʱ�ȭ
	for (int i = 0; i < BULLETS_MAX_NUM; ++i)
	{
		bullets[i] = new S_Bullet();
	}
}

// ���� ���� �Լ�
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

// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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

	// ĳ���� ��ġ ���� ������Ʈ
	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		if (characters[i]->GetType() == ObjectsType::DIE) continue;

		characters[i]->PositionUpdate(MiliSec);
	}

	// �߻�ü ��ġ ���� ������Ʈ
	for (int i = 0; i < BULLETS_MAX_NUM; ++i)
	{
		if (bullets[i]->GetType() == ObjectsType::NONE) continue;

		bullets[i]->PositionUpdate(MiliSec);
	}

	// ĳ���� �ִϸ��̼�
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

	// �߻�ü �ִϸ��̼�
	for (int i = 0; i < BULLETS_MAX_NUM; ++i)
	{
		bullets[i]->BulletAnim();
	}


	// �Ѿ� �߻� �˻�
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

	// ĳ���� <-> ��ֹ� �浹 �˻�
	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		for (int j = 0; j < OBSTACLE_NUM; ++j)
		{
			if (CC_Char_Obst(characters[i], obstacles[j]))
			{
				// ĳ���� <-> ��ֹ� �浹 ó��
				characters[i]->BounceCollision(MiliSec);
			}
		}
	}

	// ĳ���� <-> ĳ���� �浹 �˻�
	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		for (int j = i + 1; j < CLIENT_NUM; ++j)
		{
			if (CC_Char_Char(characters[i], characters[j]))
			{
				// ĳ���� <-> ĳ���� �浹 ó��
				characters[i]->BounceCollision(MiliSec);
				characters[j]->BounceCollision(MiliSec);
			}
		}
	}

	// ĳ���� <-> �߻�ü �浹 �˻�
	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		for (int j = 0; j < BULLETS_MAX_NUM; ++j)
		{
			if (bullets[j]->GetType() == ObjectsType::NONE) continue;
			if (i == bullets[j]->GetOwnerID()) continue;

			if (CC_Char_Bullet(characters[i], bullets[j]))
			{
				// ĳ���� <-> �߻�ü �浹 ó��
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

	// ���� ���� �Ǵ�
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

	// Ŭ���̾�Ʈ�� ������ �ۼ��� �϶�� �˸�
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

// ĳ���� <-> ��ֹ� �浹 �˻�
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

// ĳ���� <-> ĳ���� �浹 �˻�
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

// ĳ���� <-> �߻�ü �浹 �˻�
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