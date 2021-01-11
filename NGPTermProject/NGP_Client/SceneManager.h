#pragma once
#include <atlimage.h>
#include "C_Character.h"

#define CLIENT_NUM 2
#define BULLETS_MAX_NUM 100

#define WIN_WIDTH 1200
#define WIN_HEIGHT 990

#define CHAR_WIDTH 48
#define CHAR_HEIGHT 66

#define LIFE_HEIGHT 5

#define BULLET_SIZE 24

enum SceneState { SELECT_CHARACTER, IN_GAME, PLAYER1WIN, PLAYER2WIN, PLAYER3WIN };

// [�� ���� ��Ŷ]
struct PACKET_SceneState
{
	BYTE SceneState;
};

class SceneManager
{
	C_Character* C_Characters[CLIENT_NUM];
	C_Bullet* C_Bullets[BULLETS_MAX_NUM];

	// ĳ���� ���� �� �̹���
	CImage i_Select_Character_View;
	CImage i_Bomber_Select, i_Gunner_Select, i_Magician_Select;
	CImage i_Select_Box_G, i_Select_Box_Y;

	// �� ���� �� �̹���
	CImage i_Background, i_Upground, i_LifeGauge, i_LifeGaugeBG;
	CImage i_Bomber, i_Gunner, i_Magician;
	CImage i_Boom, i_Bullet, i_Fire;

	// �¸� �й� �� �̹���
	CImage i_Victory, i_Defeat;

	// �浹 �ڽ� �̹���
	CImage i_Char_Col, i_Bullet_Col;

	// �� �Ŵ��� ���� ����
	int MyID;
	int SceneState;
	int C_SelectBox_G;
	int C_SelectBox_Y;
	bool VisibleCollisionBox;

	// �߻� �õ� ��Ÿ�� ����
	DWORD LastAtteckTime;
	DWORD MyCoolTime;

public:
	SceneManager();
	~SceneManager();

	void ImageAlphaSetting(CImage*);
	void Render(HDC);
	CImage* GetBackground();

	void ResetClient();

	void SetMyID(BYTE);
	int GetMyID() const;

	int GetSceneState() const;
	void SetSceneState(PACKET_SceneState*);

	void SetC_SelectBox_G(int);
	int GetC_SelectBox_Y() const;
	void SetC_SelectBox_Y(int);

	DWORD GetLastAtteckTime() const;
	void SetLastAtteckTime(DWORD);

	DWORD GetMyCoolTime() const;

	float GetMyPosX() const;
	float GetMyPosY() const;

	// �߻�ü �迭 ����
	void ClearBulletArray();

	// [�߻�ü ���� ��Ŷ] Ŭ���̾�Ʈ�� ����
	void AddC_Bullet(PACKET_Bullet*);

	// [ĳ���� ���� ��Ŷ] Ŭ���̾�Ʈ�� ����(�÷��̾� ID, ��Ŷ)
	void SetC_Character(int, PACKET_Character*);

	// �ø��� �ڽ� ON / OFF ��ȯ
	void SwitchCollision();

	// ��Ÿ�� ����
	void SetMyCoolTime(int);
};

