#include <cstdint>

#include "GameTokenSystem.h"

GameTokenSystem::GameTokenSystem()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x3077bb70;
#else
	std::uintptr_t ctor = 0x306b3ef0;
#endif

	(this->*reinterpret_cast<void(GameTokenSystem::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IGameFramework
////////////////////////////////////////////////////////////////////////////////

void GameTokenSystem::GetMemoryStatistics(ICrySizer*)
{
}

IGameToken* GameTokenSystem::SetOrCreateToken(const char* tokenName, const TFlowInputData& defaultValue)
{
	return nullptr;
}

void GameTokenSystem::DeleteToken(IGameToken* pToken)
{
}

IGameToken* GameTokenSystem::FindToken(const char* tokenName)
{
	return nullptr;
}

void GameTokenSystem::RenameToken(IGameToken* pToken, const char* newTokenName)
{
}

void GameTokenSystem::RegisterListener(const char* gameToken, IGameTokenEventListener* pListener, bool forceCreate, bool immediateCallback)
{
}

void GameTokenSystem::UnregisterListener(const char* gameToken, IGameTokenEventListener* pListener)
{
}

void GameTokenSystem::RegisterListener(IGameTokenEventListener* pListener)
{
}

void GameTokenSystem::UnregisterListener(IGameTokenEventListener* pListener)
{
}

void GameTokenSystem::LoadLibs(const char* fileSpec)
{
}

void GameTokenSystem::RemoveLibrary(const char* prefix)
{
}

void GameTokenSystem::Reset()
{
}

void GameTokenSystem::Serialize(TSerialize ser)
{
}

void GameTokenSystem::DebugDraw()
{
}

////////////////////////////////////////////////////////////////////////////////
