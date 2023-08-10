#include <cstdint>

#include "ScriptBind_Vehicle.h"

ScriptBind_Vehicle::ScriptBind_Vehicle(ISystem* pSystem, IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30540730;
#else
	std::uintptr_t ctor = 0x3052ebf0;
#endif

	(this->*reinterpret_cast<void(ScriptBind_Vehicle::*&)(ISystem*, IGameFramework*)>(ctor))(pSystem, pGameFramework);
}
