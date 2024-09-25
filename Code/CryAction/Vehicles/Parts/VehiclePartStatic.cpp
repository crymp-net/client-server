#include "VehiclePartStatic.h"

extern std::uintptr_t CRYACTION_BASE;

VehiclePartStatic::VehiclePartStatic()
{
	m_type = eVPT_Static;

#ifdef BUILD_64BIT
	std::uintptr_t vtable = CRYACTION_BASE + 0x343e08;
#else
	std::uintptr_t vtable = CRYACTION_BASE + 0x261038;
#endif

	*reinterpret_cast<std::uintptr_t*>(this) = vtable;
}

void VehiclePartStatic::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "Static", VehiclePartStatic, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x424c7c) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2a6134) = id;
#endif
}
