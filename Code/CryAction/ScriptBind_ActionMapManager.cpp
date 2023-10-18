#include <cstdint>

#include "ScriptBind_ActionMapManager.h"

ScriptBind_ActionMapManager::ScriptBind_ActionMapManager(ISystem* pSystem, IActionMapManager* pActionMapManager)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30517c60;
#else
	std::uintptr_t ctor = 0x30515500;
#endif

	(this->*reinterpret_cast<void(ScriptBind_ActionMapManager::*&)(ISystem*, IActionMapManager*)>(ctor))(pSystem, pActionMapManager);
}
