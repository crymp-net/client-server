#include "VehicleSeatActionAnimation.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleSeatActionAnimation::VehicleSeatActionAnimation()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x9d7f0;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x72570;
#endif

	(this->*reinterpret_cast<void(VehicleSeatActionAnimation::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleSeatActionAnimation::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleSeatActionAnimation::GetId()
{
	return {};
}

void VehicleSeatActionAnimation::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleSeatActionAnimation::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleSeatAction
////////////////////////////////////////////////////////////////////////////////

bool VehicleSeatActionAnimation::Init(IVehicle* pVehicle, TVehicleSeatId seatId, const SmartScriptTable& table)
{
	return {};
}

void VehicleSeatActionAnimation::Reset()
{
}

void VehicleSeatActionAnimation::Release()
{
}

void VehicleSeatActionAnimation::StartUsing(EntityId passengerId)
{
}

void VehicleSeatActionAnimation::StopUsing()
{
}

void VehicleSeatActionAnimation::OnAction(const TVehicleActionId actionId, int activationMode, float value)
{
}

void VehicleSeatActionAnimation::GetMemoryStatistics(ICrySizer*)
{
}

void VehicleSeatActionAnimation::PostSerialize()
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleSeatActionAnimation::Register(IVehicleSystem* pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "Animation", VehicleSeatActionAnimation, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x426fc4) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2a82e8) = id;
#endif
}
