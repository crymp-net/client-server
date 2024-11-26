////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   PlatformSoundFmodEx400Event.cpp
//  Version:     v1.00
//  Created:     28/07/2005 by Tomas
//  Compilers:   Visual Studio.NET
//  Description: FmodEx 4.00 Implementation of a platform dependent Sound Event
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#ifdef SOUNDSYSTEM_USE_FMODEX400

#include "PlatformSoundFmodEx400Event.h"
#include "SoundBufferFmodEx400.h"
#include "AudioDeviceFmodEx400.h"
#include "Sound.h"
#include <CrySizer.h>
#include "SoundSystem.h"
#include "FmodEx/inc/fmod_errors.h"
#include "ITimer.h"
#include <StlUtils.h>

//////////////////////////////////////////////////////////////////////////
#define IS_FMODERROR (m_ExResult != FMOD_OK )

FMOD_RESULT F_CALLBACK  CPlatformSoundFmodEx400Event::OnCallBack(FMOD_EVENT *  event, FMOD_EVENT_CALLBACKTYPE  type, void *  param1, 	void *  param2, void *  userdata)
{
	if (!CSound::m_pSSys)
		return FMOD_OK;

	tSoundID nSoundID = (tSoundID) userdata;
	_smart_ptr<CSound> pSound = (CSound*) CSound::m_pSSys->GetSound(nSoundID);

	CPlatformSoundFmodEx400Event *pPlatformSound = NULL;

	if (pSound)
		pPlatformSound = (CPlatformSoundFmodEx400Event*)pSound->GetPlatformSoundPtr();

	if (pPlatformSound)
	{
		switch(type) 
		{
		case FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_START:
			if (pPlatformSound->m_State == pssLOADING || pPlatformSound->m_State == pssREADY)
			{
				pPlatformSound->m_State = pssPLAYING;
				pSound->OnEvent(SOUND_EVENT_ON_PLAYBACK_STARTED);
			}
			break;
		case FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_END:
			break;
		case FMOD_EVENT_CALLBACKTYPE_SYNCPOINT:
			{
				if (CSound::m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_SYNCPOINTS)
				{
					int nPosInMS = pSound->GetCurrentSamplePos(true);
					CSound::m_pSSys->Log(eSLT_Message, "<Sound> --> Syncpoint arrived: %s, Time:%.2f, PosInMS:%d", pSound->GetName(), gEnv->pTimer->GetAsyncCurTime(), nPosInMS);
				}

				pSound->OnEvent(SOUND_EVENT_ON_SYNCHPOINT);
				if (pPlatformSound->m_State == pssWAITINGFORSYNC)
				{

					// try to pause this event as quickly as possible
					FMOD::Event* pEvent = (FMOD::Event*) pPlatformSound->GetSoundHandle();
					if (pEvent)
					{
						FMOD_RESULT ExResult = pEvent->setPaused(true);
					}

					// do it again through platform sound for integrity reasons
					bool bPause = true;
					ptParamBOOL TempParam(bPause);
					pPlatformSound->SetParamByType(pspPAUSEMODE, &TempParam);
					pPlatformSound->m_State = pssSYNCARRIVED;
					
					if (CSound::m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_SYNCPOINTS)
						CSound::m_pSSys->Log(eSLT_Message, "<Sound> --> Waited for Syncpoint: %s, Time:%.2f", pSound->GetName(), gEnv->pTimer->GetAsyncCurTime());
				}
				
				break;
			}
		case FMOD_EVENT_CALLBACKTYPE_NET_MODIFIED:
			{
				// reset Maxvolume that is used to normalize sound volume
				FMOD::Event*		pEvent = (FMOD::Event*) pPlatformSound->GetSoundHandle();
				if (pEvent)
				{
					FMOD_RESULT ExResult = pEvent->getVolume(&(pPlatformSound->m_fMaxVolume));
				}
				break;
			}
		case FMOD_EVENT_CALLBACKTYPE_EVENTFINISHED:
			{
				if (CSound::m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_PROGRAMMERSOUND && pSound->GetFlags() & FLAG_SOUND_VOICE)
					CSound::m_pSSys->Log(eSLT_Always, "----> SoundDef %s EVENT FINISHED", pSound->GetSoundBufferPtr()->GetProps()->sName.c_str());

				pPlatformSound->m_State = pssSTOPPED;

				break;
			}
		case FMOD_EVENT_CALLBACKTYPE_STOLEN:
			{

				pPlatformSound->m_State = pssSTOLEN;
				pPlatformSound->FreeSoundHandle();

				break;
			}
		case FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_CREATE:
			{
				if (pSound->GetSoundBufferPtr() && pSound->GetSoundBufferPtr()->GetProps()->eBufferType != btEVENT)
				{
					FMOD::Sound *sound    = (FMOD::Sound *) pSound->GetSoundBufferPtr()->GetAssetHandle();
					*((FMOD::Sound **)param2) = sound;
					
					if (CSound::m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_PROGRAMMERSOUND)
						CSound::m_pSSys->Log(eSLT_Always, "----> SoundDef %s CREATE +1", pSound->GetSoundBufferPtr()->GetProps()->sName.c_str());

					pSound->GetSoundBufferPtr()->AddRef(); // FMOD holds a reference to this Buffer now
				}
				else
					return FMOD_ERR_NOTREADY;
			
				break;
			}
		case FMOD_EVENT_CALLBACKTYPE_SOUNDDEF_RELEASE:
			{
				// triggering the radio end squelch, need fadeout time for that to work
				if (pSound->GetFlags() & FLAG_SOUND_PARAM_SQUELCH)
				{
					float fOldSquelch = 0.0f;
					pPlatformSound->GetParamByName("squelch", &fOldSquelch, false);
					pPlatformSound->SetParamByName("squelch", 0.0f, false);
					pPlatformSound->SetParamByName("squelch", fOldSquelch, false);
				}

				pSound->SetState(eSoundState_ProgrammerSoundReleased);
				pSound->GetSoundBufferPtr()->Release(); // FMOD does not hold a reference to this Buffer anymore

				if (CSound::m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_PROGRAMMERSOUND)
					CSound::m_pSSys->Log(eSLT_Always, "----> SoundDef %s RELEASE -1", pSound->GetSoundBufferPtr()->GetProps()->sName.c_str());

				break;
			}
		default:
			break;
		}
	}

	return FMOD_OK;
}


//////////////////////////////////////////////////////////////////////////
CPlatformSoundFmodEx400Event::CPlatformSoundFmodEx400Event(CSound*	pSound, FMOD::System* pCSEX, const char *sEventName)
{
	assert(pCSEX);
	m_pCSEX				= pCSEX;
	m_pAudioDevice = (CAudioDeviceFmodEx400*)m_pSound->GetSoundSystemPtr()->GetIAudioDevice();
	Reset(pSound, sEventName);
}

//////////////////////////////////////////////////////////////////////////
CPlatformSoundFmodEx400Event::~CPlatformSoundFmodEx400Event(void)
{
	FreeSoundHandle();		
}

void CPlatformSoundFmodEx400Event::Reset(CSound*	pSound, const char *sEventName)
{	
	assert(pSound);
	m_pSound			= pSound;
	m_pEvent			= NULL;
	m_nRefCount		= 0;
	m_ExResult		= FMOD_OK;
	m_fMaxRadius	= 0.0f;
	m_fMaxVolume  = -1.0f;
	m_sEventName  = sEventName;
	m_State       = pssNONE;
	m_nRefCount		= 0;
	m_nLenghtInMs	= 0;
	m_nMaxSpawns = 0;
	m_nDistanceIndex = -1;
	m_nCurrentSpawns = 0;
	m_QueuedParameter.clear();
	
	//prevent some runtime allocations by reserving space for at least 15 parameters
	int nNum = max(25, CSound::m_pSSys->m_ParameterNames.size());
	m_ParameterIndex.resize(nNum);

	// make sure to invalid all entries
	for (int i=0; i<nNum; ++i)
		m_ParameterIndex[i].nEventIndex = -1;
}

//////////////////////////////////////////////////////////////////////////
int32 CPlatformSoundFmodEx400Event::AddRef()
{
	return (++m_nRefCount);
}

//////////////////////////////////////////////////////////////////////////
int32 CPlatformSoundFmodEx400Event::Release()
{
	--m_nRefCount;
	if (m_nRefCount <= 0)
	{
		m_pAudioDevice->RemovePlatformSound(this);
		//delete this;
		return (0);
		
	}
	return m_nRefCount;
}


