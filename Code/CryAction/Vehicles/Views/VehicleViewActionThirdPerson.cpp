#include "VehicleViewActionThirdPerson.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleViewActionThirdPerson::VehicleViewActionThirdPerson()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x43c00;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x31e80;
#endif

	(this->*reinterpret_cast<void(VehicleViewActionThirdPerson::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleViewActionThirdPerson::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleViewActionThirdPerson::GetId()
{
	return {};
}

void VehicleViewActionThirdPerson::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleViewActionThirdPerson::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleView
////////////////////////////////////////////////////////////////////////////////

bool VehicleViewActionThirdPerson::Init(CVehicleSeat* pSeat, const SmartScriptTable& table)
{
	return {};
}

void VehicleViewActionThirdPerson::Reset()
{
}

void VehicleViewActionThirdPerson::Release()
{
}

void VehicleViewActionThirdPerson::ResetPosition()
{
}

const char* VehicleViewActionThirdPerson::GetName()
{
	return {};
}

bool VehicleViewActionThirdPerson::IsThirdPerson()
{
	return {};
}

bool VehicleViewActionThirdPerson::IsPassengerHidden()
{
	return {};
}

void VehicleViewActionThirdPerson::OnStartUsing(EntityId playerId)
{
}

void VehicleViewActionThirdPerson::OnStopUsing()
{
}

void VehicleViewActionThirdPerson::OnAction(const TVehicleActionId actionId, int activationMode, float value)
{
}

void VehicleViewActionThirdPerson::UpdateView(SViewParams& viewParams, EntityId playerId)
{
}

void VehicleViewActionThirdPerson::SetDebugView(bool debug)
{
}

bool VehicleViewActionThirdPerson::IsDebugView()
{
	return {};
}

bool VehicleViewActionThirdPerson::ShootToCrosshair()
{
	return {};
}

bool VehicleViewActionThirdPerson::IsAvailableRemotely() const
{
	return {};
}

void VehicleViewActionThirdPerson::GetMemoryStatistics(ICrySizer*)
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleViewActionThirdPerson::Register(IVehicleSystem *pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "ActionThirdPerson", VehicleViewActionThirdPerson, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x4235d0) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2a4b58) = id;
#endif
}
