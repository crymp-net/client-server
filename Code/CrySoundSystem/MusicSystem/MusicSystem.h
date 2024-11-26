////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   MusicSystem.h
//  Version:     v1.00
//  Created:     26/7/2004 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __MusicSystem_h__
#define __MusicSystem_h__
#pragma once

#include "../StdAfx.h"

#include <vector>
#include "CryCommon/CrySoundSystem/IMusicSystem.h"
#include "CryCommon/CrySystem/ITimer.h"
#include <smartptr.h>
#include "Pattern/MusicPattern.h"
#include "RandGen/RandGen.h"
#include "../IPlatformSound.h" // tSoundHandle
#include "MusicCVars.h"

#ifndef __SPU__
#ifdef SOUNDSYSTEM_USE_FMODEX400
#include "../FmodEx/inc/fmod.hpp"
#include "../FmodEx/inc/fmod_event.hpp"
#endif
#endif
//#define TRACE_MUSIC

//#ifdef TRACE_MUSIC
//#define MTRACE TRACE
//#else
//#define MTRACE __noop
//#endif

#define PATTERNSET_CHANGE_FADETIME	2.0

// Forward declarations
struct ISystem;
struct IAudioDevice;



// Info-Structure for automated fading
struct SMusicPatternPlayInfo
{
	SMusicPatternPlayInfo()
	{
		nLayer						=	MUSICLAYER_MAIN;
		pPatternInstance	=	NULL;
		eBlendType				=	EBlend_None;
		ePlayingType			=	EPlaying_Loop;
		pRefCount					=	NULL;
		fPhase						=	1.0;
		fFadeTime					=	0.0;	// use default fade time...
		nSamplesToWait    = 0;
		bRefCountAdjusted	=	false;
	}
	~SMusicPatternPlayInfo()
	{
		pPatternInstance	=	NULL;
	}
	unsigned int				nLayer;
	TPatternInstancePtr pPatternInstance;
	EBlendingType				eBlendType;
	EPlayingType				ePlayingType;
	int const*					pRefCount;
	// internal
	double	fPhase;			// phase for fading (0-1)
	double	fFadeTime;	// time in seconds to fade
	int     nSamplesToWait;
	bool		bRefCountAdjusted;
};

// holds all required data from SMusicPatternPlayInfos for MixStreamsKernel
struct SMusicPatternPlayInfoHolder
{
	float					fFade;
	int						nSamplesToWait;	
	unsigned int	nSamples;
	unsigned int	pad16;			
} _ALIGN(16);

//////////////////////////////////////////////////////////////////////////
// A couple of typedefs for various stl-containers
//////////////////////////////////////////////////////////////////////////

typedef std::map<string,CMusicPattern*,stl::less_stricmp<string> >	TPatternMap;
typedef TPatternMap::iterator									TPatternMapIt;
typedef TPatternMap::const_iterator						TPatternMapItConst;

typedef std::vector<string>										TStringVec;
typedef TStringVec::iterator									TStringVecIt;
typedef TStringVec::const_iterator						TStringVecItConst;

typedef std::vector<SMusicPatternPlayInfo>		TPatternPlayInfoVec;
typedef TPatternPlayInfoVec::iterator					TPatternPlayInfoVecIt;
typedef TPatternPlayInfoVec::const_iterator		TPatternPlayInfoVecItConst;


//////////////////////////////////////////////////////////////////////////
// Structure to collect/handle mood-events
//////////////////////////////////////////////////////////////////////////

struct SMoodEventInfo
{
	bool operator<(const SMoodEventInfo &b) const
	{
		if (stricmp(sMood.c_str(), b.sMood.c_str()) < 0)
			return true;
		return false;
	}
	string	sMood;	// name of mood
	float		fTime;	// time of consecutive event-occurance
};

typedef std::set<SMoodEventInfo>	TMoodEventSet;
typedef TMoodEventSet::iterator		TMoodEventSetIt;

