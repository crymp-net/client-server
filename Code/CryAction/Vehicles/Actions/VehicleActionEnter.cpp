#include "VehicleActionEnter.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleActionEnter::VehicleActionEnter()
{
#ifdef BUILD_64BIT
	std::uintptr_t vtable = CRYACTION_BASE + 0x344228;
#else
	std::uintptr_t vtable = CRYACTION_BASE + 0x261218;
#endif

	*reinterpret_cast<std::uintptr_t*>(this) = vtable;
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleActionEnter::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleActionEnter::GetId()
{
	return {};
}

void VehicleActionEnter::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleActionEnter::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleAction
////////////////////////////////////////////////////////////////////////////////

bool VehicleActionEnter::Init(IVehicle* pVehicle, const SmartScriptTable& table)
{
	return {};
}

void VehicleActionEnter::Reset()
{
}

void VehicleActionEnter::Release()
{
}

int VehicleActionEnter::OnEvent(int eventType, SVehicleEventParams& eventParams)
{
	return {};
}

////////////////////////////////////////////////////////////////////////////////

void VehicleActionEnter::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "Enter", VehicleActionEnter, false);

	// CryMP: the original implementation does not assign any object ID here
	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x428650) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2a9c40) = id;
#endif
}