void CPlatformSoundFmodEx400Event::FmodErrorOutput(const char * sDescription, ESoundLogType LogType)
{
	if (m_ExResult == FMOD_ERR_INVALID_HANDLE)
	{
		if (CSound::m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_DETAIL)
		{
			TFixedResourceName sTemp = "Invalid Handle - Playback maximum reached on ";
			sTemp += m_pSound->GetName();
			sTemp += "?";

			gEnv->pSoundSystem->Log(eSLT_Warning, "<Sound> PSE: %s \n", sTemp.c_str() );
		}

		m_pEvent = NULL;
		m_State = pssERROR;
		m_pSound->Update();
		return;
	}

	gEnv->pSoundSystem->Log(LogType, "<Sound> PSE: %s %s\n", m_pSound->GetName(), sDescription, m_ExResult);
	gEnv->pSoundSystem->Log(LogType, "<Sound> PSE: (%d) %s\n", m_ExResult, FMOD_ErrorString(m_ExResult));
}

//////////////////////////////////////////////////////////////////////////
tSoundHandle CPlatformSoundFmodEx400Event::GetSoundHandle() const
{ 
	//if (m_pExChannel == NULL) 
		//return NULL;
	return ((tSoundHandle)m_pEvent); 
}

//////////////////////////////////////////////////////////////////////////
bool CPlatformSoundFmodEx400Event::Set3DPosition(Vec3* pvPosition, Vec3* pvVelocity, Vec3* pvOrientation)
{
	FMOD_VECTOR vExPos;
	FMOD_VECTOR vExVel;
	FMOD_VECTOR vExOrient;

	CSoundSystem* pSys = m_pSound->GetSoundSystemPtr();
	if (!pSys) return (false);

	//SListener *pListener = NULL;
	// axis switch
	if (pvPosition)
	{
		vExPos.x = pvPosition->x;
		vExPos.y = pvPosition->z;
		vExPos.z = pvPosition->y;
	}

	if (pvVelocity)
	{
		CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );

		// Doppler Value here does not have to be multiplied with the DopplerScale of the event
		// because FMOD will do that internally anyway
		
		vExVel.x = clamp(pvVelocity->x, -200.0f, 200.0f);
		vExVel.y = clamp(pvVelocity->z, -200.0f, 200.0f);
		vExVel.z = clamp(pvVelocity->y, -200.0f, 200.0f);
	}

	if (pvOrientation)
	{
		vExOrient.x = pvOrientation->x;
		vExOrient.y = pvOrientation->z;
		vExOrient.z = pvOrientation->y;
	}

	if (m_pEvent)
	{
		{		
			if (pSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
				m_pAudioDevice->m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::EVENT_SET3DATTRIBUTES, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*) m_pEvent, (void*) &vExPos, (void*) &vExVel);

			CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );
			m_ExResult = m_pEvent->set3DAttributes((pvPosition ? &vExPos : NULL), (pvVelocity ? &vExVel : NULL), (pvOrientation ? &vExOrient : NULL));
		}
		if (IS_FMODERROR)
		{
			FmodErrorOutput("set 3d event position failed! ", eSLT_Message);
			return false;
		}
	}
	return (true);
}

//////////////////////////////////////////////////////////////////////////
void CPlatformSoundFmodEx400Event::SetObstruction(const SObstruction *pObstruction)
{
	assert (pObstruction);

	if (m_pEvent) // && m_State != pssINFOONLY)
	{
		float fDirect = pObstruction->GetDirect();
		float fReverb = pObstruction->GetReverb();
		
		assert(fDirect <= 1.0f && fDirect >= 0.0f);
		assert(fReverb <= 1.0f && fReverb >= 0.0f);

		{
			CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );
			m_ExResult = m_pEvent->set3DOcclusion(fDirect, fReverb);
		}
		if (IS_FMODERROR)
		{
			//FmodErrorOutput("set set3DOcclusion failed! ", eSLT_Message);
			return;
		}
	}
}

