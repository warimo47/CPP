#include "SceneManager.h"
#include <iostream>

using namespace std;

SceneManager::SceneManager()
{
	// ĳ���� �迭 �޸� �Ҵ�
	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		C_Characters[i] = new C_Character;
	}

	// �߻�ü �迭 �޸� �Ҵ�
	for (int i = 0; i < BULLETS_MAX_NUM; ++i)
	{
		C_Bullets[i] = new C_Bullet();
	}

	// ĳ���� ���� �� �̹���
	i_Select_Character_View.Load("Resource/Select_Character_View.png");

	i_Bomber_Select.Load("Resource/Bomber/Bomber_Select.png");
	i_Gunner_Select.Load("Resource/Gunner/Gunner_Select.png");
	i_Magician_Select.Load("Resource/Magician/Magician_Select.png");

	i_Select_Box_G.Load("Resource/Select_Box_G.png");
	i_Select_Box_Y.Load("Resource/Select_Box_Y.png");

	// �� ���� �� �̹���
	i_Background.Load("Resource/BackGround.png");
	i_Upground.Load("Resource/BackGroundUp.png");
	i_LifeGauge.Load("Resource/LifeGauge.png");
	i_LifeGaugeBG.Load("Resource/LifeGaugeBG.png");

	i_Bomber.Load("Resource/Bomber/Bomber.png");
	i_Gunner.Load("Resource/Gunner/Gunner.png");
	i_Magician.Load("Resource/Magician/Magician.png");

	i_Boom.Load("Resource/Bomber/Boom.png");
	i_Bullet.Load("Resource/Gunner/Bullet.png");
	i_Fire.Load("Resource/Magician/Fire.png");

	// �¸�, �й� �� �̹���
	i_Victory.Load("Resource/Victory.png");
	i_Defeat.Load("Resource/Defeat.png");

	// �浹 �ڽ� �̹���
	i_Char_Col.Load("Resource/Char_Col.png");
	i_Bullet_Col.Load("Resource/Bullet_Col.png");

	// �̹����� �׸��� ���� �� ���
	ImageAlphaSetting(&i_Upground);
	ImageAlphaSetting(&i_Bomber);
	ImageAlphaSetting(&i_Gunner);
	ImageAlphaSetting(&i_Magician);
	ImageAlphaSetting(&i_Victory);
	ImageAlphaSetting(&i_Defeat);

	// ID �ʱ�ȭ
	MyID = 0; // ���� �ʱ�ȭ �Ҷ��� �ٲ�� �ȵ�

	// ���� �ʱ�ȭ
	ResetClient();
}


SceneManager::~SceneManager()
{
	// ĳ���� �迭 �޸� ��ȯ
	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		delete C_Characters[i];
	}

	// �߻�ü �迭 �޸� ��ȯ
	for (int i = 0; i < BULLETS_MAX_NUM; ++i)
	{
		delete C_Bullets[i];
	}
}

void SceneManager::ResetClient()
{
	// ���� �ʱ�ȭ
	SceneState = SELECT_CHARACTER;
	C_SelectBox_G = -200;
	C_SelectBox_Y = -200;
	VisibleCollisionBox = false;

	// �߻� �õ� ��Ÿ�� ���� �ʱ�ȭ
	LastAtteckTime = GetTickCount();
	MyCoolTime = 1000;
}

void SceneManager::ImageAlphaSetting(CImage* Img)
{
	unsigned char * pCol = 0;

	long w = Img->GetWidth();
	long h = Img->GetHeight();

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			pCol = (unsigned char *)Img->GetPixelAddress(x, y);
			unsigned char alpha = pCol[3];
			if (alpha != 255)
			{
				pCol[0] = ((pCol[0] * alpha) + 128) >> 8;
				pCol[1] = ((pCol[1] * alpha) + 128) >> 8;
				pCol[2] = ((pCol[2] * alpha) + 128) >> 8;
			}
		}
	}
}

