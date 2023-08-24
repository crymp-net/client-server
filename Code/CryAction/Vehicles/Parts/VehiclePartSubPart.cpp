#include "VehiclePartSubPart.h"

VehiclePartSubPart::VehiclePartSubPart()
{
	m_type = eVPT_SubPart;

#ifdef BUILD_64BIT
	std::uintptr_t vtable = 0x308439c8;
#else
	std::uintptr_t vtable = 0x30760e38;
#endif

	*reinterpret_cast<std::uintptr_t*>(this) = vtable;
}

void VehiclePartSubPart::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "SubPart", VehiclePartSubPart, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(0x30924c80) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307a6138) = id;
#endif
}
