////////////////////////////////////////////////////////////////////////////
//
//  CryEngine Source File.
//  Copyright (C), Crytek, 1999-2010.
//
// -------------------------------------------------------------------------
//  File name:   SoundCVars.h
//  Version:     v1.00
//  Created:     19/10/2010 by Thomas Wollenzin
//  Description: This is the implementation of the CSoundCVars class
//
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_CVARS_H_INCLUDED
#define SOUND_CVARS_H_INCLUDED

//////////////////////////////////////////////////////////////////////////
// Console variables local to SoundSystem.
//////////////////////////////////////////////////////////////////////////
class CSoundCVars
{
public:

	CSoundCVars();
	~CSoundCVars();

	void    RegisterVariables();
	void    UnregisterVariables();

	int     g_nFileCacheManagerSize;
	int			g_nAllowNotCachedAccess;
	int     g_nAllowSoundBankPatching;
	int     g_nAllowMusicFileCaching;
	int     g_nAllowStreamFileCaching;
	int			g_nGameReverbManagerPause;
	int			g_nNumLoadingThreadsToUse;
	int     g_nEffectsAffectReverb;
	int     g_nDistanceSphereSemantic;
	int			g_autoMpSoundNameFixup;
	int     g_nReverbDebugDraw;
	int     g_nReverbType;
	int     g_nReverbEchoDSP;

	float   g_fLoadRequestsDisplayTime;
	float   g_fGameMasterVolume;
	float   g_fNoFocusVolume;
	float   g_fReverbDynamic;
	float   g_fReverbReflectionDelay;
	float   g_fReverbDelay;
	float   g_fSFXVolume;
	float   g_fGameSFXVolume;
	float   g_fMusicVolume;
	float   g_fGameMusicVolume;
	float   g_fDialogVolume;
	float   g_fGameDialogVolume;

	ICVar*	g_sAudioPreloadsFile;

#ifdef INCLUDE_AUDIO_PRODUCTION_CODE
	int     g_nFileCacheManagerDebugFilter;
	int			g_nFileCacheManagerDebugLog;



#endif // INCLUDE_AUDIO_PRODUCTION_CODE

private:

	CSoundCVars(CSoundCVars const&);						// Copy protection
	CSoundCVars& operator=(CSoundCVars const&);	// Copy protection
};

extern CSoundCVars g_SoundCVars;

#endif // SOUND_CVARS_H_INCLUDED
