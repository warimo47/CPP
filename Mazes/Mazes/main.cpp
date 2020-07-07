#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <chrono>
#include <mutex>
#include <thread>
#include <Windows.h>
#include <conio.h>
#include <stdio.h>

#define MAINSCENE 0
#define INGAME 1
#define GAMEOVER 2

#define WIDTH 60
#define HEIGHT 60

using namespace std;
using namespace std::chrono;

char g_map[HEIGHT + 1][WIDTH + 1] = {0, };
COORD g_playerPos;
int g_stageNum = 0;
mutex g_lock;
time_point<system_clock> timerEnd;
int g_maxTime = 10;
int g_Scene = MAINSCENE;

void textcolor(int foreground, int background)
{
	int color = foreground + background * 16;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void mapSetting()
{
	g_stageNum++;

	// 벽 만들기
	for (int j = 0; j < HEIGHT + 1; ++j)
	{
		for (int i = 0; i < WIDTH + 1; ++i)
		{
			g_map[j][i] = 1;
		}
	}

	// 플레이어 초기 위치
	if (rand() % 2)
	{
		g_map[1][0] = 2;
		g_map[0][1] = 1;
		g_playerPos.Y = 1;
		g_playerPos.X = 0;
	}
	else
	{
		g_map[1][0] = 1;
		g_map[0][1] = 2;
		g_playerPos.Y = 0;
		g_playerPos.X = 1;
	}

	// 출구 위치
	if (rand() % 2)
	{
		g_map[HEIGHT][WIDTH - 1] = 1;
		g_map[HEIGHT - 1][WIDTH] = 3;
	}
	else
	{
		g_map[HEIGHT][WIDTH - 1] = 3;
		g_map[HEIGHT - 1][WIDTH] = 1;
	}

	// 구멍 뚫기
	for (int j = 1; j < HEIGHT + 1; ++j)
	{
		for (int i = 1; i < WIDTH + 1; ++i)
		{
			if (j % 2 == 1 && i % 2 == 1)
				g_map[j][i] = 0;
		}
	}

	// 길 만들기
	int now_X = 1;
	int now_Y = 1;
	while (true)
	{
		if (now_Y == HEIGHT - 1 && now_X == WIDTH - 1) break;

		if (rand() % 2)
		{
			if (now_X < WIDTH - 1)
			{
				g_map[now_Y][now_X + 1] = 0;
				now_X += 2;
			}
		}
		else
		{
			if (now_Y < HEIGHT - 1)
			{
				g_map[now_Y + 1][now_X] = 0;
				now_Y += 2;
			}
		}
	}

	// 미로 모양 만들기
	for (int j = 1; j < HEIGHT; ++j)
	{
		for (int i = 1; i < WIDTH; ++i)
		{
			if (j % 2 == 0 && i % 2 == 0) continue;

			if (rand() % 2 == 0)
			{
				g_map[j][i] = 0;
			}
		}
	}
}

void drawMain()
{
	system("cls");

	ifstream fin;

	fin.open("Mainscene.bin", ios::binary);
	
	fin.read(reinterpret_cast<char*>(&g_map), sizeof(g_map));

	for (int j = 0; j < HEIGHT + 1; ++j)
	{
		if (j > 0) cout << "\n";
		for (int i = 0; i < WIDTH + 1; ++i)
		{
			switch (g_map[j][i])
			{
			case 0:
				textcolor(0, 0);
				cout << "  ";
				break;
			case 1:
				textcolor(0, 15);
				cout << "  ";
				break;
			case 2:
				textcolor(0, 4);
				cout << "  ";
				break;
			default:
				break;
			}
		}
	}

	fin.close();

	textcolor(15, 0);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 41, 40 });
	cout << "아무 키나 누르시면 게임이 시작됩니다.";

	_getch();

	g_Scene = INGAME;
}

void drawIngame()
{
	// 맵 출력
	g_lock.lock();

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 });

	for (int j = 0; j < HEIGHT + 1; ++j)
	{
		for (int i = 0; i < WIDTH + 1; ++i)
		{
			switch (g_map[j][i])
			{
			case 0:
				textcolor(0, 0);
				cout << "  ";
				break;
			case 1:
				textcolor(0, 7);
				cout << "  ";
				break;
			case 2:
				textcolor(14, 0);
				cout << "★";
				break;
			case 3:
				textcolor(11, 0);
				cout << "●";
				break;
			}
		}
		if (j < HEIGHT) cout << "\n";
	}

	textcolor(15, 0);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ WIDTH * 2 + 2, 0 });
	cout << "STAGE ";

	if (g_stageNum < 10) cout << "0";
	cout << g_stageNum;

	timerEnd = system_clock::now();

	g_lock.unlock();
}

void drawGameover()
{
	system("cls");

	ifstream fin;

	fin.open("Gameover.bin", ios::binary);

	fin.read(reinterpret_cast<char*>(&g_map), sizeof(g_map));

	for (int j = 0; j < HEIGHT + 1; ++j)
	{
		if (j > 0) cout << "\n";
		for (int i = 0; i < WIDTH + 1; ++i)
		{
			switch (g_map[j][i])
			{
			case 0:
				textcolor(0, 0);
				cout << "  ";
				break;
			case 1:
				textcolor(0, 15);
				cout << "  ";
				break;
			case 2:
				textcolor(0, 4);
				cout << "  ";
				break;
			default:
				break;
			}
		}
	}

	fin.close();

	textcolor(15, 0);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 51, 40 });
	cout << "Your last stage : ";

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 70, 40 });
	cout << g_stageNum;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 37, 43 });
	cout << "아무 키나 누르시면 메인 화면으로 돌아갑니다.";

	_getch();

	g_Scene = MAINSCENE;
}

