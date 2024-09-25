#include "VehiclePartSubPart.h"

extern std::uintptr_t CRYACTION_BASE;

VehiclePartSubPart::VehiclePartSubPart()
{
	m_type = eVPT_SubPart;

#ifdef BUILD_64BIT
	std::uintptr_t vtable = CRYACTION_BASE + 0x3439c8;
#else
	std::uintptr_t vtable = CRYACTION_BASE + 0x260e38;
#endif

	*reinterpret_cast<std::uintptr_t*>(this) = vtable;
}

void VehiclePartSubPart::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "SubPart", VehiclePartSubPart, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x424c80) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2a6138) = id;
#endif
}
