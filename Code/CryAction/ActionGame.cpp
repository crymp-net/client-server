#include <cstdint>

#include "ActionGame.h"

extern std::uintptr_t CRYACTION_BASE;

bool ActionGame::Update()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x3036B0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x20CFF0;
#endif

	return (this->*reinterpret_cast<bool(ActionGame::*&)()>(func))();
}
