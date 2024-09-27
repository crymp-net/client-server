#include "VehicleDamageBehaviorGroup.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleDamageBehaviorGroup::VehicleDamageBehaviorGroup()
{
#ifdef BUILD_64BIT
	std::uintptr_t vtable = CRYACTION_BASE + 0x343d48;
#else
	std::uintptr_t vtable = CRYACTION_BASE + 0x260fe8;
#endif

	*reinterpret_cast<std::uintptr_t*>(this) = vtable;
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorGroup::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleDamageBehaviorGroup::GetId()
{
	return {};
}

void VehicleDamageBehaviorGroup::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleDamageBehaviorGroup::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleDamageBehavior
////////////////////////////////////////////////////////////////////////////////

bool VehicleDamageBehaviorGroup::Init(IVehicle* pVehicle, const SmartScriptTable& table)
{
	return {};
}

void VehicleDamageBehaviorGroup::Reset()
{
}

void VehicleDamageBehaviorGroup::Release()
{
}

void VehicleDamageBehaviorGroup::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleDamageBehaviorGroup::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorGroup::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "Group", VehicleDamageBehaviorGroup, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x428da8) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2aa364) = id;
#endif
}
