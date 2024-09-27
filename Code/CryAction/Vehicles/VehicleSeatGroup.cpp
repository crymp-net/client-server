#include <cstdint>

#include "VehicleSeatGroup.h"

extern std::uintptr_t CRYACTION_BASE;

bool VehicleSeatGroup::Init(IVehicle* pVehicle, const SmartScriptTable& seatGroup)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x9cae0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x71af0;
#endif

	return (this->*reinterpret_cast<bool(VehicleSeatGroup::*&)(IVehicle*, const SmartScriptTable&)>(func))(
		pVehicle, seatGroup
	);
}
