#include <cstdint>

#include "ScriptBind_Inventory.h"

ScriptBind_Inventory::ScriptBind_Inventory(ISystem* pSystem, IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x305319c0;
#else
	std::uintptr_t ctor = 0x30525420;
#endif

	(this->*reinterpret_cast<void(ScriptBind_Inventory::*&)(ISystem*, IGameFramework*)>(ctor))(pSystem, pGameFramework);
}
