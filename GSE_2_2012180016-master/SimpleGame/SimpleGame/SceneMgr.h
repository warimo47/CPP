#pragma once

#define MAX_OBJECTS_COUNT			500
#define MAX_BUILDINGOBJECTS_COUNT	1
#define T1_CHARACTER_RESPAWN_TIME	5000
#define T2_CHARACTER_RESPAWN_TIME	7000

#define LEVEL_BUILDING				0.1f
#define LEVEL_CHARACTER				0.2f
#define LEVEL_BULLET				0.3f
#define LEVEL_ARROW					0.3f
#define LEVEL_PARTICLE				0.4f
#define LEVEL_BACKGROUND			0.9f

#define LIFE_GAUGE_HEIGHT			3.0f

#define LIFE_FONT GLUT_BITMAP_HELVETICA_12

enum ObjectType { CHARACTER_T1, BUILDING_T1, BULLET_T1, ARROW_T1, CHARACTER_T2, BUILDING_T2, BULLET_T2, ARROW_T2 };

class SceneMgr
{
	Object* m_objects[MAX_OBJECTS_COUNT];
	Renderer* renderer;
	DWORD NowTime;

	GLuint BackgroundImage;
	GLuint Building_TEAM1_Texture;
	GLuint Building_TEAM2_Texture;
	GLuint Character_TEAM1_Texture;
	GLuint Character_TEAM2_Texture;
	GLuint ParticleImage;
	GLuint SnowImage;

	DWORD TEAM1_ChracterRespawnTime;
	DWORD TEAM2_ChracterRespawnTime;
	
public:
	SceneMgr();
	~SceneMgr();

	void AddObject(const int, const int, const ObjectType, Object*);
	DWORD ObjectsUpdate(const DWORD deltaTime);

	void CollisionCheck();
	void LifeAndLifeTimeCheck();

	bool IsCollistion(const Object*, const Object*) const;

	void Draw() const;

	bool CheckTeam1CharacterRespawnTime();

	DWORD GetTeam2CharacterRespawnTime() const;

	void ResetTeam2CharacterRespawnTime();
};

