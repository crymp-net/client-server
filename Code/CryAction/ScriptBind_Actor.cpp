#include <cstdint>

#include "ScriptBind_Actor.h"

ScriptBind_Actor::ScriptBind_Actor(ISystem* pSystem, IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30511d70;
#else
	std::uintptr_t ctor = 0x30511990;
#endif

	(this->*reinterpret_cast<void(ScriptBind_Actor::*&)(ISystem*, IGameFramework*)>(ctor))(pSystem, pGameFramework);
}
