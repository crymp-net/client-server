////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   SoundBufferFmodEx400Event.cpp
//  Version:     v1.00
//  Created:     28/07/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: FMODEX 4.00 Implementation of Event SoundBuffer
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#ifdef SOUNDSYSTEM_USE_FMODEX400

#include "CryCommon/CrySystem/ITimer.h"
#include "SoundBufferFmodEx400Event.h"
#include "AudioDeviceFmodEx400.h"
//#include "SoundSystem.h"
#include "Sound.h"
#include "FmodEx/inc/fmod_errors.h"
#include "CryCommon/CryCore/CrySizer.h"

//////////////////////////////////////////////////////////////////////////
#define IS_FMODERROR (m_ExResult != FMOD_OK )

CSoundBufferFmodEx400Event::CSoundBufferFmodEx400Event(const SSoundBufferProps &Props, FMOD::System* pCSEX) : 
CSoundBuffer(Props)
{
	m_sGroupName = Props.sUserDataText;
	m_pCSEX = pCSEX;
	m_ExResult = FMOD_OK;
	m_Props.nFlags = Props.nFlags;
	m_Props.nPrecacheFlags = Props.nPrecacheFlags;
}

CSoundBufferFmodEx400Event::~CSoundBufferFmodEx400Event()
{
	//if (m_pReadStream != NULL)
	//{
	//	m_pReadStream->Abort();	
	//	m_pReadStream = NULL;
	//}
	UnloadData(sbUNLOAD_ALL_DATA_NOW);

	// Tell Sounds that their soundbuffers are invalid now
	for (TBufferLoadReqVecIt It=m_vecLoadReq.begin(); It!=m_vecLoadReq.end(); ++It) 
		(*It)->OnBufferDelete(); 
}

void CSoundBufferFmodEx400Event::FmodErrorOutput(const char * sDescription, ESoundLogType LogType)
{
	gEnv->pSoundSystem->Log(LogType, "<Sound> SBE: (%s) %s \n", m_Props.sName.c_str(), sDescription);
	gEnv->pSoundSystem->Log(LogType, "<Sound> SBE: (%d) %s\n", m_ExResult, FMOD_ErrorString(m_ExResult));
}

uint32 CSoundBufferFmodEx400Event::GetMemoryUsed()
{
	size_t nSize = sizeof(*this);

	if (!m_BufferData)
		return nSize;

	FMOD::EventGroup *pEventGroup = (FMOD::EventGroup*)m_BufferData;

	int nMemUsed = 0;
	int nNumEvents = 0;
	m_ExResult = pEventGroup->getNumEvents(&nNumEvents);
	if (IS_FMODERROR)
	{
		FmodErrorOutput("get group numevents failed! ", eSLT_Warning);
		return nSize;
	}

	unsigned int memoryused = 0;
	unsigned int nMemoryUsedArray[FMOD_MEMTYPE_MAX] = {0};

	m_ExResult = pEventGroup->getMemoryInfo(FMOD_MEMBITS_SOUND, 0, &memoryused, nMemoryUsedArray);

	nMemUsed += memoryused;

	// on querying sound data from an event group, we dont have iterate through all events and collect individual
	// size, as this would double the result. I leave the code here for times when structural memory usage is tracked
	/*
	for (int i=0; i<nNumEvents; ++i)
	{
		FMOD::Event *pEvent = 0;
		m_ExResult = pEventGroup->getEventByIndex(i, FMOD_EVENT_INFOONLY, &pEvent);

		if (IS_FMODERROR)
		{
			FmodErrorOutput("get event by index failed! ", eSLT_Warning);
			return nSize;
		}

		unsigned int eventmemoryused = 0;

		m_ExResult = pEvent->getMemoryInfo(FMOD_MEMBITS_SOUND, 0, &eventmemoryused, 0);

		if (IS_FMODERROR)
		{
			FmodErrorOutput("get event memory info failed! ", eSLT_Warning);
			return nSize;
		}

		nMemUsed += eventmemoryused;
	}
	*/
	

	return nMemUsed;
}

