////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   SoundBufferFmodEx400Micro.h
//  Version:     v1.00
//  Created:     30/11/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: FMODEX 4.00 Implementation of SoundBuffer Micro
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __SOUNDBUFFERFMODEX400MICRO_H__
#define __SOUNDBUFFERFMODEX400MICRO_H__

#pragma once

#include "SoundBuffer.h"
#include "FmodEx/inc/fmod.hpp"
#include "CryThread.h"

class CSoundAssetManager;
struct IMicrophoneStream;

class CSoundBufferFmodEx400Micro : public CSoundBuffer
{
public:
	CSoundBufferFmodEx400Micro(const SSoundBufferProps &Props, FMOD::System* pCSEX, int nRecordDriver);
	//IMicrophoneStream* GetIMicrophoneStream() { return m_pMicrophoneStream; }

protected:
	~CSoundBufferFmodEx400Micro();

	//////////////////////////////////////////////////////////////////////////
	// Inherited Method by IStreamCallBack
	//////////////////////////////////////////////////////////////////////////
	virtual void	StreamOnComplete(IReadStream *pStream, unsigned nError);

	//////////////////////////////////////////////////////////////////////////
	//	platform dependent calls
	//////////////////////////////////////////////////////////////////////////

	virtual uint32				GetMemoryUsed() {return 0;}
	virtual void					GetMemoryUsage(class ICrySizer* pSizer) const;		// compute memory-consumption, returns size in Bytes
	virtual void					LogDependencies()	{ }

	// loads as microphone stream
	virtual tAssetHandle LoadAsMicro(IMicrophoneStream *pIMicrophoneStream);

	virtual void Update();

	// Gets and Sets Parameter defined in the enumAssetParam list
	virtual bool	GetParam(enumAssetParamSemantics eSemantics, ptParam* pParam) const;
	virtual bool	SetParam(enumAssetParamSemantics eSemantics, ptParam* pParam);

	// Closes down Stream or frees memory of the Sample
	virtual bool	UnloadData(const eUnloadDataOptions UnloadOption);
private:

	FMOD::System*			m_pCSEX;
	FMOD_RESULT				m_ExResult;
	IMicrophoneStream*		m_pMicrophoneStream;
	unsigned int			m_nLastRecordPos;
	CryCriticalSection	MicroLock;
	int									m_nRecordDriver;

	void					FmodErrorOutput(const char * sDescription, ESoundLogType LogType = eSLT_Message);

};
#endif

