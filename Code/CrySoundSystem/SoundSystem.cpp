//////////////////////////////////////////////////////////////////////
//
//  Crytek (C) 2001
//
//  CrySound Source Code
//
//  File: SoundSystem.cpp
//  Description: ISoundSystem interface implementation.
//
//  History:
//	-June 06,2001:Implemented by Marco Corbetta
//  -June 01,2005: Implemented group manager and made 3 script io routines NICK
//                3 routines are Precache, NewPlay, and NewPlayEx
//
//////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
//#ifndef _XBOX
#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CryCore/CrySizer.h"
#include <algorithm>
#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/CrySystem/ITimer.h"
#include <Cry_Math.h>
#include <IXml.h>
#include "CryCommon/CryMath/Cry_Camera.h"
#include "CryCommon/CryRenderer/IRenderer.h"
#include "CryCommon/CryRenderer/IRenderAuxGeom.h"
#include "CryCommon/CryEntitySystem/IEntitySystem.h"
#include "IAudioDevice.h"
#include "ISoundBuffer.h"
#include "CryCommon/CrySoundSystem/IReverbManager.h"
#include "SoundSystem.h"
#include "IPlatformSound.h"
#include "IMusicSystem.h"
#include "ILocalizationManager.h" //Voice file
#include "Sound.h"
#include "CryCommon/Cry3DEngine/I3DEngine.h" //needed to check if the listener is in indoor or outdoor
#include "CryCommon/CrySystem/ICryPak.h" //needed to check if the listener is in indoor or outdoor
#include <ISerialize.h>
#include "MoodManager.h"
#include "StringUtils.h"
#include <IGame.h> // musiclogic debug drawing
#include "IGameFramework.h" // musiclogic debug drawing
#include "BitFiddling.h" // char-conversiont to playback filter

#ifdef SOUNDSYSTEM_USE_FMODEX400
#include "ReverbManagerEAX.h"
#include "ReverbManagerDSP.h"

#include "Microphone.h"
#include "MicrophoneStream.h"
#endif

#include "MusicSystem/MusicSystem.h"

TFixedResourceName CSoundSystem::g_sProjectPath;
TFixedResourceName CSoundSystem::g_sProjectName;
TFixedResourceName CSoundSystem::g_sGroupName;
TFixedResourceName CSoundSystem::g_sEventName;
TFixedResourceName CSoundSystem::g_sFileName;
TFixedResourceName CSoundSystem::g_sKeyName;
unsigned int CSoundSystem::g_nBuffersTrashed;
//TFixedResourceName sStringLocal;
//CRopeSurfaceCache* CSoundSystem::g_pRopeSurfaceCache = 0;


//ISoundAssetManager* CSoundSystem::m_pSoundAssetManager = NULL;

//#define CS_BUFFERSIZE   10         /* millisecond value for FMOD buffersize. */

static ILINE CSaltHandle<> IdToHandle( const tSoundID id ) { return CSaltHandle<>(id>>16,id&0xffff); }
static ILINE tSoundID HandleToId( const CSaltHandle<> id ) { return (((uint32)id.GetSalt())<<16) | ((uint32)id.GetIndex()); }

//static ILINE tSoundID HandleToId( const CSaltHandle<> id ) { return (((uint32)id.GetSalt())<<16) | ((uint32)id.GetIndex()); }
//////////////////////////////////////////////////////////////////////////
/*
void* _DSPUnit_SFXFilter_Callback(void *pOriginalBuffer, void *pNewBuffer, int nLength, INT_PTR nParam)
{
	if (!nParam)
		return pNewBuffer;
	CSoundSystem *pOwner=(CSoundSystem*)nParam;
	return pOwner->DSPUnit_SFXFilter_Callback(pOriginalBuffer, pNewBuffer, nLength);
}

*/


int OnObstructionTest(const EventPhys *pEvent)
{
	CSoundSystem *pSSys = (CSoundSystem*) gEnv->pSoundSystem;

	if (!pSSys->g_nObstruction)
		return 1;

	EventPhysRWIResult *pRWIResult = (EventPhysRWIResult*)pEvent;
	if (pRWIResult->iForeignData == PHYS_FOREIGN_ID_SOUND_OBSTRUCTION)
	{
		SObstructionTest* pObstructionTest = (SObstructionTest*) pRWIResult->pForeignData;

		_smart_ptr<CSound> pSound = (CSound*) pSSys->GetSound(pObstructionTest->SoundID);

		if (pSound) // check if the sound is still valid
		{
			pSound->m_Obstruction.bProcessed = false;
			pObstructionTest->nHits = pRWIResult->nHits;
			pObstructionTest->bResult = true;
			pObstructionTest->nPierceability = 0;

			if (pRWIResult->nHits)
			{
				float fDistance = FLT_MAX;
				ISurfaceTypeManager *pSurfaceTypeManager = gEnv->p3DEngine->GetMaterialManager()->GetSurfaceTypeManager();

				for (int j=0; j<10; j++)
					if (pRWIResult->pHits[j].dist > 0)
					{
						ISurfaceType *pMat = pSurfaceTypeManager->GetSurfaceType(pRWIResult->pHits[j].surface_idx);
						if (pMat)
							pObstructionTest->nPierceability += (15 - pMat->GetPhyscalParams().pierceability);

						fDistance = min(fDistance, pRWIResult->pHits[j].dist);
						//if (pRWIResult->pHits[j].pCollider)
						//{
						//	IEntity * pEntity = (IEntity*) pRWIResult->pHits[j].pCollider->GetForeignData(PHYS_FOREIGN_ID_ENTITY);
						//	if (pEntity)
						//	{
						//		IPhysicalEntity* pPhysicalEntity = pEntity->GetPhysics();
						//		int bla = 0;
						//	}
						//}
					}
					pObstructionTest->fDistance = fDistance;
			}

			bool bTempFirst = pSound->m_Obstruction.bFirstTime;
			pSound->m_Obstruction.bFirstTime = false;
			pSound->m_Obstruction.bDelayPlayback = false;
			pSound->m_Obstruction.bDontAveragePrevious = false;

			if (bTempFirst && !(pSound->GetFlags() & FLAG_SOUND_START_PAUSED))
			{
				pSound->m_Obstruction.bDontAveragePrevious = true;
				pSound->Play();
			}
		}
	}


	return 1;
}


//////////////////////////////////////////////////////////////////////
CSoundSystem::CSoundSystem(void* hWnd, IAudioDevice *pAudioDevice) : CSoundSystemCommon()
{
	//m_nMemTest = 0;

	// Constructor deals with an UNINITIALIZED AudioDevice
	// Init() can rely on an initialized AudioDevice
	GUARD_HEAP;

	assert(pAudioDevice!=NULL);

	//m_pNetworkSound = NULL;
	//m_pMicrophone = NULL;
	//int nTest = sizeof(CSound);
		
	m_bSoundSystemEnabled = true;
	m_bDelayPrecaching		= true;
	m_bOK									= false;
	m_bIsInPauseCall			= false;

	m_pAudioDevice				= NULL;
	m_pSoundAssetManager	= NULL;
	m_pReverbManager      = NULL;
  m_pMoodManager				= NULL;
	m_pDebugLogger				= NULL;
	m_MemUsage						= 0;
	m_ErrorCode						= eSoundSystemErrorCode_None;

	//m_TempBufferProps.sName.reserve(200);

	if (g_nDummySound)		
		return; // creates dummy sound system

	if (!pAudioDevice) 
		return;

	// setting the platform dependent AudioDevice implementation
	m_pAudioDevice = pAudioDevice;

	m_tUpdateSounds.SetMilliSeconds(0);
	m_tUpdateSoundAssetManager.SetMilliSeconds(0);
	m_tUpdateAudioDevice.SetMilliSeconds(0);
	m_tUpdateReverbManager.SetMilliSeconds(0);

	//m_pVisArea						=	NULL;
	m_fSFXVolume					=	1.0f;
	m_fGameSFXVolume			=	1.0f;
	m_fMusicVolume				=	1.0f;
	m_fGameMusicVolume		=	1.0f;
  m_fMusicEffectVolume	= 1.0f;
	m_fDialogVolume				= 1.0f;
	m_fGameDialogVolume		= 1.0f;
	m_fMovieFadeoutVolume = 1.0f;

//	m_nSpeakerConfig			=	-1; //force to set
	m_nReverbType					= 0;
	m_nFadeOutInMS				= 0;
	m_bSoundSystemPause		=	false;

	assert (gEnv->pSystem);
	m_pStreamEngine = gEnv->pSystem->GetStreamEngine();

	m_nLastInactiveSoundsIndex = 0;
	m_nBuffersLoaded					= 0;

	// Weather related
	m_fWeatherTemperatureInCelsius = 28.0;
	m_fWeatherHumidityAsPercent = 80.0;
	m_fWeatherInversion = 0.0;
	m_fWeatherAirAbsorptionMultiplyer = 1.0;

	m_nMinSoundPriority = 0;
	//m_pDSPUnitSFXFilter=NULL;
	m_fDirAttCone				=	0.0f;
	m_nMuteRefCnt				=	0;
	m_nDialogSoundMoodAlloc = 1;
	m_nDialogEventParameterAlloc = 1;

	 m_DirAttPos = Vec3(0);
	m_DirAttDir = Vec3(0);
	m_fDirAttCone = 1.0f;
	m_fDirAttMaxScale = 0.0f;
	m_bNeedSoundZoomUpdate = false;

	// profiling
	m_nMaxUsedIDs = 0;

  m_nEaxStatus				=	0;
  m_bNotCalledYetEAX	= 0;

	// group stuff
	for (int i=0;i<MAX_SOUNDSCALE_GROUPS;i++) m_fSoundScale[i]=1.0f;
	
	//m_VisAreas.Clear();
	//memset(m_VisAreas,0,MAX_VIS_AREAS*sizeof(int));
	m_DelayedPrecaches.clear();

	m_nSampleRate = g_nFormatSampleRate;

	uint32 dwMaxIndex = (uint32)(m_SoundSaltBuffer.GetTSize());
	m_AllSoundsArray.resize(dwMaxIndex);

	m_SoundTempVisAreas.PreAllocate(MAX_VIS_AREAS);

	//m_InactiveSounds.reserve(50);
	//m_LoopedSounds.reserve(100);
	//m_OneShotSounds.reserve(100);
	//m_stoppedSoundToBeDeleted.reserve(50);
	
	//if (!m_pCVARSoundEnable->GetIVal())		
	//	return;

/*
	if (m_pDSPUnitSFXFilter)
	{
		m_fDSPUnitSFXFilterCutoff=500.0f;
		m_fDSPUnitSFXFilterResonance=0.5f;
		m_fDSPUnitSFXFilterLowLVal=0.0f;
		m_fDSPUnitSFXFilterBandLVal=0.0f;
		m_fDSPUnitSFXFilterLowRVal=0.0f;
		m_fDSPUnitSFXFilterBandRVal=0.0f;
		//CS_DSP_SetActive(m_pDSPUnitSFXFilter, TRUE);
	}
*/
	m_bOK = true;

	// create ReverbManager
	#ifdef SOUNDSYSTEM_USE_FMODEX400
	if (g_nReverbType == REVERB_TYPE_HARDWARE || g_nReverbType == REVERB_TYPE_SOFTWARE_LOW || g_nReverbType == REVERB_TYPE_SOFTWARE_HIGH || g_nReverbType == REVERB_TYPE_NONE)
		m_pReverbManager = (IReverbManager*) new CReverbManagerEAX;
	else
		m_pReverbManager = (IReverbManager*) new CReverbManagerDSP;
	#endif

	m_nReverbType = -1; // Forces to set the reverb type in Update() after ReverbManager is initilized by the Audiodevice

	m_bUpdateNeededMoodManager = true;

	strcpy(m_szEmptyName,"NONAME"); // TODO clean up

	m_bResetVolume		=	false;
	m_fSFXResetVolume	=	1.0f;

	m_pSoundAssetManager = (ISoundAssetManager*) new CSoundAssetManager(this);
	
	m_pDebugLogger = (CDebugLogger*) new CDebugLogger("Sound.log");
// MUST set this data before init

  ptParamBOOL DopplerStatus((g_nDoppler ? true : false));// ***** I need bool system gives int
  m_pAudioDevice->SetParam(adpDOPPLER, &DopplerStatus);

  ptParamINT32 mixingRate(g_nFormatSampleRate);
  m_pAudioDevice->SetParam(adpSYSTEM_FREQUENCY , &mixingRate);

	//m_nSpeakerConfig = m_pCVarSpeakerConfig->GetIVal();

  ptParamINT32 speakertype(g_nSpeakerConfig);
  m_pAudioDevice->SetParam(adpSPEAKER_MODE, &speakertype);

	AudioDeviceSettings InitSettings;
	m_pAudioDevice->GetInitSettings(&InitSettings);

	InitSettings.nMaxHWchannels			= g_nMaxHWChannels;
	InitSettings.nMinHWChannels			= g_nMinHWChannels;
	InitSettings.nSoftwareChannels	= g_nSoftwareChannels;

	//SOUND_MAX_SOFTWARE_CHANNELS;

	m_pAudioDevice->SetInitSettings(&InitSettings);

	m_dwMainThreadId = CryGetCurrentThreadId();

	//m_VisAreas.PreAllocate(MAX_VIS_AREAS);
}

bool CSoundSystem::Init()
{
	// Constructor deals with an UNINITIALIZED AudioDevice
	// Init() can rely on an initialized AudioDevice

	// sets master volume to highest level
	ptParamF32 masterVolume(1.0f);
	m_pAudioDevice->SetParam(adpMASTER_VOLUME, &masterVolume);

	// this is for Craig
	//////////////////////////////////////////////////////////////////////////
	//CMicrophoneStream MicStream;
	//IMicrophoneStream *pMicStream = &MicStream;
	//CMicrophone Mic(pMicStream, m_pAudioDevice);
	//Mic.Record(0, 16,44100,44100);
	//Mic.Stop();
	//Mic.Release();
	//////////////////////////////////////////////////////////////////////////

	string sEventName;

	//Precache("Sounds/weapons:scar:select", 0, sEventName);
		//Precache("Sounds/error:error", 0, sEventName);

	// first load a event then parse all categories
	m_pMoodManager      = (ISoundMoodManager*) new CMoodManager();
	
	gEnv->pPhysicalWorld->AddEventClient(EventPhysRWIResult::id, OnObstructionTest, 1);
	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener( this );

	// Languages workaround, makes the project loads first
	//Precache("sounds/dialog:human", 0, FLAG_SOUND_PRECACHE_LOAD_SOUND|FLAG_SOUND_PRECACHE_LOAD_GROUP|FLAG_SOUND_PRECACHE_STAY_IN_MEMORY);

	
	//Precache("island/hawker_island_ab1_01000", FLAG_SOUND_VOICE, FLAG_SOUND_PRECACHE_LOAD_SOUND|FLAG_SOUND_PRECACHE_LOAD_GROUP|FLAG_SOUND_PRECACHE_STAY_IN_MEMORY|FLAG_SOUND_PRECACHE_UNLOAD_AFTER_PLAY);

	//ISound* pSound = CreateSound("game_over", 0);
	//pSound->Play();

	// debug a crash
	
	//ISound *pSound = NULL;
	//tSoundID m_sounds[5000];

	//while (!pSound)
	//{
	//	float fRand = (float)rand()/(float)RAND_MAX;
	//	float fTest = fRand*65000.0f*65000.0f;
	//	tSoundID nNotInitialized = (tSoundID) fTest;
	//	pSound = GetSound(nNotInitialized);
	//}

	//if (pSound)
	//	int a = 1;

	// prepare 100 CSoundObjects
	// they will be stored in the UnusedSoundObjects container
	SSoundContainer PrecacheSounds;
	PrecacheSounds.reserve(100);
	m_UnusedSoundObjects.reserve(100);
	_smart_ptr<CSound> pSound = NULL;

	for (int i=0; i<100; ++i)
	{
		pSound = GetNewSoundObject();
		PrecacheSounds.push_back(pSound);
	}

	PrecacheSounds.clear();

	// call Update once so some member variables are set from cvars and reverb is build
	Update(eSoundUpdateMode_All);

	return true;
}

void CSoundSystem::Log( const ESoundLogType eType, const char *szFormat,... )
{

	if (!szFormat)
		return;

	char szBuffer[MAX_WARNING_LENGTH];
	va_list	ArgList;
	va_start(ArgList, szFormat);
	int count = vsnprintf_s(szBuffer,sizeof(szBuffer),sizeof(szBuffer)-1,szFormat, ArgList);
	szBuffer[sizeof(szBuffer)-1] = '\0';
	va_end(ArgList);

	char *szString = szBuffer;

	bool bBlock = g_fSpamFilterTimeout > 0 && m_SpamFilter.IsSpam(szString);
	bool bInsert = g_fSpamFilterTimeout > 0 && !bBlock;

	if (bInsert && eType != eSLT_Always)
		m_SpamFilter.Insert(szString, g_fSpamFilterTimeout);

	switch(eType)
	{
	case eSLT_Message:
		if (!bBlock)
			gEnv->pLog->Log("%s", szString);
		break;
	case eSLT_Warning:
		if (!bBlock)
			gEnv->pSystem->Warning(VALIDATOR_MODULE_SOUNDSYSTEM, VALIDATOR_WARNING, VALIDATOR_FLAG_SOUND, 0, "%s", szString); 
		break;
	case eSLT_Error:
		if (!bBlock)
			gEnv->pSystem->Warning(VALIDATOR_MODULE_SOUNDSYSTEM, VALIDATOR_ERROR, VALIDATOR_FLAG_SOUND, 0, "%s", szString); 
		break;
	case eSLT_Always:
		CryLogAlways("%s", szString);
		break;
	default:
		assert(0);
		break;
	}

}

EPrecacheResult CSoundSystem::Precache( const char *sGroupAndSoundName, uint32 nSoundFlags, uint32 nPrecacheFlags)
{
	if (!g_nPrecacheData)
		return ePrecacheResult_Disabled;

	if (nPrecacheFlags & FLAG_SOUND_PRECACHE_STAY_IN_MEMORY)
	{
		int a = 0;	(void)a;
	}

	CSoundBuffer *pBuf = PrecacheSound(sGroupAndSoundName, nSoundFlags, nPrecacheFlags);

	if (pBuf)
	{
		pBuf->GetProps()->nFlags = nSoundFlags;
		pBuf->GetProps()->nPrecacheFlags = nPrecacheFlags;


		//if (pBuf->GetProps()->eBufferType == btSAMPLE && !(nSoundFlags & FLAG_SOUND_VOICE))
		//{
		//	Log(eSLT_Error, "<Sound> Sample sound %s not marked as Voice!", sGroupAndSoundName);
		//}
	}

	if (m_bDelayPrecaching)
		return ePrecacheResult_Delayed;

	if (pBuf != NULL)
		return ePrecacheResult_OK;

	if (GetError() & eSoundSystemErrorCode_ProjectNotReady)
		return ePrecacheResult_ProjectNotReady;

	return ePrecacheResult_Error;

}

