#include <cstdint>

#include "ScriptBind_AI.h"

ScriptBind_AI::ScriptBind_AI(ISystem* pSystem)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x305f9240;
#else
	std::uintptr_t ctor = 0x305ada90;
#endif

	(this->*reinterpret_cast<void(ScriptBind_AI::*&)(ISystem*)>(ctor))(pSystem);
}