// Gets and Sets Parameter defined in the enumSoundParam list
//////////////////////////////////////////////////////////////////////////
bool CPlatformSoundFmodEx400Event::GetParamByType(enumPlatformSoundParamSemantics eSemantics, ptParam* pParam)
{
	CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );

	if (eSemantics == pspLENGTHINMS)
	{
		pParam->SetValue(m_nLenghtInMs);
		return (true);
	}

	if (eSemantics == pspEVENTNAME)
	{
		pParam->SetValue(m_sEventName);
		return (true);
	}

	if (m_pEvent) 
	{
		switch (eSemantics)
		{
		case pspNONE:
			break;
		case pspSOUNDTYPE:
			break;
		case pspSAMPLETYPE:
			break;
		case pspFREQUENCY:
			{
				int32 nTemp;
				if (!(pParam->GetValue(nTemp))) 
					return (false);
				//nTemp = CS_GetFrequency(m_nChannel);
				pParam->SetValue(nTemp);
				break;
			}
		case pspVOLUME:
			{
				float fTemp;
				if (!(pParam->GetValue(fTemp))) 
					return (false);

				m_ExResult = m_pEvent->getVolume(&fTemp);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event property <volume> failed! ", eSLT_Message);
					return false;
				}
/*
				if (fTemp != 1.0f)
					int a = 12;
*/				
				// volume needs to be normalized with allowed MaxVolume (which is -1.0 the first time)
				fTemp *= abs(m_fMaxVolume);

				pParam->SetValue(fTemp);
				break;
			}
		case pspLOUDNESSINDB:
			{
				float fTemp = 60.0f;
				if (!(pParam->GetValue(fTemp))) 
					return (false);
				
				m_ExResult = m_pEvent->getProperty("loudness", &fTemp);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event property <loudness> failed! ", eSLT_Message);
					return false;
				}

				pParam->SetValue(fTemp);
				break;
			}
		case pspIS3D:
			{
				bool bTemp = false;
				if (!(pParam->GetValue(bTemp))) 
					return (false);
				FMOD_MODE Mode;
				m_ExResult = m_pEvent->getPropertyByIndex(FMOD_EVENTPROPERTY_MODE, &Mode);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event property <is3D/Mode> failed! ", eSLT_Message);
					return false;
				}
				if (Mode & FMOD_3D)
					bTemp = true;
				pParam->SetValue(bTemp);
				break;
			}
		case pspISPLAYING:
			{
				bool bTemp = false;
				if (!(pParam->GetValue(bTemp))) 
					return (false);

				FMOD_EVENT_STATE EventState;
				m_ExResult = m_pEvent->getState(&EventState);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event state failed! ", eSLT_Message);
					return false;
				}

				if (EventState & FMOD_EVENT_STATE_PLAYING || EventState & FMOD_EVENT_STATE_LOADING ) // EVENT_STATE_CHANNELSACTIVE
					bTemp = true;
				else
					bTemp = false;

				pParam->SetValue(bTemp);
				break;
			}
		case pspISSIMPLE:
			{
				bool bTemp = false;
				if (!(pParam->GetValue(bTemp))) 
					return (false);
				int nType;
				m_ExResult = m_pEvent->getPropertyByIndex(FMOD_EVENTPROPERTY_EVENTTYPE, &nType);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event property <IsSimple/EventType> failed! ", eSLT_Message);
					return false;
				}
				if (nType == 0)
					bTemp = true;
				pParam->SetValue(bTemp);
				break;

			}
		case pspISSTREAM:
			{
				bool bTemp = false;
				if (!(pParam->GetValue(bTemp))) 
					return (false);

				FMOD_EVENT_INFO Info;
				FMOD_EVENT_WAVEBANKINFO BankInfo = {{0},0};
				Info.maxwavebanks = 1;
				Info.wavebankinfo = &BankInfo;

				m_ExResult = m_pEvent->getInfo(0, 0, &Info);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event info <is stream> failed! ", eSLT_Message);
					return false;
				}

				// this test only evaluates the first wavebank
				bool bIsStream = (BankInfo.type == 0); // this is a streamed wavebank

				pParam->SetValue(bIsStream);
				break;

			}
		case pspLOOPMODE:
			{
				bool bTemp;
				if (!(pParam->GetValue(bTemp))) 
					return (false);

				int nOneShot = 0 ;
				m_ExResult = m_pEvent->getPropertyByIndex(FMOD_EVENTPROPERTY_ONESHOT, &nOneShot);

				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event property <oneshot> failed! ", eSLT_Message);
					return false;
				}

				bTemp = (nOneShot == 0);

				//bTemp = (Mode != FMOD_LOOP_OFF);
				pParam->SetValue(bTemp);
				break;
			}
		case pspPAUSEMODE:
			{
				bool bTemp;
				if (!(pParam->GetValue(bTemp))) 
					return (false);
				m_ExResult = m_pEvent->getPaused(&bTemp);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event <paused> failed! ", eSLT_Message);
					return false;
				}
				pParam->SetValue(bTemp);
				break;
			}
		case pspSPEAKERPAN:
			return (false);
			break;
		case pspSPEAKER3DPAN:
			return (false);
			break;
		case pspSAMPLEPOSITION:
			{
				// sample position is not supported by sound events
				return false;
				break;
			}
		case pspTIMEPOSITION:
			{
				int32 nTemp;
				if (!(pParam->GetValue(nTemp)))
					return (false);

				FMOD_EVENT_INFO Info;

				m_ExResult = m_pEvent->getInfo(0, 0, &Info);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event info <time pos> failed! ", eSLT_Message);
					return false;
				}

				pParam->SetValue(Info.positionms);
				break;
			}
		case psp3DPOSITION:
			{
				Vec3 VecTemp;
				if (!(pParam->GetValue(VecTemp))) 
					return (false);

				FMOD_VECTOR pos;
				m_ExResult = m_pEvent->get3DAttributes(&pos, 0);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event property <3d position> failed! ", eSLT_Message);
					return false;
				}

				VecTemp.x = pos.x;
				VecTemp.y = pos.z;
				VecTemp.z = pos.y;
				pParam->SetValue(VecTemp);
				break;
			}
		case psp3DVELOCITY:
			{
				Vec3 VecTemp;
				if (!(pParam->GetValue(VecTemp))) 
					return (false);

				FMOD_VECTOR vel;
				m_ExResult = m_pEvent->get3DAttributes(0, &vel);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event property <3d velocity> failed! ", eSLT_Message);
					return false;
				}

				VecTemp.x = vel.x;
				VecTemp.y = vel.z;
				VecTemp.z = vel.y;
				pParam->SetValue(VecTemp);
				break;
			}
		case psp3DDOPPLERSCALE:
			{
				float fTemp = 0.0f;
				if (!(pParam->GetValue(fTemp))) 
					return (false);

				m_ExResult = m_pEvent->getPropertyByIndex(FMOD_EVENTPROPERTY_3D_DOPPLERSCALE, &fTemp);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event property <doppler scale> failed! ", eSLT_Message);
					return false;
				}

				pParam->SetValue(fTemp);
				break;
			}
		case pspPRIORITY:
			{
				int32 nTemp;
				if (!(pParam->GetValue(nTemp))) 
					return (false);

				m_ExResult = m_pEvent->getPropertyByIndex(FMOD_EVENTPROPERTY_PRIORITY, &nTemp);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event property <priority> failed! ", eSLT_Message);
					return false;
				}

				pParam->SetValue(nTemp);
				break;
			}
		case pspLENGTHINMS:
			{
				int32 nTemp;
				if (!(pParam->GetValue(nTemp))) 
					return (false);

				FMOD_EVENT_INFO Info;

				m_ExResult = m_pEvent->getInfo(0, 0, &Info);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event info <length ms> failed! ", eSLT_Message);
					return false;
				}

				m_nLenghtInMs = Info.lengthms;

				if (Info.lengthms < 0)
					return false;

				pParam->SetValue(Info.lengthms);
				break;
			}
		case pspLENGTHINBYTES:
			{
				int32 nTemp;
				if (!(pParam->GetValue(nTemp))) 
					return (false);

				unsigned int nMemoryUsed = 0;

				m_ExResult = m_pEvent->getMemoryInfo(FMOD_MEMBITS_SOUND, 0, &nMemoryUsed, 0);

				if (IS_FMODERROR)
				{
					FmodErrorOutput("getMemoryInfo on event <SOUND> failed! ", eSLT_Message);
					return false;
				}

				if (nMemoryUsed < 0)
					return false;

				int nMemoryUsedInt = (int32)nMemoryUsed;

				pParam->SetValue(nMemoryUsedInt);
				break;
			}
		case pspFXEFFECT:
			break;
		case pspMAXRADIUS:
			{
				float fMin = 0.0f;
				float fMax = 1.0f;

				if (!(pParam->GetValue(fMax))) 
					return (false);

				// first test if there is a <distance> parameter
				FMOD::EventParameter *pParameter = 0;
				m_ExResult = m_pEvent->getParameter("distance", &pParameter);
				if (IS_FMODERROR)
				{
					//FmodErrorOutput("get event parameter <distance> failed! ", eSLT_Message);
					return false;
				}

				// if there is one, then get the values through the properties because if INFOONLY
				//m_ExResult = m_pEvent->getPropertyByIndex(FMOD::EVENTPROPERTY_3D_MAXDISTANCE, &fMax);
				//if (IS_FMODERROR)
				//{
				//	FmodErrorOutput("get 3D maxdistance property failed! ", eSLT_Message);
				//	return false;
				//}

				if (pParameter)
				{
					m_ExResult = pParameter->getRange(&fMin, &fMax);
					if (IS_FMODERROR)
					{
						FmodErrorOutput("get event parameter <distance> range failed! ", eSLT_Message);
						return false;
					}
				}

				pParam->SetValue(fMax);
				break;
			}
		case pspAMPLITUDE:
			{
				float fTemp;
				if (!(pParam->GetValue(fTemp))) 
					return (false);

				m_ExResult = m_pEvent->getVolume(&fTemp);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event property <amplitude> failed! ", eSLT_Message);
					return false;
				}

				pParam->SetValue(fTemp);
				break;
			}
		case pspCONEINNERANGLE:
			{
				float fTemp;
				if (!(pParam->GetValue(fTemp))) 
					return (false);

				m_ExResult = m_pEvent->getPropertyByIndex(FMOD_EVENTPROPERTY_3D_CONEINSIDEANGLE, &fTemp);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event property <cone inside angle> failed! ", eSLT_Message);
					return false;
				}

				pParam->SetValue(fTemp);
				break;
			}
		case pspCONEOUTERANGLE:
			{
				float fTemp;
				if (!(pParam->GetValue(fTemp))) 
					return (false);

				m_ExResult = m_pEvent->getPropertyByIndex(FMOD_EVENTPROPERTY_3D_CONEOUTSIDEANGLE, &fTemp);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event property <cone outside angle> failed! ", eSLT_Message);
					return false;
				}

				pParam->SetValue(fTemp);
				break;
			}			
		case pspREVERBWET:
			{
				float fTemp;
				if (!(pParam->GetValue(fTemp))) 
					return (false);

				m_ExResult = m_pEvent->getPropertyByIndex(FMOD_EVENTPROPERTY_REVERBWETLEVEL, &fTemp);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event property <reverb wet> failed! ", eSLT_Message);
					return false;
				}
				//fTemp = 1 - (-fTemp/60.0f);

				pParam->SetValue(fTemp);
				break;
			}		
		case pspREVERBDRY:
			{
				float fTemp;
				if (!(pParam->GetValue(fTemp))) 
					return (false);

				m_ExResult = m_pEvent->getPropertyByIndex(FMOD_EVENTPROPERTY_REVERBDRYLEVEL, &fTemp);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event property <reverb dry> failed! ", eSLT_Message);
					return false;
				}
				//fTemp = 1 - (-fTemp/60.0f);

				pParam->SetValue(fTemp);
				break;
			}		
		case pspSPREAD:
			{
				float fTemp;
				if (!(pParam->GetValue(fTemp))) 
					return (false);

				// first test if there is a <spread> parameter
				FMOD::EventParameter *pParameter = 0;
				m_ExResult = m_pEvent->getParameter("spread", &pParameter);
				
				if (IS_FMODERROR)
				{
					//FmodErrorOutput("get event parameter <spread> failed! ", eSLT_Message);
					return false;
				}

				if (pParameter)
					return true;

				break;
			}	
		case pspFADEOUT:
			{
				int nTemp;
				if (!(pParam->GetValue(nTemp))) 
					return (false);

				m_ExResult = m_pEvent->getPropertyByIndex(FMOD_EVENTPROPERTY_FADEOUT, &nTemp);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event property <fadeout time> failed! ", eSLT_Message);
					return false;
				}

				pParam->SetValue(nTemp);
				break;
			}	
		case pspSPAWNEDINSTANCES:
			{
				int32 nTemp;
				if (!(pParam->GetValue(nTemp))) 
					return (false);

				FMOD_EVENT_INFO Info;

				m_ExResult = m_pEvent->getInfo(0, 0, &Info);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event info <spawned instances> failed! ", eSLT_Message);
					return false;
				}

				pParam->SetValue(Info.instancesactive);
				break;
			}
		case pspCHANNELSUSED:
			{
				int32 nTemp;
				if (!(pParam->GetValue(nTemp))) 
					return (false);

				FMOD_EVENT_INFO Info;

				m_ExResult = m_pEvent->getInfo(0, 0, &Info);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event info <channels used> failed! ", eSLT_Message);
					return false;
				}

				pParam->SetValue(Info.channelsplaying);
				break;
			}
		default:
			break;
		}
	}
	else
		return (false);
		


	return (true);
}


