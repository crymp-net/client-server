#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

struct ISystem;
struct IActionMapManager;

class ScriptBind_ActionMapManager : public CScriptableBase
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x70 - sizeof(CScriptableBase)] = {};
#else
	unsigned char m_data[0x58 - sizeof(CScriptableBase)] = {};
#endif

public:
	explicit ScriptBind_ActionMapManager(ISystem* pSystem, IActionMapManager* pActionMapManager);
};
