#include "VehicleDamageBehaviorMovementNotification.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleDamageBehaviorMovementNotification::VehicleDamageBehaviorMovementNotification()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0xb2720;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x810f0;
#endif

	(this->*reinterpret_cast<void(VehicleDamageBehaviorMovementNotification::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorMovementNotification::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleDamageBehaviorMovementNotification::GetId()
{
	return {};
}

void VehicleDamageBehaviorMovementNotification::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleDamageBehaviorMovementNotification::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleDamageBehavior
////////////////////////////////////////////////////////////////////////////////

bool VehicleDamageBehaviorMovementNotification::Init(IVehicle* pVehicle, const SmartScriptTable& table)
{
	return {};
}

void VehicleDamageBehaviorMovementNotification::Reset()
{
}

void VehicleDamageBehaviorMovementNotification::Release()
{
}

void VehicleDamageBehaviorMovementNotification::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleDamageBehaviorMovementNotification::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorMovementNotification::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "MovementNotification", VehicleDamageBehaviorMovementNotification, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x428db8) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2aa374) = id;
#endif
}
