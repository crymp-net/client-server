#include "VehicleViewFirstPerson.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleViewFirstPerson::VehicleViewFirstPerson()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x478a0;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x35630;
#endif

	(this->*reinterpret_cast<void(VehicleViewFirstPerson::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleEventListener
////////////////////////////////////////////////////////////////////////////////

void VehicleViewFirstPerson::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleObject
////////////////////////////////////////////////////////////////////////////////

TVehicleObjectId VehicleViewFirstPerson::GetId()
{
	return {};
}

void VehicleViewFirstPerson::Serialize(TSerialize ser, unsigned aspects)
{
}

void VehicleViewFirstPerson::Update(const float deltaTime)
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleView
////////////////////////////////////////////////////////////////////////////////

bool VehicleViewFirstPerson::Init(CVehicleSeat* pSeat, const SmartScriptTable& table)
{
	return {};
}

void VehicleViewFirstPerson::Reset()
{
}

void VehicleViewFirstPerson::Release()
{
}

void VehicleViewFirstPerson::ResetPosition()
{
}

const char* VehicleViewFirstPerson::GetName()
{
	return {};
}

bool VehicleViewFirstPerson::IsThirdPerson()
{
	return {};
}

bool VehicleViewFirstPerson::IsPassengerHidden()
{
	return {};
}

void VehicleViewFirstPerson::OnStartUsing(EntityId playerId)
{
}

void VehicleViewFirstPerson::OnStopUsing()
{
}

void VehicleViewFirstPerson::OnAction(const TVehicleActionId actionId, int activationMode, float value)
{
}

void VehicleViewFirstPerson::UpdateView(SViewParams& viewParams, EntityId playerId)
{
}

void VehicleViewFirstPerson::SetDebugView(bool debug)
{
}

bool VehicleViewFirstPerson::IsDebugView()
{
	return {};
}

bool VehicleViewFirstPerson::ShootToCrosshair()
{
	return {};
}

bool VehicleViewFirstPerson::IsAvailableRemotely() const
{
	return {};
}

void VehicleViewFirstPerson::GetMemoryStatistics(ICrySizer*)
{
}

////////////////////////////////////////////////////////////////////////////////

void VehicleViewFirstPerson::Register(IVehicleSystem *pVehicleSystem)
{
	REGISTER_FACTORY(pVehicleSystem, "FirstPerson", VehicleViewFirstPerson, false);

	const TVehicleObjectId id = pVehicleSystem->AssignVehicleObjectId();

#ifdef BUILD_64BIT
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x4235e0) = id;
#else
	*reinterpret_cast<TVehicleObjectId*>(CRYACTION_BASE + 0x2a4b60) = id;
#endif
}
