#include "stdafx.h"

SceneMgr::SceneMgr()
{
	renderer = new Renderer(WINDOWWIDTH, WINDOWHEIGHT);

	NowTime = timeGetTime();

	if (!renderer->IsInitialized())
	{
		std::cout << "Renderer could not be initialized.. \n";
	}

	for (int i = 0; i < MAX_OBJECTS_COUNT; ++i)
	{
		m_objects[i] = nullptr;
	}

	// TEAM1 Building Spawn
	AddObject(90, 170, BUILDING_T1, nullptr);
	AddObject(250, 90, BUILDING_T1, nullptr);
	AddObject(405, 170, BUILDING_T1, nullptr);

	// TEAM2 Building Spawn
	AddObject(90, 600, BUILDING_T2, nullptr);
	AddObject(250, 660, BUILDING_T2, nullptr);
	AddObject(405, 600, BUILDING_T2, nullptr);

	Building_TEAM1_Texture = renderer->CreatePngTexture("./Resource/Building_TEAM1.png");
	Building_TEAM2_Texture = renderer->CreatePngTexture("./Resource/Building_TEAM2.png");
	Character_TEAM1_Texture = renderer->CreatePngTexture("./Resource/Character_TEAM1.png");
	Character_TEAM2_Texture = renderer->CreatePngTexture("./Resource/Character_TEAM2.png");

	BackgroundImage = renderer->CreatePngTexture("./Resource/Background.png");
	ParticleImage = renderer->CreatePngTexture("./Resource/Particle.png");
	SnowImage = renderer->CreatePngTexture("./Resource/Snow.png");

	TEAM1_ChracterRespawnTime = 0;
	TEAM2_ChracterRespawnTime = T2_CHARACTER_RESPAWN_TIME;
}

SceneMgr::~SceneMgr()
{
	delete renderer;

	for (int i = 0; i < MAX_OBJECTS_COUNT; ++i)
	{
		if (m_objects[i] == nullptr) continue;

		delete m_objects[i];
		m_objects[i] = nullptr;
	}
}

void SceneMgr::AddObject(const int x, const int y, ObjectType E_Type, Object* ObjectPointer)
{
	for (int i = 0; i < MAX_OBJECTS_COUNT; ++i)
	{
		if (m_objects[i] != nullptr) continue;

		switch (E_Type)
		{
		case BUILDING_T1:
		case BUILDING_T2:
			m_objects[i] = new BuildingObject((float)x, (float)y, E_Type, ObjectPointer);
			break;
		case CHARACTER_T1:
		case CHARACTER_T2:
			m_objects[i] = new CharacterObject((float)x, (float)y, E_Type, ObjectPointer);
			break;
		case BULLET_T1:
		case BULLET_T2:
			m_objects[i] = new BulletObject((float)x, (float)y, E_Type, ObjectPointer);
			break;
		case ARROW_T1:
		case ARROW_T2:
			m_objects[i] = new ArrowObject((float)x, (float)y, E_Type, ObjectPointer);
			break;
		}
		break;
	}
}

