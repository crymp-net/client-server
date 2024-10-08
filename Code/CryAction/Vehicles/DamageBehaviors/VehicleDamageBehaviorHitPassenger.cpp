#include "VehicleDamageBehaviorHitPassenger.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleDamageBehaviorHitPassenger::VehicleDamageBehaviorHitPassenger()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0xb0dc0;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x7fe50;
#endif

	(this->*reinterpret_cast<void(VehicleDamageBehaviorHitPassenger::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorHitPassenger::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleDamageBehaviorHitPassenger::GetId()
{
	return {};
}

void VehicleDamageBehaviorHitPassenger::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleDamageBehaviorHitPassenger::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleDamageBehavior
////////////////////////////////////////////////////////////////////////////////

bool VehicleDamageBehaviorHitPassenger::Init(IVehicle* pVehicle, const SmartScriptTable& table)
{
	return {};
}

void VehicleDamageBehaviorHitPassenger::Reset()
{
}

void VehicleDamageBehaviorHitPassenger::Release()
{
}

void VehicleDamageBehaviorHitPassenger::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleDamageBehaviorHitPassenger::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorHitPassenger::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "HitPassenger", VehicleDamageBehaviorHitPassenger, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x428dac) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2aa368) = id;
#endif
}
