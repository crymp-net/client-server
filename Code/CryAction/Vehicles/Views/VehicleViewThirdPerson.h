#pragma once

#include "CryCommon/CryAction/IVehicleSystem.h"

class VehicleViewThirdPerson : public IVehicleView
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x158 - 0x8] = {};
#else
	unsigned char m_data[0x128 - 0x4] = {};
#endif

public:
	VehicleViewThirdPerson();

	////////////////////////////////////////////////////////////////////////////////
	// IVehicleEventListener
	////////////////////////////////////////////////////////////////////////////////

	void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override;

	////////////////////////////////////////////////////////////////////////////////
	// IVehicleObject
	////////////////////////////////////////////////////////////////////////////////

	TVehicleObjectId GetId() override;

	void Serialize(TSerialize ser, unsigned aspects) override;

	void Update(const float deltaTime) override;

	////////////////////////////////////////////////////////////////////////////////
	// IVehicleView
	////////////////////////////////////////////////////////////////////////////////

	bool Init(CVehicleSeat* pSeat, const SmartScriptTable& table) override;
	void Reset() override;
	void Release() override;
	void ResetPosition() override;

	const char* GetName() override;

	bool IsThirdPerson() override;

	bool IsPassengerHidden() override;

	void OnStartUsing(EntityId playerId) override;

	void OnStopUsing() override;

	void OnAction(const TVehicleActionId actionId, int activationMode, float value) override;
	void UpdateView(SViewParams& viewParams, EntityId playerId) override;

	void SetDebugView(bool debug) override;
	bool IsDebugView() override;

	bool ShootToCrosshair() override;

	bool IsAvailableRemotely() const override;

	void GetMemoryStatistics(ICrySizer*) override;

	////////////////////////////////////////////////////////////////////////////////

	static void Register(IVehicleSystem* pVehicleSystem);
};

#ifdef BUILD_64BIT
static_assert(sizeof(VehicleViewThirdPerson) == 0x158);
#else
static_assert(sizeof(VehicleViewThirdPerson) == 0x128);
#endif
