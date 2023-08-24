#pragma once

#include "CryCommon/CryAction/IVehicleSystem.h"

class VehicleDamageBehaviorSink : public IVehicleDamageBehavior
{
	void* m_reserved1 = nullptr;
	unsigned int m_reserved2 = 0;
	unsigned int m_reserved3 = 0;
	unsigned int m_reserved4 = 0;

public:
	VehicleDamageBehaviorSink();

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
static_assert(sizeof(VehicleDamageBehaviorSink) == 0x20);
#else
static_assert(sizeof(VehicleDamageBehaviorSink) == 0x14);
#endif
