#include <cstdint>

#include "ScriptBind_AI.h"

extern std::uintptr_t CRYACTION_BASE;

ScriptBind_AI::ScriptBind_AI(ISystem* pSystem)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0xf9240;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0xada90;
#endif

	(this->*reinterpret_cast<void(ScriptBind_AI::*&)(ISystem*)>(ctor))(pSystem);
}
