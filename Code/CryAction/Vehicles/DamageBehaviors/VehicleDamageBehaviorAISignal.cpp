#include "VehicleDamageBehaviorAISignal.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleDamageBehaviorAISignal::VehicleDamageBehaviorAISignal()
{
#ifdef BUILD_64BIT
	std::uintptr_t vtable = CRYACTION_BASE + 0x344168;
#else
	std::uintptr_t vtable = CRYACTION_BASE + 0x2611c8;
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
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x428d98) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2aa354) = id;
#endif
}
