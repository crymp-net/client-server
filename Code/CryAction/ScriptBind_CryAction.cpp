#include <cstdint>

#include "ScriptBind_CryAction.h"

ScriptBind_CryAction::ScriptBind_CryAction(IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x308304c0;
#else
	std::uintptr_t ctor = 0x3074bd50;
#endif

	(this->*reinterpret_cast<void(ScriptBind_CryAction::*&)(IGameFramework*)>(ctor))(pGameFramework);
}
