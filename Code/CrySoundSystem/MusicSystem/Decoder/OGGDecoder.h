#ifndef CRYSOUND_MUSICSYSTEM_OGGDECODER_H
#define CRYSOUND_MUSICSYSTEM_OGGDECODER_H

#pragma once

#include "platform.h"

#if (defined(_WIN32) || defined(_WIN64)) && !defined(XENON) && !defined(PS3)// no PS3 and Xenon for now

#include "PatternDecoder.h"
#include "../../oggvorbis/inc/vorbis/vorbisfile.h"

struct IMusicSystem;
class COGGDecoderInstance;

struct SFileAccess
{
	SFileAccess()
	{
		pPak=NULL;
		pFile=NULL;
	}
	SFileAccess(ICryPak *_pPak, FILE *_pFile)
	{
		pPak=_pPak;
		pFile=_pFile;
	}
	ICryPak *pPak;
	FILE *pFile;
};

class COGGDecoder :	public IMusicPatternDecoder
{
protected:
	IMusicSystem *m_pMusicSystem;
	SFileAccess m_FileAccess;
	SMusicPatternFileInfo m_FileInfo;
	friend class COGGDecoderInstance;
protected:
	virtual ~COGGDecoder();
public:
	COGGDecoder(IMusicSystem *pMusicSystem, CMusicPattern *pMusicPattern);
	void Release() { delete this; }
	//! Open a music-pattern-file.
	bool Open(const char *sFileName, const char *sSoundName);
	//! Close a music-pattern-file.
	bool Close();
	//! Retrieve file-info
	bool GetFileInfo(SMusicPatternFileInfo &FileInfo);
	//! Retrieve mem-info
	void GetMemoryUsage(class ICrySizer* pSizer);
	//! Create instance.
	IMusicPatternDecoderInstance* CreateInstance();
	//! Update function that may only be called from the main thread
	virtual void MainThreadUpdate(uint32 nMainThreadId) {};
};

class COGGDecoderInstance : public IMusicPatternDecoderInstance
{
protected:
	COGGDecoder *m_pDecoder;
	int m_nPos;	// position in samples
	int m_nPosBytes;
	OggVorbis_File m_OVFile;
private:
	bool FillPCMBuffer(signed long *pBuffer, int nSamples);
public:
	static size_t ReadFile(void *ptr, size_t size, size_t nmemb, void *datasource);
	static int SeekFile(void *datasource, ogg_int64_t offset, int whence);
	static int CloseFile(void *datasource);
	static int TellFile(void *datasource);
protected:
	virtual ~COGGDecoderInstance();
public:
	COGGDecoderInstance(COGGDecoder *pDecoder);
	
	virtual void Init() {};

	virtual bool Ready() { return true; };

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

#endif