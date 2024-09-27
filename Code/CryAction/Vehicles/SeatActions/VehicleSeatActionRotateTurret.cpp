#include "VehicleSeatActionRotateTurret.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleSeatActionRotateTurret::VehicleSeatActionRotateTurret()
{
#ifdef BUILD_64BIT
	std::uintptr_t vtable = CRYACTION_BASE + 0x344088;
#else
	std::uintptr_t vtable = CRYACTION_BASE + 0x261164;
#endif

	*reinterpret_cast<std::uintptr_t*>(this) = vtable;
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleSeatActionRotateTurret::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleSeatActionRotateTurret::GetId()
{
	return {};
}

void VehicleSeatActionRotateTurret::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleSeatActionRotateTurret::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleSeatAction
////////////////////////////////////////////////////////////////////////////////

bool VehicleSeatActionRotateTurret::Init(IVehicle* pVehicle, TVehicleSeatId seatId, const SmartScriptTable& table)
{
	return {};
}

void VehicleSeatActionRotateTurret::Reset()
{
}

void VehicleSeatActionRotateTurret::Release()
{
}

void VehicleSeatActionRotateTurret::StartUsing(EntityId passengerId)
{
}

void VehicleSeatActionRotateTurret::StopUsing()
{
}

void VehicleSeatActionRotateTurret::OnAction(const TVehicleActionId actionId, int activationMode, float value)
{
}

void VehicleSeatActionRotateTurret::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleSeatActionRotateTurret::PostSerialize()
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleSeatActionRotateTurret::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "RotateTurret", VehicleSeatActionRotateTurret, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x426fd4) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2a82f8) = id;
#endif
}
