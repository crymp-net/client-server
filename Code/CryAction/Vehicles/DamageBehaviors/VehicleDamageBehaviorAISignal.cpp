#include "VehicleDamageBehaviorAISignal.h"

VehicleDamageBehaviorAISignal::VehicleDamageBehaviorAISignal()
{
#ifdef BUILD_64BIT
	std::uintptr_t vtable = 0x30844168;
#else
	std::uintptr_t vtable = 0x307611c8;
#endif

	*reinterpret_cast<std::uintptr_t*>(this) = vtable;
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorAISignal::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleDamageBehaviorAISignal::GetId()
{
	return {};
}

void VehicleDamageBehaviorAISignal::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleDamageBehaviorAISignal::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleDamageBehavior
////////////////////////////////////////////////////////////////////////////////

bool VehicleDamageBehaviorAISignal::Init(IVehicle* pVehicle, const SmartScriptTable& table)
{
	return {};
}

void VehicleDamageBehaviorAISignal::Reset()
{
}

void VehicleDamageBehaviorAISignal::Release()
{
}

void VehicleDamageBehaviorAISignal::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleDamageBehaviorAISignal::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorAISignal::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "AISignal", VehicleDamageBehaviorAISignal, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(0x30928d98) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307aa354) = id;
#endif
}
