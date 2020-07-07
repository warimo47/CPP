// Copyright 2020 warimo47@naver.com, All Rights Reserved.

// C++ Header
#include <iostream>
#include <ctime>

// C Header
#include <Windows.h>
#include <conio.h>

#define EDITMAP 0
#define PRINT_ANSWER 1

#define WIDTH 9
#define HEIGHT 9

// Grobal values
int gMap[WIDTH][HEIGHT];
int gScene = EDITMAP;
COORD gCursorPos = { 1, 1 };

void textcolor(int foreground, int background)
{
	int color = background * 16 + foreground;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void initialize()
{
	for (int i = 0; i < 9; ++i)
		for (int j = 0; j < 9; ++j) gMap[i][j] = 0;

	srand(static_cast<unsigned>(time(0)));

	// 콘솔 창 크기 설정
	system("mode con:cols=40 lines=20");

	// 콘솔 창 타이틀 설정
	system("Sudoku");

	// 콘솔 커서
	CONSOLE_CURSOR_INFO cursorInfo = { 0, };
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void editMap()
{
	system("cls");

	textcolor(15, 0);

	std::cout << " ┌───┬───┬───┰───┬───┬───┰───┬───┬───┐  \n";
	for (int i = 0; i < 9; ++i)
	{
		if (i == 3 || i == 6) std::cout << " ┃ ";
		else std::cout << " │ ";
		if (gMap[0][i] == 0) std::cout << " ";
		else std::cout << gMap[0][i];
	}
	std::cout << " │  \n";
	std::cout << " ├───┼───┼───╂───┼───┼───╂───┼───┼───┤  \n";
	for (int i = 0; i < 9; ++i)
	{
		if (i == 3 || i == 6) std::cout << " ┃ ";
		else std::cout << " │ ";
		if (gMap[1][i] == 0) std::cout << " ";
		else std::cout << gMap[1][i];
	}
	std::cout << " │  \n";
	std::cout << " ├───┼───┼───╂───┼───┼───╂───┼───┼───┤  \n";
	for (int i = 0; i < 9; ++i)
	{
		if (i == 3 || i == 6) std::cout << " ┃ ";
		else std::cout << " │ ";
		if (gMap[2][i] == 0) std::cout << " ";
		else std::cout << gMap[2][i];
	}
	std::cout << " │  \n";
	std::cout << " ┝━━━┿━━━┿━━━╋━━━┿━━━┿━━━╋━━━┿━━━┿━━━┥  \n";
	for (int i = 0; i < 9; ++i)
	{
		if (i == 3 || i == 6) std::cout << " ┃ ";
		else std::cout << " │ ";
		if (gMap[3][i] == 0) std::cout << " ";
		else std::cout << gMap[3][i];
	}
	std::cout << " │  \n";
	std::cout << " ├───┼───┼───╂───┼───┼───╂───┼───┼───┤  \n";
	for (int i = 0; i < 9; ++i)
	{
		if (i == 3 || i == 6) std::cout << " ┃ ";
		else std::cout << " │ ";
		if (gMap[4][i] == 0) std::cout << " ";
		else std::cout << gMap[4][i];
	}
	std::cout << " │  \n";
	std::cout << " ├───┼───┼───╂───┼───┼───╂───┼───┼───┤  \n";
	for (int i = 0; i < 9; ++i)
	{
		if (i == 3 || i == 6) std::cout << " ┃ ";
		else std::cout << " │ ";
		if (gMap[5][i] == 0) std::cout << " ";
		else std::cout << gMap[5][i];
	}
	std::cout << " │  \n";
	std::cout << " ┝━━━┿━━━┿━━━╋━━━┿━━━┿━━━╋━━━┿━━━┿━━━┥  \n";
	for (int i = 0; i < 9; ++i)
	{
		if (i == 3 || i == 6) std::cout << " ┃ ";
		else std::cout << " │ ";
		if (gMap[6][i] == 0) std::cout << " ";
		else std::cout << gMap[6][i];
	}
	std::cout << " │  \n";
	std::cout << " ├───┼───┼───╂───┼───┼───╂───┼───┼───┤  \n";
	for (int i = 0; i < 9; ++i)
	{
		if (i == 3 || i == 6) std::cout << " ┃ ";
		else std::cout << " │ ";
		if (gMap[7][i] == 0) std::cout << " ";
		else std::cout << gMap[7][i];
	}
	std::cout << " │  \n";
	std::cout << " ├───┼───┼───╂───┼───┼───╂───┼───┼───┤  \n";
	for (int i = 0; i < 9; ++i)
	{
		if (i == 3 || i == 6) std::cout << " ┃ ";
		else std::cout << " │ ";
		if (gMap[8][i] == 0) std::cout << " ";
		else std::cout << gMap[8][i];
	}
	std::cout << " │  \n";
	std::cout << " └───┴───┴───┸───┴───┴───┸───┴───┴───┘  \n";
}

void processInput()
{
	COORD cursorPos;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { gCursorPos.X * 4 - 1, gCursorPos.Y * 2 - 1 });
	textcolor(0, 15);
	if (gMap[gCursorPos.Y - 1][gCursorPos.X - 1] == 0) std::cout << " ";
	else std::cout << gMap[gCursorPos.Y - 1][gCursorPos.X - 1];

	switch (_getch())
	{
	case 'w':
	case 'W':
		if (gCursorPos.Y > 1) gCursorPos.Y -= 1;
		break;
	case 'a':
	case 'A':
		if (gCursorPos.X > 1) gCursorPos.X -= 1;
		break;
	case 's':
	case 'S':
		if (gCursorPos.Y < 9) gCursorPos.Y += 1;
		break;
	case 'd':
	case 'D':
		if (gCursorPos.X < 9) gCursorPos.X += 1;
		break;
	case '1':
		gMap[gCursorPos.Y - 1][gCursorPos.X - 1] = 1;
		break;
	case '2':
		gMap[gCursorPos.Y - 1][gCursorPos.X - 1] = 2;
		break;
	case '3':
		gMap[gCursorPos.Y - 1][gCursorPos.X - 1] = 3;
		break;
	case '4':
		gMap[gCursorPos.Y - 1][gCursorPos.X - 1] = 4;
		break;
	case '5':
		gMap[gCursorPos.Y - 1][gCursorPos.X - 1] = 5;
		break;
	case '6':
		gMap[gCursorPos.Y - 1][gCursorPos.X - 1] = 6;
		break;
	case '7':
		gMap[gCursorPos.Y - 1][gCursorPos.X - 1] = 7;
		break;
	case '8':
		gMap[gCursorPos.Y - 1][gCursorPos.X - 1] = 8;
		break;
	case '9':
		gMap[gCursorPos.Y - 1][gCursorPos.X - 1] = 9;
		break;
	case '0':
		gMap[gCursorPos.Y - 1][gCursorPos.X - 1] = 0;
		break;
	case ' ':
		gScene = PRINT_ANSWER;
		break;
	default:
		break;
	}

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { gCursorPos.X * 4 - 1, gCursorPos.Y * 2 - 1 });
	textcolor(15, 0);
	if (gMap[gCursorPos.Y - 1][gCursorPos.X - 1] == 0) std::cout << " ";
	else std::cout << gMap[gCursorPos.Y - 1][gCursorPos.X - 1];
}

