////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   SoundBufferFmodEx400Network.h
//  Version:     v1.00
//  Created:     06/12/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: FMODEX 4.00 Implementation of SoundBuffer that is fed my network
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __SOUNDBUFFERFMODEX400NETWORK_H__
#define __SOUNDBUFFERFMODEX400NETWORK_H__

#pragma once

#include "SoundBuffer.h"
#include "FmodEx/inc/fmod.hpp"

class CSoundAssetManager;

class CSoundBufferFmodEx400Network : public CSoundBuffer
{
public:
	CSoundBufferFmodEx400Network(const SSoundBufferProps &Props, FMOD::System* pCSEX);
	INetworkSoundListener* GetINetworkStreamListener() { return m_pINetworkSoundListener; }

protected:
	~CSoundBufferFmodEx400Network();

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

	// loads as network stream
	virtual tAssetHandle LoadAsNetworkStream(INetworkSoundListener *pINetworkSoundListener);

	// Gets and Sets Parameter defined in the enumAssetParam list
	virtual bool	GetParam(enumAssetParamSemantics eSemantics, ptParam* pParam) const;
	virtual bool	SetParam(enumAssetParamSemantics eSemantics, ptParam* pParam);

	// Closes down Stream or frees memory of the Sample
	virtual bool	UnloadData(const eUnloadDataOptions UnloadOption);
private:

	FMOD::System					*m_pCSEX;
	FMOD_RESULT						m_ExResult;
	INetworkSoundListener *m_pINetworkSoundListener;
	unsigned int					m_nLastRecordPos;
	void*									m_TempBuffer;

	void									FmodErrorOutput(const char * sDescription, ESoundLogType LogType = eSLT_Message);


};
#endif