//////////////////////////////////////////////////////////////////////////
bool CPlatformSoundFmodEx400Event::SetParamByType(enumPlatformSoundParamSemantics eSemantics, ptParam* pParam)
{
	CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );

	if (!m_pEvent || m_State == pssWAITINGTOFADEOUT) 
	{
		SQueuedParameter QParam;
		QParam.eSemantic = eSemantics;

		int32 nTemp = 0;
		if (pParam->GetValue(nTemp)) 
			QParam.nValue = nTemp;

		float fTemp = 0.0f;
		if (pParam->GetValue(fTemp)) 
			QParam.fValue = fTemp;

		bool bAlreadyFound = false;
		tvecQueuedParameterIter It2End = m_QueuedParameter.end();
		for (tvecQueuedParameterIter It2 = m_QueuedParameter.begin(); It2!=It2End; ++It2)
		{
			if ((*It2).eSemantic == QParam.eSemantic)
			{
				(*It2).fValue = QParam.fValue;
				(*It2).nValue = QParam.nValue;
				bAlreadyFound = true;
				break;
			}
		}

		if (!bAlreadyFound)
			m_QueuedParameter.push_back(QParam);

		return true;
	}

	switch (eSemantics)
	{
	case pspNONE:
		return (false);
		break;
	case pspSOUNDTYPE:
		break;
	case pspSAMPLETYPE:
		break;
	case pspDISTANCE:
		{
			float fTemp = 0.0;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			//if (m_pSound->GetSoundSystemPtr()->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
				//m_pAudioDevice->m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::EVENT_SETDISTANCE, gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*) m_pEvent, (void*) &fTemp);

			FMOD::EventParameter *pParameter = 0;

			if (m_nDistanceIndex != -1)
			{
				m_ExResult = m_pEvent->getParameterByIndex(m_nDistanceIndex, &pParameter); 

				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event parameter <distance> failed! ", eSLT_Message);
					return false;
				}

				m_ExResult = pParameter->setValue(fTemp);
				
				if (IS_FMODERROR)
				{
					FmodErrorOutput("set event parameter value <distance> failed! ", eSLT_Message);
					return false;
				}
			}
			break;
		}
	case pspFREQUENCY:
		// TODO Check Pitch and stuff
		{
			int32 nTemp = 0;
			if (!(pParam->GetValue(nTemp))) 
				return (false);
			//m_pEvent->getParameter()
			//if (!CS_SetFrequency(m_nChannel,nTemp)) 
				return (false);
			break;
		}
	case pspVOLUME:
		{
			// do not set the volume on infoonly events, otherwise sound will fade to 0 on reusing the event
			if (m_State == pssINFOONLY)
				return false;

			float fTemp = 0.0;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			fTemp *= abs(m_fMaxVolume);
			
			if (m_pSound->GetSoundSystemPtr()->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
				m_pAudioDevice->m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::EVENT_SETVOLUME, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*) m_pEvent, (void*) &fTemp);
			
			m_ExResult = m_pEvent->setVolume(fTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set event property <volume> failed! ", eSLT_Message);
				return false;
			}
			break;
		}
	case pspISPLAYING:
		// dont support that
		break;
	case pspLOOPMODE:
		{
			//bool bTemp = false;
			//FMOD_MODE Mode = FMOD_LOOP_OFF;
			//if (!(pParam->GetValue(bTemp))) 
				return (false);
			//if (bTemp) 
			//	Mode = FMOD_LOOP_NORMAL;
			//bool bPaused;
			//m_pExChannel->getPaused(&bPaused);
			//if (bPaused)
			//	m_pExChannel->setMode(Mode);
			//else
			//{
			//	m_pExChannel->setPaused(true);
			//	m_pExChannel->setMode(Mode);
			//	m_pExChannel->setPaused(false);
			//}
			break;
		}
	case pspPAUSEMODE:
		{
			// do not pause on infoonly events
			if (m_State == pssINFOONLY)
				return false;

			bool bTemp = false;
			if (!(pParam->GetValue(bTemp))) 
				return (false);

			if (m_pSound->GetSoundSystemPtr()->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
				m_pAudioDevice->m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::EVENT_SETPAUSED, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*) m_pEvent, (void*) bTemp);

			m_ExResult = m_pEvent->setPaused(bTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set event property <paused> failed! ", eSLT_Message);
				return false;
			}

			if (bTemp)
				m_State = pssPAUSED;
				
			break;
		}
	case pspSPEAKERPAN:
		return (false);
		break;
	case pspSPEAKER3DPAN:
		return (false);
		break;
	case pspSAMPLEPOSITION:
		{
			// sample position if not supported by sound events
			return (false);
			break;
		}
	case pspTIMEPOSITION:
		{
			// do not set time offset on infoonly events
			if (m_State == pssINFOONLY)
				return false;

			int nTemp;
			if (!(pParam->GetValue(nTemp))) 
				return (false);

			float fOffsetInSec = nTemp / 1000.0f;

			m_ExResult = m_pEvent->setPropertyByIndex(FMOD_EVENTPROPERTY_TIMEOFFSET, &fOffsetInSec);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set event property <timeoffset> failed! ", eSLT_Message);
				return false;
			}
			break;
		}		
	case psp3DPOSITION:
		{
			Vec3 VecTemp;
			if (!(pParam->GetValue(VecTemp))) 
				return (false);
			// axis swap!
			FMOD_VECTOR pos = {VecTemp.x,VecTemp.z,VecTemp.y};
			m_ExResult = m_pEvent->set3DAttributes(&pos, NULL);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set event property <3d position> failed! ", eSLT_Message);
				return false;
			}

			break;
		}
	case psp3DVELOCITY:
		{
			Vec3 VecTemp;
			if (!(pParam->GetValue(VecTemp))) 
				return (false);
			// axis swap!
			FMOD_VECTOR vel = {VecTemp.x,VecTemp.z,VecTemp.y};
			m_ExResult = m_pEvent->set3DAttributes(NULL, &vel);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set event property <3d velocity> failed! ", eSLT_Message);
				return false;
			}

			break;
		}
	case pspPRIORITY:
		break;
	case pspFXEFFECT:
		break;
	case pspAMPLITUDE:
		break;
	case pspREVERBWET:
		{
			// do not set the reverb on infoonly events
			if (m_State == pssINFOONLY)
				return false;

			float fTemp;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_ExResult = m_pEvent->setPropertyByIndex(FMOD_EVENTPROPERTY_REVERBWETLEVEL, &fTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set event property <reverb wet> failed! ", eSLT_Message);
				return false;
			}
			break;
		}		
	case pspREVERBDRY:
		{
			// do not set the reverb on infoonly events
			if (m_State == pssINFOONLY)
				return false;

			float fTemp;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_ExResult = m_pEvent->setPropertyByIndex(FMOD_EVENTPROPERTY_REVERBDRYLEVEL, &fTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set event property <reverb dry> failed! ", eSLT_Message);
				return false;
			}
			break;
		}		
	case pspSPREAD:
		break;
	case pspFADEOUT:
		{
			// do not set the reverb on infoonly events
			if (m_State == pssINFOONLY)
				return false;

			int nTemp;
			if (!(pParam->GetValue(nTemp))) 
				return (false);

			m_ExResult = m_pEvent->setPropertyByIndex(FMOD_EVENTPROPERTY_FADEOUT, &nTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set event property <fadeout time> failed! ", eSLT_Message);
				return false;
			}

			break;
		}	
	case pspSTEALBEHAVIOUR:
		{
			int32 nTemp = 0;
			if (!(pParam->GetValue(nTemp))) 
				return (false);

			m_ExResult = m_pEvent->setPropertyByIndex(FMOD_EVENTPROPERTY_MAX_PLAYBACKS_BEHAVIOR, &nTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set event property <steal behaviour> failed! ", eSLT_Message);
				return false;
			}
			break;
		}		
	default:
		break;
	}
	return (true);
}


