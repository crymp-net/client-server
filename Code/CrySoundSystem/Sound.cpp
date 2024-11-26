//////////////////////////////////////////////////////////////////////
//
//  Crytek (C) 2001
//
//  CrySound Source Code
// 
//  File: Sound.cpp
//  Description: ISound interface implementation.
// 
//  History: 
//	-June 06,2001:Implemented by Marco Corbetta
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Sound.h"
#include "IPlatformSound.h"
#include "IAudioDevice.h"
#include "IReverbManager.h"
#include <CrySizer.h>
#include <Cry_Camera.h>
#include "SoundSystem.h"
#include <IConsole.h>
#include <ISystem.h>
#include <ITimer.h>
#include <IPhysics.h>
#include <I3DEngine.h> //needed to check if the listener is in indoor or outdoor
#include "ISoundMoodManager.h" // getVolumeDB
#include "IEntitySystem.h"
#include "ILocalizationManager.h"
#include <StlUtils.h>

#pragma warning(disable:4003)	// warning C4003: not enough actual parameters for macro 'CHECK_LOADED'
 
//#define CRYSOUND_MAXDIST	10000.0f

//////////////////////////////////////////////////////////////////////////

//#define CHECK_LOADED(_func, _retval)	if (!m_pSoundBuffer->Loaded()) { TRACE("%s(%d) Warning: %s called without a sound (%s) being fully loaded !", __FILE__, __LINE__, #_func, GetName()); return _retval; }

CSoundSystem* CSound::m_pSSys = NULL;

//////////////////////////////////////////////////////////////////////
CSound::CSound(CSoundSystem *pSSys)
:	m_bOffsetInSamples(false)
{
	assert(pSSys);
	m_pPlatformSound	= NULL;	
	m_pSoundBuffer = NULL;
	m_pEventGroupBuffer = NULL;
	m_pSSys		= pSSys;
	m_nSoundID = INVALID_SOUNDID;
	m_VoiceSpecs.pSoundInfo = NULL;
	m_SoundProxyEntityId = 0;

	Reset();
}

//////////////////////////////////////////////////////////////////////
CSound::~CSound()
{
	m_bRefCountGuard = true;

	if (m_pSoundBuffer)
		m_pSoundBuffer->RemoveFromLoadReqList(this);

	//stop the sound
	if (m_pPlatformSound && m_pPlatformSound->GetSoundHandle() && m_bPlaying)  
		Stop();

	if (m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_SIMPLE)
	{
		CSound::m_pSSys->Log(eSLT_Message, "<Sound> Destroying sound %s \n", GetName());		
	}

	// tell enities that this sound will not be valid anymore
	OnEvent( SOUND_EVENT_ON_LOAD_FAILED );

	m_pSSys->RemoveReference(this);

  m_pPlatformSound = NULL;
	m_pSoundBuffer = NULL;
	SetVoiceSpecs(NULL);
}


//////////////////////////////////////////////////////////////////////
int CSound::Release()
{
	//reset the reference
	int ref = 0;	
	if (!m_bRefCountGuard)
	{
		if((ref = --m_refCount) == 0)
		{
//#if defined(WIN64)// && defined(_DEBUG)
			// Win64 has a special variable to disable deleting all the sounds
			//ICVar* pVar = gEnv->pConsole->GetCVar("s_EnableRelease");
			//if (!pVar || pVar->GetIVal())
//#endif
				m_pSSys->RemoveSoundObject(this);
		}
	}

	return (ref);
};


void CSound::Reset()
{
	// SoundID is created here and removed by SoundSystem
	if (m_nSoundID != INVALID_SOUNDID)
		m_pSSys->RemoveReference(this);

	m_nSoundID = m_pSSys->CreateSoundID();

	m_RadiusSpecs.fOriginalMinRadius	= 1.0f;
	m_RadiusSpecs.fOriginalMaxRadius	= 1.0f;
	m_RadiusSpecs.fMaxRadius2					= 0.0f;
	m_RadiusSpecs.fMinRadius2					= 0.0f;
	m_RadiusSpecs.fPreloadRadius2			= 0.0f;
	m_RadiusSpecs.fMinRadius					= 1.0f;
	m_RadiusSpecs.fMaxRadius					= 10.0f;
	m_RadiusSpecs.fDistanceMultiplier = 1.0f;

	SetMinMaxDistance_i(m_RadiusSpecs.fMinRadius, m_RadiusSpecs.fMaxRadius);

	m_bPlaying = false;
	m_IsPaused = false;
	m_bSentOnStopEvent = false;
	m_bStopCalled = false;

	m_SoundProxyEntityId = 0;
	m_vPosition(0,0,0);
	m_vPlaybackPos(0,0,0);
	m_vSpeed(0,0,0);
	m_mTransformation.SetIdentity();
	m_fInnerAngle = 360.0f;
	m_fOuterAngle = 360.0f;

	m_eState				= eSoundState_None;
	m_eFadeState		= eFadeState_None;
	//m_fLastPlayTime.SetSeconds(0);

	m_eSemantic			= eSoundSemantic_None;

	m_nSoundScaleGroups	=	SETSCALEBIT(SOUNDSCALE_MASTER) | SETSCALEBIT(SOUNDSCALE_SCALEABLE) | SETSCALEBIT(SOUNDSCALE_DEAFNESS) | SETSCALEBIT(SOUNDSCALE_MISSIONHINT);
	m_nStartOffset			= 0;
	m_fActiveVolume			= -1;

	m_fPitching					= 0;
	m_nCurrPitch				= 0;
	m_nRelativeFreq			= 1000;
	m_nFxChannel				= -1;
	m_fRatio						= 1.0f;
	m_HDRA							= 1.0f;
	m_nFadeTimeInMS			= 0;
	m_fFadeGoal					= 1.0f;
	m_fFadeCurrent			= 1.0f;

	m_fMaxVolume			= 1.0f;
	//m_fSoundLengthSec = 0;
	SetVoiceSpecs(NULL);
	m_VoiceSpecs.fVolume = 1.0f;

	//Timur: Never addref on creation.
	m_refCount				=	0;
	m_pVisArea						= NULL;
	m_bRecomputeVisArea		= true;

	//m_fFadingValue		= 1.0f; ///5.0f; // by default 
	//m_fCurrentFade		= 1.0f; 
	m_nSoundPriority	= 0;
	m_nFlags = 0;

	m_fPostLoadRatio						= 0;
	m_bPlayAfterLoad						= false;
	m_eStopMode									= ESoundStopMode_EventFade;
	m_bPostLoadForceActiveState	= false;
	m_bPostLoadSetRatio					= false;
	m_bVolumeHasChanged					= true;
	m_bFlagsChanged							= false;
	m_bPositionHasChanged				= true;
	m_bLoopModeHasChanged				= false;
	m_bRefCountGuard						= false;
	m_bInitialized							= false;

	m_bLineSound = false;
	m_bSphereSound = false;
	m_fSphereRadius = 0;

	m_tPlayTime.SetSeconds(-1.0f);
	m_fSyncTimeout = -1.0f;
	
	// TODO deactivated because m_pPlatformSound is not availible yet

	// Obstruction
	m_fLastObstructionUpdate						= 0.0f;
	m_Obstruction.nRaysShot							= 0;
	m_Obstruction.bAssigned							= false;
	m_Obstruction.bProcessed						= false;
	m_Obstruction.bDelayPlayback				= true;
	m_Obstruction.bFirstTime						= true;
	m_Obstruction.ResetDirect();
	m_Obstruction.ResetReverb();

	if (!m_listeners.empty())
	{
		OnEvent( SOUND_EVENT_ON_STOP );
		m_listeners.clear();
	}

	m_listenersToBeRemoved.resize(0);

	if (m_pPlatformSound)
		m_pPlatformSound->FreeSoundHandle();

	m_pPlatformSound = NULL;

	if (m_pSoundBuffer)
		m_pSoundBuffer->RemoveFromLoadReqList(this);

	if (m_pEventGroupBuffer)
		m_pEventGroupBuffer->RemoveFromLoadReqList(this);

	m_pSoundBuffer = NULL;
	m_pEventGroupBuffer = NULL;
	m_sSoundName = "";
		
}

ESoundSystemErrorCode CSound::Initialize(CSoundBuffer* pInfoBuf, const unsigned int nFlags)
{
	m_nFlags = nFlags;

	if (nFlags & FLAG_SOUND_EVENT)
	{
		// min/max workaround after platform until there is support for that in Designer
		SSoundSettings NewSoundSettings;
		NewSoundSettings.Frequency = 0;
		NewSoundSettings.SoundSettings3D = NULL;
		NewSoundSettings.is3D = false;
		NewSoundSettings.bLoadData = false;
		NewSoundSettings.bLoadNonBlocking = true;
		NewSoundSettings.SpeakerPan = 0;

		//		flags |= FLAG_SOUND_EVENT;

		enumPlatformSoundErrorCodes eErrorCode = pssOK;
		if (pInfoBuf)
		{
			pInfoBuf->Load(NULL);
			eErrorCode = m_pPlatformSound->CreateSound(pInfoBuf->GetAssetHandle(), NewSoundSettings);
		}

		if (eErrorCode == pssOK)
		{
			float fMaxRadius = 5.0f; // default if event failed to prevent MaxRadius gets zero
			ptParamF32 fTemp(fMaxRadius);
			bool bRadius = m_pPlatformSound->GetParamByType(pspMAXRADIUS, &fTemp);
			fTemp.GetValue(fMaxRadius);
			SetMinMaxDistance_i(0.1f, fMaxRadius);

			if (bRadius)
				m_nFlags |= FLAG_SOUND_RADIUS;
			else
				m_nFlags &= ~FLAG_SOUND_RADIUS;

			// 3D/2D
			bool bIs3D = false;
			ptParamBOOL bTemp(bIs3D);
			m_pPlatformSound->GetParamByType(pspIS3D, &bTemp);
			bTemp.GetValue(bIs3D);
			if (bIs3D)
			{
				m_nFlags |= FLAG_SOUND_3D;
				m_nFlags &= ~FLAG_SOUND_2D;

			}
			else
			{
				m_nFlags |= FLAG_SOUND_2D;
				m_nFlags &= ~FLAG_SOUND_3D;
			}

			// retrieving priority from prototype
			int nPriority = 0;
			ptParamINT32 nTemp(nPriority);
			m_pPlatformSound->GetParamByType(pspPRIORITY, &nTemp);
			nTemp.GetValue(nPriority);
			SetSoundPriority(nPriority);

			// retrieving streaming type from prototype
			bool bIsStream = false;
			m_pPlatformSound->GetParamByType(pspISSTREAM, &bTemp);
			bTemp.GetValue(bIsStream);		

			if (bIsStream)
				m_nFlags |= FLAG_SOUND_STREAM;
			else
				m_nFlags &= ~FLAG_SOUND_STREAM;


			// retrieving loop mode from prototype
			bool bLoop = false;
			m_pPlatformSound->GetParamByType(pspLOOPMODE, &bTemp);
			bTemp.GetValue(bLoop);

			// if this is really a looped sound then length should be 0
			int nLengthInMS = 0;
			bool bLengthResult = m_pPlatformSound->GetParamByType(pspLENGTHINMS, &nTemp);
			nTemp.GetValue(nLengthInMS);

			if (bLoop)
			{
				if (bLengthResult && nLengthInMS > 0)
				{
					// wups, this is a oneshot sound that was marked not correctly
					// but with the current FMOD version granular loops have a lengths, so
					// we flag this sound as being a loop until FMOD cleans this up!
					m_nFlags |= FLAG_SOUND_LOOP;
					//nFlags &= ~FLAG_SOUND_LOOP;
					// remove this debug message for GDC09
					//Log(eSLT_Error, "<Sound> Found oneshot sound that was marked as non-oneshot! (%s)",szFile);
				}
				else
				{
					m_nFlags |= FLAG_SOUND_LOOP;
				}
			}
			else
			{
				if (bLengthResult)
				{
					if (nLengthInMS > 0)
					{
						// all fine, this is a oneshot
						m_nFlags &= ~FLAG_SOUND_LOOP;
					}
					else
					{
						// oneshot voice files use looping events to play in, so no problem on voices
						if (!(nFlags & FLAG_SOUND_VOICE))
						{
							// wups, this is a looping sound that was marked not correctly
							m_pSSys->Log(eSLT_Error, "<Sound> Found looped sound that was marked as oneshot! (%s)", GetName());
							//Log(eSLT_Error, "<Sound> Found looped sound that was marked as oneshot! (%s)",szFile);
							m_nFlags |= FLAG_SOUND_LOOP;
						}
					}
				}
			}

			if (m_nFlags & FLAG_SOUND_LOOP)
			{
				// set steal behaviour for looped sounds
				ptParamINT32 nBehaviour(5); // fail if quietest
				m_pPlatformSound->SetParamByType(pspSTEALBEHAVIOUR, &nBehaviour);
			}

			// getting doppler
			float fDoppler = 0.0f;
			m_pPlatformSound->GetParamByType(psp3DDOPPLERSCALE, &fTemp);
			fTemp.GetValue(fDoppler);
			if (fDoppler > 0)
				m_nFlags |= FLAG_SOUND_DOPPLER;
			else
				m_nFlags &= ~FLAG_SOUND_DOPPLER;

			// checking for speaker spread
			if (m_pPlatformSound->GetParamByType(pspSPREAD, &fTemp))
				m_nFlags |= FLAG_SOUND_PARAM_SPREAD;
			else
				m_nFlags &= ~FLAG_SOUND_PARAM_SPREAD;

			//checking for squelch parameter
			float fSquelch = 0.0f;
			if (m_pPlatformSound->GetParamByName("squelch", &fSquelch, false) != -1)
				m_nFlags |= FLAG_SOUND_PARAM_SQUELCH;
			else
				m_nFlags &= ~FLAG_SOUND_PARAM_SQUELCH;

			//checking for doppler parameter
			float fDopplerParam = 0.0f;
			if (m_pPlatformSound->GetParamByName("doppler_abs", &fDopplerParam, false) != -1)
				m_nFlags |= FLAG_SOUND_PARAM_DOPPLER_ABS;
			else
				m_nFlags &= ~FLAG_SOUND_PARAM_DOPPLER_ABS;

			if (m_pPlatformSound->GetParamByName("doppler_rel", &fDopplerParam, false) != -1)
				m_nFlags |= FLAG_SOUND_PARAM_DOPPLER_REL;
			else
				m_nFlags &= ~FLAG_SOUND_PARAM_DOPPLER_REL;

			//checking for environment parameter
			float fEnvironmentParam = 0.0f;
			if (m_pPlatformSound->GetParamByName("environment", &fEnvironmentParam, false) != -1)
				m_nFlags |= FLAG_SOUND_PARAM_ENVIRONMENT;
			else
				m_nFlags &= ~FLAG_SOUND_PARAM_ENVIRONMENT;

			//checking for xy plane rotation parameter
			float fFLRDegree = 0.0f;
			if (m_pPlatformSound->GetParamByName("front_left_rear", &fFLRDegree, false) != -1)
				m_nFlags |= FLAG_SOUND_PARAM_FRONTLEFTREAR;
			else
				m_nFlags &= ~FLAG_SOUND_PARAM_FRONTLEFTREAR;

			//checking for xy plane rotation parameter
			float fTRDDegree = 0.0f;
			if (m_pPlatformSound->GetParamByName("top_rear_down", &fTRDDegree, false) != -1)
				m_nFlags |= FLAG_SOUND_PARAM_TOPREARDOWN;
			else
				m_nFlags &= ~FLAG_SOUND_PARAM_TOPREARDOWN;

			//checking for xy plane rotation parameter
			float fTLDDegree = 0.0f;
			if (m_pPlatformSound->GetParamByName("top_left_down", &fTLDDegree, false) != -1)
				m_nFlags |= FLAG_SOUND_PARAM_TOPLEFTDOWN;
			else
				m_nFlags &= ~FLAG_SOUND_PARAM_TOPLEFTDOWN;


			// getting cone angles
			float fInner, fOuter;

			m_pPlatformSound->GetParamByType(pspCONEINNERANGLE, &fTemp);
			fTemp.GetValue(fInner);
			m_pPlatformSound->GetParamByType(pspCONEOUTERANGLE, &fTemp);
			fTemp.GetValue(fOuter);
			SetConeAngles(fInner, fOuter);

			m_pPlatformSound->FreeSoundHandle();

			// set that the Sound got initialized succesfully
			m_bInitialized = true;
		}
		else
		{
			if (eErrorCode == pssBUFFER_HANDLE_INVALID)
				return eSoundSystemErrorCode_ProjectNotReady; // cannot initialize now

			if ((m_pSSys->GetError() & eSoundSystemErrorCode_ProjectNotReady))
				return eSoundSystemErrorCode_ProjectNotReady; // cannot initialize now
			else
			{
				m_pSSys->Log(eSLT_Error, "<Sound> Create sound %s failed! Invalid platform sound.", GetName());
				
			}
			return eSoundSystemErrorCode_SoundNotKnown; // can never initialize
		}
	}
	else // for legacy wav files be sure not to mix 2D/3D sound buffers
	{

		//if ((!(flags & FLAG_SOUND_STREAM)) && (nit != m_soundBuffers.end()))	// we cannot share streams ! // TODO cant we really?
		if (!(m_nFlags & FLAG_SOUND_STREAM) )	// we cannot share streams ! // TODO cant we really?
		{
			//sound is already present

			if (m_nFlags & FLAG_SOUND_3D)
			{			
				if (m_pSoundBuffer->GetProps()->nFlags & FLAG_SOUND_2D)
				{
					m_pSSys->Log(eSLT_Error, "<Sound> Trying to load the same sound buffer file as 2d and 3d sound (%s)", GetName());
					return eSoundSystemErrorCode_SoundFlagsBad;
				}
			}
			else
				if (nFlags & FLAG_SOUND_2D)
				{
					if (m_pSoundBuffer->GetProps()->nFlags & FLAG_SOUND_3D)
					{
						m_pSSys->Log(eSLT_Error, "<Sound> Trying to load the same sound buffer file as 2d and 3d sound (%s)", GetName());
						return eSoundSystemErrorCode_SoundFlagsBad;
					}
				}
		}

		// set that the Sound got initialized succesfully
		m_bInitialized = true;

		/*
	// check if this is an ogg...to avoid people calling
	// loadsound and decompressing the entire ogg into memory (several megabytes)
	// but if the load sync is specified, load the sound anyway doesnt matter what
	if (!(m_nFlags & FLAG_SOUND_LOAD_SYNCHRONOUSLY))
	{
		size_t len = strlen(szFile)-1;
		const char *szExt=NULL;
		while (len)
		{
			if (szFile[len]=='.')
			{
				szExt = &szFile[len];
				break;
			}
			len--;
		}

		if (szExt && (stricmp(szExt,".ogg")==0) && (!(nFlags & FLAG_SOUND_STREAM)))
		{
			m_pSSys->Log(eSLT_Warning, "<Sound> WARNING - THE FILE %s is a streaming sound but there is an attempt to decompress it into memory!", GetName());
		}
	}
*/

	}

	return eSoundSystemErrorCode_None;

}