//////////////////////////////////////////////////////////////////////////
// The MusicSystem itself
//////////////////////////////////////////////////////////////////////////

class CMusicSystem : public IMusicSystem, public CMusicCVars
{
protected:

	// random generator
	CPseudoRandGen m_RandGen;

	// notification sink
	IMusicSystemSink *m_pSink;

	// sound-playback params
	float m_fMasterVolume;
	int		m_nSampleRate;
	float m_fLatency;
	
	// bytes/sample in output stream
	int m_nBytesPerSample;
	
	// temporary mix-buffer
	void *m_pPatternMixBuffer;
	void *m_pMixBuffer;

	//corresponding count to m_MixStreamsPlayInfos
	unsigned int m_MixStreamsPlayInfosCnt;						
	enum {EMixStreamsPlayInfoSize = 16};
	//temp array of mix stream Playinfo inputs for MixStreamsKernel to mix all at once
	SMusicPatternPlayInfoHolder m_MixStreamsPlayInfos[EMixStreamsPlayInfoSize] _ALIGN(128);		

	// fmod stream data
	tAssetHandle	m_pStream; 
	IAudioDevice	*m_pAudioDevice;
	void					*m_pChannel; 

#ifdef SOUNDSYSTEM_USE_FMODEX400
	FMOD_RESULT m_ExResult;
#endif

	// states
	bool m_bDataLoaded;
	bool m_bPause;
	bool m_bPlaying;
	bool m_bBridging;							// bridging in progress
	bool m_bCurrPatternIsBridge;
	bool m_bForcePatternChange;		// change pattern asap (next fadepoint), do not play to the end (last fadepoint)
	
	// status
	SMusicSystemStatus m_Status;
	
	// all patterns
	TPatternMap m_mapPatterns;
	
	// current main patterns
	
	TPatternInstancePtr m_pTrack1;
	int m_Track1Type;
	
	TPatternInstancePtr m_pTrack2;
	int m_Track2Type;

	TPatternInstancePtr m_pNextTrack;
	int m_NextTrackType;

	TPatternInstancesVec m_PatternInstances;
	TPatternInstancesVec m_PatternInstancesToBeRemoved;
	TPatternInstancesVec m_PatternInstancesRemovedFromStreamingThread;

	int m_nLayeredRhythmicPatterns;
	int m_nLayeredIncidentalPatterns;

	// array of currently playing patterns
	TPatternPlayInfoVec m_vecPlayingPatterns;
	
	// all themes (moods inside)
	TThemeMap m_mapThemes;
	
	// current theme/mood
	SMusicTheme *m_pCurrTheme;
	SMusicTheme *m_pNextTheme;					// in case a theme change occured, this is the new theme... NULL otherwise
	SMusicMood	*m_pCurrMood;
	SMusicMood	*m_pNextMood;

	// Timer
	CTimeValue m_MoodTime;
	CTimeValue m_ThemeTime;
	CTimeValue m_ThemeSetTime;
	int				 m_nDelayThemeInSec;

	CTimeValue m_tLastUpdate;
	
	// mood event arrays
	TMoodEventSet m_setMoodEvents;			// active mood events
	TMoodEventSet m_setFrameMoodEvents;	// mood events arrived last frame
	
	// default mood
	//SMusicMood *m_pDefaultMood;
	float		m_fDefaultMoodTime;					// current time 'till default-mood-timeout
	string	m_sDefaultMood;							// override of theme-specific default-mood (set by SetDefaultMood); set to "" to use theme-specific
	
	// fading
	double m_fCurrCrossfadeTime;
	
	// logging
	TStringVec	m_vecLog;
	bool				m_bEnableEventProcessing;
	int					m_musicEnable;

	XmlNodeRef m_musicStateNode;

private:

protected:
	virtual ~CMusicSystem();
	bool Init();
	void Shutdown();
	void FlushLog();
	bool StartPlaying();
	bool StopPlaying();
	void Silence();
	void FadeOutAllSecondaryLayers(double fFadeTime);

