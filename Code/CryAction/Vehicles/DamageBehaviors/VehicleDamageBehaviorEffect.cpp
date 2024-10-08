#include "VehicleDamageBehaviorEffect.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleDamageBehaviorEffect::VehicleDamageBehaviorEffect()
{
#ifdef BUILD_64BIT
	std::uintptr_t vtable = CRYACTION_BASE + 0x3441c8;
#else
	std::uintptr_t vtable = CRYACTION_BASE + 0x2611f0;
#endif

	*reinterpret_cast<std::uintptr_t*>(this) = vtable;
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorEffect::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleDamageBehaviorEffect::GetId()
{
	return {};
}

void VehicleDamageBehaviorEffect::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleDamageBehaviorEffect::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleDamageBehavior
////////////////////////////////////////////////////////////////////////////////

bool VehicleDamageBehaviorEffect::Init(IVehicle* pVehicle, const SmartScriptTable& table)
{
	return {};
}

void VehicleDamageBehaviorEffect::Reset()
{
}

void VehicleDamageBehaviorEffect::Release()
{
}

void VehicleDamageBehaviorEffect::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleDamageBehaviorEffect::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorEffect::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "Effect", VehicleDamageBehaviorEffect, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x428da4) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2aa360) = id;
#endif
}
