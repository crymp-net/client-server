////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   PlatformSoundFmodEx400.cpp
//  Version:     v1.00
//  Created:     8/6/2005 by Tomas
//  Compilers:   Visual Studio.NET
//  Description: FmodEx 4.00 Implementation of a platform dependent Sound
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#ifdef SOUNDSYSTEM_USE_FMODEX400

#include "PlatformSoundFmodEx400.h"
#include "SoundBufferFmodEx400.h"
#include "AudioDeviceFmodEx400.h"
#include "Sound.h"
#include "SoundSystem.h"
#include "FmodEx/inc/fmod_errors.h"
#include "CryCommon/CryCore/CrySizer.h"
#include "CryCommon/CrySystem/ITimer.h"

//////////////////////////////////////////////////////////////////////////
#define IS_FMODERROR (m_ExResult != FMOD_OK )

//////////////////////////////////////////////////////////////////////////
CPlatformSoundFmodEx400::CPlatformSoundFmodEx400(CSound*	pSound, FMOD::System* pCSEX)
{
	assert(pCSEX);
	m_pCSEX				= pCSEX;
	m_pAudioDevice = (CAudioDeviceFmodEx400*)m_pSound->GetSoundSystemPtr()->GetIAudioDevice();
	Reset(pSound, "");
}

//////////////////////////////////////////////////////////////////////////
CPlatformSoundFmodEx400::~CPlatformSoundFmodEx400(void)
{
	FreeSoundHandle();																				 
}
void CPlatformSoundFmodEx400::Reset(CSound*	pSound, const char *sEventName)
{
	assert(pSound);
	m_pSound			= pSound;
	m_pExChannel	= NULL;
	m_nRefCount		= 0;
	m_ExResult		= FMOD_OK;
	m_State       = pssNONE;
	m_nRefCount		= 0;
}

//////////////////////////////////////////////////////////////////////////
int32 CPlatformSoundFmodEx400::AddRef()
{
	return (++m_nRefCount);
}

//////////////////////////////////////////////////////////////////////////
int32 CPlatformSoundFmodEx400::Release()
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

void CPlatformSoundFmodEx400::FmodErrorOutput(const char * sDescription, ESoundLogType LogType)
{
	if (m_ExResult == FMOD_ERR_CHANNEL_STOLEN)
	{
		m_pExChannel = NULL;
		m_pSound->Stop();
		return; // not really an error, or is it ?
	}

	if (m_ExResult == FMOD_ERR_INVALID_HANDLE)
	{
		// for normal API channel this indicates that the sound already stopped
		gEnv->pSoundSystem->Log(eSLT_Warning, "<Sound> PS: Invalid Handle! Playback maximun on %s\n", m_pSound->GetName());

		m_pExChannel = NULL;
		m_pSound->Stop();
		return;
	}

	gEnv->pSoundSystem->Log(LogType, "<Sound> PS: %s %s\n", m_pSound->GetName(), sDescription, m_ExResult);
	gEnv->pSoundSystem->Log(LogType, "<Sound> PS: (%d) %s\n", m_ExResult, FMOD_ErrorString(m_ExResult));
}

//////////////////////////////////////////////////////////////////////////
tSoundHandle CPlatformSoundFmodEx400::GetSoundHandle()	const
{ 
	return ((tSoundHandle)m_pExChannel); 
}

//////////////////////////////////////////////////////////////////////////
bool CPlatformSoundFmodEx400::Set3DPosition(Vec3* pvPosition, Vec3* pvVelocity, Vec3* pvOrientation)
{
	FMOD_VECTOR vExPos;
	FMOD_VECTOR vExVel;
	//FMOD_VECTOR vExOrient;

	// axis switch
	if (pvPosition)
	{
		vExPos.x = pvPosition->x;
		vExPos.y = pvPosition->z;
		vExPos.z = pvPosition->y;
	}

	if (pvVelocity)
	{
		vExVel.x = clamp(pvVelocity->x, -200.0f, 200.0f);
		vExVel.y = clamp(pvVelocity->z, -200.0f, 200.0f);
		vExVel.z = clamp(pvVelocity->y, -200.0f, 200.0f);
	}

	//if (pvOrientation)
	//{
	//	vExOrient.x = pvOrientation->x;
	//	vExOrient.y = pvOrientation->z;
	//	vExOrient.z = pvOrientation->y;
	//}

	if (m_pExChannel)
	{
		CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );

		FMOD_MODE mode;
		m_ExResult = m_pExChannel->getMode(&mode);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("get channel mode failed! ", eSLT_Warning);
			return false;
		}


		if (m_pSound->GetFlags() & FLAG_SOUND_RELATIVE)
		{
			mode |= FMOD_3D_HEADRELATIVE;
			vExPos.x = 0;
			vExPos.y = 0;
			vExPos.z = 0;
		}
		else
		{
			mode |= FMOD_3D_WORLDRELATIVE;
		}

		m_ExResult = m_pExChannel->setMode(mode);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("set channel mode failed! ", eSLT_Warning);
			return false;
		}

		m_ExResult = m_pExChannel->set3DAttributes((pvPosition ? &vExPos : NULL), (pvPosition ? &vExVel : NULL));
		if (IS_FMODERROR)
		{
			FmodErrorOutput("set 3d channel position failed! ", eSLT_Warning);
			return false;
		}
	}
	return (true);
}