// compute memory-consumption, returns size in Bytes
void CSoundBufferFmodEx400Event::GetMemoryUsage(class ICrySizer* pSizer) const
{
	pSizer->AddObject( this, sizeof(*this) );	
	CSoundBuffer::GetMemoryUsage(pSizer);
}

void CSoundBufferFmodEx400Event::LogDependencies()
{
	if (!m_BufferData)
		return;

	FMOD::EventGroup *pEventGroup = (FMOD::EventGroup*)m_BufferData;
	int nNumEvents = 0;

	m_ExResult = pEventGroup->getNumEvents(&nNumEvents);

	if (IS_FMODERROR)
	{
		FmodErrorOutput("create group num. events failed! ", eSLT_Warning);
		return;
	}

	for (int i=0; i<nNumEvents; ++i)
	{
		FMOD::Event *pEvent = 0;
		{
			CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_BUFFER_OTHER) );
			m_ExResult = pEventGroup->getEventByIndex(i, FMOD_EVENT_INFOONLY, &pEvent);
		}

		if (IS_FMODERROR)
		{
			FmodErrorOutput("get event by index failed! ", eSLT_Warning);
			break;
		}

		FMOD_EVENT_INFO EventInfo;


		// first get number of wavebanks
		m_ExResult = pEvent->getInfo(0, 0, &EventInfo);

		if (IS_FMODERROR)
		{
			FmodErrorOutput("event system get event info failed! ", eSLT_Warning);
			return;
		}

		FMOD_EVENT_WAVEBANKINFO* pWavebankInfo = NULL;

		if (EventInfo.maxwavebanks)
		{
			// now repeat to fill the array of wavebank infos
			pWavebankInfo = new FMOD_EVENT_WAVEBANKINFO[EventInfo.maxwavebanks];

			if (!pWavebankInfo)
				return;

			EventInfo.wavebankinfo = pWavebankInfo;

			m_ExResult = pEvent->getInfo(0, 0, &EventInfo);

			if (IS_FMODERROR)
			{
				FmodErrorOutput("event system get event info with wavebanks failed! ", eSLT_Warning);
				delete[] pWavebankInfo; // remove wavebankinfo array
				return;
			}
		}

		for (int nBankIndex=0; nBankIndex<EventInfo.maxwavebanks; ++nBankIndex)
		{
			// Compose full File name to Wavebank
			TFixedResourceName sWavebankName;
			TFixedResourceName sFilePath = m_Props.sName;
			const char *pInput = sFilePath;
			const char *pColon = strstr(pInput, ":");

			if (pInput && pColon)
				{
					sFilePath.assign(sFilePath.substr(0, pColon - pInput));
					sFilePath += "/";

					sWavebankName.assign(pWavebankInfo[nBankIndex].name, strlen(pWavebankInfo[nBankIndex].name));
					sWavebankName += ".fsb";

					sWavebankName = sFilePath + sWavebankName;

					if (m_pSoundSystem->g_nDebugSound > 1)
						CryLog(" <Dependency> %s ", sWavebankName.c_str());

					FILE* pFile = gEnv->pCryPak->FOpen(sWavebankName.c_str(), "rbx");

					if (pFile)
						gEnv->pCryPak->FClose(pFile);

				}
		}

		delete[] pWavebankInfo; // remove wavebankinfo array
	}
}


