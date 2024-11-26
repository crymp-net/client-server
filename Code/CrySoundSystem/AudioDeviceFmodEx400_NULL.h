#ifndef __AUDIODEVICEFMODEX400_NULL_H_
#define __AUDIODEVICEFMODEX400_NULL_H_


#include "SoundAssetManager.h"
#include "DebugLogger.h"

class CAudioDeviceFmodEx400 : public IAudioDevice, IStreamCallback
{
public:
	~CAudioDeviceFmodEx400();

	// IAudioDevice interfaces
	virtual bool InitDevice(CSoundSystem* pSoundSystem);
	virtual ProjectIndex LoadProjectFile(const char* sProjectPath, const char*sProjectName, bool bAsync);
	virtual bool UnloadProjectFile(const ProjectIndex nProjectIndex);
	virtual bool ShutDownDevice(void);
	virtual void* GetSoundLibrary(void);
	virtual void* GetEventSystem(void) const;
	virtual void GetInitSettings(AudioDeviceSettings *InitSettings);
	virtual void SetInitSettings(AudioDeviceSettings *InitSettings);
	virtual void Reset(bool bForInit);
	virtual bool ResetAudio(void);
	virtual bool UpdateListeners(void);
	virtual bool Update(void);
	virtual void GetMemoryStats(enumAudioDeviceMemoryQuery eMemoryQuery, unsigned int *nMemoryUsed); 
	virtual bool GetCpuUsage(float *fDSP, float *fStream, float* fGeometry, float* fUpdate, float *fTotal);
	virtual bool SetFrequency(int newFreq); 
	virtual int  GetNumberSoundsPlaying(void);
	virtual int GetMemoryUsage(class ICrySizer* pSizer) const;
	virtual int GetNumOpenFileHandle();
	virtual IWavebank* GetWavebank(int nIndex);
	virtual IWavebank* GetWavebank(const char* sWavebankName);
	virtual int GetWavebankCount();
	virtual CSoundBuffer* CreateSoundBuffer(const SSoundBufferProps &BufferProps);
	virtual IPlatformSound* CreatePlatformSound(ISound* pSound, const char *sEventName);
	virtual bool GetRecordDeviceInfo(const int nRecordDevice, char* sName, const int nNameLength);
	virtual bool GetParam(enumAudioDeviceParamSemantics enumtype, ptParam* pParam); // get enum for type of info and ptr type of info gotton
	virtual bool SetParam(enumAudioDeviceParamSemantics enumType, ptParam* pParam);
	virtual void DrawInformation(IRenderer* pRenderer, float xpos, float ypos, int nSoundInfo);
	virtual void GetBandwidthStats(float * fBandwidthRequested);

	// CAudioDeviceFmodEx400 interfaces
	inline int const GetSpeakerConfig() const { return 0; }
	inline size_t const GetNumLoadRequests(size_t const nThreadID) const { return 0; }
	void OnCVarChanged(ICVar* const pCVar);
};

#endif // __AUDIODEVICEFMODEX400_NULL_H_
