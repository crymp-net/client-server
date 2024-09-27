#pragma once

#include "VehiclePartBase.h"

class VehiclePartStatic : public VehiclePartBase
{
	CryStringT<char> m_reserved1;
	CryStringT<char> m_reserved2;
	CryStringT<char> m_reserved3;

public:
	VehiclePartStatic();

	static void Register(IVehicleSystem* pVehicleSystem);
};

#ifdef BUILD_64BIT
static_assert(sizeof(VehiclePartStatic) == 0x1d0);
#else
static_assert(sizeof(VehiclePartStatic) == 0x174);
#endif