// loads a event sound
tAssetHandle CSoundBufferFmodEx400Event::LoadAsEvent(const char *AssetName)
{
	FUNCTION_PROFILER( gEnv->pSystem,PROFILE_SOUND );

	CAudioDeviceFmodEx400 *pAudioDeviceFmodEx400 =  (CAudioDeviceFmodEx400*)m_pSoundSystem->GetIAudioDevice();
	FMOD::EventSystem *pEventSystem = (FMOD::EventSystem*)pAudioDeviceFmodEx400->GetEventSystem();
	FMOD::EventGroup *pEventGroup = 0;

	assert(m_Props.nProjectIndex != PROJECTINDEX_INVALID);

	if (m_Props.nProjectIndex != PROJECTINDEX_INVALID && pEventSystem)
	{
		bool bLoadGroup = false;
		if (m_Props.nPrecacheFlags & FLAG_SOUND_PRECACHE_LOAD_GROUP)
			bLoadGroup = true;
		else 
			bLoadGroup = false;

		//bPreload = true;
		bool bCacheMemory = false;
		
		// try to fix bug
		//bLoadGroup = true;
		
		SProjectFile* pProjectFile = m_pSoundSystem->GetIAudioDevice()->GetProjectFile(m_Props.nProjectIndex);
		
		if (!pProjectFile || pProjectFile->pProjectHandle == PROJECTHANDLE_INVALID)
		{
			// Project is not ready
			return NULL;
		}
		
		FMOD::EventProject* pProject = (FMOD::EventProject*)pProjectFile->pProjectHandle;

		if (m_pSoundSystem->g_nProfiling > 1)
		{
			float f0 = gEnv->pTimer->GetAsyncCurTime();
			{
				CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_BUFFER_OTHER) );
				m_ExResult = pProject->getGroup(m_sGroupName.c_str(), bCacheMemory, &pEventGroup);
			}

			float f1 = gEnv->pTimer->GetAsyncCurTime();
			CryLog("<Sound Profile Getting Group> %s %.2f", m_Props.sName.c_str(), (f1-f0)*1000.0f);
		}
		else
		{
			CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_BUFFER_OTHER) );
			m_ExResult = pProject->getGroup(m_sGroupName.c_str(), bCacheMemory, &pEventGroup);
		}

		if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
			pAudioDeviceFmodEx400->m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::PROJECT_GETGROUP, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*) pProject, (void*) m_sGroupName.c_str(), (void*)bCacheMemory, (void*)pEventGroup);

		// precache flag has the same result as using pEventGroup->LoadEventData(EVENT_RESOURCE_STREAMS_AND_SAMPLES);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("create event group failed! ", eSLT_Warning);
			return (NULL);
		}

		int nNumEvents = 0;
		m_ExResult = pEventGroup->getNumEvents(&nNumEvents);

		m_EventTimeouts.clear();
		m_EventTimeouts.resize(nNumEvents);

		if (bLoadGroup)
		{
			m_ExResult = pEventGroup->loadEventData(FMOD_EVENT_RESOURCE_SAMPLES, FMOD_EVENT_DEFAULT);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("event group loading failed! ", eSLT_Warning);
				return (NULL);
			}
		}
			//m_ExResult = pEventGroup->loadEventData(FMOD::EVENT_RESOURCE_SAMPLES);
			
		//else
			//m_ExResult = pEventGroup->loadEventData(FMOD::EVENT_RESOURCE_STREAMS);

		//else gEnv->pLog->Log("Error: Sound-FmodEx-SoundBuffer-Event: %s %s \n", sEventName, "created succesfully.");

// debugging getInfo()
		//int nIndex = 0;
		//char *sName = 0; 
		//pEventGroup->getInfo( &nIndex,&sName );


		// Project Refcounting
		++pProjectFile->nRefCount;
	}

	return (pEventGroup);
}

