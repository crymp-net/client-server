#include <cstdint>

#include "TimeOfDayScheduler.h"

extern std::uintptr_t CRYACTION_BASE;

TimeOfDayScheduler::TimeOfDayScheduler()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x330ad0;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x24cff0;
#endif

	(this->*reinterpret_cast<void(TimeOfDayScheduler::*&)()>(ctor))();
}
