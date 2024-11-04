#include "VehicleSeatActionPassengerIK.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleSeatActionPassengerIK::VehicleSeatActionPassengerIK()
{
#ifdef BUILD_64BIT
	std::uintptr_t vtable = CRYACTION_BASE + 0x3442e8;
#else
	std::uintptr_t vtable = CRYACTION_BASE + 0x261268;
#endif

	*reinterpret_cast<std::uintptr_t*>(this) = vtable;
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleSeatActionPassengerIK::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleSeatActionPassengerIK::GetId()
{
	return {};
}

void VehicleSeatActionPassengerIK::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleSeatActionPassengerIK::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleSeatAction
////////////////////////////////////////////////////////////////////////////////

bool VehicleSeatActionPassengerIK::Init(IVehicle* pVehicle, TVehicleSeatId seatId, const SmartScriptTable& table)
{
	return {};
}

void VehicleSeatActionPassengerIK::Reset()
{
}

void VehicleSeatActionPassengerIK::Release()
{
}

void VehicleSeatActionPassengerIK::StartUsing(EntityId passengerId)
{
}

void VehicleSeatActionPassengerIK::StopUsing()
{
}

void VehicleSeatActionPassengerIK::OnAction(const TVehicleActionId actionId, int activationMode, float value)
{
}

void VehicleSeatActionPassengerIK::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleSeatActionPassengerIK::PostSerialize()
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleSeatActionPassengerIK::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "PassengerIK", VehicleSeatActionPassengerIK, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x426fd0) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2a82f4) = id;
#endif
}
