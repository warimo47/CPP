#include "NGP_Client.h"

// 서버 IP 주소
char ServerIpNum[16] = "127.0.0.1";

// 윈도우 핸들
HWND hWnd;

HINSTANCE hInst;
HANDLE hThread;
SceneManager* sceneMgr;

// 이벤트 핸들
HANDLE hEventReady;
HANDLE hEventInput;

// 패킷 구조체
PACKET_ClientID clientID;
PACKET_ChoiceCharacter choiceCharacter;
PACKET_SceneState sceneState;
PACKET_Character characterPacket;
PACKET_PlayerState playerState;
PACKET_BulletNum bulletNum;
PACKET_Bullet bullet;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	hInst = hInstance;

	// 윈도우 클래스 등록
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndclass.lpszMenuName = nullptr;
	wndclass.lpszClassName = "MyWndClass";
	if (!RegisterClass(&wndclass)) return 1;

	cout << "접속할 서버의 IPv4 입력 : ";
	cin >> ServerIpNum;

	// 윈도우 생성
	hWnd = CreateWindow("MyWndClass", "NPG Client", WS_OVERLAPPEDWINDOW,
		0, 0, WIN_WIDTH + 16, WIN_HEIGHT + 39, nullptr, nullptr, hInstance, nullptr);
	if (hWnd == NULL) return 1;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// 이벤트 생성
	hEventReady = CreateEvent(nullptr, false, false, nullptr);
	hEventInput = CreateEvent(nullptr, false, false, nullptr);

	// 소켓 통신 스레드 생성
	hThread = CreateThread(nullptr, 0, ComServer, nullptr, 0, nullptr);

	// 메시지 루프
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

// 소켓 함수 오류 출력 후 종료
void err_quit(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr, WSAGetLastError(),
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
		nullptr, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	cout << "[" << msg << "] " << lpMsgBuf << endl;
	LocalFree(lpMsgBuf);
}

