#pragma once

#include <vector>
#include <string>

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

struct lua_State;

namespace ScriptUtil
{
	std::vector<std::string> GetCallStack(lua_State *L, int level = 0);

	ScriptVarType LuaTypeToScriptVarType(int type);

	const char *ScriptAnyTypeToString(ScriptAnyType type);
	const char *ScriptVarTypeToString(ScriptVarType type);
}
