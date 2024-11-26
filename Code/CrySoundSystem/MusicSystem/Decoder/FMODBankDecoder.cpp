#include "StdAfx.h"

#include "CryCommon/CryCore/CrySizer.h"
#include "CryCommon/CrySoundSystem/IMusicSystem.h"
#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/ICryPak.h"
#include "CryCommon/CrySystem/ITimer.h"

#include <IAudioDevice.h>
#include "FMODBankDecoder.h"
#include "../Pattern/MusicPattern.h"
#include "../MusicSystem.h"

void AddPhysicalBlock(long size);

CFMODBankDecoder::CFMODBankDecoder(IMusicSystem *pMusicSystem, CMusicPattern *pMusicPattern)
{
	m_nPosInSamples = 0;
	m_nSubSoundIndex = 0;
	//m_SubSound = NULL;
	m_nMainThreadId = 0;
	m_pReadStream = 0;
	m_BufferData = 0;
	m_BufferLength = 0;
	m_pMusicSystem = (CMusicSystem*) gEnv->pMusicSystem;

	m_FileInfo.pMusicPattern = pMusicPattern;
}

CFMODBankDecoder::~CFMODBankDecoder()
{

	float f0 = 0.0f;
	float f1 = 0.0f;

	if (m_pMusicSystem->g_nMusicProfiling)
		f0 = gEnv->pTimer->GetAsyncCurTime();

	Close();

	if (m_pReadStream != NULL)
	{
		m_pReadStream->Abort();	
		m_pReadStream = NULL;
	}

	if (m_BufferData != NULL)
	{
		long nTemp = 0 - m_BufferLength;
		AddPhysicalBlock(nTemp);




		delete[] ((uint8*)m_BufferData);

		m_BufferData = 0;
		m_BufferLength = 0;

	}

	if (m_pMusicSystem->g_nMusicProfiling)
	{
		f1 = gEnv->pTimer->GetAsyncCurTime();
		CryLog(" Profile: Decoder-Destruction %s %f", m_FileInfo.sFileName.c_str(), (f1-f0)*1000.0f);
	}


}

