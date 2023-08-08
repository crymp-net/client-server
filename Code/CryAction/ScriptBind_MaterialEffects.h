#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

class MaterialEffects;

class ScriptBind_MaterialEffects : public CScriptableBase
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x78 - sizeof(CScriptableBase)] = {};
#else
	unsigned char m_data[0x5c - sizeof(CScriptableBase)] = {};
#endif

public:
	ScriptBind_MaterialEffects(ISystem* pSystem, MaterialEffects* pMaterialEffects);
};
