#include <cstdint>

#include "CallbackTimer.h"

CallbackTimer::CallbackTimer()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30816e70;
#else
	std::uintptr_t ctor = 0x3071a3c0;
#endif

	(this->*reinterpret_cast<void(CallbackTimer::*&)()>(ctor))();
}