// Gets and Sets Parameter defined in the enumSoundParam list
//////////////////////////////////////////////////////////////////////////
bool CPlatformSoundFmodEx400::GetParamByType(enumPlatformSoundParamSemantics eSemantics, ptParam* pParam)
{
	CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );

	if (!m_pExChannel) 
		return (false);

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
			int32 nTemp = 0;

			if (!(pParam->GetValue(nTemp))) 
				return (false);
			
			f32 fTemp = 0;
			m_ExResult = m_pExChannel->getFrequency(&fTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get channel frequency failed! ", eSLT_Warning);
				return false;
			}

			nTemp = (int)fTemp;
			pParam->SetValue(nTemp);
			break;
		}
	case pspVOLUME:
		{
			float fTemp = 0.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);
			
			m_ExResult = m_pExChannel->getVolume(&fTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get channel volume failed! ", eSLT_Warning);
				return false;
			}
			pParam->SetValue(fTemp);
			break;
		}
	case pspISPLAYING:
		{
			bool bTemp;
			if (!(pParam->GetValue(bTemp))) 
				return (false);
			m_ExResult = m_pExChannel->isPlaying(&bTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get channel isplaying failed! ", eSLT_Warning);
				return false;
			}

			pParam->SetValue(bTemp);
			break;
		}
	case pspISSTREAM:
		{
			bool bTemp;
			if (!(pParam->GetValue(bTemp))) 
				return (false);

			FMOD_MODE Mode;
			m_ExResult = m_pExChannel->getMode(&Mode);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get channel mode failed! ", eSLT_Warning);
				return false;
			}

			if (Mode & FMOD_CREATESTREAM)
				bTemp = true;
			else
				bTemp = false;

			pParam->SetValue(bTemp);
			break;
		}
	case pspLOOPMODE:
		{
			bool bTemp;
			if (!(pParam->GetValue(bTemp))) 
				return (false);
			FMOD_MODE Mode;
			m_ExResult = m_pExChannel->getMode(&Mode);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get channel mode failed! ", eSLT_Warning);
				return false;
			}

			bTemp = (Mode != FMOD_LOOP_OFF);
			pParam->SetValue(bTemp);
			break;
		}
	case pspPAUSEMODE:
		{
			bool bTemp;
			if (!(pParam->GetValue(bTemp))) 
				return (false);
			m_ExResult = m_pExChannel->getPaused(&bTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get channel paused failed! ", eSLT_Warning);
				return false;
			}

			pParam->SetValue(bTemp);
			break;
		}
	case pspSPEAKERPAN:
		{
			float fTemp = 0.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_ExResult = m_pExChannel->getPan(&fTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get pan level failed! ", eSLT_Warning);
				return false;
			}
			pParam->SetValue(fTemp);
			break;
		}
	case pspSPEAKER3DPAN:
		{
			float fTemp = 0.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_ExResult = m_pExChannel->get3DPanLevel(&fTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get 3d pan level failed! ", eSLT_Warning);
				return false;
			}
			pParam->SetValue(fTemp);
			break;
		}
	case pspSAMPLEPOSITION:
		{
			int32 nTemp = 0;
			if (!(pParam->GetValue(nTemp))) 
					return (false);
			uint32 nTemp2 = nTemp;
			m_ExResult = m_pExChannel->getPosition(&nTemp2, FMOD_TIMEUNIT_PCM);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get channel sample position failed! ", eSLT_Warning);
				return false;
			}

			nTemp = nTemp2;
			pParam->SetValue(nTemp);
			break;
		}
	case pspTIMEPOSITION:
		{
			int32 nTemp = 0;
			if (!(pParam->GetValue(nTemp))) 
				return (false);

			uint32 nTemp2 = nTemp;
			m_ExResult = m_pExChannel->getPosition(&nTemp2, FMOD_TIMEUNIT_MS);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get channel time position failed! ", eSLT_Warning);
				return false;
			}

			nTemp = nTemp2;
			pParam->SetValue(nTemp);
			break;
		}
