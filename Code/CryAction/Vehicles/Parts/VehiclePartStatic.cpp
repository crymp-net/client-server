#include "VehiclePartStatic.h"

VehiclePartStatic::VehiclePartStatic()
{
	m_type = eVPT_Static;

#ifdef BUILD_64BIT
	std::uintptr_t vtable = 0x30843e08;
#else
	std::uintptr_t vtable = 0x30761038;
#endif

	*reinterpret_cast<std::uintptr_t*>(this) = vtable;
}

void VehiclePartStatic::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "Static", VehiclePartStatic, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(0x30924c7c) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307a6134) = id;
#endif
}
