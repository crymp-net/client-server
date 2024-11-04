#include "VehicleDamageBehaviorDestroy.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleDamageBehaviorDestroy::VehicleDamageBehaviorDestroy()
{
#ifdef BUILD_64BIT
	std::uintptr_t vtable = CRYACTION_BASE + 0x343ce8;
#else
	std::uintptr_t vtable = CRYACTION_BASE + 0x260fc0;
#endif

	*reinterpret_cast<std::uintptr_t*>(this) = vtable;
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorDestroy::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleDamageBehaviorDestroy::GetId()
{
	return {};
}

void VehicleDamageBehaviorDestroy::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleDamageBehaviorDestroy::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleDamageBehavior
////////////////////////////////////////////////////////////////////////////////

bool VehicleDamageBehaviorDestroy::Init(IVehicle* pVehicle, const SmartScriptTable& table)
{
	return {};
}

void VehicleDamageBehaviorDestroy::Reset()
{
}

void VehicleDamageBehaviorDestroy::Release()
{
}

void VehicleDamageBehaviorDestroy::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleDamageBehaviorDestroy::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorDestroy::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "Destroy", VehicleDamageBehaviorDestroy, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x428d9c) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2aa358) = id;
#endif
}
