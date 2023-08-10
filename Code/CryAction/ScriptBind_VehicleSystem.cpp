#include <cstdint>

#include "ScriptBind_VehicleSystem.h"

ScriptBind_VehicleSystem::ScriptBind_VehicleSystem(ISystem* pSystem, IVehicleSystem* pVehicleSystem)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30533590;
#else
	std::uintptr_t ctor = 0x30526650;
#endif

	(this->*reinterpret_cast<void(ScriptBind_VehicleSystem::*&)(ISystem*, IVehicleSystem*)>(ctor))(pSystem, pVehicleSystem);
}