DWORD SceneMgr::ObjectsUpdate(DWORD deltaTime)
{
	DWORD OldTime = NowTime;

	float FrameSeconds = deltaTime / 1000.0f;

	LifeAndLifeTimeCheck();

	// Character Respawn Time Update
	TEAM1_ChracterRespawnTime = TEAM1_ChracterRespawnTime + deltaTime;
	if (CheckTeam1CharacterRespawnTime())
	{
		AddObject(
			rand() % WINDOWWIDTH,
			rand() % (WINDOWHEIGHT / 2),
			CHARACTER_T1, nullptr);
	}
	TEAM2_ChracterRespawnTime = TEAM2_ChracterRespawnTime + deltaTime;

	
	for (int i = 0; i < MAX_OBJECTS_COUNT; ++i)
	{
		if (m_objects[i] == nullptr) continue;

		m_objects[i]->update(FrameSeconds);

		if (m_objects[i]->getType() == BULLET_T1 ||
			m_objects[i]->getType() == BULLET_T2)
		{
			((BulletObject*)m_objects[i])->AddParticleTime(deltaTime);
		}

		// Bullet & Arrow Respawn Check
		if (m_objects[i]->getType() == BUILDING_T1)
		{
			if (((BuildingObject*)m_objects[i])->CheckBulletRespawnTime(deltaTime))
			{
				AddObject(
					(int)m_objects[i]->getPositionX(),
					(int)m_objects[i]->getPositionY(),
					BULLET_T1, m_objects[i]);
			}
		}
		else if (m_objects[i]->getType() == BUILDING_T2)
		{
			if (((BuildingObject*)m_objects[i])->CheckBulletRespawnTime(deltaTime))
			{
				AddObject(
					(int)m_objects[i]->getPositionX(),
					(int)m_objects[i]->getPositionY(),
					BULLET_T2, m_objects[i]);
			}
		}
		else if (m_objects[i]->getType() == CHARACTER_T1)
		{
			if (((CharacterObject*)m_objects[i])->CheckArrowRespawnTime(deltaTime))
			{
				AddObject(
					(int)m_objects[i]->getPositionX(),
					(int)m_objects[i]->getPositionY(),
					ARROW_T1, m_objects[i]);
			}
		}
		else if (m_objects[i]->getType() == CHARACTER_T2)
		{
			if (((CharacterObject*)m_objects[i])->CheckArrowRespawnTime(deltaTime))
			{
				AddObject(
					(int)m_objects[i]->getPositionX(),
					(int)m_objects[i]->getPositionY(),
					ARROW_T2, m_objects[i]);
			}
		}

		// 화면 밖으로 나간 Object 체크
		if (m_objects[i]->checkOutOfView())
		{
			delete m_objects[i];
			m_objects[i] = nullptr;
		}
	}

	CollisionCheck();

	Draw();

	NowTime = timeGetTime();

	return NowTime - OldTime;
}

void SceneMgr::LifeAndLifeTimeCheck()
{
	for (int i = 0; i < MAX_OBJECTS_COUNT; ++i)
	{
		if (m_objects[i] == nullptr) continue;

		if ((m_objects[i]->getLife() <= 0.0f) || (m_objects[i]->getLifeTime() <= 0.0f))
		{
			delete m_objects[i];
			m_objects[i] = nullptr;
		}
	}
}

void SceneMgr::CollisionCheck()
{
	for (int i = 0; i < MAX_OBJECTS_COUNT; ++i)
	{
		for (int j = i + 1; j < MAX_OBJECTS_COUNT; ++j)
		{
			if ((m_objects[i] == nullptr) ||( m_objects[j] == nullptr)) continue;

			if (IsCollistion(m_objects[i], m_objects[j]))
			{
				if (m_objects[i]->getType() == BUILDING_T1)
				{
					if ((m_objects[j]->getType() == CHARACTER_T2)
						|| (m_objects[j]->getType() == BULLET_T2)
						|| (m_objects[j]->getType() == ARROW_T2))
					{
						m_objects[i]->decreaseLife(m_objects[j]->getLife());
						delete m_objects[j];
						m_objects[j] = nullptr;
					}
				}
				else if (m_objects[i]->getType() == BUILDING_T2)
				{
					if (m_objects[j]->getType() == CHARACTER_T1
						|| (m_objects[j]->getType() == BULLET_T1)
						|| (m_objects[j]->getType() == ARROW_T1))
					{
						m_objects[i]->decreaseLife(m_objects[j]->getLife());
						delete m_objects[j];
						m_objects[j] = nullptr;
					}
				}
				else if (m_objects[i]->getType() == CHARACTER_T1)
				{
					if ((m_objects[j]->getType() == BULLET_T2)
						|| (m_objects[j]->getType() == ARROW_T2))
					{
						m_objects[i]->decreaseLife(m_objects[j]->getLife());
						delete m_objects[j];
						m_objects[j] = nullptr;
					}
					else if (m_objects[j]->getType() == BUILDING_T2)
					{
						m_objects[j]->decreaseLife(m_objects[i]->getLife());
						delete m_objects[i];
						m_objects[i] = nullptr;
					}
				}
				else if (m_objects[i]->getType() == CHARACTER_T2)
				{
					if ((m_objects[j]->getType() == BULLET_T1)
						|| (m_objects[j]->getType() == ARROW_T1))
					{
						m_objects[i]->decreaseLife(m_objects[j]->getLife());
						delete m_objects[j];
						m_objects[j] = nullptr;
					}
					else if (m_objects[j]->getType() == BUILDING_T1)
					{
						m_objects[j]->decreaseLife(m_objects[i]->getLife());
						delete m_objects[i];
						m_objects[i] = nullptr;
					}
				}
				else if (m_objects[i]->getType() == BULLET_T1)
				{
					if ((m_objects[j]->getType() == CHARACTER_T2)
						|| (m_objects[j]->getType() == BUILDING_T2))
					{
						m_objects[j]->decreaseLife(m_objects[i]->getLife());
						delete m_objects[i];
						m_objects[i] = nullptr;
					}
				}
				else if (m_objects[i]->getType() == BULLET_T2)
				{
					if ((m_objects[j]->getType() == CHARACTER_T1)
						|| (m_objects[j]->getType() == BUILDING_T1))
					{
						m_objects[j]->decreaseLife(m_objects[i]->getLife());
						delete m_objects[i];
						m_objects[i] = nullptr;
					}
				}
				else if (m_objects[i]->getType() == ARROW_T1)
				{
					if ((m_objects[j]->getType() == CHARACTER_T2)
						|| (m_objects[j]->getType() == BUILDING_T2))
					{
						m_objects[j]->decreaseLife(m_objects[i]->getLife());
						delete m_objects[i];
						m_objects[i] = nullptr;
					}
				}
				else if (m_objects[i]->getType() == ARROW_T2)
				{
					if ((m_objects[j]->getType() == CHARACTER_T1)
						|| (m_objects[j]->getType() == BUILDING_T1))
					{
						m_objects[j]->decreaseLife(m_objects[i]->getLife());
						delete m_objects[i];
						m_objects[i] = nullptr;
					}
				}
			}
		}
	}
}