// 윈도우 메시지 콜벡 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		sceneMgr = new SceneManager();

		playerState.AttackDirectionX = 0.0f;
		playerState.AttackDirectionY = 0.0f;
		playerState.DirectionState = DirectionState::DOWN;
		playerState.IsMoving = (BYTE)0;
		return 0;
	}
	case WM_SIZE:
		return 0;
	case WM_KEYDOWN:
		ProcessKeyDown(wParam);
		return 0;
	case WM_KEYUP:
		ProcessKeyUp(wParam);
		return 0;
	case WM_LBUTTONDOWN:
	{
		float x = LOWORD(lParam);
		float y = HIWORD(lParam);

		if (sceneMgr->GetSceneState() == SceneState::IN_GAME)
		{
			if ((GetTickCount() - sceneMgr->GetLastAtteckTime()) > 
				sceneMgr->GetMyCoolTime())
			{
				float DX = (x + (BULLET_SIZE / 2.0f)) -
					(sceneMgr->GetMyPosX() + (CHAR_WIDTH / 2.0f));
				float DY = (y + (BULLET_SIZE / 2.0f)) - 
					(sceneMgr->GetMyPosY() + (CHAR_HEIGHT / 2.0f));
				float Distance = sqrtf(DX * DX + DY * DY);
				playerState.AttackDirectionX = DX / Distance;
				playerState.AttackDirectionY = DY / Distance;
			}
		}
		else if (sceneMgr->GetSceneState() == SceneState::SELECT_CHARACTER)
		{
			if (150.0f < x && x < 330.0f && 600.0f < y && y < 780.0f)
			{
				choiceCharacter.CharacterType = ObjectsType::BOMBER;
				sceneMgr->SetMyCoolTime(ObjectsType::BOMBER);
				sceneMgr->SetC_SelectBox_Y(150);
				SetEvent(hEventReady);
			}
			else if (510.0f < x && x < 690.0f && 600.0f < y && y < 780.0f)
			{
				choiceCharacter.CharacterType = ObjectsType::GUNNER;
				sceneMgr->SetMyCoolTime(ObjectsType::GUNNER);
				sceneMgr->SetC_SelectBox_Y(510);
				SetEvent(hEventReady);
			}
			else if (870.0f < x && x < 1050.0f && 600.0f < y && y < 780.0f)
			{
				choiceCharacter.CharacterType = ObjectsType::MAGICIAN;
				sceneMgr->SetMyCoolTime(ObjectsType::MAGICIAN);
				sceneMgr->SetC_SelectBox_Y(870);
				SetEvent(hEventReady);
			}
			InvalidateRect(hWnd, nullptr, false);
		}
		else
		{
			SetEvent(hEventInput);
			InvalidateRect(hWnd, nullptr, false);
		}
	}
		return 0;
	case WM_LBUTTONUP:
		return 0;
	case WM_MOUSEMOVE:
		if ((sceneMgr->GetSceneState() == SceneState::SELECT_CHARACTER)
			&& (sceneMgr->GetC_SelectBox_Y() == -200))
		{
			short x = LOWORD(lParam);
			short y = HIWORD(lParam);
			if (150 < x && x < 330 && 600 < y && y < 780)
			{
				sceneMgr->SetC_SelectBox_G(150);
			}
			else if (510 < x && x < 690 && 600 < y && y < 780)
			{
				sceneMgr->SetC_SelectBox_G(510);
			}	
			else if (870 < x && x < 1050 && 600 < y && y < 780)
			{
				sceneMgr->SetC_SelectBox_G(870);
			}
			else
			{
				sceneMgr->SetC_SelectBox_G(-200);
			}
			InvalidateRect(hWnd, nullptr, false);
		}
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// Double buffering
		HBITMAP hViewBit = CreateCompatibleBitmap(hdc, WIN_WIDTH, WIN_HEIGHT);
		HDC hViewdc = CreateCompatibleDC(hdc);
		SelectObject(hViewdc, hViewBit);

		// Draw Here
		sceneMgr->Render(hViewdc);

		// Double buffering
		BitBlt(hdc, 0, 0, WIN_WIDTH, WIN_HEIGHT, hViewdc, 0, 0, SRCCOPY);

		DeleteDC(hViewdc);
		DeleteObject(hViewBit);
	}
		return 0;
	case WM_SETFOCUS:
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// 서버와 데이터 통신 스레드
DWORD WINAPI ComServer(LPVOID arg)
{
	// 오류 체크 변수
	int retval;
	int addrlen;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ServerIpNum);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// 서버 정보 얻기
	addrlen = sizeof(serveraddr);
	getpeername(sock, (SOCKADDR*)&serveraddr, &addrlen);

	// --- 서버와 데이터 통신 시작 ---

	// [ID 부여 패킷] 수신
	retval = recv(sock, (char*)&clientID, sizeof(clientID), 0);
	if (retval == SOCKET_ERROR) err_quit("recv() [ID 부여 패킷]");

	// 내 클라이언트 ID 콘솔에 출력
	sceneMgr->SetMyID(clientID.ClientID);
	cout << "MyID : " << sceneMgr->GetMyID() << endl;

	while (1)
	{
		// 사용자가 캐릭터 선택할때까지 기다림
		WaitForSingleObject(hEventReady, INFINITE);

		// [캐릭터 선택 패킷] 송신
		retval = send(sock, (char*)&choiceCharacter, sizeof(choiceCharacter), 0);
		if (retval == SOCKET_ERROR) err_quit("send() [캐릭터 선택 패킷]");

		while (1)
		{
			// [씬 상태 패킷] 수신
			retval = recv(sock, (char*)&sceneState, sizeof(sceneState), 0);
			if (retval == SOCKET_ERROR) err_quit("recv() [씬 상태 패킷]");

			// 씬 상태 갱신
			sceneMgr->SetSceneState(&sceneState);

			// 게임 종료 상태라면 캐릭터 선택으로 돌아감
			if (sceneMgr->GetSceneState() > SceneState::IN_GAME)
			{
				// 윈도우 다시 그리기
				InvalidateRect(hWnd, nullptr, false);

				// 사용자가 마우스 입력이나 키보드 입력을 하기를 기다림
				WaitForSingleObject(hEventInput, INFINITE);

				// 게임 초기화
				sceneMgr->ResetClient();

				break;
			}

			// 캐릭터 정보 수신
			for (int i = 0; i < CLIENT_NUM; ++i)
			{
				// [캐릭터 정보 패킷] 수신
				retval = recv(sock, (char*)&characterPacket, sizeof(characterPacket), 0);
				if (retval == SOCKET_ERROR) err_quit("recv() [캐릭터 정보 패킷]");

				// 캐릭터 정보 패킷 클라이언트에 복사
				sceneMgr->SetC_Character(i, &characterPacket);
			}

			// [발사체 개수 패킷] 수신
			retval = recv(sock, (char*)&bulletNum, sizeof(bulletNum), 0);
			if (retval == SOCKET_ERROR) err_quit("recv() [발사체 개수 패킷]");

			// 발사체 배열 비우기
			sceneMgr->ClearBulletArray();

			// 발사체 정보 수신
			for (int i = 0; i < bulletNum.BulletNum; ++i)
			{
				// [발사체 정보 패킷] 송신
				retval = recv(sock, (char*)&bullet, sizeof(bullet), 0);
				if (retval == SOCKET_ERROR) err_quit("recv() [발사체 개수 패킷]");

				// 발사체 정보 패킷 클라이언트에 복사
				sceneMgr->AddC_Bullet(&bullet);
			}

			// 윈도우 다시 그리기
			InvalidateRect(hWnd, nullptr, false);

			// [플레이어 상태 패킷] 송신
			retval = send(sock, (char*)&playerState, sizeof(playerState), 0);
			if (retval == SOCKET_ERROR) err_quit("send() [플레이어 상태 패킷]");

			// 발사했을때 후처리
			if (playerState.AttackDirectionX != 0.0f || playerState.AttackDirectionY != 0.0f)
			{
				playerState.AttackDirectionX = 0.0f;
				playerState.AttackDirectionY = 0.0f;
				sceneMgr->SetLastAtteckTime(GetTickCount());
			}
		}
	}

	// closesocket()
	closesocket(sock);

	return 0;
}