// Gets and Sets Parameter defined in the enumAssetParam list
//////////////////////////////////////////////////////////////////////////
bool CSoundBufferFmodEx400Event::GetParam(enumAssetParamSemantics eSemantics, ptParam* pParam) const
{
	switch (eSemantics)
	{	
	case apASSETNAME:	
		{
			string sTemp;
			if (!(pParam->GetValue(sTemp))) 
				return (false);
			sTemp = m_Props.sName.c_str();
			pParam->SetValue(sTemp);
		}
		break;
	case apASSETTYPE:	
		{
			int32 nTemp = 0;
			if (!(pParam->GetValue(nTemp))) 
				return (false);
			nTemp = m_Props.eBufferType;
			pParam->SetValue(nTemp);
			break;
		}	
	case apNUMSIMPLESOUNDS:
		{
			int32 nTemp = 0;
			if (!(pParam->GetValue(nTemp))) 
				return (false);

			if (!m_BufferData)
				return false;

			FMOD::EventGroup *pEventGroup = (FMOD::EventGroup*)m_BufferData;
			int nNumEvents = 0;
			int nNumSimple = 0;

			FMOD_RESULT	ExResult = pEventGroup->getNumEvents(&nNumEvents);

			if (ExResult != FMOD_OK)
			{
				//FmodErrorOutput("count simple events failed! ", eSLT_Warning);
				return false;
			}

			for (int i=0; i<nNumEvents; ++i)
			{
				FMOD::Event *pEvent = 0;
				ExResult = pEventGroup->getEventByIndex(i, FMOD_EVENT_INFOONLY, &pEvent);

				if (ExResult != FMOD_OK)
				{
					//FmodErrorOutput("get info event by index failed! ", eSLT_Warning);
					return false;
				}

				int nType = 0;
				ExResult = pEvent->getPropertyByIndex(FMOD_EVENTPROPERTY_EVENTTYPE, &nType);
				if (ExResult != FMOD_OK)
				{
					//FmodErrorOutput("get event property <IsSimple/EventType> failed! ", eSLT_Message);
					return false;
				}

				// this is a simple event
				if (nType == 0)
					++nNumSimple;
			}

			pParam->SetValue(nNumSimple);
		}
	//case apASSETFREQUENCY:	
	//	{
	//		int32 nTemp = 0;
	//		if (!(pParam->GetValue(nTemp))) 
	//			return (false);
	//			nTemp = m_Info.nBaseFreq;
	//			pParam->SetValue(nTemp);
	//		break;
	//	}
	//case apLENGTHINBYTES:	
	//	{
	//		int32 nTemp = 0;
	//		if (!(pParam->GetValue(nTemp))) 
	//			return (false);
	//		switch (m_Props.eBufferType)
	//		{
	//		case btSAMPLE:
	//			nTemp = m_Info.nLengthInBytes;
	//			pParam->SetValue(nTemp);
	//			break;
	//		case btSTREAM: // Stream does not support Byte Length
	//			nTemp = m_Info.nLengthInBytes;
	//			pParam->SetValue(nTemp);
	//			break;
	//		default:
	//			return (false);
	//		}
	//		break;
	//	}
	//case apLENGTHINMS:	
	//	{
	//		int32 nTemp = 1; // better not return 0
	//		if (!(pParam->GetValue(nTemp))) 
	//			return (false);
	//		nTemp = m_Info.nLengthInMs;
	//		pParam->SetValue(nTemp);
	//		break;
	//	}
	//case apLENGTHINSAMPLES:	
	//	{
	//		int32 nTemp = 0;
	//		if (!(pParam->GetValue(nTemp))) 
	//			return (false);
	//		nTemp = m_Info.nLengthInSamples;
	//		pParam->SetValue(nTemp);
	//		break;
	//	}	
	//case apBYTEPOSITION:	
	//	{
	//		int32 nTemp = 0;
	//		switch (m_Props.eBufferType)
	//		{
	//		case btSAMPLE:
	//			return (false);
	//			break;
	//		case btSTREAM: 
	//			if (!(pParam->GetValue(nTemp))) 
	//				return (false);
	//			//nTemp = CS_Stream_GetPosition((CS_STREAM*)m_BufferData);
	//			pParam->SetValue(nTemp);
	//			break;
	//		default:
	//			return (false);
	//		}
	//		return (true);
	//	}	
	//case apTIMEPOSITION:	
	//	{
	//		int32 nTemp = 0;
	//		switch (m_Props.eBufferType)
	//		{
	//		case btSAMPLE:
	//			return (false);
	//			break;
	//		case btSTREAM: 
	//			if (!(pParam->GetValue(nTemp))) 
	//				return (false);
	//			//nTemp = CS_Stream_GetTime((CS_STREAM*)m_BufferData);
	//			pParam->SetValue(nTemp);
	//			break;
	//		default:
	//			return (false);
	//		}
	//		return (true);
	//	}	
	case apLOADINGSTATE:	
		{
			int32 nTemp = 0;
			if (!(pParam->GetValue(nTemp))) 
				return (false);
			nTemp = m_Info.bLoadFailure;
			pParam->SetValue(nTemp);
			break;
		}
	case 	apNUMREFERENCEDSOUNDS:
		{
			int32 nTemp = 0;
			if (!(pParam->GetValue(nTemp))) 
				return (false);
			nTemp = m_vecLoadReq.size();
			pParam->SetValue(nTemp);
		}
	default:	
    return (false);
		break;
	}
	return (true);
}

