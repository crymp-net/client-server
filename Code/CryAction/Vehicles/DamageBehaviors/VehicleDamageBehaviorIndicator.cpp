#include "VehicleDamageBehaviorIndicator.h"

VehicleDamageBehaviorIndicator::VehicleDamageBehaviorIndicator()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x305b1fb0;
#else
	std::uintptr_t ctor = 0x30580cc0;
#endif

	(this->*reinterpret_cast<void(VehicleDamageBehaviorIndicator::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorIndicator::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleDamageBehaviorIndicator::GetId()
{
	return {};
}

void VehicleDamageBehaviorIndicator::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleDamageBehaviorIndicator::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleDamageBehavior
////////////////////////////////////////////////////////////////////////////////

bool VehicleDamageBehaviorIndicator::Init(IVehicle* pVehicle, const SmartScriptTable& table)
{
	return {};
}

void VehicleDamageBehaviorIndicator::Reset()
{
}

void VehicleDamageBehaviorIndicator::Release()
{
}

void VehicleDamageBehaviorIndicator::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleDamageBehaviorIndicator::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleDamageBehaviorIndicator::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "Indicator", VehicleDamageBehaviorIndicator, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(0x30928db4) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307aa370) = id;
#endif
}
