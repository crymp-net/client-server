#pragma once

#include "CryCommon/CryAction/IVehicleSystem.h"

class VehicleSeatActionSteeringWheel : public IVehicleSeatAction
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x70 - 0x8] = {};
#else
	unsigned char m_data[0x58 - 0x4] = {};
#endif

public:
	VehicleSeatActionSteeringWheel();

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
static_assert(sizeof(VehicleSeatActionSteeringWheel) == 0x70);
#else
static_assert(sizeof(VehicleSeatActionSteeringWheel) == 0x58);
#endif
