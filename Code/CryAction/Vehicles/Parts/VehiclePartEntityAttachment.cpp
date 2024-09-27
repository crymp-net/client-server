#include "VehiclePartEntityAttachment.h"

extern std::uintptr_t CRYACTION_BASE;

VehiclePartEntityAttachment::VehiclePartEntityAttachment()
{
	// CryMP: the original implementation uses the inherited eVPT_Base here, which is wrong
	m_type = eVPT_Attachment;

#ifdef BUILD_64BIT
	std::uintptr_t vtable = CRYACTION_BASE + 0x343b58;
#else
	std::uintptr_t vtable = CRYACTION_BASE + 0x260f00;
#endif

	*reinterpret_cast<std::uintptr_t*>(this) = vtable;
}

void VehiclePartEntityAttachment::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "EntityAttachment", VehiclePartEntityAttachment, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x4240dc) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2a560c) = id;
#endif
}
