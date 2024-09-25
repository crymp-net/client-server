#include "VehicleDamageBehaviorImpulse.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleDamageBehaviorImpulse::VehicleDamageBehaviorImpulse()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0xb1790;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x80610;
#endif

	(this->*reinterpret_cast<void(VehicleDamageBehaviorImpulse::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorImpulse::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleDamageBehaviorImpulse::GetId()
{
	return {};
}

void VehicleDamageBehaviorImpulse::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleDamageBehaviorImpulse::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleDamageBehavior
////////////////////////////////////////////////////////////////////////////////

bool VehicleDamageBehaviorImpulse::Init(IVehicle* pVehicle, const SmartScriptTable& table)
{
	return {};
}

void VehicleDamageBehaviorImpulse::Reset()
{
}

void VehicleDamageBehaviorImpulse::Release()
{
}

void VehicleDamageBehaviorImpulse::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleDamageBehaviorImpulse::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorImpulse::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "Impulse", VehicleDamageBehaviorImpulse, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x428db0) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2aa36c) = id;
#endif
}
