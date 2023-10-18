#pragma once

#include "VehiclePartBase.h"

class VehiclePartSubPart : public VehiclePartBase
{
	Matrix34 m_reserved;

public:
	VehiclePartSubPart();

	static void Register(IVehicleSystem* pVehicleSystem);
};

#ifdef BUILD_64BIT
static_assert(sizeof(VehiclePartSubPart) == 0x1e8);
#else
static_assert(sizeof(VehiclePartSubPart) == 0x198);
#endif
