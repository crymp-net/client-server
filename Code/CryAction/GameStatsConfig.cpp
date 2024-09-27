#include <cstdint>

#include "GameStatsConfig.h"

extern std::uintptr_t CRYACTION_BASE;

GameStatsConfig::GameStatsConfig()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x2e2a90;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x1f56b0;
#endif

	(this->*reinterpret_cast<void(GameStatsConfig::*&)()>(ctor))();
}

GameStatsConfig::~GameStatsConfig()
{
}

void GameStatsConfig::Init()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x2e2cf0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x1f5740;
#endif

	(this->*reinterpret_cast<void(GameStatsConfig::*&)()>(func))();
}

////////////////////////////////////////////////////////////////////////////////
// IGameStatsConfig
////////////////////////////////////////////////////////////////////////////////

int GameStatsConfig::GetStatsVersion()
{
	return {};
}

int GameStatsConfig::GetCategoryMod(const char* cat)
{
	return {};
}

const char* GameStatsConfig::GetValueNameByCode(const char* cat, int id)
{
	return {};
}

////////////////////////////////////////////////////////////////////////////////