bool SceneMgr::IsCollistion(const Object* front, const Object* back) const
{
	if (front->getMaxX() < back->getMinX()) return false;
	if (front->getMinX() > back->getMaxX()) return false;

	if (front->getMaxY() < back->getMinY()) return false;
	if (front->getMinY() > back->getMaxY()) return false;
	
	return true;
}

void SceneMgr::Draw() const
{
	renderer->DrawTexturedRect(0, 0, 0, WINDOWHEIGHT, 1.0f, 1.0f, 1.0f, 1.0f, BackgroundImage, LEVEL_BACKGROUND);

	for (int i = 0; i < MAX_OBJECTS_COUNT; ++i)
	{
		if (m_objects[i] == nullptr) continue;

		if (m_objects[i]->getType() == BUILDING_T1)
		{
			renderer->DrawTexturedRect(
				m_objects[i]->getPositionX() - (WINDOWWIDTH / 2),
				(WINDOWHEIGHT / 2) - m_objects[i]->getPositionY(),
				m_objects[i]->getPositionZ(),
				m_objects[i]->getSize(),
				m_objects[i]->getColorRed(),
				m_objects[i]->getColorGreen(),
				m_objects[i]->getColorBlue(),
				m_objects[i]->getColorAlpha(),
				Building_TEAM1_Texture,
				LEVEL_BUILDING
			);
			renderer->DrawSolidRectGauge(
				m_objects[i]->getPositionX() - (WINDOWWIDTH / 2),
				(WINDOWHEIGHT / 2) - m_objects[i]->getPositionY() - (m_objects[i]->getSize() / 1.5f),
				m_objects[i]->getPositionZ(),
				m_objects[i]->getSize(),
				LIFE_GAUGE_HEIGHT,
				1.0f, 0.0f, 0.0f, 1.0f,
				m_objects[i]->getLife() / BUILDING_MAX_LIFE,
				LEVEL_BUILDING
			);
			char lifestr[20];
			sprintf_s(lifestr, "%.1f", m_objects[i]->getLife());
			renderer->DrawTextW(
				m_objects[i]->getPositionX() - (WINDOWWIDTH / 2.0f) + m_objects[i]->getSize() / 2.0f,
				(WINDOWHEIGHT / 2) - m_objects[i]->getPositionY() - (m_objects[i]->getSize() / 1.5f),
				LIFE_FONT, 0.0f, 0.0f, 0.0f, lifestr
			);
		}
		else if (m_objects[i]->getType() == BUILDING_T2)
		{
			renderer->DrawTexturedRect(
				m_objects[i]->getPositionX() - (WINDOWWIDTH / 2),
				(WINDOWHEIGHT / 2) - m_objects[i]->getPositionY(),
				m_objects[i]->getPositionZ(),
				m_objects[i]->getSize(),
				m_objects[i]->getColorRed(),
				m_objects[i]->getColorGreen(),
				m_objects[i]->getColorBlue(),
				m_objects[i]->getColorAlpha(),
				Building_TEAM2_Texture,
				LEVEL_BUILDING
			);
			renderer->DrawSolidRectGauge(
				m_objects[i]->getPositionX() - (WINDOWWIDTH / 2),
				(WINDOWHEIGHT / 2) - m_objects[i]->getPositionY() - (m_objects[i]->getSize() / 1.5f),
				m_objects[i]->getPositionZ(),
				m_objects[i]->getSize(),
				LIFE_GAUGE_HEIGHT,
				0.0f, 0.0f, 1.0f, 1.0f,
				m_objects[i]->getLife() / BUILDING_MAX_LIFE,
				LEVEL_BUILDING
			);
			char lifestr[20];
			sprintf_s(lifestr, "%.1f", m_objects[i]->getLife());
			renderer->DrawTextW(
				m_objects[i]->getPositionX() - (WINDOWWIDTH / 2.0f) + m_objects[i]->getSize() / 2.0f,
				(WINDOWHEIGHT / 2) - m_objects[i]->getPositionY() - (m_objects[i]->getSize() / 1.5f),
				LIFE_FONT, 0.0f, 0.0f, 0.0f, lifestr
			);
		}
		else if (m_objects[i]->getType() == CHARACTER_T1)
		{
			renderer->DrawTexturedRectSeq(
				m_objects[i]->getPositionX() - (WINDOWWIDTH / 2),
				(WINDOWHEIGHT / 2) - m_objects[i]->getPositionY(),
				m_objects[i]->getPositionZ(),
				m_objects[i]->getSize(),
				m_objects[i]->getColorRed(),
				m_objects[i]->getColorGreen(),
				m_objects[i]->getColorBlue(),
				m_objects[i]->getColorAlpha(),
				Character_TEAM1_Texture,
				m_objects[i]->getAniCount(),
				m_objects[i]->getAniState(),
				4, 4, LEVEL_CHARACTER
			);
			renderer->DrawSolidRectGauge(
				m_objects[i]->getPositionX() - (WINDOWWIDTH / 2),
				(WINDOWHEIGHT / 2) - m_objects[i]->getPositionY() - (m_objects[i]->getSize() / 1.5f),
				m_objects[i]->getPositionZ(),
				m_objects[i]->getSize(),
				LIFE_GAUGE_HEIGHT,
				1.0f, 0.0f, 0.0f, 1.0f,
				m_objects[i]->getLife() / CHARACTER_MAX_LIFE,
				LEVEL_CHARACTER
			);
			char lifestr[20];
			sprintf_s(lifestr, "%.1f", m_objects[i]->getLife());
			renderer->DrawTextW(
				m_objects[i]->getPositionX() - (WINDOWWIDTH / 2.0f) + m_objects[i]->getSize() / 2.0f,
				(WINDOWHEIGHT / 2) - m_objects[i]->getPositionY() - (m_objects[i]->getSize() / 1.5f),
				LIFE_FONT, 0.0f, 0.0f, 0.0f, lifestr
			);
		}
		else if (m_objects[i]->getType() == CHARACTER_T2)
		{
			renderer->DrawTexturedRectSeq(
				m_objects[i]->getPositionX() - (WINDOWWIDTH / 2),
				(WINDOWHEIGHT / 2) - m_objects[i]->getPositionY(),
				m_objects[i]->getPositionZ(),
				m_objects[i]->getSize(),
				m_objects[i]->getColorRed(),
				m_objects[i]->getColorGreen(),
				m_objects[i]->getColorBlue(),
				m_objects[i]->getColorAlpha(),
				Character_TEAM2_Texture,
				m_objects[i]->getAniCount(),
				m_objects[i]->getAniState(),
				4, 4, LEVEL_CHARACTER
			);
			renderer->DrawSolidRectGauge(
				m_objects[i]->getPositionX() - (WINDOWWIDTH / 2),
				(WINDOWHEIGHT / 2) - m_objects[i]->getPositionY() - (m_objects[i]->getSize() / 1.5f),
				m_objects[i]->getPositionZ(),
				m_objects[i]->getSize(),
				LIFE_GAUGE_HEIGHT,
				0.0f, 0.0f, 1.0f, 1.0f,
				m_objects[i]->getLife() / CHARACTER_MAX_LIFE,
				LEVEL_CHARACTER
			);
			char lifestr[20];
			sprintf_s(lifestr, "%.1f", m_objects[i]->getLife());
			renderer->DrawTextW(
				m_objects[i]->getPositionX() - (WINDOWWIDTH / 2.0f) + m_objects[i]->getSize() / 2.0f,
				(WINDOWHEIGHT / 2) - m_objects[i]->getPositionY() - (m_objects[i]->getSize() / 1.5f),
				LIFE_FONT, 0.0f, 0.0f, 0.0f, lifestr
			);
		}
		else if (m_objects[i]->getType() == BULLET_T1)
		{
			renderer->DrawSolidRect(
				m_objects[i]->getPositionX() - (WINDOWWIDTH / 2),
				(WINDOWHEIGHT / 2) - m_objects[i]->getPositionY(),
				m_objects[i]->getPositionZ(),
				m_objects[i]->getSize(),
				m_objects[i]->getColorRed(),
				m_objects[i]->getColorGreen(),
				m_objects[i]->getColorBlue(),
				m_objects[i]->getColorAlpha(),
				LEVEL_BULLET
			);
			renderer->DrawParticle(
				m_objects[i]->getPositionX() - (WINDOWWIDTH / 2),
				(WINDOWHEIGHT / 2) - m_objects[i]->getPositionY(),
				m_objects[i]->getPositionZ(),
				m_objects[i]->getSize(),
				1.0f, 1.0f, 0.0f, 1.0f,
				m_objects[i]->getDirection().X * -1.0f,
				m_objects[i]->getDirection().Y * 1.0f,
				ParticleImage,
				((BulletObject*)m_objects[i])->GetParticleTime() / 500.0f,
				LEVEL_PARTICLE
			);
		}
		else if (m_objects[i]->getType() == BULLET_T2)
		{
			renderer->DrawSolidRect(
				m_objects[i]->getPositionX() - (WINDOWWIDTH / 2),
				(WINDOWHEIGHT / 2) - m_objects[i]->getPositionY(),
				m_objects[i]->getPositionZ(),
				m_objects[i]->getSize(),
				m_objects[i]->getColorRed(),
				m_objects[i]->getColorGreen(),
				m_objects[i]->getColorBlue(),
				m_objects[i]->getColorAlpha(),
				LEVEL_BULLET
			);
			renderer->DrawParticle(
				m_objects[i]->getPositionX() - (WINDOWWIDTH / 2),
				(WINDOWHEIGHT / 2) - m_objects[i]->getPositionY(),
				m_objects[i]->getPositionZ(),
				m_objects[i]->getSize(),
				1.0f, 1.0f, 0.0f, 1.0f,
				m_objects[i]->getDirection().X * -1.0f,
				m_objects[i]->getDirection().Y * 1.0f,
				ParticleImage,
				((BulletObject*)m_objects[i])->GetParticleTime() / 500.0f,
				LEVEL_PARTICLE
			);
		}
		else
		{
			renderer->DrawSolidRect(
				m_objects[i]->getPositionX() - (WINDOWWIDTH / 2),
				(WINDOWHEIGHT / 2) - m_objects[i]->getPositionY(),
				m_objects[i]->getPositionZ(),
				m_objects[i]->getSize(),
				m_objects[i]->getColorRed(),
				m_objects[i]->getColorGreen(),
				m_objects[i]->getColorBlue(),
				m_objects[i]->getColorAlpha(),
				LEVEL_BULLET
			);
		}
	}

	renderer->DrawParticleClimate(
		0.0f, 0.0f, 0.0f, 2.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, SnowImage, timeGetTime() / 1000.0f, 0.01f
	);
}

bool SceneMgr::CheckTeam1CharacterRespawnTime()
{
	if (TEAM1_ChracterRespawnTime > T1_CHARACTER_RESPAWN_TIME)
	{
		TEAM1_ChracterRespawnTime = TEAM1_ChracterRespawnTime - T1_CHARACTER_RESPAWN_TIME;
		return true;
	}
	return false;
}

DWORD SceneMgr::GetTeam2CharacterRespawnTime() const
{
	return TEAM2_ChracterRespawnTime;
}

void SceneMgr::ResetTeam2CharacterRespawnTime()
{
	TEAM2_ChracterRespawnTime = 0;
}