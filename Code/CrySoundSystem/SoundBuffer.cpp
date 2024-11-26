////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   SoundBuffer.cpp
//  Version:     v1.00
//  Created:     ?? refactored 4/4/2005 by Tomas
//  Compilers:   Visual Studio.NET
//  Description: Generic Implementation of SoundBuffer
// -------------------------------------------------------------------------
//  History:
//   8/6/05 NICK fixed load call to handle calls with cSound == NULL for precache calls
//
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include <algorithm>
#include "SoundBuffer.h"
#include "SoundSystem.h"
#include "Sound.h"
#include "ITimer.h"

//////////////////////////////////////////////////////////////////////////
CSoundBuffer::CSoundBuffer(const SSoundBufferProps &Props) : m_Props(Props)
{
	m_pSoundSystem						=	(CSoundSystem*) gEnv->pSoundSystem; 
	m_pSoundAssetManager			= NULL;
	if (m_pSoundSystem)
		m_pSoundAssetManager = (CSoundAssetManager*) m_pSoundSystem->GetISoundAssetManager();
	m_BufferData							=	NULL;
	m_pReadStream							=	NULL;
	m_StreamBuffer						= NULL;
	m_pManager								= NULL;
	m_nRef										=	0;		

	m_Info.nBaseFreq					=	44100;
	m_Info.nBitsPerSample			= 16;
	m_Info.nChannels					= 1;
	m_Info.nTimesUsed					= 1;
	m_Info.nTimedOut					= 0;
	m_Info.nLengthInBytes			=	0;
	m_Info.nLengthInMs				=	0;
	m_Info.nLengthInSamples		=	0;
	m_Info.bLoadFailure				=	false;

	m_bCallbackGuard					=	false;
	m_tLastTimeUsed.SetSeconds(0.0f);

	m_vecLoadReq.clear();
	m_vecLoadReqToRemove.clear();
	m_vecLoadReqToAdd.clear();
}
//////////////////////////////////////////////////////////////////////////
#define CALLBACK_FUNC(_func) \
	m_bCallbackGuard = true; \
	TBufferLoadReqVecItConst ItEnd = m_vecLoadReq.end(); \
	for (TBufferLoadReqVecItConst It=m_vecLoadReq.begin(); It!=ItEnd; ++It) \
{ \
	(*It)->_func(); \
} \
	m_bCallbackGuard = false; \
	UpdateCallbacks();


CSoundBuffer::~CSoundBuffer()
{
	if (m_pReadStream != NULL)
	{
		m_pReadStream->Abort();	
		m_pReadStream = NULL;
	}
	//CALLBACK_FUNC(OnBufferLoadFailed);

//	DestroyData();
}


//////////////////////////////////////////////////////////////////////////
/*
#define CALLBACK_FUNC(_func) \
m_bCallbackGuard = true; \
for (TBufferLoadReqVecIt It=m_vecLoadReq.begin();It!=m_vecLoadReq.end();++It) \
{ \
(*It)->_func(); \
} \
m_vecLoadReq.clear(); \
m_bCallbackGuard = false; \
UpdateCallbacks();
*/


int CSoundBuffer::AddRef()
{
	return ++m_nRef;
}

int CSoundBuffer::Release()
{
	--m_nRef;
	int returnRefs = m_nRef;	

	if (returnRefs == 0)
	{
		delete this;
	}

	//if (returnRefs < 0)
	//{
	//	int bla = 1;
	//}

	return returnRefs;
}

#if defined(_DEBUG) && defined(WIN64)
static __int64 g_numSBSetSample = 0;
#endif

void CSoundBuffer::SetProps(SSoundBufferProps NewProps)
{
	m_Props.sName = NewProps.sName;
	m_Props.nFlags		= NewProps.nFlags;
}

//////////////////////////////////////////////////////////////////////////
void CSoundBuffer::SetAssetHandle(tAssetHandle AssetHandle, enumBufferType eBufferType)
{
	//GUARD_HEAP;

	//TODO What is this supposed to do?
#if defined(_DEBUG) && defined(WIN64)
	++g_numSBSetSample;
#endif

	// New AssetHandle and not set yet, so we have a new Buffer
	if (AssetHandle && (!m_BufferData))
	{
		if (m_pSoundSystem)
			m_pSoundSystem->BufferLoaded();
		m_Props.eBufferType = eBufferType;
	}

	// No AssetHandle and there is one set, so we have to remove it
	if ((!AssetHandle) && m_BufferData)
	{
		if (m_pSoundSystem)
			m_pSoundSystem->BufferUnloaded();
		m_Props.eBufferType = eBufferType;
	}
	m_BufferData = AssetHandle;
}

