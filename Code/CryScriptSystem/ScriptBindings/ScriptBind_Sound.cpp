#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySoundSystem/ISound.h"

#include "ScriptBind_Sound.h"

ScriptBind_Sound::ScriptBind_Sound(ISystem *pSystem, IScriptSystem *pSS)
{
	CScriptableBase::Init(pSS, pSystem);
	SetGlobalName("Sound");

	pSS->SetGlobalValue("SOUND_LOOP", FLAG_SOUND_LOOP);
	pSS->SetGlobalValue("SOUND_2D", FLAG_SOUND_2D);
	pSS->SetGlobalValue("SOUND_3D", FLAG_SOUND_3D);
	pSS->SetGlobalValue("SOUND_STEREO", FLAG_SOUND_STEREO);
	pSS->SetGlobalValue("SOUND_16BITS", FLAG_SOUND_16BITS);
	pSS->SetGlobalValue("SOUND_STREAM", FLAG_SOUND_STREAM);
	pSS->SetGlobalValue("SOUND_RELATIVE", FLAG_SOUND_RELATIVE);
	pSS->SetGlobalValue("SOUND_RADIUS", FLAG_SOUND_RADIUS);
	pSS->SetGlobalValue("SOUND_DOPPLER", FLAG_SOUND_DOPPLER);
	pSS->SetGlobalValue("SOUND_NO_SW_ATTENUATION", FLAG_SOUND_NO_SW_ATTENUATION);
	pSS->SetGlobalValue("SOUND_MUSIC", FLAG_SOUND_MUSIC);
	pSS->SetGlobalValue("SOUND_OUTDOOR", FLAG_SOUND_OUTDOOR);
	pSS->SetGlobalValue("SOUND_INDOOR", FLAG_SOUND_INDOOR);
	pSS->SetGlobalValue("SOUND_UNSCALABLE", FLAG_SOUND_UNSCALABLE);
	pSS->SetGlobalValue("SOUND_CULLING", FLAG_SOUND_CULLING);
	pSS->SetGlobalValue("SOUND_LOAD_SYNCHRONOUSLY", FLAG_SOUND_LOAD_SYNCHRONOUSLY);
	pSS->SetGlobalValue("SOUND_MANAGED", FLAG_SOUND_MANAGED);
	pSS->SetGlobalValue("SOUND_FADE_OUT_UNDERWATER", FLAG_SOUND_FADE_OUT_UNDERWATER);
	pSS->SetGlobalValue("SOUND_OBSTRUCTION", FLAG_SOUND_OBSTRUCTION);
	pSS->SetGlobalValue("SOUND_SELFMOVING", FLAG_SOUND_SELFMOVING);
	pSS->SetGlobalValue("SOUND_START_PAUSED", FLAG_SOUND_START_PAUSED);
	pSS->SetGlobalValue("SOUND_VOICE", FLAG_SOUND_VOICE);
	pSS->SetGlobalValue("SOUND_EVENT", FLAG_SOUND_EVENT);
	pSS->SetGlobalValue("SOUND_EDITOR", FLAG_SOUND_EDITOR);
	pSS->SetGlobalValue("SOUND_SPREAD", FLAG_SOUND_SPREAD);
	pSS->SetGlobalValue("SOUND_DAYLIGHT", FLAG_SOUND_DAYLIGHT);
	pSS->SetGlobalValue("SOUND_SQUELCH", FLAG_SOUND_SQUELCH);
	pSS->SetGlobalValue("SOUND_DOPPLER_PARAM", FLAG_SOUND_DOPPLER_PARAM);
	pSS->SetGlobalValue("SOUND_DEFAULT_3D", FLAG_SOUND_DEFAULT_3D);
	pSS->SetGlobalValue("SOUND_ACTIVELIST", FLAG_SOUND_ACTIVELIST);

	pSS->SetGlobalValue("SOUND_PRECACHE_LOAD_SOUND", FLAG_SOUND_PRECACHE_LOAD_SOUND);
	pSS->SetGlobalValue("SOUND_PRECACHE_LOAD_GROUP", FLAG_SOUND_PRECACHE_LOAD_GROUP);
	pSS->SetGlobalValue("SOUND_PRECACHE_STAY_IN_MEMORY", FLAG_SOUND_PRECACHE_STAY_IN_MEMORY);
	pSS->SetGlobalValue("SOUND_PRECACHE_UNLOAD_AFTER_PLAY", FLAG_SOUND_PRECACHE_UNLOAD_AFTER_PLAY);
	pSS->SetGlobalValue("SOUND_PRECACHE_UNLOAD_NOW", FLAG_SOUND_PRECACHE_UNLOAD_NOW);
	pSS->SetGlobalValue("SOUND_PRECACHE_EVENT_DEFAULT", FLAG_SOUND_PRECACHE_EVENT_DEFAULT);
	pSS->SetGlobalValue("SOUND_PRECACHE_DIALOG_DEFAULT", FLAG_SOUND_PRECACHE_DIALOG_DEFAULT);
	pSS->SetGlobalValue("SOUND_PRECACHE_READABILITY_DEFAULT", FLAG_SOUND_PRECACHE_READABILITY_DEFAULT);

	pSS->SetGlobalValue("SOUNDSCALE_MASTER", SOUNDSCALE_MASTER);
	pSS->SetGlobalValue("SOUNDSCALE_SCALEABLE", SOUNDSCALE_SCALEABLE);
	pSS->SetGlobalValue("SOUNDSCALE_DEAFNESS", SOUNDSCALE_DEAFNESS);
	pSS->SetGlobalValue("SOUNDSCALE_UNDERWATER", SOUNDSCALE_UNDERWATER);
	pSS->SetGlobalValue("SOUNDSCALE_MISSIONHINT", SOUNDSCALE_MISSIONHINT);

	pSS->SetGlobalValue("SOUND_SEMANTIC_NONE", eSoundSemantic_None);
	pSS->SetGlobalValue("SOUND_SEMANTIC_ONLYVOICE", eSoundSemantic_OnlyVoice);
	pSS->SetGlobalValue("SOUND_SEMANTIC_NOVOICE", eSoundSemantic_NoVoice);
	pSS->SetGlobalValue("SOUND_SEMANTIC_UNUSED1", eSoundSemantic_Unused1);
	pSS->SetGlobalValue("SOUND_SEMANTIC_UNUSED2", eSoundSemantic_Unused2);
	pSS->SetGlobalValue("SOUND_SEMANTIC_UNUSED3", eSoundSemantic_Unused3);
	pSS->SetGlobalValue("SOUND_SEMANTIC_UNUSED4", eSoundSemantic_Unused4);
	pSS->SetGlobalValue("SOUND_SEMANTIC_AMBIENCE", eSoundSemantic_Ambience);
	pSS->SetGlobalValue("SOUND_SEMANTIC_AMBIENCE_ONESHOT", eSoundSemantic_Ambience_OneShot);
	pSS->SetGlobalValue("SOUND_SEMANTIC_PHYSICS_COLLISION", eSoundSemantic_Physics_Collision);
	pSS->SetGlobalValue("SOUND_SEMANTIC_DIALOG", eSoundSemantic_Dialog);
	pSS->SetGlobalValue("SOUND_SEMANTIC_MP_CHAT", eSoundSemantic_MP_Chat);
	pSS->SetGlobalValue("SOUND_SEMANTIC_PHYSICS_FOOTSTEP", eSoundSemantic_Physics_Footstep);
	pSS->SetGlobalValue("SOUND_SEMANTIC_PHYSICS_GENERAL", eSoundSemantic_Physics_General);
	pSS->SetGlobalValue("SOUND_SEMANTIC_HUD", eSoundSemantic_HUD);
	pSS->SetGlobalValue("SOUND_SEMANTIC_UNUSED5", eSoundSemantic_Unused5);
	pSS->SetGlobalValue("SOUND_SEMANTIC_FLOWGRAPH", eSoundSemantic_FlowGraph);
	pSS->SetGlobalValue("SOUND_SEMANTIC_PLAYER_FOLEY_VOICE", eSoundSemantic_Player_Foley_Voice);
	pSS->SetGlobalValue("SOUND_SEMANTIC_LIVING_ENTITY", eSoundSemantic_Living_Entity);
	pSS->SetGlobalValue("SOUND_SEMANTIC_MECHANIC_ENTITY", eSoundSemantic_Mechanic_Entity);
	pSS->SetGlobalValue("SOUND_SEMANTIC_NANOSUIT", eSoundSemantic_NanoSuit);
	pSS->SetGlobalValue("SOUND_SEMANTIC_SOUNDSPOT", eSoundSemantic_SoundSpot);
	pSS->SetGlobalValue("SOUND_SEMANTIC_PARTICLE", eSoundSemantic_Particle);
	pSS->SetGlobalValue("SOUND_SEMANTIC_AI_PAIN_DEATH", eSoundSemantic_AI_Pain_Death);
	pSS->SetGlobalValue("SOUND_SEMANTIC_AI_READABILITY", eSoundSemantic_AI_Readability);
	pSS->SetGlobalValue("SOUND_SEMANTIC_AI_READABILITY_RESPONSE", eSoundSemantic_AI_Readability_Response);
	pSS->SetGlobalValue("SOUND_SEMANTIC_TRACKVIEW", eSoundSemantic_TrackView);
	pSS->SetGlobalValue("SOUND_SEMANTIC_PROJECTILE", eSoundSemantic_Projectile);
	pSS->SetGlobalValue("SOUND_SEMANTIC_VEHICLE", eSoundSemantic_Vehicle);
	pSS->SetGlobalValue("SOUND_SEMANTIC_WEAPON", eSoundSemantic_Weapon);
	pSS->SetGlobalValue("SOUND_SEMANTIC_EXPLOSION", eSoundSemantic_Explosion);
	pSS->SetGlobalValue("SOUND_SEMANTIC_PLAYER_FOLEY", eSoundSemantic_Player_Foley);
	pSS->SetGlobalValue("SOUND_SEMANTIC_ANIMATION", eSoundSemantic_Animation);

#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &ScriptBind_Sound::

	SCRIPT_REG_FUNC(AddToScaleGroup);
	SCRIPT_REG_FUNC(EndMusicTheme);
	SCRIPT_REG_FUNC(GetSoundLength);
	SCRIPT_REG_FUNC(GetSoundVolume);
	SCRIPT_REG_FUNC(IsPlaying);
	SCRIPT_REG_FUNC(Load3DSound);
	SCRIPT_REG_FUNC(LoadSound);
	SCRIPT_REG_FUNC(LoadStreamSound);
	SCRIPT_REG_FUNC(Play);
	SCRIPT_REG_FUNC(PlayEx);
	SCRIPT_REG_FUNC(PlayPattern);
	SCRIPT_REG_FUNC(PlaySound);
	SCRIPT_REG_FUNC(PlaySoundFadeUnderwater);
	SCRIPT_REG_FUNC(PlayStinger);
	SCRIPT_REG_FUNC(Precache);
	SCRIPT_REG_FUNC(RegisterReverbSound);
	SCRIPT_REG_FUNC(RegisterSoundMood);
	SCRIPT_REG_FUNC(RegisterWeightedEaxEnvironment);
	SCRIPT_REG_FUNC(SetDefaultMusicMood);
	SCRIPT_REG_FUNC(SetEaxEnvironment);
	SCRIPT_REG_FUNC(SetFadeTime);
	SCRIPT_REG_FUNC(SetGroupScale);
	SCRIPT_REG_FUNC(SetMasterVolumeScale);
	SCRIPT_REG_FUNC(SetMenuMusic);
	SCRIPT_REG_FUNC(SetMinMaxDistance);
	SCRIPT_REG_FUNC(SetMusicMood);
	SCRIPT_REG_FUNC(SetMusicTheme);
	SCRIPT_REG_FUNC(SetParameterValue);
	SCRIPT_REG_FUNC(SetSoundLoop);
	SCRIPT_REG_FUNC(SetSoundPaused);
	SCRIPT_REG_FUNC(SetSoundPosition);
	SCRIPT_REG_FUNC(SetSoundProperties);
	SCRIPT_REG_FUNC(SetSoundVolume);
	SCRIPT_REG_FUNC(SetWeatherAndPerceptionFlags);
	SCRIPT_REG_FUNC(Silence);
	SCRIPT_REG_FUNC(StopSound);
	SCRIPT_REG_FUNC(UnregisterSoundMood);
	SCRIPT_REG_FUNC(UnregisterWeightedEaxEnvironment);
	SCRIPT_REG_FUNC(UpdateSoundMood);
	SCRIPT_REG_FUNC(UpdateWeightedEaxEnvironment);
}

int ScriptBind_Sound::AddToScaleGroup(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::EndMusicTheme(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::GetSoundLength(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::GetSoundVolume(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::IsPlaying(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::Load3DSound(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::LoadSound(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::LoadStreamSound(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::Play(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::PlayEx(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::PlayPattern(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::PlaySound(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::PlaySoundFadeUnderwater(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::PlayStinger(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::Precache(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::RegisterReverbSound(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::RegisterSoundMood(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::RegisterWeightedEaxEnvironment(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::SetDefaultMusicMood(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::SetEaxEnvironment(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::SetFadeTime(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::SetGroupScale(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::SetMasterVolumeScale(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::SetMenuMusic(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::SetMinMaxDistance(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::SetMusicMood(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::SetMusicTheme(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::SetParameterValue(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::SetSoundLoop(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::SetSoundPaused(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::SetSoundPosition(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::SetSoundProperties(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::SetSoundVolume(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::SetWeatherAndPerceptionFlags(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::Silence(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::StopSound(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::UnregisterSoundMood(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::UnregisterWeightedEaxEnvironment(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::UpdateSoundMood(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}

int ScriptBind_Sound::UpdateWeightedEaxEnvironment(IFunctionHandler *pH)
{
	// TODO
	return pH->EndFunction();
}
