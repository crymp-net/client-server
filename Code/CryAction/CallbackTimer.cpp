#include <cstdint>

#include "CallbackTimer.h"

extern std::uintptr_t CRYACTION_BASE;

CallbackTimer::CallbackTimer()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x316e70;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x21a3c0;
#endif

	(this->*reinterpret_cast<void(CallbackTimer::*&)()>(ctor))();
}

void CallbackTimer::UpdateTimer()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x3172f0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x21a580;
#endif

	(this->*reinterpret_cast<void(CallbackTimer::*&)()>(func))();
}