void SceneManager::Render(HDC hViewdc)
{
	if (SceneState == SceneState::IN_GAME)
	{
		// ��� �׸���
		i_Background.AlphaBlend(hViewdc,
			0, 0, WIN_WIDTH, WIN_HEIGHT, 0, 0, WIN_WIDTH, WIN_HEIGHT);

		// ĳ���� �׸���
		for (int i = 0; i < CLIENT_NUM; ++i)
		{
			switch (C_Characters[i]->GetType())
			{
			case ObjectsType::DIE:
				if (VisibleCollisionBox)
				{
					i_Char_Col.AlphaBlend(hViewdc,
						(int)C_Characters[i]->GetPosX(),
						(int)C_Characters[i]->GetPosY(),
						CHAR_WIDTH, CHAR_HEIGHT,
						0, 0, CHAR_WIDTH, CHAR_HEIGHT
					);
				}
				break;
			case ObjectsType::BOMBER:
				i_Bomber.AlphaBlend(hViewdc,
					(int)C_Characters[i]->GetPosX(),
					(int)C_Characters[i]->GetPosY(),
					CHAR_WIDTH, CHAR_HEIGHT,
					C_Characters[i]->GetAnimCount() * CHAR_WIDTH,
					C_Characters[i]->GetAnimState() * CHAR_HEIGHT,
					CHAR_WIDTH, CHAR_HEIGHT
				);
				i_LifeGaugeBG.AlphaBlend(hViewdc,
					(int)C_Characters[i]->GetPosX(),
					(int)C_Characters[i]->GetPosY() + CHAR_HEIGHT,
					CHAR_WIDTH, LIFE_HEIGHT,
					0, 0, CHAR_WIDTH, LIFE_HEIGHT
				);
				i_LifeGauge.AlphaBlend(hViewdc,
					(int)C_Characters[i]->GetPosX() + 1,
					(int)C_Characters[i]->GetPosY() + CHAR_HEIGHT + 1,
					C_Characters[i]->GetLife(), LIFE_HEIGHT - 2,
					0, 0, CHAR_WIDTH - 2, LIFE_HEIGHT - 2
				);
				if (VisibleCollisionBox)
				{
					i_Char_Col.AlphaBlend(hViewdc,
						(int)C_Characters[i]->GetPosX(),
						(int)C_Characters[i]->GetPosY(),
						CHAR_WIDTH, CHAR_HEIGHT,
						0, 0, CHAR_WIDTH, CHAR_HEIGHT
					);
				}
				break;
			case ObjectsType::GUNNER:
				i_Gunner.AlphaBlend(hViewdc,
					(int)C_Characters[i]->GetPosX(),
					(int)C_Characters[i]->GetPosY(),
					CHAR_WIDTH, CHAR_HEIGHT,
					C_Characters[i]->GetAnimCount() * CHAR_WIDTH,
					C_Characters[i]->GetAnimState() * CHAR_HEIGHT,
					CHAR_WIDTH, CHAR_HEIGHT
				);
				i_LifeGaugeBG.AlphaBlend(hViewdc,
					(int)C_Characters[i]->GetPosX(),
					(int)C_Characters[i]->GetPosY() + CHAR_HEIGHT,
					CHAR_WIDTH, LIFE_HEIGHT,
					0, 0, CHAR_WIDTH, LIFE_HEIGHT
				);
				i_LifeGauge.AlphaBlend(hViewdc,
					(int)C_Characters[i]->GetPosX() + 1,
					(int)C_Characters[i]->GetPosY() + CHAR_HEIGHT + 1,
					C_Characters[i]->GetLife(), LIFE_HEIGHT - 2,
					0, 0, CHAR_WIDTH - 2, LIFE_HEIGHT - 2
				);
				if (VisibleCollisionBox)
				{
					i_Char_Col.AlphaBlend(hViewdc,
						(int)C_Characters[i]->GetPosX(),
						(int)C_Characters[i]->GetPosY(),
						CHAR_WIDTH, CHAR_HEIGHT,
						0, 0, CHAR_WIDTH, CHAR_HEIGHT
					);
				}
				break;
			case ObjectsType::MAGICIAN:
				i_Magician.AlphaBlend(hViewdc,
					(int)C_Characters[i]->GetPosX(),
					(int)C_Characters[i]->GetPosY(),
					CHAR_WIDTH, CHAR_HEIGHT,
					C_Characters[i]->GetAnimCount() * CHAR_WIDTH,
					C_Characters[i]->GetAnimState() * CHAR_HEIGHT,
					CHAR_WIDTH, CHAR_HEIGHT
				);
				i_LifeGaugeBG.AlphaBlend(hViewdc,
					(int)C_Characters[i]->GetPosX(),
					(int)C_Characters[i]->GetPosY() + CHAR_HEIGHT,
					CHAR_WIDTH, LIFE_HEIGHT,
					0, 0, CHAR_WIDTH, LIFE_HEIGHT
				);
				i_LifeGauge.AlphaBlend(hViewdc,
					(int)C_Characters[i]->GetPosX() + 1,
					(int)C_Characters[i]->GetPosY() + CHAR_HEIGHT + 1,
					C_Characters[i]->GetLife(), LIFE_HEIGHT - 2,
					0, 0, CHAR_WIDTH - 2, LIFE_HEIGHT - 2
				);
				if (VisibleCollisionBox)
				{
					i_Char_Col.AlphaBlend(hViewdc,
						(int)C_Characters[i]->GetPosX(),
						(int)C_Characters[i]->GetPosY(),
						CHAR_WIDTH, CHAR_HEIGHT,
						0, 0, CHAR_WIDTH, CHAR_HEIGHT
					);
				}
				break;
			default:
				printf("ĳ���� Ÿ�� ����\n");
				break;
			}
		}

		// ĳ���� ���� ���� �׷����� ��� �׸���
		i_Upground.AlphaBlend(hViewdc,
			0, 0, WIN_WIDTH, WIN_HEIGHT, 0, 0, WIN_WIDTH, WIN_HEIGHT);


		// �߻�ü �׸���
		for (int i = 0; i < BULLETS_MAX_NUM; ++i)
		{
			switch (C_Bullets[i]->GetType())
			{
			case ObjectsType::NONE:
				break;
			case ObjectsType::BOOM:
				i_Boom.AlphaBlend(hViewdc,
					(int)C_Bullets[i]->GetPosX(),
					(int)C_Bullets[i]->GetPosY(),
					BULLET_SIZE, BULLET_SIZE,
					C_Bullets[i]->GetAnimCount() * BULLET_SIZE,
					0,
					BULLET_SIZE, BULLET_SIZE
				);
				if (VisibleCollisionBox)
				{
					i_Bullet_Col.AlphaBlend(hViewdc,
						(int)C_Bullets[i]->GetPosX(),
						(int)C_Bullets[i]->GetPosY(),
						BULLET_SIZE, BULLET_SIZE,
						0, 0, BULLET_SIZE, BULLET_SIZE
					);
				}
				break;
			case ObjectsType::BULLET:
				i_Bullet.AlphaBlend(hViewdc,
					(int)C_Bullets[i]->GetPosX(),
					(int)C_Bullets[i]->GetPosY(),
					BULLET_SIZE, BULLET_SIZE,
					C_Bullets[i]->GetAnimCount() * BULLET_SIZE,
					0,
					BULLET_SIZE, BULLET_SIZE
				);
				if (VisibleCollisionBox)
				{
					i_Bullet_Col.AlphaBlend(hViewdc,
						(int)C_Bullets[i]->GetPosX(),
						(int)C_Bullets[i]->GetPosY(),
						BULLET_SIZE, BULLET_SIZE,
						0, 0, BULLET_SIZE, BULLET_SIZE
					);
				}
				break;
			case ObjectsType::FIRE:
				i_Fire.AlphaBlend(hViewdc,
					(int)C_Bullets[i]->GetPosX(),
					(int)C_Bullets[i]->GetPosY(),
					BULLET_SIZE, BULLET_SIZE,
					C_Bullets[i]->GetAnimCount() * BULLET_SIZE,
					0,
					BULLET_SIZE, BULLET_SIZE
				);
				if (VisibleCollisionBox)
				{
					i_Bullet_Col.AlphaBlend(hViewdc,
						(int)C_Bullets[i]->GetPosX(),
						(int)C_Bullets[i]->GetPosY(),
						BULLET_SIZE, BULLET_SIZE,
						0, 0, BULLET_SIZE, BULLET_SIZE
					);
				}
				break;
			default:
				printf("�߻�ü Ÿ�� ����\n");
				break;
			}
		}
	}
	else if (SceneState == SceneState::SELECT_CHARACTER)
	{
		// ��� �׸���
		i_Select_Character_View.AlphaBlend(hViewdc,
			0, 0, WIN_WIDTH, WIN_HEIGHT, 0, 0, WIN_WIDTH, WIN_HEIGHT);

		// ĳ���� �׸���
		i_Bomber_Select.AlphaBlend(hViewdc, 150, 600, 180, 180, 0, 0, 90, 90);
		i_Gunner_Select.AlphaBlend(hViewdc, 510, 600, 180, 180, 0, 0, 90, 90);
		i_Magician_Select.AlphaBlend(hViewdc, 870, 600, 180, 180, 0, 0, 90, 90);

		// ĳ���� ���� �ڽ� �׸���
		i_Select_Box_G.AlphaBlend(hViewdc, C_SelectBox_G, 600, 180, 180, 0, 0, 180, 180);
		i_Select_Box_Y.AlphaBlend(hViewdc, C_SelectBox_Y, 600, 180, 180, 0, 0, 180, 180);

	}
	else if (SceneState == MyID + 2)
	{
		i_Victory.AlphaBlend(hViewdc,
			0, 0, WIN_WIDTH, WIN_HEIGHT, 0, 0, WIN_WIDTH, WIN_HEIGHT);
	}
	else
	{
		i_Defeat.AlphaBlend(hViewdc,
			0, 0, WIN_WIDTH, WIN_HEIGHT, 0, 0, WIN_WIDTH, WIN_HEIGHT);
	}
}

