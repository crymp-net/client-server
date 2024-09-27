#include <cstdint>

#include "ScriptBind_ActionMapManager.h"

extern std::uintptr_t CRYACTION_BASE;

ScriptBind_ActionMapManager::ScriptBind_ActionMapManager(ISystem* pSystem, IActionMapManager* pActionMapManager)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x17c60;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x15500;
#endif

	(this->*reinterpret_cast<void(ScriptBind_ActionMapManager::*&)(ISystem*, IActionMapManager*)>(ctor))(pSystem, pActionMapManager);
}
