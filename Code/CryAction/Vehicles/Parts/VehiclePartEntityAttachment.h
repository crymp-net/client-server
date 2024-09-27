#pragma once

#include "VehiclePartBase.h"

class VehiclePartEntityAttachment : public VehiclePartBase
{
	EntityId m_reserved1;
	Matrix34 m_reserved2;
	Matrix34 m_reserved3;

public:
	VehiclePartEntityAttachment();

	static void Register(IVehicleSystem* pVehicleSystem);
};

#ifdef BUILD_64BIT
static_assert(sizeof(VehiclePartEntityAttachment) == 0x220);
#else
static_assert(sizeof(VehiclePartEntityAttachment) == 0x1cc);
#endif