void CSound::SetPhysicsToBeSkipObstruction(EntityId *pSkipEnts,int nSkipEnts)
{
	for (int i=0; i<nSkipEnts && i<SOUND_MAX_SKIPPED_ENTITIES; ++i)
		m_Obstruction.pOnstructionSkipEntIDs[i] = pSkipEnts[i];

	m_Obstruction.nObstructionSkipEnts = min(nSkipEnts, SOUND_MAX_SKIPPED_ENTITIES);
}


void CSound::CalculateObstruction()
{
	// start with half of before
	float fDirectOld = m_Obstruction.GetDirect();
	float fReverbOld = m_Obstruction.GetReverb();
	m_Obstruction.ResetDirect();
	m_Obstruction.ResetReverb();
	//m_Obstruction.AddDirect(fDirect);
	//m_Obstruction.AddReverb(fReverb);

	// run through and disable those testoftest which hit something
	// check how many valid ray we got
	int nValidRays = m_Obstruction.nRaysShot;
	for (int i=0; i<m_Obstruction.nRaysShot && i<SOUND_MAX_OBSTRUCTION_TESTS; ++i)
	{
		SObstructionTest* pObstructionTest = &(m_Obstruction.ObstructionTests[i]);
		if (pObstructionTest->nTestForTest && pObstructionTest->nHits)
		{
			assert((unsigned int)pObstructionTest->nTestForTest < SOUND_MAX_OBSTRUCTION_TESTS);
			m_Obstruction.ObstructionTests[pObstructionTest->nTestForTest].bResult = false;
			--nValidRays;
		}
	}

	// the direct ray weights 50% of all rays for direct and 
	float fAdditionalWeight = 0.5f;
	float fDirectWeight = 1.0f;
	
	if (m_Obstruction.nRaysShot > 1)
	{
		fAdditionalWeight = (1.0f / ((m_Obstruction.nRaysShot-1)*2.0f));
		fDirectWeight = 1.0f - (m_Obstruction.nRaysShot-1) * fAdditionalWeight;
	}

	for (int i=0; i<m_Obstruction.nRaysShot; ++i)
	{
		SObstructionTest* pObstructionTest = &(m_Obstruction.ObstructionTests[i]);
		//pObstructionTest->nPierceability = 0;

		if (pObstructionTest->bResult && pObstructionTest->nTestForTest==0 && pObstructionTest->nHits)
		{
			//float fRatio = (m_RadiusSpecs.fMaxRadius - sqrt(pObstructionTest->fDistance))/m_RadiusSpecs.fMaxRadius;
			float fRatio = (m_RadiusSpecs.fMaxRadius - pObstructionTest->fDistance)/m_RadiusSpecs.fMaxRadius;
			
			if (m_pSSys->g_fObstructionMaxRadius > 0)
				fRatio *= (m_pSSys->g_fObstructionMaxRadius - m_RadiusSpecs.fMaxRadius) / m_pSSys->g_fObstructionMaxRadius;
			
			fRatio = clamp(fRatio, 0.0f, 1.0f);

			float fPierecabilityRatio = 1.0f;
			
			if (m_pSSys->g_fObstructionMaxPierecability > 0)
				fPierecabilityRatio = pObstructionTest->nPierceability / m_pSSys->g_fObstructionMaxPierecability;
			
			if (pObstructionTest->bDirect)
			{
				//m_Obstruction.AddDirect(fRatio*(pObstructionTest->nHits*0.3f)/(1.0f*(float)nValidRays));
				m_Obstruction.AddDirect(fPierecabilityRatio * fRatio * fDirectWeight);
				//m_Obstruction.AddReverb(fPierecabilityRatio* fRatio * 2.0f * fAdditionalWeight);
			}
			else
			{
				m_Obstruction.AddDirect(fPierecabilityRatio * fRatio * fAdditionalWeight);
				m_Obstruction.AddReverb(fPierecabilityRatio* fRatio * fAdditionalWeight * 2.0f);
			}
		}
	}

	// VisArea based Obstruction
	if (m_pSSys->g_fObstructionVisArea)
	{
		CListener* pListener = (CListener*)m_pSSys->GetListener(m_pSSys->GetClosestActiveListener(GetPlaybackPosition()));
		if (pListener && pListener->bActive)
		{
			int nVisObstruction = 0;

			// both inside
			if (m_pVisArea != pListener->GetVisArea())
			{
				int nSearchDepth = 3;
				if (m_pVisArea && pListener->GetVisArea())
				{
					if (!m_pVisArea->FindVisArea(pListener->GetVisArea(), nSearchDepth, true))
					{
						nSearchDepth = 5;
						if (!m_pVisArea->FindVisArea(pListener->GetVisArea(), nSearchDepth, true))
						{
							nVisObstruction = 3;
						}
						else
							nVisObstruction = 2;
					}
					else
						nVisObstruction = 1;
				}
			}

			// Listener outside, sound inside
			if (m_pVisArea && !pListener->GetVisArea())
			{
				// just approximate
				if (m_pVisArea->IsConnectedToOutdoor())
					nVisObstruction = 1;
				else
					nVisObstruction = 2;
			}

			// Listener inside, sound outside
			if (!m_pVisArea && pListener->GetVisArea())
			{
				// just approximate
				if (pListener->GetVisArea()->IsConnectedToOutdoor())
					nVisObstruction = 1;
				else
					nVisObstruction = 2;
			}

			m_Obstruction.AddDirect(nVisObstruction*m_pSSys->g_fObstructionVisArea*0.5f);
			m_Obstruction.AddReverb(nVisObstruction*m_pSSys->g_fObstructionVisArea);
		}
	}

	//m_Obstruction.fObstruction = (nHitsAccum*0.4f)/(1.0f*(float)m_Obstruction.nRaysShot); 
	float fDirectNew = min(m_Obstruction.GetAccumulatedDirect(), m_pSSys->g_fObstructionMaxValue);
	float fReverbNew = min(m_Obstruction.GetAccumulatedReverb(), m_pSSys->g_fObstructionMaxValue);

	// first time do not average with old value
	if (m_Obstruction.bDontAveragePrevious)
	{
		fDirectOld = fDirectNew;
		fReverbOld = fReverbNew;
	}

	m_Obstruction.SetDirect( (fDirectNew + fDirectOld)*0.5f );
	m_Obstruction.SetReverb( (fReverbNew + fReverbOld)*0.5f );

	//if ( abs(fDirectOld-m_Obstruction.GetDirect()) > 0.01f || abs(fDirectOld-m_Obstruction.GetDirect()) )
		m_Obstruction.bAssigned = false;
	
	m_Obstruction.LastUpdateTime	= gEnv->pTimer->GetFrameStartTime();
}

void CSound::ProcessObstruction(CListener *pListener, bool bImmediatly)
{
	assert(pListener!=NULL);
	
	if (!m_pSSys->g_nObstruction)
	{
		m_Obstruction.bDelayPlayback = false;
		m_Obstruction.nRaysShot = 0;
		return;
	}

	if (!(m_nFlags & FLAG_SOUND_OBSTRUCTION) && !(m_nFlags & FLAG_SOUND_3D))
	{
		m_Obstruction.bDelayPlayback = false;
		return;
	}

	if (m_Obstruction.bProcessed)
	{
		m_Obstruction.bDelayPlayback = false;
		return;
	}

	Vec3 vSoundDir = m_vPlaybackPos - pListener->GetPosition();

	// can we optimize a little and cull sounds which have a radius smaller than the current distance?
	if (vSoundDir.IsZero() || (m_nFlags & FLAG_SOUND_RADIUS && (vSoundDir.IsZero() || (vSoundDir.GetLengthSquared() > m_RadiusSpecs.fMaxRadius2)) && !bImmediatly))
	{
		m_Obstruction.bDelayPlayback = false;
		m_Obstruction.nRaysShot = 0;
		return;
	}

	CTimeValue currTime	= gEnv->pTimer->GetFrameStartTime();
	int ntimeDiffMS = (int)((currTime - m_Obstruction.LastUpdateTime).GetMilliSeconds());

	if (abs(ntimeDiffMS) > 100)
	{

		// evaluate all results gathered till now
		CalculateObstruction();

		//if (m_Obstruction.nObstructionSkipEnts>0)
			//int test = 0;

		IPhysicalEntity *pSkipEnts[SOUND_MAX_SKIPPED_ENTITIES+1];
		int nSkipEnts = 0;

		// add player skipent if valid, must be *FIRST* of other, ask Anton why
		IEntity *pEntity = gEnv->pEntitySystem->GetEntity(pListener->nEntityID);
		if (pEntity)
		{
			IPhysicalEntity* pPhysicalEntity = pEntity->GetPhysics();
			if (pPhysicalEntity)
			{
				pSkipEnts[nSkipEnts] = pPhysicalEntity;
				++nSkipEnts;
			}
		}

		// test if SkipEnts still valid and add them
		if (m_Obstruction.nObstructionSkipEnts)
		{
			for (int i=0; i<m_Obstruction.nObstructionSkipEnts; ++i)
			{
				pEntity = gEnv->pEntitySystem->GetEntity(m_Obstruction.pOnstructionSkipEntIDs[i]);
				if (pEntity)
				{
					IPhysicalEntity* pPhysicalEntity = pEntity->GetPhysics();
					if (pPhysicalEntity)
					{
						pSkipEnts[nSkipEnts] = pPhysicalEntity;
						++nSkipEnts;
					}
				}
			}
		}
		
		PREFAST_SUPPRESS_WARNING(6326) if (SOUND_MAX_OBSTRUCTION_TESTS < 9)
			return;

		int nObInx = 0;

		int nAccuracy = m_pSSys->g_nObstructionAccuracy;

		if (nAccuracy == 0)
		{
			m_Obstruction.bDelayPlayback = false;
			return;
		}

		Vec3 vSoundDirNorm = vSoundDir;
		vSoundDirNorm.normalize();

		if (nAccuracy > 0)
		{
			m_Obstruction.ObstructionTests[nObInx].vOrigin = pListener->GetPosition();
			m_Obstruction.ObstructionTests[nObInx].vDirection = vSoundDir - (vSoundDirNorm*0.01f); // shorten ray by a cm
			m_Obstruction.ObstructionTests[nObInx].bDirect = true;
			m_Obstruction.ObstructionTests[nObInx].bResult = true;
			++nObInx;
		}

		if (!bImmediatly)
		{
			float fRadius = sqrtf(sqrtf(m_RadiusSpecs.fMaxRadius2));
			//vSoundDir *= fRadius;
			assert(vSoundDirNorm.GetLength()!=0);

			if (nAccuracy > 1)
			{			
				// test the sidetest
				m_Obstruction.ObstructionTests[nObInx].vOrigin = m_vPlaybackPos;
				m_Obstruction.ObstructionTests[nObInx].vDirection.x = vSoundDirNorm.y*fRadius;
				m_Obstruction.ObstructionTests[nObInx].vDirection.y = -vSoundDirNorm.x*fRadius;
				m_Obstruction.ObstructionTests[nObInx].vDirection.z = 0;
				m_Obstruction.ObstructionTests[nObInx].bDirect = false;
				m_Obstruction.ObstructionTests[nObInx].bResult = true;
				++nObInx;
				m_Obstruction.ObstructionTests[nObInx-1].nTestForTest = nObInx;

				// test to the side
				m_Obstruction.ObstructionTests[nObInx].vOrigin = pListener->GetPosition();
				m_Obstruction.ObstructionTests[nObInx].vDirection = 
					m_Obstruction.ObstructionTests[nObInx-1].vDirection + m_Obstruction.ObstructionTests[0].vDirection;
				m_Obstruction.ObstructionTests[nObInx].bDirect = false;
				m_Obstruction.ObstructionTests[nObInx].bResult = true;
				++nObInx;
			}

			if (nAccuracy > 2)
			{
				// test the other sidetest
				m_Obstruction.ObstructionTests[nObInx].vOrigin = m_vPlaybackPos;
				m_Obstruction.ObstructionTests[nObInx].vDirection.x = -vSoundDirNorm.y*fRadius;
				m_Obstruction.ObstructionTests[nObInx].vDirection.y = vSoundDirNorm.x*fRadius;
				m_Obstruction.ObstructionTests[nObInx].vDirection.z = 0;
				m_Obstruction.ObstructionTests[nObInx].bDirect = false;
				m_Obstruction.ObstructionTests[nObInx].bResult = true;
				++nObInx;
				m_Obstruction.ObstructionTests[nObInx-1].nTestForTest = nObInx;

				// test to the other side
				m_Obstruction.ObstructionTests[nObInx].vOrigin = pListener->GetPosition();
				m_Obstruction.ObstructionTests[nObInx].vDirection = 
					m_Obstruction.ObstructionTests[nObInx-1].vDirection + m_Obstruction.ObstructionTests[0].vDirection;
				m_Obstruction.ObstructionTests[nObInx].bDirect = false;
				m_Obstruction.ObstructionTests[nObInx].bResult = true;
				++nObInx;
			}

			if (nAccuracy > 3)
			{
				// test the uptest
				m_Obstruction.ObstructionTests[nObInx].vOrigin = m_vPlaybackPos;
				m_Obstruction.ObstructionTests[nObInx].vDirection.x = 0;
				m_Obstruction.ObstructionTests[nObInx].vDirection.y = 0;
				m_Obstruction.ObstructionTests[nObInx].vDirection.z = fRadius;
				m_Obstruction.ObstructionTests[nObInx].bDirect = false;
				m_Obstruction.ObstructionTests[nObInx].bResult = true;
				++nObInx;
				m_Obstruction.ObstructionTests[nObInx-1].nTestForTest = nObInx;

				// test up
				m_Obstruction.ObstructionTests[nObInx].vOrigin = pListener->GetPosition();
				m_Obstruction.ObstructionTests[nObInx].vDirection = 
					m_Obstruction.ObstructionTests[nObInx-1].vDirection + m_Obstruction.ObstructionTests[0].vDirection;
				m_Obstruction.ObstructionTests[nObInx].bDirect = false;
				m_Obstruction.ObstructionTests[nObInx].bResult = true;
				++nObInx;
			}

			if (nAccuracy > 4)
			{
				// test the downtest
				m_Obstruction.ObstructionTests[nObInx].vOrigin = m_vPlaybackPos;
				m_Obstruction.ObstructionTests[nObInx].vDirection.x = 0;
				m_Obstruction.ObstructionTests[nObInx].vDirection.y = 0;
				m_Obstruction.ObstructionTests[nObInx].vDirection.z = -fRadius*0.5f;
				m_Obstruction.ObstructionTests[nObInx].bDirect = false;
				m_Obstruction.ObstructionTests[nObInx].bResult = true;
				++nObInx;
				m_Obstruction.ObstructionTests[nObInx-1].nTestForTest = nObInx;

				// test down
				m_Obstruction.ObstructionTests[nObInx].vOrigin = pListener->GetPosition();
				m_Obstruction.ObstructionTests[nObInx].vDirection = 
					m_Obstruction.ObstructionTests[nObInx-1].vDirection + m_Obstruction.ObstructionTests[0].vDirection;
				m_Obstruction.ObstructionTests[nObInx].bDirect = false;
				m_Obstruction.ObstructionTests[nObInx].bResult = true;
				++nObInx;
			}
		}


		//float fTempObstruction = m_Obstruction.fObstructionAccu;
		//m_Obstruction.fObstructionAccu *= 0.5f; // start with half of before

		int nHitsAccum = 0;
		m_Obstruction.nRaysShot = nObInx;
		unsigned int nPhysicsFlags = ent_water|ent_static|ent_sleeping_rigid|ent_rigid|ent_terrain;

		// make sure that we use non-blocking RWI
		assert(!bImmediatly);

		if (bImmediatly)
		{
			// taking first guess of obstruction
			// only one RWI on Timurs request

			SObstructionTest* pObstructionTest = &(m_Obstruction.ObstructionTests[0]);

			ray_hit hits[10];
			int rayresult = gEnv->pPhysicalWorld->RayWorldIntersection(pObstructionTest->vOrigin, pObstructionTest->vDirection, nPhysicsFlags, rwi_pierceability0, &hits[0], 10, pSkipEnts, nSkipEnts, NULL, PHYS_FOREIGN_ID_SOUND_OBSTRUCTION);

			pObstructionTest->nHits = rayresult;
			pObstructionTest->bResult = true;
			if (rayresult)
			{
				float fDistance = FLT_MAX;
				for (int j=0; j<10; j++)
				{
					if (hits[j].dist > 0)
					{
//						IEntity* pTemp2 = NULL;
//						IPhysicalEntity* pTemp = hits[j].pCollider;
//						if (pTemp)
//							pTemp2 = (IEntity*) pTemp->GetForeignData(PHYS_FOREIGN_ID_ENTITY);
						//pTemp->GetForeignData();
						//pTemp->GetiForeignData();
						fDistance = min(fDistance, hits[j].dist);
					}
				}

				pObstructionTest->fDistance = fDistance;
			}
		}

		else
		{
			m_Obstruction.bDelayPlayback = true;

			for (int i=0; i<m_Obstruction.nRaysShot; ++i)
			{
				SObstructionTest* pObstructionTest = &(m_Obstruction.ObstructionTests[i]);
				pObstructionTest->nPierceability = 0;

				if (pObstructionTest->bResult)
				{
					//AddRef(); // making that sound to live until the result is ready
					// only new rwi if the last one was computed
					pObstructionTest->SoundID = GetId();
					gEnv->pPhysicalWorld->RayWorldIntersection(pObstructionTest->vOrigin, pObstructionTest->vDirection, nPhysicsFlags, rwi_pierceability0|rwi_queue, NULL, 10, pSkipEnts, nSkipEnts, pObstructionTest, PHYS_FOREIGN_ID_SOUND_OBSTRUCTION);
					pObstructionTest->bResult = false;
				}
				nHitsAccum += m_Obstruction.ObstructionTests[i].nHits;
			}
		}

		if (bImmediatly)
		{
			// evaluate all results gathered till now
			m_Obstruction.bProcessed = true;
			m_Obstruction.bDelayPlayback = false;
			CalculateObstruction();
		}
		else
			m_Obstruction.bProcessed = false;
	}

	// check for closest listener?? )

	//if(pEntity)
	//myPhysicalEntity = pEntity->GetPhysics();


}

