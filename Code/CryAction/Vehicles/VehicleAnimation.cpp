#include "VehicleAnimation.h"

VehicleAnimation::VehicleAnimation()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x3058bf10;
#else
	std::uintptr_t ctor = 0x30563f60;
#endif

	(this->*reinterpret_cast<void(VehicleAnimation::*&)()>(ctor))();
}

VehicleAnimation::~VehicleAnimation()
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicleAnimation
////////////////////////////////////////////////////////////////////////////////

bool VehicleAnimation::Init(IVehicle* pVehicle, const SmartScriptTable& table)
{
	return {};
}

void VehicleAnimation::Reset()
{
}

void VehicleAnimation::Release()
{
}

bool VehicleAnimation::StartAnimation()
{
	return {};
}

void VehicleAnimation::StopAnimation()
{
}

bool VehicleAnimation::ChangeState(TVehicleAnimStateId stateId)
{
	return {};
}

TVehicleAnimStateId VehicleAnimation::GetState()
{
	return {};
}

string VehicleAnimation::GetStateName(TVehicleAnimStateId stateId)
{
	return {};
}

TVehicleAnimStateId VehicleAnimation::GetStateId(const string& name)
{
	return {};
}

void VehicleAnimation::SetSpeed(float speed)
{
}

float VehicleAnimation::GetAnimTime(bool raw)
{
	return {};
}

void VehicleAnimation::ToggleManualUpdate(bool isEnabled)
{
}

bool VehicleAnimation::IsUsingManualUpdates()
{
	return {};
}

void VehicleAnimation::SetTime(float time, bool force)
{
}

////////////////////////////////////////////////////////////////////////////////
