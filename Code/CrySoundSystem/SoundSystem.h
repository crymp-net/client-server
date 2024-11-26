//////////////////////////////////////////////////////////////////////
//
//  Crytek (C) 2001
//
//  CrySound Source Code
//
//  File: SoundSystem.h
//  Description: Sound system interface.
//
//  History:
//	-June 06,2001:Implemented by Marco Corbetta
//  -June 01,2005:Updated by NickV added code for group manager and xml reading routines
//
//////////////////////////////////////////////////////////////////////

#ifndef CRYSOUND_SOUNDSYSTEM_H
#define CRYSOUND_SOUNDSYSTEM_H

#include "SoundSystemCommon.h"
//#include "Sound.h"
#include "SoundAssetManager.h"
#include "STLPoolAllocator.h"
#include "IPlatformSound.h"
#include "ISoundAssetManager.h"

#include"../CryEntitySystem/SaltBufferArray.h"         // SaltBufferArray<>
#include "CryArray.h"

#include "DebugLogger.h"

// forward declaration
struct ISoundAssetManager;


//////////////////////////////////////////////////////////////////////////////////////////////
class		CSoundAssetManager;
class		CSound;
struct  ISoundMoodManager;
struct	ICVar;
struct	SoundBuffer;
struct	IStreamEngine;
struct	SSGHandle;
class   CDebugLogger;

#define MAX_ACTIVE_SOUNDSPOTS		16
#define MAX_SOUNDIDS						4096

#define SETSCALEBIT(bit) (1<<bit)

#define CSSPEAKERCONFIG_5POINT1			0x00000001
#define CSSPEAKERCONFIG_HEADPHONE		0x00000002
#define CSSPEAKERCONFIG_MONO				0x00000003
#define CSSPEAKERCONFIG_QUAD				0x00000004
#define CSSPEAKERCONFIG_STEREO			0x00000005
#define CSSPEAKERCONFIG_SURROUND		0x00000006

#define SFXFILTER_PRIORITY					CS_DSP_DEFAULTPRIORITY_SFXUNIT+1

// ParseSoundNameReturnCodes
#define PSNRC_EVENT						0x00000001
#define PSNRC_GROUP						0x00000002
#define PSNRC_WAV							0x00000004
#define PSNRC_CONVERTEDTOWAV	0x00000008
#define PSNRC_VOICE						0x00000010
#define PSNRC_KEY							0x00000020

enum EDebuggingModes
{
	SOUNDSYSTEM_DEBUG_NONE,						//!< no debugging output
	SOUNDSYSTEM_DEBUG_SIMPLE,					//!< simple debugging to console
	SOUNDSYSTEM_DEBUG_DETAIL,					//!< more detail debugging to console
	SOUNDSYSTEM_DEBUG_EVENTLISTENERS, //!< logs event listeners
	SOUNDSYSTEM_DEBUG_SYNCPOINTS,			//!< logs syncpoints
	SOUNDSYSTEM_DEBUG_MEMORY,					//!< simple Memory debugging
	SOUNDSYSTEM_DEBUG_FMOD_SIMPLE,		//!< simple FMOD debugging into fmod.log
	SOUNDSYSTEM_DEBUG_FMOD_COMPLEX,		//!< complex FMOD debugging into fmod.log
	SOUNDSYSTEM_DEBUG_FMOD_ALL,				//!< all FMOD debugging into fmod.log
	SOUNDSYSTEM_DEBUG_RECORD_COMMANDS,//!< records FMOD calls in SoundCommands.xml
	SOUNDSYSTEM_DEBUG_PROGRAMMERSOUND	//!< logs information on Voice files which have programmersounds
};



//////////////////////////////////////////////////////////////////////////////////////////////

struct SSoundContainer : public std::vector<_smart_ptr<CSound> >
{

public: 
	SSoundContainer () {};

	void Push(CSound* pSound)
	{
		push_back(pSound);
	}

	void AddUnique(CSound* pSound)
	{
		if (!Find(pSound))
			Push(pSound);
	}

	bool EraseUnique(CSound* pSound)
	{
		iterator ItEnd = end();
		for (iterator Iter = begin(); Iter != ItEnd; ++Iter)
		{
			CSound* pContainerSound = (*Iter);
			if (pSound == pContainerSound)
			{
				iterator ItLast = end()-1;
				if (Iter != ItLast)
					(*Iter) = (*ItLast);

				pop_back();
				return true;
			}
		}
		return false;
	}

	bool Find(const CSound* pSound)
	{
		iterator ItEnd = end();
		for (iterator Iter = begin(); Iter != ItEnd; ++Iter)
		{
			CSound* pContainerSound = (*Iter);
			if (pSound == pContainerSound)
				return true;
		}
		return false;
	}