	//Pattern
	bool										FlushPatterns();
	void										PushPatternToMixList(SMusicPatternPlayInfo &PlayInfo);
	bool										UpdateCurrentPatternSet(SMusicMood *pMood, int nSamples, bool bAllowChange);
	bool										ChoosePatternSet(SMusicMood *pMood, const bool bUseSameIndex=false);
	CMusicPattern*					AddPattern(const char *pszName, const char *pszFilename);
	CMusicPatternInstance*	GetPatternInstance(const char *pszPattern);
	CMusicPatternInstance*	ChooseBridge(SMusicTheme *pCurrTheme, SMusicTheme *pNewTheme);
	CMusicPatternInstance*	ChoosePattern(SMusicMood *pMood, const int nLayer);

	// Mood
	void				EnterDefaultMood();
	bool				SetMood(SMusicMood *pNewMood, const bool bPlayFromStart=true, const bool bForceChange=true);
	SMusicMood* GetMood(SMusicTheme *pTheme, const char *pszMood) const;
	SMusicMood* GetDefaultMood(SMusicTheme *pTheme) const;

	// Theme
	bool SetTheme(SMusicTheme *pNewTheme, bool bForceChange);
	
	// Stream
	void MixStreams(void *const __restrict pBuffer, int nLength);
	bool FadeStream(SMusicPatternPlayInfo &PlayInfo, void *pBuffer, int nSamples);

	// Callbacks
	signed char StreamingCallback(tAssetHandle pStream, void *pBuffer, int nLength);

#if (defined SOUNDSYSTEM_USE_FMODEX400)
	//#pragma message (">>> Music-callback declaration:")
	friend FMOD_RESULT F_CALLBACK _StreamingCallback(FMOD_SOUND *sound, void *data, unsigned int datalen);
#endif

public:

	CMusicSystem(IAudioDevice *pAudioDevice);
	
	// System
	void							Release();
	void							ReleaseData();
	void							Unload();
	IMusicSystemSink* SetSink(IMusicSystemSink *pSink);
	bool							SetData( struct SMusicInfo::Data *pMusicData );
	void							Update();

	// Information
	int									GetBytesPerSample() { return m_nBytesPerSample; }
	SMusicSystemStatus* GetStatus();		// retrieve status of music-system... dont keep returning pointer !
	void								ClearStatus();	// removes current temp data

	//! compute memory-consumption
	void								GetMemoryUsage(class ICrySizer* pSizer) const;
	void								LogMsg( const int nVerbosity, const char *pszFormat, ... ) PRINTF_PARAMS(3, 4);
	virtual bool				IsNullImplementation() const { return false; }

  // Playback
	void Pause(bool bPause);
	void EnableEventProcessing(bool bEnable) { m_bEnableEventProcessing = bEnable; }

	// Theme
	virtual bool	SetTheme(const char *pszTheme, bool bForceChange=true, bool bKeepMood=true, int nDelayInSec=-1);
	virtual bool	EndTheme(EThemeFadeType ThemeFade=EThemeFade_FadeOut, int nForceEndLimitInSec=10, bool bEndEverything=false);
	const char*		GetTheme() const { if (m_pCurrTheme) return m_pCurrTheme->sName.c_str(); return ""; }
	IStringItVec* GetThemes() const;
	virtual CTimeValue GetThemeTime() const { return gEnv->pTimer->GetFrameStartTime() - m_ThemeTime; }

	// Mood
	bool					SetMood(const char *pszMood, const bool bPlayFromStart=true, const bool bForceChange=false);
	bool					SetDefaultMood(const char *pszMood);
	const char*		GetMood() const { if (m_pCurrMood) return m_pCurrMood->sName.c_str(); return ""; }	
	IStringItVec* GetMoods(const char *pszTheme) const;
	bool					AddMusicMoodEvent(const char *pszMood, float fTimeout);
	virtual CTimeValue GetMoodTime() const { return gEnv->pTimer->GetFrameStartTime() - m_MoodTime; };

