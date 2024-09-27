#pragma once

#include "CryCommon/CryAction/IVehicleSystem.h"

class VehicleComponent : public IVehicleComponent
{
#ifdef BUILD_64BIT
	unsigned char m_reserved1[0xb0 - sizeof(IVehicleComponent)] = {};
#else
	unsigned char m_reserved1[0x6c - sizeof(IVehicleComponent)] = {};
#endif

	bool m_reserved2 = false;
	bool m_reserved3 = false;
	bool m_isMajorComponent = false;
	float m_vehicleHealthProportion = 0;
	unsigned int m_reserved4[6] = {};

public:
	VehicleComponent();

	bool Init(IVehicle* pVehicle, const SmartScriptTable& component);

	void AddPart(IVehiclePart* pPart);

	void SetVehicleHealthProportion(float proportion);

	bool IsMajorComponent() const { return m_isMajorComponent; }

	////////////////////////////////////////////////////////////////////////////////
	// IVehicleComponent
	////////////////////////////////////////////////////////////////////////////////

	const char* GetComponentName() const override;

	unsigned int GetPartCount() const override;
	IVehiclePart* GetPart(unsigned int index) const override;

	const AABB& GetBounds() override;

	float GetDamageRatio() const override;
	void SetDamageRatio(float ratio) override;

	float GetMaxDamage() const override;

	////////////////////////////////////////////////////////////////////////////////
};
