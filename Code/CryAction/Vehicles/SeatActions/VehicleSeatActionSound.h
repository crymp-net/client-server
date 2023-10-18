#pragma once

#include "CryCommon/CryAction/IVehicleSystem.h"

class VehicleSeatActionSound : public IVehicleSeatAction
{
	void* m_reserved1 = nullptr;
	void* m_reserved2 = nullptr;
	unsigned int m_reserved3 = 0;
	unsigned int m_reserved4 = 0;
	unsigned int m_reserved5 = 0;
	CryStringT<char> m_reserved6;

public:
	VehicleSeatActionSound();

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
	// IVehicleSeatAction
	////////////////////////////////////////////////////////////////////////////////

	bool Init(IVehicle* pVehicle, TVehicleSeatId seatId, const SmartScriptTable& table) override;
	void Reset() override;
	void Release() override;

	void StartUsing(EntityId passengerId) override;
	void StopUsing() override;
	void OnAction(const TVehicleActionId actionId, int activationMode, float value) override;

	void GetMemoryStatistics(ICrySizer*) override;

	void PostSerialize() override;

	////////////////////////////////////////////////////////////////////////////////

	static void Register(IVehicleSystem* pVehicleSystem);
};

#ifdef BUILD_64BIT
static_assert(sizeof(VehicleSeatActionSound) == 0x30);
#else
static_assert(sizeof(VehicleSeatActionSound) == 0x1c);
#endif