// Precaches the sound data of a sound definition
CSoundBuffer* CSoundSystem::PrecacheSound( const char *sGroupAndSoundName, uint32 nSoundFlags, uint32 nPrecacheFlags)
{
	FUNCTION_PROFILER( gEnv->pSystem, PROFILE_SOUND );

	if (!sGroupAndSoundName || stricmp(sGroupAndSoundName,"")==0 || !m_bSoundSystemEnabled) 
		return (NULL);

	g_sFileName = sGroupAndSoundName;
	int nReturnFlags = ParseGroupString(g_sFileName, g_sProjectPath, g_sProjectName, g_sGroupName, g_sEventName, g_sKeyName);

	// something wrong
	if (nReturnFlags == 0)
		return NULL;

	CSoundBuffer *pBuf			 = NULL;
	m_TempBufferProps.nFlags = nSoundFlags & SOUNDBUFFER_FLAG_MASK;

	bool bSysPreload = false;

	//GDC2008 console workaround, remove AND with global preload
	//ICVar *pSysPreload = gEnv->pConsole->GetCVar( "sys_preload" );
	//if (pSysPreload && pSysPreload->GetIVal() != 0)
		bSysPreload = true;

	// Load data if precaching and sysPreload is activated
	m_TempBufferProps.nPrecacheFlags = (g_nPrecacheData && bSysPreload) ? nPrecacheFlags : (nPrecacheFlags & ~FLAG_SOUND_PRECACHE_LOAD_GROUP);

	if (nReturnFlags & PSNRC_EVENT || nReturnFlags & PSNRC_GROUP)
	{
		m_TempBufferProps.eBufferType = btEVENT;

		const char *pProjectPath = g_sProjectPath;
		m_TempBufferProps.sName.assign(pProjectPath, g_sProjectPath.length()-1); // skip the last slash
		m_TempBufferProps.sName += ":";
		m_TempBufferProps.sName += g_sGroupName;
		m_TempBufferProps.sUserDataText = g_sGroupName.c_str();

		// Check if Project is loaded
		g_sProjectName += ".fev";

		bool bAsyncLoading = 	!(nSoundFlags & FLAG_SOUND_LOAD_SYNCHRONOUSLY) && !(nPrecacheFlags & FLAG_SOUND_PRECACHE_LOAD_PROJECT) && g_StreamProjectFiles;
		m_TempBufferProps.nProjectIndex = m_pAudioDevice->LoadProjectFile(g_sProjectPath, g_sProjectName, bAsyncLoading);

	}
	else
	{
		if (nSoundFlags & FLAG_SOUND_STREAM)
			m_TempBufferProps.eBufferType = btSTREAM;
		else
			m_TempBufferProps.eBufferType = btSAMPLE;

		m_TempBufferProps.sName = g_sFileName;

		bool bFoundFile = false;

		// check if suggested compression is available
		int nPos = g_sFileName.length() - 4;
		if (nPos > 0)
		{
			int nTryThisCompression = g_nCompression ? g_nCompression : g_nInternCompression;

			if (nTryThisCompression == 1 && !bFoundFile)
			{
				g_sFileName.replace(nPos, 4, ".fsb");
				uint32 nSize = gEnv->pCryPak->FGetSize(g_sFileName.c_str());
				bFoundFile = (nSize > 0);
				nTryThisCompression = 4;
			}

			if (nTryThisCompression == 4 && !bFoundFile)
			{
				g_sFileName.replace(nPos, 4, ".xma");
				uint32 nSize = gEnv->pCryPak->FGetSize(g_sFileName.c_str());
				bFoundFile = (nSize > 0);
				nTryThisCompression = 2;
			}

			if (nTryThisCompression == 2 && !bFoundFile)
			{
				g_sFileName.replace(nPos, 4, ".mp2");
				uint32 nSize = gEnv->pCryPak->FGetSize(g_sFileName.c_str());
				bFoundFile = (nSize > 0);
				nTryThisCompression = 3;
			}

			if (nTryThisCompression == 3 && !bFoundFile)
			{
				g_sFileName.replace(nPos, 4, ".mp3");
				uint32 nSize = gEnv->pCryPak->FGetSize(g_sFileName.c_str());
				bFoundFile = (nSize > 0);
				nTryThisCompression = 5;
			}

			if (nTryThisCompression == 5 && !bFoundFile)
			{
				g_sFileName.replace(nPos, 4, ".wav");
				uint32 nSize = gEnv->pCryPak->FGetSize(g_sFileName.c_str());
				bFoundFile = (nSize > 0);

				if (!bFoundFile)
				{
					// restore original file name
					g_sFileName = sGroupAndSoundName;
					nSize = gEnv->pCryPak->FGetSize(g_sFileName.c_str());
					bFoundFile = (nSize > 0);
				}
			}
		}

		if (!bFoundFile)
			return NULL;

		m_TempBufferProps.sName = g_sFileName;
	}

	if (nReturnFlags & PSNRC_VOICE || nSoundFlags & FLAG_SOUND_VOICE)
		m_TempBufferProps.nFlags |= FLAG_SOUND_VOICE;

	//if (nPrecacheFlags & FLAG_SOUND_STREAM)
	//	m_TempBufferProps.eBufferType = btSTREAM;


	pBuf = m_pSoundAssetManager->GetOrCreateSoundBuffer(m_TempBufferProps, NULL);

	// Load if sound OR Group should be 
	//if (pBuf && (nPrecacheFlags & (FLAG_SOUND_PRECACHE_LOAD_SOUND|FLAG_SOUND_PRECACHE_LOAD_GROUP)))
	{
		if (m_bDelayPrecaching)
			//pBuf->Load(NULL);
		//else
			m_DelayedPrecaches[m_TempBufferProps] = NULL;
		else
		{
			if (m_TempBufferProps.nProjectIndex != PROJECTINDEX_INVALID)
			{
				pBuf->Load(NULL);
			}
		}
	}

	if (!pBuf->Loading() && pBuf->LoadFailure() && !g_nErrorSound)
		return NULL;

	if (gEnv->pCryPak->GetLvlResStatus() || g_nProfiling)
	{
		// dont add event groups into sound info map
		if (!(nReturnFlags & PSNRC_GROUP))
		{
			ISoundProfileInfo* pSoundInfo = NULL;

			tmapSoundInfo::iterator It = m_SoundInfos.find(CONST_TEMP_STRING(g_sFileName));

			if (It == m_SoundInfos.end())
			{
				pSoundInfo = new CSoundProfileInfo(g_sFileName);

				if (pSoundInfo)
					m_SoundInfos[g_sFileName] = pSoundInfo;
			}
			else
				pSoundInfo = (*It).second;

			pSoundInfo->GetInfo()->nSoundIDsUsed = m_nMaxUsedIDs;

			if (!pSoundInfo->GetInfo()->bWavebanksLogged)
			{
				// create all wavebanks dependencies for this Soundinfo
				pBuf->LogDependencies();
				pSoundInfo->GetInfo()->bWavebanksLogged = true;
			}
		}
	}

	return pBuf;  
}

CSound* CSoundSystem::GetNewSoundObject()
{
	CSound* pSound = NULL;
	
	if (m_UnusedSoundObjects.empty())
	{
		pSound = new CSound(this);
	}
	else
	{
		pSound = m_UnusedSoundObjects[m_UnusedSoundObjects.size()-1];
		m_UnusedSoundObjects.pop_back();
	}

	return pSound;
}

bool CSoundSystem::RemoveSoundObject(CSound* pSound)
{
	if (pSound)
	{
		pSound->Reset();
		m_UnusedSoundObjects.push_back(pSound);
	}

	return true;
}

ISound* CSoundSystem::CreateSound( const char *sSoundOrEventName, uint32 nFlags )
{
	ISound *pSound = NULL;

	// dont call this from other than Main thread
	if (!IsMainThread())
	{
		Log(eSLT_Error, "<Sound> %s Called CreateSound from other than Main thread", sSoundOrEventName );

//		int nCreateSoundCalledFromNonMainThread = 0;
//		assert(nCreateSoundCalledFromNonMainThread);

		m_ErrorCode = (ESoundSystemErrorCode) (m_ErrorCode | eSoundSystemErrorCode_NotCalledFromMainThread);
		return pSound;
	}

	if (nFlags == 0 || nFlags == 4)
	{
		int nCatchme = 1;	(void)nCatchme;
	}
	//if no Sound.pak is present, this will crash on any platform
#if !defined(PS3) || defined(SOUNDSYSTEM_USE_FMODEX400)
	pSound = LoadSound(sSoundOrEventName, nFlags, FLAG_SOUND_PRECACHE_LOAD_SOUND);
#endif
	return pSound;
}

ISound* CSoundSystem::CreateLineSound( const char *sSoundOrEventName, uint32 nFlags, const Vec3 &vStart, const Vec3 &VEnd )
{
	if (!vStart.IsValid() || !VEnd.IsValid())
	{
		Log(eSLT_Warning, "<Sound> %s Invalid input values (possible NaNs) on CreateLineSound()", sSoundOrEventName );
		assert(0);
		return false;
	}
	
	ISound *pSound = NULL;

#if !defined(PS3) || defined(SOUNDSYSTEM_USE_FMODEX400)
	pSound = LoadSound(sSoundOrEventName, nFlags, FLAG_SOUND_PRECACHE_LOAD_SOUND);
	if (pSound)
		pSound->SetLineSpec(vStart, VEnd);
#endif
	return pSound;
}

ISound* CSoundSystem::CreateSphereSound( const char *sSoundOrEventName, uint32 nFlags, const float fRadius )
{
	if (!NumberValid(fRadius))
	{
		Log(eSLT_Warning, "<Sound> %s Invalid input values (possible NaNs) on CreateSphereSound()", sSoundOrEventName);
		assert(0);
		return false;
	}

	ISound *pSound = NULL;

#if !defined(PS3) || defined(SOUNDSYSTEM_USE_FMODEX400)
	pSound = LoadSound(sSoundOrEventName, nFlags, FLAG_SOUND_PRECACHE_LOAD_SOUND);
	if (pSound)
		pSound->SetSphereSpec(fRadius);
#endif
	return pSound;
}

//////////////////////////////////////////////////////////////////////////
tSoundID CSoundSystem::CreateSoundID() // TODO change to Entity ID System
{
	tSoundID SoundID = HandleToId(m_SoundSaltBuffer.InsertDynamic());	// get entity id and mark it

	if(!SoundID)
	{
		//Log(eSLT_Error, "<Sound> Create SoundID failed, ID range is full (internal error)");
		return INVALID_SOUNDID;
	}

	m_currentSoundIDs.insert(SoundID);

	//Log(eSLT_Always, "Sound ID destroyed: %d\n", SoundID);

	return SoundID;
}

//////////////////////////////////////////////////////////////////////////
bool CSoundSystem::DestroySoundID(tSoundID SoundID)
{
	//Log(eSLT_Always, "Sound ID destroyed: %d\n", SoundID);
	m_SoundSaltBuffer.Remove(IdToHandle(SoundID));
	m_currentSoundIDs.erase(SoundID);

	return true;
}
/*
/////////////////////////////////////////////////////////////////////////
void* CSoundSystem::DSPUnit_SFXFilter_Callback(void *pOriginalBuffer, void *pNewBuffer, int nLength)
{
	GUARD_HEAP;
	// originalbuffer = crysounds original mixbuffer.
// newbuffer = the buffer passed from the previous DSP unit.
// length = length in samples at this mix time.
// param = user parameter passed through in CS_DSP_Create.
//
// modify the buffer in some fashion
	int nMixer=CS_GetMixer();
	if (nMixer!=CS_MIXER_QUALITY_FPU)
		return pNewBuffer;
	float f, q, h;
	f=2.0f*(float)sin(3.1415962*m_fDSPUnitSFXFilterCutoff/(float)CS_GetOutputRate());
	q=m_fDSPUnitSFXFilterResonance;
	float fInput;
	nLength<<=1;
	for (int i=0;i<nLength;)
	{
		// left channel
		fInput=((float*)pNewBuffer)[i];
		m_fDSPUnitSFXFilterLowLVal+=f*m_fDSPUnitSFXFilterBandLVal;
		h=fInput-m_fDSPUnitSFXFilterLowLVal-(m_fDSPUnitSFXFilterBandLVal*q);
		m_fDSPUnitSFXFilterBandLVal+=f*h;
		// writing output
		((float*)pNewBuffer)[i]=m_fDSPUnitSFXFilterLowLVal;
		// right channel
		i++;
		fInput=((float*)pNewBuffer)[i];
		m_fDSPUnitSFXFilterLowRVal+=f*m_fDSPUnitSFXFilterBandRVal;
		h=fInput-m_fDSPUnitSFXFilterLowRVal-(m_fDSPUnitSFXFilterBandRVal*q);
		m_fDSPUnitSFXFilterBandRVal+=f*h;
		// writing output
		((float*)pNewBuffer)[i]=m_fDSPUnitSFXFilterLowRVal;
		i++;
	}
	return pNewBuffer;
}

////////////////////////////////////////////////////////////////////// */
void CSoundSystem::Reset()
{
	//GUARD_HEAP;

	// reset listener vis areas - just to be 100% sure we dont use an invalid vis area
	RecomputeSoundOcclusion(false, false, true);

	// group stuff
	// reset sound-scaling
	for (int i=0; i<MAX_SOUNDSCALE_GROUPS; i++)
	{
		m_fSoundScale[i] = 1.0f;
	}

	if (m_pSoundAssetManager)
		m_pSoundAssetManager->ShutDown();

	if (m_pReverbManager)
		m_pReverbManager->Release();

	// stop all sounds
	Silence(true, false);

	// quick test, sets all Sys-Ptrs in Sounds to NULL
	//CSound* pSound = new CSound(NULL, 0, NULL);

	// for the sake of clarity...
	m_OneShotSounds.clear();
	m_LoopedSounds.clear();
	m_stoppedSoundToBeDeleted.clear();

	m_InactiveSounds.clear();
	m_nLastInactiveSoundsIndex = 0;

	m_AllSoundsArray.clear();	

	uint32 dwMaxIndex = (uint32)(m_SoundSaltBuffer.GetTSize());
	m_AllSoundsArray.resize(dwMaxIndex);

	SoundVecIter IterEnd = m_AllSoundsArray.end();
	for (SoundVecIter Iter = m_AllSoundsArray.begin(); Iter!=IterEnd; ++Iter)
		(*Iter) = NULL;

	// unload all data
	Silence(true, true);

	if (m_bOK)
		m_pAudioDevice->ShutDownDevice();

}

//////////////////////////////////////////////////////////////////////
CSoundSystem::~CSoundSystem()
{
	GUARD_HEAP;

	Reset();

	//if (m_pDSPUnitSFXFilter)
		//CS_DSP_Free(m_pDSPUnitSFXFilter);
	//m_pDSPUnitSFXFilter=NULL;

	ISoundProfileInfo *pSoundInfo = NULL;
	tmapSoundInfo::iterator ItEnd = m_SoundInfos.end();
	for (tmapSoundInfo::const_iterator It = m_SoundInfos.begin(); It!=ItEnd; ++It)
	{
		pSoundInfo = (*It).second;
		delete pSoundInfo;
	}

	m_bOK = false;	
	delete m_pAudioDevice;
	delete m_pSoundAssetManager;
	delete m_pReverbManager;
	delete m_pMoodManager;
	delete m_pDebugLogger;

	//m_VisAreas.Clear();

	int nEnd = m_UnusedSoundObjects.size();
	for (int i=0; i<nEnd; ++i)
		delete m_UnusedSoundObjects[i];

	//SetEaxListenerEnvironment(EAX_PRESET_OFF);
	
	gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);
}

//////////////////////////////////////////////////////////////////////
void CSoundSystem::Release()
{
	delete this;
}

//////////////////////////////////////////////////////////////////////////
void CSoundSystem::DeactivateSound( CSound *pSound )
{
	if (pSound->IsPlaying())
		pSound->Stop();

	if (pSound->GetState() == eSoundState_Active)
	{
		if (pSound->GetLoopMode()) 
		{
			m_LoopedSounds.EraseUnique(pSound);
			m_InactiveSounds.AddUnique(pSound);
		}
		else
		{
			// OneShot Sounds are not moved to inactive
			m_OneShotSounds.EraseUnique(pSound);
		}

		pSound->SetState(eSoundState_Inactive);
//		m_setSoundSpotsInactive.insert(pSound);

	}
}

//////////////////////////////////////////////////////////////////////////
bool CSoundSystem::ProcessActiveSound( CSound *pSound )
{
	//FUNCTION_PROFILER( GetSystem(),PROFILE_SOUND );

	bool bFadeoutFinished = false;
	float fRatio = 1.0f;

	if (pSound->GetFlags() & FLAG_SOUND_RADIUS)
	{
		float fDist2 = GetSqDistanceToClosestListener(pSound);
  
		if (fDist2 > pSound->GetSquardedMaxRadius())
			return false; // Too far

		if (bFadeoutFinished)
			return false;

		//calc attenuation, cal sound ratio between min & max radius
		fRatio = pSound->GetRatioByDistance(sqrt_tpl(fDist2));
	}
	(void)fRatio;
	// if Steve doesn't want to change frequency and pan, this can be replaced just with set volume
	//pSound->SetRatio(fRatio*cs->GetCurrentFade()*cs->GetRatio());
	return true;
}

bool CSoundSystem::ProcessReverb()
{
	assert(m_pReverbManager);
	assert (m_pMoodManager);
	
	CListener* pListener = (CListener*)GetListener(LISTENERID_STANDARD);
	
	if (!pListener)
		return false;
	
	float fDepth = pListener->GetUnderwater();
	
	if (fDepth < 0.0f)
	{
		float fMoodFade = -fDepth/25.0f + 0.6f;
		fMoodFade = min(1.0f, fMoodFade);

		if (m_pMoodManager)
		{
			m_pMoodManager->RegisterSoundMood("underwater");

			if (m_pMoodManager->GetSoundMoodFade("underwater") != fMoodFade)
				m_pMoodManager->UpdateSoundMood("underwater", fMoodFade, 100);
		}

		if (m_nReverbType)
		{
			m_pReverbManager->SetListenerReverb(REVERB_PRESET_UNDERWATER, 0, 0);
			m_pReverbManager->Update(pListener->bInside);
		}
	}
	else
	{
		// still updating?
		if (m_pMoodManager)
		{
			if (m_pMoodManager->GetSoundMoodFade("underwater") > 0.0f)
				m_pMoodManager->UpdateSoundMood("underwater", 0.0f, 100);
			
			if (m_pMoodManager->GetSoundMoodFade("underwater") == 0.0f)
				m_pMoodManager->UnregisterSoundMood("underwater");
		}

		if (m_nReverbType)
		{
			m_pReverbManager->SetListenerReverb(REVERB_PRESET_OFF, 0, 0);
			m_pReverbManager->Update(pListener->bInside);
		}
	}


	return true;
}

