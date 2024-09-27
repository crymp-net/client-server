#include <cstdint>

#include "TimeDemoRecorder.h"

extern std::uintptr_t CRYACTION_BASE;

TimeDemoRecorder::TimeDemoRecorder(ISystem* pSystem)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x25d9e0;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x19cf00;
#endif

	(this->*reinterpret_cast<void(TimeDemoRecorder::*&)(ISystem*)>(ctor))(pSystem);
}

void TimeDemoRecorder::PreUpdate()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x25d450;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x19caf0;
#endif

	(this->*reinterpret_cast<void(TimeDemoRecorder::*&)()>(func))();
}
