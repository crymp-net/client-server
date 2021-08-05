#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CryMovie/IMovieSystem.h"

#include "ScriptBind_Movie.h"

ScriptBind_Movie::ScriptBind_Movie(IScriptSystem* pSS)
{
	CScriptableBase::Init(pSS, gEnv->pSystem);
	SetGlobalName("Movie");

#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &ScriptBind_Movie::

	SCRIPT_REG_TEMPLFUNC(PlaySequence, "sSequenceName");
	SCRIPT_REG_TEMPLFUNC(StopSequence, "sSequenceName");
	SCRIPT_REG_TEMPLFUNC(AbortSequence, "sSequenceName");
	SCRIPT_REG_FUNC(StopAllSequences);
	SCRIPT_REG_FUNC(StopAllCutScenes);
	SCRIPT_REG_FUNC(PauseSequences);
	SCRIPT_REG_FUNC(ResumeSequences);
}

int ScriptBind_Movie::PlaySequence(IFunctionHandler *pH, const char *sSequenceName)
{
	bool resetFx = true;
	if (pH->GetParamCount() >= 2)
		pH->GetParam(2, resetFx);

	IMovieSystem *pMovieSystem = gEnv->pMovieSystem;

	if (pMovieSystem)
		pMovieSystem->PlaySequence(sSequenceName, resetFx);

	return pH->EndFunction();
}

int ScriptBind_Movie::StopSequence(IFunctionHandler *pH, const char *sSequenceName)
{
	IMovieSystem *pMovieSystem = gEnv->pMovieSystem;

	if (pMovieSystem)
		pMovieSystem->StopSequence(sSequenceName);

	return pH->EndFunction();
}

int ScriptBind_Movie::AbortSequence(IFunctionHandler *pH, const char *sSequenceName)
{
	bool leaveTime = false;
	if (pH->GetParamCount() >= 2)
		pH->GetParam(2, leaveTime);

	IMovieSystem *pMovieSystem = gEnv->pMovieSystem;

	if (pMovieSystem)
	{
		IAnimSequence *seq = pMovieSystem->FindSequence(sSequenceName);
		if (seq)
			pMovieSystem->AbortSequence(seq, leaveTime);
	}

	return pH->EndFunction();
}

int ScriptBind_Movie::StopAllSequences(IFunctionHandler *pH)
{
	IMovieSystem *pMovieSystem = gEnv->pMovieSystem;

	if (pMovieSystem)
		pMovieSystem->StopAllSequences();

	return pH->EndFunction();
}

int ScriptBind_Movie::StopAllCutScenes(IFunctionHandler *pH)
{
	IMovieSystem *pMovieSystem = gEnv->pMovieSystem;

	if (pMovieSystem)
		pMovieSystem->StopAllCutScenes();

	return pH->EndFunction();
}

int ScriptBind_Movie::PauseSequences(IFunctionHandler *pH)
{
	IMovieSystem *pMovieSystem = gEnv->pMovieSystem;

	if (pMovieSystem)
		pMovieSystem->Pause();

	return pH->EndFunction();
}

int ScriptBind_Movie::ResumeSequences(IFunctionHandler *pH)
{
	IMovieSystem *pMovieSystem = gEnv->pMovieSystem;

	if (pMovieSystem)
		pMovieSystem->Resume();

	return pH->EndFunction();
}