//////////////////////////////////////////////////////////////////////
void CSoundSystem::Update(ESoundUpdateMode UpdateMode)
{
	GUARD_HEAP;
	FUNCTION_PROFILER( gEnv->pSystem, PROFILE_SOUND );

	//++m_nMemTest;

	//if (m_nMemTest > 50)
	//	m_nMemTest = 0;

	//if (m_nMemTest == 10)
	//{
	//	ISound *pSound = LoadSound("Sounds/alien:scout:pain", 0 ,0);
	//	if (pSound)
	//	pSound->Play();
	//	
	//	pSound = LoadSound("Sounds/environment:tank:tree_break_fall_01", 0 ,0);
	//	if (pSound)
	//	pSound->Play();
	//	
	//	pSound = LoadSound("Sounds/interface:hud:target", 0 ,0);
	//	if (pSound)
	//	pSound->Play();
	//	
	//	pSound = LoadSound("Sounds/physics:mat_dirt:mat_dirt", 0 ,0);
	//	if (pSound)
	//	pSound->Play();
	//	
	//	pSound = LoadSound("Sounds/vehicles:us_ltv:gear", 0 ,0);
	//	if (pSound)
	//	pSound->Play();
	//	
	//	pSound = LoadSound("Sounds/weapons:scar:select", 0 ,0);
	//	if (pSound)
	//	pSound->Play();

	//}

	//if (m_nSpeakerConfig != m_pCVarSpeakerConfig->GetIVal())
	//{
	//	Silence();
	//	m_pSoundAssetManager->ShutDown();
	//	m_nSpeakerConfig = m_pCVarSpeakerConfig->GetIVal();
	//	ptParamINT32 speakertype(m_nSpeakerConfig);
	//	m_pAudioDevice->SetParam(ADP_SPEAKER_MODE, &speakertype);
	//}
	
	//if (!m_pNetworkSound)
	//{
	//	m_pData = (uint32*) new char[1000];
	//	memset(m_pData, 1, 256);
	//	m_pNetworkSound = CreateNetworkSound((INetworkSoundListener*)this, 8, 8000, 256, 1);
	//	m_pNetworkSound->SetFlags(m_pNetworkSound->GetFlags() | FLAG_SOUND_RADIUS);
	//	m_pNetworkSound->SetMinMaxDistance(0.1f, 100.0f);
	//	m_pMicrophone = CreateMicrophone((IMicrophoneStream*)this, 0, 8, 8000, 256);
	//	m_pMicrophone->Record(0, 8, 8000, 256);
	//	m_pNetworkSound->Play();
	//}


	//ProcessHDRS(); 

	if (UpdateMode & eSoundUpdateMode_Rest)
	{
		if (m_pMoodManager && g_nSoundMoods)
			m_pMoodManager->Update();

		// check if the soundsystem is currently fading out
		if (m_nFadeOutInMS > 0)
		{
			int nDifferenceMS = (int) (gEnv->pTimer->GetRealFrameTime() * 1000);
			float fTimeRatio = 1.0f - (min(nDifferenceMS, (int)m_nFadeOutInMS)) / float(m_nFadeOutInMS);
			
			nDifferenceMS = m_nFadeOutInMS - nDifferenceMS;
			m_nFadeOutInMS = max(0, nDifferenceMS);

			g_fGameDialogVolume *= fTimeRatio;
			g_fGameSFXVolume *= fTimeRatio;


			// we reached the fade out goal. Pause all sounds if SoundSystem is supposed to be paused
			if (m_bSoundSystemPause && m_nFadeOutInMS == 0)
			{
				Pause(true, false, 0);
				g_fGameDialogVolume = 1.0f;
				g_fGameSFXVolume = 1.0f;
			}
		}



		// check if Music volume has changed about a delta
		if ((Ffabs(g_fMusicVolume - m_fMusicVolume) > 0.01f) || (Ffabs(g_fGameMusicVolume - m_fGameMusicVolume) > 0.01f))
		{
			// Setting MIN or MAX values to the Music volume
			g_fMusicVolume = clamp(g_fMusicVolume, 0.0f, 1.0f);
			m_fMusicVolume = g_fMusicVolume;// * m_fMusicEffectVolume;

			g_fGameMusicVolume = clamp(g_fGameMusicVolume, 0.0f, 1.0f);
			m_fGameMusicVolume = g_fGameMusicVolume;
		}

		// check if SFX volume has changed about a delta
		if ( (Ffabs(g_fSFXVolume - m_fSFXVolume) > 0.01f) 
			|| (Ffabs(g_fGameSFXVolume - m_fGameSFXVolume) > 0.01f) 
			|| (Ffabs(g_fGameDialogVolume - m_fGameDialogVolume) > 0.01f)
			|| (Ffabs(g_fDialogVolume - m_fDialogVolume) > 0.01f))
		{
			// Setting MIN or MAX values to the SFX volume
			g_fSFXVolume = clamp(g_fSFXVolume, 0.0f, 1.0f);
			m_fSFXVolume = g_fSFXVolume;		

			g_fGameSFXVolume = clamp(g_fGameSFXVolume, 0.0f, 1.0f);
			m_fGameSFXVolume = g_fGameSFXVolume;		

			g_fGameDialogVolume = clamp(g_fGameDialogVolume, 0.0f, 1.0f);
			m_fGameDialogVolume = g_fGameDialogVolume;	

			g_fDialogVolume = clamp(g_fDialogVolume, 0.0f, 1.0f);
			m_fDialogVolume = g_fDialogVolume;	

			// call set volume for each sound so it can rescale itself		
			// TODO optimize only go through set of registered IDs
			TSoundIDSet::const_iterator ItEnd = m_currentSoundIDs.end();
			for (TSoundIDSet::const_iterator It=m_currentSoundIDs.begin(); It!=ItEnd; ++It)
			{			
				CSound* pSound = (CSound*)GetSound(*It);

				if (pSound)
				{
					if (pSound->GetFlags() & FLAG_SOUND_MUSIC)			
						continue;	// this one is a music sound, so skip it		
					pSound->SetVolume(pSound->GetVolume());
					//pSound->SetFrequency(m_nSampleRate); // new line
				}
			} // for loop
		}


		// Check for ReverbType change in console
#ifdef SOUNDSYSTEM_USE_FMODEX400
		if (g_nReverbType != m_nReverbType)
		{
			if (m_pReverbManager)
			{
				// switch from EAX to DSP
				if ( (m_nReverbType == REVERB_TYPE_HARDWARE || m_nReverbType == REVERB_TYPE_SOFTWARE_LOW || m_nReverbType == REVERB_TYPE_SOFTWARE_HIGH || m_nReverbType == REVERB_TYPE_NONE)
					&& (g_nReverbType != REVERB_TYPE_HARDWARE && g_nReverbType != REVERB_TYPE_SOFTWARE_LOW && g_nReverbType != REVERB_TYPE_SOFTWARE_HIGH && g_nReverbType != REVERB_TYPE_NONE))
				{
					m_pReverbManager->Release();
					delete m_pReverbManager;
					m_pReverbManager = new CReverbManagerDSP();
					m_pReverbManager->Init(m_pAudioDevice, g_nReverbInstances, this);
					m_pSoundAssetManager->UnloadAllSoundBuffers();
				}

				// switch from DSP to EAX
				if ( (m_nReverbType != REVERB_TYPE_HARDWARE && m_nReverbType != REVERB_TYPE_SOFTWARE_LOW && m_nReverbType != REVERB_TYPE_SOFTWARE_HIGH && m_nReverbType != REVERB_TYPE_NONE) 
					&& (g_nReverbType == REVERB_TYPE_HARDWARE || g_nReverbType == REVERB_TYPE_SOFTWARE_LOW || g_nReverbType == REVERB_TYPE_SOFTWARE_HIGH || g_nReverbType == REVERB_TYPE_NONE))
				{
					m_pReverbManager->Release();
					delete m_pReverbManager;
					m_pReverbManager = new CReverbManagerEAX();
					m_pReverbManager->Init(m_pAudioDevice, g_nReverbInstances, this);
					m_pSoundAssetManager->UnloadAllSoundBuffers();
				}

				// Set the current ReverbType
				m_nReverbType = g_nReverbType;
				m_pReverbManager->SelectReverb(m_nReverbType);
			}
		}
#endif

		m_SpamFilter.Update(gEnv->pTimer->GetFrameTime());
	}

	if (UpdateMode & eSoundUpdateMode_Listeners)
	{
		// Set listeners first
		m_pAudioDevice->UpdateListeners();
	}

	// if sound is disabled in the console always calls Silence()
	if (g_nSoundEnable != 0)
		m_bSoundSystemEnabled = true;
	else
	{
		if (m_bSoundSystemEnabled)
		{
			Silence(true, true);
			if (m_pMoodManager)
				m_pMoodManager->RefreshCategories();
			m_bSoundSystemEnabled = false;
		}
		else
		{
			// Draw Audio Information to the screen
			RenderAudioInformation();
			return;
		}
	}

	if (UpdateMode & eSoundUpdateMode_Rest)
	{

		// checking Indoor/Outdoor and VisAreas
		//bool bWasInside = m_bInside;
		// reset listener vis areas - just to be 100% sure we dont use an invalid vis area
		RecomputeSoundOcclusion(true, false);
		tVecListenersIter IterEnd = m_vecListeners.end();
		for (tVecListenersIter Iter = m_vecListeners.begin(); Iter != IterEnd; ++Iter)
		{
			CListener* pListener = (*Iter);
			pListener->bInside = false;
			pListener->bInside = (pListener->GetVisArea() != NULL);
		}

		// check if sample rate has changed
		m_nSampleRate = g_nFormatSampleRate;

		// TODO Reactivate
		// Do the reverb calculation, blending of EAX areas
		CTimeValue const tTimeDif = gEnv->pTimer->GetFrameStartTime() - m_tUpdateReverbManager;
		float const fMS	= tTimeDif.GetMilliSeconds();
		if (abs(fMS) > UPDATE_SOUND_REVERB_MANAGER_IN_MS)
		{
			ProcessReverb();
			m_tUpdateReverbManager = gEnv->pTimer->GetFrameStartTime();
		}
	}
 
	//////////////////////////////////////////////////////////////////////////
	// calling Update() for all active sound spots
	CTimeValue tTimeDif = gEnv->pTimer->GetFrameStartTime() - m_tUpdateSounds;

	//if (tTimeDif.GetMilliSeconds() > UPDATE_SOUNDS_IN_MS) // UPDATE_SOUNDS_IN_MS
	if (UpdateMode & eSoundUpdateMode_Sounds || UpdateMode & eSoundUpdateMode_LostFocus)
	{
		// if there is no mission hint play fade it out
		// sound updates will overwrite this sound mood

		ProcessInactiveSounds();
		ProcessOneShotSounds();
		ProcessLoopedSounds();
		m_tUpdateSounds = gEnv->pTimer->GetFrameStartTime();

		// Here actually delete all auto stop sounds.
		if (!m_stoppedSoundToBeDeleted.empty())
		{
			//gEnv->pLog->Log("DEBUG: ToBeDeleted gets cleared: %d\n", m_stoppedSoundToBeDeleted.size());
			m_stoppedSoundToBeDeleted.clear();
		}

		m_bNeedSoundZoomUpdate = false;
	}

	if (UpdateMode & eSoundUpdateMode_Rest)
	{
		// Update the AssetManager
		tTimeDif = gEnv->pTimer->GetFrameStartTime() - m_tUpdateSoundAssetManager;
		float fMS	= tTimeDif.GetMilliSeconds();
		if (abs(fMS) > UPDATE_SOUND_ASSET_MANAGER_IN_MS)
		{
			m_pSoundAssetManager->Update();
			m_tUpdateSoundAssetManager = gEnv->pTimer->GetFrameStartTime();
		}

		//////////////////////////////////////////////////////////////////////////
		// Update the AudioDevice
		tTimeDif = gEnv->pTimer->GetFrameStartTime() - m_tUpdateAudioDevice;
		fMS = tTimeDif.GetMilliSeconds();
		//gEnv->pLog->LogWarning("<Sound> WARNING - Update every %f ms",nMS);
		if (abs(fMS) > 20 || fMS == 0.0f) // game is paused, no time passed
		{
			m_pAudioDevice->Update();
			m_tUpdateAudioDevice = gEnv->pTimer->GetFrameStartTime();
		}
		else
		{
			int a = 0;	(void)a;
		}
		// Draw Audio Information to the screen
		RenderAudioInformation();
	}

	if (g_nProfiling)
	{
		uint32 nIDsinUse = 0;
		SoundVecIter ItEnd = m_AllSoundsArray.end();
		for (SoundVecIter It=m_AllSoundsArray.begin(); It!=ItEnd; ++It)
		{			
			CSound *pSound = (*It);

			if (pSound)
				++nIDsinUse;
		}

		m_nMaxUsedIDs = max(m_nMaxUsedIDs, nIDsinUse);
	}

	//if (m_nMemTest == 0)
	//	m_pSoundAssetManager->UnloadAllSoundBuffers();
		//m_pAudioDevice->ResetAudio();

}

//////////////////////////////////////////////////////////////////////////
bool CSoundSystem::IsSoundPH(CSound *pSound)
{
	//GUARD_HEAP;
	// check if indoor only or outdoor only 
	//if (pSound->GetFlags() & FLAG_SOUND_OUTDOOR)
	//{			
	//	return (!m_bInside); // fade out or fade in
	//}
	
	//if (pSound->GetFlags() & FLAG_SOUND_INDOOR)		
	//{	
	//	return(m_bInside); // fade out or fade in
	//}

	bool bResult = false;

		// at this point check if should use sound occlusion at all
		if (!(pSound->GetFlags() & FLAG_SOUND_CULLING))
			return (true); // always hearable, no occlusion - proceed with fade in
	
		I3DEngine* p3dEngine = gEnv->p3DEngine;
		IVisArea *pSoundArea = NULL;

		if (p3dEngine)	
		{
			pSoundArea = p3dEngine->GetVisAreaFromPos(pSound->GetPlaybackPosition());	
			pSound->SetSoundVisArea(pSoundArea);
		}

	// test for all listeners
	CListener* pListener = NULL;
	tVecListenersIter IterEnd = m_vecListeners.end();
	for (tVecListenersIter Iter = m_vecListeners.begin(); Iter != IterEnd; ++Iter)
	{
		pListener = (*Iter);

		if (pListener->bActive)
		{
			// if listener and sound are in the same VisArea (or both in none) then its hearable
			if (pListener->GetVisArea() == pSoundArea)
				return (true);

			// Listener outside, Sound inside
			if (!pListener->GetVisArea() && pSoundArea)
			{
				if (pSoundArea->IsConnectedToOutdoor())
					return (true);

				// test more VisAreas around the SoundArea
				m_SoundTempVisAreas.Clear();
				int nMaxDepth = 3;
				pSoundArea->FindSurroundingVisArea( nMaxDepth, true, &m_SoundTempVisAreas, MAX_VIS_AREAS);

				for(int i=0; i<m_SoundTempVisAreas.Count(); i++)
					if (IVisArea * pArea = m_SoundTempVisAreas.GetAt(i))
						if (pArea->IsConnectedToOutdoor())
							return (true);
			}

			//	// if the listener is not inside a vis area but the sound is,
			//	// then the sound is not hearable 
			//	if (pSound->GetSoundVisArea())
			//		return (false); // fade out

			//	return (true); // both listener and sound in outdoor - fade in

			// Listener inside, Sound outside
			if (pListener->GetVisArea() && !pSoundArea)
			{
				if (pListener->GetVisArea()->IsConnectedToOutdoor())
					return true;

				// test all VisAreas in the listeners cache
				for(int i=0; i<pListener->pVisAreas->Count(); i++)
					if (IVisArea * pArea = pListener->pVisAreas->GetAt(i))
						if (pArea->IsConnectedToOutdoor())
							return (true);
			}

			// both inside, but not in cache of VisAreas
			if (pListener->pVisAreas->Find(pSound->GetSoundVisArea()) > 0)
				return true;

		}

		// now check that the area where the sound is in, isn't occluded by doors, 
		// objects or buildings	
		//int numToCheck = m_pCVarVisAreaProp->GetIVal();
		//for (int k=0; k < numToCheck; k++)
		//{
		//	IVisArea *pArea=m_VisAreas[k];
		//	if (!pArea)
		//		break;
		//	if (pArea==pSound->GetSoundVisArea())
		//		return (true); // both listener and sound in indoor, and in a not-occluded area - fade in
		//} //k

		//return (false); //fade out

	}
			
	return bResult;
}

//////////////////////////////////////////////////////////////////////////
IMusicSystem* CSoundSystem::CreateMusicSystem()
{
	IMusicSystem *pMusicSystem = NULL;
	
	pMusicSystem = new CMusicSystem(m_pAudioDevice);

	return pMusicSystem;
}

//////////////////////////////////////////////////////////////////////////
void CSoundSystem::AddToSoundSystem(CSound *pSound)
{
	//GUARD_HEAP;
	
	if (pSound->GetFlags() & FLAG_SOUND_UNSCALABLE)
	{
		pSound->RemoveFromScaleGroup(SOUNDSCALE_SCALEABLE);
		pSound->RemoveFromScaleGroup(SOUNDSCALE_MISSIONHINT);
	}

	m_AllSoundsArray[IdToHandle(pSound->GetId()).GetIndex()] = pSound;

	// All Sounds go to the Inactive set
	m_InactiveSounds.AddUnique(pSound);

	pSound->SetState(eSoundState_Inactive);

}

// remove this later if we dont need it anymore

////////////////////////////////////////////////////////////////////////////
//void CSoundSystem::RegisterOneShotSound( CSound *pSound )
//{
//	m_OneShotSounds.insert( pSound );
//}
//
////////////////////////////////////////////////////////////////////////////
//void CSoundSystem::UnregisterOneShotSound( CSound *pSound )
//{
//	m_stoppedSoundToBeDeleted.insert( pSound );
//	m_OneShotSounds.erase( pSound );
//}

//////////////////////////////////////////////////////////////////////////
void CSoundSystem::SetSoundActiveState(ISound *pSound, ESoundActiveState State)	// take care from where you call this function, because it may cause crashes if you change the state while the system is iterating through the lists (Update())
{
	//GUARD_HEAP;

	CRY_ASSERT(pSound);
	if(!pSound)
	{
		Log(eSLT_Error, "<Sound> SetSoundActiveState called with a null sound");
		return;
	}

	CSound *pCSound = (CSound*) pSound;

	if (pCSound->GetState() == State)
		return;

	_smart_ptr<CSound> pTempSmart = pCSound;

	switch (pCSound->GetState())
	{
		case eSoundState_Active:
		{
			if (pCSound->GetLoopMode())
			{
				m_LoopedSounds.EraseUnique(pCSound);
			}
			else
			{
				m_OneShotSounds.EraseUnique(pCSound);
			}
			pCSound->Deactivate();
			break;
		}
		case eSoundState_Inactive:
		{
			m_InactiveSounds.EraseUnique(pCSound);
			break;
		}		
	}

	pCSound->SetState(State);

	switch (pCSound->GetState())
	{
		case eSoundState_Active:
		{
			if (pCSound->GetLoopMode())
				m_LoopedSounds.AddUnique(pCSound);
			else
				m_OneShotSounds.AddUnique(pCSound);

			break;
		}
		case eSoundState_Inactive:
		{
			if (pCSound->GetLoopMode())
			{
				m_InactiveSounds.AddUnique(pCSound);
			}
			else
			{
				m_stoppedSoundToBeDeleted.AddUnique(pCSound);
			}

			break;
		}
		case eSoundState_None:
			{
				break;
			}
	}
}

void CSoundSystem::SetMasterPitch(float fPitch)
{
	if (m_pAudioDevice)
	{
		ptParamF32 fParam(fPitch);
		m_pAudioDevice->SetParam(adpPITCH, &fParam);
	}
}

//! Converts dialog file name syntax into dialog key
const char* CSoundSystem::MakeDialogKey(const char* sDialogFileName)
{
	g_sFileName = sDialogFileName;
	g_sKeyName = "";
	int nReturnFlags = ParseGroupString(g_sFileName, g_sProjectPath, g_sProjectName, g_sGroupName, g_sEventName, g_sKeyName);

	return g_sKeyName.c_str();
}

// takes in string = "group name : sound name" we make 2 strings from one
int CSoundSystem::ParseGroupString(TFixedResourceName &sGroupAndEventName, TFixedResourceName &sProjectPath, TFixedResourceName &sProjectName, TFixedResourceName &sGroupName, TFixedResourceName &sEventName, TFixedResourceName &sKeyName)
{
	FUNCTION_PROFILER( gEnv->pSystem, PROFILE_SOUND );

	const char *pInput = sGroupAndEventName;

	sProjectPath = "";
	sProjectName = "";
	sGroupName = "";
	sEventName = "";
	sKeyName = "";

	// Format:
	// (   sProjectPath                   ) 
	// xxx/xxx/xxx/xxx / sProjectName : sGroupName : sEventName

	const char *pPoint  = strstr(pInput,".");
	const char *pColon1 = strstr(pInput,":");

	if (!pPoint && !pColon1)
	{
		// This is a sound key
		sKeyName = sGroupAndEventName;

		sProjectName = "dialog";
		sProjectPath = "languages/dialog/";
		sGroupAndEventName = sProjectPath;
		sGroupAndEventName += sKeyName;
		sGroupAndEventName += ".wav";

		return PSNRC_KEY|PSNRC_CONVERTEDTOWAV|PSNRC_VOICE;
	}

	if (pPoint && !pColon1)
	{
		// This is already a wave file, so lets create the keyname
		int nPosLastSlash = sGroupAndEventName.rfind('/', sGroupAndEventName.size());
		int nPos2ndLastSlash = sGroupAndEventName.rfind('/', nPosLastSlash-1);
		
		sKeyName.assign(sGroupAndEventName.substr(nPos2ndLastSlash+1, sGroupAndEventName.size()-nPos2ndLastSlash-5));

		return PSNRC_WAV;
	}

	// assign sProjectPath
	sProjectPath.assign(pInput, pColon1-pInput);
	sProjectPath += "/";
	sProjectPath.MakeLower();

	const char *pEnd = pColon1;		

	// find end
	while(*pEnd)
		++pEnd;		

	for(const char *pLastSlash = pColon1; pLastSlash>=pInput; --pLastSlash)
	{
		if(*pLastSlash=='\\' || *pLastSlash=='/')
		{
			// assign sProjectName
			sProjectName.assign(pLastSlash+1, pColon1-pLastSlash-1);
			sProjectName.MakeLower();
			break;
		}
		if (pLastSlash == pInput)
		{
			// no slash found, so project name and path are same
   		sProjectName.assign(pInput, pColon1-pInput);
			sProjectName.MakeLower();
		}
	}		

	
	const char *pColon2 = strrchr(pColon1+1,':');

	if(!pColon2)  // no 2nd : -> no eventname, everything behind the : is the groupname
	{
		// assign sGroupName
		sGroupName.assign(pColon1+1, pEnd-pColon1-1);
		sGroupName.MakeLower();

		return PSNRC_GROUP;
	}
	else
	{
		// assign sGroupName
		sGroupName.assign(pColon1+1, pColon2-pColon1-1);
		sGroupName.MakeLower();
	}

	// invalid event name?
	if (pEnd-pColon2-1 == 0)
		return 0;

	// assign sEventName
	assert (pEnd-pColon2-1 > 0);
	sEventName.assign(pColon2+1, pEnd-pColon2-1);
	sEventName.MakeLower();
	assert (!sEventName.empty());

	if (g_nLanguagesConversion)
	{
		if (sProjectName == "languages")
		{
			// convert legacy languages events to direct wav-files
			Log(eSLT_Warning, "<Sound> Trying to play an old languages sound event! Please fix this using the WAV file and a Dialog entity for play back. Sound: (%s)",sGroupAndEventName.c_str());
			
			sGroupAndEventName = sProjectName;
			sGroupAndEventName += "/dialog/";
			sGroupAndEventName += sGroupName;
			sGroupAndEventName += "/";
			sGroupAndEventName += sEventName;
			sGroupAndEventName += ".wav";
			
			sEventName = "";
			Log(eSLT_Warning, "<Sound> Use this file location instead: %s",sGroupAndEventName.c_str());
			return PSNRC_CONVERTEDTOWAV| PSNRC_VOICE;
		}
	}

	return PSNRC_EVENT;
}

