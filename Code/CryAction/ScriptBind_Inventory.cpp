#include <cstdint>

#include "ScriptBind_Inventory.h"

extern std::uintptr_t CRYACTION_BASE;

ScriptBind_Inventory::ScriptBind_Inventory(ISystem* pSystem, IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x319c0;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x25420;
#endif

	(this->*reinterpret_cast<void(ScriptBind_Inventory::*&)(ISystem*, IGameFramework*)>(ctor))(pSystem, pGameFramework);
}
