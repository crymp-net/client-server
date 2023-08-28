#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

struct ISound;

class ScriptBind_Sound : public CScriptableBase
{
	bool m_isMenuMusicLoaded = false;

	ISound *GetSound(IFunctionHandler *pH, int index);
	SmartScriptTable CreateSoundTable(IFunctionHandler *pH, ISound *pSound);

	static int SoundTableDestructor(IFunctionHandler *pH, void *buffer, int size);

public:
	ScriptBind_Sound(IScriptSystem *pSS);

	// sound
	int AddToScaleGroup(IFunctionHandler *pH);
	int GetSoundLength(IFunctionHandler *pH);
	int GetSoundVolume(IFunctionHandler *pH);
	int IsPlaying(IFunctionHandler *pH);
	int Load3DSound(IFunctionHandler *pH, const char *sound);
	int LoadSound(IFunctionHandler *pH, const char *sound);
	int LoadStreamSound(IFunctionHandler *pH, const char *sound);
	int Play(IFunctionHandler* pH, const char* sound, Vec3 pos);
	int PlayEx(IFunctionHandler *pH, const char *sound, Vec3 pos, uint32_t flags, float volume, float minRadius, float maxRadius, uint32_t semantic);
	int PlaySound(IFunctionHandler *pH);
	int Precache(IFunctionHandler *pH, const char *sound, uint32_t flags);
	int RegisterSoundMood(IFunctionHandler *pH, const char *mood);
	int RegisterWeightedEaxEnvironment(IFunctionHandler *pH, const char *preset);
	int SetFadeTime(IFunctionHandler *pH);
	int SetGroupScale(IFunctionHandler *pH, int group, float scale);
	int SetMasterVolumeScale(IFunctionHandler *pH, float scale);
	int SetMinMaxDistance(IFunctionHandler *pH);
	int SetParameterValue(IFunctionHandler *pH);
	int GetParameterValue(IFunctionHandler* pH);
	int SetSoundLoop(IFunctionHandler *pH);
	int SetSoundPaused(IFunctionHandler *pH);
	int SetSoundPosition(IFunctionHandler *pH);
	int SetSoundVolume(IFunctionHandler *pH);
	int Silence(IFunctionHandler *pH);
	int StopSound(IFunctionHandler *pH);
	int UnregisterSoundMood(IFunctionHandler *pH, const char *mood);
	int UnregisterWeightedEaxEnvironment(IFunctionHandler *pH, const char *preset);
	int UpdateSoundMood(IFunctionHandler *pH, const char *mood, float fade, uint32_t fadeTimeMs);
	int UpdateWeightedEaxEnvironment(IFunctionHandler *pH, const char *preset, float weight);

	// music
	int EndMusicTheme(IFunctionHandler *pH);
	int PlayPattern(IFunctionHandler *pH);
	int PlayStinger(IFunctionHandler *pH);
	int SetDefaultMusicMood(IFunctionHandler *pH, const char *mood);
	int SetMenuMusic(IFunctionHandler *pH, bool playMenuMusic, const char *theme, const char *mood);
	int SetMusicMood(IFunctionHandler *pH, const char *mood);
	int SetMusicTheme(IFunctionHandler *pH, const char *theme);
};
