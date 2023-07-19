#include <cstdint>

#include "GameRulesSystem.h"

GameRulesSystem::GameRulesSystem(ISystem* pSystem, IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x306b7f60;
#else
	std::uintptr_t ctor = 0x30631b00;
#endif

	(this->*reinterpret_cast<void(GameRulesSystem::*&)(ISystem*, IGameFramework*)>(ctor))(pSystem, pGameFramework);
}

////////////////////////////////////////////////////////////////////////////////
// IGameRulesSystem
////////////////////////////////////////////////////////////////////////////////

bool GameRulesSystem::RegisterGameRules(const char* rulesName, const char* extensionName)
{
	return false;
}

bool GameRulesSystem::CreateGameRules(const char* rulesName)
{
	return false;
}

bool GameRulesSystem::DestroyGameRules()
{
	return false;
}

void GameRulesSystem::AddGameRulesAlias(const char* rulesName, const char* alias)
{
}

void GameRulesSystem::AddGameRulesLevelLocation(const char* rulesName, const char* mapLocation)
{
}

const char* GameRulesSystem::GetGameRulesLevelLocation(const char* rulesName, int index)
{
	return nullptr;
}

const char* GameRulesSystem::GetGameRulesName(const char* alias) const
{
	return nullptr;
}

bool GameRulesSystem::HaveGameRules(const char* rulesName)
{
	return false;
}

void GameRulesSystem::SetCurrentGameRules(IGameRules* pGameRules)
{
}

IGameRules* GameRulesSystem::GetCurrentGameRules() const
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
