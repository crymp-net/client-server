#include <cstdint>

#include "ScriptBind_MaterialEffects.h"

extern std::uintptr_t CRYACTION_BASE;

ScriptBind_MaterialEffects::ScriptBind_MaterialEffects(ISystem* pSystem, MaterialEffects* pMaterialEffects)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x277040;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x1b2810;
#endif

	(this->*reinterpret_cast<void(ScriptBind_MaterialEffects::*&)(ISystem*, MaterialEffects*)>(ctor))(pSystem, pMaterialEffects);
}
