////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   PlatformSoundFmodEx400Event.h
//  Version:     v1.00
//  Created:     28/7/2005 by Tomas
//  Compilers:   Visual Studio.NET
//  Description: FmodEx 4.00 Implementation of a platform dependent Sound Event
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __PLATFORMSOUNDFMODEX400EVENT_H__
#define __PLATFORMSOUNDFMODEX400EVENT_H__

#include "IPlatformSound.h"
#include "FmodEx/inc/fmod.hpp"
#include "FmodEx/inc/fmod_event.hpp"

#pragma once

class CAudioDeviceFmodEx400;

typedef struct SEventParameter 
{
	SEventParameter()
		:	nEventIndex(-1),
			fRangeMin(0.0f),
			fRangeMax(1.0f)
	{
	}

	int nEventIndex;
	float fRangeMin;
	float fRangeMax;

	void GetMemoryUsage(ICrySizer *pSizer ) const{}
}SEventParameter;

typedef struct SQueuedParameter
{
	public:
	int nNameIndex;
	enumPlatformSoundParamSemantics eSemantic;
	int nEventIndex;
	float fValue;
	int nValue;

	// Constructor
	SQueuedParameter() 
	{
		nNameIndex = -1;
		eSemantic	= pspNONE;
		nEventIndex	= -1;
		fValue = 0.0f;
		nValue = 0;
	}

	void GetMemoryUsage(ICrySizer *pSizer ) const{}
}SQueuedParameter;

typedef std::vector<SQueuedParameter>							tvecQueuedParameter;
typedef tvecQueuedParameter::iterator							tvecQueuedParameterIter;
typedef tvecQueuedParameter::const_iterator				tvecQueuedParameterIterConst;

typedef std::vector<SEventParameter>							tvecLocalParameterIndex;
typedef tvecLocalParameterIndex::iterator					tvecLocalParameterIndexIter;
typedef tvecLocalParameterIndex::const_iterator		tvecLocalParameterIndexIterConst;

class CPlatformSoundFmodEx400Event : public IPlatformSound
{
public:
	CPlatformSoundFmodEx400Event(CSound*	pSound, FMOD::System* pCSEX, const char *sEventName);
	~CPlatformSoundFmodEx400Event(void);

	//////////////////////////////////////////////////////////////////////////
	// Management
	//////////////////////////////////////////////////////////////////////////
	virtual void						SetSoundPtr(CSound* pSound) {m_pSound = pSound; }
	virtual int32						AddRef();
	virtual int32						Release();
	virtual enumPlatformSoundErrorCodes	CreateSound(tAssetHandle AssetHandle, SSoundSettings SoundSettings);
	virtual bool						PlaySound(bool bStartPaused);	// should or could be done through SetParam PlayModes
	virtual bool						StopSound();	// should or could be done through SetParam PlayModes
	virtual bool						FreeSoundHandle();
	virtual bool						Set3DPosition(Vec3* pvPosition, Vec3* pvVelocity, Vec3* pvOrientation);
	virtual void  					SetObstruction(const SObstruction *pObstruction);
	virtual void  					SetPlatformSoundName( const char * sPlatformSoundName) { m_sEventName = sPlatformSoundName; };

	virtual enumPlatformSoundStates	GetState() { return m_State; }

	//////////////////////////////////////////////////////////////////////////
	// Information
	//////////////////////////////////////////////////////////////////////////

	virtual enumPlatformSoundClass	GetClass() const { return pscEVENT; }
	virtual void						Reset(CSound*	pSound, const char *sEventName);

	// Gets and Sets Parameter defined in the enumAssetParam list
	virtual bool						GetParamByType(enumPlatformSoundParamSemantics eSemantics, ptParam* pParam);
	virtual bool						SetParamByType(enumPlatformSoundParamSemantics eSemantics, ptParam* pParam);

	// Gets and Sets Parameter defined by string and float value, returns the index of that parameter
	virtual int							GetParamByName(const char *sParameter, float *fValue, bool bOutputWarning=true);
	virtual int 						SetParamByName(const char *sParameter, float fValue, bool bOutputWarning=true);

	// Gets and Sets Parameter defined by index and float value
	virtual int							GetEventIndexByNameIndex(int const nNameIndex) { return m_ParameterIndex[nNameIndex].nEventIndex; }
	virtual bool						GetParamByIndex(int nEventIndex, float* const pfValue, float* const pfRangeMin=NULL, float* const pfRangeMax=NULL, char const** const ppcParameterName=NULL, bool const bOutputWarning=true);
	virtual bool						SetParamByIndex(int nEventIndex, float fValue, bool bOutputWarning=true);  

	virtual tSoundHandle		GetSoundHandle() const;	

	virtual bool						IsInCategory(const char* sCategory);

	// Memory
	virtual void						GetMemoryUsage(ICrySizer* pSizer) const;

private:
	
	int32										m_nRefCount;	// Ref-Counting, although it does not really make sense, but who knows

	FMOD::Event*						m_pEvent;
	enumPlatformSoundStates m_State;

	mutable FMOD_RESULT			m_ExResult;
	float										m_fMaxRadius;
	float										m_fMaxVolume;
	
	int32										m_nLenghtInMs;
	int											m_nCurrentSpawns;
	int											m_nMaxSpawns;
	int											m_nDistanceIndex;

	string									m_sEventName;
	
	FMOD::System*						m_pCSEX;
	CAudioDeviceFmodEx400*	m_pAudioDevice;
	CSound*									m_pSound;			// Ptr to the sound this implementation belongs to

	tvecQueuedParameter			m_QueuedParameter;

	tvecLocalParameterIndex m_ParameterIndex;

	//FMOD_REVERB_CHANNELPROPERTIES m_ReverbProps; //TODO evaluate and set event specific reverb props

	void										FmodErrorOutput(const char * sDescription, ESoundLogType LogType = eSLT_Message);
	static FMOD_RESULT F_CALLBACK  OnCallBack(FMOD_EVENT *  event, FMOD_EVENT_CALLBACKTYPE  type, void *  param1, 	void *  param2, void *  userdata);

	void										UpdateFMODState();
	int											GetNameIndexByEventIndex(int const nEventIndex);
};
#endif