	void GetMemoryUsage( ICrySizer *pSizer ) const
	{
		pSizer->AddObject(static_cast< const std::vector<_smart_ptr<CSound> >&>(*this));		
	}
private:

};


class CSpamFilter
{
private:
	typedef std::map<CryFixedStringT<MAX_WARNING_LENGTH>, float> TSpamMap;
	TSpamMap	m_SpamMap;
	float			m_fSpamDetectedTimeout;

public:

	CSpamFilter()
	{
		m_fSpamDetectedTimeout = 0.0f;
	}

	void Insert(const char* sEntry, float fTimeout)
	{
		// include into SpamMap with fTimeout sec timeout
		m_SpamMap[sEntry] = fTimeout;
	}

	bool IsSpam(const char* sEntry)
	{
		// check if entry is known
		TSpamMap::const_iterator It = m_SpamMap.find(sEntry);
		if (It != m_SpamMap.end())
		{
			m_fSpamDetectedTimeout = 5.0f;
			return true;
		}

		return false;
	}

	void Update(float fTimeSpent)
	{
		// update detection timeout
		m_fSpamDetectedTimeout = max(0.0f, m_fSpamDetectedTimeout - fTimeSpent);


		// update Spam timeouts
		for (TSpamMap::iterator SpamIt = m_SpamMap.begin(); SpamIt!=m_SpamMap.end(); )
		{
			SpamIt->second -= fTimeSpent;

			if (SpamIt->second < 0.0f)
			{
				m_SpamMap.erase(SpamIt++);
			}
			else
			{
				++SpamIt;
			}
		}
	}

	bool WasThereSpam()
	{
		return (m_fSpamDetectedTimeout > 0.0f);
	}
};

// global parameter
struct SParameter
{
public: 
	SParameter() : nNameIndex(-1), fValue(0.0f), bIsGlobal(false), bChanged(false){};
	SParameter(const int _nNameIndex, const float _fValue, const bool _bIsGlobal, const bool _bChanged)
	{
		nNameIndex = _nNameIndex;
		fValue = _fValue;
		bIsGlobal = _bIsGlobal;
		bChanged = _bChanged;
	}

	int   nNameIndex;
	float fValue;
	bool  bIsGlobal;	
	bool  bChanged;
};

// queued parameter
typedef std::map<string, SParameter>				tmapParameterNames;
typedef tmapParameterNames::iterator				tmapParameterNamesIter;
typedef tmapParameterNames::const_iterator	tmapParameterNamesIterConst;

struct SParameterNamesContainer : public tmapParameterNames
{
public: 
	SParameterNamesContainer () {};

	bool AddOrSetParameter(const char* sParameterName, const bool bIsGlobal, const float fValue, int &nIndex)
	{
		bool bNewEntry = false;

		tmapParameterNamesIter It = find(CONST_TEMP_STRING(sParameterName));

		if (It == end())
		{
			nIndex = size();
			SParameter NewParam(nIndex, fValue, bIsGlobal, false);
			insert(std::pair<string, SParameter>(sParameterName, NewParam));
			bNewEntry = true;
		}
		else
		{
			nIndex = It->second.nNameIndex;
			if (bIsGlobal)
			{
				It->second.fValue = fValue;
				It->second.bIsGlobal = true;
			}
		}	

		return bNewEntry;
	}

	void ApplyGlobalParameters(IPlatformSound *pPlatformSound)
	{
		tmapParameterNamesIterConst ItEnd = end();
		for (tmapParameterNamesIterConst It = begin(); It != ItEnd; ++It)
		{
			if (It->second.bIsGlobal)
			{
				int nEventIndex = pPlatformSound->GetEventIndexByNameIndex(It->second.nNameIndex);
				pPlatformSound->SetParamByIndex(nEventIndex, It->second.fValue, false);
			}
		}
	}

	char const* GetNameByIndex(unsigned int const nIndex)
	{
		tmapParameterNamesIterConst ItEnd = end();
		for(tmapParameterNamesIterConst It = begin(); It != ItEnd; ++It)
		{
			if(It->second.nNameIndex == nIndex)
				return It->first.c_str();
		}

		return 0;
	}

};

/*
struct SoundName
{
	SoundName(const char *_name,bool _b3d)
	{
		name=_name;
		b3d=_b3d;
	}
	SoundName(const SoundName& o)
	{
		name=o.name;
		b3d=o.b3d;
	}
	bool operator<(const SoundName& o)const
	{
		if(b3d!=o.b3d)
		{
			if (b3d)
				return true;
			return false;
		}
		return (stricmp(name.c_str(),o.name.c_str())<0);
	}
	string name;
	bool b3d;
};*/

class CListener : public IListener
{
private:
	Matrix34	  TransformationNewest;
	Matrix34	  TransformationSet;

