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

#ifndef __MICROPHONE_H__
#define __MICROPHONE_H__

#include "CryCommon/CrySoundSystem/ISound.h"
#include "CryThread.h"
#include "ISoundBuffer.h"

#pragma once

class  CSoundBuffer;

class CMicrophone : public IMicrophone,public IMicrophoneStream
{

public:
	CMicrophone(IAudioDevice *pAudioDevice);
	~CMicrophone(void);

	//////////////////////////////////////////////////////////////////////////
	// Initialization
	//////////////////////////////////////////////////////////////////////////

	void Release();

	bool Record(const unsigned int nRecordDevice, const unsigned int nBitsPerSample, 
							const unsigned int nSamplesPerSecond, const unsigned int nBufferSizeInSamples);

	void Update();

	bool Stop();

	int GetDataSize();

	int16* GetData();

	//from IMicrophoneStream
	bool ReadDataBuffer(const void* ptr1,const unsigned int len1,const void* ptr2,const unsigned int len2);

	void OnError(const char *error);
private:
	typedef CryMutex TMicroMutex;

	TMicroMutex									m_Mutex;
	IAudioDevice								*m_pAudioDevice; // AudioDevice
	CSoundBuffer								*m_pBufMicro;

	std::auto_ptr<uint8>						m_TempBuffer;
	unsigned int								m_DataSize;
	unsigned int								m_TempBufferSize;
};
#endif