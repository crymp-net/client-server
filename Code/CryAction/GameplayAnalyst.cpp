#include <cstdint>

#include "GameplayAnalyst.h"

GameplayAnalyst::GameplayAnalyst()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x307f9e80;
#else
	std::uintptr_t ctor = 0x307058c0;
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
