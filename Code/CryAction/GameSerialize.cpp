#include <cstdint>

#include "GameSerialize.h"

extern std::uintptr_t CRYACTION_BASE;

GameSerialize::GameSerialize()
{
	// TODO: this is not good, implement this class ASAP
	// probably some C++ allocator stuff
#ifdef BUILD_64BIT
	void* magicValue = reinterpret_cast<void*>(sizeof(GameSerialize));
#else
	void* magicValue = reinterpret_cast<void*>(reinterpret_cast<unsigned int>(this) >> 24);
#endif

	m_reserved[2] = &m_reserved[0];
	m_reserved[3] = &m_reserved[0];
	m_reserved[5] = magicValue;
	m_reserved[8] = &m_reserved[6];
	m_reserved[9] = &m_reserved[6];
	m_reserved[11] = magicValue;
}

void GameSerialize::RegisterFactories(IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x1a3510;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x124170;
#endif

	(this->*reinterpret_cast<void(GameSerialize::*&)(IGameFramework*)>(func))(pGameFramework);
}

void GameSerialize::RegisterLoadGameFactory(const char* name, ILoadGame* (*pCreator)())
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x19E2A0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x120240;
#endif

	(this->*reinterpret_cast<void(GameSerialize::*&)(const char*, ILoadGame* (*)())>(func))(name, pCreator);
}

void GameSerialize::RegisterSaveGameFactory(const char* name, ISaveGame* (*pCreator)())
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x19E0E0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x120190;
#endif

	(this->*reinterpret_cast<void(GameSerialize::*&)(const char*, ISaveGame* (*)())>(func))(name, pCreator);
}

bool GameSerialize::SaveGame(GameFramework* pGameFramework, const char* method, const char* name,
	ESaveGameReason reason, const char* checkPoint)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x19E460;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x1202F0;
#endif

	return (this->*reinterpret_cast<bool(GameSerialize::*&)
		(GameFramework*, const char*, const char*, ESaveGameReason, const char*)>(func))
		(pGameFramework, method, name, reason, checkPoint);
}

GameSerialize::LoadResult GameSerialize::LoadGame(GameFramework* pGameFramework, const char* method,
	const char* path, SGameStartParams& params, bool requireQuickLoad, bool loadingSaveGame)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x1A0A40;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x1215D0;
#endif

	return (this->*reinterpret_cast<LoadResult(GameSerialize::*&)
		(GameFramework*, const char*, const char*, SGameStartParams&, bool, bool)>(func))
		(pGameFramework, method, path, params, requireQuickLoad, loadingSaveGame);
}
