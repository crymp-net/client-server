#include "VehicleSeatActionSound.h"

VehicleSeatActionSound::VehicleSeatActionSound()
{
#ifdef BUILD_64BIT
	std::uintptr_t vtable = 0x308440f8;
#else
	std::uintptr_t vtable = 0x30761194;
#endif

	*reinterpret_cast<std::uintptr_t*>(this) = vtable;
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleSeatActionSound::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleSeatActionSound::GetId()
{
	return {};
}

void VehicleSeatActionSound::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleSeatActionSound::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleSeatAction
////////////////////////////////////////////////////////////////////////////////

bool VehicleSeatActionSound::Init(IVehicle* pVehicle, TVehicleSeatId seatId, const SmartScriptTable& table)
{
	return {};
}

void VehicleSeatActionSound::Reset()
{
}

void VehicleSeatActionSound::Release()
{
}

void VehicleSeatActionSound::StartUsing(EntityId passengerId)
{
}

void VehicleSeatActionSound::StopUsing()
{
}

void VehicleSeatActionSound::OnAction(const TVehicleActionId actionId, int activationMode, float value)
{
}

void VehicleSeatActionSound::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleSeatActionSound::PostSerialize()
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleSeatActionSound::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "Sound", VehicleSeatActionSound, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(0x3092737c) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307a8684) = id;
#endif
}
