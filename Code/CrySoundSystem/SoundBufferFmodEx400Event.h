////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   SoundBufferFmodEx400Event.h
//  Version:     v1.00
//  Created:     28/07/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: FMODEX 4.00 Implementation of Event SoundBuffer 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __SOUNDBUFFERFMODEX400EVENT_H__
#define __SOUNDBUFFERFMODEX400EVENT_H__

#pragma once

#include "SoundBuffer.h"
#include "FmodEx/inc/fmod.hpp"

class CSoundAssetManager;
struct IMicrophoneStream;

class CSoundBufferFmodEx400Event : public CSoundBuffer
{
public:
	CSoundBufferFmodEx400Event(const SSoundBufferProps &Props, FMOD::System* pCSEX);

protected:
	~CSoundBufferFmodEx400Event();

	//////////////////////////////////////////////////////////////////////////
	// Inherited Method by IStreamCallBack
	//////////////////////////////////////////////////////////////////////////
	virtual void	StreamOnComplete(IReadStream *pStream, unsigned nError) {};

	//////////////////////////////////////////////////////////////////////////
	//	platform dependent calls
	//////////////////////////////////////////////////////////////////////////

	virtual uint32				GetMemoryUsed();
	virtual void					GetMemoryUsage(class ICrySizer* pSizer) const;		// compute memory-consumption, returns size in Bytes
	virtual void					LogDependencies();

	//////////////////////////////////////////////////////////////////////////
	// Information
	//////////////////////////////////////////////////////////////////////////
	virtual bool NotLoaded() const;
	virtual bool Loaded() const;
	virtual bool Loading() const;
	virtual bool LoadFailure() const;

	// loads a event sound
	virtual tAssetHandle LoadAsEvent(const char *AssetName);

	// Gets and Sets Parameter defined in the enumAssetParam list
	virtual bool	GetParam(enumAssetParamSemantics eSemantics, ptParam* pParam) const;
	virtual bool	SetParam(enumAssetParamSemantics eSemantics, ptParam* pParam);

	// Closes down Stream or frees memory of the Sample
	virtual bool	UnloadData(const eUnloadDataOptions UnloadOption);
private:

	FMOD::System* m_pCSEX;
	FMOD_RESULT		m_ExResult;
	string m_sGroupName;
	void					FmodErrorOutput(const char * sDescription, ESoundLogType LogType = eSLT_Message);

	std::vector<CTimeValue> m_EventTimeouts;

};
#endif

