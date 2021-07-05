#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CryEntitySystem/IEntitySystem.h"

#include "ScriptBind_Script.h"
#include "../ScriptSystem.h"
#include "../ScriptTimerManager.h"

ScriptTimerManager *ScriptBind_Script::GetTimerManager()
{
	return static_cast<CScriptSystem*>(m_pSS)->GetScriptTimerManager();
}

ScriptBind_Script::ScriptBind_Script(ISystem *pSystem, IScriptSystem *pSS)
{
	CScriptableBase::Init(pSS, pSystem);
	SetGlobalName("Script");

#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &ScriptBind_Script::

	SCRIPT_REG_FUNC(LoadScript);
	SCRIPT_REG_FUNC(ReloadScript);
	SCRIPT_REG_FUNC(ReloadScripts);
	SCRIPT_REG_TEMPLFUNC(ReloadEntityScript, "className");
	SCRIPT_REG_FUNC(UnloadScript);
	SCRIPT_REG_FUNC(DumpLoadedScripts);
	SCRIPT_REG_TEMPLFUNC(SetTimer, "nMilliseconds, Function");
	SCRIPT_REG_TEMPLFUNC(SetTimerForFunction, "nMilliseconds, Function");
	SCRIPT_REG_TEMPLFUNC(KillTimer, "nTimerId");
}

int ScriptBind_Script::LoadScript(IFunctionHandler *pH)
{
	bool raiseError = true;
	if (pH->GetParamCount() >= 3)
		pH->GetParam(3, raiseError);

	bool reload = false;
	if (pH->GetParamCount() >= 2)
		pH->GetParam(2, reload);

	const char *scriptFile = nullptr;
	pH->GetParam(1, scriptFile);

	bool status = false;

	if (scriptFile)
	{
		status = m_pSS->ExecuteFile(scriptFile, raiseError, reload);
	}

	return pH->EndFunction(status);
}

int ScriptBind_Script::ReloadScript(IFunctionHandler *pH)
{
	const char *fileName = nullptr;
	if (!pH->GetParams(fileName))
		return pH->EndFunction();

	const bool raiseError = true;
	const bool forceReload = gEnv->bEditor;

	m_pSS->ExecuteFile(fileName, raiseError, forceReload);

	return pH->EndFunction();
}

int ScriptBind_Script::ReloadScripts(IFunctionHandler *pH)
{
	SCRIPT_CHECK_PARAMETERS(0);

	m_pSS->ReloadScripts();

	return pH->EndFunction();
}

int ScriptBind_Script::ReloadEntityScript(IFunctionHandler *pH, const char *className)
{
	IEntityClass *pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(className);
	if (pClass)
	{
		pClass->LoadScript(false);
	}

	return pH->EndFunction();
}

int ScriptBind_Script::UnloadScript(IFunctionHandler *pH)
{
	const char *scriptFile = nullptr;
	if (!pH->GetParams(scriptFile))
		return pH->EndFunction();

	m_pSS->UnloadScript(scriptFile);

	return pH->EndFunction();
}

int ScriptBind_Script::DumpLoadedScripts(IFunctionHandler *pH)
{
	m_pSS->DumpLoadedScripts();

	return pH->EndFunction();
}

int ScriptBind_Script::SetTimer(IFunctionHandler *pH, int nMilliseconds, HSCRIPTFUNCTION hFunc)
{
	if (nMilliseconds < 0)
		nMilliseconds = 0;

	IScriptTable *pData = nullptr;
	if (pH->GetParamCount() > 2)
		pH->GetParam(3, pData);

	const int timerID = GetTimerManager()->AddTimer(nMilliseconds, hFunc, pData);

	ScriptHandle timerHandle;
	timerHandle.n = timerID;

	return pH->EndFunction(timerHandle);
}

int ScriptBind_Script::SetTimerForFunction(IFunctionHandler *pH, int nMilliseconds, const char *sFunctionName)
{
	if (nMilliseconds < 0)
		nMilliseconds = 0;

	IScriptTable *pData = nullptr;
	if (pH->GetParamCount() > 2)
		pH->GetParam(3, pData);

	const ScriptTimerID timerID = GetTimerManager()->AddTimer(nMilliseconds, sFunctionName, pData);

	ScriptHandle timerHandle;
	timerHandle.n = timerID;

	return pH->EndFunction(timerHandle);
}

int ScriptBind_Script::KillTimer(IFunctionHandler *pH, ScriptHandle nTimerId)
{
	GetTimerManager()->RemoveTimer(static_cast<ScriptTimerID>(nTimerId.n));

	return pH->EndFunction();
}