//		TIMEPOSITION
			//CS_Sample_GetDefaults((CS_SAMPLE*)m_pSoundBuffer->GetAssetHandle(), &nFreq, NULL, NULL, NULL);
			//return (unsigned int)((float)CS_GetCurrentPosition(m_nChannel)/(float)nFreq*1000.0f);

	case psp3DPOSITION:
		{
		Vec3 VecTemp;
		if (!(pParam->GetValue(VecTemp))) 
			return (false);

		FMOD_MODE Mode;
		m_ExResult = m_pExChannel->getMode(&Mode);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("get channel mode failed! ", eSLT_Warning);
			return false;
		}

		if (Mode & FMOD_3D)
		{
			FMOD_VECTOR pos;
			m_ExResult = m_pExChannel->get3DAttributes(&pos, NULL);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get channel 3d position failed! ", eSLT_Warning);
				return false;
			}

			VecTemp.x = pos.x;
			VecTemp.y = pos.z;
			VecTemp.z = pos.y;
			pParam->SetValue(VecTemp);
			break;		
		}
		return false;

		}
	case psp3DVELOCITY:
		{
			Vec3 VecTemp;
			if (!(pParam->GetValue(VecTemp))) 
				return (false);
			FMOD_VECTOR vel;
			m_ExResult = m_pExChannel->get3DAttributes(NULL, &vel);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get channel 3d velocity failed! ", eSLT_Warning);
				return false;
			}

			VecTemp.x = vel.x;
			VecTemp.y = vel.z;
			VecTemp.z = vel.y;			
			pParam->SetValue(VecTemp);
			break;
		}
	case pspPRIORITY:
		{
			int32 nTemp = 0;
			if (!(pParam->GetValue(nTemp))) 
				return (false);
			m_ExResult = m_pExChannel->getPriority(&nTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get channel priority failed! ", eSLT_Warning);
				return false;
			}

			pParam->SetValue(nTemp);
			break;
		}
	case pspFXEFFECT:
		break;
	case pspAMPLITUDE:
		{
			float fTemp = 0;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_ExResult = m_pExChannel->getAudibility(&fTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get channel audibility failed! ", eSLT_Warning);
				return false;
			}

			pParam->SetValue(fTemp);
			break;
		}
	default:
		break;
	}

	return (true);
}

