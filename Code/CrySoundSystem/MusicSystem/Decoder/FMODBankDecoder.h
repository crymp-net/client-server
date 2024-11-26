//////////////////////////////////////////////////////////////////////
//
//  Crytek (C) 2001
//
//  CrySound Source Code
//
//  File: FMODBankDecoder.h
//  Description: FSB Decoder for music
//
//  IMPORTANT: You can only have 1 file in each FSB for now ! Index 0 will be used.
//
//  History:
//	-June 06,2001:Implemented by Marco Corbetta
//  -June 01,2005:Updated by NickV added code for group manager and xml reading routines
//
//////////////////////////////////////////////////////////////////////

#ifndef CRYSOUND_MUSICSYSTEM_FMODBANKDECODER_H
#define CRYSOUND_MUSICSYSTEM_FMODBANKDECODER_H

#pragma once

#include "platform.h"

#include <IStreamEngine.h>

#include "../../FmodEx/inc/fmod.hpp"

#include "PatternDecoder.h"


struct IMusicSystem;
class CFMODBankDecoderInstance;
class CMusicSystem;
struct ICryPak;

class CFMODBankDecoder :	public IStreamCallback, IMusicPatternDecoder
{
protected:
	int m_nPosInSamples;	// current position (in bytes) in the file to reduce seeking-overhead

	SMusicPatternFileInfo m_FileInfo;
	FMOD::Sound *m_BankSound;
	FMOD::Sound *m_SubSound;

	uint32 m_nMainThreadId;

	IReadStreamPtr			m_pReadStream;				// used Stream Ptr of StreamingEngine
	void*								m_BufferData;					// wavebank file in memory
	unsigned int				m_BufferLength;				// length of wavebank file in bytes
	CMusicSystem*				m_pMusicSystem;

	int						m_nSubSoundIndex;
	friend class CFMODBankDecoderInstance;
private:

	int ReadFile(void *pBuf, int nSize, int nCount, int *pNewFilePos=NULL);

protected:
	virtual ~CFMODBankDecoder();

		//////////////////////////////////////////////////////////////////////////
	// Inherited Method by IStreamCallBack
	//////////////////////////////////////////////////////////////////////////
	virtual void	StreamOnComplete(IReadStream *pStream, unsigned nError);

public:
	CFMODBankDecoder(IMusicSystem *pMusicSystem, CMusicPattern *pMusicPattern); 
	void Release() { delete this; }
	//! Open a music-pattern-file.
	bool Open(const char *sBankFileName, const char *sSoundName);
	//! Close a music-pattern-file.
	bool Close();
	//! Retrieve file-info
	bool GetFileInfo(SMusicPatternFileInfo &FileInfo);
	//! Retrieve mem-info
	void GetMemoryUsage(class ICrySizer* pSizer);
	//! Create instance.
	IMusicPatternDecoderInstance* CreateInstance();
	//! Update function that may only be called from the main thread
	virtual void MainThreadUpdate(uint32 nMainThreadId);
};

class CFMODBankDecoderInstance : public IMusicPatternDecoderInstance
{
	static const unsigned int scuiEncodedBlockSize = 2048;	//block size for static allocation

protected:
	CFMODBankDecoder *m_pDecoder;
	int			m_nPosInSamples;				// position in samples
	int			m_nStreamPosInSamples;	// position in samples in the stream

	bool		m_bCopyFromLastFrame;	// indicates some copying from last frame (due to odd sample count request)
	signed long m_lLastSample;		// sample to reuse for next frame 

	FMOD::Sound *m_BankSound;
	FMOD::Sound *m_SubStream;

	uint32 m_nMainThreadId;

	IReadStreamPtr			m_pReadStream;				// used Stream Ptr of StreamingEngine

	CMusicSystem*				m_pMusicSystem;

private:
	//bool SeekBytes(int nBytes);

protected:
	unsigned int   m_aEncodedBlock[scuiEncodedBlockSize];			//static allocated encoded block, only needed for 22 kHz mode

	virtual ~CFMODBankDecoderInstance();
public:
	CFMODBankDecoderInstance(CFMODBankDecoder *pDecoder);
	
	virtual void Init();

	virtual bool Ready() { return (m_SubStream != NULL); };

	virtual void Release() { delete this; }
	
	//! Seek to beginning of pattern (if nDelay is set it will wait nDelay-samples before starting playback).
	virtual bool Seek0(const int nDelayInSamples=0);
	
	//! Seek to pos in number of samples of pattern.
	virtual bool SeekPos(const int nSamplePos);
	
	//! Retrieve the current position in the file (in samples)
	virtual int GetPos();
	
	//! Decode and retrieve pcm-data (stereo/16bit).
	virtual bool GetPCMData(signed long *pDataOut, int nSamples, bool bLoop=true);

	//! Retrieve mem-info
	void GetMemoryUsage(class ICrySizer* pSizer);
};

#endif