CImage* SceneManager::GetBackground()
{
	return &i_Background;
}

void SceneManager::SetMyID(BYTE ID)
{
	MyID = ID;
}

int SceneManager::GetMyID() const
{
	return MyID;
}

int SceneManager::GetSceneState() const
{
	return SceneState;
}

void SceneManager::SetSceneState(PACKET_SceneState* scenestate)
{
	SceneState = scenestate->SceneState;
}

void SceneManager::SetC_SelectBox_G(int state)
{
	C_SelectBox_G = state;
}

int SceneManager::GetC_SelectBox_Y() const
{
	return C_SelectBox_Y;
}

void SceneManager::SetC_SelectBox_Y(int state)
{
	C_SelectBox_Y = state;
}

DWORD SceneManager::GetLastAtteckTime() const
{
	return LastAtteckTime;
}

void SceneManager::SetLastAtteckTime(DWORD time)
{
	LastAtteckTime = time;
}

DWORD SceneManager::GetMyCoolTime() const
{
	return MyCoolTime;
}

float SceneManager::GetMyPosX() const
{
	return C_Characters[MyID]->GetPosX();
}

float SceneManager::GetMyPosY() const
{
	return C_Characters[MyID]->GetPosY();
}

// �߻�ü �迭 ����
void SceneManager::ClearBulletArray()
{
	for (int i = 0; i < BULLETS_MAX_NUM; ++i)
	{
		C_Bullets[i]->SetTypeNone();
	}
}