// Gets and Sets Parameter defined by string and float value
int CPlatformSoundFmodEx400Event::GetParamByName(const char *sParameter, float *fValue, bool bOutputWarning)
{
	int nIndex = -1;

	if (m_pEvent && sParameter) 
	{
		FMOD::EventParameter *pParameter = 0;
		
		{
			CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );
			m_ExResult = m_pEvent->getParameter((char*)sParameter, &pParameter);
		}
		
		if (IS_FMODERROR)
		{
			if (bOutputWarning)
			{
				string sPotentialError = "get event parameter <";
				sPotentialError += sParameter;
				sPotentialError = sPotentialError + "> failed!";
				FmodErrorOutput(sPotentialError.c_str(), eSLT_Message);
			}
			return nIndex;
		}

		if (pParameter)
		{
			m_ExResult = pParameter->getInfo(&nIndex, NULL);
			if (IS_FMODERROR)
			{
				if (bOutputWarning)
				{
					string sPotentialError = "get event parameter <";
					sPotentialError += sParameter;
					sPotentialError = sPotentialError + "> info failed!";
					FmodErrorOutput(sPotentialError.c_str(), eSLT_Message);
				}
			}

			m_ExResult = pParameter->getValue(fValue);
			if (IS_FMODERROR)
			{
				if (bOutputWarning)
				{
					string sPotentialError = "get event parameter <";
					sPotentialError += sParameter;
					sPotentialError = sPotentialError + "> value failed!";
					FmodErrorOutput(sPotentialError.c_str(), eSLT_Message);
				}
			}
		}
	}
	else
	{
		// queuing this value
		bool bNewEntry = CSound::m_pSSys->m_ParameterNames.AddOrSetParameter(sParameter, false, 0.0f, nIndex);

		if (bNewEntry)
		{
			*fValue = 0.0f;
			return nIndex;
		}

		if (bOutputWarning)
		{
			string sPotentialError = "get event parameter <";
			sPotentialError += sParameter;
			sPotentialError = sPotentialError + "> value failed!";
			FmodErrorOutput(sPotentialError.c_str(), eSLT_Message);
		}
	}

	return nIndex;
}

int CPlatformSoundFmodEx400Event::SetParamByName(const char *sParameter, float fValue, bool bOutputWarning)
{
	SQueuedParameter QParam;
	QParam.fValue = fValue;
	bool bFoundInName = false;
	
	assert( sParameter[0] );

	if (!sParameter[0])
		return QParam.nNameIndex;


	int nIndex = -1;
	bool bNewEntry = CSound::m_pSSys->m_ParameterNames.AddOrSetParameter(sParameter, false, 0.0f, nIndex);

	if (nIndex < (int)m_ParameterIndex.size())
	{
		QParam.nEventIndex = m_ParameterIndex[nIndex].nEventIndex;
		QParam.nNameIndex = nIndex;
		bFoundInName = true;
	}

	// Queue it if no event is there
	if (!m_pEvent || m_State == pssWAITINGTOFADEOUT)
	{
		bool bAlreadyFound = false;
		tvecQueuedParameterIter It2End = m_QueuedParameter.end();
		for (tvecQueuedParameterIter It2 = m_QueuedParameter.begin(); It2!=It2End; ++It2)
		{
			if ((*It2).nNameIndex == QParam.nNameIndex)
			{
				(*It2).fValue = fValue;
				bAlreadyFound = true;
				break;
			}
		}

		if (!bAlreadyFound && bFoundInName)
			m_QueuedParameter.push_back(QParam);

	}
	else
	{
		FMOD::EventParameter *pParameter = 0;

		{
			CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );

			if (QParam.nEventIndex != -1)
				m_ExResult = m_pEvent->getParameterByIndex(QParam.nEventIndex, &pParameter);
			else
			{
				// just set the error code
				m_ExResult = FMOD_ERR_EVENT_NOTFOUND;
			}
		}

		if (m_pSound->GetSoundSystemPtr()->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
			m_pAudioDevice->m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::EVENT_GETPARAMETER, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*) m_pEvent, (void*) sParameter, pParameter);

		if (IS_FMODERROR)
		{
			if (bOutputWarning)
			{
				string sPotentialError = "set event parameter <";
				sPotentialError += sParameter;
				sPotentialError = sPotentialError + "> failed!";
				FmodErrorOutput(sPotentialError.c_str(), eSLT_Warning);
			}

			return QParam.nNameIndex;
		}

		if (pParameter)
		{

			if (m_pSound->GetSoundSystemPtr()->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
				m_pAudioDevice->m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::PARAMETER_SETVALUE, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*) pParameter, (void*) &fValue);

			{
				CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );
				m_ExResult = pParameter->setValue(fValue);
			}

			if (IS_FMODERROR)
			{
				if (bOutputWarning)
				{
					string sPotentialError = "set event parameter <";
					sPotentialError += sParameter;
					sPotentialError = sPotentialError + "> value failed!";
					FmodErrorOutput(sPotentialError.c_str(), eSLT_Message);
				}
			}

			// all go and set
		}
	}

	return QParam.nNameIndex;;
}

int	CPlatformSoundFmodEx400Event::GetNameIndexByEventIndex(int const nEventIndex)
{
	for(int i=0; i<m_ParameterIndex.size(); ++i)
	{
		if(m_ParameterIndex[i].nEventIndex == nEventIndex)
		{
			return i;
		}
	}

	return -1;
}


// Gets and Sets Parameter defined by index and float value
bool CPlatformSoundFmodEx400Event::GetParamByIndex(int nEventIndex,
																									 float* const pfValue,
																									 float* const pfRangeMin/* =NULL */,
																									 float* const pfRangeMax/* =NULL */,
																									 char const** const ppcParameterName/* =NULL */,
																									 bool const bOutputWarning/* =true */)
{
	CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );

	// make sure we dont access via a invalid index
	if (nEventIndex < 0)
		return false;

	if (m_pEvent) 
	{
		FMOD::EventParameter *pParameter = 0;
		m_ExResult = m_pEvent->getParameterByIndex(nEventIndex, &pParameter);
		if (IS_FMODERROR)
		{
			if (bOutputWarning)
			{
				char cTemp[64] = {'\0'};
				sprintf_s(cTemp, 64, "%d", nEventIndex);

				string sPotentialError = "Get event parameter <index: ";
				sPotentialError += cTemp;
				//sPotentialError += sParameter;
				sPotentialError += "> failed!";
				FmodErrorOutput(sPotentialError.c_str(), eSLT_Message);
			}
			return false;
		}

		if (pParameter)
		{
			m_ExResult = pParameter->getValue(pfValue);
			if (IS_FMODERROR)
			{
				if (bOutputWarning)
				{
					char cTemp[64] = {'\0'};
					sprintf_s(cTemp, 64, "%d", nEventIndex);

					string sPotentialError = "Get event parameter <index: ";
					sPotentialError += cTemp;
					//sPotentialError += sParameter;
					sPotentialError += "> value failed!";
					FmodErrorOutput(sPotentialError.c_str(), eSLT_Message);
				}
				return false;
			}

			if(ppcParameterName)
			{
				int nNameIndex = GetNameIndexByEventIndex(nEventIndex);
				
				if (nNameIndex != -1)
					*ppcParameterName = CSound::m_pSSys->m_ParameterNames.GetNameByIndex(nNameIndex);
			}
			
			if(pfRangeMin)
			{
				m_ExResult = pParameter->getRange(pfRangeMin, NULL);
				if(IS_FMODERROR)
				{
					if(bOutputWarning)
					{
						char cTemp[64] = {'\0'};
						sprintf_s(cTemp, 64, "%d", nEventIndex);

						string sPotentialError = "Get event parameter <index: ";
						sPotentialError += cTemp;
						//sPotentialError += sParameter;
						sPotentialError += "> range min failed!";
						FmodErrorOutput(sPotentialError.c_str(), eSLT_Message);
					}
					return false;
				}
			}

			if(pfRangeMax)
			{
				m_ExResult = pParameter->getRange(NULL, pfRangeMax);
				if(IS_FMODERROR)
				{
					if(bOutputWarning)
					{
						char cTemp[64] = {'\0'};
						sprintf_s(cTemp, 64, "%d", nEventIndex);

						string sPotentialError = "Get event parameter <index: ";
						sPotentialError += cTemp;
						//sPotentialError += sParameter;
						sPotentialError += "> range max failed!";
						FmodErrorOutput(sPotentialError.c_str(), eSLT_Message);
					}
					return false;
				}
			}

			return true;
		}
	}
	else
	{
		int nNameIndex = GetNameIndexByEventIndex(nEventIndex);

		if (nNameIndex != -1)
		{
			if(ppcParameterName)
				*ppcParameterName = CSound::m_pSSys->m_ParameterNames.GetNameByIndex(nNameIndex);

			if(pfValue)
				*pfValue = 0.0f;

			if(pfRangeMin)
				*pfRangeMin = m_ParameterIndex[nNameIndex].fRangeMin;

			if(pfRangeMax)
				*pfRangeMax = m_ParameterIndex[nNameIndex].fRangeMax;

			return true;
		}


		if (bOutputWarning)
		{
			char cTemp[64] = {'\0'};
			sprintf_s(cTemp, 64, "%d", nEventIndex);

			string sPotentialError = "get event parameter < Index: ";
			sPotentialError += cTemp;
			sPotentialError += "> value failed!";
			FmodErrorOutput(sPotentialError.c_str(), eSLT_Message);
		}
	}
	return false;
}

