#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

class ScriptBind_Sound : public CScriptableBase
{
public:
	ScriptBind_Sound(ISystem *pSystem, IScriptSystem *pSS);

	int AddToScaleGroup(IFunctionHandler *pH);
	int EndMusicTheme(IFunctionHandler *pH);
	int GetSoundLength(IFunctionHandler *pH);
	int GetSoundVolume(IFunctionHandler *pH);
	int IsPlaying(IFunctionHandler *pH);
	int Load3DSound(IFunctionHandler *pH);
	int LoadSound(IFunctionHandler *pH);
	int LoadStreamSound(IFunctionHandler *pH);
	int Play(IFunctionHandler *pH);
	int PlayEx(IFunctionHandler *pH);
	int PlayPattern(IFunctionHandler *pH);
	int PlaySound(IFunctionHandler *pH);
	int PlaySoundFadeUnderwater(IFunctionHandler *pH);
	int PlayStinger(IFunctionHandler *pH);
	int Precache(IFunctionHandler *pH);
	int RegisterReverbSound(IFunctionHandler *pH);
	int RegisterSoundMood(IFunctionHandler *pH);
	int RegisterWeightedEaxEnvironment(IFunctionHandler *pH);
	int SetDefaultMusicMood(IFunctionHandler *pH);
	int SetEaxEnvironment(IFunctionHandler *pH);
	int SetFadeTime(IFunctionHandler *pH);
	int SetGroupScale(IFunctionHandler *pH);
	int SetMasterVolumeScale(IFunctionHandler *pH);
	int SetMenuMusic(IFunctionHandler *pH);
	int SetMinMaxDistance(IFunctionHandler *pH);
	int SetMusicMood(IFunctionHandler *pH);
	int SetMusicTheme(IFunctionHandler *pH);
	int SetParameterValue(IFunctionHandler *pH);
	int SetSoundLoop(IFunctionHandler *pH);
	int SetSoundPaused(IFunctionHandler *pH);
	int SetSoundPosition(IFunctionHandler *pH);
	int SetSoundProperties(IFunctionHandler *pH);
	int SetSoundVolume(IFunctionHandler *pH);
	int SetWeatherAndPerceptionFlags(IFunctionHandler *pH);
	int Silence(IFunctionHandler *pH);
	int StopSound(IFunctionHandler *pH);
	int UnregisterSoundMood(IFunctionHandler *pH);
	int UnregisterWeightedEaxEnvironment(IFunctionHandler *pH);
	int UpdateSoundMood(IFunctionHandler *pH);
	int UpdateWeightedEaxEnvironment(IFunctionHandler *pH);
};
