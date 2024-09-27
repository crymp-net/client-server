#include <cstdint>

#include "ScriptBind_Network.h"

extern std::uintptr_t CRYACTION_BASE;

ScriptBind_Network::ScriptBind_Network(ISystem* pSystem, IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x2c3e30;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x1e1810;
#endif

	(this->*reinterpret_cast<void(ScriptBind_Network::*&)(ISystem*, IGameFramework*)>(ctor))(pSystem, pGameFramework);
}
