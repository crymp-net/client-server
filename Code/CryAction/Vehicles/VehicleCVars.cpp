#include <cstdint>

#include "VehicleCVars.h"

VehicleCVars::VehicleCVars()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30533d80;
#else
	std::uintptr_t ctor = 0x30526cf0;
#endif

	(this->*reinterpret_cast<void(VehicleCVars::*&)()>(ctor))();
}