//////////////////////////////////////////////////////////////////////////
void CFMODBankDecoder::StreamOnComplete(IReadStream *pStream, unsigned nError)
{
	FUNCTION_PROFILER( gEnv->pSystem, PROFILE_MUSIC );

	MEMSTAT_CONTEXT_FMT(EMemStatContextTypes::MSC_FSB, 0, "Music FSB %s", m_FileInfo.sFileName.c_str() );
	
	
	// Dont need stream anymore.
	m_pReadStream = NULL;

	if (nError)
	{
		gEnv->pSoundSystem->Log(eSLT_Error, "<Sound> Music - Cannot load FSB %s\n", m_FileInfo.sFileName.c_str());
		//m_Info.bLoadFailure = true;
		//LoadFailed();
		return;
	}

	// make a local copy of the file and store it only once in the decoder
	m_BufferLength = pStream->GetBytesRead();

	if (m_BufferData)
		return;

	float f0, f1, f2, f3;
	
	if (m_pMusicSystem->g_nMusicProfiling)
		f0 = gEnv->pTimer->GetAsyncCurTime();




		m_BufferData = new int8[ m_BufferLength ];


	if (!m_BufferData)
		return;

	// Added statistics for static memory block
	AddPhysicalBlock(m_BufferLength);

	memcpy(m_BufferData, pStream->GetBuffer(), m_BufferLength);

	FMOD::System *pSystem = (FMOD::System *)gEnv->pSoundSystem->GetIAudioDevice()->GetSoundLibrary(); // unsafe!
	FMOD_MODE Mode = FMOD_OPENONLY | FMOD_LOWMEM | FMOD_CREATESTREAM | FMOD_OPENMEMORY_POINT;
	FMOD_RESULT Result = FMOD_OK;
	FMOD_CREATESOUNDEXINFO  ExInfo = {0}; 
	ExInfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	ExInfo.length = m_BufferLength;

	{
		FRAME_PROFILER( "FMOD-CreateSound Music FSB-Decoder", GetISystem(), PROFILE_MUSIC );
		//Result = pSystem->createSound(sBankFileName, Mode, 0, &m_BankSound);
		Result = pSystem->createSound((const char*)m_BufferData, Mode, &ExInfo, &m_BankSound);
	}
	if (m_pMusicSystem->g_nMusicProfiling)
	{
		f1 = gEnv->pTimer->GetAsyncCurTime();
		CryLog(" Profile: StreamOnComplete Decoder %s %f", m_FileInfo.sFileName.c_str(), (f1-f0)*1000.0f);
	}

	if (!m_BankSound)
	{
		gEnv->pSoundSystem->Log(eSLT_Error, "<Sound> Music - Cannot load FSB %s\n", m_FileInfo.sFileName.c_str());
		//m_Info.bLoadFailure = true;
		//LoadFailed();
		return;
	}	

	if (Result != FMOD_OK || m_BankSound == NULL)
		return;


	if (m_pMusicSystem->g_nMusicProfiling)
		f2 = gEnv->pTimer->GetAsyncCurTime();


	Result = m_BankSound->getSubSound(m_nSubSoundIndex, &m_SubSound);

	if (m_SubSound)
	{
		Result = m_SubSound->getFormat(0, 0, &(m_FileInfo.nChannels), &(m_FileInfo.nBitsPerSample));
		Result = m_SubSound->getDefaults(&(m_FileInfo.fFrequency), 0, 0, 0);

		Result = m_SubSound->getLength(&(m_FileInfo.nSamples), FMOD_TIMEUNIT_PCM);
		Result = m_SubSound->getLength(&(m_FileInfo.nRawBytes), FMOD_TIMEUNIT_RAWBYTES);

		m_FileInfo.pMusicPattern->SetNumSamples(m_FileInfo.nSamples);

		assert (m_FileInfo.nChannels);
		assert (m_FileInfo.nBitsPerSample);
		assert (m_FileInfo.fFrequency);
		assert (m_FileInfo.nSamples);
		assert (m_FileInfo.nRawBytes);

		if ( !m_FileInfo.nChannels || !m_FileInfo.nBitsPerSample || !m_FileInfo.fFrequency || !m_FileInfo.nSamples || !m_FileInfo.nRawBytes )
		{
			int catchme = 1;
			(void)catchme;
		}
		if (m_pMusicSystem->g_nMusicProfiling)
		{
			f3 = gEnv->pTimer->GetAsyncCurTime();
			CryLog(" Profile: Decoder SubSound %s %f", m_FileInfo.sFileName.c_str(), (f3-f2)*1000.0f);
		}

		// DO NOT CLOSE sound from other thread than main
		//Result = m_SubSound->release();
	}

	// Sounds will be release on Close() from Main thread

	m_nPosInSamples = 0;

}

bool CFMODBankDecoder::Open(const char *sBankFileName, const char *sSoundName)
{
	m_nSubSoundIndex = 0;

	m_BankSound = NULL;
	m_SubSound = NULL;

	m_FileInfo.sFileName = sBankFileName;
	m_FileInfo.sSoundName = sSoundName;	

	if (!m_pReadStream && !m_BankSound)
	{
		m_pReadStream = gEnv->pSystem->GetStreamEngine()->StartRead(eStreamTaskTypeMusic, sBankFileName, this);
		if (m_pReadStream == 0)
			return false;
	}
	return true;
}

bool CFMODBankDecoder::Close()
{
	//uint32 nMainThreadId = gEnv->pFrameProfileSystem->GetMainThreadId();
	//if (CryGetCurrentThreadId() != nMainThreadId)
	//{	
	//	int a = 0; // catch me
	//}

	//if (m_SubSound)
	//{
	//	m_SubSound->release();
	//}

	

	if (m_BankSound != NULL)
	{
		float f0, f1;

		if (m_pMusicSystem->g_nMusicProfiling)
			f0 = gEnv->pTimer->GetAsyncCurTime();

		FMOD_RESULT Result = m_BankSound->release();
		
		if (m_pMusicSystem->g_nMusicProfiling)
		{
			f1 = gEnv->pTimer->GetAsyncCurTime();
			CryLog(" Profile: Decoder-BankRelease %s %f", m_FileInfo.sFileName.c_str(), (f1-f0)*1000.0f);
		}
	}

	m_SubSound = 0;
	m_BankSound = 0;

	return true;
}

