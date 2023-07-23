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

void CallbackTimer::UpdateTimer()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x308172f0;
#else
	std::uintptr_t func = 0x3071a580;
#endif

	(this->*reinterpret_cast<void(CallbackTimer::*&)()>(func))();
}
