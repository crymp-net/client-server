#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

class ScriptBind_Movie : public CScriptableBase
{
public:
	ScriptBind_Movie(ISystem *pSystem, IScriptSystem *pSS);

	//! <code>Movie.PlaySequence(sSequenceName)</code>
	//! <param name="sSequenceName">Sequence name.</param>
	//! <description>Plays the specified sequence.</description>
	int PlaySequence(IFunctionHandler *pH, const char *sSequenceName);

	//! <code>Movie.StopSequence(sSequenceName)</code>
	//! <param name="sSequenceName">Sequence name.</param>
	//! <description>Stops the specified sequence.</description>
	int StopSequence(IFunctionHandler *pH, const char *sSequenceName);

	//! <code>Movie.AbortSequence(sSequenceName)</code>
	//! <param name="sSequenceName">Sequence name.</param>
	//! <description>Aborts the specified sequence.</description>
	int AbortSequence(IFunctionHandler *pH, const char *sSequenceName);

	//! <code>Movie.StopAllSequences()</code>
	//! <description>Stops all the video sequences.</description>
	int StopAllSequences(IFunctionHandler *pH);

	//! <code>Movie.StopAllCutScenes()</code>
	//! <description>Stops all the cut scenes.</description>
	int StopAllCutScenes(IFunctionHandler *pH);

	//! <code>Movie.PauseSequences()</code>
	//! <description>Pauses all the sequences.</description>
	int PauseSequences(IFunctionHandler *pH);

	//! <code>Movie.ResumeSequences()</code>
	//! <description>Resume all the sequences.</description>
	int ResumeSequences(IFunctionHandler *pH);
};
