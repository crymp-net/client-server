#include <cstdint>

#include "GameSerialize.h"

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
	std::uintptr_t func = 0x306a3510;
#else
	std::uintptr_t func = 0x30624170;
#endif

	(this->*reinterpret_cast<void(GameSerialize::*&)(IGameFramework*)>(func))(pGameFramework);
}