	float				fUnderwater;
	IVisArea*		pVisArea;
	Vec3				vVelocity;



	CListener(const CListener&) {}

public:
	
	ListenerID	nListenerID;	
	EntityId    nEntityID;
	PodArray<IVisArea*>* pVisAreas;
	float				fRecordLevel;	
	bool				bActive;	
	bool				bMoved;
	bool				bRotated;
	bool				bDirty;
	bool				bInside;


	// Constructor
	CListener() 
	{
		nListenerID		= LISTENERID_INVALID;
		vVelocity			= Vec3(0,0,0);
		fRecordLevel	= 0.0f;
		bActive				= false;
		bMoved				= false;
		bRotated			= false;
		bDirty				= true;
		bInside				= false;
		fUnderwater   = 1.0f;
		nEntityID     = 0;
		pVisArea			= NULL;
		TransformationSet.SetIdentity();
		TransformationNewest.SetIdentity();

		pVisAreas			= new (PodArray<IVisArea*>);
		//VisAreas.PreAllocate(MAX_VIS_AREAS);
	}
	~CListener()
	{
		if (pVisAreas)
		{
			pVisAreas->clear();
			delete pVisAreas;
		}
	}

	virtual ListenerID GetID() const
	{
		return nListenerID;
	}

	virtual EntityId GetEntityID() const
	{
		return nEntityID;
	}

	virtual bool GetActive() const
	{
		return bActive;
	}
	
	virtual void SetActive(bool bNewActive)
	{
		bActive = bNewActive;
	}

	virtual void SetRecordLevel(float fRecord)
	{
		fRecordLevel = fRecord;
	}

	virtual float GetRecordLevel()
	{
		return fRecordLevel;
	}

	virtual Vec3 GetPosition() const
	{
		return TransformationSet.GetColumn3();
	}

	virtual void SetPosition(const Vec3 Position)
	{
		TransformationNewest.SetColumn(3, Position);
		bDirty = true;
	}

	virtual Vec3 GetForward() const
	{ 
		Vec3 ForwardVec(0,1,0); // Forward.
		ForwardVec = TransformationSet.TransformVector(ForwardVec);
		ForwardVec.Normalize();
		return ForwardVec;
	}

	virtual Vec3 GetTop() const
	{
		Vec3 TopVec(0,0,1); // Up.
		TopVec = TransformationSet.TransformVector(TopVec);
		TopVec.Normalize();
		return TopVec;
	}

	virtual Vec3 GetVelocity() const
	{
		return vVelocity;
	}

	virtual void SetVelocity(Vec3 vVel)
	{
		vVelocity = vVel;
	}

	virtual void SetMatrix(const Matrix34 newTransformation)
	{
		Vec3 vOldPos = TransformationSet.GetColumn3();
		Vec3 vNewPos = newTransformation.GetColumn3();
		bMoved = !vOldPos.IsEquivalent(vNewPos, 0.01f);
		bRotated = !TransformationSet.IsEquivalent(newTransformation, 0.01f);

		if (bMoved || bRotated)
		{
			TransformationNewest = newTransformation;
			bDirty = true;
		}

	}

	virtual Matrix34 GetMatrix() const
	{
		return TransformationSet;
	}

	virtual float GetUnderwater() const
	{
		return fUnderwater;
	}

	virtual void SetUnderwater(const float fUnder)
	{
		fUnderwater = fUnder;
	}

	virtual IVisArea* GetVisArea() const
	{
		return pVisArea;
	}

	virtual void SetVisArea(IVisArea* pVArea)
	{
		pVisArea = pVArea;
	}

	void MarkAsSet()
	{
		bDirty = false;
		TransformationSet = TransformationNewest;
	}
};

typedef std::vector<CSound*>	SoundVec;
typedef SoundVec::iterator		SoundVecIter;

typedef std::vector<IMicrophone* >	MicroVec;
typedef MicroVec::iterator					MicroVecIter;
typedef MicroVec::const_iterator		MicroVecIterConst;

typedef std::vector<CListener*>				tVecListeners;
typedef tVecListeners::iterator				tVecListenersIter;
typedef tVecListeners::const_iterator	tVecListenersIterConst;

//#ifdef WIN64
//#undef PlaySound
//#endif
//////////////////////////////////////////////////////////////////////////////////////////////
// Sound system interface
class CSoundSystem : public CSoundSystemCommon, public ISystemEventListener //, INetworkSoundListener, IMicrophoneStream
{
	friend class CSound;
	friend class CAudioDeviceFmodEx400;
	friend class CPlatformSoundFmodEx400Event;

protected:
	virtual ~CSoundSystem();

public:
	CSoundSystem(void* hWnd, IAudioDevice *pAudioDevice);

