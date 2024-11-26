////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   PlatformSoundFmodEx400.h
//  Version:     v1.00
//  Created:     8/6/2005 by Tomas
//  Compilers:   Visual Studio.NET
//  Description: FmodEx 4.00 Implementation of a platform dependent Sound
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __PLATFORMSOUNDFMODEX400_H__
#define __PLATFORMSOUNDFMODEX400_H__

#include "IPlatformSound.h"
#include "FmodEx/inc/fmod.hpp"

#pragma once

class CAudioDeviceFmodEx400;

class CPlatformSoundFmodEx400 : public IPlatformSound
{
public:
	CPlatformSoundFmodEx400(CSound*	pSound, FMOD::System* pCSEX);
	~CPlatformSoundFmodEx400(void);

	//////////////////////////////////////////////////////////////////////////
	// Management
	//////////////////////////////////////////////////////////////////////////
	virtual void	SetSoundPtr(CSound* pSound) {m_pSound = pSound; }
	virtual int32	AddRef();
	virtual int32	Release();
	virtual enumPlatformSoundErrorCodes	CreateSound(tAssetHandle AssetHandle, SSoundSettings SoundSettings);
	virtual bool	PlaySound(bool bStartPaused);	// should or could be done through SetParam PlayModes
	virtual bool	StopSound();	// should or could be done through SetParam PlayModes
	virtual bool	FreeSoundHandle();
	virtual bool	Set3DPosition(Vec3* pvPosition, Vec3* pvVelocity, Vec3* pvOrientation);
	virtual void  SetObstruction(const SObstruction *pObstruction) {};
	virtual void  SetPlatformSoundName( const char * sPlatformSoundName) {};

	virtual enumPlatformSoundStates  GetState() { return m_State; }

	//////////////////////////////////////////////////////////////////////////
	// Information
	//////////////////////////////////////////////////////////////////////////

	virtual enumPlatformSoundClass GetClass() const { return pscSOUND; }
	virtual void						Reset(CSound*	pSound, const char *sEventName);

	// Gets and Sets Parameter defined in the enumAssetParam list
	virtual bool						GetParamByType(enumPlatformSoundParamSemantics eSemantics, ptParam* pParam);
	virtual bool						SetParamByType(enumPlatformSoundParamSemantics eSemantics, ptParam* pParam);

	// Gets and Sets Parameter defined by string and float value, returns the index of that parameter
	virtual int							GetParamByName(const char *sParameter, float *fValue, bool bOutputWarning=true) {return (-1);}
	virtual int							SetParamByName(const char *sParameter, float fValue, bool bOutputWarning=true) {return (-1);}

	// Gets and Sets Parameter defined by index and float value
	virtual int							GetEventIndexByNameIndex(const int nNameIndex) { return -1; }
	virtual bool						GetParamByIndex(int nEventIndex, float* const pfValue, float* const pfRangeMin=NULL, float* const pfRangeMax=NULL, char const** const ppcParameterName=NULL, bool const bOutputWarning=true) {return false;}
	virtual bool						SetParamByIndex(int nEventIndex, float fValue, bool bOutputWarning=true)  {return (false);}

	virtual tSoundHandle		GetSoundHandle() const;	

	virtual bool						IsInCategory(const char* sCategory) { return false; }

	// Memory
	virtual void						GetMemoryUsage(ICrySizer* pSizer) const;

private:
	CSound*									m_pSound;			// Ptr to the sound this implementation belongs to
	int32										m_nRefCount;	// Refcounting, although it does not really make sense, but who knows

	enumPlatformSoundStates m_State;

	//int			m_nChannel;		// the actual FMOD Sound Handle is a channel
	FMOD::System*						m_pCSEX;
	FMOD::Channel*					m_pExChannel;
	FMOD_RESULT							m_ExResult;
	CAudioDeviceFmodEx400*	m_pAudioDevice;

	void										FmodErrorOutput(const char * sDescription, ESoundLogType LogType = eSLT_Message);
	static FMOD_RESULT F_CALLBACK	OnCallBack(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, int command, unsigned int commanddata1, unsigned int commanddata2);

};
#endif