//////////////////////////////////////////////////////////////////////////
bool CSoundBufferFmodEx400Event::SetParam(enumAssetParamSemantics eSemantics, ptParam* pParam)
{
	switch (eSemantics)
	{
	case apASSETNAME:	// TODO Review this, Reset the Asset?
		return (false); // Setting this parameter not supported
		break;
	case apASSETTYPE:	// TODO Review this
		return (false); // Setting this parameter not supported
		break;
	//case apASSETFREQUENCY:	
	//	{
	//		int32 nTemp;
	//		if (!(pParam->GetValue(nTemp))) return (false);
	//		switch (m_Props.eBufferType)
	//		{
	//		case btSAMPLE:
	//			//if (!CS_Sample_SetDefaults((CS_SAMPLE*)m_BufferData, nTemp, NULL, NULL, NULL)) 
	//			return (false);
	//			//m_Info.nBaseFreq = nTemp;
	//			break;
	//		case btSTREAM: // Stream does not support Frequency change
	//			return (false);
	//			break;
	//		default:
	//			return (false);
	//			break;
	//		}
	//	}
	//	break;
	//case apLENGTHINBYTES:		
	//	return (false); // Setting this parameter not supported		break;
	//case apLENGTHINMS:	
	//	return (false); // Setting this parameter not supported		break;
	//case apBYTEPOSITION:	
	//	{
	//		int32 nTemp;
	//		if (!(pParam->GetValue(nTemp))) return (false);
	//		switch (m_Props.eBufferType)
	//		{
	//		case btSAMPLE:
	//			return (false); // Setting this parameter not supported
	//			break;
	//		case btSTREAM:
	//			//if (!CS_Stream_SetPosition((CS_STREAM*)m_BufferData, nTemp))
	//			return (false);
	//			break;
	//		default:
	//			return (false);
	//			break;
	//		}
	//	}
	//	break;
	//case apTIMEPOSITION:	// TODO Review if seconds (float) or milliseconds (int) should be used
	//	{
	//		int32 nTemp;
	//		if (!(pParam->GetValue(nTemp))) 
	//			return (false);
	//		switch (m_Props.eBufferType)
	//		{
	//		case btSAMPLE:
	//			return (false); // Setting this parameter not supported
	//			break;
	//		case btSTREAM: 
	//			//if (!CS_Stream_SetTime((CS_STREAM*)m_BufferData, nTemp))
	//			return (false);
	//			break;
	//		default:
	//			return (false);
	//			break;
	//		}
	//	}
	//	break;
	case apLOADINGSTATE:		
		return (false); // Setting this parameter not supported		
		break;
	default:	
		return (false);
		break;
	}

}

