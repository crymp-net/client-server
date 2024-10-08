#include "VehicleSeatActionLights.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleSeatActionLights::VehicleSeatActionLights()
{
#ifdef BUILD_64BIT
	std::uintptr_t vtable = CRYACTION_BASE + 0x344278;
#else
	std::uintptr_t vtable = CRYACTION_BASE + 0x261238;
#endif

	*reinterpret_cast<std::uintptr_t*>(this) = vtable;
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleSeatActionLights::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleSeatActionLights::GetId()
{
	return {};
}

void VehicleSeatActionLights::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleSeatActionLights::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleSeatAction
////////////////////////////////////////////////////////////////////////////////

bool VehicleSeatActionLights::Init(IVehicle* pVehicle, TVehicleSeatId seatId, const SmartScriptTable& table)
{
	return {};
}

void VehicleSeatActionLights::Reset()
{
}

void VehicleSeatActionLights::Release()
{
}

void VehicleSeatActionLights::StartUsing(EntityId passengerId)
{
}

void VehicleSeatActionLights::StopUsing()
{
}

void VehicleSeatActionLights::OnAction(const TVehicleActionId actionId, int activationMode, float value)
{
}

void VehicleSeatActionLights::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleSeatActionLights::PostSerialize()
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleSeatActionLights::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "Lights", VehicleSeatActionLights, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x426fc8) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2a82ec) = id;
#endif
}