	//////////////////////////////////////////////////////////////////////////
	// ISOUND INTERFACE
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// CORE FUNCTIONALITY
	//////////////////////////////////////////////////////////////////////////

	//!	Update the sound system
	void Update(ESoundUpdateMode UpdateMode);
	
	// for serialization
	virtual void Serialize(TSerialize ser);

	virtual void Log( const ESoundLogType eType, const char *szFormat,... );

	// Summary:
	//	 Queries the SoundSystem for the last thrown errors, eg. after getting a NULL pointer from CreateSound
	virtual ESoundSystemErrorCode GetError() 
	{ 
		ESoundSystemErrorCode TempErrorCode = m_ErrorCode;
		m_ErrorCode = eSoundSystemErrorCode_None; 
		return TempErrorCode; 
	}

	//!	Release the sound system
	void Release();

	// Register listener to the sound.
	virtual void AddEventListener( ISoundSystemEventListener *pListener, bool bOnlyVoiceSounds );
	virtual void RemoveEventListener( ISoundSystemEventListener *pListener );

	void LockResources();
	void UnlockResources();

	// Summary:
	//   Sets a global parameter value to all events which have a parameter with this name
	virtual void SetGlobalParameter( const char *sGlobalParameter, const float fValue);

	//////////////////////////////////////////////////////////////////////////
	// SOUND CREATION & HANDLING
	//////////////////////////////////////////////////////////////////////////

	/*! Creates a sound object by combining the definition of the sound and the buffer of sound data
	@param sGroupAndSoundName name of group (with /path/) and name of sound split by ":"
	@param int Sound Flags
	@return ISound* For now this will return a Ptr to a Sound, this will change to the unique Sound ID later
	*/	
	virtual ISound* CreateSound				( const char *sGroupAndSoundName, uint32 nFlags );
	virtual ISound* CreateLineSound		( const char *sGroupAndSoundName, uint32 nFlags, const Vec3 &vStart, const Vec3 &VEnd );
	virtual ISound* CreateSphereSound	( const char *sGroupAndSoundName, uint32 nFlags, const float fRadius );

	/*! Get a sound interface from the sound system
	@param nSoundId sound id
	*/	
	ISound* GetSound(tSoundID nSoundID) const;

	/*! Precaches the buffer(s) of a group, subgroup or sound 
	@param sGroupAndSoundName name of group (with /path/) and name of sound split by ":"
	@param nSoundFlags some Flags that can be set, needs to be defined yet
	@param nPrecacheFlags some Flags that can be set to change precaching behaviour
	@return EPrecacheResult result code
	*/	
	EPrecacheResult Precache( const char *sGroupAndSoundName, uint32 nSoundFlags, uint32 nPrecacheFlags );

	//////////////////////////////////////////////////////////////////////////
	// VOLUME CONTROL
	//////////////////////////////////////////////////////////////////////////

	/*! SetMasterVolume
	@param fVol volume (0.0f - 1.0f)
	*/		
	void SetMasterVolume(float fVol);

	/*! Set the volume scale for all sounds with FLAG_SOUND_SCALABLE
	@param fScale volume scale (default 1.0)
	*/		
	void SetMasterVolumeScale(float fScale, bool bForceRecalc=true);

	/*! GetSFXVolume
	@param fVol volume (0.0f - 1.0f)
	*/		
	virtual float GetSFXVolume() { return m_fSFXVolume; }

	/*! SetMovieFadeoutVolume
	@param movieFadeoutVolume fadeout volume (0.0f - 1.0f)
	*/
	virtual void SetMovieFadeoutVolume(const float movieFadeoutVolume) {m_fMovieFadeoutVolume = movieFadeoutVolume;}

	/*! GetMovieFadeoutVolume
	@param movieFadeoutVolume fadeout volume (0.0f - 1.0f)
	*/
	virtual float GetMovieFadeoutVolume() const {return m_fMovieFadeoutVolume;}

	//! sets parameters for directional attenuation (for directional microphone effect); set fConeInDegree to 0 to disable the effect
	void CalcDirectionalAttenuation(const Vec3 &Pos, const Vec3 &Dir, const float fConeInRadians);

	//! returns the maximum sound-enhance-factor to use it in the binoculars as "graphical-equalizer"...
	float GetDirectionalAttenuationMaxScale() { return m_fDirAttMaxScale; }

	//! returns if directional attenuation is used
	bool UsingDirectionalAttenuation() { return (m_fDirAttCone!=0.0f); }

	//////////////////////////////////////////////////////////////////////////
	// PLAYBACK FUNCTIONALITY
	//////////////////////////////////////////////////////////////////////////

