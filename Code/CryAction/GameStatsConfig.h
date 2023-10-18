#pragma once

#include "CryCommon/CryAction/IGameFramework.h"

class GameStatsConfig : public IGameStatsConfig
{
	void* m_reserved[11 - 1] = {};

public:
	GameStatsConfig();
	~GameStatsConfig() override;

	void Init();

	////////////////////////////////////////////////////////////////////////////////
	// IGameStatsConfig
	////////////////////////////////////////////////////////////////////////////////

	int GetStatsVersion() override;
	int GetCategoryMod(const char* cat) override;
	const char* GetValueNameByCode(const char* cat, int id) override;

	////////////////////////////////////////////////////////////////////////////////
};
