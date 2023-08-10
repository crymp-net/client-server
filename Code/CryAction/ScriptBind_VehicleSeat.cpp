#include <cstdint>

#include "ScriptBind_VehicleSeat.h"

ScriptBind_VehicleSeat::ScriptBind_VehicleSeat(ISystem* pSystem, IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x305417b0;
#else
	std::uintptr_t ctor = 0x3052fef0;
#endif

	(this->*reinterpret_cast<void(ScriptBind_VehicleSeat::*&)(ISystem*, IGameFramework*)>(ctor))(pSystem, pGameFramework);
}
