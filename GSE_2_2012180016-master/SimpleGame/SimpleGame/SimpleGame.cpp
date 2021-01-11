/*
Copyright 2017 Lee Taek Hee (Korea Polytech University)

This program is free software: you can redistribute it and/or modify
it under the terms of the What The Hell License. Do it plz.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.
*/

#include "stdafx.h"

SceneMgr* SceneManager = nullptr;
Sound* m_sound = nullptr;
DWORD frameTime;
bool g_LeftMouseButtonDown = false;
int oldMousePosX;
int oldMousePosY;

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	frameTime = SceneManager->ObjectsUpdate(frameTime);

	glutSwapBuffers();
}

void Idle(void)
{
	RenderScene();
}

void MouseInput(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		g_LeftMouseButtonDown = true;
		oldMousePosX = x;
		oldMousePosY = y;
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		if (g_LeftMouseButtonDown
			&& (oldMousePosX - 2 < x && x < oldMousePosX + 2)
			&& (oldMousePosY - 2 < y && y < oldMousePosY + 2))
		{
			if ((0 < x) && (x - WINDOWWIDTH) && ((WINDOWHEIGHT / 2) < y) && (y < WINDOWHEIGHT)
				&& (SceneManager->GetTeam2CharacterRespawnTime() > T2_CHARACTER_RESPAWN_TIME))
			{
				SceneManager->AddObject(x, y, CHARACTER_T2, nullptr);
				SceneManager->ResetTeam2CharacterRespawnTime();
			}
		}
		g_LeftMouseButtonDown = false;
	}

	RenderScene();
}

void MotionInput(int x, int y)
{
	if (g_LeftMouseButtonDown)
	{
		SceneManager->AddObject(x, y, CHARACTER_T2, nullptr);
	}
	RenderScene();
}

void KeyInput(unsigned char key, int x, int y)
{
	RenderScene();
}

void SpecialKeyInput(int key, int x, int y)
{
	RenderScene();
}

int main(int argc, char **argv)
{
	srand((unsigned int)time(nullptr));

	// Initialize GL things
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOWWIDTH, WINDOWHEIGHT);
	glutCreateWindow("Game Software Engineering KPU");

	glewInit();
	if (glewIsSupported("GL_VERSION_3_0")) cout << " GLEW Version is 3.0\n ";
	else std::cout << "GLEW 3.0 not supported\n ";

	// Initialize Renderer
	SceneManager = new SceneMgr();
	m_sound = new Sound();
	int soundBG = m_sound->CreateSound("./Resource/SoundSamples/Background.mp3");
	m_sound->PlaySound(soundBG, true, 0.5f);

	glutDisplayFunc(RenderScene);
	glutIdleFunc(Idle);
	glutKeyboardFunc(KeyInput);
	glutMouseFunc(MouseInput);
	// glutMotionFunc(MotionInput); 마우스 움직일 때 호출되는 함수 설정
	glutSpecialFunc(SpecialKeyInput);

	glutMainLoop();

	SceneManager->~SceneMgr();

    return 0;
}

