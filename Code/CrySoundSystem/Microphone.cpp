////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   Microphone.h
//  Version:     v1.00
//  Created:     30/11/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: implementation of a Microphone class.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#ifdef SOUNDSYSTEM_USE_FMODEX400

#include "IAudioDevice.h"
#include "Microphone.h"
#include "SoundBuffer.h"
#include "Sound.h"
#include "FmodEx/inc/fmod.hpp"

//////////////////////////////////////////////////////////////////////////
// Initialization
//////////////////////////////////////////////////////////////////////////

CMicrophone::CMicrophone(IAudioDevice *pAudioDevice)
{
	m_pAudioDevice			= pAudioDevice;
	m_pBufMicro					= NULL;
	m_DataSize			=0;
	
}

CMicrophone::~CMicrophone()
{
}


void CMicrophone::Release()
{
	CryAutoLock<TMicroMutex> lock(m_Mutex);
	Stop();
}

void CMicrophone::Update()
{
	CryAutoLock<TMicroMutex> lock(m_Mutex);
	if(!m_pBufMicro)
	{
		m_DataSize=0;
		return;
	}
	if (m_pBufMicro)
	{
		m_pBufMicro->Update();
	}
}

bool CMicrophone::Stop()
{
	CryAutoLock<TMicroMutex> lock(m_Mutex);
	if (m_pBufMicro)
	{
		m_pBufMicro->UnloadData(sbUNLOAD_ALL_DATA);
		m_pBufMicro = NULL;
	}

	m_TempBuffer.reset();

	return true;
}

bool CMicrophone::Record(const unsigned int nRecordDevice, const unsigned int nBitsPerSample, 
					const unsigned int nSamplesPerSecond, const unsigned int nBufferSizeInSamples)
{
	CryAutoLock<TMicroMutex> lock(m_Mutex);
	SSoundBufferProps MicroProp = SSoundBufferProps("MicrophoneStream", 0, btMICRO, 0, 0); 
	m_pBufMicro = m_pAudioDevice->CreateSoundBuffer(MicroProp);

	m_pBufMicro->GetInfo()->nBitsPerSample = nBitsPerSample;
	m_pBufMicro->GetInfo()->nBaseFreq = nSamplesPerSecond;
	m_pBufMicro->GetInfo()->nChannels = 1;
	m_pBufMicro->GetInfo()->nLengthInSamples = nBufferSizeInSamples;
	
	m_pBufMicro->LoadAsMicro(this);

	//allocate temporary buffer 
	m_TempBufferSize=nBufferSizeInSamples;
	m_TempBuffer.reset(new uint8[m_TempBufferSize]);

	return true;
}

int16* CMicrophone::GetData()
{
	return (int16*)m_TempBuffer.get();
}

int CMicrophone::GetDataSize()
{
	return m_DataSize;
}

bool CMicrophone::ReadDataBuffer(const void* ptr1,const unsigned int len1,const void* ptr2,const unsigned int len2)
{
	CryAutoLock<TMicroMutex> lock(m_Mutex);

	memcpy(m_TempBuffer.get(),ptr1,min(len1,m_TempBufferSize));
	if(len1<m_TempBufferSize)
		memcpy(m_TempBuffer.get()+len1,ptr2,min(len2,m_TempBufferSize-len1));

	m_DataSize=min(m_TempBufferSize,len1+len2);

	return true;
}

void CMicrophone::OnError(const char *error)
{
	//FIXME
}

#include UNIQUE_VIRTUAL_WRAPPER(IMicrophone)

#endif

