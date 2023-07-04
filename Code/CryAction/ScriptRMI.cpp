#include <cstdint>

#include "ScriptRMI.h"

ScriptRMI::ScriptRMI()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x307c9870;
#else
	std::uintptr_t ctor = 0x306e4190;
#endif

	(this->*reinterpret_cast<void(ScriptRMI::*&)()>(ctor))();
}
