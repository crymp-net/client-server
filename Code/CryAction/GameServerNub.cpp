#include <cstdint>

#include "GameServerNub.h"

extern std::uintptr_t CRYACTION_BASE;

void GameServerNub::Update()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x2BFBD0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x1DE990;
#endif

	(this->*reinterpret_cast<void(GameServerNub::*&)()>(func))();
}
