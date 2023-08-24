#include "VehicleDamageBehaviorSink.h"

VehicleDamageBehaviorSink::VehicleDamageBehaviorSink()
{
#ifdef BUILD_64BIT
	std::uintptr_t vtable = 0x30843da8;
#else
	std::uintptr_t vtable = 0x30761010;
#endif

	*reinterpret_cast<std::uintptr_t*>(this) = vtable;
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorSink::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleDamageBehaviorSink::GetId()
{
	return {};
}

void VehicleDamageBehaviorSink::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleDamageBehaviorSink::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleDamageBehavior
////////////////////////////////////////////////////////////////////////////////

bool VehicleDamageBehaviorSink::Init(IVehicle* pVehicle, const SmartScriptTable& table)
{
	return {};
}

void VehicleDamageBehaviorSink::Reset()
{
}

void VehicleDamageBehaviorSink::Release()
{
}

void VehicleDamageBehaviorSink::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleDamageBehaviorSink::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorSink::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "Sink", VehicleDamageBehaviorSink, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(0x30928dbc) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307aa378) = id;
#endif
}