bool CFMODBankDecoder::GetFileInfo(SMusicPatternFileInfo &FileInfo)
{

	FileInfo = m_FileInfo;

	return true;
}

void CFMODBankDecoder::GetMemoryUsage(class ICrySizer* pSizer)
{
	if (m_BufferData)
		pSizer->AddObject(m_BufferData, m_BufferLength);

	if (!pSizer->Add(*this))
		return;
}

IMusicPatternDecoderInstance* CFMODBankDecoder::CreateInstance()
{
	return new CFMODBankDecoderInstance(this);
}

void CFMODBankDecoder::MainThreadUpdate(uint32 nMainThreadId)
{
	m_nMainThreadId = nMainThreadId;
	Close(); // we dont need Decoders sounds anymore.
}

//////////////////////////////////////////////////////////////////////////
/*** INSTANCE ***/
//////////////////////////////////////////////////////////////////////////

CFMODBankDecoderInstance::CFMODBankDecoderInstance(CFMODBankDecoder *pDecoder) : m_bCopyFromLastFrame(false)
{
	m_pDecoder = pDecoder;
	m_BankSound = 0;
	m_SubStream = 0;
	m_nPosInSamples = 0;	
	m_nStreamPosInSamples = 0;
	m_nMainThreadId = 0;
	m_pReadStream = 0;

	m_pMusicSystem = (CMusicSystem*) gEnv->pMusicSystem;

	Init();

	Seek0();
}

CFMODBankDecoderInstance::~CFMODBankDecoderInstance()
{
	//uint32 nMainThreadId = gEnv->pFrameProfileSystem->GetMainThreadId();
	//if (CryGetCurrentThreadId() != nMainThreadId)
	//{	
	//	int a = 0; // catch me
	//}

	//if (m_SubStream)
	//	m_SubStream->release();

	if (m_BankSound)
	{
		float f0, f1;
		if (m_pMusicSystem->g_nMusicProfiling)
			f0 = gEnv->pTimer->GetAsyncCurTime();

		FMOD_RESULT Result = m_BankSound->release();

		if (m_pMusicSystem->g_nMusicProfiling)
		{
			f1 = gEnv->pTimer->GetAsyncCurTime();
			CryLog(" Profile: DecoderInstance-BankRelease %s %f", m_pDecoder->m_FileInfo.sFileName.c_str(), (f1-f0)*1000.0f);
		}
	}
		
	m_BankSound = 0;
	m_SubStream = 0;
}

void CFMODBankDecoderInstance::Init()
{
	if(!m_BankSound && m_pDecoder->m_BufferData)
	{
		FMOD_RESULT Result = FMOD_OK;
		FMOD::System *pSystem = (FMOD::System *)gEnv->pSoundSystem->GetIAudioDevice()->GetSoundLibrary(); // unsafe!
		FMOD_MODE Mode = FMOD_CREATESTREAM | FMOD_LOWMEM | FMOD_OPENMEMORY_POINT;

		FMOD_CREATESOUNDEXINFO  ExInfo = {0}; 
		ExInfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		ExInfo.length = m_pDecoder->m_BufferLength;

		float f0, f1;

		if (m_pMusicSystem->g_nMusicProfiling)
			f0 = gEnv->pTimer->GetAsyncCurTime();

		{
			FRAME_PROFILER( "FMOD-CreateSound Music FSB-DecoderInstace", GetISystem(), PROFILE_MUSIC );
			Result = pSystem->createSound((const char *)m_pDecoder->m_BufferData, Mode, &ExInfo, &m_BankSound);
		}

		if (m_pMusicSystem->g_nMusicProfiling)
		{
			f1 = gEnv->pTimer->GetAsyncCurTime();
			CryLog(" Profile: DecoderInstance %s %f", m_pDecoder->m_FileInfo.sFileName.c_str(), (f1-f0)*1000.0f);
		}

		if (Result == FMOD_OK && m_BankSound)
		{
			Result = m_BankSound->getSubSound(m_pDecoder->m_nSubSoundIndex, &m_SubStream);
		}

	}
}