// [�߻�ü ���� ��Ŷ] Ŭ���̾�Ʈ�� ����
void SceneManager::AddC_Bullet(PACKET_Bullet* bulletpacket)
{
	for (int i = 0; i < BULLETS_MAX_NUM; ++i)
	{
		if (C_Bullets[i]->GetType() == ObjectsType::NONE)
		{
			C_Bullets[i]->SetC_Bullet(bulletpacket);
			break;
		}
	}
}

// [ĳ���� ���� ��Ŷ] Ŭ���̾�Ʈ�� ����(�÷��̾� ID, ��Ŷ)
void SceneManager::SetC_Character(int playerID, PACKET_Character* charpacket)
{
	C_Characters[playerID]->SetC_Character(charpacket);
}

// �ø��� �ڽ� ON / OFF ��ȯ
void SceneManager::SwitchCollision()
{
	if (VisibleCollisionBox) VisibleCollisionBox = false;
	else VisibleCollisionBox = true;
}

// ��Ÿ�� ����
void SceneManager::SetMyCoolTime(int type)
{
	switch (type)
	{
	case ObjectsType::BOMBER:
		MyCoolTime = 1000;
		break;
	case ObjectsType::GUNNER:
		MyCoolTime = 750;
		break;
	case ObjectsType::MAGICIAN:
		MyCoolTime = 500;
		break;
	}
	
}