//////////////////////////////////////////////////////////////////////////
bool CSoundBufferFmodEx400Event::UnloadData(const eUnloadDataOptions UnloadOption)
{
	//GUARD_HEAP;
	bool bResult = true;

	CAudioDeviceFmodEx400 *pAudioDeviceFmodEx400 =  (CAudioDeviceFmodEx400*)m_pSoundSystem->GetIAudioDevice();

	if (m_BufferData && (UnloadOption == sbUNLOAD_ALL_DATA || UnloadOption == sbUNLOAD_ALL_DATA_NOW))
	{
		//FUNCTION_PROFILER( m_pSoundSystem->GetSystem(),PROFILE_SOUND );

		// store processed sounds in a container to prevent racing condition and to delay the release() of the sound
		m_ProtectedSounds.reserve(m_vecLoadReq.size() + m_vecLoadReqToAdd.size());

		// Tell Sounds that their soundbuffers are unloaded now
		for (TBufferLoadReqVecIt It=m_vecLoadReq.begin(); It!=m_vecLoadReq.end(); ++It) 
		{
			CSound* pSound = (*It);
			m_ProtectedSounds.push_back(pSound);
			(*It)->OnAssetUnloaded(); 
		}
		for (TBufferLoadReqVecIt It=m_vecLoadReqToAdd.begin(); It!=m_vecLoadReqToAdd.end(); ++It) 
		{
			CSound* pSound = (*It);
			m_ProtectedSounds.push_back(pSound);
			(*It)->OnAssetUnloaded(); 
		}

		if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_DETAIL)
		{
			m_pSoundSystem->Log(eSLT_Message, "<Sound> Destroying sound buffer %s \n", m_Props.sName.c_str());		
		}

		FMOD::EventGroup *pEventGroup = (FMOD::EventGroup*)m_BufferData;

		if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
			pAudioDeviceFmodEx400->m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::GROUP_FREEDATA, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*) pEventGroup);

		{
			CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_BUFFER_OTHER) );
			m_ExResult = pEventGroup->freeEventData(0, (UnloadOption == sbUNLOAD_ALL_DATA_NOW));
		}
		if (IS_FMODERROR)
		{
			if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_DETAIL)
				m_pSoundSystem->Log(eSLT_Message, "<Sound> Couldn't free event group data! %s \n", m_Props.sName.c_str());

			// FMOD_ERR_NOTREADY might happen if FMOD could not get critical section, we need to try later
			if (m_ExResult != FMOD_ERR_NOTREADY)
			 FmodErrorOutput("release event group failed! ", eSLT_Warning);

			return false;
		}

		CSoundSystem::g_nBuffersTrashed++;

		if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_DETAIL)
			m_pSoundSystem->Log(eSLT_Message, "<Sound> Freeing event group data! %s \n", m_Props.sName.c_str());
		//else gEnv->pLog->Log("Error: Sound-FmodEx-SoundBuffer-Event: %s %s \n", m_Props.sName.c_str(), "released succesfully.");

		// Project Refcounting
		SProjectFile* pProjectFile = m_pSoundSystem->GetIAudioDevice()->GetProjectFile(m_Props.nProjectIndex);

		if (pProjectFile)
		{
			assert (pProjectFile->nRefCount > 0);
			--pProjectFile->nRefCount;
		}

		switch (m_Props.eBufferType)
		{
		case btSAMPLE:	
			SetAssetHandle(NULL, btSAMPLE);	
			break;
		case btSTREAM:	
			SetAssetHandle(NULL, btSTREAM);	
			break;
		case btEVENT:	
			SetAssetHandle(NULL, btEVENT);	
			break;
		default:
			{
				SetAssetHandle(NULL, btNONE);	
				break;
			}
		}

		m_ProtectedSounds.resize(0);
	}

	bool bAllEventsUnloaded = true;

	if (m_BufferData && (UnloadOption == sbUNLOAD_UNNEEDED_DATA || UnloadOption == sbUNLOAD_UNNEEDED_DATA_NOW))
	{
		FMOD::EventGroup *pEventGroup = (FMOD::EventGroup*)m_BufferData;
		CTimeValue tNow = gEnv->pTimer->GetFrameStartTime();

		for (uint32 i=0; i<m_EventTimeouts.size(); ++i)
		{
			FMOD::Event *pEvent = 0;
			{
				CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_BUFFER_OTHER) );
				m_ExResult = pEventGroup->getEventByIndex(i, FMOD_EVENT_INFOONLY, &pEvent);
			}
			
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get event by index failed! ", eSLT_Warning);
				bResult = false;
			}

			FMOD_EVENT_INFO EventInfo;

			{
				CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_BUFFER_OTHER) );
				m_ExResult = pEvent->getInfo(0,0,&EventInfo);
			}
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get event info failed! ", eSLT_Warning);
				bResult = false;
			}

			if (EventInfo.instancesactive == 0)
			{
				CTimeValue tDiff = tNow - m_EventTimeouts[i];
				int nSeconds = (int)tDiff.GetSeconds();
				if (nSeconds > 15)
				{
					FMOD_EVENT_STATE EventState;
					{
						CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_BUFFER_OTHER) );
						m_ExResult = pEvent->getState(&EventState);
					}
					if (IS_FMODERROR)
					{
						FmodErrorOutput("get event state failed! ", eSLT_Warning);
						return false;
					}

					if (!(EventState & FMOD_EVENT_STATE_LOADING))
					{
						if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
							pAudioDeviceFmodEx400->m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::GROUP_FREEEVENTDATA, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*) pEventGroup, (void*) i);

						{
							CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_BUFFER_RELEASE) );
							m_ExResult = pEventGroup->freeEventData(pEvent, (UnloadOption == sbUNLOAD_UNNEEDED_DATA_NOW));
						}
						if (IS_FMODERROR)
						{
							// FMOD_ERR_NOTREADY might happen if FMOD could not get critical section, we need to try later
							if (m_ExResult != FMOD_ERR_NOTREADY)
								FmodErrorOutput("release event group failed! ", eSLT_Warning);

							return false;
						}
						
						CSoundSystem::g_nBuffersTrashed++;

						m_EventTimeouts[i] = tNow;
					}
				}
			}
			else
			{
				m_EventTimeouts[i] = tNow;
				bAllEventsUnloaded = false;
			}
		}
		if (bAllEventsUnloaded)
			UnloadData(sbUNLOAD_ALL_DATA);
	}

	return bResult;
}

