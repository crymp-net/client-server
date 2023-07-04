#include <cstdint>

#include "TimeDemoRecorder.h"

TimeDemoRecorder::TimeDemoRecorder(ISystem* pSystem)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x3075d9e0;
#else
	std::uintptr_t ctor = 0x3069cf00;
#endif

	(this->*reinterpret_cast<void(TimeDemoRecorder::*&)(ISystem*)>(ctor))(pSystem);
}
