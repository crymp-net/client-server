////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   AudioDeviceFmodEx400.h
//  Version:     v1.00
//  Created:     8/6/2005 by Tomas
//  Compilers:   Visual Studio.NET
//  Description: FmodEx 4.00 Implementation of a Audio Device
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __AUDIODEVICEFMODEX400_H__
#define __AUDIODEVICEFMODEX400_H__

#include "ISound.h"
#include "SoundSystemCommon.h"
#include "IAudioDevice.h"
#include "ISoundBuffer.h"
//#include <IStreamEngine.h>
#include "FmodEx/inc/fmod.hpp"
//#include "FmodEx/inc/fmod_event.hpp"

#include "DebugLogger.h"
#include "CommandPlayerFmodEx400.h"






//#define FMOD_MEMORY_DEBUGGING
//#define FMOD_MEMORY_DEBUGGING_SIMPLE
#define FMOD_STREAMING_DEBUGGING

class CSoundSystem;
class CSoundAssetManager;
class CSoundBuffer;
class CSound;
class CPlatformSoundFmodEx400Event;
class CPlatformSoundFmodEx400;

enum adfpFMODProfileSection
{
	adfpFMODProfileSection_API_INFO,
	adfpFMODProfileSection_API_OTHER,
	adfpFMODProfileSection_BUFFER_OTHER,
	adfpFMODProfileSection_BUFFER_RELEASE,
	adfpFMODProfileSection_EVENT_OTHER
};

class CFMODFileStreamCallback : public IStreamCallback
{
	virtual void StreamAsyncOnComplete (IReadStream* pStream, unsigned nError);
	virtual void StreamOnComplete (IReadStream* pStream, unsigned nError);
};

// filesystem
static FMOD_RESULT F_CALLBACK  FMODFileOpenCallback(const char *fileName, int unicode, unsigned int *filesize, void **handle, void **userdata);
static FMOD_RESULT F_CALLBACK  FMODFileCloseCallback(void *handle, void *userdata);
static FMOD_RESULT F_CALLBACK  FMODFileReadCallback(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata);
static FMOD_RESULT F_CALLBACK  FMODFileSeekCallback(void *handle, unsigned int pos, void *userdata);

static CFMODFileStreamCallback s_FMODFileCallback;

struct sSoundFileEntry 
{
	string sFilename;
	size_t nOffsetInFile;
	size_t nFileSize;
	size_t nBytesReady;
	IReadStreamPtr pReadStream;
};

typedef std::map<void*, sSoundFileEntry> tSoundFileMap;

class CAudioDeviceFmodEx400 : public IAudioDevice, IStreamCallback
{
public:
	CAudioDeviceFmodEx400(void* hWnd);
	~CAudioDeviceFmodEx400();

	virtual bool InitDevice(CSoundSystem* pSoundSystem);			// returns true if init went well 
	virtual bool ShutDownDevice(void);												// shut down fmod and free system
	virtual void* GetSoundLibrary() {return m_pAPISystem; }		// returns ptr to the sound library if possible, else NULL
	virtual void*	GetEventSystem() {return m_pEventSystem;}		// returns ptr to the event system if possible, else NULL
	virtual void  GetOutputHandle( void **pHandle, void **pHandle2, EOutputHandle *HandleType);	// returns ptr to the output handle if possible, else NULL and handletype

	virtual void GetInitSettings(AudioDeviceSettings *InitSettings);
	virtual void SetInitSettings(AudioDeviceSettings *InitSettings);

	bool ResetAudio(void);													// major system attrib has been changed del all sounds clean system start
	bool UpdateListeners(void);											// this is called every frame to update all listeners and must be called *before* SubSystemUpdate()
	bool Update(void);															// example would be CS_update()
	virtual void GetMemoryStats(enumAudioDeviceMemoryQuery eMemoryQuery, unsigned int *nMemoryUsed);
	int  GetNumberSoundsPlaying(void);							//
	virtual bool GetCpuUsage(float *fDSP, float *fStream, float* fGeometry, float* fUpdate, float *fTotal);	// returns percent of cpu usage, 1.0 is 100%
	bool SetFrequency(int newFreq);									// sets sound system global frequency
	int GetMemoryUsage(class ICrySizer* pSizer) const;	// compute memory-consumption, returns rough estimate in MB
	virtual int GetNumOpenFileHandle();							// returns number of open file handles opened by FMOD

