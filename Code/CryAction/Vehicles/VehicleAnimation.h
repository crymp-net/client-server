#pragma once

#include "CryCommon/CryAction/IVehicleSystem.h"

class VehicleAnimation : public IVehicleAnimation
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x38 - sizeof(IVehicleAnimation)] = {};
#else
	unsigned char m_data[0x20 - sizeof(IVehicleAnimation)] = {};
#endif

public:
	VehicleAnimation();
	virtual ~VehicleAnimation();

	////////////////////////////////////////////////////////////////////////////////
	// IVehicleAnimation
	////////////////////////////////////////////////////////////////////////////////

	bool Init(IVehicle* pVehicle, const SmartScriptTable& table) override;
	void Reset() override;
	void Release() override;

	bool StartAnimation() override;
	void StopAnimation() override;

	bool ChangeState(TVehicleAnimStateId stateId) override;

	TVehicleAnimStateId GetState() override;
	string GetStateName(TVehicleAnimStateId stateId) override;
	TVehicleAnimStateId GetStateId(const string& name) override;

	void SetSpeed(float speed) override;

	float GetAnimTime(bool raw) override;

	void ToggleManualUpdate(bool isEnabled) override;
	bool IsUsingManualUpdates() override;

	void SetTime(float time, bool force) override;

	////////////////////////////////////////////////////////////////////////////////
};

#ifdef BUILD_64BIT
static_assert(sizeof(VehicleAnimation) == 0x38);
#else
static_assert(sizeof(VehicleAnimation) == 0x20);
#endif