//////////////////////////////////////////////////////////////////////////
void CSoundBuffer::UpdateCallbacks()
{
	if (m_bCallbackGuard)
		return;

	//GUARD_HEAP;
	for (TBufferLoadReqVecItConst It=m_vecLoadReqToRemove.begin(); It!=m_vecLoadReqToRemove.end(); ++It)
	{
		TBufferLoadReqVecIt RemIt = std::find(m_vecLoadReq.begin(), m_vecLoadReq.end(), *It);
		if (RemIt != m_vecLoadReq.end())
			m_vecLoadReq.erase(RemIt);
	}
	m_vecLoadReqToRemove.clear();
	for (TBufferLoadReqVecItConst It=m_vecLoadReqToAdd.begin(); It!=m_vecLoadReqToAdd.end(); ++It)
	{
		TBufferLoadReqVecIt RemIt = std::find(m_vecLoadReq.begin(), m_vecLoadReq.end(), *It);
		if (RemIt == m_vecLoadReq.end())
			m_vecLoadReq.push_back(*It);
	}
	m_vecLoadReqToAdd.clear();
}

//////////////////////////////////////////////////////////////////////////
void CSoundBuffer::SoundBufferLoaded()
{
	FUNCTION_PROFILER( gEnv->pSystem,PROFILE_SOUND );

	if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_DETAIL)
		m_pSoundSystem->Log(eSLT_Message, "<Sound> file %s, loaded \n", m_Props.sName.c_str());
	// broadcast OnBufferLoaded
	//GUARD_HEAP;
	CALLBACK_FUNC(OnBufferLoaded);
}

//////////////////////////////////////////////////////////////////////////
void CSoundBuffer::LoadFailed()
{
	if ( m_pSoundSystem )
	{
		ISystem *pSystem = GetISystem();

		m_pSoundSystem->Log(eSLT_Warning, "<Sound> Sound %s failed to load!", m_Props.sName.c_str());

		// File not found, extra error log
		TFixedResourceName sTemp = "File not found: " + m_Props.sName;
		m_pSoundSystem->m_pDebugLogger->LogString(sTemp.c_str());

	}
	// broadcast OnBufferLoadFailed
	CALLBACK_FUNC(OnBufferLoadFailed);
}

//////////////////////////////////////////////////////////////////////////
void CSoundBuffer::RemoveFromLoadReqList(CSound *pSound)
{
	//GUARD_HEAP;
	m_vecLoadReqToRemove.push_back(pSound);
	UpdateCallbacks();
}

//////////////////////////////////////////////////////////////////////////
void CSoundBuffer::AddToLoadReqList(CSound *pSound)
{
	//GUARD_HEAP;
	m_vecLoadReqToAdd.push_back(pSound);
	UpdateCallbacks();
}

