#include <cstdint>

#include "ScriptBind_Network.h"

ScriptBind_Network::ScriptBind_Network(ISystem* pSystem, IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x307c3e30;
#else
	std::uintptr_t ctor = 0x306e1810;
#endif

	(this->*reinterpret_cast<void(ScriptBind_Network::*&)(ISystem*, IGameFramework*)>(ctor))(pSystem, pGameFramework);
}