	// accesses wavebanks
	virtual IWavebank* GetWavebank(int nIndex);
	virtual IWavebank* GetWavebank(const char* sWavebankName);
	virtual int GetWavebankCount() { return m_Wavebanks.size(); }

	CSoundBuffer*				CreateSoundBuffer(const SSoundBufferProps &BufferProps);
	IPlatformSound*			CreatePlatformSound(CSound* pSound, const char *sEventName);
	bool								RemovePlatformSound(IPlatformSound* pPlatformSound);

	// check for multiple record devices and expose them, write name to sName pointer
	bool GetRecordDeviceInfo(const int nRecordDevice, char* sName, const int nNameLength);


	// sound system settings
	bool GetParam(enumAudioDeviceParamSemantics enumtype, ptParam* pParam); // get enum for type of info and ptr type of info gotton
	// the bool returns if this info is available on this type
	bool SetParam(enumAudioDeviceParamSemantics enumtype, ptParam* pParam);

	bool IsEax(void);

	// loads a project (FEV file) and returns handle if Project is available 
	virtual ProjectIndex LoadProjectFile(const string &sProjectPath, const string &sProjectName, bool bAsync);

	// releases the data of a project but keeps it known to the system 
	virtual bool UnloadProjectFile(const ProjectIndex nProjectIndex);

	// retrieves the project entry by the project index 
	virtual SProjectFile* GetProjectFile(const ProjectIndex nProjectIndex);


	// writes output to screen in debug
	virtual void DrawInformation(IRenderer* pRenderer, float xpos, float ypos, int nSoundInfo); 

	static CFrameProfiler* GetFMODFrameProfiler(adfpFMODProfileSection enumProfileSection);

	void FindLostEvent();

#ifdef FMOD_MEMORY_DEBUGGING
	// memory debugging
	static void TraceMemory(void *ptr, int nSize, int nMemMode);

	typedef std::pair<void*, int> tMemPair;

	typedef std::map<void*, int> tMemMap;
	static tMemMap MemMap;
	static CDebugLogger MemLogger;
	static char	MemString[128];
#endif

#ifdef FMOD_MEMORY_DEBUGGING_SIMPLE
	static uint32 nFMODMemUsage;
	static uint32 nFMODMemTotalSmallAllocCount;
	static uint32 nFMODMemTotalLargeAllocCount;
	static uint32 nFMODMemActiveAllocCount;
#endif

	//static tMemPair	m_Malloc[1000];
	//static int			m_MallocIndex;
	//static tMemPair m_Free[1000];
	//static int		m_FreeIndex;

	CCommandPlayerFmodEx400 m_CommandPlayer;

private:
	FMOD::System*				m_pAPISystemDSound;
	FMOD::System*				m_pAPISystem;
	FMOD::EventSystem*	m_pEventSystem;
	mutable FMOD_RESULT	m_ExResult;
	CSoundSystem*				m_pSoundSystem;
	void*								m_pCacheStaticMemBlock;
	uint32							m_nCacheStaticMemBlockSize;













	CTimeValue m_tLastUpdate;

	int					 m_nfModDriverType;
	int					 m_nCurrentMemAlloc;
	int					 m_nMaxMemAlloc;
	unsigned int m_nMemoryStatInc;
	FMOD_CAPS		m_nFMODDriverCaps;
	//int					m_nEaxStatusFMOD;

	// init vars
	AudioDeviceSettings m_InitSettings;

	int m_nHardware3DChannels;
	int m_nHardware2DChannels;
	int m_nTotalHardwareChannelsAvail;
	//int m_nSoftwareChannels;
	FMOD_SPEAKERMODE m_nSpeakerMode;
	int m_nTotalActiveSounds; // for fmod total hardware and software channels -- for xennon total voices
	float m_fMasterVolume;// volume between 0-1, 1 being max
	bool m_bSystemPaused;
	bool m_bGetSystemStats;
	bool m_bHaveListenerAttributes;
	int m_nSystemFrequency;
	bool m_bMuteStatus;
	bool m_bDopplerStatus;
	int m_nMixingBuffSize;
	float g_fInternCacheSize;		// not a CVar
	int g_nInternCacheStatic;		// not a CVar