//////////////////////////////////////////////////////////////////////////
bool CPlatformSoundFmodEx400::SetParamByType(enumPlatformSoundParamSemantics eSemantics, ptParam* pParam)
{
	CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );

	if (!m_pExChannel) 
		return (false);

	switch (eSemantics)
	{
	case pspNONE:
		return (false);
		break;
	case pspSOUNDTYPE:
		break;
	case pspSAMPLETYPE:
		break;
	case pspFREQUENCY:
		// TODO Check Pitch and stuff
		{
			int32 nTemp = 0;
			if (!(pParam->GetValue(nTemp))) 
				return (false);
			f32 fTemp = (f32)nTemp;
			m_ExResult = m_pExChannel->setFrequency(fTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set channel frequency failed! ", eSLT_Warning);
				return false;
			}

			break;
		}
	case pspVOLUME:
		{
			float fTemp = 0;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_ExResult = m_pExChannel->setVolume(fTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set channel volume failed! ", eSLT_Warning);
				return false;
			}

			break;
		}
	case pspISPLAYING:
		// dont support that
		break;
	case pspLOOPMODE:
		{
			bool bTemp = false;
			FMOD_MODE Mode = FMOD_LOOP_OFF;
			if (!(pParam->GetValue(bTemp))) 
				return (false);
			if (bTemp) 
				Mode = FMOD_LOOP_NORMAL;
			bool bPaused;
			m_ExResult = m_pExChannel->getPaused(&bPaused);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get channel paused failed! ", eSLT_Warning);
				return false;
			}

			if (bPaused)
			{
				m_ExResult = m_pExChannel->setMode(Mode);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("set channel mode failed! ", eSLT_Warning);
					return false;
				}

			}
			else
			{
				m_ExResult = m_pExChannel->setPaused(true);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("set channel paused failed! ", eSLT_Warning);
					return false;
				}

				m_ExResult = m_pExChannel->setMode(Mode);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("set channel mode failed! ", eSLT_Warning);
					return false;
				}

				m_ExResult = m_pExChannel->setPaused(false);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("set channel paused failed! ", eSLT_Warning);
					return false;
				}

			}
			break;
		}
	case pspPAUSEMODE:
		{
			bool bTemp = false;
			if (!(pParam->GetValue(bTemp))) 
				return (false);

			m_ExResult = m_pExChannel->setPaused(bTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set channel paused failed! ", eSLT_Warning);
				return false;
			}
			if (bTemp)
				m_State = pssPAUSED;

			break;
		}
	case pspSPEAKERPAN:
		{
			float fTemp = 0.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_ExResult = m_pExChannel->setPan(fTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set pan level failed! ", eSLT_Warning);
				return false;
			}

			break;
		}
	case pspSPEAKER3DPAN:
		{
			float fTemp = 0.0f;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			m_ExResult = m_pExChannel->set3DPanLevel(fTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set 3d pan level failed! ", eSLT_Warning);
				return false;
			}

			break;
		}
	case pspSAMPLEPOSITION:
		{
			int32 nTemp = 0;
			if (!(pParam->GetValue(nTemp))) 
				return (false);

			m_ExResult = m_pExChannel->setPosition(nTemp, FMOD_TIMEUNIT_PCM);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set channel sample position failed! ", eSLT_Warning);
				return false;
			}

			break;
		}
	case pspTIMEPOSITION:
		{
			int32 nTemp = 0;
			if (!(pParam->GetValue(nTemp))) 
				return (false);

			m_ExResult = m_pExChannel->setPosition(nTemp, FMOD_TIMEUNIT_MS);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set channel time position failed! ", eSLT_Warning);
				return false;
			}

			break;
		}
	case psp3DPOSITION:
		{
			Vec3 VecTemp;
			if (!(pParam->GetValue(VecTemp))) 
				return (false);
			FMOD_VECTOR pos = {VecTemp.x, VecTemp.z, VecTemp.y};
			m_ExResult = m_pExChannel->set3DAttributes(&pos, NULL);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set channel 3d position failed! ", eSLT_Warning);
				return false;
			}

			break;
		}
	case psp3DVELOCITY:
		{
			Vec3 VecTemp;
			if (!(pParam->GetValue(VecTemp))) 
				return (false);
			FMOD_VECTOR vel = {VecTemp.x, VecTemp.z, VecTemp.y};
			m_ExResult = m_pExChannel->set3DAttributes(NULL, &vel);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set channel 3d velocity failed! ", eSLT_Warning);
				return false;
			}

			break;
		}
	case pspMINRADIUS:
		{
			float fMin;
			if (!(pParam->GetValue(fMin))) 
				return (false);
			m_ExResult = m_pExChannel->set3DMinMaxDistance(fMin, 10000.0f);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set channel min/max radius failed! ", eSLT_Warning);
				return false;
			}

			break;
		}
	case pspPRIORITY:
		{
			int32 nTemp = 0;
			if (!(pParam->GetValue(nTemp))) 
				return (false);
			m_ExResult = m_pExChannel->setPriority(nTemp);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set channel priority failed! ", eSLT_Warning);
				return false;
			}

			break;
		}
	case pspREVERBWET:
		{
			float fWet;
			if (!(pParam->GetValue(fWet))) 
				return (false);

			FMOD_REVERB_CHANNELPROPERTIES TempReverb = {0};
			m_ExResult = m_pExChannel->getReverbProperties(&TempReverb);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get channel reverb props failed! ", eSLT_Warning);
				return false;
			}

			// -10,000 (none) to 1000 (full)
			TempReverb.Room = (int)(fWet*11000) - 10000;

			m_ExResult = m_pExChannel->setReverbProperties(&TempReverb);

			if (IS_FMODERROR)
			{
				FmodErrorOutput("set channel reverb props failed! ", eSLT_Warning);
				return false;
			}

			break;
		}
	case pspREVERBDRY:
		{
			float fDry;
			if (!(pParam->GetValue(fDry))) 
				return (false);

			FMOD_REVERB_CHANNELPROPERTIES TempReverb = {0};
			m_ExResult = m_pExChannel->getReverbProperties(&TempReverb);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get channel reverb props failed! ", eSLT_Warning);
				return false;
			}

			// -10,000 (none) to 1000 (full)
			TempReverb.Direct = (int)(fDry*11000) - 10000;

			m_ExResult = m_pExChannel->setReverbProperties(&TempReverb);

			if (IS_FMODERROR)
			{
				FmodErrorOutput("set channel reverb props failed! ", eSLT_Warning);
				return false;
			}

			break;
		}
	case pspFXEFFECT:
		break;
	case pspAMPLITUDE:
		{
			return (false);
			break;
		}
	default:
		break;
	}
	return (true);
}

