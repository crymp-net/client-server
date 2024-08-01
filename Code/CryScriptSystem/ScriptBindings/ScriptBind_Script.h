#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

class ScriptTimerManager;

class ScriptBind_Script : public CScriptableBase
{
	ScriptTimerManager & GetTimerManager();

public:
	ScriptBind_Script(IScriptSystem *pSS);

	int LoadScript(IFunctionHandler *pH);
	int ReloadScript(IFunctionHandler *pH);
	int ReloadScripts(IFunctionHandler *pH);
	int ReloadEntityScript(IFunctionHandler *pH, const char *className);
	int UnloadScript(IFunctionHandler *pH);
	int DumpLoadedScripts(IFunctionHandler *pH);
	int SetTimer(IFunctionHandler *pH, int nMilliseconds, HSCRIPTFUNCTION hFunc);
	int SetTimerForFunction(IFunctionHandler *pH, int nMilliseconds, const char *sFunctionName);
	int KillTimer(IFunctionHandler *pH, ScriptHandle nTimerId);
};
