#include <cstdint>

#include "ScriptBind_ItemSystem.h"

ScriptBind_ItemSystem::ScriptBind_ItemSystem(ISystem* pSystem, ItemSystem* pItemSystem, IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x305328d0;
#else
	std::uintptr_t ctor = 0x30525d40;
#endif

	(this->*reinterpret_cast<void(ScriptBind_ItemSystem::*&)(ISystem*, ItemSystem*, IGameFramework*)>(ctor))(pSystem, pItemSystem, pGameFramework);
}
