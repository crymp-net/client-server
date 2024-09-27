#pragma once

#include "CryCommon/CryAction/IVehicleSystem.h"

class VehicleDamageBehaviorDestroy : public IVehicleDamageBehavior
{
	void* m_reserved1 = nullptr;

public:
	VehicleDamageBehaviorDestroy();

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
static_assert(sizeof(VehicleDamageBehaviorDestroy) == 0x10);
#else
static_assert(sizeof(VehicleDamageBehaviorDestroy) == 0x8);
#endif
