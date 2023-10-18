#include "VehicleDamageBehaviorImpulse.h"

VehicleDamageBehaviorImpulse::VehicleDamageBehaviorImpulse()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x305b1790;
#else
	std::uintptr_t ctor = 0x30580610;
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
	*reinterpret_cast<TVehicleObjectId*>(0x30928db0) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307aa36c) = id;
#endif
}
