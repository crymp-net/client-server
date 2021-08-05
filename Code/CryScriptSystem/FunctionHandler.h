#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

struct lua_State;

class ScriptSystem;

class FunctionHandler : public IFunctionHandler
{
	lua_State *m_L = nullptr;
	ScriptSystem *m_pSS = nullptr;
	const char *m_funcName = nullptr;
	int m_paramIdOffset = 0;

public:
	FunctionHandler(lua_State *L, ScriptSystem *pSS, const char *funcName, int paramIdOffset)
	: m_L(L),
	  m_pSS(pSS),
	  m_funcName(funcName),
	  m_paramIdOffset(paramIdOffset)
	{
	}

	//////////////////////
	// IFunctionHandler //
	//////////////////////

	IScriptSystem *GetIScriptSystem() override;

	void *GetThis() override;
	bool GetSelfAny(ScriptAnyValue & any) override;

	const char *GetFuncName() override;

	int GetParamCount() override;
	ScriptVarType GetParamType(int index) override;
	bool GetParamAny(int index, ScriptAnyValue & any) override;

	int EndFunctionAny(const ScriptAnyValue & any) override;
	int EndFunctionAny(const ScriptAnyValue & any1, const ScriptAnyValue & any2) override;
	int EndFunctionAny(const ScriptAnyValue & any1, const ScriptAnyValue & any2, const ScriptAnyValue & any3) override;
	int EndFunction() override;
};