bool CPlatformSoundFmodEx400Event::SetParamByIndex(int nEventIndex, float fValue, bool bOutputWarning)
{
	CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );

	// Make sure we don't access via a invalid index
	if (nEventIndex < 0)
		return false;

	if (!m_pEvent) 
	{
		SQueuedParameter QParam;
		QParam.nEventIndex	= nEventIndex;
		QParam.nNameIndex		= GetNameIndexByEventIndex(nEventIndex);
		QParam.fValue				= fValue;

		bool bAlreadyFound = false;
		tvecQueuedParameterIter It2End = m_QueuedParameter.end();
		for (tvecQueuedParameterIter It2 = m_QueuedParameter.begin(); It2!=It2End; ++It2)
		{
			if ((*It2).nNameIndex == QParam.nNameIndex)
			{
				(*It2).fValue = fValue;
				bAlreadyFound = true;
				break;
			}
		}

		if (!bAlreadyFound)
			m_QueuedParameter.push_back(QParam);

		return true;
	}

	if (m_pEvent) 
	{
		FMOD::EventParameter *pParameter = 0;
		m_ExResult = m_pEvent->getParameterByIndex(nEventIndex, &pParameter);

		if (IS_FMODERROR)
		{
			char cTemp[64] = {'\0'};
			sprintf_s(cTemp, 64, "%d", nEventIndex);

			string sPotentialError = "set event parameter <index: ";
			sPotentialError += cTemp;
			//sPotentialError += sParameter;
			sPotentialError += "> failed!";
			FmodErrorOutput(sPotentialError.c_str(), eSLT_Warning);
			return false;
		}

		if (pParameter)
		{
			m_ExResult = pParameter->setValue(fValue);
			if (IS_FMODERROR)
			{
				if (bOutputWarning)
				{
					char cTemp[64] = {'\0'};
					sprintf_s(cTemp, 64, "%d", nEventIndex);

					string sPotentialError = "set event parameter <index: ";
					sPotentialError += cTemp;
					//sPotentialError += sParameter;
					sPotentialError += "> value failed!";
					FmodErrorOutput(sPotentialError.c_str(), eSLT_Message);

					float fVerify = 0;
					pParameter->getValue(&fVerify);

					float fMin, fMax;
					pParameter->getRange(&fMin, &fMax);

					if (fValue < fMin || fValue > fMax)
					{
						char* sParameterName = 0;
						pParameter->getInfo(0, &sParameterName);
						CryLogAlways("Parameter %s value %1.2f out of Range %1.2f - %1.2f", sParameterName, fValue, fMin, fMax);
					}

					if ( abs(fVerify-fValue) > 0.01 && m_State != pssINFOONLY)
					{
						char* sParameterName = 0;
						pParameter->getInfo(0, &sParameterName);
						CryLogAlways("Parameter %s set to %1.2f, got %1.2f back", sParameterName, fValue, fVerify);
					}
				}
				return false;
			}
		}
		return true;
	}
	return false;
}

bool CPlatformSoundFmodEx400Event::IsInCategory(const char* sCategory)
{
	if (!m_pEvent)
		return false;

	FMOD::EventCategory *pCat = NULL;

	{
		CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );
		m_ExResult = m_pEvent->getCategory(&pCat);
	}

	if (m_ExResult == FMOD_OK && pCat)
	{
		int nIndex = 0;
		char *sName = 0; 

		{
			CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );
			pCat->getInfo( &nIndex, &sName );
		}

		if (strcmp(sCategory, sName) == 0)
			return true;
	}

	return false;
}

void  CPlatformSoundFmodEx400Event::GetMemoryUsage(ICrySizer* pSizer) const
{
	pSizer->AddObject(m_QueuedParameter);
	pSizer->AddObject(m_ParameterIndex);

	size_t nSize = sizeof(*this);

	if (m_pEvent)
	{
		int nIndex;
		char *name; 
		//char **soundbanks;
		TFixedResourceName sBankName;
		FMOD_EVENT_INFO EventInfo;

		// first get number of wavebanks
		m_ExResult = m_pEvent->getInfo(&nIndex, &name, &EventInfo);

		if (IS_FMODERROR)
		{
			const_cast<CPlatformSoundFmodEx400Event*>(this)->FmodErrorOutput("event system get event info failed! ", eSLT_Warning);
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

			m_ExResult = m_pEvent->getInfo(0, 0, &EventInfo);
			if (IS_FMODERROR)
			{
				const_cast<CPlatformSoundFmodEx400Event*>(this)->FmodErrorOutput("event system get event info with wavebanks failed! ", eSLT_Warning);
				delete[] pWavebankInfo; // remove wavebankinfo array
				return;
			}
		}

		for (int i=0; i<EventInfo.maxwavebanks; ++i)
		{
			sBankName = pWavebankInfo[i].name;

			if (m_pSound->GetSoundBufferPtr())
			{
				// Compose full File name to Wavebank
				TFixedResourceName sFilePath = m_pSound->GetSoundBufferPtr()->GetProps()->sName;
				const char *pInput = sFilePath;
				const char *pColon = strstr(pInput, ":");

				if (pInput && pColon)
				{
					sFilePath.assign(sFilePath.substr(0, pColon - pInput));
					sFilePath += "/";

					//sBankName.assign(pWavebankInfo[nBankIndex].name, strlen(pWavebankInfo[nBankIndex].name));
					sBankName += ".fsb";

					sBankName = sFilePath + sBankName;
				}
			}

			pSizer->GetResourceCollector().AddResource(sBankName.c_str(), 0xffffffff);
		}
		
		delete[] pWavebankInfo; // remove wavebankinfo array

	}
	//nSize += m_pSoundAssetManager->GetMemUsage();
	pSizer->AddObject(this, nSize);

}

