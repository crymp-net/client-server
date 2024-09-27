#include <cstdint>

#include "ScriptBind_Actor.h"

extern std::uintptr_t CRYACTION_BASE;

ScriptBind_Actor::ScriptBind_Actor(ISystem* pSystem, IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x11d70;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x11990;
#endif

	(this->*reinterpret_cast<void(ScriptBind_Actor::*&)(ISystem*, IGameFramework*)>(ctor))(pSystem, pGameFramework);
}
