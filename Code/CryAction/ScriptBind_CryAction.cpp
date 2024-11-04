#include <cstdint>

#include "ScriptBind_CryAction.h"

extern std::uintptr_t CRYACTION_BASE;

ScriptBind_CryAction::ScriptBind_CryAction(IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x3304c0;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x24bd50;
#endif

	(this->*reinterpret_cast<void(ScriptBind_CryAction::*&)(IGameFramework*)>(ctor))(pGameFramework);
}
