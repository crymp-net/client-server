#include <cstdint>

#include "VehicleCVars.h"

extern std::uintptr_t CRYACTION_BASE;

VehicleCVars::VehicleCVars()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x33d80;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x26cf0;
#endif

	(this->*reinterpret_cast<void(VehicleCVars::*&)()>(ctor))();
}
