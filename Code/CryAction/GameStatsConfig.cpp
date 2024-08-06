#include <cstdint>

#include "GameStatsConfig.h"

GameStatsConfig::GameStatsConfig()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x307e2a90;
#else
	std::uintptr_t ctor = 0x306f56b0;
#endif

	(this->*reinterpret_cast<void(GameStatsConfig::*&)()>(ctor))();
}

GameStatsConfig::~GameStatsConfig()
{
}

void GameStatsConfig::Init()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x307e2cf0;
#else
	std::uintptr_t func = 0x306f5740;
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
