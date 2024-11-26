#pragma once

//forward declaration
class CMusicPattern;

struct SMusicPatternFileInfo
{
	SMusicPatternFileInfo()
	{
		sFileName = "";
		sSoundName = "";
		nHeaderSize = 0;
		nSamples = 0;
		nRawBytes = 0;
		nChannels = 0;
		nBitsPerSample = 0;
		fFrequency = 0.0f;
		pMusicPattern = 0;
	}
	string sFileName;
	string sSoundName;
	int nHeaderSize;
	unsigned int nSamples;
	unsigned int nRawBytes;
	int nChannels;
	int nBitsPerSample;
	float fFrequency;
	CMusicPattern* pMusicPattern;
};

enum EWaveFormatIDs
{
	WAVE_FORMAT_ID_PCM					= 0x0001, 
	WAVE_FORMAT_ID_ADPCM				= 0x0002,
};

struct IMusicPatternDecoderInstance
{
	virtual void Init() = 0;

	virtual bool Ready() = 0;

	virtual void Release() = 0;

	//! Seek to beginning of pattern (if nDelay is set it will wait nDelay-samples before starting playback).
	virtual bool Seek0(const int nDelayInSamples=0) = 0;
	
	//! Seek to pos in number of samples of pattern.
	virtual bool SeekPos(const int nSamplePos) = 0;
	
	//! Retrieve the current position in the file (in samples).
	virtual int GetPos() = 0;
	
	//! Decode and retrieve pcm-data (stereo/16bit).
	virtual bool GetPCMData(signed long *pDataOut, int nSamples, bool bLoop=true)=0;

	//! Retrieve mem-info.
	virtual void GetMemoryUsage(class ICrySizer* pSizer) = 0;
};

struct IMusicPatternDecoder
{
	//! Destroys object.
	virtual void Release() = 0;
	//! Open a music-pattern-file.
	virtual bool Open(const char *sFileName, const char *sSoundName) = 0;
	//! Close a music-pattern-file.
	virtual bool Close() = 0;
	//! Retrieve file-info.
	virtual bool GetFileInfo(SMusicPatternFileInfo &FileInfo) = 0;
	//! Retrieve mem-info.
	virtual void GetMemoryUsage(class ICrySizer* pSizer) = 0;
	//! Create instance.
	virtual IMusicPatternDecoderInstance* CreateInstance() = 0;
	//! Update function that may only be called from the main thread
	virtual void MainThreadUpdate(uint32 nMainThreadId) = 0;
};