//////////////////////////////////////////////////////////////////////////
bool CSoundBuffer::Load(CSound *pSound)
{
	FUNCTION_PROFILER( gEnv->pSystem, PROFILE_SOUND );

	MEMSTAT_CONTEXT_FMT(EMemStatContextTypes::MSC_SoundBuffer, 0, "%s", GetName() );

	CTimeValue TimeNow = gEnv->pTimer->GetFrameStartTime();
	UpdateTimer(TimeNow);

  if (pSound)
  {
		AddToLoadReqList(pSound);
	}
	
	if (m_BufferData)
	{
		if (pSound)
			pSound->OnBufferLoaded();
		return true; // already loaded
	}

	if (Loading())
		return true; // Loading already in progress

	if (LoadFailure())
	{
		if (pSound)
			pSound->OnBufferLoadFailed();
		return false; //already tried to open the file before
	}

	if (Loaded())
		return true;

	if (m_Props.sName.empty())
		return false;

	switch (m_Props.eBufferType)
	{
	case btSAMPLE:
		{
			assert(m_pSoundSystem->m_pStreamEngine);
			if (!m_pReadStream && !m_BufferData && !m_StreamBuffer)
			{
				//float f0 = gEnv->pTimer->GetAsyncCurTime();

				uint32 nSize = gEnv->pCryPak->FGetSize(m_Props.sName.c_str());

				//float f1 = gEnv->pTimer->GetAsyncCurTime();
				//CryLog("<Sound Profile GetFileSize> %s %.2f", m_Props.sName.c_str(), (f1-f0)*1000.0f);

				// file does not exist
				if (!nSize) 
					return false;

				// TODO, use RSX memory for PS3 if possible
				m_StreamBuffer = new int8[ nSize ];
				//gEnv->pSoundSystem->Log(eSLT_Always, "<Sound> SBBase-Load-StreamBufferCreate: %s Buffer:%d\n", m_Props.sName.c_str(), m_StreamBuffer);

				if (!m_StreamBuffer)
					return false;

				// Remember size for correct tracking
				m_nStreamBufferSize = nSize;

				StreamReadParams StrParams;
				StrParams.nFlags = 0;
				StrParams.dwUserData = 0;
				StrParams.nLoadTime = 0;
				StrParams.nMaxLoadTime = 0;
				StrParams.nPriority = 0;
				StrParams.pBuffer = m_StreamBuffer;
				StrParams.nOffset = 0;
				StrParams.nSize = nSize;
				m_pReadStream = m_pSoundSystem->m_pStreamEngine->StartRead(eStreamTaskTypeSound, m_Props.sName.c_str(), this, &StrParams);
			}

			// added this so can be called by precache with out crash cause pSound == NULL
			if (m_Props.nFlags & FLAG_SOUND_LOAD_SYNCHRONOUSLY || m_pSoundSystem->g_nLoadNonBlocking==0)
			{
				if (m_pReadStream)
					m_pReadStream->Wait();
			}

			// Placeholder sound.
			if (m_pSoundSystem->g_nErrorSound > 2)
			{
				if (m_pReadStream)
					m_pReadStream->Wait();

				if (m_Info.bLoadFailure)
				{
					// prevent recursive calls
					if (m_Props.sName.compare("Sounds/errors/errors.wav"))
					{
						m_Info.bLoadFailure = false;
						m_Props.sName = "Sounds/errors/errors.wav";
						return Load( pSound );
					}
				}
			}
			break;
		}
	case btSTREAM:
		{
			void*  pSoundStream		= NULL;		

			pSoundStream = LoadAsStream(m_Props.sName.c_str(), 0);

			if (!pSoundStream)
			{
				//cannot be loaded
				m_pSoundSystem->Log(eSLT_Warning, "<Sound> Error: Cannot load streaming sound %s\n", m_Props.sName.c_str());
				m_Info.bLoadFailure = true;
				return false;
			}

			//set the sound buffer
			SetAssetHandle(pSoundStream, btSTREAM);
			SoundBufferLoaded();
			break;
		}
	case btEVENT:
		{
			void*  pSoundStream		= NULL;		

			pSoundStream = LoadAsEvent(m_Props.sName.c_str());

			if (!pSoundStream)
			{
				//cannot be loaded
				m_pSoundSystem->Log(eSLT_Error, "<Sound> Cannot load event group %s\n", m_Props.sName.c_str());
				m_Info.bLoadFailure = true;
				return false;
			}

			//set the sound buffer
			SetAssetHandle(pSoundStream, btEVENT);
			SoundBufferLoaded();
			break;
		}
		//{
		//	void*  pSoundStream		= NULL;		
		//	const char* pFileName = m_Props.sName.c_str();

		//	pSoundStream = LoadAsEvent(pFileName);

		//	if (!pSoundStream)
		//	{
		//		//cannot be loaded
		//		gEnv->pLog->LogToFile("Error: <Sound> Cannot load event sound %s\n", m_Props.sName.c_str());
		//		m_Info.bLoadFailure = true;
		//		return false;
		//	}

		//	//set the sound buffer
		//	SetAssetHandle(pSoundStream, btEVENT);
		//	SoundBufferLoaded();
		//	break;
		//}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CSoundBuffer::WaitForLoad()
{
	if (!Loading())
		return true;
	m_pReadStream->Wait();
	return !LoadFailure();
}

//////////////////////////////////////////////////////////////////////////
void CSoundBuffer::AbortLoading()
{
	if (m_pReadStream != NULL)
	{
		m_pReadStream->Abort();
		m_pReadStream = NULL;
		LoadFailed();
	}
}

//////////////////////////////////////////////////////////////////////////
void CSoundBuffer::OnSoundEvent( ESoundCallbackEvent event,ISound *pSound )
{
	switch (event) {
	case SOUND_EVENT_ON_STOP:
		{
			if (pSound)
			{
				pSound->RemoveEventListener(this);
			}
			break;
		}
	case SOUND_EVENT_ON_PLAYBACK_STOPPED:
		{
			if (pSound)
			{
				uint32 nFlags = GetProps()->nPrecacheFlags;
				if (nFlags & FLAG_SOUND_PRECACHE_UNLOAD_AFTER_PLAY)
				{
					nFlags &= ~FLAG_SOUND_PRECACHE_STAY_IN_MEMORY;
					nFlags |= FLAG_SOUND_PRECACHE_UNLOAD_NOW;
					GetProps()->nPrecacheFlags = nFlags;

					UpdateTimer(gEnv->pTimer->GetFrameStartTime());
				}
			}
			break;

		}
	case SOUND_EVENT_ON_LOAD_FAILED:
		{
			if (pSound)
				pSound->RemoveEventListener(this);

			break;
		}		

	}
}

void CSoundBuffer::GetMemoryUsage( ICrySizer *pSizer ) const
{	
	pSizer->AddObject(m_StreamBuffer,m_nStreamBufferSize);		
	pSizer->AddObject(m_vecLoadReq);		
	pSizer->AddObject(m_vecLoadReqToRemove);		
	pSizer->AddObject(m_vecLoadReqToAdd);		
	pSizer->AddObject(m_ProtectedSounds);				
}