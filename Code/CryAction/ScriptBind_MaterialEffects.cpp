#include <cstdint>

#include "ScriptBind_MaterialEffects.h"

ScriptBind_MaterialEffects::ScriptBind_MaterialEffects(ISystem* pSystem, MaterialEffects* pMaterialEffects)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30777040;
#else
	std::uintptr_t ctor = 0x306b2810;
#endif

	(this->*reinterpret_cast<void(ScriptBind_MaterialEffects::*&)(ISystem*, MaterialEffects*)>(ctor))(pSystem, pMaterialEffects);
}
