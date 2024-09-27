#pragma once

struct IVehicle;

class SmartScriptTable;

class VehicleSeatGroup
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x30] = {};
#else
	unsigned char m_data[0x18] = {};
#endif

public:
	VehicleSeatGroup() = default;

	bool Init(IVehicle* pVehicle, const SmartScriptTable& seatGroup);
};

#ifdef BUILD_64BIT
static_assert(sizeof(VehicleSeatGroup) == 0x30);
#else
static_assert(sizeof(VehicleSeatGroup) == 0x18);
#endif
