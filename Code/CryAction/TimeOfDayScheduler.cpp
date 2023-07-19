#include <cstdint>

#include "TimeOfDayScheduler.h"

TimeOfDayScheduler::TimeOfDayScheduler()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30830ad0;
#else
	std::uintptr_t ctor = 0x3074cff0;
#endif

	(this->*reinterpret_cast<void(TimeOfDayScheduler::*&)()>(ctor))();
}
