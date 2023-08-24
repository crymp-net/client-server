#include "VehicleSeatActionSteeringWheel.h"

VehicleSeatActionSteeringWheel::VehicleSeatActionSteeringWheel()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x305a25d0;
#else
	std::uintptr_t ctor = 0x30576ca0;
#endif

	(this->*reinterpret_cast<void(VehicleSeatActionSteeringWheel::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleSeatActionSteeringWheel::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleSeatActionSteeringWheel::GetId()
{
	return {};
}

void VehicleSeatActionSteeringWheel::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleSeatActionSteeringWheel::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleSeatAction
////////////////////////////////////////////////////////////////////////////////

bool VehicleSeatActionSteeringWheel::Init(IVehicle* pVehicle, TVehicleSeatId seatId, const SmartScriptTable& table)
{
	return {};
}

void VehicleSeatActionSteeringWheel::Reset()
{
}

void VehicleSeatActionSteeringWheel::Release()
{
}

void VehicleSeatActionSteeringWheel::StartUsing(EntityId passengerId)
{
}

void VehicleSeatActionSteeringWheel::StopUsing()
{
}

void VehicleSeatActionSteeringWheel::OnAction(const TVehicleActionId actionId, int activationMode, float value)
{
}

void VehicleSeatActionSteeringWheel::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleSeatActionSteeringWheel::PostSerialize()
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleSeatActionSteeringWheel::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "SteeringWheel", VehicleSeatActionSteeringWheel, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(0x30927380) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307a8688) = id;
#endif
}