void CSound::Update()
{
	//GUARD_HEAP;
	FUNCTION_PROFILER( gEnv->pSystem,PROFILE_SOUND );

	assert (m_pPlatformSound != NULL); 
	assert (m_pSoundBuffer != NULL); 
	
	// Check if platform sound has

	// Check state of platform sound is INFOONLY
	if (m_pPlatformSound && m_pPlatformSound->GetState() == pssINFOONLY)
	{
		m_pPlatformSound->FreeSoundHandle();
		SetState(eSoundState_Stopped);
	}

	// Check state of platform sound ended by its own
	if (m_pPlatformSound && m_pPlatformSound->GetState() == pssSTOPPED)
	{
		m_pPlatformSound->FreeSoundHandle();
		SetState(eSoundState_Stopped);
	}

	// Check state if programmer-sound ended 
	if (GetState() == eSoundState_ProgrammerSoundReleased)
	{
		SetState(eSoundState_Ending);
	}

	// check if shutdown ended
	if (GetState() == eSoundState_Ending || (m_bStopCalled && GetState() != eSoundState_Stopped))
	{
		//assert (GetState() == eSoundState_Ending);
		bool bStopAtOnce = false;

		// stopped by syncpoint
		if (m_pPlatformSound && m_pPlatformSound->GetState() == pssSYNCARRIVED)
			bStopAtOnce = true;

		// syncpoint time out
		if (m_fSyncTimeout >= 0.0f)
		{
			m_fSyncTimeout -= gEnv->pTimer->GetFrameTime();
			if (m_fSyncTimeout < 0)
				bStopAtOnce = true;
		}

		if (bStopAtOnce)
			Stop(ESoundStopMode_AtOnce);

		if (!IsPlayingOnChannel())
		{
			SetState(eSoundState_Stopped);
		}
		else
		{
			SetState(eSoundState_Ending);
			m_pPlatformSound->StopSound();
			UpdateVoiceProperties(true);
		}
	}

	// Sound got stolen or other error
	bool bStolen = false;
	if (m_pPlatformSound && (m_pPlatformSound->GetState() == pssSTOLEN || m_pPlatformSound->GetState() == pssERROR))
	{
		bStolen = true;

		m_pPlatformSound->FreeSoundHandle();

		//if (IsActive() && (m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_SIMPLE))
			//m_pSSys->Log(eSLT_Message, "<Sound> Stop sound: %s, ID:%d",bStolen?" stolen":"",GetName(), GetId());
	}
	

	if (GetState() == eSoundState_Stopped)
	{
		// dealing with static sounds
		if (GetISystem() && gEnv->pEntitySystem)
		{
			IEntity *pEntity = gEnv->pEntitySystem->GetEntity(m_SoundProxyEntityId);
			if (pEntity)
			{
				IEntitySoundProxy *pSoundProxy = (IEntitySoundProxy*)pEntity->GetProxy(ENTITY_PROXY_SOUND);
				if (pSoundProxy)
				{
					if (pSoundProxy->GetStaticSound(GetId()))
					{
						Deactivate();
						SetState(eSoundState_Inactive);
						//SetPaused(true);
						return;
					}
				}
			}
		}

		if (!(GetFlags() & FLAG_SOUND_LOOP) || m_bStopCalled)
			m_bPlaying = false;

		if (!m_bPlaying && !m_bSentOnStopEvent)
		{
			OnEvent( SOUND_EVENT_ON_STOP );	
			m_bSentOnStopEvent = true;
		}

		return;
	} // stopped

	// Get the Listener
	CListener* pListener = (CListener*)m_pSSys->GetListener(m_pSSys->GetClosestActiveListener(m_vPlaybackPos));

	if (!pListener)
		return;

	// TODO Double check? This is only for 3D Sounds
	//if (m_bPositionHasChanged || m_pSSys->GetSoundGroupPropertiesChanged() || FLAG_SOUND_SELFMOVING)  
	if (m_bPositionHasChanged || m_vSpeed.GetLengthSquared() || (m_nFlags & FLAG_SOUND_SELFMOVING) || m_bFlagsChanged)
	{
		if (m_bFlagsChanged)
			SetPosition(m_vPosition);

		UpdatePositionAndVelocity();
		
		assert (m_pPlatformSound != NULL);
		if (m_pPlatformSound)
		{
			float fDoppler;
			if (m_nFlags & FLAG_SOUND_PARAM_DOPPLER_REL)
			{
				Vec3 const vRelativeVelocity	= pListener->GetVelocity() - m_vSpeed;
				Vec3 const vCurrentDist				=	pListener->GetPosition() - m_vPosition;
				fDoppler											= vRelativeVelocity.GetLength() * ((m_vSpeed.Dot(vCurrentDist) > 0.0f) ? 3.6f : -3.6f); // Km/h (positive fDoppler means both approach each other)
				m_pPlatformSound->SetParamByName("doppler_rel", fDoppler, false);
			}

			if (m_nFlags & FLAG_SOUND_PARAM_DOPPLER_ABS)
			{
				fDoppler = m_vSpeed.GetLength() * 3.6f; // Km/h
				m_pPlatformSound->SetParamByName("doppler_abs", fDoppler, false);
			}
		}

		// invalidate current obstruction
		if (pListener->bMoved)
			m_Obstruction.bProcessed = false;
	}

	//recompute VisArea if position changed or VisAreas were invalidated
	if (m_bRecomputeVisArea)
	{
		I3DEngine* p3dEngine = gEnv->p3DEngine;

		if (p3dEngine)		
			m_pVisArea = p3dEngine->GetVisAreaFromPos(m_vPlaybackPos);		
		
		m_bRecomputeVisArea = false;
	}

	if (GetFlags() & FLAG_SOUND_3D && GetFlags() & FLAG_SOUND_OBSTRUCTION)  //only 3D sounds can have obstruction
	{
		// only update every defined timestep
		float fCurrTime = gEnv->pTimer->GetCurrTime();
		float fSeconds = m_tPlayTime.GetSeconds();
		if (fSeconds > 0 && (fCurrTime - m_fLastObstructionUpdate > m_pSSys->g_fObstructionUpdate))
		{
			ProcessObstruction(pListener, false);
			m_fLastObstructionUpdate = fCurrTime;
		}
	}

	// update global parameter
	if (m_pPlatformSound)
		m_pSSys->m_ParameterNames.ApplyGlobalParameters(m_pPlatformSound);

	// Player direction Parameters
	if (GetFlags() & FLAG_SOUND_PARAM_FRONTLEFTREAR || GetFlags() & FLAG_SOUND_PARAM_FRONTLEFTREAR || GetFlags() & FLAG_SOUND_PARAM_FRONTLEFTREAR)
	{

		Matrix34 mInv = m_mTransformation;
		mInv.InvertFast();
		Vec3 v3RelativeListenerPosition(mInv.TransformPoint(pListener->GetPosition()));

		// update the direction of the player to the sound in degree
		if (GetFlags() & FLAG_SOUND_PARAM_FRONTLEFTREAR)
		{
			Vec2 v2SoundDirection(0, 1); // Forward.
			Vec2 v2ListenerDirection(v3RelativeListenerPosition.x, v3RelativeListenerPosition.y);
			v2ListenerDirection.NormalizeSafe();

			float fCos = v2ListenerDirection.Dot(v2SoundDirection);
			float fAngle = RAD2DEG(acosf(fCos));

			Vec2 vPerpendicular(-v2SoundDirection.y, v2SoundDirection.x);

			float fPerpendicularDot = v2ListenerDirection.Dot(vPerpendicular);
			if (fPerpendicularDot < 0)
			{
				fAngle = 360.0f - fAngle;
			}

			if (m_pPlatformSound)
				m_pPlatformSound->SetParamByName("front_left_rear", fAngle);
		}

		// update the direction of the player to the sound in degree
		if (GetFlags() & FLAG_SOUND_PARAM_TOPREARDOWN)
		{
			// invert y (2D) so have correct counterclock degree
			Vec2 v2SoundDirection(1, 0); // Up.
			Vec2 v2ListenerDirection(v3RelativeListenerPosition.z, -v3RelativeListenerPosition.y);
			v2ListenerDirection.NormalizeSafe();

			float fCos = v2ListenerDirection.Dot(v2SoundDirection);
			float fAngle = RAD2DEG(acosf(fCos));

			Vec2 vPerpendicular(-v2SoundDirection.y, v2SoundDirection.x);

			float fPerpendicularDot = v2ListenerDirection.Dot(vPerpendicular);
			if (fPerpendicularDot < 0)
			{
				fAngle = 360.0f - fAngle;
			}

			if (m_pPlatformSound)
				m_pPlatformSound->SetParamByName("top_rear_down", fAngle);
		}

		// update the direction of the player to the sound in degree
		if (GetFlags() & FLAG_SOUND_PARAM_TOPLEFTDOWN)
		{
			// invert y (2D) so have correct counterclock degree
			Vec2 v2SoundDirection(1, 0); // Up.
			Vec2 v2ListenerDirection(v3RelativeListenerPosition.z, -v3RelativeListenerPosition.x);
			v2ListenerDirection.NormalizeSafe();

			float fCos = v2ListenerDirection.Dot(v2SoundDirection);
			float fAngle = RAD2DEG(acosf(fCos));

			Vec2 vPerpendicular(-v2SoundDirection.y, v2SoundDirection.x);

			float fPerpendicularDot = v2ListenerDirection.Dot(vPerpendicular);
			if (fPerpendicularDot < 0)
			{
				fAngle = 360.0f - fAngle;
			}

			if (m_pPlatformSound)
				m_pPlatformSound->SetParamByName("top_left_down", fAngle);
		}
	}



	UpdateSoundProperties();
	UpdateVoiceProperties(false);

	if (m_bLoopModeHasChanged && m_pPlatformSound)
	{
		if (IsActive())
		{
			ptParamBOOL NewParam(GetLoopMode());
			if (!m_pPlatformSound->SetParamByType(pspLOOPMODE, &NewParam))
			{
				// throw error
			}
		}
	}

	m_bVolumeHasChanged		= false;
	m_bPositionHasChanged	= false;
	m_bLoopModeHasChanged	= false;
	m_bFlagsChanged       = false;


	// touch the sound
	if (m_bPlaying && m_pSoundBuffer)
		m_pSoundBuffer->UpdateTimer(gEnv->pTimer->GetFrameStartTime());


}

void CSound::SetSoundBufferPtr(CSoundBuffer* pNewSoundBuffer)
{ 
	if (m_pSoundBuffer)
		m_pSoundBuffer->RemoveFromLoadReqList(this);

	m_pSoundBuffer = pNewSoundBuffer; 
	m_pSoundBuffer->AddToLoadReqList(this);

	/*if (m_pSoundBuffer->GetProps()->eBufferType = btEVENT)
	{
		m_pSoundBuffer->Load(NULL);
		m_pSoundBuffer->GetAssetHandle();

	}*/
}

