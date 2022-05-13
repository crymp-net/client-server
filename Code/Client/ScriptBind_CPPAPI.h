#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

class ScriptBind_CPPAPI : public CScriptableBase
{
public:
	ScriptBind_CPPAPI();
	~ScriptBind_CPPAPI();

	int AddCCommand(IFunctionHandler *pH, const char *name, HSCRIPTFUNCTION handler);
	int ApplyMaskOne(IFunctionHandler *pH, ScriptHandle entity, int mask, bool apply);
	int ApplyMaskAll(IFunctionHandler* pH, int mask, bool apply);
	int FSetCVar(IFunctionHandler *pH, const char *cvar, const char *value);
	int GetLocaleInformation(IFunctionHandler *pH);
	int GetMapName(IFunctionHandler *pH);
	int MakeUUID(IFunctionHandler *pH, const char *salt);
	int Random(IFunctionHandler *pH);
	int Request(IFunctionHandler *pH, SmartScriptTable params, HSCRIPTFUNCTION callback);
	int SetCallback(IFunctionHandler *pH, int callback, HSCRIPTFUNCTION handler);
	int SHA256(IFunctionHandler *pH, const char *text);
	int URLEncode(IFunctionHandler *pH, const char *text);
	int GetMasters(IFunctionHandler *pH);
	int GetRenderType(IFunctionHandler* pH);
	int GetKeyName(IFunctionHandler* pH, const char* action);
	int IsKeyUsed(IFunctionHandler* pH, const char* key);
	int CreateKeyBind(IFunctionHandler* pH, const char* key, const char* action);
	int ClearKeyBinds(IFunctionHandler* pH);
	int GetModelFilePath(IFunctionHandler* pH, ScriptHandle entityId, int slot);
};
