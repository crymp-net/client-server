#include <cstdint>

#include "ScriptBind_DialogSystem.h"

extern std::uintptr_t CRYACTION_BASE;

ScriptBind_DialogSystem::ScriptBind_DialogSystem(ISystem* pSystem, IDialogSystem* pDialogSystem)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x2aac70;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x1d02e0;
#endif

	(this->*reinterpret_cast<void(ScriptBind_DialogSystem::*&)(ISystem*, IDialogSystem*)>(ctor))(pSystem, pDialogSystem);
}