	//! pause all sounds
	virtual void	Pause(bool bPause, bool bResetVolume=false, unsigned int nFadeOutInMS=0);
	virtual bool  IsPaused() {return (m_bSoundSystemPause); }

	//! Mute/unmute all sounds
	void	Mute(bool bMute);

	//! Stop all sounds and music
	bool	Silence(bool bStopLoopingSounds, bool bUnloadData);

	//////////////////////////////////////////////////////////////////////////
	// Listener
	//////////////////////////////////////////////////////////////////////////

	/*! Set the listener position
	@param vPos position
	@param vVel velocity
	@param vAngles angles
	*/	
	bool SetListener(const ListenerID nListenerID, const Matrix34 &matOrientation, const Vec3 &vVel, bool bActive, float fRecordLevel);

	virtual void SetListenerEntity( ListenerID nListenerID, EntityId nEntityID ); 

	// Listener Management
	ListenerID	CreateListener();
	bool				RemoveListener(ListenerID nListenerID);
	ListenerID	GetClosestActiveListener(Vec3 vPosition) const;
	IListener*	GetListener(ListenerID nListenerID);
	IListener*	GetListener(EntityId nEntityID);
	IListener*	GetNextListener(ListenerID nListenerID);
	uint32			GetNumActiveListeners() const;

	//////////////////////////////////////////////////////////////////////////
	// VOIP
	//////////////////////////////////////////////////////////////////////////

	virtual bool GetRecordDeviceInfo(const int nRecordDevice, char* sName, int nNameLength);

	virtual IMicrophone* CreateMicrophone(
		const unsigned int nRecordDevice,
		const unsigned int nBitsPerSample, 
		const unsigned int nSamplesPerSecond,
		const unsigned int nBufferSizeInSamples);

	virtual bool RemoveMicrophone( IMicrophone *pMicrophone );

	virtual ISound* CreateNetworkSound(	INetworkSoundListener *pNetworkSoundListener,
		const unsigned int nBitsPerSample, 
		const unsigned int nSamplesPerSecond,
		const unsigned int nBufferSizeInSamples,
		EntityId PlayerID);

	virtual void RemoveNetworkSound (ISound *pSound);


	//////////////////////////////////////////////////////////////////////////
	// Interfaces
	//////////////////////////////////////////////////////////////////////////

	virtual ISoundSystem_Extended* GetInterfaceExtended() {return this;}
	virtual ISoundSystem_Deprecated* GetInterfaceDeprecated() {return this;}
	virtual ISoundSystem_WorkInProgress* GetInterfaceWorkInProgress() {return this;}

	IAudioDevice*				GetIAudioDevice()				const { return m_pAudioDevice; }
	ISoundMoodManager*	GetIMoodManager()				const { return m_pMoodManager; }
	IReverbManager*			GetIReverbManager()			const { return m_pReverbManager; }


	//////////////////////////////////////////////////////////////////////////
	// ISOUNDSYSTEM_EXTENDED
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// SYSTEM CONTROL
	//////////////////////////////////////////////////////////////////////////

	//!	Initialize the soundsystem after AudioDevice was initialized
	bool Init();

	/*! Create a music-system. You should only create one music-system at a time.
	*/
	IMusicSystem* CreateMusicSystem();

	// set special music effects volume
	void SetMusicEffectsVolume(float fVolume ) {m_fMusicEffectVolume = fVolume;}

	//! get music-volume
	float GetMusicVolume() const;
	
	virtual void SetMasterPitch(float fPitch);

	bool DeactivateAudioDevice();
	bool ActivateAudioDevice();

	//////////////////////////////////////////////////////////////////////////
	// PROFILING
	//////////////////////////////////////////////////////////////////////////

	void TraceMemoryUsage(int nMemUsage) { m_MemUsage += nMemUsage; }

	//! compute memory-consumption
	virtual void GetMemoryUsage(class ICrySizer* pSizer) const;

	//! get memory usage info
	void GetSoundMemoryUsageInfo(int *nCur,int *nMax) const;

	virtual int GetMemoryUsageInMB(bool bGetPoolSize = false);

	//! get number of voices playing
	int	GetUsedVoices() const;

	//! get cpu-usuage
	float	GetCPUUsage() const;

	//! Profiling Sounds
	virtual ISoundProfileInfo* GetSoundInfo(int nIndex);
	virtual ISoundProfileInfo* GetSoundInfo(const char* sSoundName);
	virtual int GetSoundInfoCount() { return m_SoundInfos.size(); }

	//////////////////////////////////////////////////////////////////////////
	// INFORMATION
	//////////////////////////////////////////////////////////////////////////

	virtual bool IsNullImplementation() const { return false; }
	//virtual bool DebuggingSound() = 0;	Implementation can be found in SoundSystemCommon