void  CPlatformSoundFmodEx400::GetMemoryUsage(ICrySizer* pSizer) const
{
	pSizer->AddObject(this, sizeof(*this));
}

//////////////////////////////////////////////////////////////////////////
bool CPlatformSoundFmodEx400::PlaySound(bool bStartPaused)
{
	CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );

	if (m_pExChannel)
	{
		FMOD_MODE mode;
		m_ExResult = m_pExChannel->getMode(&mode);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("get channel mode failed! ", eSLT_Warning);
			return false;
		}


		if (m_pSound->GetFlags() & FLAG_SOUND_3D)
			if (m_pSound->GetFlags() & FLAG_SOUND_RELATIVE)
			{
				mode = (FMOD_MODE)(mode | FMOD_3D_HEADRELATIVE);
				mode = (FMOD_MODE)(mode & ~FMOD_3D_WORLDRELATIVE);
			}
			else
			{
				mode = (FMOD_MODE)(mode | FMOD_3D_WORLDRELATIVE);
				mode = (FMOD_MODE)(mode & ~FMOD_3D_HEADRELATIVE);
			}
		else
		{
			mode = (FMOD_MODE)(mode | FMOD_3D_HEADRELATIVE);
			mode = (FMOD_MODE)(mode & ~FMOD_3D_WORLDRELATIVE);
		}

		m_ExResult = m_pExChannel->setMode(mode);		if (IS_FMODERROR)
		{
			FmodErrorOutput("set channel mode failed! ", eSLT_Warning);
			return false;
		}

		if (m_pSound->m_pSSys->g_nProfiling > 1)
		{
			float f0 = gEnv->pTimer->GetAsyncCurTime();
			{
				FRAME_PROFILER( "SoundSystem::FMOD-StartChannel", GetISystem(), PROFILE_SOUND );
				m_ExResult = m_pExChannel->setPaused(bStartPaused);
			}
			float f1 = gEnv->pTimer->GetAsyncCurTime();
			CryLog(" Profile: %s %f", m_pSound->GetName(), (f1-f0)*1000.0f);
		}
		else
		{
			FRAME_PROFILER( "SoundSystem::FMOD-StartChannel", GetISystem(), PROFILE_SOUND );
			m_ExResult = m_pExChannel->setPaused(bStartPaused);
		}

		if (IS_FMODERROR)
		{
			FmodErrorOutput("set channel paused failed! ", eSLT_Warning);
			return false;
		}

		//CTimeValue test = gEnv->pTimer->GetFrameStartTime();
		//gEnv->pLog->Log("DEBUG: Sound started on FMOD: %s : %f \n", m_pSound->GetName(), test.GetSeconds());
		
		//int testVolume = CS_GetVolume(m_nChannel);
		//float testPos[3];
		//CS_3D_GetAttributes(m_nChannel, testPos, NULL);
		//bool irgendwas;
		
		m_pSound->OnEvent( SOUND_EVENT_ON_PLAYBACK_STARTED );
		m_State = pssPLAYING;
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool CPlatformSoundFmodEx400::StopSound()
{
	CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );

	if (m_pExChannel)
	{
		bool bPlaying = false;
		m_ExResult = m_pExChannel->isPlaying(&bPlaying);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("get channel isplaying failed! ", eSLT_Warning);
			return false;
		}

		if (bPlaying)
		{
			m_ExResult = m_pExChannel->stop();
			if (IS_FMODERROR)
			{
				FmodErrorOutput("stop channel failed! ", eSLT_Warning);
				return false;
			}
		}
	}
	m_pExChannel = NULL;
	m_State = pssNONE;

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CPlatformSoundFmodEx400::FreeSoundHandle()
{
	//CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );

	if (m_pExChannel)
	{
		CSoundBuffer* SoundBuf	= NULL;
		SoundBuf								= m_pSound->GetSoundBufferPtr();

		// TODO Generic approach to Playmodes (Play/Pause/Stop)
		// Stopping a Stream does not stop the streaming itself !

		//To actually free a channel FreeSoundHandle has to be called.
		// if it was a stream, or if it was the only ref to the Asset, the Asset will stop.
		StopSound();	

		if (SoundBuf)
		{
			// TODO Review stuff below
			switch (SoundBuf->GetProps()->eBufferType)
			{
			case btSTREAM:
				// if (m_pSoundBuffer->GetSoundBuffer()) // was that, changed it TN
				if (SoundBuf->Loaded())
					// review this when this works fine for some time

					//CS_Stream_Stop((CS_STREAM*)SoundBuf->GetAssetHandle()); 
				break;
			case btSAMPLE:
				//			CS_StopSound(m_nChannel);
				break;
			}
			//SoundBuf->Release(); // TODO Review ALL Release and AddRef in the whole project
		}
	}
	m_State = pssNONE;

	return true;
}

