#include "VehicleViewThirdPerson.h"

VehicleViewThirdPerson::VehicleViewThirdPerson()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30549400;
#else
	std::uintptr_t ctor = 0x30536d80;
#endif

	(this->*reinterpret_cast<void(VehicleViewThirdPerson::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleViewThirdPerson::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleViewThirdPerson::GetId()
{
	return {};
}

void VehicleViewThirdPerson::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleViewThirdPerson::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleView
////////////////////////////////////////////////////////////////////////////////

bool VehicleViewThirdPerson::Init(CVehicleSeat* pSeat, const SmartScriptTable& table)
{
	return {};
}

void VehicleViewThirdPerson::Reset()
{
}

void VehicleViewThirdPerson::Release()
{
}

void VehicleViewThirdPerson::ResetPosition()
{
}

const char* VehicleViewThirdPerson::GetName()
{
	return {};
}

bool VehicleViewThirdPerson::IsThirdPerson()
{
	return {};
}

bool VehicleViewThirdPerson::IsPassengerHidden()
{
	return {};
}

void VehicleViewThirdPerson::OnStartUsing(EntityId playerId)
{
}

void VehicleViewThirdPerson::OnStopUsing()
{
}

void VehicleViewThirdPerson::OnAction(const TVehicleActionId actionId, int activationMode, float value)
{
}

void VehicleViewThirdPerson::UpdateView(SViewParams& viewParams, EntityId playerId)
{
}

void VehicleViewThirdPerson::SetDebugView(bool debug)
{
}

bool VehicleViewThirdPerson::IsDebugView()
{
	return {};
}

bool VehicleViewThirdPerson::ShootToCrosshair()
{
	return {};
}

bool VehicleViewThirdPerson::IsAvailableRemotely() const
{
	return {};
}

void VehicleViewThirdPerson::GetMemoryStatistics(ICrySizer*)
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleViewThirdPerson::Register(IVehicleSystem *pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "ThirdPerson", VehicleViewThirdPerson, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(0x309235ec) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(0x307a4b6c) = id;
#endif
}
