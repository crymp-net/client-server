#include "VehicleSeat.h"

VehicleSeat::VehicleSeat()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30597010;
#else
	std::uintptr_t ctor = 0x3056d910;
#endif

	(this->*reinterpret_cast<void(VehicleSeat::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleSeat
////////////////////////////////////////////////////////////////////////////////

bool VehicleSeat::Init(IVehicle* pVehicle, TVehicleSeatId seatId, const SmartScriptTable& paramsTable)
{
	return {};
}

void VehicleSeat::PostInit(IVehicle* pVehicle, const SmartScriptTable& paramsTable)
{
}

void VehicleSeat::Reset()
{
}

void VehicleSeat::Release()
{
}

const char* VehicleSeat::GetSeatName() const
{
	return {};
}

TVehicleSeatId VehicleSeat::GetSeatId()
{
	return {};
}

EntityId VehicleSeat::GetPassenger(bool remoteUser)
{
	return {};
}

int VehicleSeat::GetCurrentTransition() const
{
	return {};
}

TVehicleViewId VehicleSeat::GetCurrentView()
{
	return {};
}

IVehicleView* VehicleSeat::GetView(TVehicleViewId viewId)
{
	return {};
}

bool VehicleSeat::SetView(TVehicleViewId viewId)
{
	return {};
}

TVehicleViewId VehicleSeat::GetNextView(TVehicleViewId viewId)
{
	return {};
}

bool VehicleSeat::IsDriver()
{
	return {};
}

bool VehicleSeat::IsGunner()
{
	return {};
}

bool VehicleSeat::IsLocked()
{
	return {};
}

bool VehicleSeat::Enter(EntityId actorId, bool isTransitionEnabled)
{
	return {};
}

bool VehicleSeat::Exit(bool isTransitionEnabled, bool force)
{
	return {};
}

void VehicleSeat::OnPassengerDeath()
{
}

void VehicleSeat::ForceAnimGraphInputs()
{
}

void VehicleSeat::UnlinkPassenger(bool ragdoll)
{
}

bool VehicleSeat::IsPassengerHidden()
{
	return {};
}

bool VehicleSeat::IsPassengerExposed() const
{
	return {};
}

IVehicle* VehicleSeat::GetVehicle()
{
	return {};
}

IVehicleHelper* VehicleSeat::GetSitHelper()
{
	return {};
}

const SSeatSoundParams& VehicleSeat::GetSoundParams()
{
	static const SSeatSoundParams params;
	return params;
}

bool VehicleSeat::IsAnimGraphStateExisting(EntityId actorId)
{
	return {};
}

void VehicleSeat::OnCameraShake(float& angleAmount, float& shiftAmount, const Vec3& pos, const char* source) const
{
}

void VehicleSeat::ForceFinishExiting()
{
}

////////////////////////////////////////////////////////////////////////////////
