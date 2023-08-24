#include "VehicleDamageBehaviorDetachPart.h"

VehicleDamageBehaviorDetachPart::VehicleDamageBehaviorDetachPart()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x305af530;
#else
	std::uintptr_t ctor = 0x3057ef70;
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
	*reinterpret_cast<TVehicleObjectId*>(0x30928da0) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307aa35c) = id;
#endif
}
