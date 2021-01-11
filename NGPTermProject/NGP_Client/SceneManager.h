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

// [씬 상태 패킷]
struct PACKET_SceneState
{
	BYTE SceneState;
};

class SceneManager
{
	C_Character* C_Characters[CLIENT_NUM];
	C_Bullet* C_Bullets[BULLETS_MAX_NUM];

	// 캐릭터 선택 씬 이미지
	CImage i_Select_Character_View;
	CImage i_Bomber_Select, i_Gunner_Select, i_Magician_Select;
	CImage i_Select_Box_G, i_Select_Box_Y;

	// 인 게임 씬 이미지
	CImage i_Background, i_Upground, i_LifeGauge, i_LifeGaugeBG;
	CImage i_Bomber, i_Gunner, i_Magician;
	CImage i_Boom, i_Bullet, i_Fire;

	// 승리 패배 씬 이미지
	CImage i_Victory, i_Defeat;

	// 충돌 박스 이미지
	CImage i_Char_Col, i_Bullet_Col;

	// 씬 매니저 상태 변수
	int MyID;
	int SceneState;
	int C_SelectBox_G;
	int C_SelectBox_Y;
	bool VisibleCollisionBox;

	// 발사 시도 쿨타임 변수
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

	// 발사체 배열 비우기
	void ClearBulletArray();

	// [발사체 정보 패킷] 클라이언트에 복사
	void AddC_Bullet(PACKET_Bullet*);

	// [캐릭터 정보 패킷] 클라이언트에 복사(플레이어 ID, 패킷)
	void SetC_Character(int, PACKET_Character*);

	// 컬리전 박스 ON / OFF 전환
	void SwitchCollision();

	// 쿨타임 적용
	void SetMyCoolTime(int);
};

