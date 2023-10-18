#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

struct ISystem;

class ScriptBind_AI : public CScriptableBase
{
#ifdef BUILD_64BIT
	unsigned char m_data[0xc0 - sizeof(CScriptableBase)] = {};
#else
	unsigned char m_data[0x80 - sizeof(CScriptableBase)] = {};
#endif

public:
	explicit ScriptBind_AI(ISystem* pSystem);
};