void ProcessKeyDown(WPARAM wParam)
{
	switch (wParam)
	{
	case 'w':
	case 'W':
		switch (playerState.DirectionState)
		{
		case DirectionState::DOWN:
			playerState.DirectionState = DirectionState::UP;
			break;
		case DirectionState::DOWN_LEFT:
			playerState.DirectionState = DirectionState::UP_LEFT;
			break;
		case DirectionState::DOWN_RIGHT:
			playerState.DirectionState = DirectionState::UP_RIGHT;
			break;
		case DirectionState::LEFT:
			if (playerState.IsMoving == 0)
				playerState.DirectionState = DirectionState::UP;
			else
				playerState.DirectionState = DirectionState::UP_LEFT;
			break;
		case DirectionState::RIGHT:
			if (playerState.IsMoving == 0)
				playerState.DirectionState = DirectionState::UP;
			else
				playerState.DirectionState = DirectionState::UP_RIGHT;
			break;
		case DirectionState::UP:
		case DirectionState::UP_LEFT:
		case DirectionState::UP_RIGHT:
			break;
		}
		playerState.IsMoving = (BYTE)1;
		break;
	case 'd':
	case 'D':
		switch (playerState.DirectionState)
		{
		case DirectionState::DOWN:
			if (playerState.IsMoving == 0)
				playerState.DirectionState = DirectionState::RIGHT;
			else
				playerState.DirectionState = DirectionState::DOWN_RIGHT;
			break;
		case DirectionState::DOWN_LEFT:
			playerState.DirectionState = DirectionState::DOWN_RIGHT;
			break;
		case DirectionState::LEFT:
			playerState.DirectionState = DirectionState::RIGHT;
			break;
		case DirectionState::UP:
			if (playerState.IsMoving == 0)
				playerState.DirectionState = DirectionState::RIGHT;
			else
				playerState.DirectionState = DirectionState::UP_RIGHT;
			break;
		case DirectionState::UP_LEFT:
			playerState.DirectionState = DirectionState::UP_RIGHT;
			break;
		case DirectionState::DOWN_RIGHT:
		case DirectionState::RIGHT:
		case DirectionState::UP_RIGHT:
			break;
		}
		playerState.IsMoving = (BYTE)1;
		break;
	case 'a':
	case 'A':
		switch (playerState.DirectionState)
		{
		case DirectionState::DOWN:
			if (playerState.IsMoving == 0)
				playerState.DirectionState = DirectionState::LEFT;
			else
				playerState.DirectionState = DirectionState::DOWN_LEFT;
			break;
		case DirectionState::DOWN_RIGHT:
			playerState.DirectionState = DirectionState::DOWN_LEFT;
			break;
		case DirectionState::RIGHT:
			playerState.DirectionState = DirectionState::LEFT;
			break;
		case DirectionState::UP:
			if (playerState.IsMoving == 0)
				playerState.DirectionState = DirectionState::LEFT;
			else
				playerState.DirectionState = DirectionState::UP_LEFT;
			break;
		case DirectionState::UP_RIGHT:
			playerState.DirectionState = DirectionState::UP_LEFT;
			break;
		case DirectionState::DOWN_LEFT:
		case DirectionState::LEFT:
		case DirectionState::UP_LEFT:
			break;
		}
		playerState.IsMoving = (BYTE)1;
		break;
	case 's':
	case 'S':
		switch (playerState.DirectionState)
		{
		case DirectionState::LEFT:
			if (playerState.IsMoving == 0)
				playerState.DirectionState = DirectionState::DOWN;
			else
				playerState.DirectionState = DirectionState::DOWN_LEFT;
			break;
		case DirectionState::RIGHT:
			if (playerState.IsMoving == 0)
				playerState.DirectionState = DirectionState::DOWN;
			else
				playerState.DirectionState = DirectionState::DOWN_RIGHT;
			break;
		case DirectionState::UP:
			playerState.DirectionState = DirectionState::DOWN;
			break;
		case DirectionState::UP_LEFT:
			playerState.DirectionState = DirectionState::DOWN_LEFT;
			break;
		case DirectionState::UP_RIGHT:
			playerState.DirectionState = DirectionState::DOWN_RIGHT;
			break;
		case DirectionState::DOWN:
		case DirectionState::DOWN_LEFT:
		case DirectionState::DOWN_RIGHT:
			break;
		}
		playerState.IsMoving = (BYTE)1;
		break;
	case 'c':
	case 'C':
		sceneMgr->SwitchCollision();
		break;
	}
}