bool CFMODBankDecoderInstance::Seek0(const int nDelayInSamples)
{
	m_nPosInSamples = -nDelayInSamples;
	m_nStreamPosInSamples = m_nPosInSamples + 1; // enforce next seek
	m_bCopyFromLastFrame = false;
	
	return SeekPos(0);
}

//! Seek to pos in number of samples of pattern.
bool CFMODBankDecoderInstance::SeekPos(const int nSamplePos)
{
	if (!m_SubStream)
	{
		// instance not ready, save the sample pos so we go there later
		m_nPosInSamples = nSamplePos;
		return true;
	}

	if (nSamplePos != m_nStreamPosInSamples)
	{
		FMOD_RESULT Result = m_SubStream->seekData(nSamplePos);
		if (Result == FMOD_OK)
		{
			m_nPosInSamples = nSamplePos;
			m_nStreamPosInSamples = m_nPosInSamples;

			return true;
		}
		else
			return false;

	}
	
	return true;
}

int CFMODBankDecoderInstance::GetPos()
{
	if (!m_SubStream)
		return -1;

	return m_nPosInSamples;
}

bool CFMODBankDecoderInstance::GetPCMData(signed long *pDataOut, int nSamples, bool bLoop)
{
	if (!m_SubStream)
		return false;

	FRAME_PROFILER( "Decoder", GetISystem(), PROFILE_MUSIC );	

	int nOfs = 0;

	if (m_nPosInSamples < 0)
	{
		if (-m_nPosInSamples >= nSamples)
		{
			memset(pDataOut, 0, nSamples * gEnv->pMusicSystem->GetBytesPerSample());
			m_nPosInSamples += nSamples;
			return true;
		}
		else
		{ 
			nOfs = -m_nPosInSamples;
			m_nPosInSamples = 0;
			nSamples -= nOfs;
			memset(pDataOut, 0, nOfs * gEnv->pMusicSystem->GetBytesPerSample());
		}
	}

	if (!SeekPos(m_nPosInSamples))
		return false;

	int nSamplesToRead;
	for (;;)
	{
		if ((m_nPosInSamples + nSamples) > (int)m_pDecoder->m_FileInfo.nSamples)
			nSamplesToRead = m_pDecoder->m_FileInfo.nSamples - m_nPosInSamples;
		else
			nSamplesToRead = nSamples;

		unsigned int nBytes = nSamples * (m_pDecoder->m_FileInfo.nBitsPerSample / 4);
		unsigned int nBytesRead = 0;

		FMOD_RESULT Result = m_SubStream->readData(&(pDataOut[nOfs]), nBytes, &nBytesRead);

		nSamplesToRead = nBytesRead / ((m_pDecoder->m_FileInfo.nBitsPerSample / 4));

		if (Result == FMOD_ERR_FILE_EOF)
		{
			int a = 1;(void)a;
			if (nBytesRead < (m_pDecoder->m_FileInfo.nSamples - m_nPosInSamples) )
			{
				nBytes = (m_pDecoder->m_FileInfo.nSamples - m_nPosInSamples) * (m_pDecoder->m_FileInfo.nBitsPerSample / 4);
				Result = m_SubStream->readData(&(pDataOut[nOfs]), nBytes, &nBytesRead);
			}
		}
		else
		{
			if (nBytes != nBytesRead)
			{
				int a = 0; (void)a;// catch me!
			}
		}

		// FIXME: conversation of samplerate might be needed here
		m_nPosInSamples += nSamplesToRead;
		m_nStreamPosInSamples = m_nPosInSamples;

		if (nSamplesToRead == nSamples)
			break;

		nOfs += nSamplesToRead;

		if (!bLoop)
		{
			memset(&(pDataOut[nOfs]), 0, (nSamples-nSamplesToRead) * gEnv->pMusicSystem->GetBytesPerSample());
			return false;
			break;
		}

		nSamples -= nSamplesToRead;
		Seek0();
	}
	return true;
}

void CFMODBankDecoderInstance::GetMemoryUsage(class ICrySizer* pSizer)
{
	if (!pSizer->Add(*this))
		return;
}

