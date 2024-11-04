#pragma once

#include "CryCommon/CryAction/IVehicleSystem.h"

class VehicleSeat : public IVehicleSeat
{
#ifdef BUILD_64BIT
	unsigned char m_reserved1[0xa0 - sizeof(IVehicleSeat)] = {};
#else
	unsigned char m_reserved1[0x54 - sizeof(IVehicleSeat)] = {};
#endif

	IVehicleHelper* m_pEnterHelper = nullptr;
	IVehicleHelper* m_pExitHelper = nullptr;

#ifdef BUILD_64BIT
	unsigned char m_reserved2[0xf0] = {};
#else
	unsigned char m_reserved2[0xb0] = {};
#endif

public:
	VehicleSeat();

	////////////////////////////////////////////////////////////////////////////////
	// IVehicleSeat
	////////////////////////////////////////////////////////////////////////////////

	bool Init(IVehicle* pVehicle, TVehicleSeatId seatId, const SmartScriptTable& paramsTable) override;
	void PostInit(IVehicle* pVehicle, const SmartScriptTable& paramsTable) override;
	void Reset() override;
	void Release() override;

	const char* GetSeatName() const override;

	TVehicleSeatId GetSeatId() override;
	EntityId GetPassenger(bool remoteUser) override;
	int GetCurrentTransition() const override;

	TVehicleViewId GetCurrentView() override;
	IVehicleView* GetView(TVehicleViewId viewId) override;
	bool SetView(TVehicleViewId viewId) override;
	TVehicleViewId GetNextView(TVehicleViewId viewId) override;

	bool IsDriver() override;
	bool IsGunner() override;
	bool IsLocked() override;

	bool Enter(EntityId actorId, bool isTransitionEnabled) override;
	bool Exit(bool isTransitionEnabled, bool force) override;

	void OnPassengerDeath() override;
	void ForceAnimGraphInputs() override;
	void UnlinkPassenger(bool ragdoll) override;
	bool IsPassengerHidden() override;
	bool IsPassengerExposed() const override;

	IVehicle* GetVehicle() override;
	IVehicleHelper* GetSitHelper() override;
	const SSeatSoundParams& GetSoundParams() override;

	bool IsAnimGraphStateExisting(EntityId actorId) override;
	void OnCameraShake(float& angleAmount, float& shiftAmount, const Vec3& pos, const char* source) const override;

	void ForceFinishExiting() override;

	////////////////////////////////////////////////////////////////////////////////

	IVehicleHelper* GetExitHelper() { return m_pExitHelper ? m_pExitHelper : m_pEnterHelper; }
};

#ifdef BUILD_64BIT
static_assert(sizeof(VehicleSeat) == 0x1a0);
#else
static_assert(sizeof(VehicleSeat) == 0x10c);
#endif
