#ifndef CRYSOUND_MUSICSYSTEM_PCMDECODER_H
#define CRYSOUND_MUSICSYSTEM_PCMDECODER_H

#pragma once

#include "platform.h"

#include "PatternDecoder.h"

struct IMusicSystem;
class CPCMDecoderInstance;
struct ICryPak;

struct SWaveHdr
{
	char RIFF[4];					// "RIFF" tag
	uint32 dwSize;				// Size of data to follow
	char WAVE[4];					// "WAVE" tag
	char fmt_[4];					// "fmt " tag
	uint32 dw16;					// 16
	uint16 wOne_0;				// 1
	uint16 wChnls;				// Number of Channels
	uint32 dwSRate;				// Sample Rate
	uint32 BytesPerSec;		// Bytes per second
	uint16 wBlkAlign;			// Block align
	uint16 BitsPerSample; // Sample size
	char DATA[4];					// "DATA"
	uint32 dwDSize;				// Number of Samples

	AUTO_STRUCT_INFO_LOCAL
};




struct SPCMFileInfo 
{
	SPCMFileInfo()
	{
		nBytesPerSample=0;
	}
	int nBytesPerSample;
};

class CPCMDecoder :	public IMusicPatternDecoder
{
protected:
	IMusicSystem *m_pMusicSystem;
	ICryPak *m_pPak;
	FILE *m_pFile;
	int m_nPosBytes;	// current position (in bytes) in the file to reduce seeking-overhead
	SMusicPatternFileInfo m_FileInfo;
	SPCMFileInfo m_PCMFileInfo;
	friend class CPCMDecoderInstance;
private:
	bool m_b44KHz;	//keeps track of encoded frequency

	int ReadFile(void *pBuf, int nSize, int nCount, int *pNewFilePos=NULL);
	bool InitStreamWAV();
protected:
	virtual ~CPCMDecoder();
public:
	CPCMDecoder(IMusicSystem *pMusicSystem, CMusicPattern *pMusicPattern); 
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
	//! retrieves frequency encoding
	const bool Is44KHz() const {return m_b44KHz;}
};

class CPCMDecoderInstance : public IMusicPatternDecoderInstance
{
	static const unsigned int scuiEncodedBlockSize = 2048;	//block size for static allocation

protected:
	CPCMDecoder *m_pDecoder;
	int			m_nPos;					// position in samples
	int			m_nPosBytes;			// file position in bytes
	bool		m_bCopyFromLastFrame;	// indicates some copying from last frame (due to odd sample count request)
	signed long m_lLastSample;			// sample to reuse for next frame 
private:
	bool SeekBytes(int nBytes);
protected:
	unsigned int   m_aEncodedBlock[scuiEncodedBlockSize];			//static allocated encoded block, only needed for 22 kHz mode
	//! fills a dest buffer with uncompressed data
	const bool FillPCMBuffer22KHz(signed long *pBuffer, int nSamples);

	virtual ~CPCMDecoderInstance();
public:
	CPCMDecoderInstance(CPCMDecoder *pDecoder);

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