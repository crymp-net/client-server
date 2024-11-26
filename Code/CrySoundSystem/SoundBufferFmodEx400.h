////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   SoundBufferFmodEx400.h
//  Version:     v1.00
//  Created:     8/6/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: FMODEX 4.00 Implementation of SoundBuffer
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __SOUNDBUFFERFMODEX400_H__
#define __SOUNDBUFFERFMODEX400_H__

#pragma once

#include "SoundBuffer.h"
#include "FmodEx/inc/fmod.hpp"

class CSoundAssetManager;
struct IMicrophoneStream;

class CSoundBufferFmodEx400 : public CSoundBuffer
{
public:
	CSoundBufferFmodEx400(const SSoundBufferProps &Props, FMOD::System* pCSEX);

protected:
	~CSoundBufferFmodEx400();

	//////////////////////////////////////////////////////////////////////////
	// Inherited Method by IStreamCallBack
	//////////////////////////////////////////////////////////////////////////
	virtual void	StreamOnComplete(IReadStream *pStream, unsigned nError);

	//////////////////////////////////////////////////////////////////////////
	//	platform dependent calls
	//////////////////////////////////////////////////////////////////////////

	virtual uint32				GetMemoryUsed();
	virtual void					GetMemoryUsage(class ICrySizer* pSizer) const;		// compute memory-consumption, returns size in Bytes
	virtual void					LogDependencies()	{ }

	// Loads a file or memory block into memory of the used sound library
	// TODO change interface to (void* Data, void* DataPros) to make it very generic
	virtual tAssetHandle LoadAsSample(const char *AssetDataPtrOrName, int nLength);

	// Opens a file or memory block as a stream 
	virtual tAssetHandle LoadAsStream(const char *AssetName, int nLength);

	// Gets and Sets Parameter defined in the enumAssetParam list
	virtual bool	GetParam(enumAssetParamSemantics eSemantics, ptParam* pParam) const;
	virtual bool	SetParam(enumAssetParamSemantics eSemantics, ptParam* pParam);

	// Closes down Stream or frees memory of the Sample
	virtual bool	UnloadData(const eUnloadDataOptions UnloadOption);
private:

	FMOD::System* m_pCSEX;
	FMOD_RESULT		m_ExResult;
	void					FmodErrorOutput(const char * sDescription, ESoundLogType LogType = eSLT_Message);

};
#endif