	//////////////////////////////////////////////////////////////////////////
	// VIDEO PLAYBACK
	//////////////////////////////////////////////////////////////////////////

	virtual void	GetOutputHandle( void **pHandle, void **pHandle2, EOutputHandle *HandleType) const;

	//////////////////////////////////////////////////////////////////////////
	// FUNCTIONALITY
	//////////////////////////////////////////////////////////////////////////

	//! Sets minimal priority for sound to be played.
	int SetMinSoundPriority( int nPriority );

	/*! to be called when something changes in the environment which could affect
	sound occlusion, for example a door closes etc.
	*/
	void	RecomputeSoundOcclusion(bool bRecomputeListener,bool bForceRecompute,bool bReset=false);

	//! Converts dialog file name syntax into dialog key
	virtual const char* MakeDialogKey(const char* sDialogFileName);


	//////////////////////////////////////////////////////////////////////////
	// ISOUNDSYSTEM_DEPRECATED
	//////////////////////////////////////////////////////////////////////////

	bool SetGroupScale(int nGroup, float fScale);

	void SetSoundActiveState(ISound *pSound, ESoundActiveState State);

	//! Check for EAX support.
	bool IsEAX(void);

	//////////////////////////////////////////////////////////////////////////
	// ISOUNDSYSTEM_WORKINPROGRESS
	//////////////////////////////////////////////////////////////////////////

	//! sets the weather condition that affect acoustics
	bool SetWeatherCondition(float fWeatherTemperatureInCelsius, float fWeatherHumidityAsPercent, float fWeatherInversion);

	//! gets the weather condition that affect acoustics
	bool GetWeatherCondition(float &fWeatherTemperatureInCelsius, float &fWeatherHumidityAsPercent, float &fWeatherInversion);


	//////////////////////////////////////////////////////////////////////////
	// ISOUNDSYSTEM_INTERNAL
	//////////////////////////////////////////////////////////////////////////


	ISoundAssetManager* GetISoundAssetManager() const { return m_pSoundAssetManager; }

	//! DSP unit callback for sfx-lowpass filter
	void* DSPUnit_SFXFilter_Callback(void *pOriginalBuffer, void *pNewBuffer, int nLength);

	bool IsOK() { return m_bOK; }


	tSoundID	CreateSoundID();											// returns an unused SoundID which also registers it
  bool			DestroySoundID(tSoundID SoundID);			// unregisters an used SoundID and frees it

	/*! Remove a sound from the sound system
	@param nSoundId sound id
	*/
	void RemoveSound(tSoundID nSoundID);

	/*! Remove a sound reference from the sound system
	@param nSoundId sound id
	*/	
	void RemoveReference(CSound *);


	CSoundBuffer* PrecacheSound( const char *sGroupAndSoundName, uint32 nSoundFlags, uint32 nPrecacheFlags );



	// quick shortcut to get individual Group/Sound Names, return PSNRC flag
	int	  ParseGroupString(TFixedResourceName &sGroupAndEventName, TFixedResourceName &sProjectPath, TFixedResourceName &sProjectName, TFixedResourceName &sGroupName, TFixedResourceName &sEventName, TFixedResourceName &sKeyName);


	// Listener Management
	float				GetSqDistanceToClosestListener(CSound* pSound);

	int GetMinSoundPriority() { return m_nMinSoundPriority; };

	void	FadeOutSound(CSound *pSound);

	//! reset the sound system (between loading levels)
	void	Reset();

	//! Will set speaker config
	void SetSpeakerConfig();

	//! remove a sound
	void RemoveBuffer(SSoundBufferProps &sn);



	void BufferLoaded() { ++m_nBuffersLoaded; }
	void BufferUnloaded() { --m_nBuffersLoaded; }





	//! Returns FALSE if sound can be stopped.
	bool ProcessActiveSound( CSound *pSound );
	bool ProcessReverb();



	bool IsEnabled() const { return m_bSoundSystemEnabled; }

	void RenderAudioInformation();



	// interface ISystemEventListener -------------------------------------------------------
	virtual void OnSystemEvent( ESystemEvent event,UINT_PTR wparam,UINT_PTR lparam );
	// ------------------------------------------------------------------------------------
	


	// Debuggin


	bool IsInPauseCall() const { return m_bIsInPauseCall; }

	bool IsEventUsedInPlatformSound (void* pHandle);



	//////////////////////////////////////////////////////////////////////////
	// Interfaces
	//////////////////////////////////////////////////////////////////////////



public:


	//IMicrophone * m_pMicrophone;
	//ISound * m_pNetworkSound;
	//uint32* m_pData;
	
	// A network sound listener
	//bool FillDataBuffer(	unsigned int nBitsPerSample, unsigned int nSamplesPerSecond,		
	//		unsigned int nNumSamples, void* pData )
	//{
	//	memcpy(pData, m_pData, nNumSamples);
	//	return true;
	//}

