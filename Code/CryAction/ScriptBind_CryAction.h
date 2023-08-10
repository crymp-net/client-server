#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

struct IGameFramework;

class ScriptBind_CryAction : public CScriptableBase
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x68 - sizeof(CScriptableBase)] = {};
#else
	unsigned char m_data[0x54 - sizeof(CScriptableBase)] = {};
#endif

public:
	explicit ScriptBind_CryAction(IGameFramework* pGameFramework);
};
