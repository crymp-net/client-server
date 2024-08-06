#include "VehicleDamageBehaviorSpawnDebris.h"

VehicleDamageBehaviorSpawnDebris::VehicleDamageBehaviorSpawnDebris()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x305b3880;
#else
	std::uintptr_t ctor = 0x30581ce0;
#endif

	(this->*reinterpret_cast<void(VehicleDamageBehaviorSpawnDebris::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorSpawnDebris::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleDamageBehaviorSpawnDebris::GetId()
{
	return {};
}

void VehicleDamageBehaviorSpawnDebris::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleDamageBehaviorSpawnDebris::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleDamageBehavior
////////////////////////////////////////////////////////////////////////////////

bool VehicleDamageBehaviorSpawnDebris::Init(IVehicle* pVehicle, const SmartScriptTable& table)
{
	return {};
}

void VehicleDamageBehaviorSpawnDebris::Reset()
{
}

void VehicleDamageBehaviorSpawnDebris::Release()
{
}

void VehicleDamageBehaviorSpawnDebris::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleDamageBehaviorSpawnDebris::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorSpawnDebris::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "SpawnDebris", VehicleDamageBehaviorSpawnDebris, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(0x30928dc0) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307aa37c) = id;
#endif
}