//////////////////////////////////////////////////////////////////////
ISound* CSoundSystem::LoadSound(const char *szFile, uint32 nFlags, uint32 nPrecacheFlags)
{
	//GUARD_HEAP;
	FUNCTION_PROFILER( gEnv->pSystem, PROFILE_SOUND );

	if (!m_bSoundSystemEnabled)
		return NULL;

	//if (flags & FLAG_SOUND_3D)	// make all 3d sounds use the sw-attenuation
		//flags|=FLAG_SOUND_RADIUS;


	bool bOldValue = m_bDelayPrecaching;
	m_bDelayPrecaching = false;
	CSoundBuffer *pBuf = (CSoundBuffer*) PrecacheSound(szFile, nFlags, nPrecacheFlags);
	m_bDelayPrecaching = bOldValue;

	if (!pBuf)
	{
		//m_DebugLogger.LogString(sTemp + szFile);

		// if project ready, then the sound most likely is unknown
		if (!(m_ErrorCode & eSoundSystemErrorCode_ProjectNotReady))
		{
			m_ErrorCode = (ESoundSystemErrorCode) (m_ErrorCode | eSoundSystemErrorCode_SoundNotKnown);
			Log(eSLT_Error, "<Sound> Create sound %s failed - Invalid Buffer! Does file exist?", szFile);
		}

		return NULL;
	}

	//GUARD_HEAP;
	//create a new instance	
	CSound* pSound = GetNewSoundObject();

	//GUARD_HEAP;
	 
	if (!pSound)
	{
		Log(eSLT_Warning, "<Sound> Create sound %s failed! No free sound availible.", szFile);
		m_ErrorCode = (ESoundSystemErrorCode) (m_ErrorCode | eSoundSystemErrorCode_SoundCreateFailed);
		return NULL; // something wrong
	}

	if (pSound->GetId() == INVALID_SOUNDID)
	{
		Log(eSLT_Warning, "<Sound> Create sound %s failed! No SoundID left.", szFile);
		m_ErrorCode = (ESoundSystemErrorCode) (m_ErrorCode | eSoundSystemErrorCode_SoundIDRangeFull);
		return NULL; // bad soundid, salt array full?!
	}

	// make this sound safe within that function
	pSound->AddRef();
	pSound->SetSoundBufferPtr(pBuf);		

	g_sFileName = szFile;
	int nReturnFlags = ParseGroupString(g_sFileName, g_sProjectPath, g_sProjectName, g_sGroupName, g_sEventName, g_sKeyName);

	//if (nReturnFlags == PSNRC_GROUP)
	//{
	//	// The provided sound name was only a group, spawning an event is not possible
	//	pSound->Release();
	//	Log(eSLT_Warning, "<Sound> Create sound %s failed! Only a group name was provided.", szFile);
	//	return NULL; // something wrong
	//}

	TFixedResourceName sLocalEventName = g_sEventName;

	// Catch warning
	//if (pBuf->GetProps()->eBufferType == btSAMPLE && !(nFlags & FLAG_SOUND_VOICE))
	//{
	//	Log(eSLT_Warning, "<Sound> Sample sound %s not marked as Voice!", szFile);
	//}

	// remove the voice flags on event sounds
	if (pBuf->GetProps()->eBufferType == btEVENT && (nFlags & FLAG_SOUND_VOICE))
	{
		nFlags &= ~FLAG_SOUND_VOICE;
		int nEventHasVoiceFlag = 0;
		//assert(nEventHasVoiceFlag);
	}

	if (pBuf->GetProps()->eBufferType == btSAMPLE && (nFlags & FLAG_SOUND_VOICE) && (nReturnFlags & (PSNRC_WAV|PSNRC_KEY|PSNRC_VOICE)) )
	{
		SLocalizedSoundInfoGame* pSoundInfo = new SLocalizedSoundInfoGame;
		memset(pSoundInfo, 0, sizeof(SLocalizedSoundInfoGame));

		if (!pSoundInfo)
			return false;

		bool bError = false;
		bool bSearchAgain = false;

		// first guess on allocs
		pSoundInfo->nNumSoundMoods = m_nDialogSoundMoodAlloc;
		pSoundInfo->nNumEventParameters = m_nDialogEventParameterAlloc;
		SLocalizedAdvancesSoundEntry* pSoundMoodAlloc = NULL;
		SLocalizedAdvancesSoundEntry* pEventParameterAlloc = NULL;

		if (m_nDialogSoundMoodAlloc > 0)
		{
			pSoundMoodAlloc = new SLocalizedAdvancesSoundEntry[m_nDialogSoundMoodAlloc];
			pSoundInfo->pSoundMoods = pSoundMoodAlloc;
		}

		if (m_nDialogEventParameterAlloc > 0)
		{
			pEventParameterAlloc = new SLocalizedAdvancesSoundEntry[m_nDialogEventParameterAlloc];
			pSoundInfo->pEventParameters = pEventParameterAlloc;
		}
		
		bError = gEnv->pSystem->GetLocalizationManager()->GetLocalizedInfoByKey( g_sKeyName.c_str(), pSoundInfo);
 		
		if (!bError)
		{
			// more SoundMoods than expected, lets increase the buffer
			if (pSoundInfo->nNumSoundMoods > m_nDialogSoundMoodAlloc)
			{
				if (pSoundMoodAlloc)
					delete[] pSoundMoodAlloc;

				m_nDialogSoundMoodAlloc = pSoundInfo->nNumSoundMoods;
				pSoundMoodAlloc = new SLocalizedAdvancesSoundEntry[m_nDialogSoundMoodAlloc];
				pSoundInfo->pSoundMoods = pSoundMoodAlloc;
				bSearchAgain = true;
			}

			// more EventParameters than expected, lets increase the buffer
			if (pSoundInfo->nNumEventParameters > m_nDialogEventParameterAlloc)
			{
				if (pEventParameterAlloc)
					delete[] pEventParameterAlloc;

				m_nDialogEventParameterAlloc = pSoundInfo->nNumEventParameters;
				pEventParameterAlloc = new SLocalizedAdvancesSoundEntry[m_nDialogEventParameterAlloc];
				pSoundInfo->pEventParameters = pEventParameterAlloc;
				bSearchAgain = true;
			}

			if (bSearchAgain)
				bError = gEnv->pSystem->GetLocalizationManager()->GetLocalizedInfoByKey( g_sKeyName.c_str(), pSoundInfo);
			
		}


		if (bError)
		{
			pSound->SetVoiceSpecs(pSoundInfo);
			sLocalEventName = pSoundInfo->sSoundEvent; 

			if ( sLocalEventName.empty() ) // no eventname found, but this is still a voice line, so lets choose clean
				sLocalEventName = "clean";
		}
		else
		{
			nFlags &= ~FLAG_SOUND_VOICE; // voice files have to be in the localization manager
			Log(eSLT_Warning, "<Sound> Sound key %s not found in Localization Manager!", g_sKeyName.c_str());
		}
	}
	
	if (!sLocalEventName.empty() || (nReturnFlags & PSNRC_GROUP))
		nFlags |= FLAG_SOUND_EVENT;
	else
		nFlags &= ~FLAG_SOUND_EVENT;

	CSoundBuffer *pInfoBuf = NULL;

	if (pBuf->GetProps()->eBufferType == btSAMPLE && (nFlags & FLAG_SOUND_EVENT))
	{
		if ( sLocalEventName.empty() )
			sLocalEventName = "clean";

		TFixedResourceName sTemp = "sounds/dialog:dialog:" + sLocalEventName;

		pInfoBuf = (CSoundBuffer*) PrecacheSound(sTemp.c_str(), nFlags, FLAG_SOUND_PRECACHE_LOAD_SOUND);
		
		if (!pInfoBuf)
		{
			Log(eSLT_Error, "<Sound> Cannot precache sound: %s", szFile);
			m_ErrorCode = (ESoundSystemErrorCode) (m_ErrorCode | eSoundSystemErrorCode_SoundNotKnown);
			return NULL;
		}

		pSound->m_pEventGroupBuffer = pInfoBuf;
		pSound->m_pEventGroupBuffer->AddToLoadReqList(pSound);
	}
	else
	{
		pInfoBuf = pBuf;
	}

	// Now create the platform sound
	IPlatformSound* pPlatformSound	= m_pAudioDevice->CreatePlatformSound(pSound, sLocalEventName.c_str());

	if (!pPlatformSound)
	{
		pSound->Release();
		delete pPlatformSound;
		m_ErrorCode = (ESoundSystemErrorCode) (m_ErrorCode | eSoundSystemErrorCode_SoundNotKnown);
		return NULL; // something wrong
	}

	pSound->SetPlatformSoundPtr(pPlatformSound);

	// support allow unload after the sound got played
	if (pBuf->GetProps()->nPrecacheFlags & FLAG_SOUND_PRECACHE_UNLOAD_AFTER_PLAY)
	{
		pSound->AddEventListener(pBuf, "SoundBuffer");
	}

	ESoundSystemErrorCode eResult = pSound->Initialize(pInfoBuf, nFlags);

	if (eResult != eSoundSystemErrorCode_None)
	{
		m_ErrorCode = (ESoundSystemErrorCode) (m_ErrorCode | eResult);

		if (eResult == eSoundSystemErrorCode_SoundNotKnown)
		{
			pSound->Release();
			return NULL;
		}
	}

	//Log(eSLT_Message, "<Sound> %s loaded, sound size=%d",szFile,m_setSounds.size());

	AddToSoundSystem(pSound);
	// now the sound is referenced somewhere else so revert the addRef from above
	pSound->Release();

	if (g_nDebugSound == SOUNDSYSTEM_DEBUG_SIMPLE)
	{
		// if there is a playback filter set, then we dont want to spam the console
		// sadly on creating the sound the eSemantic on the sound is not yet availible
		// so we block all logging
		if (!g_nPlaybackFilter)
			Log(eSLT_Message, "<Sound> Create Sound %s, ID:%d", pSound->GetName(), pSound->GetId());
	}

	return (pSound);
}

//////////////////////////////////////////////////////////////////////
void CSoundSystem::RemoveSound(tSoundID nSoundID)
{
	ISound *pSound = GetSound(nSoundID);
	RemoveReference((CSound*)pSound);
}