	//// A single microphone stream
	//bool ReadDataBuffer(	const unsigned int nBitsPerSample, const unsigned int nSamplesPerSecond,	
	//		const unsigned int nNumSamples, void* pData )
	//{
	//	//memset(m_pData, 1, nNumSamples);
	//	memcpy(m_pData, pData, nNumSamples);
	//	return true;
	//}

	IStreamEngine *m_pStreamEngine; // is used by SoundBuffer also
	CDebugLogger	 *m_pDebugLogger;

	// for debugging
	std::map<FILE* ,string >	m_FMODFilesMap;

	//static ISoundAssetManager* m_pSoundAssetManager;

	static TFixedResourceName g_sProjectPath;
	static TFixedResourceName g_sProjectName;
	static TFixedResourceName g_sGroupName;
	static TFixedResourceName	g_sEventName;
	static TFixedResourceName	g_sFileName;
	static TFixedResourceName	g_sKeyName;
	static unsigned int				g_nBuffersTrashed;;
	//static TFixedResourceName g_sStringLocal;

private:

	// platform dependent implementation of the AudioDevice
	IAudioDevice *m_pAudioDevice;
	CTimeValue		m_tUpdateAudioDevice;

	bool	m_bSoundSystemPause;
	bool	m_bOK;
	//bool  m_bInside;
	bool m_bIsInPauseCall;
	unsigned int m_nFadeOutInMS;

	ESoundSystemErrorCode m_ErrorCode;

	int		m_nBuffersLoaded;

	IReverbManager *m_pReverbManager;
	bool	m_bNotCalledYetEAX;
	int		m_nEaxStatus;

	//! if sound is potentially hear able
	bool	IsSoundPH(CSound *pSound);
	void	AddToSoundSystem(CSound *pSound);
	void	DeactivateSound( CSound *pSound );	
	void	ProcessInactiveSounds();
	void	ProcessOneShotSounds();
	void	ProcessLoopedSounds();
	void	ProcessHDRS();

	/*! Load a sound from disk
	@param szfile filename
	@param nFlags sound flags combination
	@return	sound interface
	*/
	ISound* LoadSound(const char *szFile, uint32 nFlags, uint32 nPrecacheFlags);

	// CSound class generation and removal
	CSound*		GetNewSoundObject();
	bool			RemoveSoundObject(CSound* pSound);

	void SetMinMaxRadius(CSound &Sound, float fMin, float fMax);

	SoundVec	m_UnusedSoundObjects;


	// AssetManager
	ISoundAssetManager*		m_pSoundAssetManager;
	CTimeValue						m_tUpdateSoundAssetManager;

	// ReverbManager
	CTimeValue						m_tUpdateReverbManager;

  string								m_sGroupPath;

	SoundVec							m_AllSoundsArray;

	int										m_nLastInactiveSoundsIndex;
	SSoundContainer				m_InactiveSounds;
	SSoundContainer				m_LoopedSounds;
	SSoundContainer				m_OneShotSounds;
	SSoundContainer				m_stoppedSoundToBeDeleted;

	MicroVec							m_Microphones;

	CTimeValue						m_tUpdateSounds;

  ISoundMoodManager					*m_pMoodManager;
	
	CSaltBufferArray<uint16, uint16, MAX_SOUNDIDS>		m_SoundSaltBuffer;			// used to create new entity ids (with uniqueid=salt)

	typedef std::set<tSoundID, std::less<tSoundID>, stl::STLPoolAllocator<tSoundID, stl::PoolAllocatorSynchronizationSinglethreaded> > TSoundIDSet;
	TSoundIDSet m_currentSoundIDs;

	// Listener Management
	tVecListeners	m_vecListeners;

	// sfx-filter stuff //////////////////////////////////////////////////////
//	CS_DSPUNIT *m_pDSPUnitSFXFilter; 
	float m_fDSPUnitSFXFilterCutoff;
	float m_fDSPUnitSFXFilterResonance;
	float m_fDSPUnitSFXFilterLowLVal;
	float m_fDSPUnitSFXFilterBandLVal;
	float m_fDSPUnitSFXFilterLowRVal;
	float m_fDSPUnitSFXFilterBandRVal;
	//////////////////////////////////////////////////////////////////////////

	// group stuff
	float m_fSoundScale[MAX_SOUNDSCALE_GROUPS];
	
	// new SoundGroup
	//SoundGroupVec m_vecSoundGroups;
	//bool					m_bSoundGroupPropertiesChanged;

	//

	int m_nMuteRefCnt;
	int m_nDialogSoundMoodAlloc;
	int m_nDialogEventParameterAlloc;

