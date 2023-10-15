#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

class ScriptBind_Movie : public CScriptableBase
{
public:
	ScriptBind_Movie(IScriptSystem *pSS);

	int PlaySequence(IFunctionHandler *pH, const char *sSequenceName);
	int StopSequence(IFunctionHandler *pH, const char *sSequenceName);
	int StopAllSequences(IFunctionHandler *pH);
	int StopAllCutScenes(IFunctionHandler *pH);
	int PauseSequences(IFunctionHandler *pH);
	int ResumeSequences(IFunctionHandler *pH);
};