bool CSoundBufferFmodEx400Event::NotLoaded() const		
{
	SProjectFile* pProjectFile = m_pSoundSystem->GetIAudioDevice()->GetProjectFile(m_Props.nProjectIndex);

	if (!pProjectFile 
		|| (pProjectFile->pProjectHandle == PROJECTHANDLE_INVALID && pProjectFile->pReadStream == NULL)
		|| (m_BufferData == NULL && m_pReadStream == NULL))
		return true;
	else
		return false;
}

bool CSoundBufferFmodEx400Event::Loaded() const				
{
	SProjectFile* pProjectFile = m_pSoundSystem->GetIAudioDevice()->GetProjectFile(m_Props.nProjectIndex);

	if (pProjectFile && (pProjectFile->pProjectHandle != PROJECTHANDLE_INVALID) 
		&& (pProjectFile->pReadStream == NULL)
		&& (m_BufferData != NULL)
		&& (m_pReadStream == NULL))
		return true;
	else
		return false;
}

bool CSoundBufferFmodEx400Event::Loading() const			
{ 
	SProjectFile* pProjectFile = m_pSoundSystem->GetIAudioDevice()->GetProjectFile(m_Props.nProjectIndex);

	if ((pProjectFile && (pProjectFile->pProjectHandle == PROJECTHANDLE_INVALID) && (pProjectFile->pReadStream != NULL))
		|| m_pReadStream)
		return true;
	else
		return false;
}

bool CSoundBufferFmodEx400Event::LoadFailure() const	
{ 
	SProjectFile* pProjectFile = m_pSoundSystem->GetIAudioDevice()->GetProjectFile(m_Props.nProjectIndex);

	if (!pProjectFile 
		|| (pProjectFile->pProjectHandle == PROJECTHANDLE_INVALID && pProjectFile->pReadStream == NULL)
		|| m_Info.bLoadFailure)
		return true;
	else
		return false;
}



#endif