void ProcessKeyUp(WPARAM wParam)
{
	switch (wParam)
	{
	case 'w':
	case 'W':
		switch (playerState.DirectionState)
		{
		case DirectionState::DOWN:
		case DirectionState::DOWN_LEFT:
		case DirectionState::DOWN_RIGHT:
		case DirectionState::LEFT:
		case DirectionState::RIGHT:
			cout << "KeyUp 오류" << endl;
			break;
		case DirectionState::UP:
			playerState.IsMoving = (BYTE)0;
			break;
		case DirectionState::UP_LEFT:
			playerState.DirectionState = DirectionState::LEFT;
			break;
		case DirectionState::UP_RIGHT:
			playerState.DirectionState = DirectionState::RIGHT;
			break;
		}
		break;
	case 'd':
	case 'D':
		switch (playerState.DirectionState)
		{
		case DirectionState::DOWN:
		case DirectionState::DOWN_LEFT:
		case DirectionState::UP:
		case DirectionState::UP_LEFT:
		case DirectionState::LEFT:
			cout << "KeyUp 오류" << endl;
			break;
		case DirectionState::DOWN_RIGHT:
			playerState.DirectionState = DirectionState::DOWN;
			break;
		case DirectionState::RIGHT:
			playerState.IsMoving = (BYTE)0;
			break;
		case DirectionState::UP_RIGHT:
			playerState.DirectionState = DirectionState::UP;
			break;
		}
		break;
	case 'a':
	case 'A':
		switch (playerState.DirectionState)
		{
		case DirectionState::DOWN_LEFT:
			playerState.DirectionState = DirectionState::DOWN;
			break;
		case DirectionState::LEFT:
			playerState.IsMoving = (BYTE)0;
			break;
		case DirectionState::UP_LEFT:
			playerState.DirectionState = DirectionState::UP;
			break;
		case DirectionState::DOWN:
		case DirectionState::DOWN_RIGHT:
		case DirectionState::RIGHT:
		case DirectionState::UP:
		case DirectionState::UP_RIGHT:
			cout << "KeyUp 오류" << endl;
			break;
		}
		break;
	case 's':
	case 'S':
		switch (playerState.DirectionState)
		{
		case DirectionState::DOWN:
			playerState.IsMoving = (BYTE)0;
			break;
		case DirectionState::DOWN_LEFT:
			playerState.DirectionState = DirectionState::LEFT;
			break;
		case DirectionState::DOWN_RIGHT:
			playerState.DirectionState = DirectionState::RIGHT;
			break;
		case DirectionState::LEFT:
		case DirectionState::RIGHT:
		case DirectionState::UP:
		case DirectionState::UP_LEFT:
		case DirectionState::UP_RIGHT:
			cout << "KeyUp 오류" << endl;
			break;
		}
		break;
	}
}