	CDebugLogger	 *m_pEventDumpLogger;


	// make this public as long as FMOD has no correct internal MediaPath 
	// and FOpen Callback has to parse project names
public:
	typedef std::vector<SProjectFile> VecProjectFiles;
	typedef VecProjectFiles::iterator VecProjectFilesIter;
	VecProjectFiles										m_VecLoadedProjectFiles;
private:

	typedef std::map<string, IWavebank* > tmapWavebanks;
	tmapWavebanks													m_Wavebanks;
	TFixedResourceName m_sFullWaveBankName;

	int32 m_nCountProject;
	int32 m_nCountGroup;
	int32 m_nCountEvent;

	void UpdateGroupEventCount();
	void GroupEventCount(FMOD::EventGroup* pGroup, FMOD::EventProject* pProject, TFixedResourceName sReferenceName);

	std::vector<CPlatformSoundFmodEx400Event*>	m_UnusedPlatformSoundEvents;
	std::vector<CPlatformSoundFmodEx400*>				m_UnusedPlatformSounds;

	//////////////////////////////////////////////////////////////////////////
	// Inherited Method by IStreamCallBack
	//////////////////////////////////////////////////////////////////////////
	virtual void	StreamOnComplete(IReadStream *pStream, unsigned nError);

	void *m_pHWnd; // for windows which window is active
	//float m_fpctCpuUsed;

	void FmodErrorOutput(const char * sDescription, ESoundLogType LogType = eSLT_Message) const;
	static FMOD_RESULT F_CALLBACK	FMODThreadCallback(FMOD_SYSTEM *system, FMOD_SYSTEM_CALLBACKTYPE type, void* commanddata1, void* commanddata2);

	// filesystem
	public:
		static tSoundFileMap s_SoundFileMap;
		static unsigned int s_SoundFileIndex;
#ifdef FMOD_STREAMING_DEBUGGING
		static uint32 s_nFMODFileSeekCount;
		static uint32 s_nFMODFileReadBandwidth;
		CTimeValue 	m_tLastBandwidthTime;
		float m_fStreamKBPerSecond;
#endif	

private:

	// profiling
	static CFrameProfiler* s_pFrameProfiler_API_Info;
	static CFrameProfiler* s_pFrameProfiler_API_Other;
	static CFrameProfiler* s_pFrameProfiler_Buffer_Other; 
	static CFrameProfiler* s_pFrameProfiler_Buffer_Release; 
	static CFrameProfiler* s_pFrameProfiler_Event_Other;

};


class CWavebankFmodEx400 : public IWavebank
{
public:
	CWavebankFmodEx400(const char* sWavebankName) 
		{ 
			m_sWavebankName = sWavebankName; 
			m_WavebankInfo.nFileSize = 0;
	}
	~CWavebankFmodEx400();

	virtual const char* GetName() { return m_sWavebankName.c_str(); }
	virtual const char* GetPath() { return m_sWavebankPath.c_str(); }
	virtual void SetPath(const char* sWavebankPath) { m_sWavebankPath = sWavebankPath; }

	virtual SWavebankInfo* GetInfo() { return &m_WavebankInfo; }
	virtual void AddInfo(SWavebankInfo &WavebankInfo)
	{
		m_WavebankInfo.nFileSize = max(m_WavebankInfo.nFileSize, WavebankInfo.nFileSize);
		m_WavebankInfo.nTimesAccessed = max(m_WavebankInfo.nTimesAccessed, WavebankInfo.nTimesAccessed);
		m_WavebankInfo.nMemCurrentlyInByte = WavebankInfo.nMemCurrentlyInByte;
		m_WavebankInfo.nMemPeakInByte = max(m_WavebankInfo.nMemPeakInByte, WavebankInfo.nMemPeakInByte);
	}

private:
	string				m_sWavebankName;
	string				m_sWavebankPath;
	SWavebankInfo m_WavebankInfo;
};

#endif