void initialize()
{
	srand(static_cast<unsigned>(time(0)));

	// 콘솔 창 크기 설정
	system("mode con:cols=130 lines=61");
	// 콘솔 창 타이틀 설정
	system("title Mazes 게임공학과 2012180016 배강산");
	// 콘솔 커서
	CONSOLE_CURSOR_INFO cursorInfo = { 0, };
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void timerThread()
{
	while (true)
	{
		auto sec = duration_cast<seconds>(system_clock::now() - timerEnd);

		if ((g_maxTime / g_stageNum) - sec.count() == 0) break;

		g_lock.lock();

		textcolor(15, 0);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ WIDTH * 2 + 3, 1 });
		cout << ((g_maxTime / g_stageNum) - sec.count()) / 60 << ":";
		if (((g_maxTime / g_stageNum) - sec.count()) % 60 < 10) cout << "0";
		cout << ((g_maxTime / g_stageNum) - sec.count()) % 60;

		g_lock.unlock();

		Sleep(1000);
	}

	g_Scene = GAMEOVER;
}

void processInput()
{
	COORD cursorPos;

	while (true)
	{
		if (_kbhit())
		{
			switch (_getch())
			{
			case 'w':
			case 'W':
				if (g_playerPos.Y > 0)
					if (g_map[g_playerPos.Y - 1][g_playerPos.X] == 3) mapSetting();
				if (g_map[g_playerPos.Y - 1][g_playerPos.X] == 0)
				{
					g_map[g_playerPos.Y][g_playerPos.X] = 0;

					cursorPos = { g_playerPos.X * 2, g_playerPos.Y };

					g_lock.lock();

					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPos);
					textcolor(0, 0);
					cout << "  ";

					g_map[g_playerPos.Y - 1][g_playerPos.X] = 2;
					g_playerPos.Y--;

					cursorPos = { g_playerPos.X * 2, g_playerPos.Y };
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPos);
					textcolor(14, 0);
					cout << "★";

					g_lock.unlock();
				}
				break;
			case 'a':
			case 'A':
				if (g_playerPos.X > 0)
					if (g_map[g_playerPos.Y][g_playerPos.X - 1] == 3) mapSetting();
				if (g_map[g_playerPos.Y][g_playerPos.X - 1] == 0)
				{
					g_map[g_playerPos.Y][g_playerPos.X] = 0;

					cursorPos = { g_playerPos.X * 2, g_playerPos.Y };

					g_lock.lock();

					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPos);
					textcolor(0, 0);
					cout << "  ";

					g_map[g_playerPos.Y][g_playerPos.X - 1] = 2;
					g_playerPos.X--;

					cursorPos = { g_playerPos.X * 2, g_playerPos.Y };
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPos);
					textcolor(14, 0);
					cout << "★";

					g_lock.unlock();
				}
				break;
			case 's':
			case 'S':
				if (g_playerPos.Y < HEIGHT)
					if (g_map[g_playerPos.Y + 1][g_playerPos.X] == 3) mapSetting();
				if (g_map[g_playerPos.Y + 1][g_playerPos.X] == 0)
				{
					g_map[g_playerPos.Y][g_playerPos.X] = 0;

					cursorPos = { g_playerPos.X * 2, g_playerPos.Y };

					g_lock.lock();

					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPos);
					textcolor(0, 0);
					cout << "  ";

					g_map[g_playerPos.Y + 1][g_playerPos.X] = 2;
					g_playerPos.Y++;

					cursorPos = { g_playerPos.X * 2, g_playerPos.Y };
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPos);
					textcolor(14, 0);
					cout << "★";

					g_lock.unlock();
				}
				break;
			case 'd':
			case 'D':
				if (g_playerPos.X < WIDTH)
					if (g_map[g_playerPos.Y][g_playerPos.X + 1] == 3) mapSetting();
				if (g_map[g_playerPos.Y][g_playerPos.X + 1] == 0)
				{
					g_map[g_playerPos.Y][g_playerPos.X] = 0;

					cursorPos = { g_playerPos.X * 2, g_playerPos.Y };

					g_lock.lock();

					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPos);
					textcolor(0, 0);
					cout << "  ";

					g_map[g_playerPos.Y][g_playerPos.X + 1] = 2;
					g_playerPos.X++;

					cursorPos = { g_playerPos.X * 2, g_playerPos.Y };
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPos);
					textcolor(14, 0);
					cout << "★";

					g_lock.unlock();
				}
				break;
			default:
				break;
			}
		}
		else
		{
			if (g_Scene != INGAME) break;
			Sleep(100);
		}
	}
}

int main()
{
	initialize();

	while (true)
	{
		switch (g_Scene)
		{
		case MAINSCENE:
			g_stageNum = 0;
			drawMain();
			break;
		case INGAME:
		{
			mapSetting();
			drawIngame();

			thread th = thread{ timerThread };
			
			processInput();

			th.join();
		}
		break;
		case GAMEOVER:
			drawGameover();
			break;
		default:
			break;
		}
	}

	return 0;
}