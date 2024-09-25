#include "VehicleSeatActionMovement.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleSeatActionMovement::VehicleSeatActionMovement()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x9e950;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x731a0;
#endif

	(this->*reinterpret_cast<void(VehicleSeatActionMovement::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleSeatActionMovement::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleSeatActionMovement::GetId()
{
	return {};
}

void VehicleSeatActionMovement::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleSeatActionMovement::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleSeatAction
////////////////////////////////////////////////////////////////////////////////

bool VehicleSeatActionMovement::Init(IVehicle* pVehicle, TVehicleSeatId seatId, const SmartScriptTable& table)
{
	return {};
}

void VehicleSeatActionMovement::Reset()
{
}

void VehicleSeatActionMovement::Release()
{
}

void VehicleSeatActionMovement::StartUsing(EntityId passengerId)
{
}

void VehicleSeatActionMovement::StopUsing()
{
}

void VehicleSeatActionMovement::OnAction(const TVehicleActionId actionId, int activationMode, float value)
{
}

void VehicleSeatActionMovement::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleSeatActionMovement::PostSerialize()
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleSeatActionMovement::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "Movement", VehicleSeatActionMovement, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x426fcc) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2a82f0) = id;
#endif
}
