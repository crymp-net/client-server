#include <cstdint>

#include "GameplayAnalyst.h"

extern std::uintptr_t CRYACTION_BASE;

GameplayAnalyst::GameplayAnalyst()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x2f9e80;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x2058c0;
#endif

	(this->*reinterpret_cast<void(GameplayAnalyst::*&)()>(ctor))();
}

GameplayAnalyst::~GameplayAnalyst()
{
}

////////////////////////////////////////////////////////////////////////////////
// IGameplayListener
////////////////////////////////////////////////////////////////////////////////

void GameplayAnalyst::OnGameplayEvent(IEntity* pEntity, const GameplayEvent& event)
{
}

////////////////////////////////////////////////////////////////////////////////