bool checkDone()
{
	for (int j = 0; j < 9; ++j)
	{
		for (int i = 0; i < 9; ++i)
		{
			if (gMap[j][i] == 0) return false;
		}
	}

	return true;
}

void calculate()
{
	while (true)
	{
		if (checkDone() == true) break;


	}
}

void printAnswer()
{
	system("cls");

	textcolor(15, 0);

	std::cout << " ┌───┬───┬───┰───┬───┬───┰───┬───┬───┐  \n";
	for (int i = 0; i < 9; ++i)
	{
		if (i == 3 || i == 6) std::cout << " ┃ ";
		else std::cout << " │ ";
		if (gMap[0][i] == 0) std::cout << " ";
		else std::cout << gMap[0][i];
	}
	std::cout << " │  \n";
	std::cout << " ├───┼───┼───╂───┼───┼───╂───┼───┼───┤  \n";
	for (int i = 0; i < 9; ++i)
	{
		if (i == 3 || i == 6) std::cout << " ┃ ";
		else std::cout << " │ ";
		if (gMap[1][i] == 0) std::cout << " ";
		else std::cout << gMap[1][i];
	}
	std::cout << " │  \n";
	std::cout << " ├───┼───┼───╂───┼───┼───╂───┼───┼───┤  \n";
	for (int i = 0; i < 9; ++i)
	{
		if (i == 3 || i == 6) std::cout << " ┃ ";
		else std::cout << " │ ";
		if (gMap[2][i] == 0) std::cout << " ";
		else std::cout << gMap[2][i];
	}
	std::cout << " │  \n";
	std::cout << " ┝━━━┿━━━┿━━━╋━━━┿━━━┿━━━╋━━━┿━━━┿━━━┥  \n";
	for (int i = 0; i < 9; ++i)
	{
		if (i == 3 || i == 6) std::cout << " ┃ ";
		else std::cout << " │ ";
		if (gMap[3][i] == 0) std::cout << " ";
		else std::cout << gMap[3][i];
	}
	std::cout << " │  \n";
	std::cout << " ├───┼───┼───╂───┼───┼───╂───┼───┼───┤  \n";
	for (int i = 0; i < 9; ++i)
	{
		if (i == 3 || i == 6) std::cout << " ┃ ";
		else std::cout << " │ ";
		if (gMap[4][i] == 0) std::cout << " ";
		else std::cout << gMap[4][i];
	}
	std::cout << " │  \n";
	std::cout << " ├───┼───┼───╂───┼───┼───╂───┼───┼───┤  \n";
	for (int i = 0; i < 9; ++i)
	{
		if (i == 3 || i == 6) std::cout << " ┃ ";
		else std::cout << " │ ";
		if (gMap[5][i] == 0) std::cout << " ";
		else std::cout << gMap[5][i];
	}
	std::cout << " │  \n";
	std::cout << " ┝━━━┿━━━┿━━━╋━━━┿━━━┿━━━╋━━━┿━━━┿━━━┥  \n";
	for (int i = 0; i < 9; ++i)
	{
		if (i == 3 || i == 6) std::cout << " ┃ ";
		else std::cout << " │ ";
		if (gMap[6][i] == 0) std::cout << " ";
		else std::cout << gMap[6][i];
	}
	std::cout << " │  \n";
	std::cout << " ├───┼───┼───╂───┼───┼───╂───┼───┼───┤  \n";
	for (int i = 0; i < 9; ++i)
	{
		if (i == 3 || i == 6) std::cout << " ┃ ";
		else std::cout << " │ ";
		if (gMap[7][i] == 0) std::cout << " ";
		else std::cout << gMap[7][i];
	}
	std::cout << " │  \n";
	std::cout << " ├───┼───┼───╂───┼───┼───╂───┼───┼───┤  \n";
	for (int i = 0; i < 9; ++i)
	{
		if (i == 3 || i == 6) std::cout << " ┃ ";
		else std::cout << " │ ";
		if (gMap[8][i] == 0) std::cout << " ";
		else std::cout << gMap[8][i];
	}
	std::cout << " │  \n";
	std::cout << " └───┴───┴───┸───┴───┴───┸───┴───┴───┘  \n";
}

int main()
{
	initialize();

	while (true)
	{
		switch (gScene)
		{
		case EDITMAP:
			editMap();
			processInput();
			break;
		case PRINT_ANSWER:
			calculate();
			printAnswer();
			_getch();
			break;
		}
	}
	
	return 0;
}