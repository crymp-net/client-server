#include <cstdint>

#include "ScriptBind_DialogSystem.h"

ScriptBind_DialogSystem::ScriptBind_DialogSystem(ISystem* pSystem, IDialogSystem* pDialogSystem)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x307aac70;
#else
	std::uintptr_t ctor = 0x306d02e0;
#endif

	(this->*reinterpret_cast<void(ScriptBind_DialogSystem::*&)(ISystem*, IDialogSystem*)>(ctor))(pSystem, pDialogSystem);
}