FMOD_RESULT F_CALLBACK CPlatformSoundFmodEx400::OnCallBack(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, int command, unsigned int commanddata1, unsigned int commanddata2)
{
	//FMOD::Channel *cppchannel = (FMOD::Channel *)channel;

	// More code goes here.
	return FMOD_OK;
}

//////////////////////////////////////////////////////////////////////////
enumPlatformSoundErrorCodes CPlatformSoundFmodEx400::CreateSound(tAssetHandle AssetHandle, SSoundSettings SoundSettings)
{
	CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_EVENT_OTHER) );

	// TODO Verify that we dont need any StopChannel or (m_nChannel != CS_FREE) checks at all
	CSoundBuffer* SoundBuf	= NULL;
	SoundBuf								= m_pSound->GetSoundBufferPtr();

	if (m_pExChannel)
		return pssOK;

	if (SoundBuf && !SoundBuf->LoadFailure())
	{
		switch (SoundBuf->GetProps()->eBufferType)
		{
		case btSTREAM:
			m_ExResult = m_pCSEX->playSound(FMOD_CHANNEL_FREE, (FMOD::Sound*)AssetHandle, true, &m_pExChannel);
			if (IS_FMODERROR || m_pExChannel==NULL)
			{
				FmodErrorOutput("create stream channel failed! ", eSLT_Warning);
				return pssBUFFER_HANDLE_INVALID;
			}

			//m_nChannel = CS_Stream_PlayEx(CS_FREE, (CS_STREAM*), NULL, true);

			// streambug ?
			//CS_Stream_Stop((CS_STREAM*)SoundBuf->GetAssetHandle());
			break;
		case btSAMPLE:
			//m_nChannel = CS_PlaySoundEx(CS_FREE, (CS_SAMPLE*)AssetHandle, NULL, true);
			m_ExResult = m_pCSEX->playSound(FMOD_CHANNEL_FREE, (FMOD::Sound*)AssetHandle, true, &m_pExChannel);
			if (IS_FMODERROR || m_pExChannel==NULL)
			{
				FmodErrorOutput("create sample channel failed! ", eSLT_Warning);
				return pssBUFFER_HANDLE_INVALID;
			}

			//CS_SetVolume(m_nChannel, 0);

			break;
		case btNETWORK:
			//m_nChannel = CS_PlaySoundEx(CS_FREE, (CS_SAMPLE*)AssetHandle, NULL, true);
			m_ExResult = m_pCSEX->playSound(FMOD_CHANNEL_FREE, (FMOD::Sound*)AssetHandle, true, &m_pExChannel);
			if (IS_FMODERROR || m_pExChannel==NULL)
			{
				FmodErrorOutput("create sample channel failed! ", eSLT_Warning);
				return pssBUFFER_HANDLE_INVALID;
			}

			//CS_SetVolume(m_nChannel, 0);

			break;
		case btEVENT:
			{
				int nCatchme = 0;
				(void)nCatchme;
				break;
			}
		}
			
		if ((m_pSound->GetFlags() & FLAG_SOUND_RELATIVE) && m_pExChannel)
		{
			FMOD_MODE Mode;
			m_ExResult = m_pExChannel->getMode(&Mode);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("get channel mode failed! ", eSLT_Warning);
				return pssCHANNEL_INVALID;
			}
			Mode = Mode|FMOD_3D_HEADRELATIVE;
				m_pExChannel->setMode(Mode);
		}
	}

	m_State = pssREADY;

	return pssOK;
}

#endif
