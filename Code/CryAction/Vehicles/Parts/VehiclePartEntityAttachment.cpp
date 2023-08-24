#include "VehiclePartEntityAttachment.h"

VehiclePartEntityAttachment::VehiclePartEntityAttachment()
{
	// CryMP: the original implementation uses the inherited eVPT_Base here, which is wrong
	m_type = eVPT_Attachment;

#ifdef BUILD_64BIT
	std::uintptr_t vtable = 0x30843b58;
#else
	std::uintptr_t vtable = 0x30760f00;
#endif

	*reinterpret_cast<std::uintptr_t*>(this) = vtable;
}

void VehiclePartEntityAttachment::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "EntityAttachment", VehiclePartEntityAttachment, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(0x309240dc) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307a560c) = id;
#endif
}
