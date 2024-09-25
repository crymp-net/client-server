#include "VehicleDamageBehaviorDetachPart.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleDamageBehaviorDetachPart::VehicleDamageBehaviorDetachPart()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0xaf530;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x7ef70;
#endif

	(this->*reinterpret_cast<void(VehicleDamageBehaviorDetachPart::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorDetachPart::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleDamageBehaviorDetachPart::GetId()
{
	return {};
}

void VehicleDamageBehaviorDetachPart::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleDamageBehaviorDetachPart::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleDamageBehavior
////////////////////////////////////////////////////////////////////////////////

bool VehicleDamageBehaviorDetachPart::Init(IVehicle* pVehicle, const SmartScriptTable& table)
{
	return {};
}

void VehicleDamageBehaviorDetachPart::Reset()
{
}

void VehicleDamageBehaviorDetachPart::Release()
{
}

void VehicleDamageBehaviorDetachPart::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleDamageBehaviorDetachPart::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorDetachPart::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "DetachPart", VehicleDamageBehaviorDetachPart, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x428da0) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2aa35c) = id;
#endif
}
