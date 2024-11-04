#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

struct ISystem;
struct IDialogSystem;

class ScriptBind_DialogSystem : public CScriptableBase
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x80 - sizeof(CScriptableBase)] = {};
#else
	unsigned char m_data[0x60 - sizeof(CScriptableBase)] = {};
#endif

public:
	explicit ScriptBind_DialogSystem(ISystem* pSystem, IDialogSystem* pDialogSystem);
};