//////////////////////////////////////////////////////////////////////////
bool CPlatformSoundFmodEx400Event::PlaySound(bool bStartPaused)
{

	//m_ExResult = m_pEvent->set3DOcclusion(1.0f, 0.0f);

	if (m_pEvent && m_State != pssINFOONLY)
	{
		//m_ExResult = m_pEvent->setVolume(0.0f);

		if (m_pSound->GetSoundSystemPtr()->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
			m_pAudioDevice->m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::EVENT_PLAY, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*) m_pEvent);

		if (m_State != pssPAUSED && m_pSound->m_pSSys->g_nProfiling > 1)
		{
			float f0 = gEnv->pTimer->GetAsyncCurTime();
			{
				FRAME_PROFILER( "SoundSystem::FMOD-StartEvent", GetISystem(), PROFILE_SOUND );	
				m_ExResult = m_pEvent->start();
			}

			float f1 = gEnv->pTimer->GetAsyncCurTime();
			CryLog(" <Sound Profile starting> %s %.2f", m_pSound->GetName(), (f1-f0)*1000.0f);
		}
		else
		{
			FRAME_PROFILER( "SoundSystem::FMOD-StartEvent", GetISystem(), PROFILE_SOUND );	
			//SetParam("distance", 0.0f);
			m_ExResult = m_pEvent->start();	
		}

		if (IS_FMODERROR || !m_pEvent)
		{
			// Callbacks set m_pEvent to zero or something else went wrong
			m_ExResult = FMOD_ERR_EVENT_FAILED; // set error so output works
			FmodErrorOutput("start event failed! ", eSLT_Warning);
			return false;
		}

		FMOD_EVENT_STATE EventState;
		{
			CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );
			m_ExResult = m_pEvent->getState(&EventState);
		}

		if (IS_FMODERROR)
		{
			FmodErrorOutput("get event state failed! ", eSLT_Warning);
			return false;
		}

		if (EventState & FMOD_EVENT_STATE_PLAYING)
			m_State = pssPLAYING;

		if (EventState & FMOD_EVENT_STATE_LOADING)
			m_State = pssLOADING;

		//if (bStartPaused)
		//{
		//	m_ExResult = m_pEvent->setPaused(true);
		//	m_State = pssPAUSED;
		//}
		//else
		//{
		//	m_ExResult = m_pEvent->setPaused(false);
		//	m_State = pssPLAYING;
		//}

			return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool CPlatformSoundFmodEx400Event::StopSound()
{
	if (m_pEvent && m_State != pssINFOONLY && m_State != pssSTOPPED)// && !m_bCallbackActive)
	{
		if (m_State != pssENDING) // prevent loop of stopping while ending
		{
			m_State = pssENDING;

			if (m_pSound->GetSoundSystemPtr()->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
				m_pAudioDevice->m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::EVENT_STOP, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*) m_pEvent);
			
			//m_ExResult = m_pEvent->setVolume(0.0f);
			bool bAtOnce = (m_pSound->m_pSSys->g_nStopSoundsImmediately!=0) || (m_pSound->m_eStopMode == ESoundStopMode_AtOnce);
			if (bAtOnce || m_pSound->m_eStopMode != ESoundStopMode_OnSyncPoint)
			{
	
				if (CSound::m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_PROGRAMMERSOUND && m_pSound->GetFlags() & FLAG_SOUND_VOICE)
					CSound::m_pSSys->Log(eSLT_Always, "----> Event %s Before STOP", m_sEventName.c_str());

				CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );
				m_ExResult = m_pEvent->stop(bAtOnce);

				if (CSound::m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_PROGRAMMERSOUND && m_pSound->GetFlags() & FLAG_SOUND_VOICE)
					CSound::m_pSSys->Log(eSLT_Always, "----> Event %s After STOP", m_sEventName.c_str());
			}
			else
			{
				m_State = pssWAITINGFORSYNC;

				if (CSound::m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_SYNCPOINTS)
					CSound::m_pSSys->Log(eSLT_Message, "<Sound> --> Waiting for Syncpoint: %s, Time:%.2f", m_pSound->GetName(), gEnv->pTimer->GetAsyncCurTime());

				return true;
			}
		}

			if (IS_FMODERROR)
			{
				FmodErrorOutput("stop event failed! ", eSLT_Warning);
				m_State = pssNONE;
				m_pEvent = NULL;
				return false;
			}
			
			return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool CPlatformSoundFmodEx400Event::FreeSoundHandle()
{
	if (m_pEvent)
	{
		CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );
			
		StopSound();	
		
		// deactivate callback for now
		if (m_State != pssINFOONLY)
		{
			FMOD_EVENT_STATE State;
			m_ExResult = m_pEvent->getState(&State);

			if (!(State & FMOD_EVENT_STATE_CHANNELSACTIVE))
			{
				m_ExResult = m_pEvent->setCallback(NULL, INVALID_SOUNDID); //OnCallBack
				m_pEvent = NULL;
				m_State = pssNONE;
			}
			else
				m_State = pssWAITINGTOFADEOUT;


		}
		else
		{
			m_pEvent = NULL;
			m_State = pssNONE;
		}

		// TODO Generic approach to Playmodes (Play/Pause/Stop)
		// Stopping a Stream does not stop the streaming itself !

		//To actually free a channel FreeSoundHandle has to be called.
		// if it was a stream, or if it was the only ref to the Asset, the Asset will stop.
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
enumPlatformSoundErrorCodes CPlatformSoundFmodEx400Event::CreateSound(tAssetHandle AssetHandle, SSoundSettings SoundSettings)
{
	// TODO Verify that we dont need any StopChannel or (m_nChannel != CS_FREE) checks at all
	CSoundBuffer* SoundBuf	= NULL;
	SoundBuf								= m_pSound->GetSoundBufferPtr();

	ptParamF32		fParam(0.0f);
	ptParamINT32	nParam(0);

	if (!SoundBuf)
		return pssBUFFER_INVALID;

	UpdateFMODState();

	if (!AssetHandle)
		return pssBUFFER_HANDLE_INVALID;

	if (!SoundBuf)
		return pssBUFFER_INVALID;

	bool bSoundBufLoaded = (SoundBuf->Loaded() || m_pSound->m_pEventGroupBuffer);

	if (!bSoundBufLoaded)
		return pssBUFFER_NOT_AVAILIBLE;

	switch (SoundBuf->GetProps()->eBufferType)
	{
	case btSTREAM:
		{
			return pssBUFFER_BAD_TYPE;
			break;
		}
	case btNETWORK:
		{

			return pssBUFFER_BAD_TYPE;
			break;
		}
	case btEVENT: case btSAMPLE:
		{
			if (m_pEvent && m_State==pssINFOONLY && !SoundSettings.bLoadData)
				return pssOK;

			if (m_pEvent && m_State!=pssINFOONLY && SoundSettings.bLoadData)
				return pssOK;

			m_pEvent = NULL;
			m_State = pssNONE;
			FMOD::EventGroup  *pEventGroup = NULL;

			if (SoundBuf->GetProps()->eBufferType == btSAMPLE)
				pEventGroup = (FMOD::EventGroup*)m_pSound->m_pEventGroupBuffer->GetAssetHandle();
			else
				pEventGroup = (FMOD::EventGroup*)AssetHandle;

			if (!pEventGroup)
			{
				int nReallyBad = 1;
				(void)nReallyBad;
				return pssBUFFER_HANDLE_INVALID;
			}

			// first get the INFOONLY event retrieve info
			if (!m_pEvent && m_State != pssINFOONLY)
			{
				FMOD_EVENT_MODE EventMode = FMOD_EVENT_INFOONLY;
				{
					CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );
					m_ExResult = pEventGroup->getEvent((char*)m_sEventName.c_str(), EventMode, &m_pEvent);
				}

				if (m_pSound->GetSoundSystemPtr()->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
					m_pAudioDevice->m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::GROUP_GETEVENT, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*) pEventGroup, (void*) m_sEventName.c_str(), (void*)EventMode, (void*)m_pEvent);

				if (IS_FMODERROR)
				{
					FmodErrorOutput("get info event failed! (Event does not exist!) ", eSLT_Message);
					m_State = pssERROR;
					return pssEVENT_UNKNOWN;
				}

				m_State = pssINFOONLY;


				// retrieve all availible parameters and store their name indicies
				FMOD::EventParameter *pParameter = 0;
				int nIndex = 0;

				do 
				{
					char *name; 
					pParameter = NULL;
					{
						CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );
						m_ExResult = m_pEvent->getParameterByIndex(nIndex, &pParameter);
					}

					if (pParameter)
					{
						m_ExResult = pParameter->getInfo(0, &name);

						if (IS_FMODERROR)
						{
							FmodErrorOutput("get event info failed! ", eSLT_Message);
							return pssPARAMETER_INVALID;
						}

						int nNameIndex = -1;

						bool bNewEntry = CSound::m_pSSys->m_ParameterNames.AddOrSetParameter(name, false, 0.0f, nNameIndex);

						// make sure the Parameterindex matches the size of ParameterNames
						assert(nNameIndex < m_ParameterIndex.size());
						if (nNameIndex >= m_ParameterIndex.size())
						{
							m_ParameterIndex.resize(CSound::m_pSSys->m_ParameterNames.size());
						}

						// Fill event parameter struct
						float fRangeMin;
						float fRangeMax;
						pParameter->getRange(&fRangeMin, &fRangeMax);

						m_ParameterIndex[nNameIndex].nEventIndex	= nIndex;
						m_ParameterIndex[nNameIndex].fRangeMin		= fRangeMin;
						m_ParameterIndex[nNameIndex].fRangeMax		= fRangeMax;

						// shortcut for distance parameter
						if (strcmp(name, "distance") == 0)
							m_nDistanceIndex = nIndex;
					}

					++nIndex;

				} while(!IS_FMODERROR);

				// more info stuff

				fParam.SetValue(m_fMaxRadius);
				GetParamByType(pspMAXRADIUS, &fParam);
				fParam.GetValue(m_fMaxRadius);

				float fTemp = 0.0f;
				GetParamByType(pspVOLUME, &fParam);
				fParam.GetValue(fTemp);
				if (m_fMaxVolume < 0)
					m_fMaxVolume = fTemp;
				assert(m_fMaxVolume != 0.0f);

				FMOD_EVENT_INFO Info;

				m_ExResult = m_pEvent->getInfo(0, 0, &Info);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event info <length ms> failed! ", eSLT_Message);
					return pssEVENT_INVALID;
				}
				m_nCurrentSpawns = Info.instancesactive;
				m_nLenghtInMs = Info.lengthms;

				m_ExResult = m_pEvent->getPropertyByIndex(FMOD_EVENTPROPERTY_MAX_PLAYBACKS, &m_nMaxSpawns);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get event property <max playbacks> failed! ", eSLT_Message);
					return pssEVENT_INVALID;
				}

				// PS3 Temp optimization
				///m_nMaxSpawns = m_nMaxSpawns/1.5f;
				//m_ExResult = m_pEvent->setPropertyByIndex(FMOD_EVENTPROPERTY_MAX_PLAYBACKS, &m_nMaxSpawns, false);
				// PS3 Temp optimization




				//EVENTPROPERTY_MAX_PLAYBACKS
				FMOD_MODE Mode;
				m_ExResult = m_pEvent->getPropertyByIndex(FMOD_EVENTPROPERTY_MODE, &Mode);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get Property Mode failed!", eSLT_Warning);
					return pssPROPERTY_INVALID;
				}

				int nPriority = 0;
				m_ExResult = m_pEvent->getPropertyByIndex(FMOD_EVENTPROPERTY_PRIORITY, &nPriority);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("get Property Priority failed!", eSLT_Warning);
					return pssPROPERTY_INVALID;
				}
			}	

			// now set all parameters to the info only event
			if (m_pEvent && m_State == pssINFOONLY)
			{
				// Reset TimeOffset on infoonly event
				//float fMS = 0.0f;
				//ptParamF32 fParam(fMS);
				//SetParamByType(pspTIMEPOSITION, &fParam);

				// set queued Parameters
				int nNum = m_QueuedParameter.size();
				for (int i=0; i<nNum; ++i)
				{
					if (m_QueuedParameter[i].nEventIndex != -1)
					{
						SetParamByIndex(m_QueuedParameter[i].nEventIndex, m_QueuedParameter[i].fValue, false);
						continue;
					}

					if (m_QueuedParameter[i].eSemantic != pspNONE)
					{
						nParam.SetValue(m_QueuedParameter[i].nValue);
						fParam.SetValue(m_QueuedParameter[i].fValue);
						SetParamByType(m_QueuedParameter[i].eSemantic, &nParam);
						SetParamByType(m_QueuedParameter[i].eSemantic, &fParam);
						continue;
					}

					// should never go here
					if (m_QueuedParameter[i].nEventIndex > -1)
					{
						SetParamByIndex(m_QueuedParameter[i].nEventIndex, m_QueuedParameter[i].fValue, false);
						continue;
					}
				}
			}

			// now spawn the real event if needed
			if (SoundSettings.bLoadData)
			{
				m_pEvent = NULL;
				m_State = pssNONE;

				FMOD_EVENT_MODE EventMode = FMOD_EVENT_DEFAULT; // | FMOD_EVENT_USERDSP;

				if (SoundSettings.bLoadNonBlocking && m_pSound->GetSoundSystemPtr()->g_nLoadNonBlocking > 0)
					EventMode = FMOD_EVENT_NONBLOCKING;

				assert (pEventGroup);
				if (pEventGroup)
				{
					{
						CSoundSystem* pSys = m_pSound->GetSoundSystemPtr();
						if (pSys && (pSys->g_nProfiling > 1))
						{
							float f0 = gEnv->pTimer->GetAsyncCurTime();
							{
								CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );
								m_ExResult = pEventGroup->getEvent((char*)m_sEventName.c_str(), EventMode, &m_pEvent);
							}

							float f1 = gEnv->pTimer->GetAsyncCurTime();
							CryLog("<Sound Profile Getting Event> %s %.2f", m_sEventName.c_str(), (f1-f0)*1000.0f);
						}
						else
						{
							CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );
							m_ExResult = pEventGroup->getEvent((char*)m_sEventName.c_str(), EventMode, &m_pEvent);
						}
					}

					if (m_pSound->GetSoundSystemPtr()->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
						m_pAudioDevice->m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::GROUP_GETEVENT, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*) pEventGroup, (void*) m_sEventName.c_str(), (void*)EventMode, (void*)m_pEvent);

				}
				else
				{
					m_pEvent = NULL;
					return pssBUFFER_HANDLE_INVALID;
				}


				if (IS_FMODERROR)
				{
					if (m_ExResult == FMOD_ERR_EVENT_FAILED)
					{
						m_State = pssNONE;
						return pssEVENT_NOT_AVAILIBLE;
					}

					FmodErrorOutput("create event failed! (Typo in event name or event not available?)", eSLT_Error);
					return pssEVENT_UNKNOWN;
				}

				// set queued Parameters again TODO check if that is needed
				for (uint32 i=0; i<m_QueuedParameter.size(); ++i)
				{
					if (m_QueuedParameter[i].nEventIndex != -1)
					{
						SetParamByIndex(m_QueuedParameter[i].nEventIndex, m_QueuedParameter[i].fValue, false);
						continue;
					}

					if (m_QueuedParameter[i].eSemantic != pspNONE)
					{
						nParam.SetValue(m_QueuedParameter[i].nValue);
						fParam.SetValue(m_QueuedParameter[i].fValue);
						SetParamByType(m_QueuedParameter[i].eSemantic, &nParam);
						SetParamByType(m_QueuedParameter[i].eSemantic, &fParam);
						continue;
					}

					// should never go here
					if (m_QueuedParameter[i].nEventIndex > -1)
					{
						SetParamByIndex(m_QueuedParameter[i].nEventIndex, m_QueuedParameter[i].fValue, false);
						continue;
					}
				}
				////

				UpdateFMODState();

				if (m_State != pssINFOONLY)
				{
					// activate callback for now
					m_ExResult = m_pEvent->setCallback(OnCallBack, (void*)m_pSound->GetId());
				}

				if (IS_FMODERROR)
				{
					FmodErrorOutput("set callback on event failed!", eSLT_Warning);
				}

				break;
			}
		}
	}

	if (m_pSound->GetSoundSystemPtr()->g_nProfiling > 0)
	{
		FMOD_EVENT_INFO EventInfo;

		// first get number of wavebanks
		m_ExResult = m_pEvent->getInfo(0, 0, &EventInfo);

		if (IS_FMODERROR)
		{
			FmodErrorOutput("event system get event info failed! ", eSLT_Warning);
			return pssEVENT_INVALID;
		}

		FMOD_EVENT_WAVEBANKINFO* pWavebankInfo = NULL;

		if (EventInfo.maxwavebanks)
		{
			// now repeat to fill the array of wavebank infos
			pWavebankInfo = new FMOD_EVENT_WAVEBANKINFO[EventInfo.maxwavebanks];

			if (!pWavebankInfo)
				return pssMEMORY_ERROR;

			EventInfo.wavebankinfo = pWavebankInfo;

			m_ExResult = m_pEvent->getInfo(0, 0, &EventInfo);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("event system get event info with wavebanks failed! ", eSLT_Warning);
				delete[] pWavebankInfo; // remove wavebankinfo array
				return pssEVENT_INVALID;
			}
		}


		for (int i=0; i<EventInfo.maxwavebanks; ++i)
		{
			IWavebank *pWavebank = m_pSound->GetSoundSystemPtr()->GetIAudioDevice()->GetWavebank(pWavebankInfo[i].name);
		
			if (pWavebank && strcmp(pWavebank->GetPath(),"") == 0)
			{
				if (m_pSound->GetSoundBufferPtr())
				{
					// Compose full File name to Wavebank
					TFixedResourceName sFilePath = m_pSound->GetSoundBufferPtr()->GetProps()->sName;
					const char *pInput = sFilePath;
					const char *pColon = strstr(pInput, ":");

					if (pInput && pColon)
					{
						sFilePath.assign(sFilePath.substr(0, pColon - pInput));
						sFilePath += "/";
						pWavebank->SetPath(sFilePath.c_str());
					}
				}
			}
		}

		delete[] pWavebankInfo; // remove wavebankinfo array
	}

	return pssOK;
}

void CPlatformSoundFmodEx400Event::UpdateFMODState()
{
	m_State = pssNONE;

	if (m_pEvent)
	{
		FMOD_EVENT_STATE EventState;
		m_ExResult = m_pEvent->getState(&EventState);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("get event state failed! ", eSLT_Warning);
			return;
		}

		if (EventState & FMOD_EVENT_STATE_READY)
			m_State = pssREADY;			

		if (EventState & FMOD_EVENT_STATE_LOADING)
			m_State = pssLOADING;

		if (EventState & FMOD_EVENT_STATE_PLAYING)
			m_State = pssPLAYING;

		if (EventState & FMOD_EVENT_STATE_INFOONLY)
			m_State = pssINFOONLY;
	}
}

#endif
