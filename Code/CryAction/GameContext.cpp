#include <cstdint>

#include "GameContext.h"

extern std::uintptr_t CRYACTION_BASE;

GameContext::GameContext()
{
#ifdef BUILD_64BIT
	static_assert(sizeof(GameContext) == 0x1a8);
	static_assert(offsetof(GameContext, m_pNetContext) == 0x70);
	static_assert(offsetof(GameContext, m_flags) == 0x140);
	static_assert(offsetof(GameContext, m_isInLevelLoad) == 0x150);
	static_assert(offsetof(GameContext, m_isGameStarted) == 0x151);
	static_assert(offsetof(GameContext, m_isLoadingSaveGame) == 0x152);
#else
	static_assert(sizeof(GameContext) == 0xe8);
	static_assert(offsetof(GameContext, m_pNetContext) == 0x38);
	static_assert(offsetof(GameContext, m_flags) == 0xa8);
	static_assert(offsetof(GameContext, m_isInLevelLoad) == 0xb4);
	static_assert(offsetof(GameContext, m_isGameStarted) == 0xb5);
	static_assert(offsetof(GameContext, m_isLoadingSaveGame) == 0xb6);
#endif

	// TODO
}

void GameContext::ResetMap(bool isServer)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x2BACB0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x1DAEC0;
#endif

	(this->*reinterpret_cast<void(GameContext::*&)(bool)>(func))(isServer);
}

bool GameContext::ClassIdFromName(std::uint16_t& id, const CryStringT<char>& name) const
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x2B3860;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x1D6850;
#endif

	return (this->*reinterpret_cast<bool(GameContext::*&)(std::uint16_t&, const CryStringT<char>&) const>(func))
		(id, name);
}

bool GameContext::ClassNameFromId(CryStringT<char>& name, std::uint16_t id) const
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x2B3870;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x1D6860;
#endif

	return (this->*reinterpret_cast<bool(GameContext::*&)(CryStringT<char>&, std::uint16_t) const>(func))
		(name, id);
}
