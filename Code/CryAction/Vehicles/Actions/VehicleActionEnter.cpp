#include "VehicleActionEnter.h"

VehicleActionEnter::VehicleActionEnter()
{
#ifdef BUILD_64BIT
	std::uintptr_t vtable = 0x30844228;
#else
	std::uintptr_t vtable = 0x30761218;
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
	*reinterpret_cast<TVehicleObjectId*>(0x30928650) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307a9c40) = id;
#endif
}
