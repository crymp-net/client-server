#include <cstdint>

#include "VehicleSeatGroup.h"

bool VehicleSeatGroup::Init(IVehicle* pVehicle, const SmartScriptTable& seatGroup)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x3059cae0;
#else
	std::uintptr_t func = 0x30571af0;
#endif

	return (this->*reinterpret_cast<bool(VehicleSeatGroup::*&)(IVehicle*, const SmartScriptTable&)>(func))(
		pVehicle, seatGroup
	);
}