//load the sound for real
//////////////////////////////////////////////////////////////////////
bool CSound::Preload()
{
	assert (m_pSoundBuffer != NULL); 

	//if (!m_pSSys->IsEnabled())
	//{
	//	OnEvent( SOUND_EVENT_ON_LOADED );
	//	return true;
	//}

	bool bResult = false;

	if (m_pSoundBuffer)
		bResult = m_pSoundBuffer->Load(this);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////
void CSound::SetSoundPriority(int nSoundPriority)
{
	m_nSoundPriority = nSoundPriority;

	//if (nSoundPriority==255)
	//{
		// keep in memory all sounds with highest priority
		// Not needed.
		//m_nFlags|=FLAG_SOUND_LOAD_SYNCHRONOUSLY;
		//Preload(); 
	//}	
}

//! set soundinfo struct for voice sounds
void CSound::SetVoiceSpecs( SLocalizedSoundInfoGame *pSoundInfo)
{
	// remove old entry
	if (pSoundInfo == NULL)
	{
		if (m_VoiceSpecs.pSoundInfo)
		{
			if (m_VoiceSpecs.pSoundInfo->pSoundMoods)
				delete[] m_VoiceSpecs.pSoundInfo->pSoundMoods;

			if (m_VoiceSpecs.pSoundInfo->pEventParameters)
				delete[] m_VoiceSpecs.pSoundInfo->pEventParameters;

			delete m_VoiceSpecs.pSoundInfo;
			m_VoiceSpecs.pSoundInfo = NULL;
		}

		return;
	}

	// check if old entry exist and remove before setting new one
	if (m_VoiceSpecs.pSoundInfo)
		SetVoiceSpecs(NULL);

	m_VoiceSpecs.pSoundInfo = pSoundInfo;

	// keep volume in struct to prevent null-pointer checks on pSoundInfo across the code
	m_VoiceSpecs.fVolume = pSoundInfo->fVolume;
	m_VoiceSpecs.sCleanEventName	= pSoundInfo->sSoundEvent;
}


//calc the sound volume taking into account game sound attenuation,
//reduction ratio, sound system sfx volume, master volume (concentration feature)
//////////////////////////////////////////////////////////////////////
float CSound::CalcSoundVolume(float fSoundVolume,float fRatio) const
{
	//FRAME_PROFILER( "CSound:CalcSoundVolume",m_pSSys->GetSystem(),PROFILE_SOUND );

	float fVolume;

	for (int i=0;i<MAX_SOUNDSCALE_GROUPS;i++)
	{
		if (m_nSoundScaleGroups & SETSCALEBIT(i))
			fRatio	*=	m_pSSys->m_fSoundScale[i];
	}

	fVolume = fSoundVolume * fRatio * m_HDRA;

	if (m_nFlags & FLAG_SOUND_VOICE)
		fVolume = fVolume * m_pSSys->m_fDialogVolume * m_pSSys->m_fGameDialogVolume * m_VoiceSpecs.fVolume;
	else
		fVolume =  fVolume * m_pSSys->m_fSFXVolume * m_pSSys->m_fGameSFXVolume;

	if (!(m_nFlags & FLAG_SOUND_MOVIE))
		fVolume *= m_pSSys->m_fMovieFadeoutVolume;

	return (fVolume);
}

void CSound::UpdateVoiceProperties(bool bSoundEnded)
{

	if ((GetFlags() & FLAG_SOUND_VOICE) && m_VoiceSpecs.pSoundInfo)
	{
		// Apply SoundMoods
		if (m_pSSys->m_pMoodManager)
		{
			for (int i = 0; i < m_VoiceSpecs.pSoundInfo->nNumSoundMoods; ++i)
			{
				float fNewFade = 0.0f;
				float fCurrentFade = 0.0f;

				if (m_VoiceSpecs.pSoundInfo->pSoundMoods[i].fValue > 0.0f)
				{
					m_pSSys->m_pMoodManager->RegisterSoundMood(m_VoiceSpecs.pSoundInfo->pSoundMoods[i].sName.c_str());
					fCurrentFade = m_pSSys->m_pMoodManager->GetSoundMoodFade(m_VoiceSpecs.pSoundInfo->pSoundMoods[i].sName.c_str());
					fNewFade = max(fCurrentFade, m_VoiceSpecs.pSoundInfo->pSoundMoods[i].fValue);

					if (bSoundEnded)
						fNewFade = 0.0f; // fade out the effect when the sound stopped
				}

				// only update soundmood if needed
				if (fNewFade > fCurrentFade || bSoundEnded)
					m_pSSys->m_pMoodManager->UpdateSoundMood(m_VoiceSpecs.pSoundInfo->pSoundMoods[i].sName.c_str(), fNewFade, 200);

			}
		}

		if (!bSoundEnded)
		{
			// Apply EventParameters
			for (int i = 0; i < m_VoiceSpecs.pSoundInfo->nNumEventParameters; ++i)
			{
				if (m_VoiceSpecs.pSoundInfo->pEventParameters[i].fValue > 0.0f)
				{
					SetParam(m_VoiceSpecs.pSoundInfo->pEventParameters[i].sName.c_str(), m_VoiceSpecs.pSoundInfo->pEventParameters[i].fValue, false);
				}
			}
		}
	}

}

//////////////////////////////////////////////////////////////////////
bool CSound::UpdateSoundProperties()
{
	assert (m_pPlatformSound != NULL);

	// MasterVolume is set by SoundSystem directly to FMOD
	//int nNewVolume = (int)(m_nMaxVolume * SoundGroupProps.fCurrentVolumeRatio); 
	
	// temporally obstruction effect done by volume
	float fRatio = m_fRatio;

	// This may be also obstruction coming in from soundmoods
	if (m_nFlags & FLAG_SOUND_3D && m_nFlags & FLAG_SOUND_OBSTRUCTION && m_pPlatformSound) 
	{

		// real DSP based obstruction
		if ((m_pSSys->g_nObstruction == 1))
		{
			if (!m_Obstruction.bAssigned)
			{
				m_pPlatformSound->SetObstruction(&m_Obstruction);
			}
			//m_Obstruction.bAddObstruction = true;

			// additional volume based Obstruction
			//fRatio = max(1.0f - m_pSSys->g_fObstructionMaxValue, min(1.0f - m_Obstruction.GetDirect()*0.4f - m_Obstruction.GetReverb()*0.1f, 1.0f)) * m_fRatio; 
			fRatio = m_fRatio;

		}

		// stronger volume based Obstruction for fallback
		if ((m_pSSys->g_nObstruction == 2))
		{
			fRatio = max(1.0f - m_pSSys->g_fObstructionMaxValue, min(1.0f - m_Obstruction.GetDirect()*0.6f - m_Obstruction.GetReverb()*0.7f, 1.0f)) * m_fRatio; 
		}

		m_bVolumeHasChanged = true;
		m_Obstruction.bAssigned = true;
	}

	if (m_bVolumeHasChanged || m_fActiveVolume == -1.0f || m_nFadeTimeInMS)
	{
		m_fActiveVolume = CalcSoundVolume(m_fMaxVolume, fRatio);

		if (m_nFadeTimeInMS > 0 )
		{
			int nDifferenceMS = (int) (gEnv->pTimer->GetFrameTime() * 1000);

			float fTimeRatio = (min(nDifferenceMS, m_nFadeTimeInMS)) / float(m_nFadeTimeInMS);

			if (m_nFadeTimeInMS > nDifferenceMS)
				m_nFadeTimeInMS -= nDifferenceMS;

			//float fRatio = (m_fFadeCurrent + m_fFadeGoal)/2.0f;
			float fDiff = m_fFadeGoal-m_fFadeCurrent;

			m_fFadeCurrent += fDiff*fTimeRatio;

			m_fFadeCurrent = min(1.0f,m_fFadeCurrent);
			m_fFadeCurrent = max(0.0f,m_fFadeCurrent);	

			if (fDiff < 0.0f)
				m_eFadeState = eFadeState_FadingOut;

			if (fDiff > 0.0f)
				m_eFadeState = eFadeState_FadingIn;


			if (m_fFadeCurrent == m_fFadeGoal)
			{
				m_nFadeTimeInMS = 0;

				if (m_eFadeState != eFadeState_JustFinished)
					m_eFadeState = eFadeState_JustFinished;
			}

			m_fActiveVolume *= m_fFadeCurrent;
		}
		else
		{
			m_eFadeState = eFadeState_None;
		}

		UpdateActiveVolume();

		if (m_pPlatformSound && m_pPlatformSound->GetState() == pssINFOONLY)
			m_fActiveVolume = -1.0f;
	}

	return (true);
}

//////////////////////////////////////////////////////////////////////
bool CSound::IsPlayingOnChannel() const
{
	if (!IsActive() || !m_pPlatformSound)
		return false;

	// if this sound is not even allocated in the sound library then return false
	if (!m_pPlatformSound->GetSoundHandle())
		return false;

	//GUARD_HEAP;

	bool bIsPlayingOnChannel = false;
	ptParamBOOL NewParam(bIsPlayingOnChannel);
	m_pPlatformSound->GetParamByType(pspISPLAYING, &NewParam);
	NewParam.GetValue(bIsPlayingOnChannel);

	//bool bIsPaused = false;
	//ptParamBOOL NewParam(bIsPlayingOnChannel);
	//m_pPlatformSound->GetParam(pspISPLAYING, &NewParam);
	//NewParam.GetValue(bIsPlayingOnChannel);

	if (bIsPlayingOnChannel || m_IsPaused)
	{
		//gEnv->pLog->LogToConsole("channel %d IS playing!!",m_nChannel);
		return (true);
	}

	//gEnv->pLog->LogToConsole("channel %d,not playing!!",m_nChannel);
	return (false);	
}

//////////////////////////////////////////////////////////////////////////
bool CSound::IsPlayingVirtual() const
{
	bool bDebug1 = IsPlaying();
	bool bDebug2 = GetLoopMode();
	return (bDebug1 && bDebug2);
}
 
//////////////////////////////////////////////////////////////////////
void CSound::Play(float fRatio, bool bForceActiveState, bool bSetRatio, IEntitySoundProxy *pEntitySoundProxy)
{
	if (!NumberValid(fRatio))
	{
		m_pSSys->Log(eSLT_Warning, "<Sound> %s Invalid input values (possible NaNs) on Sound::Play()", GetName() );
		assert(0);
		return;
	}

	int ntest = GetLengthMs();

	//GUARD_HEAP;
  	FUNCTION_PROFILER( gEnv->pSystem,PROFILE_SOUND );

	assert (m_pPlatformSound != NULL); 
	assert (m_pSoundBuffer != NULL); 
	assert (m_eSemantic != eSoundSemantic_None);

	if (pEntitySoundProxy) // for soundinfo reason
		m_SoundProxyEntityId = pEntitySoundProxy->GetEntity()->GetId();

	//CTimeValue test = gEnv->pTimer->GetFrameStartTime();
	//gEnv->pLog->Log("DEBUG: Sound started to be played: %s : %f \n", GetName(), test.GetSeconds());

//	fRatio = 0.0;

	if (!m_pSoundBuffer || m_pSoundBuffer->LoadFailure())
	{
		// load failure proceedure
		OnEvent( SOUND_EVENT_ON_LOAD_FAILED );
		return;
	}

	if (m_pSSys->g_nProfiling > 0)
	{
		ISoundProfileInfo* pSoundInfo = NULL;

		if (m_pPlatformSound  && (m_pPlatformSound->GetState() == pssNONE))
		{
			CSoundSystem::tmapSoundInfo::iterator It = m_pSSys->m_SoundInfos.find(CONST_TEMP_STRING(GetName()));

			if (It != m_pSSys->m_SoundInfos.end())
				pSoundInfo = (*It).second;

			if (pSoundInfo)
			{
				++pSoundInfo->GetInfo()->nTimesPlayed;
			}
		}
	}

	if (m_eState == eSoundState_Stopped)
	{
//		int nCatchme = 1;
		return;
	}

	m_bPlaying = true;
	m_bStopCalled = false;

	// raise how often the SoundBuffer is used to a reasonable limit
	int nTimesUsed = m_pSoundBuffer->GetInfo()->nTimesUsed;
	if (nTimesUsed < 1000)
		m_pSoundBuffer->GetInfo()->nTimesUsed = ++nTimesUsed;

	if (!m_pSSys->IsEnabled())
	{
		// Simulate that sound is played for disabled sound system.
		OnEvent( SOUND_EVENT_ON_LOADED );
		OnEvent( SOUND_EVENT_ON_START );
		if (!(m_nFlags & FLAG_SOUND_LOOP))
			OnEvent( SOUND_EVENT_ON_STOP );
		return;
	}

	if (!m_bInitialized)
	{
		ESoundSystemErrorCode eResult = Initialize(m_pEventGroupBuffer ? m_pEventGroupBuffer : m_pSoundBuffer, m_nFlags);

		if (eResult == eSoundSystemErrorCode_ProjectNotReady)
		{
			// save the state, will be played later
			m_bPlayAfterLoad						= true;
			m_fPostLoadRatio						= fRatio;
			m_bPostLoadForceActiveState	= bForceActiveState;
			m_bPostLoadSetRatio					= bSetRatio;
			m_pSoundBuffer->AddToLoadReqList(this);
		}

		if (eResult != eSoundSystemErrorCode_None)
		{
			return;
		}

	}


		/*
		float fTime;
		if (m_pSSys->m_pCVarDebugSound->GetIVal())
		fTime = gEnv->pTimer->GetAsyncCurTime();
		TRACE("INFO: Synchroneous reading of %s due to Play() call before loading was finished.", GetName());
		if (!m_pSoundBuffer->WaitForLoad())
		return;
		if (m_pSSys->m_pCVarDebugSound->GetIVal())
		gEnv->pLog->Log("Warning: Synchroneous reading of %s due to Play() call before loading was finished stalled for %1.3f milliseconds !", GetName(), (gEnv->pTimer->GetAsyncCurTime()-fTime)*1000.0f);
		*/

	// TODO remove this when Play is called only once

	//////////////////////////////////////////////////////////////////////////
	// If sound is already playing, skip until it stops.
	// otherwise it creates a lot of overhead and channel leaks.
	//////////////////////////////////////////////////////////////////////////
	//if (IsPlayingOnChannel())
	// TODO remove this when Play is called only once
	if (IsActive() && m_pPlatformSound && !((m_pPlatformSound->GetState() == pssPAUSED) || (m_pPlatformSound->GetState() == pssLOADING)) && !m_IsPaused)
	{
			// TODO Verify with future sound class design
		// if (!bUnstoppable)
		
		//m_pPlatformSound->StopSound();
		//SetPaused(true);
		
		Deactivate();
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	// For Not looping sounds too far away, stop immediately not even preload sound if too far.
	//////////////////////////////////////////////////////////////////////////


	if (bSetRatio)
		m_fRatio *= fRatio;

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	//load the sound for real the first time is played
	//if (!Preload())
	//return; //the sound buffer cannot be loaded

	// Preloading is also playing.
	//m_bPlaying = true;

	// if there are already too many sounds playing, this one
	// should not be started
	// TODO combine with all active Sound lists

	// todo if the sound is start paused, it wont be culled, but maybe it still should be, when unpaused
	Preload();

	// if the soundbuffer is still loading, initiate the callback to play
	if (m_pSoundBuffer->Loading())
	{
		if (m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_DETAIL)
			m_pSSys->Log(eSLT_Warning, "<Sound> %s, Play() called before loading was finished", GetName());

		// save the state, will be played later
		m_bPlayAfterLoad						= true;
		m_fPostLoadRatio						= fRatio;
		m_bPostLoadForceActiveState	= bForceActiveState;
		m_bPostLoadSetRatio					= bSetRatio;
		m_pSoundBuffer->AddToLoadReqList(this);

		if (m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_DETAIL)
			m_pSSys->Log(eSLT_Message, "<Sound> %s Playing delayed until all data loaded!", GetName());
		return; 
	}

	assert(m_bInitialized);

	// Sounds with lower priority then current minimum set by sound system are ignored.
	if (m_nSoundPriority < m_pSSys->GetMinSoundPriority())
		return;

	float fDistanceToListener = 0.0f;
	float fDistSquared = 0.0f;
	CListener* pListener = (CListener*)m_pSSys->GetListener(m_pSSys->GetClosestActiveListener(GetPlaybackPosition()));

	if (CanBeCulled(pListener, fDistanceToListener, fDistSquared))
	{
		CTimeValue currTime = gEnv->pTimer->GetFrameStartTime();
		m_tPlayTime	= currTime;

		OnEvent( SOUND_EVENT_ON_LOADED );
		OnEvent( SOUND_EVENT_ON_START );
		
		if (GetLoopMode())
		{
			Deactivate();
		}
		else
		{
			Stop();
			
			// remove oneshot sounds immediately
			m_pSSys->SetSoundActiveState(this, eSoundState_Inactive);
		}

		return;
	}

	if (m_nFlags & FLAG_SOUND_START_PAUSED)
	{
		// all sounds that start paused, are culled
		return;
	}

	if (m_pSoundBuffer->Loaded())// && m_pPlatformSound && m_pPlatformSound->GetState() != pssPLAYING)
		StartPlayback(pListener, fDistanceToListener, fDistSquared);
			
	// All Sounds turn active now, going in their correct container
	if (m_pPlatformSound && (m_pPlatformSound->GetState() == pssPLAYING || m_pPlatformSound->GetState() == pssLOADING))
	{
		m_pSSys->SetSoundActiveState(this, eSoundState_Active);
	}

	// if the event or the soundbuffer is still loading, initiate the callback to play
	//if (m_pPlatformSound && m_pPlatformSound->GetState() == pssLOADING)
	//{
	//	if (m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_DETAIL)
	//		gEnv->pLog->Log("<Sound> Warning: %s, Play() called before loading was finished", GetName());

	//	// save the state, will be played later
	//	m_bPlayAfterLoad						= true;
	//	m_fPostLoadRatio						= fRatio;
	//	m_bPostLoadForceActiveState	= bForceActiveState;
	//	m_bPostLoadSetRatio					= bSetRatio;
	//	//m_pSoundBuffer->AddToLoadReqList(this);
	//	return; 
	//}

	//m_fLastPlayTime = currTime;
}

bool CSound::CanBeCulled(CListener* pListener, float &fDistanceToListener, float &fDistSquared)
{
	// cull all sound if there is no active listener to pick it up
	if (!pListener)
	{
		if (m_pSSys->DebuggingSound())
			m_pSSys->Log(eSLT_Warning, "<Sound> Tried to start sound %s with no active Listener", GetName());

		return true;
	}

	//bool bAllocatedChannel = false;
	//bool bStarted = false;
	uint32 nPlaybackFilter = m_pSSys->g_nPlaybackFilter;
	if (nPlaybackFilter)
	{
		bool bFilter = false;

		if (m_nFlags & FLAG_SOUND_VOICE && nPlaybackFilter==2)
			bFilter = true;

		if (!(m_nFlags & FLAG_SOUND_VOICE) && nPlaybackFilter==1)
			bFilter = true;

		if (nPlaybackFilter > 32 && !(m_eSemantic & nPlaybackFilter) && (m_eSemantic != eSoundSemantic_None))
			bFilter = true;

		if (bFilter && !(m_nFlags & FLAG_SOUND_START_PAUSED))
		{
			return true; 
		}
	}

	bool bCullOnDistance = false;

	int nActiveSounds = (int)m_pSSys->m_OneShotSounds.size() + (int)m_pSSys->m_LoopedSounds.size();
	bool bTooManyActiveSounds = (nActiveSounds > m_pSSys->g_nMaxActiveSounds);
	bool bTooManyChannels = false;

	int nThreshold = m_pSSys->g_nPriorityThreshold;
	if (nThreshold)
	{
		int nPlaying = m_pSSys->GetIAudioDevice()->GetNumberSoundsPlaying();
		float fFloodRate = nPlaying/(float)m_pSSys->g_nMaxChannels;

		if (fFloodRate > 0.8f && m_nSoundPriority > nThreshold) // high prio means lower prio !
		//if (m_pSSys->m_pCVarMaxChannels->GetIVal() < nPlaying)// && m_nSoundPriority < nThreshold)
			bTooManyChannels = true;	
	}

	if (bTooManyActiveSounds || bTooManyChannels)
		return true;

	if (m_nFlags & FLAG_SOUND_RADIUS)
	{
		fDistSquared = pListener->GetPosition().GetSquaredDistance(GetPlaybackPosition());
		bCullOnDistance = ((fDistSquared > 0) && (fDistSquared > m_RadiusSpecs.fMaxRadius2));
		fDistanceToListener = sqrt_tpl(fDistSquared);

		// switch to radio if needed
		// do that before Culling by distance
		if (m_nFlags & FLAG_SOUND_VOICE)
		{
			float fDialogRadioRatio = m_VoiceSpecs.pSoundInfo ? m_VoiceSpecs.pSoundInfo->fRadioRatio : 1.0f;
			float fRadioRadius = m_RadiusSpecs.fMaxRadius2*(fDialogRadioRatio*fDialogRadioRatio);
			if (fRadioRadius > 0 && fDistSquared > fRadioRadius)
			{
				m_pPlatformSound->SetPlatformSoundName("radio3d");
				m_nFlags &= ~FLAG_SOUND_RADIUS;
				m_nFlags &= ~FLAG_SOUND_3D;
				m_nFlags |= FLAG_SOUND_2D;
				// TODO set reverblevel to 0
			}
		}
	}

	if (m_nFlags & FLAG_SOUND_RADIUS)
	{
		if (bCullOnDistance) // && !(m_nFlags & FLAG_SOUND_START_PAUSED))
		{
			return true; //too far
		}
		else
		{
			m_fRatio = GetRatioByDistance(fDistanceToListener);
		}
	}

	if (m_nFlags & FLAG_SOUND_3D) // && !(m_nFlags & FLAG_SOUND_START_PAUSED))
	{
		if (!m_pSSys->IsSoundPH(this))
			return true; // not hearable
	}

	// sound is marked to play outdoor but the listener is inside a visarea
	if (m_nFlags & FLAG_SOUND_OUTDOOR && pListener->GetVisArea())
	{
			return true;
	}

	// sound is marked to play indoor but the listener is not inside a visarea
	if (m_nFlags & FLAG_SOUND_INDOOR && !pListener->GetVisArea())		
	{	
		return true;
	}

		// slider for SFX sounds on mute, so all non voice sounds can be culled
	if( m_pSSys->m_fSFXVolume == 0.0f && !(m_nFlags & FLAG_SOUND_VOICE) )
	{
		return true;
	}

	// slider for voice sounds on mute, so all voice sounds can be culled
	if( m_pSSys->m_fDialogVolume == 0.0f && (m_nFlags & FLAG_SOUND_VOICE) )
	{
		return true;
	}


	return false;

}

void CSound::StartPlayback(CListener* pListener, float &fDistanceToListener, float &fDistSquared)
{
	if (!m_pSoundBuffer || !m_pPlatformSound)
		return;

	m_bVolumeHasChanged = true; // make sure new volume is calculated and set to FMOD

	CTimeValue currTime					 = gEnv->pTimer->GetFrameStartTime();
	CTimeValue timeSinceLastPlay = currTime - m_tPlayTime;

	// reset Firsttime for obstruction if a sound got its event stolen to prevent further bad stealing behavior
	m_Obstruction.bFirstTime = (!m_pPlatformSound->GetSoundHandle() && m_bPlaying);

	bool bStartPlaybackNow = true;
	if ((m_nFlags & FLAG_SOUND_OBSTRUCTION) && (m_nFlags & FLAG_SOUND_3D) && pListener && m_Obstruction.bFirstTime)
	{
		//ProcessObstruction(pListener, false);

		//m_Obstruction.bDelayPlayback = !m_Obstruction.bDelayPlayback;

		ProcessObstruction(pListener, false);
		// this call might change bDelayPlayback immediately
		bStartPlaybackNow = !m_Obstruction.bDelayPlayback;
		m_Obstruction.bFirstTime = m_Obstruction.bDelayPlayback;
	}

	// TODO REview this
	SSoundSettings NewSoundSettings;
	NewSoundSettings.Frequency = 0;
	NewSoundSettings.is3D = false;
	NewSoundSettings.bLoadData = true; // always start loading in the data if the decision was made to play the sound
	NewSoundSettings.bLoadNonBlocking = (m_nFlags&FLAG_SOUND_LOAD_SYNCHRONOUSLY)?false:true;
	NewSoundSettings.SoundSettings3D = NULL;
	NewSoundSettings.SpeakerPan = 0;

	// TODO still needed here?
	m_pSoundBuffer->UpdateTimer(gEnv->pTimer->GetFrameStartTime());

	bool bOldLoadData = false;
	enumPlatformSoundErrorCodes eErrorCode = pssOK;

	switch (m_pSoundBuffer->GetProps()->eBufferType)
	{
	case btEVENT:
		{
			// TODO Probably get rid of the Soundsettings at CreateSound and set all we need afterwards

			if ((m_nFlags&FLAG_SOUND_OBSTRUCTION) && !m_Obstruction.bFirstTime)
			{
				// set obstruction value on infoonly instance before creating real instance
				m_Obstruction.bAssigned = false;
				CalculateObstruction();
			}

			if (!m_pPlatformSound->GetSoundHandle())
			{
				bOldLoadData = NewSoundSettings.bLoadData;
				NewSoundSettings.bLoadData = false;
				eErrorCode = m_pPlatformSound->CreateSound(m_pSoundBuffer->GetAssetHandle(), NewSoundSettings);
				NewSoundSettings.bLoadData = bOldLoadData;

				if (eErrorCode != pssOK)
				{
					assert (!"Create PlatformSound InfoOnly Handle failed");
					return;
				}
			}

			m_fRatio = GetRatioByDistance(fDistanceToListener); // reset the current distance
			UpdateSoundProperties();
			UpdateVoiceProperties(false);
			UpdatePositionAndVelocity();

			eErrorCode = m_pPlatformSound->CreateSound(m_pSoundBuffer->GetAssetHandle(), NewSoundSettings);

			if (eErrorCode != pssOK)
			{
				return;
			}

			m_fRatio = GetRatioByDistance(fDistanceToListener); // reset the current distance
			m_tPlayTime	= currTime;
			//SetFadeTime(200);
			//bAllocatedChannel		= true;
			//bStarted = true;
			//SetLoopMode(GetLoopMode()); //TODO should be review
			
			if (m_nStartOffset)
			{
				// Offset in milliseconds for events only supported atm!
				ptParamINT32 NewParam(m_nStartOffset);

				if (m_bOffsetInSamples)
					m_pPlatformSound->SetParamByType(pspSAMPLEPOSITION, &NewParam);
				else
					m_pPlatformSound->SetParamByType(pspTIMEPOSITION, &NewParam);
			}
			m_nStartOffset = 0;
		}
		break;
	case btSTREAM:
		{
			if ((!GetLoopMode()) || (!IsPlayingOnChannel()))
			{
				// TODO Probably get rid of the Soundsettings at CreateSound and set all we need afterwards
				eErrorCode = m_pPlatformSound->CreateSound(m_pSoundBuffer->GetAssetHandle(), NewSoundSettings);
				m_tPlayTime	= currTime;
				//bAllocatedChannel		= true;
				//bStarted = true;

			}
			SetLoopMode(GetLoopMode()); //TODO should be review
		}
		break;
	case btSAMPLE:
		{										
			if (m_pEventGroupBuffer)
			{
				if ((m_nFlags&FLAG_SOUND_OBSTRUCTION) && !m_Obstruction.bFirstTime)
				{
					// set obstruction value on infoonly instance before creating real instance
					m_Obstruction.bAssigned = false;
					CalculateObstruction();
				}

				m_fRatio = GetRatioByDistance(fDistanceToListener); // reset the current distance
				UpdateSoundProperties();
				UpdateVoiceProperties(false);
				UpdatePositionAndVelocity();

				eErrorCode = m_pPlatformSound->CreateSound(m_pSoundBuffer->GetAssetHandle(), NewSoundSettings);
				m_fRatio = GetRatioByDistance(fDistanceToListener); // reset the current distance
				m_tPlayTime	= currTime;

			}
			else
			{
				if (m_nFlags & FLAG_SOUND_3D)
				{
					SSoundSettings3D NewSoundSettings3D;
					NewSoundSettings3D.bRelativetoListener	= false;
					NewSoundSettings3D.bUseVelocity					= false;
					NewSoundSettings3D.vPosition						= m_vPosition;

					NewSoundSettings.is3D = true;
					NewSoundSettings.SoundSettings3D = &NewSoundSettings3D;

					if (!fDistSquared)
					{
						//check the distance from the listener
						if (!pListener)
							return;
						Vec3 vListenerPos = pListener->GetPosition();
						fDistSquared = vListenerPos.GetSquaredDistance(m_vPlaybackPos);
					}

					if (fDistSquared > m_RadiusSpecs.fMaxRadius2)// && bCullOnDistance)
					{
						// listener is beyond the MAX Radius
						if (!GetLoopMode())
						{
							Deactivate(); //free a channel
							return; //too far
						}
						//bCanPlay = false;
					}

					// todo ask Craig or Timur how frame_profiler works
					//FRAME_PROFILER( "CSound:Create/PlaySoundEX", m_pSSys->GetSystem(), PROFILE_SOUND );						
					//if ((!GetLoopMode()) || (!IsPlayingOnChannel())) // Loopfix
					if (!IsPlayingOnChannel())
					{						
						eErrorCode = m_pPlatformSound->CreateSound(m_pSoundBuffer->GetAssetHandle(), NewSoundSettings);
						//m_fChannelPlayTime	= currTime;
						//bAllocatedChannel		= true;
						//bStarted = true;
					}
					SetLoopMode(GetLoopMode()); // Todo should be reviewed
					SetPosition(m_vPosition);
					//min max distance and loop mode must be set before 'cos it operates
					//on a per-sound basis
					SetMinMaxDistance_i(m_RadiusSpecs.fMinRadius, m_RadiusSpecs.fMaxRadius);
				}
				else	// 2D Sound
				{
					//if ((!GetLoopMode()) || (!IsPlayingOnChannel())) // loopfix
					if (!IsPlayingOnChannel())
					{
						eErrorCode = m_pPlatformSound->CreateSound(m_pSoundBuffer->GetAssetHandle(), NewSoundSettings);
						//m_fChannelPlayTime	= currTime;
						//bAllocatedChannel		= true;
						//bStarted = true;
					}
					//loop mode must be set before 'cos it operates
					//on a per-sound basis
					SetLoopMode(GetLoopMode()); // todo should be reviewed
				}
			}
			if (m_fPitching != 0.0f)
			{			
				float fRand = (float)rand()/(float)RAND_MAX;
				fRand = (fRand*2.0f) - 1.0f;
				m_nCurrPitch = (int)(fRand*m_fPitching);
				// TODO activate
				//
			}
			else
				m_nCurrPitch = 0;

			SetFrequency(m_nRelativeFreq);
			if (m_nStartOffset)
			{
				ptParamINT32 NewParam(m_nStartOffset);

				if(m_bOffsetInSamples)
					m_pPlatformSound->SetParamByType(pspSAMPLEPOSITION, &NewParam);
				else
					m_pPlatformSound->SetParamByType(pspTIMEPOSITION, &NewParam);
			}
			m_nStartOffset = 0;
		}


		break;
	case btNETWORK:
		// TODO Probably get rid of the Soundsettings at CreateSound and set all we need afterwards
		eErrorCode = m_pPlatformSound->CreateSound(m_pSoundBuffer->GetAssetHandle(), NewSoundSettings);
		m_fRatio = GetRatioByDistance(fDistanceToListener); // reset the current distance
		//m_fChannelPlayTime	= currTime;
		//bAllocatedChannel		= true;
		//bStarted = true;
		//SetLoopMode(GetLoopMode()); //TODO should be review
		break;
	}

	if (eErrorCode != pssOK)
	{
		// something is wrong, or event simply failed (max playbacks), which is no big deal
		if (m_pPlatformSound->GetState() == pssERROR)
			Stop();

		return;
	}
	else
	{
		SetState(eSoundState_None);
	}
	
	// update environment parameter
	if (GetFlags() & FLAG_SOUND_PARAM_ENVIRONMENT)
	{
		if (m_pPlatformSound)
			m_pPlatformSound->SetParamByName("environment", m_pSSys->m_pReverbManager->GetEnvironment(this) );
	}

	//if (m_pPlatformSound->GetState() == pssLOADING)
	//{
		// sound event is not ready yet, so we have to wait
		//return;
	//}

	if (m_pPlatformSound->GetSoundHandle())
	{
		ptParamINT32 NewParam(m_nSoundPriority);
		NewParam.SetValue((int32)m_nSoundPriority);
		m_pPlatformSound->SetParamByType(pspPRIORITY, &NewParam);

		m_bPositionHasChanged = true;



		// lets do a Update before unpausing the sound
		//if ((m_nFlags & FLAG_SOUND_OBSTRUCTION) && (m_nFlags & FLAG_SOUND_3D) && pListener && m_Obstruction.bFirstTime)
		//{
		//	//ProcessObstruction(pListener, false);
		//	
		//	//m_Obstruction.bDelayPlayback = !m_Obstruction.bDelayPlayback;
		//	
		//	ProcessObstruction(pListener, false);
		//	// this call might change bDelayPlayback immediately
		//	bStartPlaybackNow = !m_Obstruction.bDelayPlayback;
		//	m_Obstruction.bFirstTime = m_Obstruction.bDelayPlayback;
		//}

		if (!(m_nFlags & FLAG_SOUND_START_PAUSED) && bStartPlaybackNow )
		{
			m_IsPaused = false;

			Update();

			OnEvent( SOUND_EVENT_ON_START );
			bool bIsPlaying = m_pPlatformSound->PlaySound(false);

			if (bIsPlaying)
				SetState(eSoundState_Active);

			m_tPlayTime	= currTime;
			//UpdateActiveVolume();
		}
		else
		{
			m_IsPaused = true;
			
			// start event paused so it counts as an active instances and prevents more sounds to play in the same frame
			//m_pPlatformSound->PlaySound(true);
			
			Update();

			// TODO REVIEW this
			// no Platform sound send this event, so it needs to be send here
			//OnEvent( SOUND_EVENT_ON_PLAY );
		}
	}

	if (m_pSSys->g_nProfiling > 0)
	{
		ISoundProfileInfo* pSoundInfo = NULL;

		CSoundSystem::tmapSoundInfo::iterator It = m_pSSys->m_SoundInfos.find(CONST_TEMP_STRING(GetName()));

		if (It != m_pSSys->m_SoundInfos.end())
			pSoundInfo = (*It).second;

		int nTemp = 0;
		if (m_pPlatformSound && (m_pPlatformSound->GetState() == pssPLAYING))
		{
			if (pSoundInfo)
			{
				++pSoundInfo->GetInfo()->nTimesPlayedOnChannel;

				ptParamINT32 ptTemp(nTemp);
				if (m_pPlatformSound->GetParamByType(pspLENGTHINBYTES, &ptTemp))
					ptTemp.GetValue(nTemp);
				else
					nTemp = m_pSoundBuffer->GetMemoryUsed();

				pSoundInfo->GetInfo()->nMemorySize = nTemp;

				nTemp = 0;
				if (m_pPlatformSound->GetParamByType(pspSPAWNEDINSTANCES, &ptTemp))
					ptTemp.GetValue(nTemp);

				int nSpawn = pSoundInfo->GetInfo()->nPeakSpawn;
				pSoundInfo->GetInfo()->nPeakSpawn = max(nSpawn, nTemp);

				nTemp = 0;
				if (m_pPlatformSound->GetParamByType(pspCHANNELSUSED, &ptTemp))
					ptTemp.GetValue(nTemp);

				pSoundInfo->GetInfo()->nChannelsUsed = nTemp;

			}
		}
	}

	if (m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_SIMPLE && !m_IsPaused && m_pPlatformSound->GetSoundHandle())
	{
		m_pSSys->Log(eSLT_Message, "<Sound> Start playback: %s,volume=%.2f,priority=%d ID:%d, TimeDiff:%.2f", GetName(), m_fActiveVolume, m_nSoundPriority, GetId(), timeSinceLastPlay.GetSeconds());
	}
}

//////////////////////////////////////////////////////////////////////////
void CSound::Deactivate()
{
	if (!m_pPlatformSound || m_pPlatformSound->GetSoundHandle() == NULL)
		return;

	m_fActiveVolume = -1;

	OnEvent(SOUND_EVENT_ON_PLAYBACK_STOPPED);
	m_pPlatformSound->FreeSoundHandle();

	if (m_pPlatformSound->GetState() == pssNONE)
		SetState(eSoundState_Stopped);

	m_fLastObstructionUpdate						= 0.0f;
	m_Obstruction.nRaysShot							= 0;
	m_Obstruction.bAssigned							= false;
	m_Obstruction.bProcessed						= false;
	m_Obstruction.bDelayPlayback				= true;
	m_Obstruction.bFirstTime						= false;
	m_Obstruction.ResetDirect();
	m_Obstruction.ResetReverb();

	m_tPlayTime.SetSeconds(-1.0f);
}

//////////////////////////////////////////////////////////////////////
void CSound::Stop(const ESoundStopMode eStopMode)
{
	FUNCTION_PROFILER( gEnv->pSystem,PROFILE_SOUND );
	//GUARD_HEAP;
	// don't play after finishing with loading - if still loading yet...

	SetState(eSoundState_Ending);

	m_eStopMode = eStopMode;
	m_bPlayAfterLoad = false; 

	m_IsPaused = false;
	m_bStopCalled = true;

	assert (m_pPlatformSound != NULL);
	if (m_pPlatformSound)
		m_pPlatformSound->StopSound();

	if (eStopMode == ESoundStopMode_OnSyncPoint)
	{
		m_fSyncTimeout = (m_fSyncTimeout>0.0f)? m_fSyncTimeout:0.1f;
	}

	m_fActiveVolume = -1.0f;


	Update(); // enforce an Update so ending and stopping is dealt with


		if (m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_SIMPLE)
		{
			bool bBlockLog = false;

			uint32 nPlaybackFilter = m_pSSys->g_nPlaybackFilter;
			if (nPlaybackFilter)
			{
				if (m_nFlags & FLAG_SOUND_VOICE && nPlaybackFilter==2)
					bBlockLog = true;

				if (!(m_nFlags & FLAG_SOUND_VOICE) && nPlaybackFilter==1)
					bBlockLog = true;

				if (nPlaybackFilter > 32 && !(m_eSemantic & nPlaybackFilter) && (m_eSemantic != eSoundSemantic_None))
					bBlockLog = true;
			}

			if (!bBlockLog)
				m_pSSys->Log(eSLT_Message, "<Sound> Stop Sound %s, ID:%d", GetName(), GetId());
		}

}

//////////////////////////////////////////////////////////////////////
void CSound::SetPaused(const bool bPaused)
{
 	assert (m_pPlatformSound != NULL); 

	if (!m_pPlatformSound)
		return;

	//FUNCTION_PROFILER( m_pSSys->GetSystem(),PROFILE_SOUND );
	//GUARD_HEAP;
	
	m_IsPaused					= bPaused;

	// renew the timer
	m_tPlayTime	= gEnv->pTimer->GetCurrTime();

	m_nFlags &= ~FLAG_SOUND_START_PAUSED;

	ptParamBOOL NewParam(bPaused);
	m_pPlatformSound->SetParamByType(pspPAUSEMODE, &NewParam);

	if (!m_pPlatformSound->GetSoundHandle())
	{
		Play();
	}

	if (m_pPlatformSound->GetState() != pssINFOONLY)
	{
		OnEvent( bPaused?SOUND_EVENT_ON_PLAYBACK_PAUSED:SOUND_EVENT_ON_PLAYBACK_UNPAUSED );

		if (!m_pSSys->IsInPauseCall())
			OnEvent( bPaused?SOUND_EVENT_ON_PLAYBACK_STOPPED:SOUND_EVENT_ON_PLAYBACK_STARTED );
	}

	return;
}

//////////////////////////////////////////////////////////////////////
const bool CSound::GetPaused() const
{
	if (m_pPlatformSound &&
			m_pPlatformSound->GetState() != pssINFOONLY &&
			m_pPlatformSound->GetState() != pssNONE)
	{
		bool bPaused = false;
		ptParamBOOL NewParam(bPaused);
		m_pPlatformSound->GetParamByType(pspPAUSEMODE, &NewParam);
		NewParam.GetValue(bPaused);
		return bPaused;
	}
	return m_IsPaused;
}

//////////////////////////////////////////////////////////////////////
void CSound::SetFade(const float fFadeGoal, const int nFadeTimeInMS)
{
	if (!NumberValid(fFadeGoal))
	{
		m_pSSys->Log(eSLT_Warning, "<Sound> %s Invalid input values (possible NaNs) on Sound::SetFade()", GetName() );
		assert(0);
		return;
	}

	if (fFadeGoal != m_fFadeGoal)
	{
		m_fFadeGoal = fFadeGoal;

		if (nFadeTimeInMS != m_nFadeTimeInMS)
			m_nFadeTimeInMS = nFadeTimeInMS;

	}
}

//////////////////////////////////////////////////////////////////////
const char* CSound::GetName()
{
	assert (m_pPlatformSound != NULL); 
	assert (m_pSoundBuffer != NULL); 

	if (m_sSoundName.empty() && m_pSoundBuffer && m_pPlatformSound)
	{
		if (m_pSoundBuffer->GetProps()->eBufferType == btEVENT)
		{
			string sTemp;
			ptParamCRYSTRING Temp(sTemp);
			m_pPlatformSound->GetParamByType(pspEVENTNAME, &Temp);
			Temp.GetValue(sTemp);
			m_sSoundName = m_pSoundBuffer->GetName();
			m_sSoundName += ":" + sTemp;
		}
		else
			m_sSoundName = m_pSoundBuffer->GetName();
	}

	return m_sSoundName.c_str();
}

//////////////////////////////////////////////////////////////////////
const tSoundID CSound::GetId() const
{
	return m_nSoundID;
}

//////////////////////////////////////////////////////////////////////
void CSound::SetId(tSoundID SoundID)
{
	m_nSoundID = SoundID;
}

//////////////////////////////////////////////////////////////////////
void CSound::SetLoopMode(bool bLoopMode)
{
	//GUARD_HEAP;
	//FRAME_PROFILER( "CSound:SetLoopMode",m_pSSys->GetSystem(),PROFILE_SOUND );

//  Bugsearch

	//if (IsActive())
	//{
	//	ptParamBOOL NewParam(bLoopMode);
	//	if (!m_pPlatformSound->SetParam(pspLOOPMODE, &NewParam))
	//	{
	//		// throw error
	//	}
	//}

	// only set the flag if the value really will change
	//m_bLoopModeHasChanged = ((m_nFlags & FLAG_SOUND_LOOP) != bLoopMode);
	m_bLoopModeHasChanged = true;

	if (bLoopMode)
		m_nFlags |= FLAG_SOUND_LOOP;
	else
		m_nFlags &= ~FLAG_SOUND_LOOP;
}

//////////////////////////////////////////////////////////////////////
void CSound::SetPitching(float fPitching)
{
	if (!NumberValid(fPitching))
	{
		m_pSSys->Log(eSLT_Warning, "<Sound> %s Invalid input values (possible NaNs) on Sound::SetPitching()", GetName() );
		assert(0);
		return;
	}

	m_fPitching = fPitching;
}

//////////////////////////////////////////////////////////////////////
//void CSound::SetBaseFrequency(int nFreq)
//{
//	//m_nBaseFreq = nFreq;
//}

//////////////////////////////////////////////////////////////////////
int	 CSound::GetBaseFrequency() const
{
	if (!m_pSoundBuffer)
		return 0;

	return (m_pSoundBuffer->GetInfo()->nBaseFreq);
}

//////////////////////////////////////////////////////////////////////
void CSound::SetPan(float fPan)
{
	if (m_pPlatformSound)
	{
		ptParamF32 NewParam(fPan);
		m_pPlatformSound->SetParamByType(pspSPEAKERPAN, &NewParam);
	}

	m_fPan = fPan;
}

//////////////////////////////////////////////////////////////////////
float CSound::GetPan() const
{
	float fPan = 0.0f;

	if (m_pPlatformSound)
	{
		ptParamF32 NewParam(fPan);
		m_pPlatformSound->GetParamByType(pspSPEAKERPAN, &NewParam);
		NewParam.GetValue(fPan);
	}

	return fPan;
}

//////////////////////////////////////////////////////////////////////
void CSound::Set3DPan(float f3DPan)
{
	if (m_pPlatformSound)
	{
		ptParamF32 NewParam(f3DPan);
		m_pPlatformSound->SetParamByType(pspSPEAKER3DPAN, &NewParam);
	}
}

//////////////////////////////////////////////////////////////////////
float CSound::Get3DPan() const
{
	float f3DPan = 0.0f;

	if (m_pPlatformSound)
	{
		ptParamF32 NewParam(f3DPan);
		m_pPlatformSound->GetParamByType(pspSPEAKER3DPAN, &NewParam);
		NewParam.GetValue(f3DPan);
	}

	return f3DPan;
}

//////////////////////////////////////////////////////////////////////
void CSound::SetFrequency(int nFreq)
{
	//FRAME_PROFILER( "CSound:SetFrequency",m_pSSys->GetSystem(),PROFILE_SOUND );

	assert (m_pPlatformSound != NULL); 
	assert (m_pSoundBuffer != NULL); 
	
	// TODO 
	// rework the whole pitch thing

	m_nRelativeFreq = nFreq;

	if (!m_pSoundBuffer || !m_pPlatformSound)
		return;

	float fFreq = (float)m_pSoundBuffer->GetInfo()->nBaseFreq*((float)(m_nRelativeFreq+m_nCurrPitch)/1000.0f);
	ptParamINT32 NewParam((int)fFreq);
	m_pPlatformSound->SetParamByType(pspFREQUENCY, &NewParam);

	int32 nTestFreq = 0;
	ptParamINT32 NewParam2(nTestFreq);
	m_pPlatformSound->GetParamByType(pspFREQUENCY, &NewParam2);
	NewParam2.GetValue(nTestFreq);
}

//////////////////////////////////////////////////////////////////////
void CSound::SetMinMaxDistance(float fMinDist, float fMaxDist)
{
	if (!NumberValid(fMinDist) || !NumberValid(fMaxDist))
	{
		m_pSSys->Log(eSLT_Warning, "<Sound> %s Invalid input values (possible NaNs) on Sound::SetMinMaxDistance()", GetName() );
		assert(0);
		return;
	}

	if (m_nFlags & FLAG_SOUND_EVENT)
	{
		// TODO enable gate on shipping
		//if (m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_DETAIL)
		{
			m_pSSys->Log(eSLT_Warning, "<Sound> Setting min/max radius on an event (%s) prevented", GetName());
		}
	}
	else
	{
		SetMinMaxDistance_i(fMinDist, fMaxDist);
	}
}

//////////////////////////////////////////////////////////////////////
void CSound::SetMinMaxDistance_i(float fMinDist, float fMaxDist)
{
	//FRAME_PROFILER( "CSound:SetMinMaxDistance",m_pSSys->GetSystem(),PROFILE_SOUND );

	assert (m_pSSys); 

	m_RadiusSpecs.fOriginalMinRadius = fMinDist;
	m_RadiusSpecs.fOriginalMaxRadius = fMaxDist;

	m_RadiusSpecs.fMinRadius = fMinDist;
	m_RadiusSpecs.fMinRadius2 = fMinDist*fMinDist;

	m_RadiusSpecs.fMaxRadius = fMaxDist*m_RadiusSpecs.fDistanceMultiplier;
	m_RadiusSpecs.fMaxRadius2 = m_RadiusSpecs.fMaxRadius*m_RadiusSpecs.fMaxRadius;
	
	float fPreloadRadius					= m_RadiusSpecs.fMaxRadius + SOUND_PRELOAD_DISTANCE;
	m_RadiusSpecs.fPreloadRadius2 = fPreloadRadius*fPreloadRadius;

#ifdef _DEBUG
	if(fMinDist < 0.1f)
	{
		m_pSSys->Log(eSLT_Warning, "<Sound> [%s] min out of range %f",GetName(), fMinDist);
	}
	if(fMaxDist>1000000000.0f)
	{
		m_pSSys->Log(eSLT_Warning, "<Sound> [%s] max out of range %f",GetName(), fMaxDist);
	}
#endif

	if (m_pPlatformSound && !(m_nFlags & FLAG_SOUND_EVENT))
	{
		ptParamF32 NewParam(fMinDist);
		m_pPlatformSound->SetParamByType(pspMINRADIUS, &NewParam);
	}
}

//! Sets a distance multiplier so sound event's distance can be tweak (sadly pretty workaround feature)
void CSound::SetDistanceMultiplier(const float fMultiplier)
{
#if !defined(_RELEASE)
	//NumberValid() is extremely expensive on consoles
	if (!NumberValid(fMultiplier))
	{
		m_pSSys->Log(eSLT_Warning, "<Sound> %s Invalid input values (possible NaNs) on Sound::SetDistanceMultiplier()", GetName() );
		assert(0);
		return;
	}
#endif

	m_RadiusSpecs.fDistanceMultiplier = fMultiplier;
	SetMinMaxDistance_i(m_RadiusSpecs.fOriginalMinRadius, m_RadiusSpecs.fOriginalMaxRadius);
}


//////////////////////////////////////////////////////////////////////
void CSound::SetAttrib(float fMaxVolume, float fRatio, int nFreq, bool bSetRatio)
{
	assert (m_pPlatformSound != NULL); 
	assert(nFreq == 1000); // TODO why is that so?

	m_bVolumeHasChanged = true;

	if (bSetRatio)
		m_fRatio = fRatio;

	m_fMaxVolume = fMaxVolume;

	//TODO Optimize all the Change/Set/Calc-Volume stuff
	if (m_pPlatformSound && m_pPlatformSound->GetSoundHandle()) 
	{
		CalcSoundVolume(m_fMaxVolume,fRatio);
	}
}
	
//////////////////////////////////////////////////////////////////////
void CSound::SetAttrib(const Vec3 &pos, const Vec3 &speed)
{
	m_bPositionHasChanged = true;
  m_vPosition						= pos;
  m_vSpeed							= speed;
}

//////////////////////////////////////////////////////////////////////////
void CSound::SetRatio(float fRatio)
{
	if (!NumberValid(fRatio))
	{
		m_pSSys->Log(eSLT_Warning, "<Sound> %s Invalid input values (possible NaNs) on Sound::SetRatio()", GetName() );
		assert(0);
		return;
	}

	if ( abs(m_fRatio-fRatio) > 0.01f)
	{
		m_bVolumeHasChanged = true;
		m_fRatio = fRatio;
	}
}

//////////////////////////////////////////////////////////////////////////
void CSound::UpdatePositionAndVelocity()
{
	assert (m_pPlatformSound != NULL);

	Vec3 ForwardVec(0,1,0); // Forward.
	ForwardVec = m_mTransformation.TransformVector(ForwardVec);
	ForwardVec.Normalize();
		
	if (m_nFlags & FLAG_SOUND_SELFMOVING)
	{
		float fTimeDelta	= gEnv->pTimer->GetFrameTime();
		Vec3 vNewPos			= (m_vPosition + ForwardVec*fTimeDelta );
		SetPosition(vNewPos);
	}

	if (m_nFlags & FLAG_SOUND_RELATIVE)
		SetPosition(m_vPosition);


	if (IsActive() && (m_nFlags & FLAG_SOUND_3D) && m_pPlatformSound)
		//if (m_nFlags & FLAG_SOUND_RELATIVE)
		//{
		//	m_pPlatformSound->Set3DPosition(&m_vPosition, &m_vSpeed);
		//}
		//else
		{
			m_pPlatformSound->Set3DPosition(&m_vPlaybackPos, &m_vSpeed, ForwardVec.IsZero() ? 0 : &ForwardVec);
		}
}

//////////////////////////////////////////////////////////////////////
void CSound::SetPosition(const Vec3 &vPos)
{
#if !defined(_RELEASE)
	//IsValid() is incredibly expensive on consoles
	assert (m_pSSys);
	if (!vPos.IsValid())
	{
		m_pSSys->Log(eSLT_Warning, "<Sound> %s Invalid input values (possible NaNs) on Sound::SetPosition()", GetName() );
		return;
	}
#endif

	// for now both 2D and 3D can have a position
	//if (!(m_nFlags & FLAG_SOUND_3D))
	//	return;
	
	if (vPos.IsEquivalent(m_vPosition) && !(GetFlags() & FLAG_SOUND_RELATIVE) && (m_pSSys->m_fDirAttCone == 0.0f))
		return;

  m_vPlaybackPos = m_vPosition;

	// Setting the Velocity for Doppler
	if (m_pSSys->g_nDoppler && (m_nFlags & (FLAG_SOUND_DOPPLER|FLAG_SOUND_PARAM_DOPPLER_ABS|FLAG_SOUND_PARAM_DOPPLER_REL)) && m_bPlaying)
	{
		Vec3 vNewSpeed		= (vPos - m_vPosition);
		float fTimeDelta	= gEnv->pTimer->GetFrameTime();
		float fMultiplier = (float)__fsel(-fTimeDelta, 0.0f, 1.0f);
		float fSafeTimeDelta = (float)__fsel(-fTimeDelta, 1.0f, fTimeDelta);
		m_vSpeed					= vNewSpeed * fMultiplier * (m_pSSys->g_fDopplerScale * __fres(fSafeTimeDelta));

		//GUARD_HEAP;
	}

	m_bPositionHasChanged = true;
	m_vPosition = vPos;

	IListener* pListener = m_pSSys->GetListener(LISTENERID_STANDARD);
	Vec3 vListenerPos(ZERO);

	// snap sound to Listener if its pretty close
	if (pListener)
	{
		vListenerPos = pListener->GetPosition();
		if(vListenerPos.IsEquivalent(vPos, 0.3f))
		{
			m_vPosition = vListenerPos;
		}
	}

	if (GetFlags() & FLAG_SOUND_RELATIVE)
	{
		if (!pListener)
			return;

		//Matrix34 mTemp = pListener->GetMatrix();
		//mTemp.SetTranslation(Vec3(0.0f));
		//Vec3 vTemp = mTemp.GetInvertedFast() * vPos; //(pListener->GetPosition());

		m_vPlaybackPos = /* vTemp + */ vListenerPos;
		//gEnv->pLog->Log("## Set rel. Sound Pos: %.2f,%.2f,%.2f", m_vPlaybackPos.x, m_vPlaybackPos.y, m_vPlaybackPos.z);
	}
	else
	{
		m_vPlaybackPos	= m_vPosition;
	}

	//if (!(m_nFlags & FLAG_SOUND_3D))
	//	return;

	/*
	if (m_pSSys->m_pCVarDebugSound->GetIVal()==1)
	{
		gEnv->pLog->Log("Sound %s, pos+%f,%f,%f",m_strName.c_str(),m_position.x,m_position.y,m_position.z);
	}
	*/

	//m_pSSys->m_fDirAttCone = 0.8f;
	//m_pSSys->m_fDirAttMaxScale = 0.0f;
	//m_pSSys->m_DirAttPos = pListener->GetPosition();
	//m_pSSys->m_DirAttDir = pListener->GetForward();

	if ((m_pSSys->m_fDirAttCone > 0.0f) && (m_pSSys->m_fDirAttCone < 1.0f))
	{
		Vec3 DirToPlayer = m_vPosition - m_pSSys->m_DirAttPos;
		DirToPlayer.Normalize();
		float fScale = ((1.0f - (DirToPlayer*m_pSSys->m_DirAttDir) ) * __fres(1.0f - m_pSSys->m_fDirAttCone));

		if ((fScale > 0.0f) && (fScale <= 1.0f))	// inside cone
		{
			m_pSSys->m_fDirAttMaxScale = max((1.0f - fScale), m_pSSys->m_fDirAttMaxScale);
			
			// not move the position anymore, but increase the distance range instead
			float fReverseDistanceMultiplier = __fres(fScale);
			if (m_RadiusSpecs.fDistanceMultiplier != fReverseDistanceMultiplier)
				SetDistanceMultiplier(fReverseDistanceMultiplier);
			
			//m_vPlaybackPos = (m_vPosition * fScale) + (m_pSSys->m_DirAttPos + (m_pSSys->m_DirAttDir * 0.1f))*(1.0f - fScale);
			//TRACE("Sound-Proj: %s, %1.3f, %1.3f, %1.3f, (%5.3f, %5.3f, %5.3f)", m_strName.c_str(), fScale, DirToPlayer*m_pSSys->m_DirAttDir, m_pSSys->m_fDirAttCone, m_RealPos.x, m_RealPos.y, m_RealPos.z);
		}
		else
		{
			if (m_RadiusSpecs.fDistanceMultiplier != 1.0f)
				SetDistanceMultiplier(1.0f);
		}
	}
	else
	{
		if (m_pSSys->m_bNeedSoundZoomUpdate)
			SetDistanceMultiplier(1.0f);
	}

	// check if the sound must be considered 
	// for sound occlusion 
	if (m_nFlags & FLAG_SOUND_CULLING)
	{		
		m_bRecomputeVisArea = true;
	}

	// line sound position
	if (m_bLineSound)
	{
		if (!pListener)
			return;

		float fT = 0.0f;
		float fDistance = Distance::Point_Lineseg(vListenerPos, m_Line, fT);
		m_vPlaybackPos = m_Line.start + fT*(m_Line.end - m_Line.start);
	}

	// sphere sound position
	if (m_bSphereSound)
	{
		if (!pListener)
			return;

		Vec3 vTemp = vListenerPos - m_vPosition;
		if ((vTemp.GetLengthSquared()) < m_fSphereRadius*m_fSphereRadius )
		{
			// inside
			m_vPlaybackPos = vListenerPos;
		}
		else
		{
			// outside
			vTemp = vTemp.normalize() * m_fSphereRadius;
			m_vPlaybackPos = m_vPosition + vTemp;

		}
	}

	//gEnv->pLog->Log("**** Set Sound Pos: %.2f,%.2f,%.2f", m_vPlaybackPos.x, m_vPlaybackPos.y, m_vPlaybackPos.z);

}

// modify a line sound
void CSound::SetLineSpec(const Vec3 &vStart, const Vec3 &vEnd)
{
	m_Line.start = vStart;
	m_Line.end   = vEnd;
	m_bLineSound = true;
	m_bFlagsChanged = true;
}

// retrieve a line sound
bool CSound::GetLineSpec(  Vec3 &vStart,   Vec3 &vEnd)
{
	vStart = m_Line.start;
	vEnd = m_Line.end;

	return m_bLineSound;
}

// modify a sphere sound
void CSound::SetSphereSpec(const float fRadius)
{
	m_fSphereRadius = fRadius;
	m_bSphereSound = true;
	m_bFlagsChanged = true;
}

//////////////////////////////////////////////////////////////////////
void CSound::SetVelocity(const Vec3 &speed)
{  
  m_vSpeed = speed;  
}

////////////////////////////////////////////////////////////////////////
void CSound::SetDirection(const Vec3 &vDir)
{
	m_mTransformation.SetIdentity();
	m_mTransformation.SetColumn(1, vDir);
}

//////////////////////////////////////////////////////////////////////
Vec3 CSound::GetForward() const
{ 
	Vec3 ForwardVec(0,1,0); // Forward.
	ForwardVec = m_mTransformation.TransformVector(ForwardVec);
	ForwardVec.Normalize();
	return ForwardVec;
}


//////////////////////////////////////////////////////////////////////
float CSound::GetRatioByDistance(const float fDistance)
{
	assert (m_pPlatformSound != NULL); 

	// Sounds without a radius always play at full volume
	if (!(m_nFlags & FLAG_SOUND_RADIUS) || !m_pPlatformSound)
		return 1.0f;

	// Events attenuate using their distance parameter
	if (m_nFlags & FLAG_SOUND_EVENT)
	{
		if (m_pPlatformSound->GetSoundHandle())
		{
			ptParamF32 fParam(fDistance/m_RadiusSpecs.fDistanceMultiplier);
			m_pPlatformSound->SetParamByType(pspDISTANCE, &fParam);
		}
		
		if (m_nFlags & FLAG_SOUND_PARAM_SPREAD)
		{
			ptParamF32 fParam((fDistance/m_RadiusSpecs.fDistanceMultiplier)/m_RadiusSpecs.fMaxRadius);
			m_pPlatformSound->SetParamByType(pspSPREAD, &fParam);
		}
		
		return (1.0f);
	}
	else
	{
		if (!(m_nFlags & FLAG_SOUND_NO_SW_ATTENUATION))
		{
			float fSquaredDistance = fDistance*fDistance;
			float fMinFactor = __min(fSquaredDistance, (m_RadiusSpecs.fMaxRadius2 - m_RadiusSpecs.fMinRadius2));
			float fMaxFactor = __max(0.f, ( (fMinFactor - m_RadiusSpecs.fMinRadius2) / m_RadiusSpecs.fMaxRadius2 ) );
			float fRatio = 1.0f - fMaxFactor;
			return fRatio;
		}
		else
			return (1.0f);
	}
}

//////////////////////////////////////////////////////////////////////
void CSound::UpdateActiveVolume()
{
	assert (m_pPlatformSound != NULL); 

	// Tomas
	//if (!(m_nFlags & FLAG_SOUND_VOICE))
	//{
	//	m_fActiveVolume = 0.01f;
	//	m_fMaxVolume = 0.01f;
	//}

	if (m_pPlatformSound && m_pPlatformSound->GetSoundHandle())// && nVolume != m_nActiveVolume)
	{
		ptParamF32 NewParam(m_fActiveVolume);
		m_pPlatformSound->SetParamByType(pspVOLUME, &NewParam);
//		m_nActiveVolume = nVolume;
	}
}

//////////////////////////////////////////////////////////////////////
void CSound::SetVolume(const float fMaxVolume)
{
	if (!NumberValid(fMaxVolume))
	{
		m_pSSys->Log(eSLT_Warning, "<Sound> %s Invalid input values (possible NaNs) on Sound::SetVolume()", GetName() );
		assert(0);
		return;
	}

	assert (fMaxVolume < 5);

	m_bVolumeHasChanged = true;

	m_fMaxVolume	= max(0.0f, fMaxVolume);
	m_fMaxVolume  = min(1.0f, m_fMaxVolume);

	//if (m_pSSys->m_pCVarDebugSound->GetIVal()==5)
	//	gEnv->pLog->LogToConsole("VOL:%s,ch=%d,Set vol=%d",GetName(),m_nChannel,nVolume);
	//ChangeVolume(nVolume);
}

//////////////////////////////////////////////////////////////////////
void CSound::SetConeAngles(const float fInnerAngle, const float fOuterAngle) 
{
	m_fInnerAngle = fInnerAngle;
	m_fOuterAngle = fOuterAngle;
	
}

//////////////////////////////////////////////////////////////////////
void CSound::GetConeAngles(float &fInnerAngle,float &fOuterAngle) 
{
	 fInnerAngle = m_fInnerAngle;
	 fOuterAngle = m_fOuterAngle;
}

//////////////////////////////////////////////////////////////////////
void CSound::SetPitch(int nValue)
{
	assert (m_pPlatformSound != NULL); 
	assert (m_pSoundBuffer != NULL); 

	if (!m_pSoundBuffer || !m_pPlatformSound)
		return;

	// freq = The frequency to set. Valid ranges are from 100 to 705600.
	// Scale from 0 - 1000 to FMOD range
	int iReScaledValue;

	iReScaledValue = (int)((float)nValue / 1000.0f * (float)m_pSoundBuffer->GetInfo()->nBaseFreq);
	iReScaledValue = __max(iReScaledValue, 100);
 		
	//if (m_pPlatformSound->GetSoundHandle() && (m_pSoundBuffer->GetType() == btSAMPLE)) // Was restrictied to sanple only
	if (m_pPlatformSound->GetSoundHandle() && (m_pSoundBuffer->GetProps()->eBufferType == btSAMPLE))
	{
		//GUARD_HEAP;

		ptParamINT32 NewParam(iReScaledValue);
		m_pPlatformSound->SetParamByType(pspFREQUENCY, &NewParam);
	}
}

/*
//////////////////////////////////////////////////////////////////////
void CSound::SetLoopPoints(const int iLoopStart, const int iLoopEnd)
{
	// if (m_pSoundBuffer->GetSoundBuffer()) was this, changed it TN TODO
	if (!m_pSoundBuffer)
		return;

	if (m_pSoundBuffer->Loaded())
	{
		GUARD_HEAP;

		// TODO Solve this. what about a INTpair 
		//ptParamINT32 NewParam(iLoopStart);
		//m_pSoundBuffer->SetParam(pspLOOPPOINTS, &NewParam);  // FIX THIS
		//CS_Sample_SetLoopPoints((CS_SAMPLE*)m_pSoundBuffer->GetAssetHandle(), iLoopStart, iLoopEnd);
	}
}
*/

//////////////////////////////////////////////////////////////////////
void CSound::FXEnable(int nFxNumber)
{
	assert (m_pPlatformSound != NULL); 
	assert (m_pSoundBuffer != NULL); 
	assert (m_pSSys);

	//if (!m_pSSys->g_nSoundFX->GetIVal())
		//return;
	// TODO REVIEW: Restricted to Samples only ?!

	if (!m_pSoundBuffer)
		return;

	if (!m_pPlatformSound)
		return;

	if ((m_pSoundBuffer->GetProps()->eBufferType == btSAMPLE) && m_pPlatformSound->GetSoundHandle())
	{
		//GUARD_HEAP;
		ptParamINT32 NewParam(nFxNumber);
		//m_pSoundBuffer->SetParam(pspFXENABLE, &NewParam); // FIX THIS

		//m_nFxChannel=CS_FX_Enable(m_nChannel, nFxNumber);
	}
}

//////////////////////////////////////////////////////////////////////
void CSound::FXSetParamEQ(float fCenter,float fBandwidth,float fGain)
{
	assert (m_pSoundBuffer != NULL); 
	assert (m_pSSys != NULL);

	//if (!m_pSSys->m_pCVarEnableSoundFX->GetIVal())
		//return;

	if (!m_pSoundBuffer)
		return;

	if ((m_pSoundBuffer->GetProps()->eBufferType==btSAMPLE) && (m_nFxChannel>=0))
	{
		// deactivated for Xenon, TODO reactivate and make FX stuff platformindependent
		//GUARD_HEAP;
		//CS_FX_SetParamEQ(m_nFxChannel, fCenter, fBandwidth, fGain);	
	}
}

//////////////////////////////////////////////////////////////////////
//! returns the size of the stream in ms
int CSound::GetLengthMs() const
{
	assert (m_pSoundBuffer != NULL); 
	assert (m_pPlatformSound != NULL); 

	int nLengthInMs = 1;
	ptParamINT32 NewParam(nLengthInMs);

	if (m_pSoundBuffer && m_pPlatformSound)
	{
		if (m_pSoundBuffer->GetParam(apLENGTHINMS, &NewParam))
			NewParam.GetValue(nLengthInMs);
		else if (m_pPlatformSound->GetParamByType(pspLENGTHINMS, &NewParam))
			NewParam.GetValue(nLengthInMs);
	}

	return (nLengthInMs);
}

//////////////////////////////////////////////////////////////////////
//! returns the size of the stream in bytes
int CSound::GetLengthInBytes() const
{  
	assert (m_pSoundBuffer != NULL); 
	assert (m_pPlatformSound != NULL); 

	int nLengthInMs = 1;
	ptParamINT32 NewParam(nLengthInMs);

	if (m_pSoundBuffer && m_pPlatformSound)
	{
		if (m_pSoundBuffer->GetParam(apLENGTHINBYTES, &NewParam))
			NewParam.GetValue(nLengthInMs);
		else if (m_pPlatformSound->GetParamByType(pspLENGTHINBYTES, &NewParam))
			NewParam.GetValue(nLengthInMs);
	}

	return (nLengthInMs);
}

//////////////////////////////////////////////////////////////////////
//! retrieves the currently played sample-pos, in milliseconds or bytes
unsigned int CSound::GetCurrentSamplePos(bool bMilliSeconds) const
{
	assert (m_pPlatformSound != NULL); 
	assert (m_pSoundBuffer != NULL); 

	if (!m_pSoundBuffer || !m_pPlatformSound)
		return 0;

	//GUARD_HEAP;

	if (m_pSoundBuffer->NotLoaded())
	{
		//if (!Preload())
		return (0); //the sound buffer cannot be loaded
	}

	int32 nLength = 0;
	ptParamINT32 NewParam(nLength);

	switch (m_pSoundBuffer->GetProps()->eBufferType)
	{
	case btEVENT:
		if (bMilliSeconds)
			m_pPlatformSound->GetParamByType(pspTIMEPOSITION, &NewParam);
		else
			m_pPlatformSound->GetParamByType(pspSAMPLEPOSITION, &NewParam);
		break;
	case btSAMPLE:
		if (bMilliSeconds)
			m_pPlatformSound->GetParamByType(pspTIMEPOSITION, &NewParam);
		else
			m_pPlatformSound->GetParamByType(pspSAMPLEPOSITION, &NewParam);
		break;
	case btSTREAM:
		if (bMilliSeconds) // TODO Not LENGTH but POSITION
			m_pSoundBuffer->GetParam(apTIMEPOSITION, &NewParam);
		else
			m_pSoundBuffer->GetParam(apBYTEPOSITION, &NewParam);
	}
	NewParam.GetValue(nLength);
	return (nLength);
}

//! set the currently played sample-pos in bytes or milliseconds
//////////////////////////////////////////////////////////////////////
void CSound::SetCurrentSamplePos(unsigned int nPos, bool bMilliSeconds)
{
	assert (m_pPlatformSound != NULL); 
	assert (m_pSoundBuffer != NULL); 

	if (!m_pSoundBuffer || !m_pPlatformSound)
		return;

	//GUARD_HEAP;

	// reset whatever is currently set
	m_nStartOffset = 0;

	if (m_pSoundBuffer->NotLoaded())		
	{
		//if (!Preload())
			return; //the sound buffer cannot be loaded
	}
	switch (m_pSoundBuffer->GetProps()->eBufferType)
	{
		case btSAMPLE:
			{
				if (IsPlayingOnChannel())
				{
					ptParamINT32 NewParam(nPos);

					if (bMilliSeconds)
						m_pPlatformSound->SetParamByType(pspTIMEPOSITION, &NewParam);
					else
						m_pPlatformSound->SetParamByType(pspSAMPLEPOSITION, &NewParam);
				}
				else
				{
					m_nStartOffset = nPos;
					m_bOffsetInSamples = !bMilliSeconds;
				}
			}
			break;
		case btSTREAM:
			{
				ptParamINT32 NewParam(nPos);
				if (bMilliSeconds)
					m_pSoundBuffer->SetParam(apTIMEPOSITION, &NewParam);
					//CS_Stream_SetTime((CS_STREAM*)m_pSoundBuffer->GetAssetHandle(),nPos);
				else
					m_pSoundBuffer->SetParam(apBYTEPOSITION, &NewParam);
					//CS_Stream_SetPosition((CS_STREAM*)m_pSoundBuffer->GetAssetHandle(),nPos);
			}
			break;
		case btEVENT:
			{
				if( bMilliSeconds )
				{
					m_nStartOffset = nPos;
					m_bOffsetInSamples = false;
				}
			}
			break;
	}
}


//////////////////////////////////////////////////////////////////////////
//bool CSound::FadeIn()
//{
//	if (m_fCurrentFade >= 1)
//	{
//		m_fCurrentFade = 1;
//		return true;
//	}
//
//	// this type of fading is purely frame rate dependent
//	m_fCurrentFade += m_fFadingValue*m_pSSys->m_pISystem->GetITimer()->GetFrameTime(); 
//	
//	//gEnv->pLog->LogToConsole("Fade IN, current fade=%f",m_fCurrentFade);
//	if (m_fCurrentFade<0)
//		m_fCurrentFade=0; 
//	else
//	{
//		if (m_fCurrentFade>1.0f)
//		{
//			m_fCurrentFade=1.0f;		
//			//m_nFadeType=0;
//			return (true);
//		}
//	}
//	return (false);
//}

//////////////////////////////////////////////////////////////////////////
//bool CSound::FadeOut()
//{	
//	// this type of fading is purely frame rate dependent
//	m_fCurrentFade-=m_fFadingValue*m_pSSys->m_pISystem->GetITimer()->GetFrameTime(); 
//
//	//gEnv->pLog->LogToConsole("Fade OUT, current fade=%f",m_fCurrentFade);
//
//	if (m_fCurrentFade<0)
//	{
//		m_fCurrentFade=0;		
//		if (IsActive())
//			Deactivate();
//		return (true);
//	}
//	else
//		if (m_fCurrentFade>1.0f)
//			m_fCurrentFade=1.0f;
//
//	return (false);
//}

//////////////////////////////////////////////////////////////////////////
void CSound::OnBufferLoaded()
{
	//if (!m_bAlreadyLoaded)
	{
		AddRef();
		OnEvent( SOUND_EVENT_ON_LOADED );

		if (m_bPlayAfterLoad)
		{
			ESoundSystemErrorCode eResult = eSoundSystemErrorCode_None;

			if (!m_bInitialized)
			{
				// re-init, most likely because the project was not ready during creating
				eResult = Initialize(m_pSoundBuffer, m_nFlags);
				
			}

			if (eResult == eSoundSystemErrorCode_None)
			{
				if (m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_DETAIL)
					m_pSSys->Log(eSLT_Message, "<Sound> %s All data arrived, startin playback again!", GetName());

				m_bPlayAfterLoad = false;
				Play(m_fPostLoadRatio,m_bPostLoadForceActiveState,m_bPostLoadSetRatio);

				//if (m_IsPaused)
				{
					//SetPaused(false);
					//OnEvent( SOUND_EVENT_ON_PLAY );
				}
			}
		}
		Release();
	}
	//m_bAlreadyLoaded = true;
}

//////////////////////////////////////////////////////////////////////////
void CSound::OnAssetUnloaded()
{
	Deactivate();
}
//////////////////////////////////////////////////////////////////////////
void CSound::OnBufferLoadFailed()
{
	AddRef();
	OnEvent( SOUND_EVENT_ON_LOAD_FAILED );
	Release();
}

//////////////////////////////////////////////////////////////////////////
void CSound::OnBufferDelete()
{
	if (m_pSoundBuffer)
		m_pSoundBuffer->RemoveFromLoadReqList(this);

	Stop();
	m_pSoundBuffer = NULL;
	//OnEvent( SOUND_EVENT_ON_LOAD_FAILED );
}

//////////////////////////////////////////////////////////////////////////
void CSound::GetMemoryUsage(class ICrySizer* pSizer) const
{
	// TODO MAJOR REVIEW HERE !
	if(!pSizer->Add(*this))
		return;

	if(m_pPlatformSound && !m_pSoundBuffer)
		m_pPlatformSound->GetMemoryUsage(pSizer);

	if (m_pSoundBuffer)
	{
		//CSoundBuffer *pBuffer = m_pSoundBuffer;
		//pSizer->Add(*pBuffer);

		uint32 dwSize=0xffffffff;

		if(pSizer->GetResourceCollector().AddResource(m_pSoundBuffer->GetName(),dwSize))
		{
			pSizer->GetResourceCollector().OpenDependencies(m_pSoundBuffer->GetName());
	
			// TODO try to gather Resource also via SoundBuffer, which knows about actual file and wavebank references
			pSizer->GetResourceCollector().AddResource(m_pSoundBuffer->GetProps()->sName.c_str(), 0xffffffff);

			if(m_pPlatformSound)
				m_pPlatformSound->GetMemoryUsage(pSizer);

			pSizer->GetResourceCollector().CloseDependencies();
		}
	}
}

bool CSound::IsInCategory(const char* sCategory)
{
	bool bResult = false;

	if (m_pPlatformSound)
		bResult = m_pPlatformSound->IsInCategory(sCategory);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////
bool CSound::IsLoading() const
{
	if (m_pSoundBuffer)
		return m_pSoundBuffer->Loading();
	
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool CSound::IsLoaded() const
{
	if (m_pSoundBuffer)
		return m_pSoundBuffer->Loaded();
		
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool CSound::UnloadBuffer(eUnloadDataOptions UnloadOption)
{
	assert (m_pSoundBuffer != NULL); 

	if (m_pSoundBuffer)
		return m_pSoundBuffer->UnloadData(UnloadOption);

	return false;
}


bool CSound::IsActive() const
{
	if (m_pPlatformSound && m_pPlatformSound->GetSoundHandle() && m_pPlatformSound->GetState() != pssINFOONLY && m_pPlatformSound->GetState() != pssSTOPPED)
		return true;

	return false;
}

//------------------------------------------------------------------------
void CSound::AddEventListener(ISoundEventListener *pListener, const char *sWho)
{
	TEventListenerInfoVector::iterator ItEnd = m_listeners.end();
	for (TEventListenerInfoVector::iterator it = m_listeners.begin(); it != ItEnd; ++it)
	{
		if (it->pListener == pListener)
			return;
	}

	SSoundEventListenerInfo ListenerInfo;
	ListenerInfo.pListener = pListener;

//#ifdef _DEBUG
	memset(ListenerInfo.sWho, 0, sizeof(ListenerInfo.sWho));
	strncpy(ListenerInfo.sWho, sWho, 64);
	ListenerInfo.sWho[63]=0;

	if (m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_EVENTLISTENERS)
	{
		m_pSSys->Log(eSLT_Message, "<Sound> Adding Listener %s on %d\n", ListenerInfo.sWho, (uint32)pListener);		
	}
//#endif

	m_listeners.push_back(ListenerInfo);
	//stl::binary_insert_unique( m_listeners, ListenerInfo );
}

//------------------------------------------------------------------------
void CSound::RemoveEventListener(ISoundEventListener *pListener)
{
	TEventListenerInfoVector::iterator ItEnd = m_listenersToBeRemoved.end();
	for (TEventListenerInfoVector::iterator it = m_listenersToBeRemoved.begin(); it != ItEnd; ++it)
	{
		if (it->pListener == pListener)
			return;
	}

	SSoundEventListenerInfo ListenerInfo;
	ListenerInfo.pListener = pListener;
	ListenerInfo.sWho[0] = 'R';

	m_listenersToBeRemoved.push_back(ListenerInfo);
	//stl::binary_insert_unique( m_listenersToBeRemoved, ListenerInfo );
}

//////////////////////////////////////////////////////////////////////////
void CSound::OnEvent( ESoundCallbackEvent event )
{
	// remove accumulated listeners that unregistered so far
	TEventListenerInfoVector::iterator ItREnd = m_listenersToBeRemoved.end();
	for (TEventListenerInfoVector::iterator ItR = m_listenersToBeRemoved.begin(); ItR != ItREnd; ++ItR)
	{
		//SSoundEventListenerInfo* pListenerInfo = (ItR);
		//stl::binary_erase(m_listeners, *pListenerInfo );
		TEventListenerInfoVector::iterator ItLEnd = m_listeners.end();
		for (TEventListenerInfoVector::iterator itL = m_listeners.begin(); itL != ItLEnd; ++itL)
		{
			if (itL->pListener == ItR->pListener)
			{
				m_listeners.erase(itL);
				break;
			}
		}
	}

	// send the event to listeners
	// for now work on copy because sending SoundEvent might end up in sending another event and that
	// would remove listener, so trashing the m_listener list and iterators. TODO this should be fixed
	// in a clean fashion in a different change
	if (!m_listeners.empty())
	{
		m_listenersTemp = m_listeners;
		//m_listenersTemp.reserve(m_listeners.size());
		//m_listenersTemp.assign(m_listeners.begin(), m_listeners.end());

		// first run through to call event listeners
		TEventListenerInfoVector::const_iterator ItEnd2 = m_listenersTemp.end();
		for (TEventListenerInfoVector::const_iterator It2 = m_listenersTemp.begin(); It2 != ItEnd2; ++It2)
		{
			It2->pListener->OnSoundEvent( event, this );
		}

		// 2nd run through only if in debug
		if (m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_EVENTLISTENERS)
		{
			for (TEventListenerInfoVector::const_iterator It2 = m_listenersTemp.begin(); It2 != ItEnd2; ++It2)
			{
				m_pSSys->Log(eSLT_Message, "<Sound> Calling Listener %s on %d\n", It2->sWho, (uint32)It2->pListener);		
			}
		}
	}

	// Tell the Soundsystem about this
	m_pSSys->OnEvent((ESoundSystemCallbackEvent) event, this);

	m_listenersTemp.resize(0);
}

//////////////////////////////////////////////////////////////////////////
bool CSound::IsPlayLengthExpired( float fCurrTime  ) const
{
	assert (m_pSSys);

	if (m_pPlatformSound && (m_pPlatformSound->GetState() == pssWAITINGFORSYNC) && (m_fSyncTimeout < 0))
		return true;

	if (GetLoopMode() || m_pSSys->m_bSoundSystemPause)
		return false;

	float fSeconds = m_tPlayTime.GetSeconds();

	// If there is no valid playtime maybe the sound was deactivated
	if (fSeconds < 0)
 		return true;

	float fDuration = (GetLengthMs()+1) / 1000.0f;

	// 1 extra second for sounds that are paused but got never unpaused (maybe because of missing obstruction callback
	if (m_IsPaused)
		fDuration += 1; 

	if (fCurrTime > fSeconds + fDuration + 0.5f) // half a second longer to give the callback a chance to catch it
		return true;
	
	return false;
}


//////////////////////////////////////////////////////////////////////////
// Information
//////////////////////////////////////////////////////////////////////////

// Gets and Sets Parameter defined in the enumAssetParam list
bool CSound::GetParam(enumSoundParamSemantics eSemantics, ptParam* pParam) const
{
	switch (eSemantics)
	{
	case spNONE:	
		return (false); 
		break;
	case spINITIALIZED:
		{
			bool bTemp;
			if (!(pParam->GetValue(bTemp))) 
				return (false);
			bTemp = m_bInitialized;
			pParam->SetValue(bTemp);
			break;
		}
	case spSOUNDID:
		{
			int32 nTemp;
			if (!(pParam->GetValue(nTemp))) 
				return (false);
			nTemp = m_nSoundID;
			pParam->SetValue(nTemp);
			break;
		}
	case spMINRADIUS:
		{
			float fTemp;
			if (!(pParam->GetValue(fTemp))) 
				return (false);
			fTemp = m_RadiusSpecs.fMinRadius;
			pParam->SetValue(fTemp);
			break;
		}
	case spMAXRADIUS:
		{
			float fTemp;
			if (!(pParam->GetValue(fTemp))) 
				return (false);
			fTemp = m_RadiusSpecs.fMaxRadius;
			pParam->SetValue(fTemp);
			break;
		}
	case spPITCH:
		{
			float fTemp;
			if (!(pParam->GetValue(fTemp))) 
				return (false);
			fTemp = m_fPitching;
			pParam->SetValue(fTemp);
			break;
		}
	case spSYNCTIMEOUTINSEC:
		{
			float fTemp;
			if (!(pParam->GetValue(fTemp))) 
				return (false);
			fTemp = m_fSyncTimeout;
			pParam->SetValue(fTemp);
			break;
		}
	case spLENGTHINMS:
		{
			int32 nTemp;
			if (!(pParam->GetValue(nTemp))) 
				return (false);
			nTemp = GetLengthMs();
			pParam->SetValue(nTemp);
			break;
		}
	default:
		return (false);
		break;
	}

	
	return (false);
}

bool CSound::SetParam(enumSoundParamSemantics eSemantics, ptParam* pParam)
{	
	float fNanTest = 0.0f;
	if (pParam->GetValue(fNanTest) && !NumberValid(fNanTest))
	{
		m_pSSys->Log(eSLT_Warning, "<Sound> %s Invalid input values (possible NaNs) on Sound::SetParam() by Type", GetName() );
		assert(0);
		return false;
	}

	switch (eSemantics)
	{
	case spNONE:	
		return (false); 
		break;
	case spINITIALIZED:
		{
			bool bTemp;
			if (!(pParam->GetValue(bTemp))) 
				return (false);
			m_bInitialized = bTemp;
			break;
		}
	case spPITCH:
		{
			float fTemp;
			if (!(pParam->GetValue(fTemp))) 
				return (false);
			m_fPitching = fTemp;
			break;
		}
	case spSYNCTIMEOUTINSEC:
		{
			float fTemp;
			if (!(pParam->GetValue(fTemp))) 
				return (false);
			m_fSyncTimeout = fTemp;
			break;
		}
	case spREVERBWET:
		{
			float fTemp;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			if (m_pPlatformSound)
				return m_pPlatformSound->SetParamByType(pspREVERBWET, pParam);

			break;
		}
	case spREVERBDRY:
		{
			float fTemp;
			if (!(pParam->GetValue(fTemp))) 
				return (false);

			if (m_pPlatformSound)
				return m_pPlatformSound->SetParamByType(pspREVERBDRY, pParam);

			break;
		}
	case spTIMEPOSITION:
		{
			int32 nTemp;
			if (!(pParam->GetValue(nTemp))) 
				return (false);

			if (m_pPlatformSound)
				return m_pPlatformSound->SetParamByType(pspTIMEPOSITION, pParam);

			break;
		}
	default:
		return (false);
		break;
	}
	return (false);
}


// Gets and Sets Parameter defined by string and float value
int CSound::GetParam(const char *sParameter, float *fValue, bool bOutputWarning) const
{
	assert (m_pPlatformSound != NULL); 
	assert(fValue);

	if (stricmp(sParameter,"HDRA")==0)
	{
		*fValue = m_HDRA;
		return true;
	}

	int nIndex = -1;

	if (m_pPlatformSound)
		return m_pPlatformSound->GetParamByName(sParameter, fValue, bOutputWarning);
		
	//gEnv->pLog->Log("Error: Sound: Parameter <%s> couldn't be get on %s\n", sParameter, GetName().c_str());
	return nIndex;
}

int CSound::SetParam(const char *sParameter, float fValue, bool bOutputWarning)
{
	if (!NumberValid(fValue))
	{
		m_pSSys->Log(eSLT_Warning, "<Sound> %s Invalid input values (possible NaNs) on Sound::SetParam() by Name %s", GetName(), sParameter );
		assert(0);
		return false;
	}

	assert (m_pPlatformSound != NULL); 

	//if (stricmp(sParameter,"HDRA")==0)
	//{
	//	m_HDRA = fValue;
	//	return true;
	//}

	int nIndex = -1;

	if (m_pPlatformSound)
		return m_pPlatformSound->SetParamByName(sParameter, fValue, bOutputWarning);

	//gEnv->pLog->Log("Error: Sound: Parameter <%s> (%f) couldn't be set on %s\n", sParameter, fValue, GetName().c_str());
	return nIndex;
}


// Gets and Sets Parameter defined by index and float value
bool CSound::GetParam(int nIndex,
											float* const pfValue,
											float* const pfRangeMin/* =NULL */,
											float* const pfRangeMax/* =NULL */,
											char const** const ppcParameterName/* =NULL */,
											bool const bOutputWarning/* =true */) const
{
	assert(pfValue);
	assert(m_pPlatformSound != NULL); 

	if(m_pPlatformSound)
		return m_pPlatformSound->GetParamByIndex(nIndex, pfValue, pfRangeMin, pfRangeMax, ppcParameterName, bOutputWarning);

	//gEnv->pLog->Log("Error: Sound: Parameter <%s> couldn't be get on %s\n", sParameter, GetName().c_str());
	return false;
}

bool CSound::SetParam(int nIndex, float fValue, bool bOutputWarning)
{
	if (!NumberValid(fValue))
	{
		m_pSSys->Log(eSLT_Warning, "<Sound> %s Invalid input values (possible NaNs) on Sound::SetParam() by Index", GetName() );
		assert(0);
		return false;
	}

	assert(m_pPlatformSound != NULL); 

	if (m_pPlatformSound)
		return m_pPlatformSound->SetParamByIndex(nIndex, fValue, bOutputWarning);

	//gEnv->pLog->Log("Error: Sound: Parameter <%s> (%f) couldn't be set on %s\n", sParameter, fValue, GetName().c_str());
	return false;
}

#pragma warning(default:4003)

//#endif //_XBOX