//////////////////////////////////////////////////////////////////////////
void	CSoundSystem::RecomputeSoundOcclusion(bool bRecomputeListener, bool bForceRecompute, bool bReset)
{
	FUNCTION_PROFILER( gEnv->pSystem, PROFILE_SOUND );

	if (bReset)
	{
		tVecListenersIter IterEnd = m_vecListeners.end();
		for (tVecListenersIter Iter = m_vecListeners.begin(); Iter != IterEnd; ++Iter)
		{
			CListener* pListener = (*Iter);
			pListener->SetVisArea(NULL);
		}

		SoundVecIter ItEnd = m_AllSoundsArray.end();
		for (SoundVecIter It=m_AllSoundsArray.begin(); It!=ItEnd; ++It)
		{			
			CSound *pSound = (*It);

			if (pSound)
			{
				pSound->InvalidateVisArea();
			}
		}

		return;
	}

	if (m_nMuteRefCnt > 0)
		return; // don't do this during mute (usually during loading)

	tVecListenersIter IterEnd = m_vecListeners.end();
	for (tVecListenersIter Iter = m_vecListeners.begin(); Iter != IterEnd; ++Iter)
	{
		CListener* pListener = (*Iter);

		I3DEngine	*p3dEngine = gEnv->p3DEngine;
		if (p3dEngine)	
		{			
			//check where the listener is 	
			IVisArea *pCurrArea = NULL;

			if (bRecomputeListener)
			{
				//IListener* pListener = GetListener(LISTENERID_STANDARD);
				//if (!pListener)
				//	return;

				Vec3 vPos = pListener->GetPosition();
				pCurrArea = p3dEngine->GetVisAreaFromPos(vPos);
			}
			else
				pCurrArea = pListener->GetVisArea();

			// avoid silly people to mess around with too many visareas
			int nMaxVis = (g_nVisAreaPropagation & (MAX_VIS_AREAS-1));

			if ((pCurrArea != pListener->GetVisArea()) || (bForceRecompute))
			{
				// listener has changed sector or has moved from indoor to outdoor or viceversa
				pListener->SetVisArea(pCurrArea);
				pListener->pVisAreas->Clear();

				//memset(m_VisAreas,0,sizeof(m_VisAreas));

				if (pListener->GetVisArea())
				{
					//PodArray<IVisArea*> pAreas;
					pListener->GetVisArea()->FindSurroundingVisArea( nMaxVis, true, pListener->pVisAreas, MAX_VIS_AREAS);
					//m_VisAreas.Add(m_pVisArea);


					// if inside let's get a list of visible areas to use for sound occlusion
					//int nAreas=m_pVisArea->GetVisAreaConnections(m_VisAreas,nMaxVis,true);

					// engine doesn't return the current area - be sure to add it or
					// all the sounds currently playing in the listener area will fade out
					//if (nAreas<nMaxVis)
					//m_VisAreas[nAreas]=m_pVisArea;
					//else
					// override the first one - this one is more important
					//m_VisAreas[0]=m_pVisArea;
				}

			}			
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CSoundSystem::RemoveReference(CSound *pSound)
{
	//GUARD_HEAP;

	if (pSound) 
	{
		tSoundID nSoundID = pSound->GetId();
		if(m_SoundSaltBuffer.IsUsed(IdToHandle(nSoundID).GetIndex()))		
		{
			m_AllSoundsArray[IdToHandle(nSoundID).GetIndex()] = NULL;
			DestroySoundID(nSoundID);
		}
	}
}

//////////////////////////////////////////////////////////////////////
ISound* CSoundSystem::GetSound(tSoundID nSoundID) const
{
	if(m_SoundSaltBuffer.IsValid(IdToHandle(nSoundID)))
		return m_AllSoundsArray[IdToHandle(nSoundID).GetIndex()];
	else
		return NULL;
}

//////////////////////////////////////////////////////////////////////
//void CSoundSystem::PlaySound(tSoundID nSoundID)
//{
//	//GUARD_HEAP;
//	CSound *pSound = (CSound *)GetSound(nSoundID);
//	if (pSound)	
//		pSound->Play();	
//}

void	CSoundSystem::GetOutputHandle( void **pHandle, void **pHandle2, EOutputHandle *HandleType) const
{
	if (m_pAudioDevice)
		m_pAudioDevice->GetOutputHandle(pHandle, pHandle2, HandleType);
	else
	{
		*pHandle = NULL;
		*pHandle2 = NULL;
		*HandleType = eOUTPUT_MAX;
	}
}

//////////////////////////////////////////////////////////////////////
bool CSoundSystem::SetListener(const ListenerID nListenerID, const Matrix34 &matTransformation, const Vec3 &vVel, bool bActive, float fRecordLevel)
{
	if (!matTransformation.IsValid() || !vVel.IsValid() || !NumberValid(fRecordLevel))
	{
		Log(eSLT_Warning, "<Sound> Invalid input values (possible NaNs) on SetListener()");
		assert(0);
		return false;
	}

	bool bResult = false;
	Vec3 vOldPos;
	tVecListenersIter Iter = m_vecListeners.begin();
	tVecListenersIter IterEnd = m_vecListeners.end();
	CListener* pListener = NULL;
  
	for (Iter = m_vecListeners.begin(); Iter != IterEnd; ++Iter)
	{
		pListener = (*Iter);

		if (pListener->GetID() == nListenerID)
		{
			vOldPos = pListener->GetPosition();
			pListener->SetMatrix(matTransformation);
			pListener->bActive = bActive;
			pListener->fRecordLevel = fRecordLevel;
		
			bResult = true;
			break;
		}
	}

	if (!bResult)
		return false;

	Vec3 vPos			= matTransformation.GetTranslation();

	//Log(eSLT_Message, "<Sound> System   SetListener Pos: %.2f,%.2f,%.2f", vPos.x, vPos.y, vPos.z);

	if (m_bSoundSystemEnabled)
	{ 
		Vec3 vPosDelta = (vPos - vOldPos);
		float fTimeDelta = gEnv->pTimer->GetFrameTime();

		//Log(eSLT_Message, "<Sound> FrameTime delta %f sec",fTimeDelta);

		Vec3 vNewVelocity = fTimeDelta>0.f ? vPosDelta * (g_fDopplerScale/fTimeDelta) : Vec3(0);

		vNewVelocity.x = clamp(vNewVelocity.x, -30.0f, 30.0f);
		vNewVelocity.y = clamp(vNewVelocity.y, -30.0f, 30.0f);
		vNewVelocity.z = clamp(vNewVelocity.z, -30.0f, 30.0f);

    pListener->SetVelocity( (vNewVelocity + pListener->GetVelocity()) / 2.0f); // averaging with last Velocity
	}

	return (bResult);
}

void CSoundSystem::SetListenerEntity( ListenerID nListenerID, EntityId nEntityID )
{
	CListener * pListener = (CListener*)GetListener(nListenerID);

	if (pListener)
		pListener->nEntityID = nEntityID;
}

ListenerID CSoundSystem::CreateListener()
{
	if (m_vecListeners.empty())
	{
		// Setting up the standard Listener
		CListener* pNewListener = new CListener;
	
		if (!pNewListener)
			return LISTENERID_INVALID;

		pNewListener->nListenerID		= LISTENERID_STANDARD;
		pNewListener->bActive				= false; //true
		pNewListener->fRecordLevel	=	1.0;
		m_vecListeners.push_back(pNewListener);
		return LISTENERID_STANDARD;
	}

	// Create Unused ListenerID
	int32 nNewListenerID = LISTENERID_STANDARD;
	tVecListenersIter IterEnd = m_vecListeners.end();
	for (tVecListenersIter Iter = m_vecListeners.begin(); Iter != IterEnd; ++Iter)
	{
		CListener* pListener = (*Iter);
		
		if (nNewListenerID == pListener->nListenerID)
			++nNewListenerID;
		else
			break;
		//nNewListenerID = max(pListener->nListenerID, nNewListenerID);
	}
	

	// TODO verify ID Management always picks highest free ID. Could search for smallest.
	if (nNewListenerID > 32000)
		return (LISTENERID_INVALID);

	// Setting up the new Listener
	CListener* pNewListener = new CListener;
	pNewListener->nListenerID = nNewListenerID;
	m_vecListeners.push_back(pNewListener);

	return (nNewListenerID);
}

bool CSoundSystem::RemoveListener(ListenerID nListenerID)
{
	bool bResult = false;
	
	tVecListenersIter IterEnd = m_vecListeners.end();
	for (tVecListenersIter Iter = m_vecListeners.begin(); Iter != IterEnd; ++Iter)
	{
		if ((*Iter)->nListenerID == nListenerID)
		{
			m_vecListeners.erase(Iter);
			bResult = true;
			break;
		}
	}

	return (bResult);
}

ListenerID	CSoundSystem::GetClosestActiveListener(Vec3 vPosition) const
{
	bool bFirstTime = true;
	float fDistance = 0.0f;
	tVecListenersIterConst ClosestIter = m_vecListeners.begin();
	tVecListenersIterConst IterEnd = m_vecListeners.end();
	for (tVecListenersIterConst Iter = m_vecListeners.begin(); Iter != IterEnd; ++Iter)
	{
		if ((*Iter)->bActive == true)
		{
			float fTempDistance = (*Iter)->GetPosition().GetSquaredDistance(vPosition);
			if (bFirstTime)
			{
				fDistance = fTempDistance;
			}

			if (fDistance >= fTempDistance)
			{
				bFirstTime = false;
				fDistance = fTempDistance;
				ClosestIter = Iter;
			}
		}
	}

	if (!bFirstTime)
		return (*ClosestIter)->nListenerID;
	else
		return LISTENERID_INVALID;

}

uint32 CSoundSystem::GetNumActiveListeners() const 
{ 
	uint32 nNumActiveListeners = 0;

	tVecListenersIterConst IterEnd = m_vecListeners.end();
	for (tVecListenersIterConst Iter = m_vecListeners.begin(); Iter != IterEnd; ++Iter)
	{
		if ((*Iter)->bActive)
		{
			++nNumActiveListeners;
		}
	} 

	return nNumActiveListeners;
}

IListener* CSoundSystem::GetListener(ListenerID nListenerID)
{
	IListener* pListener = NULL;

	tVecListenersIter IterEnd = m_vecListeners.end();
	for (tVecListenersIter Iter = m_vecListeners.begin(); Iter != IterEnd; ++Iter)
	{
		if ((*Iter)->nListenerID == nListenerID)
		{
			pListener = (IListener*)(*Iter);
			break;
		}
	}

	return pListener;
}

IListener* CSoundSystem::GetListener(EntityId nEntityID)
{
	IListener* pListener = NULL;

	tVecListenersIter IterEnd = m_vecListeners.end();
	for (tVecListenersIter Iter = m_vecListeners.begin(); Iter != IterEnd; ++Iter)
	{
		if ((*Iter)->nEntityID == nEntityID)
		{
			pListener = (IListener*)(*Iter);
			break;
		}
	}

	return pListener;
}

IListener* CSoundSystem::GetNextListener(ListenerID nListenerID)
{
	CListener* pHighListener = NULL;
	CListener* pNextListener = NULL;

	tVecListenersIter IterEnd = m_vecListeners.end();
	for (tVecListenersIter Iter = m_vecListeners.begin(); Iter != IterEnd; ++Iter)
	{
		if ((*Iter)->nListenerID > nListenerID)
		{
			// if ID is higher than 
			pHighListener = (*Iter);

			
			// if this is the first Listener we found, its the best we have till now
			if (!pNextListener)
				pNextListener = pHighListener;

			if ((*Iter)->nListenerID < pNextListener->nListenerID)
			{
				// if the current ListenerID is smaller than the one we found before, lets switch
				pNextListener = pHighListener;
			}
		}
	}
	// NULL if no higher ID was found
	return (IListener*)pNextListener;
}
//////////////////////////////////////////////////////////////////////////
float CSoundSystem::GetSqDistanceToClosestListener(CSound* pSound)
{
	assert (pSound);

	IListener* pListener = GetListener(GetClosestActiveListener(pSound->GetPlaybackPosition()));

	if (!pListener)
		return 0.0f;

	return pListener->GetPosition().GetSquaredDistance(pSound->GetPlaybackPosition());
}

//////////////////////////////////////////////////////////////////////
bool	CSoundSystem::Silence(bool bStopLoopingSounds, bool bUnloadData)
{	
	//GUARD_HEAP;

	// stops all sounds from playing does not need a param
	if (m_pAudioDevice)
	{
		m_pAudioDevice->SetParam(adpSTOPALL_CHANNELS, NULL);
	}

	SoundVecIter ItEnd = m_AllSoundsArray.end();
	for (SoundVecIter It=m_AllSoundsArray.begin(); It!=ItEnd; ++It)
	{			
		CSound *pSound = (*It);

		if (pSound)
		{
			if (!pSound->GetLoopMode() || bStopLoopingSounds)
			{
				pSound->Stop();
			}

			//*It = NULL;
		}
	}

	if (m_pSoundAssetManager && bUnloadData)
		m_pSoundAssetManager->UnloadAllSoundBuffers();

	if (m_pAudioDevice && bUnloadData)
		m_pAudioDevice->ResetAudio();

	if (m_pMoodManager)
		m_pMoodManager->Reset();

	if (m_pReverbManager)
		m_pReverbManager->Reset();

	return true;
}

bool CSoundSystem::DeactivateAudioDevice() 
{
	g_nSoundEnable = 0; 
	Update(eSoundUpdateMode_All); 

	return m_pAudioDevice->ShutDownDevice();
}

bool CSoundSystem::ActivateAudioDevice() 
{
	g_nSoundEnable = 1;
	AudioDeviceSettings InitSettings;
	m_pAudioDevice->GetInitSettings(&InitSettings);

	InitSettings.nMaxHWchannels			= g_nMaxHWChannels;
	InitSettings.nMinHWChannels			= g_nMinHWChannels;
	InitSettings.nSoftwareChannels	= g_nSoftwareChannels;

	//SOUND_MAX_SOFTWARE_CHANNELS;

	m_pAudioDevice->SetInitSettings(&InitSettings);
	return m_pAudioDevice->InitDevice(this);
}


//////////////////////////////////////////////////////////////////////////
void	CSoundSystem::Pause(bool bPause, bool bResetVolume, unsigned int nFadeOutInMS)
{
	m_bSoundSystemPause = bPause;
	m_bIsInPauseCall = true;

	if (nFadeOutInMS == 0)
	{
		if (bPause)
		{
			SSoundContainer::iterator itEnd = m_OneShotSounds.end();
			for (SSoundContainer::iterator it = m_OneShotSounds.begin(); it != itEnd; ++it)
			{
				CSound *pSound = (*it);
				if (pSound && !(pSound->GetFlags() & FLAG_SOUND_EDITOR))
				{
					pSound->SetPaused(true);
				}
			}

			itEnd = m_LoopedSounds.end();
			for (SSoundContainer::iterator it=m_LoopedSounds.begin(); it!=itEnd; ++it)
			{
				//_smart_ptr<CSound> pSound = *It;
				CSound *pSound = (*it);
				if (pSound && !(pSound->GetFlags() & FLAG_SOUND_EDITOR))
				{
					pSound->SetPaused(true);
				}
			}

			itEnd = m_InactiveSounds.end();
			for (SSoundContainer::iterator it=m_InactiveSounds.begin(); it!=itEnd; ++it)
			{
				//_smart_ptr<CSound> pSound = *It;
				CSound *pSound = (*it);
				if (pSound && !(pSound->GetFlags() & FLAG_SOUND_EDITOR) && pSound->GetState() == eSoundState_Active)
				{
					pSound->SetPaused(true);
				}
			}
		}
		else
		{
			SSoundContainer::iterator itEnd = m_OneShotSounds.end();
			for (SSoundContainer::iterator it = m_OneShotSounds.begin(); it != itEnd; ++it)
			{
				CSound *pSound = (*it);
				if (pSound && !(pSound->GetFlags() & FLAG_SOUND_EDITOR))
				{
					pSound->SetPaused(false);
				}
			}

			itEnd = m_LoopedSounds.end();
			for (SSoundContainer::iterator it=m_LoopedSounds.begin(); it!=itEnd; ++it)
			{

				//_smart_ptr<CSound> pSound = *It;
				CSound *pSound = (*it);
				if (pSound && !(pSound->GetFlags() & FLAG_SOUND_EDITOR))
				{
					pSound->SetPaused(false);
				}
			}

			itEnd = m_InactiveSounds.end();
			for (SSoundContainer::iterator it=m_InactiveSounds.begin(); it!=itEnd; ++it)
			{
				//_smart_ptr<CSound> pSound = *It;
				CSound *pSound = (*it);
				if (pSound && !(pSound->GetFlags() & FLAG_SOUND_EDITOR) && pSound->GetState() == eSoundState_Active)
				{
					pSound->SetPaused(false);
				}
			}

		}
	}
	else
	{
		m_nFadeOutInMS = nFadeOutInMS;
	}


  //ptParamBOOL bTemp(bPause);
  //m_pAudioDevice->SetParam(adpPAUSED_STATUS, &bTemp);
    
	if (bResetVolume)
	{
		// this is needed for instance when going back to menu
		// after setting the sound volume to 0. The sound volume
		// calculation takes the general volume into account,
		// so if it was set to 0, when going to menu the sliders
		// and buttons won't produce any sound, because the sound system
		// is not updated anymore so it doesn't have a chance to 
		// update the general volume. Hence we reset the volume to 1,
		// and the individual volume of sound and music sliders is set
		// by the menu scripts calling setvolume
		//m_fSFXResetVolume=m_pCVarSFXVolume->GetFVal();
		//m_pCVarSFXVolume->Set(1.0f);				
		m_bResetVolume	= true;
		//m_fSFXVolume		= 1.0f; //m_pCVarSFXVolume->GetFVal();				
		g_fGameSFXVolume		= 1.0f;
		g_fGameMusicVolume	= 1.0f;
	}
	
	if (!bPause && m_bResetVolume)
	{	
		// restore normal volume back
		//m_pCVarSFXVolume->Set(m_fSFXResetVolume);				
		m_bResetVolume = false;

    // TODO REPLACE THE SYSTEM VAR CALL
		m_fSFXVolume = g_fSFXVolume;
		//SetGroupScale(SOUNDSCALE_MISSIONHINT,m_fSFXVolume*0.65f);		
	}

	m_bIsInPauseCall = false;

}

//////////////////////////////////////////////////////////////////////////
void	CSoundSystem::Mute(bool bMute)
{
	//GUARD_HEAP;
	if (bMute)
		m_nMuteRefCnt++;
	else
		m_nMuteRefCnt--;
	if (m_nMuteRefCnt < 0)
		m_nMuteRefCnt = 0;
	bool bSetMute = (m_nMuteRefCnt != 0);
  ptParamINT32 muteParam(bSetMute ? 0 : 255);
  m_pAudioDevice->SetParam(adpMUTE_STATUS, &muteParam);
	// TODO: uses differnt types int32/bool, eitherway use bool to set mute  or int32 to set ADP_MASTER_VOLUME
	// hard coded 255 is always bad! us SFXMastervolume and Musicvolume insted
}
 
//////////////////////////////////////////////////////////////////////
bool CSoundSystem::IsEAX(void)
{
  bool tempB=false;
  ptParamBOOL boolParam(tempB);
  m_pAudioDevice->GetParam(adpEAX_STATUS, &boolParam);
  boolParam.GetValue(tempB);
  return tempB;
}

//////////////////////////////////////////////////////////////////////
//bool CSoundSystem::SetEaxListenerEnvironment(SOUND_REVERB_PRESETS nPreset, CRYSOUND_REVERB_PROPERTIES *tpProps, int nFlags)
//{
//	if (m_pReverbManager)
//		m_pReverbManager->SetListenerReverb(nPreset, tpProps, nFlags);
//
//	return (true);
//}
/*
//////////////////////////////////////////////////////////////////////////
bool CSoundSystem::GetCurrentEaxEnvironment(int &nPreset, CRYSOUND_REVERB_PROPERTIES &Props)
{
	if (m_pReverbManager)
		m_pReverbManager->GetCurrentReverbEnvironment(nPreset, Props);
	return true;
}
*/

//////////////////////////////////////////////////////////////////////////
//bool CSoundSystem::InitSoundGroups()
//{
//	SSoundGroupProperties NewSoundGroupProperties;
//
//	NewSoundGroupProperties.fCurrentVolumeRatio				= 1.0;
//	NewSoundGroupProperties.fTargetVolumeRatio				= 1.0;
//
//	NewSoundGroupProperties.fCurrentObstructionRatio	= 0.0;
//	NewSoundGroupProperties.fTargetObstructionRatio		= 0.0;
//
//	NewSoundGroupProperties.fCurrentReverbFeed				= 1.0;
//	NewSoundGroupProperties.fTargetReverbFeed					= 1.0;
//
//	NewSoundGroupProperties.fTimeleft									= 0.0;
//	NewSoundGroupProperties.bHasChanged								= true;
//
//	m_vecSoundGroups.resize(0);
//
//	SetSoundGroupProperties(eSOUNDGROUP_NOGROUP, &NewSoundGroupProperties);
//	SetSoundGroupProperties(eSOUNDGROUP_AMBIENT, &NewSoundGroupProperties);
//	SetSoundGroupProperties(eSOUNDGROUP_ENTITY, &NewSoundGroupProperties);
//	SetSoundGroupProperties(eSOUNDGROUP_PLAYER_PERSONAL, &NewSoundGroupProperties);
//	SetSoundGroupProperties(eSOUNDGROUP_PLAYER_WORLD, &NewSoundGroupProperties);
//	SetSoundGroupProperties(eSOUNDGROUP_MISSION, &NewSoundGroupProperties);
//	SetSoundGroupProperties(eSOUNDGROUP_INTERFACE, &NewSoundGroupProperties);
//	SetSoundGroupProperties(eSOUNDGROUP_MUSIC, &NewSoundGroupProperties);
//	SetSoundGroupProperties(eSOUNDGROUP_DEAFNESS, &NewSoundGroupProperties);
//	NewSoundGroupProperties.fCurrentReverbFeed	= 0.0;
//	SetSoundGroupProperties(eSOUNDGROUP_NOREVERB, &NewSoundGroupProperties);
//
//	m_bSoundGroupPropertiesChanged = true;
//	return true;
//}

//////////////////////////////////////////////////////////////////////////
//bool CSoundSystem::SoundGroupChanged(ESOUNDGROUP eSoundGroup)
//{
//	bool bSoundGroupChanged = false;
//	for (SoundGroupVecIter It=m_vecSoundGroups.begin(); It!=m_vecSoundGroups.end(); ++It)
//	{
//		if ( (*It).eSoundGroupID == eSoundGroup)
//		{
//			bSoundGroupChanged = (*It).sProperties.bHasChanged;
//			break;
//		}
//	}
//	return (bSoundGroupChanged);
//}

//////////////////////////////////////////////////////////////////////////
//bool CSoundSystem::SetSoundGroupProperties(size_t eSoundGroup, SSoundGroupProperties *pSoundGroupProperties)
//{
//	bool SoundGroupFound						= false;
//	m_bSoundGroupPropertiesChanged	= true;
//	for (SoundGroupVecIter It=m_vecSoundGroups.begin(); It!=m_vecSoundGroups.end(); ++It)
//	{
//		// if ( (*It).eSoundGroupID > eSoundGroup) break; // this works for sorted vector only
//		if ( (*It).eSoundGroupID == eSoundGroup)
//		{
//			// if Group was found then only overwrite the new target values and the time
//			(*It).sProperties.fTargetVolumeRatio		  = pSoundGroupProperties->fTargetVolumeRatio;
//			(*It).sProperties.fTargetObstructionRatio = pSoundGroupProperties->fTargetObstructionRatio;
//			(*It).sProperties.fTargetReverbFeed			  = pSoundGroupProperties->fTargetReverbFeed;
//			(*It).sProperties.fTimeleft								= pSoundGroupProperties->fTimeleft;
//			(*It).sProperties.bHasChanged							= true;
//			SoundGroupFound														= true;
//			break;
//		}
//	}
//	if (!SoundGroupFound)
//	{
//		SSoundGroup	NewSoundGroup;
//		NewSoundGroup.eSoundGroupID												= (ESOUNDGROUP)eSoundGroup;
//
//		// if inserted also overwrite the current Values
//		NewSoundGroup.sProperties.fTargetVolumeRatio			= pSoundGroupProperties->fTargetVolumeRatio;
//		NewSoundGroup.sProperties.fCurrentVolumeRatio			= pSoundGroupProperties->fCurrentVolumeRatio;
//
//		NewSoundGroup.sProperties.fTargetObstructionRatio = pSoundGroupProperties->fTargetObstructionRatio;
//		NewSoundGroup.sProperties.fCurrentObstructionRatio = pSoundGroupProperties->fCurrentObstructionRatio;
//
//		NewSoundGroup.sProperties.fTargetReverbFeed			  = pSoundGroupProperties->fTargetReverbFeed;
//		NewSoundGroup.sProperties.fCurrentReverbFeed			  = pSoundGroupProperties->fCurrentReverbFeed;
//
//		NewSoundGroup.sProperties.fTimeleft								= pSoundGroupProperties->fTimeleft;
//		NewSoundGroup.sProperties.bHasChanged							= true;
//		SoundGroupFound																		= true;
//
//		m_vecSoundGroups.push_back(NewSoundGroup);
//		//m_vecSoundGroups.sort(m_vecSoundGroups.begin(),m_vecSoundGroups.end());
//	}
//	return (true);
//}

//////////////////////////////////////////////////////////////////////////
//bool CSoundSystem::GetSoundGroupProperties(size_t eSoundGroup, SSoundGroupProperties &pSoundGroupProperties)
//{
//	bool SoundGroupFound = false;
//	for (SoundGroupVecIter It=m_vecSoundGroups.begin(); It!=m_vecSoundGroups.end(); ++It)
//	{
//		// if ( (*It).eSoundGroupID > eSoundGroup) break; // this works for sorted vector only
//		if ( (*It).eSoundGroupID == eSoundGroup)
//		{
//			pSoundGroupProperties.fCurrentVolumeRatio				= (*It).sProperties.fCurrentVolumeRatio;
//			pSoundGroupProperties.fTargetVolumeRatio				= (*It).sProperties.fTargetVolumeRatio;
//
//			pSoundGroupProperties.fCurrentObstructionRatio	= (*It).sProperties.fCurrentObstructionRatio;
//			pSoundGroupProperties.fTargetObstructionRatio		= (*It).sProperties.fTargetObstructionRatio;
//
//			pSoundGroupProperties.fCurrentReverbFeed				= (*It).sProperties.fCurrentReverbFeed;
//			pSoundGroupProperties.fTargetReverbFeed					= (*It).sProperties.fTargetReverbFeed;
//
//			pSoundGroupProperties.bHasChanged								= (*It).sProperties.bHasChanged;
//			pSoundGroupProperties.fTimeleft									= (*It).sProperties.fTimeleft;
//
//			SoundGroupFound = true;
//			break;
//		}
//	}
//	return (SoundGroupFound);
//}

//////////////////////////////////////////////////////////////////////////
bool CSoundSystem::SetGroupScale(int nGroup, float fScale)
{
	//GUARD_HEAP;
	if ((nGroup < 0) || (nGroup >= MAX_SOUNDSCALE_GROUPS))
		return false;

	if (fabsf(m_fSoundScale[nGroup] - fScale) < 0.001f)
		return true; // too small change

	m_fSoundScale[nGroup] = clamp(fScale, 0.0f, 1.0f);
	//change volume of all looping/playing sounds
	//Log(eSLT_Message, "<Sound> Scaling group for %s",cs->GetName());

	SoundVecIter ItEnd = m_AllSoundsArray.end();
	for (SoundVecIter It=m_AllSoundsArray.begin(); It!=ItEnd; ++It)
	{			
		CSound *pSound = (*It);

		if (pSound)
		{
			//pSound->m_bVolumeHasChanged = true;
			pSound->SetVolume(pSound->GetVolume());
			//pSound->SetRatio(pSound->GetRatio()); // change the volume so it updates with new group scale value at sounds update
		}
	}

	return true;
}

void CSoundSystem::SetMasterVolume(float fVol)
{
	if (!NumberValid(fVol))
	{
		Log(eSLT_Warning, "<Sound> %s Invalid input values (possible NaNs) on SetMasterVolume()");
		assert(0);
		return;
	}

	ptParamF32 NewParam(fVol);
	m_pAudioDevice->SetParam(adpMASTER_VOLUME, &NewParam);
}

//////////////////////////////////////////////////////////////////////
void CSoundSystem::SetMasterVolumeScale(float fScale, bool bForceRecalc)
{ 
	if (!NumberValid(fScale))
	{
		Log(eSLT_Warning, "<Sound> %s Invalid input values (possible NaNs) on SetMasterVolumeScale()");
		assert(0);
		return;
	}

	//GUARD_HEAP;
	if ((!bForceRecalc) && ((fabsf(fScale-m_fSoundScale[0]))<0.01f))
		return;
	SetGroupScale(0, fScale);
}

//////////////////////////////////////////////////////////////////////
//! sets the weather condition that affect acoustics
bool CSoundSystem::SetWeatherCondition(float fWeatherTemperatureInCelsius, float fWeatherHumidityAsPercent, float fWeatherInversion)
{
	//GUARD_HEAP;
	
	bool localresult = true;
	if (fWeatherTemperatureInCelsius >= -273.15f && fWeatherTemperatureInCelsius <= 100.0f)
	{
		m_fWeatherTemperatureInCelsius = fWeatherTemperatureInCelsius;
	}
	else 
	{
		if (fWeatherTemperatureInCelsius < -273.15f) m_fWeatherTemperatureInCelsius = -273.15f;
		if (fWeatherTemperatureInCelsius > 100.0f) m_fWeatherTemperatureInCelsius = 100.0;
		localresult = false;
	}

	if (fWeatherHumidityAsPercent >= 0.0f && fWeatherHumidityAsPercent <= 100.0f)
	{
		m_fWeatherHumidityAsPercent = m_fWeatherHumidityAsPercent;
	}
	else 
	{
		if (fWeatherHumidityAsPercent < 0.0f) m_fWeatherHumidityAsPercent = 0;
		if (fWeatherHumidityAsPercent > 100.0f) m_fWeatherHumidityAsPercent = 100.0f;
		localresult = false;
	}

	if (fWeatherInversion >= 0.0f && fWeatherInversion <= 1.0f)
	{
		m_fWeatherInversion = fWeatherInversion;
	}
	else 
	{
		if (m_fWeatherInversion < 0.0f) m_fWeatherInversion = 0.0f;
		if (m_fWeatherInversion > 1.0f) m_fWeatherInversion = 1.0f;
		localresult = false;
	}

	// Calculation of the Airabsorptionmultiplyer
	float RelativeAirPressure = 1;
	float TemperatureInKelvin = m_fWeatherTemperatureInCelsius + 273.15f;
	float RelativeTemperature = TemperatureInKelvin/293.15f;  // convert to relative air temp (re 20 deg C)
	float Frequency2 = 2500.0f * 2500.0f;
	float C = 4.6151f - 6.8346f * cry_powf((273.16f/TemperatureInKelvin),1.261f);
	float h = m_fWeatherHumidityAsPercent*cry_powf(10,C)*RelativeAirPressure;	
	
	float FrequencyRelaxiationOfOxygen = RelativeAirPressure*(24.0f + 4.04e4f*h*(0.02f + h) / (0.391f + h));
	float FrequencyRelaxiationOfNitrogen=RelativeAirPressure*cry_powf(RelativeTemperature,-0.5f)*(9.0f+280.0f*h*cry_expf(-4.17f*(cry_powf(RelativeTemperature,(-1.0f/3.0f))-1.0f)));

	float TempResult1 = 0.01275f*(cry_expf(-2239.1f/TemperatureInKelvin)*1.0f
						/ (FrequencyRelaxiationOfOxygen + Frequency2/FrequencyRelaxiationOfOxygen));

	float TempResult2 = 0.1068f * (cry_expf(-3352.0f/TemperatureInKelvin)* 1.0f
						/(FrequencyRelaxiationOfNitrogen+Frequency2 / FrequencyRelaxiationOfNitrogen));

	float alpha = 8.686f*Frequency2*(1.84e-11f*(1.0f/RelativeAirPressure)*sqrtf(RelativeTemperature) 
				  + cry_powf(RelativeTemperature,-2.5f)* (TempResult1 + TempResult2));

	//float dBAttenuationPerKilometer = 1000*alpha;
	//float mdBAttenuationPerMeter = -100 * alpha; // EAX wants negative attenuation in mdB = 1/100th dB

	// TODO Get Airabsorption from ReverbManager
	//if (mdBAttenuationPerMeter > 0.0001) m_fWeatherAirAbsorptionMultiplyer=0;
	//else m_fWeatherAirAbsorptionMultiplyer = mdBAttenuationPerMeter/m_sTempEaxPreset.EAX.AirAbsorptionHF;

	return localresult;
}

//////////////////////////////////////////////////////////////////////
//! gets the weather condition that affect acoustics
bool CSoundSystem::GetWeatherCondition(float &fWeatherTemperatureInCelsius, float &fWeatherHumidityAsPercent, float &fWeatherInversion)
{
	//GUARD_HEAP;
	fWeatherTemperatureInCelsius = m_fWeatherTemperatureInCelsius;
	fWeatherHumidityAsPercent = m_fWeatherHumidityAsPercent;
	fWeatherInversion = m_fWeatherInversion;
	return true;
}

//////////////////////////////////////////////////////////////////////
void	CSoundSystem::GetSoundMemoryUsageInfo(int *nCurrentMemory,int *nMaxMemory) const
{
	// TODO verify is this working?
	//GUARD_HEAP;
  int tmpnCurrentMemory = *nCurrentMemory;
  int tmpnMaxMemory = *nMaxMemory;

  ptParamINT32 curMemUse(tmpnCurrentMemory);
  ptParamINT32 maxMemUse(tmpnMaxMemory); 
  m_pAudioDevice->GetParam(adpSNDSYSTEM_MEM_USAGE_NOW,&curMemUse);
  m_pAudioDevice->GetParam(adpSNDSYSTEM_MEM_USAGE_HIGHEST, &maxMemUse);
 
	curMemUse.GetValue(tmpnCurrentMemory);
  maxMemUse.GetValue(tmpnMaxMemory);

  *nCurrentMemory = tmpnCurrentMemory;
  *nMaxMemory = tmpnMaxMemory;
}

//////////////////////////////////////////////////////////////////////////
int	CSoundSystem::GetUsedVoices() const
{
	//GUARD_HEAP;
  int nUsedVoices = 0;

  ptParamINT32 TempParam(nUsedVoices);

  // actively played sounds
  if (m_pAudioDevice->GetParam(adpCHANNELS_PLAYING ,&TempParam))
	{
		TempParam.GetValue(nUsedVoices);
	}
	else
	{
		// software fall back
		nUsedVoices = 0;
		TSoundIDSet::const_iterator ItEnd = m_currentSoundIDs.end();
		for (TSoundIDSet::const_iterator It=m_currentSoundIDs.begin(); It!=ItEnd; ++It)
		{			
			CSound* pSound = (CSound*)GetSound(*It);

			if (pSound)
			{
				if (pSound->IsPlayingOnChannel())
					++nUsedVoices;
			}
		}
	}

  return nUsedVoices;
}

///////////////////////////////////	///////////////////////////////////////
float	CSoundSystem::GetCPUUsage() const
{
	//GUARD_HEAP;
  float fCpuPctUsed = 0.0f;

  m_pAudioDevice->GetCpuUsage(0, 0, 0, 0, &fCpuPctUsed);

  return fCpuPctUsed;
}

//////////////////////////////////////////////////////////////////////////

float CSoundSystem::GetMusicVolume() const
{
	return (m_fMusicVolume*m_fSoundScale[SOUNDSCALE_MISSIONHINT]*m_fSoundScale[SOUNDSCALE_DEAFNESS] * m_fMusicEffectVolume * m_fGameMusicVolume);
}

//////////////////////////////////////////////////////////////////////////
void CSoundSystem::CalcDirectionalAttenuation(const Vec3 &vPos, const Vec3 &vDir, const float fConeInRadians)
{
	//GUARD_HEAP;
	float fCosCone = (float)cry_cosf(fConeInRadians);
	if (IsEquivalent(vPos, m_DirAttPos, VEC_EPSILON) &&
			IsEquivalent(vDir, m_DirAttDir, VEC_EPSILON) &&
			(fCosCone == m_fDirAttCone))
	 return;

	m_DirAttPos       = vPos;
	m_DirAttDir       = vDir;
	m_fDirAttCone     = fCosCone;
	m_fDirAttMaxScale = 0.0f;
	m_bNeedSoundZoomUpdate = true;

	// TODO remove the whole for loop when Update() of sound takes care of this automatically
	//update position of all looping/playing sounds
	//for (SoundSetIter It=m_setAllSounds.begin(); It!=m_setAllSounds.end(); ++It)
	//{
	//	CSound *cs = (*It);
	//	if (cs->IsPlaying() || cs->IsPlayingVirtual())
	//		cs->SetPosition();
	//}
}

//////////////////////////////////////////////////////////////////////////
void CSoundSystem::GetMemoryUsage(class ICrySizer* pSizer) const
{
	//GUARD_HEAP;
	if(pSizer==NULL)
		return;
	

#ifndef _LIB // Only when compiling as dynamic library
	{
		//SIZER_COMPONENT_NAME(pSizer,"Strings");
		//pSizer->AddObject( (this+1),string::_usedMemory(0) );
	}
	{
		SIZER_COMPONENT_NAME(pSizer,"STL Allocator Waste");
		CryModuleMemoryInfo meminfo;
		ZeroStruct(meminfo);
		CryGetMemoryInfoForModule( &meminfo );
		pSizer->AddObject( (this+2),(size_t)meminfo.STL_wasted );
	}
#endif

	{
		SIZER_COMPONENT_NAME(pSizer, "System");
		pSizer->AddObject( this, sizeof(*this));
	}
	
	pSizer->AddObject(m_DelayedPrecaches);
	pSizer->AddObject(m_AllSoundsArray);					
	pSizer->AddObject(m_pSoundAssetManager);	
	pSizer->AddObject(m_InactiveSounds);	
	pSizer->AddObject(m_UnusedSoundObjects);	
	pSizer->AddObject(m_pMoodManager);			
	pSizer->AddObject(m_pReverbManager);
	pSizer->AddObject(m_pAudioDevice);

	stl::STLPoolAllocator<tSoundID, stl::PoolAllocatorSynchronizationSinglethreaded>::GetMemoryUsage(pSizer);		
}


//////////////////////////////////////////////////////////////////////////
int CSoundSystem::GetMemoryUsageInMB(bool bGetPoolSize)
{
	if (bGetPoolSize) {
		ICVar* pCacheStatic = gEnv->pConsole->GetCVar("s_CacheStatic");
		ICVar* pCacheSize = gEnv->pConsole->GetCVar("s_CacheSize");

		if (pCacheStatic && pCacheSize && pCacheStatic->GetIVal() == 1 )
		{
			return (int)pCacheSize->GetFVal();
		}

		return 0;


	} else {
		//GUARD_HEAP;
		int nSizeInByte = 0;

		nSizeInByte += m_pSoundAssetManager->GetMemUsage();

		nSizeInByte += m_pAudioDevice->GetMemoryUsage(0);


		int nSoundSize = 0;
		TSoundIDSet::const_iterator ItEnd = m_currentSoundIDs.end();
		for (TSoundIDSet::const_iterator It=m_currentSoundIDs.begin(); It!=ItEnd; ++It)
		{			
			nSoundSize += sizeof(CSound);
		}
		nSizeInByte += nSoundSize;

		nSizeInByte = nSizeInByte/(1024*1024);

		return nSizeInByte;
	}
}

//////////////////////////////////////////////////////////////////////////
int CSoundSystem::SetMinSoundPriority( int nPriority )
{
	int nPrev = m_nMinSoundPriority;
	m_nMinSoundPriority = nPriority;
	return nPrev;
}

//////////////////////////////////////////////////////////////////////////
void CSoundSystem::LockResources()
{
	m_pSoundAssetManager->LockResources();
}

//////////////////////////////////////////////////////////////////////////
void CSoundSystem::UnlockResources()
{
	m_pSoundAssetManager->UnlockResources();
}

void CSoundSystem::SetGlobalParameter( const char *sGlobalParameter, const float fValue)
{
	int nIndex = -1;
	m_ParameterNames.AddOrSetParameter(sGlobalParameter, true, fValue, nIndex);
}

void CSoundSystem::ProcessHDRS()
{
	//if (m_pCVarEnableHDRA->GetIVal() == 0)
	//	return;

	float fEffectRadius = 0.25f;
	
	TSoundIDSet::iterator ItEnd = m_currentSoundIDs.end();
	for (TSoundIDSet::iterator It1=m_currentSoundIDs.begin(); It1!=ItEnd; ++It1)
	{			
		CSound* pSound1 = (CSound*)GetSound(*It1);

		if (pSound1)
		{
			if (!pSound1->IsPlaying())
				continue;

			for (TSoundIDSet::iterator It2=It1; It2!=ItEnd; ++It2)
			{			
				CSound* pSound2 = (CSound*)GetSound(*It2);
				if (pSound2)
				{
					if (!pSound2->IsPlaying())
						continue;

					// check distance if the affect each other at all
					float fDistance = pSound1->GetPlaybackPosition().GetDistance(pSound2->GetPlaybackPosition());
					float fMax1 = pSound1->GetMaxDistance()*fEffectRadius;
					float fMax2 = pSound2->GetMaxDistance()*fEffectRadius;
					
					if (fDistance > max(fMax1, fMax2))
						continue;

					// check loudness of sounds.
					float fLoudness1, fLoudness2 = 0.0f;
				
					if (!pSound1->GetParam("loudness", &fLoudness1) || !pSound2->GetParam("loudness", &fLoudness2))
						continue;

					float fEffect = 0.0f;
					float fAttenuation = 0.0f;

					if (fLoudness1 > fLoudness2)
					{
						fEffect = 1 - min(1.0f, (fDistance/fMax1));
						fAttenuation = square(log10f(fLoudness2)/log10f(fLoudness1));
						fAttenuation = 1 - fAttenuation*fEffect;
						pSound1->SetParam("HDRS", fAttenuation);
					}
					else
						if (fLoudness2 > fLoudness1)
						{
							fEffect = 1 - max(1.0f, (fDistance/fMax2));
							fAttenuation = square(log10f(fLoudness1)/log10f(fLoudness2));
							fAttenuation = 1 - fAttenuation*fEffect;
							pSound1->SetParam("HDRA", fAttenuation);
						} 
				}
			}
		}
	}
}

void CSoundSystem::ProcessInactiveSounds()
{
	FRAME_PROFILER( "SoundSystem::Update:ProcessInactive", gEnv->pSystem, PROFILE_SOUND );
	float fCurrTime = gEnv->pTimer->GetCurrTime(); // store the current time locally

	int nActiveSize = m_OneShotSounds.size(); // size of active sound spots
	if ((nActiveSize < g_nMaxActiveSounds) && (!m_InactiveSounds.empty()))
	{		
		int nInactiveSounds = m_InactiveSounds.size();

		// lets choose the iteration-count, so it iterates through all sounds every m_pCVarInactiveSoundIterationTimeout seconds
		int nInactiveSoundItCount = (int)cry_ceilf((gEnv->pTimer->GetFrameTime()/g_fInactiveSoundIterationTimeout) * (float)nInactiveSounds);	

		// make sure we at least process one
		nInactiveSoundItCount = max(nInactiveSoundItCount, 1);
		int nSoundSpotAdded = 0;

		SSoundContainer::iterator It = m_InactiveSounds.begin();
		SSoundContainer::iterator ItEnd = m_InactiveSounds.end();

		// continue where we stopped last time if index is within the range
		if (nInactiveSounds > m_nLastInactiveSoundsIndex)
			It += m_nLastInactiveSoundsIndex;
		else
			m_nLastInactiveSoundsIndex = 0;

		for (int i=0; i < nInactiveSoundItCount; ++i)
		{
			bool bActivateSound = false;
			bool bDeactivateSound = false;
			bool bTooFar = false;
			
			CSound *pSound = (*It);
			bool bIsSoundPH = IsSoundPH(pSound);


			// catch oneshots which got stolen or somehow ended playing but aren't actually
			if (!pSound->GetLoopMode() && !pSound->IsActive() && !pSound->GetPaused() && !pSound->IsLoading())
			{
				// catch sounds which are not updated through looping or oneshot lists
				if (!pSound->IsPlaying())
					pSound->Update();
				else
					pSound->Stop();
			}

			// catch loops which somehow ended playing but aren't actually
			if (pSound->GetLoopMode() /* && pSound->IsActive() */ && pSound->GetState() == eSoundState_Ending)
			{
				// catch sounds which are not updated through looping or oneshot lists
				pSound->Update();
			}

			if ((pSound->GetFlags() & FLAG_SOUND_RADIUS))
			{
				if (m_bNeedSoundZoomUpdate)
					pSound->SetPosition(pSound->GetPosition());
			}

			if (((pSound->IsPlaying() && !pSound->GetPaused()) || pSound->GetFlags() & FLAG_SOUND_RELATIVE) && !pSound->IsLoading() && bIsSoundPH)
			{
				IListener* pListener = GetListener(GetClosestActiveListener(pSound->GetPlaybackPosition()));
				if (!pListener)
					return;
				float fDist2 = pListener->GetPosition().GetSquaredDistance(pSound->GetPlaybackPosition());

				if (fDist2 <= pSound->GetSquardedPreloadRadius() || !(pSound->GetFlags() & FLAG_SOUND_RADIUS) ) // Prevent sounds without a radius from being taken into account here (this caused CRYII-2268)
				{					
					//////////////////////////////////////////////////////////////////////////					
					// are we getting too many sound spots?					
					if ((nActiveSize+nSoundSpotAdded) > g_nMaxActiveSounds)
					{
						break; // resume next frame
					}
					//////////////////////////////////////////////////////////////////////////

					bActivateSound = true;
				}
				else 
				{
					// remove all non-looping sounds that are expired
					if (!pSound->GetLoopMode())
					{
						if (pSound->IsPlayLengthExpired(fCurrTime) && !pSound->IsActive())
							bDeactivateSound = true;
					}					
					else
						bTooFar = true;
				}
			}
			else
			{
				if (((!pSound->IsPlaying() && !pSound->IsLoading()) || (!bIsSoundPH && !pSound->GetLoopMode())) && !pSound->IsActive() )
				{
					if (pSound->IsPlaying())
						pSound->Update();

					// Only deactivate non-looping sounds or every sound that is stopped
					if (!pSound->GetLoopMode() || pSound->GetState() == eSoundState_Stopped)
						bDeactivateSound = true;
				}
			}

			if (bActivateSound)
			{					
				int nActiveSounds = (int)m_OneShotSounds.size() + (int)m_LoopedSounds.size();
				bool bTooManyActiveSounds = (nActiveSounds > g_nMaxActiveSounds);

				if (!bTooManyActiveSounds)
				{
					// move sound to active list...
					if (pSound->GetLoopMode())
						m_LoopedSounds.AddUnique(pSound);
					else
						m_OneShotSounds.AddUnique(pSound);

					nSoundSpotAdded++;

					// remove it: copy last element (which was not processed) and pop back
					if (It != ItEnd-1)
					{
						(*It) = *(ItEnd-1);
					}
					else
					{
						// wrap by pop_back
						It = m_InactiveSounds.begin();
						m_nLastInactiveSoundsIndex = 0;
					}

					m_InactiveSounds.pop_back();
					ItEnd = m_InactiveSounds.end(); // renew ItEnd

					pSound->SetState(eSoundState_Active);
					pSound->Preload();			
					continue; // It already points to next unprocessed, dont ++It
				}
			}

			if (bDeactivateSound)
			{
				pSound->SetState(eSoundState_None);
				
				assert(!pSound->IsActive());
				
				// pass it over
				m_stoppedSoundToBeDeleted.AddUnique(pSound); 

				if (!pSound->GetLoopMode() && pSound->IsPlaying())
					pSound->Stop();

				// remove it: copy last element and pop back
				if (It != ItEnd-1)
				{
					(*It) = *(ItEnd-1);
				}
				else
				{
					// wrap by pop_back
					It = m_InactiveSounds.begin();
					m_nLastInactiveSoundsIndex = 0;
				}

				m_InactiveSounds.pop_back();
				ItEnd = m_InactiveSounds.end(); // renew ItEnd
				continue; // It already points to next unprocessed, dont ++It
			}

			if (!bActivateSound && !bDeactivateSound && !bTooFar)
			{
				// not sure what happened here, stuck in obstruction?
				if (pSound->GetFlags() & FLAG_SOUND_OBSTRUCTION && g_nObstructionAccuracy > 0 && pSound->GetObstruction()->nRaysShot == 0)
				{
					if (pSound->GetObstruction()->bDelayPlayback)
						pSound->Update();
					else
						pSound->Play();
				}
			}

			// next unprocessed element
			++It;
			++m_nLastInactiveSoundsIndex;

			// if current It is the last sound spot then reset It to begin
			if (It == ItEnd)
			{
				It = m_InactiveSounds.begin();
				m_nLastInactiveSoundsIndex = 0;
			}
		} // for loop
	}
}

//////////////////////////////////////////////////////////////////////////
void CSoundSystem::ProcessOneShotSounds()
{
	FRAME_PROFILER( "SoundSystem::Update:ProcessOneShot", gEnv->pSystem, PROFILE_SOUND );
	//////////////////////////////////////////////////////////////////////////
	// Stop expired auto stop sounds.
	//////////////////////////////////////////////////////////////////////////
	if (!m_OneShotSounds.empty())
	{
		float fCurrTime = gEnv->pTimer->GetCurrTime(); // store the current time locally

		//Log(eSLT_Message, "<Sound> Processing OneShotSounds: %d\n", m_OneShotSounds.size());
		//float fCurrTime = gEnv->pTimer->GetCurrTime(); // store the current time locally
		SSoundContainer::iterator ItEnd = m_OneShotSounds.end();
		for (SSoundContainer::iterator It = m_OneShotSounds.begin(); It != ItEnd; )
		{
			CSound *pSound = (*It);

			if ((pSound->GetFlags() & FLAG_SOUND_RADIUS))
			{
				if (m_bNeedSoundZoomUpdate)
					pSound->SetPosition(pSound->GetPosition());
			}

			bool bIsSoundPotentiallyHearable = ((pSound->GetFlags() & FLAG_SOUND_2D) || IsSoundPH(pSound));
			if (bIsSoundPotentiallyHearable)
			{
				pSound->SetFade(1.0f, 1000);
			}
			else
			{
				pSound->SetFade(0.0f, 1000);
			}

			bool bIsEnding = pSound->GetState() == eSoundState_Ending;
			bool bRemove = pSound->GetState() == eSoundState_Stopped || pSound->GetPlatformSoundPtr()->GetSoundHandle() == NULL;
			bool bIsPlaying = (pSound->IsPlaying() && !bRemove); // timed out or ended by callback
			bool bProcessSound = ProcessActiveSound(pSound);
			bool bFadeState = pSound->GetFadeState() == eFadeState_JustFinished;
			
			if ((!(bIsPlaying && bProcessSound)) || (!bIsSoundPotentiallyHearable && bFadeState) || bIsEnding)// || bExpired) // && pSound->IsPlayingOnChannel()
			{

				if (!bIsEnding)
				{
					pSound->Stop(); 
				}
			}

			if (bRemove)
			{
				//CTimeValue test = gEnv->pTimer->GetFrameStartTime();
				//Log(eSLT_Message, "<Sound> Sound moved to ToBeDeleted: %s : %f\n", pSound->GetName(), test.GetSeconds());
				m_stoppedSoundToBeDeleted.AddUnique( pSound );

				// copy last element (not processed) and pop back
				if (It != ItEnd-1)
				{
					(*It) = *(ItEnd-1);
				}

				m_OneShotSounds.pop_back();
				ItEnd = m_OneShotSounds.end();

				continue; // It already points to next unprocessed, dont ++It
			}

			//if (pSound->GetState() == eSoundState_Active || pSound->GetState() == eSoundState_Ending)

			pSound->Update();

		// next unprocessed element
		++It;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CSoundSystem::ProcessLoopedSounds()
{
	FRAME_PROFILER( "SoundSystem::Update:ProcessLooped", gEnv->pSystem, PROFILE_SOUND );
	// process all active looped sounds for fading and occlusion
	//float fCurrTime = gEnv->pTimer->GetCurrTime(); // store the current time locally
	//std::pair< SoundSmartSetIter, bool > SoundSetIterBoolPair;

	SSoundContainer::iterator ItEnd = m_LoopedSounds.end();
	for (SSoundContainer::iterator It = m_LoopedSounds.begin(); It!=ItEnd; )
	{
//		FRAME_PROFILER( "SoundSystem::Update:ProcessActive",GetSystem(),PROFILE_SOUND );

		_smart_ptr<CSound> pSound = *It;

		// if loading failed: set NONE state and remove it from active sound spots
		if (!pSound->GetSoundBufferPtr() || (pSound->GetSoundBufferPtr() && pSound->GetSoundBufferPtr()->LoadFailure()) || !pSound->IsPlaying())
		{
			pSound->Stop();
			m_stoppedSoundToBeDeleted.AddUnique(pSound);

			// copy last element (not processes) and pop back
			if (It != ItEnd-1)
			{
				(*It) = *(ItEnd-1);
			}
			m_LoopedSounds.pop_back();
			ItEnd = m_LoopedSounds.end(); // renew ItEnd
			
			continue; // It already points to next unprocessed, dont ++It
		}

		// check if the sound became occluded
		//bool bZeroFadeCheckDist = false;
		bool bDeactivate = false;

		bool bIsSoundPotentiallyHearable = IsSoundPH(pSound);
		if (bIsSoundPotentiallyHearable)
		{
			pSound->SetFade(1.0f, 1000);
		}
		else
		{
			pSound->SetFade(0.0f, 1000);
		}

		float fRatio				= 1.0f;
		float fDistSquared	= 0.0f;
		CListener* pListener = NULL;

		if ((pSound->GetFlags() & FLAG_SOUND_RADIUS))
		{
			pListener = (CListener*)GetListener(GetClosestActiveListener(pSound->GetPlaybackPosition()));

			if (pListener)
				fDistSquared = pListener->GetPosition().GetSquaredDistance(pSound->GetPlaybackPosition());

			if (m_bNeedSoundZoomUpdate)
				pSound->SetPosition(pSound->GetPosition());
		}

		float fDistanceToListener = sqrt_tpl(fDistSquared);
		bool bBeyondPreload = (fDistSquared > pSound->GetSquardedPreloadRadius());
		bool bBeyondRadius = (fDistSquared > pSound->GetSquardedMaxRadius());
		bool bMoveToInactive = false;

		if ( bBeyondPreload || (!bIsSoundPotentiallyHearable && pSound->GetFadeState() == eFadeState_JustFinished))
		{
			// the listener is beyond the sound's preload radius
			if (pSound->IsActive())
			{
				// end the playback
				pSound->SetRatio(0.0f);
				pSound->Deactivate();
				pSound->Update();
			}
			else
			{
				// if the playback ended move sound to inactive list...
				bMoveToInactive = true;
			}
		}			

		if (bBeyondRadius)
		{
			// the listener is beyond the sound's max radius
			if (pSound->IsActive())
			{
				// wait until the sound ended
				pSound->SetRatio(0.0f);
				pSound->Deactivate();
				pSound->Update();
			}
			else
			{
				// if the playback ended move sound to inactive list...
				bMoveToInactive = true;
			}
		}


		if (bMoveToInactive && bBeyondPreload)
		{
			// move sound to inactive list...
			m_InactiveSounds.AddUnique(pSound);

			// copy last element (not processed) and pop back
			if (It != ItEnd-1)
			{
				(*It) = *(ItEnd-1);
			}
			m_LoopedSounds.pop_back();
			ItEnd = m_LoopedSounds.end(); // renew ItEnd

			pSound->SetState(eSoundState_Inactive);
			continue; // It already points to next unprocessed, dont ++It
		}


		// from here the sound is within the radius




		// the sound is faded out to 0, we just do this check here because it might also be too far away in which case it will be moved to the inactive list again...
		//if (bZeroFadeCheckDist)
		{
			// next unprocessed element
			//++It;
			//continue;	// jump to the next for loop
		}

		if ((pSound->GetFlags() & FLAG_SOUND_RADIUS))
		{
			//calc attenuation, cal sound ratio between min & max radius
			fRatio = pSound->GetRatioByDistance(sqrt_tpl(fDistSquared));
			pSound->SetRatio(fRatio);
		}


		if (pSound->GetSoundBufferPtr())
		{
			if (!pSound->GetSoundBufferPtr()->Loaded())
				pSound->GetSoundBufferPtr()->Load(pSound);
		}
		// if the sound is not allocated in the sound library yet and is a looping sound then trigger play event
		if (!pSound->IsActive() && pSound->IsPlayingVirtual() && bIsSoundPotentiallyHearable) //loop check 
		{
			if (!pListener)
			{
				int a = 0;	(void)a;
			}

			if (!pSound->CanBeCulled(pListener, fDistanceToListener, fDistSquared))
				pSound->StartPlayback(pListener, fDistanceToListener, fDistSquared);
		}

		//if (pSound->GetState() == eSoundState_Active)
		pSound->Update();

		// next unprocessed element
		++It;

		//{
			//FRAME_PROFILER( "SoundSystem::Update:Active-SetAttrib", gEnv->pSystem, PROFILE_SOUND );
			// TODO this next call is probably worthless, bad style anyway TN
			//pSound->Update();
		//}
	} // for loop AllActiveSounds
}

bool CSoundSystem::GetRecordDeviceInfo(const int nRecordDevice, char* sName, int nNameLength)
{
	return false;
}

IMicrophone* CSoundSystem::CreateMicrophone(
																			const unsigned int nRecordDevice,
																			const unsigned int nBitsPerSample, 
																			const unsigned int nSamplesPerSecond,
																			const unsigned int nBufferSizeInSamples)
{
#if defined(SOUNDSYSTEM_USE_FMODEX400)
	IMicrophone * pMicrophone = new CMicrophone(m_pAudioDevice );
	m_Microphones.push_back(pMicrophone);
	return pMicrophone;
#else
	return NULL;
#endif
}

bool CSoundSystem::RemoveMicrophone( IMicrophone *pMicrophone )
{
	//remove Microphones
	MicroVecIter ItEnd = m_Microphones.end();
	for (MicroVecIter It=m_Microphones.begin(); It!=ItEnd; ++It)
	{	
		if (pMicrophone == (*It))
		{
			pMicrophone->Stop();
			m_Microphones.erase(It);
			return true;
		}
	}
	return false;
}

ISound* CSoundSystem::CreateNetworkSound(	INetworkSoundListener *pNetworkSoundListener,
																	 const unsigned int nBitsPerSample, 
																	 const unsigned int nSamplesPerSecond,
																	 const unsigned int nBufferSizeInSamples,
																	 EntityId PlayerID)
{
	//GUARD_HEAP;
	//FUNCTION_PROFILER( GetSystem(),PROFILE_SOUND );

	uint32 nFlags = /*FLAG_SOUND_DEFAULT_3D |*/ FLAG_SOUND_LOOP;

	IEntity *pWho = gEnv->pEntitySystem->GetEntity(PlayerID);
	
	if (!pWho)
		return NULL;

	SSoundBufferProps NetworkProp = SSoundBufferProps(pWho->GetName(), nFlags, btNETWORK, 0, 0); 
	CSoundBuffer *pBufNetwork = m_pAudioDevice->CreateSoundBuffer(NetworkProp);

	//create a new instance	
	CSound* pSound = GetNewSoundObject();

	if (!pSound || !pBufNetwork)
		return NULL; // something wrong

	pBufNetwork->GetInfo()->nBitsPerSample = nBitsPerSample;
	pBufNetwork->GetInfo()->nBaseFreq = nSamplesPerSecond;
	pBufNetwork->GetInfo()->nLengthInSamples = nBufferSizeInSamples;

	pSound->SetSoundBufferPtr(pBufNetwork);		
	pBufNetwork->LoadAsNetworkStream(pNetworkSoundListener);

	// Now create the platform sound
	IPlatformSound* pPlatformSound	= m_pAudioDevice->CreatePlatformSound(pSound, ""); // pWho->GetName()

	if (!pPlatformSound)
	{
		delete pPlatformSound;
		return NULL; // something wrong
	}

	pSound->SetPlatformSoundPtr(pPlatformSound);
	SetMinMaxRadius(*pSound, 0.1f, 25);

	AddToSoundSystem(pSound);
	
	return pSound;
}

void CSoundSystem::RemoveNetworkSound (ISound *pSound)
{
	pSound->Stop();
	pSound->UnloadBuffer(sbUNLOAD_ALL_DATA);

}

void CSoundSystem::RenderAudioInformation()
{
	if (g_nSoundInfo)
	{ 
		IRenderer* pRenderer = gEnv->pRenderer;
		if (pRenderer)
		{
			int32 nEventCount = 0;
			int32 nGroupCount = 0;
			ptParamINT32 nTemp(nEventCount);
			m_pAudioDevice->GetParam(adpEVENTCOUNT, &nTemp);
			nTemp.GetValue(nEventCount);
			m_pAudioDevice->GetParam(adpGROUPCOUNT, &nTemp);
			nTemp.GetValue(nGroupCount);

			int nChannels = m_pAudioDevice->GetNumberSoundsPlaying();
			float fDSP = 0.0f;
			float fStream = 0.0f;
			float fGeometry = 0.0f;
			float fUpdate = 0.0f;
			float fTotal = 0.0f;
			bool bCPUUsage = m_pAudioDevice->GetCpuUsage(&fDSP, &fStream, &fGeometry, &fUpdate, &fTotal);
			bool bSpam = m_SpamFilter.WasThereSpam();
			bool bCacheStatic = false;
			float fCacheSize = 0.0f;
			float fCacheRSXSize = 0.0f;

			ICVar* pCacheSize = gEnv->pConsole->GetCVar("s_CacheSize");
			fCacheSize = pCacheSize ? pCacheSize->GetFVal() : 0.0f;

			ICVar* pCacheStatic = gEnv->pConsole->GetCVar("s_CacheStatic");
			bCacheStatic = (pCacheStatic && pCacheStatic->GetIVal());

			ICVar* pCacheRSXSize = gEnv->pConsole->GetCVar("s_CacheRSXSize");
			fCacheRSXSize = pCacheRSXSize ? pCacheRSXSize->GetFVal() : 0.0f;

			unsigned int nSoundMemAll = 0;
			unsigned int nSoundMemMain = 0;
			unsigned int nSoundMemRSX = 0;
			unsigned int nSystemMem = 0;
			unsigned int nSoundCacheFillMain = 0;
			unsigned int nSoundCacheFillSecondary = 0;
			unsigned int nSoundCacheSizeMain = 0;
			unsigned int nSoundCacheSizeSecondary = 0;
			m_pAudioDevice->GetMemoryStats(admqSOUNDSECONDARY, &nSoundMemRSX);
			m_pAudioDevice->GetMemoryStats(admqSOUNDALL, &nSoundMemAll);
			m_pAudioDevice->GetMemoryStats(admqALL, &nSystemMem);
			m_pAudioDevice->GetMemoryStats(admqCACHEFILLMAIN, &nSoundCacheFillMain);
			m_pAudioDevice->GetMemoryStats(admqCACHEFILLSECONDARY, &nSoundCacheFillSecondary);
			m_pAudioDevice->GetMemoryStats(admqCACHESIZEMAIN, &nSoundCacheSizeMain);
			m_pAudioDevice->GetMemoryStats(admqCACHESIZESECONDARY, &nSoundCacheSizeSecondary);

			if (nSoundMemRSX)
				m_pAudioDevice->GetMemoryStats(admqSOUNDMAIN, &nSoundMemMain);
			
			int nPlayingSounds = 0;
			int nUsedSoundIDs = 0;
			int nEntitySounds = 0;
			std::map<string, int> SoundEntity;


			TSoundIDSet::const_iterator ItEnd = m_currentSoundIDs.end();
			for (TSoundIDSet::const_iterator It=m_currentSoundIDs.begin(); It!=ItEnd; ++It)
			{			
				CSound* pSound = (CSound*)GetSound(*It);

				if (pSound)
				{
					++nUsedSoundIDs;

					if (pSound->m_SoundProxyEntityId != 0)
					{
						IEntity *pEntity = gEnv->pEntitySystem->GetEntity(pSound->m_SoundProxyEntityId);
						if (pEntity)
						{
							//pEntity->GetClass()->GetName()
							
							std::map<string, int>::iterator SPIt = SoundEntity.find(CONST_TEMP_STRING(pEntity->GetName()));
							
							if (SPIt == SoundEntity.end())
							{
								// insert new
								SoundEntity[pEntity->GetName()] = 1;
							}
							else
							{
								++SPIt->second;
								
							}
							++nEntitySounds;
						}
					}

					if (pSound->IsPlayingOnChannel())
						++nPlayingSounds;
				}
			}

			// renders default stuff in the top left corner
			float x = 20;
			float fColor[4]				={1.0f, 1.0f, 1.0f, 0.7f};
			float fColorRed[4]		={1.0f, 0.0f, 0.0f, 0.7f};
			float fColorYellow[4]	={1.0f, 1.0f, 0.0f, 0.7f};
			float fColorGreen[4]	={0.0f, 1.0f, 0.0f, 0.7f};
			float fColorOrange[4]	={1.0f, 0.5f, 0.0f, 0.7f};
			float fColorBlue[4]		={0.4f, 0.4f, 7.0f, 0.7f};

			pRenderer->Draw2dLabel(x, 2, 1.35f, fColor, false, "All: %.2f Sounds %.2f Main: %.2f / %.1f MB %s Cache, RSX: %.2f / %.1f MB, %s", (nSystemMem/1024)/1024.0f, (nSoundMemAll/1024)/1024.0f, (nSoundMemMain/1024)/1024.0f, (nSoundCacheSizeMain/1024)/1024.0f, bCacheStatic ? "static" : "", (nSoundMemRSX/1024)/1024.0f, fCacheRSXSize, bSpam ? "/ SPAM" : "" );
			pRenderer->Draw2dLabel(x, 13, 1.35f, fColor, false, "  Current Events: %d (Channels: %d), DSP: %3.1f, CPU: %3.1f", GetUsedVoices(), nChannels, fDSP, fUpdate);
			//pRenderer->Draw2dLabel(x, 20, 1.35f, fColor, false, "  Loaded SoundBuffers: %d (%d)", m_nBuffersLoaded, m_soundBuffers.size());
			pRenderer->Draw2dLabel(x, 24, 1.35f, fColor, false, "  SoundBuffers %d\tTrashedCount %d", m_pSoundAssetManager->GetNumberBuffersLoaded(), CSoundSystem::g_nBuffersTrashed);
			pRenderer->Draw2dLabel(x, 35, 1.35f, fColor, false, "  Groups Total: %d, Events Total: %d", nGroupCount, nEventCount);			
			pRenderer->Draw2dLabel(x, 46, 1.35f, fColor, false, "  Sounds Active: %d, Inactive: %d, Total: %d", (int)m_OneShotSounds.size()+(int)m_LoopedSounds.size(), (int)m_InactiveSounds.size(), nUsedSoundIDs);			

			//pRenderer->Draw2dLabel(x, 170, 1.5, fColor, false, "  Total size of vecSounds: %d, ", m_AllSoundsArray.size());		

			float ypos = 57;

			tVecListenersIterConst LIterEnd = m_vecListeners.end();
			for (tVecListenersIterConst LIter = m_vecListeners.begin(); LIter != LIterEnd; ++LIter)
			{
				IListener* pListener = (*LIter);
				bool bActive = pListener->GetActive();
				float fRecord = pListener->GetRecordLevel();
				float fColorListener[4];// ={0.0f, 1.0f, 0.0f, 0.7f};
				fColorListener[0] = bActive?fColorGreen[0]:fColorRed[0];
				fColorListener[1] = bActive?fColorGreen[1]:fColorRed[1];
				fColorListener[2] = bActive?fColorGreen[2]:fColorRed[2];
				fColorListener[3] = clamp(fRecord+0.3f, 0.0f, 1.0f);

				if ((*LIter)->bInside && (*LIter)->GetVisArea())
					pRenderer->Draw2dLabel(x, ypos, 1.35f, fColorListener, false, "  Listener %d INSIDE: Vis area=: %s", (*LIter)->GetID(), (*LIter)->GetVisArea()->GetName());			
				else
					pRenderer->Draw2dLabel(x, ypos, 1.35f, fColorListener, false, "  Listener %d OUTSIDE ", (*LIter)->GetID());	

				ypos += 11;
			}


			// soundinfo 1 shows only playing sound in the middle
			// soundinfo 2 adds inactive and active looping sounds to the left
			// soundinfo 3 adds oneshots sounds to the left
			// soundinfo 4 shows only "lost" sounds on the left
			// soundinfo 5 shows loaded event soundbuffers on the left
			// soundinfo 6 shows loaded soundbuffers on the left (dialog)
			// soundinfo 7 shows loaded wavebanks on the left
			// soundinfo 8 shows music debug
			// soundinfo 9 shows Sound Moods
			// soundinfo 10 shows Sound Moods values set within FMOD

			switch(g_nSoundInfo)
			{
			case 1 :
				{
					ypos += 5;
					if (m_pReverbManager)
						m_pReverbManager->DrawInformation(pRenderer, x, ypos);

					m_pAudioDevice->DrawInformation(pRenderer, 600, 120, g_nSoundInfo);

					ypos	= 120;
					pRenderer->Draw2dLabel(x, ypos, 1, fColor, false, "----- All Playing Sounds: %d -----", nPlayingSounds );
					ypos += 10;

					break;
				}
			case 2:
				{
					ypos	= 120;
					pRenderer->Draw2dLabel(x, ypos, 1, fColor, false, "----- Detailed Looping Sounds: %d -----", nPlayingSounds );
					ypos += 10;
					break;
				}
			case 3:
				{
					ypos	= 120;
					pRenderer->Draw2dLabel(x, ypos, 1, fColor, false, "----- Detailed OneShot Sounds: %d -----", nPlayingSounds );
					ypos += 10;
					break;
				}
			case 4:
				{
					ypos	= 120;
					pRenderer->Draw2dLabel(x, ypos, 1, fColor, false, "----- Lost Sounds: %d -----", nPlayingSounds );
					pRenderer->Draw2dLabel(x+600, ypos, 1, fColor, false, "----- Entity Sounds: %d -----", nEntitySounds );
					ypos += 10;
					break;
				}
			case 5 :
				ypos = 100;
				m_pSoundAssetManager->DrawInformation(pRenderer, x, ypos, amdiONLYEVENTBUFFERS);
				break;
			case 6 :
				ypos = 100;
				m_pSoundAssetManager->DrawInformation(pRenderer, x, ypos, amdiONLYSAMPLEBUFFERS);
				break;
			case 7 :
				ypos = 100;
				pRenderer->Draw2dLabel(x, ypos, 1, fColor, false, "----- Wavebanks ----- (set s_Profiling 1)" );
				ypos += 10;
				m_pAudioDevice->DrawInformation(pRenderer, x, ypos, g_nSoundInfo);
				break;
			case 8 :
				{
					ypos = 80;

					if (gEnv->pGame)
						if (gEnv->pGame->GetIGameFramework())
							if (gEnv->pGame->GetIGameFramework()->GetMusicLogic())
								gEnv->pGame->GetIGameFramework()->GetMusicLogic()->DrawInformation(pRenderer, x, ypos, g_nSoundInfo);

					gEnv->pMusicSystem->DrawInformation(pRenderer, x, ypos);

					break;
				}
			case 9 :
			case 10 :
				if (m_pMoodManager)
					m_pMoodManager->DrawInformation(pRenderer, x, ypos, g_nSoundInfo);
				break;

			default :
				break;
			} //switch

			float oldypos = ypos;

			// per sound soundinfo
			TSoundIDSet::const_iterator IterEnd = m_currentSoundIDs.end();
			for (TSoundIDSet::const_iterator It=m_currentSoundIDs.begin(); It!=IterEnd; ++It)
			{			
				CSound* pSound = (CSound*)GetSound(*It);

				if (pSound)
				{
					bool bIsStream = (pSound->GetFlags() & FLAG_SOUND_STREAM) != 0;

					// "lost" color is default
					float *pCurColor = fColorRed;
					bool bFound = false;

					if (m_InactiveSounds.Find(pSound))
					{
						pCurColor = fColor;
						bFound = true;
					}

					if (m_LoopedSounds.Find(pSound))
					{
						pCurColor = fColorYellow;
						bFound = true;
					}

					if (m_OneShotSounds.Find(pSound) && g_nSoundInfo > 2)
					{
						pCurColor = fColorGreen;
						bFound = true;
					}

					// static sounds in a soundproxy are orange
					if (pCurColor == fColorRed && !bFound)
					{
						// if the sound is playing anyway then its a static sound used by a sound proxy
						if (pSound->m_SoundProxyEntityId != 0)
						{
							pCurColor = fColorBlue;

							IEntity *pEntity = gEnv->pEntitySystem->GetEntity(pSound->m_SoundProxyEntityId);
							if (pEntity)
							{
								IEntitySoundProxy *pSoundProxy = (IEntitySoundProxy*)pEntity->GetProxy(ENTITY_PROXY_SOUND);
								if (pSoundProxy)
								{
									if (pSoundProxy->GetStaticSound(pSound->GetId()))
										pCurColor = fColorOrange;
								}
							}
							bFound = true;
						}
					}

					switch(g_nSoundInfo)
					{
					case 1 :
						{
								_smart_ptr<IPlatformSound> ps = pSound->GetPlatformSoundPtr();

							if (ps && !ps->GetSoundHandle())
								continue;

							if (!pSound->IsPlayingOnChannel())
								continue;

							pCurColor = fColor;

							if (pSound->GetFlags() & FLAG_SOUND_3D)
							{
								Vec3 SoundPosition = pSound->GetPosition();

								pRenderer->Draw2dLabel(x, ypos, 1.2f, bIsStream?fColorOrange:pCurColor, false, "3D %s",pSound->GetName());
							}
							else
							{
								pRenderer->Draw2dLabel(x, ypos, 1.2f, bIsStream?fColorOrange:pCurColor, false, "%s",pSound->GetName());
							}

							ypos += 10;
							break;
						} // 1
					case 2: case 3:
						{
							if (bFound && pCurColor != fColorRed && ( (g_nSoundInfo == 2 && pSound->GetLoopMode() ) || (g_nSoundInfo == 3 && !pSound->GetLoopMode() ) ) )
							{
								uint32 nSoundID = pSound->GetId();
								if (pSound->IsPlayingOnChannel())
									pRenderer->Draw2dLabel(5, ypos, 1.1f, fColorGreen, false, "[%d]", nSoundID);
								else //if (cs->IsActive())
									pRenderer->Draw2dLabel(5, ypos, 1.1f, fColor, false, "[%d]", nSoundID);

								float fSpread = 0.0f;
								pSound->GetParam("spread", &fSpread, false);
								Vec3 SoundPosition = pSound->GetPosition();

								char sText[512];
								size_t nTextLen = 0;

								sprintf(sText,"%s, %s",pSound->GetName(), (pSound->GetFlags()&FLAG_SOUND_3D)?"3D":"2D");

								if (true)
								{
									nTextLen = strlen(sText);
									sprintf(sText+nTextLen,", pos=%d, %d, %d",(int)(SoundPosition.x),(int)(SoundPosition.y),(int)(SoundPosition.z));
								}

								if (true)
								{
									nTextLen = strlen(sText);
									sprintf(sText+nTextLen,", vol=%1.2f", pSound->GetActiveVolume());
								}

								if (pSound->GetFlags() & FLAG_SOUND_RADIUS)
								{
									nTextLen = strlen(sText);
									sprintf(sText+nTextLen,", minR=%d, maxR=%d",(int)(pSound->GetMinRadius()),(int)(pSound->GetMaxDistance()));
								}

								if (pSound->GetFlags() & FLAG_SOUND_PARAM_SPREAD)
								{
									nTextLen = strlen(sText);
									sprintf(sText+nTextLen,", spread=%1.2f", fSpread);
								}

								if ((pSound->GetFlags() & FLAG_SOUND_OBSTRUCTION) && (pSound->GetFlags() & FLAG_SOUND_3D))
								{
									nTextLen = strlen(sText);
									sprintf(sText+nTextLen,", obstr=%1.2f", pSound->GetObstruction()->GetAccumulatedDirect());
								}

								// static sounds
								bool bIsStatic = false;
								if (GetISystem() && gEnv->pEntitySystem)
								{
									IEntity *pEntity = gEnv->pEntitySystem->GetEntity(pSound->m_SoundProxyEntityId);
									if (pEntity)
									{
										IEntitySoundProxy *pSoundProxy = (IEntitySoundProxy*)pEntity->GetProxy(ENTITY_PROXY_SOUND);
										if (pSoundProxy)
										{
											if (pSoundProxy->GetStaticSound(pSound->GetId()))
											{
												bIsStatic = true;
											}
										}
									}
								}

								if (pSound->m_SoundProxyEntityId)
								{
									size_t idLen = strlen(sText);
									sprintf(sText+idLen,", proxy=%d", pSound->m_SoundProxyEntityId);

									if (bIsStatic)
									{
										idLen = strlen(sText);
										sprintf(sText+idLen,", static=%d", bIsStatic);
										pCurColor = fColorOrange;

									}
								}

								if (bFound)	
								{
									pRenderer->Draw2dLabel(x+40, ypos, 1.1f, bIsStream?fColorOrange:pCurColor, false, "%s", sText);
									ypos += 10;
								}
							}
							break;
						} // 2
					case 4:
						{
							if (!bFound)
							{
								uint32 nSoundID = pSound->GetId();
								if (pSound->IsPlayingOnChannel())
									pRenderer->Draw2dLabel(5, ypos, 1, fColorGreen, false, "[%d]", nSoundID);
								else //if (cs->IsActive())
									pRenderer->Draw2dLabel(5, ypos, 1, fColor, false, "[%d]", nSoundID);


								if (pSound->GetFlags() & FLAG_SOUND_3D)
								{
									Vec3 SoundPosition = pSound->GetPosition();

									if (pSound->GetFlags() & FLAG_SOUND_VOICE)
										pRenderer->Draw2dLabel(x+40, ypos, 1, pCurColor, false, "Dialog: %s,3d, pos=%d, %d, %d, minR=%d, maxR=%d, vol=%1.2f", pSound->GetName(),(int)(SoundPosition.x),(int)(SoundPosition.y),(int)(SoundPosition.z),(int)(pSound->GetMinRadius()),(int)(pSound->GetMaxDistance()),pSound->GetActiveVolume() );
									else
										pRenderer->Draw2dLabel(x+40, ypos, 1, pCurColor, false, "%s,3d, pos=%d, %d, %d, minR=%d, maxR=%d, vol=%1.2f", pSound->GetName(),(int)(SoundPosition.x),(int)(SoundPosition.y),(int)(SoundPosition.z),(int)(pSound->GetMinRadius()),(int)(pSound->GetMaxDistance()),pSound->GetActiveVolume() );
								}
								else
								{
									if (pSound->GetFlags() & FLAG_SOUND_VOICE)
										pRenderer->Draw2dLabel(x+40, ypos, 1, pCurColor, false, "Dialog: %s,2d, volume=%1.2f", pSound->GetName(), pSound->GetActiveVolume() );
									else
										pRenderer->Draw2dLabel(x+40, ypos, 1, pCurColor, false, "%s,2d, volume=%1.2f", pSound->GetName(), pSound->GetActiveVolume() );
								}

								ypos += 10;
							}
							break;
						} // 4
					} // switch
				} // pSound
			} //it

			// Draw SoundEntites
			if (g_nSoundInfo == 4)
			{
				// Entity Sounds
				std::map<string, int>::iterator SPItEnd = SoundEntity.end();
				for (std::map<string, int>::iterator SPIt = SoundEntity.begin(); SPIt != SPItEnd; ++SPIt)
				{
					float *pCurColor = fColor;

					if (SPIt->second > 5)
						pCurColor = fColorOrange;
					
					if (SPIt->second > 10)
						pCurColor = fColorRed;
					
					pRenderer->Draw2dLabel(x+600, oldypos, 1, pCurColor, false, "%s [%d]", SPIt->first.c_str(), SPIt->second);
					oldypos += 10;
				}
			}
		}
	}
	// Drawing a red ball at the sound position and write the filename next to it
	if (g_nDrawSounds)
	{ 
		IRenderer *pRenderer = gEnv->pRenderer;
		if (pRenderer)
		{
			IRenderAuxGeom* pAuxGeom( pRenderer->GetIRenderAuxGeom() );
			pAuxGeom->SetRenderFlags( e_Def3DPublicRenderflags );
			float colorwhite[4]={1,1,1,1};

			// draw a green ball where a listener is
			tVecListenersIterConst LIterEnd = m_vecListeners.end();
			for (tVecListenersIterConst LIter = m_vecListeners.begin(); LIter != LIterEnd; ++LIter)
			{
				if ((*LIter)->bActive)
					pAuxGeom->DrawSphere( (*LIter)->GetPosition(), 0.2f, ColorB( 10, 255, 10, 255 ) );	
				else
					pAuxGeom->DrawSphere( (*LIter)->GetPosition(), 0.2f, ColorB( 10, 10, 255, 255 ) );	
				pRenderer->DrawLabelEx((*LIter)->GetPosition(), 1.3f, colorwhite, true, false, "Listener: %d RecordLevel:%1.2f", (*LIter)->nListenerID, (*LIter)->fRecordLevel);
			}

			TSoundIDSet::const_iterator ItEnd = m_currentSoundIDs.end();
			for (TSoundIDSet::const_iterator It=m_currentSoundIDs.begin(); It!=ItEnd; ++It)
			{			
				CSound* pSound = (CSound*)GetSound(*It);

				if (pSound)
				{
					_smart_ptr<IPlatformSound> pPlatformSound = pSound->GetPlatformSoundPtr();

					float fSoundAmplitude = 0.0f;
					int nSoundHandle = 0;
					if (pPlatformSound)
					{
						ptParamF32 TempParam(fSoundAmplitude);
						pPlatformSound->GetParamByType(pspAMPLITUDE, &TempParam);
						TempParam.GetValue(fSoundAmplitude);
						nSoundHandle = (int)(INT_PTR)pPlatformSound->GetSoundHandle();
					}


					Vec3 vPos = pSound->GetPlaybackPosition();
					Vec3 vListenerPos;

					// Joes request to not draw sounds right in front of the face
					IListener* pListener = GetListener(LISTENERID_STANDARD);
					if (pListener)
					{
						vListenerPos = pListener->GetPosition();
						if (vPos.GetSquaredDistance(vListenerPos) > 1.5f)
						{
							bool bDrawSound = false;

							if (g_nDrawSounds == 1)
							{
								if (pSound->IsPlayingOnChannel())
								{
									bDrawSound = true;
									pRenderer->DrawLabelEx(vPos,1.3f,colorwhite, true, false, "%s Vol. on Channel:%1.2f", pSound->GetName(), pSound->GetActiveVolume());
								}
							} 
							if (g_nDrawSounds == 2)
							{
								if (pSound->IsPlayingOnChannel())
								{
									bDrawSound = true;
									//float colorwhite[4]={1,1,1,1};
									pRenderer->DrawLabelEx(vPos,1.3f,colorwhite, true, false, "Channel: %d static Vol.:%1.2f, Vol. on Channel:%1.2f, Sound-ID: %d",nSoundHandle, pSound->GetActiveVolume(), fSoundAmplitude, pSound->GetId());
								}
							} 
							if (g_nDrawSounds == 3)
							{
								if (pSound->IsPlayingOnChannel())
								{
									bDrawSound = true;
									//float colorwhite[4]={1,1,1,1};

									float fDirect = 0.0f;
									float fReverb = 0.0f;
									if (pSound->GetObstruction())
									{
										fDirect = pSound->GetObstruction()->GetAccumulatedDirect();
										fReverb = pSound->GetObstruction()->GetAccumulatedReverb();
									}

									pRenderer->DrawLabelEx(vPos,1.3f,colorwhite, true, false, "%s\n static Vol.:%1.2f, Vol. on Channel:%1.2f\n Channel: %d ID: %d\n DirectOcc.: %f ReverbOcc.: %f",pSound->GetName(),pSound->GetActiveVolume(), fSoundAmplitude, nSoundHandle, pSound->GetId(), fDirect, fReverb);
								}
							} 
							if (g_nDrawSounds == 4)
							{
								bDrawSound = true;
								//float colorwhite[4]={1,1,1,1};
								pRenderer->DrawLabelEx(pSound->GetPlaybackPosition(),1.3f,colorwhite, true, false, "%s\n static Vol.:%1.2f, Vol. on Channel:%1.2f, \n Channel: %d Sound-ID: %d",pSound->GetName(),pSound->GetActiveVolume(), fSoundAmplitude, nSoundHandle, pSound->GetId());
							} 

							Vec3 vStart, vEnd;
							if (pSound->GetLineSpec(vStart, vEnd))
							{
								ColorB linecol = ColorB( 200, 10, 10, 200 );
								pAuxGeom->DrawLine(vStart, linecol, vEnd, linecol, 2.0f);
							}

							if (bDrawSound)
							{
								float fInner, fOuter;
								pSound->GetConeAngles(fInner, fOuter);
								float innerradius = sinf(fInner*0.5f*3.1415f/180.0f);
								float innerheight = cosf(fInner*0.5f*3.1415f/180.0f);
								//float outerradius = sin(fOuter*0.5f*3.1415/180.0f);
								//float outerheight = cos(fOuter*0.5f*3.1415/180.0f);

								if (innerradius < 0 || innerheight < 0)
									pAuxGeom->DrawSphere( vPos, 0.2f, ColorB( 255, 10, 10, 255 ) );
								else
								{
									//pAuxGeom->DrawCone(vPos, pSound->GetDirection(), outerradius*0.2f, outerheight*0.2f, ColorB( 10, 255, 10, 255 ));
									//pAuxGeom->DrawCone(vPos+pSound->GetDirection()*innerheight*0.2f, -pSound->GetDirection(), innerradius*0.2f, innerheight*0.2f+0.2f, ColorB( 255, 255, 10, 255 ));
									pAuxGeom->DrawCone(vPos+pSound->GetForward()*innerheight*0.2f, -pSound->GetForward(), innerradius*0.2f, innerheight*0.2f, ColorB( 255, 255, 10, 255 ));
								}
							}
						}
					}
				}
			}
		}
	}

	// Drawing a blue radius ball at the sound position and show obstruction test rays
	if (g_nDrawObstruction)
	{ 
		IRenderer *pRenderer = gEnv->pRenderer;
		if (pRenderer)
		{
			IRenderAuxGeom* pAuxGeom( pRenderer->GetIRenderAuxGeom() );
			pAuxGeom->SetRenderFlags( e_Def3DPublicRenderflags|e_AlphaBlended );
			
			float colorwhite[4]={1,1,1,1};
			Vec3	vListener;

			// draw a green ball where a listener is
			tVecListenersIterConst LIterEnd = m_vecListeners.end();
			for (tVecListenersIterConst LIter = m_vecListeners.begin(); LIter != LIterEnd; ++LIter)
			{
				if ((*LIter)->bActive)
				{
					pAuxGeom->DrawSphere( (*LIter)->GetPosition(), 0.2f, ColorB( 10, 255, 10, 255 ) );	
					vListener = (*LIter)->GetPosition();
				}
				else
					pAuxGeom->DrawSphere( (*LIter)->GetPosition(), 0.2f, ColorB( 10, 10, 255, 255 ) );	
				pRenderer->DrawLabelEx((*LIter)->GetPosition(), 1.3f, colorwhite, true, false, "Listener: %d RecordLevel:%1.2f", (*LIter)->nListenerID, (*LIter)->fRecordLevel);
			}

			TSoundIDSet::const_iterator ItEnd = m_currentSoundIDs.end();
			for (TSoundIDSet::const_iterator It=m_currentSoundIDs.begin(); It!=ItEnd; ++It)
			{			
				CSound* pSound = (CSound*)GetSound(*It);

				if (!pSound)
					continue;
					
				if ((pSound->GetFlags() & FLAG_SOUND_OBSTRUCTION) && (pSound->GetFlags() & FLAG_SOUND_3D) && !(pSound->GetFlags() & FLAG_SOUND_2D))
				{
					_smart_ptr<IPlatformSound> pPlatformSound = pSound->GetPlatformSoundPtr();
					SObstruction* pObstruction = pSound->GetObstruction();
					if (pObstruction)
					{
						if (pObstruction->nRaysShot==0)
							continue;

						//if (!pSound->IsPlaying())
						//	continue;
						
						Vec3 vPos;
						if (pPlatformSound)
						{
							ptParamVEC3F32 TempPos(vPos);
							pPlatformSound->GetParamByType(psp3DPOSITION, &TempPos);
							TempPos.GetValue(vPos);
						}

						vListener.z -= 0.1f;
						float fRadius = sqrtf(pSound->GetMaxDistance());

						pAuxGeom->DrawSphere( pSound->GetPlaybackPosition(), fRadius, ColorF( 0.1f, pObstruction->GetDirect(), pObstruction->GetReverb(), 0.1f+pObstruction->GetDirect()+pObstruction->GetReverb() ) );
						pRenderer->DrawLabelEx(vPos,1.3f,colorwhite, true, false, "DirectOcc :%f \nReverbOcc :%f", pObstruction->GetDirect(), pObstruction->GetReverb());

						for (int i=0; i<pObstruction->nRaysShot; ++i)
						{
							ColorB linecol = ColorB( 200, 10, 10, 200 );

							if (pObstruction->ObstructionTests[i].nHits)
								linecol = ColorB( 200, 10, 10, 200 );
							else
								linecol = ColorB( 200, 200, 10, 200 );

							if (pObstruction->ObstructionTests[i].nTestForTest)
							{
								if (pObstruction->ObstructionTests[i].nHits)
									linecol = ColorB( 100, 100, 100, 200 );
								else
									linecol = ColorB( 200, 200, 200, 200 );
							}

							pAuxGeom->DrawLine(pObstruction->ObstructionTests[i].vOrigin, linecol, pObstruction->ObstructionTests[i].vOrigin+pObstruction->ObstructionTests[i].vDirection, linecol, 10.0f);

						}
					}
				}
			}
		}
	}

	// Draw DynamicReverb debug info
	if (g_fReverbDynamic && g_nReverbDebugDraw && m_pReverbManager)
	{
		IRenderer* const pRenderer = gEnv->pRenderer;
		if (pRenderer)
			m_pReverbManager->DrawDynamicReverbDebugInfo(pRenderer);
	}
}

// accesses SoundInfos
ISoundProfileInfo* CSoundSystem::GetSoundInfo(int nIndex)
{
	ISoundProfileInfo *pSoundInfo = NULL;

	int nIdx = 0;
	tmapSoundInfo::iterator ItEnd = m_SoundInfos.end();
	for (tmapSoundInfo::const_iterator It = m_SoundInfos.begin(); It!=ItEnd; ++It)
	{
		if (nIndex == nIdx)
			pSoundInfo = (*It).second;
		++nIdx;
	}

	return pSoundInfo;
}

// accesses SoundInfos
ISoundProfileInfo* CSoundSystem::GetSoundInfo(const char* sSoundName)
{
	ISoundProfileInfo *pSoundInfo = NULL;

	tmapSoundInfo::iterator It = m_SoundInfos.find(CONST_TEMP_STRING(sSoundName));
	if (It != m_SoundInfos.end())
	{
		pSoundInfo = (*It).second;
	}

	return pSoundInfo;
}

void CSoundSystem::SetMinMaxRadius(CSound &Sound, float fMin, float fMax)
{
	Sound.SetMinMaxDistance_i(fMin, fMax);
}

//------------------------------------------------------------------------
void CSoundSystem::AddEventListener(ISoundSystemEventListener *pListener, bool bOnlyVoiceSounds)
{
	stl::push_back_unique(m_EventListeners, SEventListener(pListener, bOnlyVoiceSounds));
}

//------------------------------------------------------------------------
void CSoundSystem::RemoveEventListener(ISoundSystemEventListener *pListener)
{
	stl::find_and_erase(m_EventListeners, pListener);
}

//////////////////////////////////////////////////////////////////////////
void CSoundSystem::OnEvent( ESoundSystemCallbackEvent event, ISound *pSound )
{
	if (!pSound) // play safe, avoid compiler warning
		return;

	// send the event to listeners
	if (!m_EventListeners.empty())
	{
		m_EventListenersTemp = m_EventListeners;

		TSoundSystenEventListenerVector::iterator ItEnd = m_EventListenersTemp.end();
		for (TSoundSystenEventListenerVector::iterator It = m_EventListenersTemp.begin(); It != ItEnd; ++It)
		{
			const SEventListener& entry = *It;
			assert (entry.pListener);
			if (!entry.bOnlyVoiceSounds || (pSound && (pSound->GetFlags()&FLAG_SOUND_VOICE)))
				entry.pListener->OnSoundSystemEvent( event, pSound );
		}
	}

//	CSound* pCSound = (CSound*)pSound;
	
	switch(event)
	{
	case SOUNDSYSTEM_EVENT_ON_START:
		break;
	case SOUNDSYSTEM_EVENT_ON_PLAYBACK_STARTED:
		break;
	case SOUNDSYSTEM_EVENT_ON_PLAYBACK_UNPAUSED:
		break;
	case SOUNDSYSTEM_EVENT_ON_STOP:
		break;
	case SOUNDSYSTEM_EVENT_ON_PLAYBACK_PAUSED:
	    break;
	case SOUNDSYSTEM_EVENT_ON_PLAYBACK_STOPPED:
		break;
	default:
	    break;
	}
}

//////////////////////////////////////////////////////////////////////////
void CSoundSystem::OnSystemEvent( ESystemEvent event,UINT_PTR wparam,UINT_PTR lparam )
{
	switch (event)
	{
	case ESYSTEM_EVENT_LEVEL_POST_UNLOAD:
		{
			//gEnv->pMusicSystem->m_musicStateNode = 0;
			break;
	}
	case ESYSTEM_EVENT_LEVEL_LOAD_START:
		m_bDelayPrecaching = true;
		Silence(true, true);
		g_nBuffersTrashed = 0;
		break;
	case ESYSTEM_EVENT_LEVEL_LOAD_END:
		{
			if (g_nPrecacheData)
			{
				// turn non-blocking loading of Projects off so that the precaching of sounds actually work
				int nOldCVarValueProject = g_StreamProjectFiles;
				g_StreamProjectFiles = 0;

				m_bDelayPrecaching = false;
				int nNum = m_DelayedPrecaches.size();
				int nOldLoadNonBlocking = g_nLoadNonBlocking;
				g_nLoadNonBlocking = 0;
				int nErrors = 0;

				SoundBufferPropsMapItorConst ItEnd = m_DelayedPrecaches.end();
				for (SoundBufferPropsMapItorConst It=m_DelayedPrecaches.begin(); It!=ItEnd; ++It)
				{			
					CSoundBuffer *pBuf = m_pSoundAssetManager->GetOrCreateSoundBuffer(It->first, NULL);

					if (pBuf && (It->first.nPrecacheFlags & FLAG_SOUND_PRECACHE_LOAD_SOUND || It->first.nPrecacheFlags & FLAG_SOUND_PRECACHE_LOAD_GROUP ))
						pBuf->Load(NULL);

					if (gEnv->pSystem->IsDevMode() || gEnv->pCryPak->GetLvlResStatus())
					{
						// temporarily remove sound obstruction so playback is not delayed
						uint32 nFlags = It->first.nFlags;
						nFlags &= ~FLAG_SOUND_OBSTRUCTION;

						_smart_ptr<ISound> pSound = LoadSound(It->first.sName.c_str(), nFlags, It->first.nPrecacheFlags);

						if (pSound)
						{
							pSound->SetSemantic(eSoundSemantic_Sandbox);
							pSound->GetInterfaceExtended()->SetVolume(0.0f);
							pSound->Play();
							pSound->Stop(ESoundStopMode_AtOnce);
						}
						else
						{
							Log(eSLT_Warning, "<Sound> Create sound %s failed", It->first.sName.c_str());
							++nErrors;
						}
					}
				}

				if (nErrors)
					Log(eSLT_Error, "%d Sound errors occurred, check log files!", nErrors);


				g_nLoadNonBlocking = nOldLoadNonBlocking;
				g_StreamProjectFiles = nOldCVarValueProject;
			}

			m_DelayedPrecaches.clear();

			break;
		}
	case ESYSTEM_EVENT_CHANGE_FOCUS:
		{
			// unless we have NetworkAudition pause all audio on focus loss
			if (!g_nNetworkAudition)
			{
				// wparam != 0 is focused, wparam == 0 is not focused
				bool bPause = (wparam == 0); // pause on no focus
				ptParamBOOL bParam(bPause);
				m_pAudioDevice->SetParam(adpMASTER_PAUSE, &bParam);
			}

			break;
		}
	}
}

void CSoundSystem::Serialize( TSerialize ser )
{
	// cannot change m_lBaseTime, as this is used for async time (which shouldn't be affected by save games)
	if (ser.IsWriting())
	{
		//ser.Value("curTime", m_lCurrentTime);
	}
	else
	{
		//ser.Value("curTime", curTime);
	}

	// Serialize other modules
	
	if (m_pMoodManager)
	{
		m_pMoodManager->SerializeState(ser);
	}

	if (m_pReverbManager)
	{
		m_pReverbManager->SerializeState(ser);
	}

}

// workaround for looping weapon sound MP bug
bool CSoundSystem::IsEventUsedInPlatformSound (void* pHandle)
{
	SSoundContainer::iterator It;
	SSoundContainer::const_iterator itend = m_LoopedSounds.end();

	// check all looping sounds
	for (It=m_LoopedSounds.begin(); It!=itend; ++It)
	{
		_smart_ptr<CSound> pSound = *It;

		if (pSound->GetFlags() & FLAG_SOUND_EVENT)
		{
			IPlatformSound* pPlatformSound = pSound->GetPlatformSoundPtr();

			if (pPlatformSound)
			{
				tSoundHandle pSoundHandle = pPlatformSound->GetSoundHandle();
				if (pSoundHandle)
				{
					if (pHandle == pSoundHandle)
						return true;
				}
			}
		}
	}

	// check also dialog sounds in oneshots as they are based on looping sound definitions
	itend = m_OneShotSounds.end();
	for (It=m_OneShotSounds.begin(); It!=itend; ++It)
	{
		_smart_ptr<CSound> pSound = *It;

		if ((pSound->GetFlags() & FLAG_SOUND_EVENT) && (pSound->GetFlags() & FLAG_SOUND_VOICE))
		{
			IPlatformSound* pPlatformSound = pSound->GetPlatformSoundPtr();

			if (pPlatformSound)
			{
				tSoundHandle pSoundHandle = pPlatformSound->GetSoundHandle();
				if (pSoundHandle)
				{
					if (pHandle == pSoundHandle)
						return true;
				}
			}
		}
	}
	return false;
}

// returns true if called from the main thread for this engine
bool CSoundSystem::IsMainThread() 
{
	return CryGetCurrentThreadId() == m_dwMainThreadId;
}

//#endif //#ifndef _XBOX

#include UNIQUE_VIRTUAL_WRAPPER(ISoundProfileInfo)
