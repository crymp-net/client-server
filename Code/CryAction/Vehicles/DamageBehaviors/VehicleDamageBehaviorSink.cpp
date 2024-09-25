#include "VehicleDamageBehaviorSink.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleDamageBehaviorSink::VehicleDamageBehaviorSink()
{
#ifdef BUILD_64BIT
	std::uintptr_t vtable = CRYACTION_BASE + 0x343da8;
#else
	std::uintptr_t vtable = CRYACTION_BASE + 0x261010;
#endif

	*reinterpret_cast<std::uintptr_t*>(this) = vtable;
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorSink::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleDamageBehaviorSink::GetId()
{
	return {};
}

void VehicleDamageBehaviorSink::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleDamageBehaviorSink::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleDamageBehavior
////////////////////////////////////////////////////////////////////////////////

bool VehicleDamageBehaviorSink::Init(IVehicle* pVehicle, const SmartScriptTable& table)
{
	return {};
}

void VehicleDamageBehaviorSink::Reset()
{
}

void VehicleDamageBehaviorSink::Release()
{
}

void VehicleDamageBehaviorSink::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleDamageBehaviorSink::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorSink::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "Sink", VehicleDamageBehaviorSink, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x428dbc) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2aa378) = id;
#endif
}
