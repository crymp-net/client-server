#include "StdAfx.h"
#include "AudioDeviceFmodEx400_NULL.h"

CAudioDeviceFmodEx400::~CAudioDeviceFmodEx400()
{
}

bool CAudioDeviceFmodEx400::InitDevice(CSoundSystem* pSoundSystem)
{
	return false;
}

ProjectIndex CAudioDeviceFmodEx400::LoadProjectFile(const char* sProjectPath, const char*sProjectName, bool bAsync)
{
	return PROJECTINDEX_INVALID;
}

bool CAudioDeviceFmodEx400::UnloadProjectFile(const ProjectIndex nProjectIndex)
{
	return false;
}

bool CAudioDeviceFmodEx400::ShutDownDevice(void)
{
	return false;
}

void* CAudioDeviceFmodEx400::GetSoundLibrary(void)
{
	return NULL;
}

void* CAudioDeviceFmodEx400::GetEventSystem(void) const
{
	return NULL;
}

void CAudioDeviceFmodEx400::GetInitSettings(AudioDeviceSettings *InitSettings)
{
}

void CAudioDeviceFmodEx400::SetInitSettings(AudioDeviceSettings *InitSettings)
{
}

void CAudioDeviceFmodEx400::Reset(bool bForInit)
{
}

bool CAudioDeviceFmodEx400::ResetAudio(void)
{
	return false;
}

bool CAudioDeviceFmodEx400::UpdateListeners(void)
{
	return false;
}

bool CAudioDeviceFmodEx400::Update(void)
{
	return false;
}

void CAudioDeviceFmodEx400::GetMemoryStats(enumAudioDeviceMemoryQuery eMemoryQuery, unsigned int *nMemoryUsed)
{
}

bool CAudioDeviceFmodEx400::GetCpuUsage(float *fDSP, float *fStream, float* fGeometry, float* fUpdate, float *fTotal)
{
	return false;
}

bool CAudioDeviceFmodEx400::SetFrequency(int newFreq)
{
	return false;
}

int  CAudioDeviceFmodEx400::GetNumberSoundsPlaying(void)
{
	return false;
}

int CAudioDeviceFmodEx400::GetMemoryUsage(class ICrySizer* pSizer) const
{
	return 0;
}

int CAudioDeviceFmodEx400::GetNumOpenFileHandle()
{
	return 0;
}

IWavebank* CAudioDeviceFmodEx400::CAudioDeviceFmodEx400::GetWavebank(int nIndex)
{
	return NULL;
}

IWavebank* CAudioDeviceFmodEx400::GetWavebank(const char* sWavebankName)
{
	return NULL;
}

int CAudioDeviceFmodEx400::GetWavebankCount()
{
	return 0;
}

CSoundBuffer* CAudioDeviceFmodEx400::CreateSoundBuffer(const SSoundBufferProps &BufferProps)
{
	return NULL;
}

IPlatformSound* CAudioDeviceFmodEx400::CreatePlatformSound(ISound* pSound, const char *sEventName)
{
	return NULL;
}

bool CAudioDeviceFmodEx400::GetRecordDeviceInfo(const int nRecordDevice, char* sName, const int nNameLength)
{
	return false;
}

bool CAudioDeviceFmodEx400::GetParam(enumAudioDeviceParamSemantics enumtype, ptParam* pParam)
{
	return false;
}

bool CAudioDeviceFmodEx400::SetParam(enumAudioDeviceParamSemantics enumType, ptParam* pParam)
{
	return false;
}

void CAudioDeviceFmodEx400::DrawInformation(IRenderer* pRenderer, float xpos, float ypos, int nSoundInfo)
{
}

void CAudioDeviceFmodEx400::GetBandwidthStats(float * fBandwidthRequested)
{
}

void CAudioDeviceFmodEx400::OnCVarChanged(ICVar* const pCVar)
{
}