	// temp array for sound visareas to prevent runtime allocs
	PodArray<IVisArea*> m_SoundTempVisAreas;
	
	char			m_szEmptyName[32];

	float			m_fSFXVolume;					// Player controlled SFX volume slider within menu
	float			m_fGameSFXVolume;			// Game controlled SFX volume slider within eg. FlowGraph
	float			m_fMusicVolume;				// Player controlled Music volume slider within menu
	float			m_fGameMusicVolume;		// Game controlled Music volume slider within eg. FlowGraph
	float			m_fDialogVolume;			// Player controlled Voice volume slider within menu
	float			m_fGameDialogVolume;	// Game controlled Voice volume slider within eg. FlowGraph
	float			m_fMovieFadeoutVolume;
	
	float			m_fSFXResetVolume;
  float     m_fMusicEffectVolume;
	int				m_nSampleRate;
	int				m_MemUsage;

	// this is used for the binocular-feature where sounds are heard according to their screen-projection
	Vec3  m_DirAttPos;
	Vec3  m_DirAttDir;
	float m_fDirAttCone;
	float m_fDirAttMaxScale;
	bool  m_bNeedSoundZoomUpdate;

	// profiling
	typedef std::map<string, ISoundProfileInfo* > tmapSoundInfo;
	tmapSoundInfo	m_SoundInfos;
	uint32				m_nMaxUsedIDs;
	
	//int		m_nSpeakerConfig;	
	int		m_nReverbType;
	int		m_nMinSoundPriority;
	bool  m_bResetVolume;

	bool  m_bUpdateNeededMoodManager;
	bool	m_bSoundSystemEnabled;

	float m_fWeatherTemperatureInCelsius;
	float m_fWeatherHumidityAsPercent;
	float m_fWeatherInversion;
	float m_fWeatherAirAbsorptionMultiplyer;

	SSoundBufferProps m_TempBufferProps;

	bool m_bDelayPrecaching;
	//std::vector<SSoundBufferProps> m_DelayedPrecaches;
	SoundBufferPropsMap m_DelayedPrecaches;

	CSpamFilter m_SpamFilter;

	// parameter names
	SParameterNamesContainer m_ParameterNames;
	
	// Thread test
	DWORD m_dwMainThreadId; // this is the id of the main thread in which this engine operates
	bool IsMainThread(); // returns true if called from the main thread for this engine
	


  //bool SoundGroupChanged(ESOUNDGROUP eSoundGroup);

	// Out of convenience and because this is rarely used map is used
	struct SEventListener
	{
		SEventListener(ISoundSystemEventListener* pListener, bool bOnlyVoiceSounds) 
			: pListener(pListener), bOnlyVoiceSounds(bOnlyVoiceSounds) {}
		bool operator==(const SEventListener& other) const
		{
			return other.pListener == pListener;
		}
		bool operator==(const ISoundSystemEventListener* pOtherListener) const
		{
			return pListener == pOtherListener;
		}

		ISoundSystemEventListener* pListener;
		bool bOnlyVoiceSounds;
	};
	typedef std::vector<SEventListener>	TSoundSystenEventListenerVector;

	TSoundSystenEventListenerVector m_EventListeners;
	TSoundSystenEventListenerVector m_EventListenersTemp;

	//! Fires event for all listeners to this sound.
	void  OnEvent( ESoundSystemCallbackEvent event, ISound *pSound );

	//int m_nMemTest;

};

class CSoundProfileInfo : public ISoundProfileInfo
{
public:	
	CSoundProfileInfo(const char* sSoundName) 
	{ 
		m_sSoundName = sSoundName; 
	}
	~CSoundProfileInfo(){};

	VIRTUAL const char* GetName() { return m_sSoundName.c_str(); }
	VIRTUAL ISoundProfileInfo::SSoundProfileInfo* GetInfo() { return &m_SoundInfo; }
	VIRTUAL void AddInfo(SSoundProfileInfo &SoundInfo)
	{
		m_SoundInfo.nTimesPlayed					= max(m_SoundInfo.nTimesPlayed, SoundInfo.nTimesPlayed);
		m_SoundInfo.nTimesPlayedOnChannel = max(m_SoundInfo.nTimesPlayedOnChannel, SoundInfo.nTimesPlayedOnChannel);
		m_SoundInfo.nMemorySize						= max(m_SoundInfo.nMemorySize, SoundInfo.nMemorySize);
		m_SoundInfo.nPeakSpawn						= max(m_SoundInfo.nPeakSpawn, SoundInfo.nPeakSpawn);
	}

private:
	string						m_sSoundName;
	SSoundProfileInfo	m_SoundInfo;
};


#endif // CRYSOUND_SOUNDSYSTEM_H
