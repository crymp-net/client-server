#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

struct ISystem;
struct IGameFramework;

class ScriptBind_Inventory : public CScriptableBase
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x78 - sizeof(CScriptableBase)] = {};
#else
	unsigned char m_data[0x5c - sizeof(CScriptableBase)] = {};
#endif

public:
	explicit ScriptBind_Inventory(ISystem* pSystem, IGameFramework* pGameFramework);
};
