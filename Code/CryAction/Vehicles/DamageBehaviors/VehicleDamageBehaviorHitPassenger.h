#pragma once

#include "CryCommon/CryAction/IVehicleSystem.h"

class VehicleDamageBehaviorHitPassenger : public IVehicleDamageBehavior
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x18 - 0x8] = {};
#else
	unsigned char m_data[0x10 - 0x4] = {};
#endif

public:
	VehicleDamageBehaviorHitPassenger();

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
	// IVehicleDamageBehavior
	////////////////////////////////////////////////////////////////////////////////

	bool Init(IVehicle* pVehicle, const SmartScriptTable& table) override;
	void Reset() override;
	void Release() override;
	void GetMemoryStatistics(ICrySizer*) override;

	void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& params) override;

	////////////////////////////////////////////////////////////////////////////////

	static void Register(IVehicleSystem* pVehicleSystem);
};

#ifdef BUILD_64BIT
static_assert(sizeof(VehicleDamageBehaviorHitPassenger) == 0x18);
#else
static_assert(sizeof(VehicleDamageBehaviorHitPassenger) == 0x10);
#endif