	//////////////////////////////////////////////////////////////////////////
	//! Load music data from XML.
	//! @param bAddAdata if true data from XML will be added to currently loaded music data.
	bool LoadFromXML( const char *sFilename, bool bAddData, bool bReplaceData);

	//////////////////////////////////////////////////////////////////////////
	// Editing support.
	//////////////////////////////////////////////////////////////////////////
	virtual void UpdatePattern( SMusicInfo::Pattern *pPattern );
	virtual void RenamePattern( const char *sOldName,const char *sNewName );
	virtual void PlayPattern(		const char *sPattern, bool bStopPrevious, bool bPlaySynched );
	virtual void DeletePattern( const char *sPattern );

	virtual void PlayStinger();

	// writes output to screen in debug
	virtual void DrawInformation(IRenderer* pRenderer, float xpos, float ypos); 

	// for serialization
	virtual void Serialize(TSerialize ser);
	virtual bool SerializeInternal(bool bSave);

private:
	//////////////////////////////////////////////////////////////////////////
	// Loading.
	//////////////////////////////////////////////////////////////////////////
	void LoadPatternFromXML(	XmlNodeRef node,	SPatternDef *pPattern );
	void LoadMoodFromXML(			XmlNodeRef &node,	SMusicMood *pMood );
	void LoadThemeFromXML(		XmlNodeRef &node,	SMusicTheme *pTheme );
	void UpdatePatternDef( SPatternDef *pPattern );
	
	SMusicPatternPlayInfo* GetPlayingPatternInfo( const int nLayer, const EBlendingType BlendType );
	bool IsPatternPlaying( const TPatternInstancePtr pPattern );

	bool PlayStart(EBlendingType eBlendType);

	//helper routine for MixStreams
	void MixStreamsStreamEnds(SMusicPatternPlayInfo & PlayInfo);
	//need all args to bypass cache entirely on spu
	void MixStreamsKernel(void *const __restrict pBuffer, int nSamplesToCopy, unsigned int mixStreamsPlayInfosCnt, void *m_pPatternMixBuffer);
	void PushBackPlayInfo(const SMusicPatternPlayInfo & PlayInfo, int nSamples)
	{
		assert(m_MixStreamsPlayInfosCnt < EMixStreamsPlayInfoSize);
		if (m_MixStreamsPlayInfosCnt < EMixStreamsPlayInfoSize)
		{
			SMusicPatternPlayInfoHolder &curPlayInfo = m_MixStreamsPlayInfos[m_MixStreamsPlayInfosCnt++];
			curPlayInfo.nSamples				= nSamples;
			curPlayInfo.nSamplesToWait	= PlayInfo.nSamplesToWait;
			curPlayInfo.fFade						= 1.0f;
			if (PlayInfo.pPatternInstance->GetPattern())
				curPlayInfo.fFade = PlayInfo.pPatternInstance->GetPattern()->GetLayeringVolume();
			else
			{
				assert(0);
			}
		}
	}
};

//////////////////////////////////////////////////////////////////////////
// String-vector-class to iterate data from other DLLs
//////////////////////////////////////////////////////////////////////////

class CStringItVec : public IStringItVec
{
public:
	CStringItVec(TStringVec &Vec)
	{
		m_nRefCount=0;
		m_Vec=Vec;
		MoveFirst();
	};
	bool IsEnd() { return (m_itVec==m_Vec.end()); };
	const char* Next() { return IsEnd() ? "" : (*m_itVec++).c_str(); };
	void MoveFirst() { m_itVec = m_Vec.begin(); };
	void AddRef() { m_nRefCount++; }
	void Release() { --m_nRefCount; if (m_nRefCount<=0) { delete this; } };
protected:
	int m_nRefCount;
	TStringVec m_Vec;
	TStringVecIt m_itVec;
};

#endif // __MusicSystem_h__
