//////////////////////////////////////////////////////////////////////
//
//  CrySound Source Code
//
//  File: Sound.h
//  Description: ISound interface implementation.
//
//  History:
//	-June 06,2001:Implemented by Marco Corbetta
//
//////////////////////////////////////////////////////////////////////

#ifndef CRYSOUND_SOUND_H
#define CRYSOUND_SOUND_H

//////////////////////////////////////////////////////////////////////////
#include "IPlatformSound.h"
#include "SoundBuffer.h"
#include "SoundSystem.h"
#include "Cry_Geo.h"


// Forward declaration
struct IVisArea;
class CListener;
struct SLocalizedSoundInfoGame;

#define SOUND_PRELOAD_DISTANCE	10		// start preloading this amount of meters before the sound is potentially hearable (max_radius)

//////////////////////////////////////////////////////////////////////////////////////////////
// A sound...
class CSound : public ISound_Extended, public ISound_Deprecated
{

	friend void CSoundSystem::SetMinMaxRadius( CSound &Sound, float fMin, float fMax);

	struct RadiusSpecs
	{
		float	fOriginalMinRadius;
		float fOriginalMaxRadius;	
		float	fMinRadius;
		float fMaxRadius;	
		float	fMaxRadius2;			//used to skip playing of sounds if using sound's sphere
		float	fMinRadius2;			//used to skip playing of sounds if using sound's sphere
		float fPreloadRadius2;	// used to start preloading
		float fDistanceMultiplier; // used to tweak distance updating on sound events;
	};

	struct VoiceSpecs
	{
		string sCleanEventName;
		SLocalizedSoundInfoGame* pSoundInfo;
		float fVolume;
	};


protected:

public:
	CSound(class CSoundSystem *pSSys);
	virtual ~CSound();

	//////////////////////////////////////////////////////////////////////////
	// ISOUND INTERFACE
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// CORE FUNCTIONALITY
	//////////////////////////////////////////////////////////////////////////

	VIRTUAL void						SetSemantic(const ESoundSemantic eSemantic) {m_eSemantic = eSemantic; }
	VIRTUAL const ESoundSemantic	GetSemantic()const{return m_eSemantic;}

	void  AddEventListener( ISoundEventListener *pListener, const char *sWho );
	void  RemoveEventListener( ISoundEventListener *pListener );

	//////////////////////////////////////////////////////////////////////////
	// PLAYBACK CONTROL
	//////////////////////////////////////////////////////////////////////////

	void Play(const float fVolumeScale=1.0f, const bool bForceActiveState=true, const bool bSetRatio=true, IEntitySoundProxy *pEntitySoundProxy=NULL);
	void Stop(const ESoundStopMode eStopMode=ESoundStopMode_EventFade);
	void SetPaused(const bool bPaused);
	const bool GetPaused() const;

	//! Fading In/Out - 0.0f is Out 1.0f is In
	VIRTUAL void				SetFade(const float fFadeGoal, const int nFadeTimeInMS); 
	VIRTUAL EFadeState	GetFadeState() const { return m_eFadeState; }

	//////////////////////////////////////////////////////////////////////////
	// SPATIALIZATION
	//////////////////////////////////////////////////////////////////////////

	//! Set sound source position.
	void SetPosition(const Vec3 &vPos);

	//! Get sound source position.
	Vec3 GetPosition() const { return (m_vPosition); }	
	
	//! Set orientation of sound.
	//! Only relevant when cone angles are specified.	
	VIRTUAL void			SetMatrix(const Matrix34 newTransformation) { m_mTransformation = newTransformation; }
	VIRTUAL Matrix34	GetMatrix() const { return m_mTransformation; }
	VIRTUAL void			SetDirection(const Vec3 &vDir); // overwrites transformation matrix
	VIRTUAL Vec3			GetForward() const;

	//////////////////////////////////////////////////////////////////////////
	// MANIPULATION
	//////////////////////////////////////////////////////////////////////////

	// Gets and Sets Parameter defined in the enumAssetParam list
	VIRTUAL bool			GetParam(enumSoundParamSemantics eSemantics, ptParam* pParam) const;
	VIRTUAL bool			SetParam(enumSoundParamSemantics eSemantics, ptParam* pParam);

	// Gets and Sets Parameter defined by string and float value, returns the index of that parameter
	VIRTUAL int				GetParam(const char *sParameter, float *fValue, bool bOutputWarning=true) const;
	VIRTUAL int				SetParam(const char *sParameter, float fValue, bool bOutputWarning=true);

	// Gets and Sets Parameter defined by index and float value
	VIRTUAL bool			GetParam(int nIndex, float* const pfValue, float* const pfRangeMin=NULL, float* const pfRangeMax=NULL, char const** const ppcParameterName=NULL, bool const bOutputWarning=true) const;
	VIRTUAL bool			SetParam(int nIndex, float fValue, bool bOutputWarning=true);

	//! Sets a distance multiplier so sound event's distance can be tweak (sadly pretty workaround feature)
	VIRTUAL void			SetDistanceMultiplier(const float fMultiplier);

	//////////////////////////////////////////////////////////////////////////
	// INFORMATION
	//////////////////////////////////////////////////////////////////////////

	//! Get unique id of sound.
	const tSoundID	GetId() const;

	//! Get name of sound file.
	const char* GetName();

	uint32  GetFlags() const { return (m_nFlags); }

	bool IsPlaying() const { return (m_bPlaying); }	// returns true if the timer has started to play the sound
	bool IsPlayingVirtual() const;									// returns true if the sound is looping and not stopped. 
	bool  IsLoading() const;												// returns true if the Asset is still being loaded
	bool  IsLoaded() const;													// returns true if Asset is loaded

	int		GetLengthMs() const; //! returns the size of the stream in ms
	int		GetLengthInBytes() const;   //! returns the size of the stream 

	float GetMaxDistance() const						{ return (m_RadiusSpecs.fMaxRadius); }
	
	bool		IsRelative() const { return (m_nFlags & FLAG_SOUND_RELATIVE) ? (true) : (false); }	

	//////////////////////////////////////////////////////////////////////////
	// EXTENDED CONTROL
	//////////////////////////////////////////////////////////////////////////

	// modify a line sound
	VIRTUAL void SetLineSpec(const Vec3 &vStart, const Vec3 &vEnd);
	VIRTUAL bool GetLineSpec(  Vec3 &vStart,   Vec3 &vEnd);

	// modify a sphere sound
	VIRTUAL void SetSphereSpec(const float fRadius);

	void SetPhysicsToBeSkipObstruction(EntityId *pSkipEnts,int nSkipEnts);

	bool  UnloadBuffer(eUnloadDataOptions UnloadOption); // Unloads the soundbuffer of this sound.

	//////////////////////////////////////////////////////////////////////////
	// INTERFACES
	//////////////////////////////////////////////////////////////////////////

	virtual ISound_Extended* GetInterfaceExtended() {return this;}
	virtual ISound_Deprecated* GetInterfaceDeprecated() {return this;}

	//////////////////////////////////////////////////////////////////////////
	// ISOUND EXTENDED
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// PLAYBACK CONTROL
	//////////////////////////////////////////////////////////////////////////

	void	SetVolume(const float fVolume);									// Sets sound volume. Range: 0-1
	float	GetVolume() const		{ return (m_fMaxVolume); }	// Gets sound volume. Range: 0-1
		
	void	SetFlags(uint32 nFlags) { m_bFlagsChanged = true; m_nFlags = nFlags; }

	//! Set sound pitch. 1000 is default pitch.	
	void	SetPitch(int nValue); 	

	void	SetVelocity(const Vec3 &vVel);							//! Set sound source velocity.
	Vec3	GetVelocity( void ) const { return (m_vSpeed); }	//! Get sound source velocity.

	void  SetRatio(float fRatio); 

	//////////////////////////////////////////////////////////////////////////
	// MANIPULATION
	//////////////////////////////////////////////////////////////////////////

	//! set the currently played sample-pos in bytes or milliseconds
	void SetCurrentSamplePos(unsigned int nPos,bool bMilliSeconds);	

	//! retrieves the currently played sample-pos, in milliseconds or bytes
	unsigned int GetCurrentSamplePos(bool bMilliSeconds) const;

	//! set sound priority (0-255)
	void	SetSoundPriority(int nSoundPriority);

	//! Define sound code. Angles are in degrees, in range 0-360.	
	void	SetConeAngles(const float fInnerAngle,const float fOuterAngle);
	void	GetConeAngles(float &fInnerAngle, float &fOuterAngle);

	//! load the sound only when it is played
	bool	Preload();

	//! Set panning values (-1.0 left, 0.0 center, 1.0 right)
	VIRTUAL void	SetPan(float fPan);
	VIRTUAL float GetPan() const;

	//! Set 3d panning values (0.0 no directional, 1.0 full 3d directional effect)
	VIRTUAL void	Set3DPan(float f3DPan);
	VIRTUAL float	Get3DPan() const;

	//////////////////////////////////////////////////////////////////////////
	// INFORMATION
	//////////////////////////////////////////////////////////////////////////

	SObstruction* GetObstruction( void ) { return (&m_Obstruction); }		//! Get sound obstruction level.

	VIRTUAL bool IsInCategory(const char* sCategory);


	//////////////////////////////////////////////////////////////////////////
	// ISOUND DEPRECATED
	//////////////////////////////////////////////////////////////////////////


	//! sets automatic pitching amount (0-1000)
	void SetPitching(float fPitching);

	//! Set Minimal/Maximal distances for sound.
	//! Sound is not attenuated below minimal distance and not heared outside of max distance.	
	//! If deprecated this will move to private CSound
	void SetMinMaxDistance(float fMinDist, float fMaxDist);

	//! enable fx effects for this sound
	//! _ust be called after each play
	void	FXEnable(int nEffectNumber);

	void	FXSetParamEQ(float fCenter,float fBandwidth,float fGain);	


	// group stuff
	void	AddToScaleGroup(int nGroup)							{ m_nSoundScaleGroups|=(1<<nGroup); }
	void	RemoveFromScaleGroup(int nGroup)				{ m_nSoundScaleGroups&=~(1<<nGroup); }
	void	SetScaleGroup(unsigned int nGroupBits)	{ m_nSoundScaleGroups=nGroupBits; }

	//! Set looping mode of sound.
	void SetLoopMode(bool bLoopMode);

	int		GetFrequency() const
	{ 
		if (m_pSoundBuffer)
			return (m_pSoundBuffer->GetInfo()->nBaseFreq);
		return (0);
	}

	//////////////////////////////////////////////////////////////////////////
	// CSOUND INTERFACE
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// CSOUND CORE CONTROL
	//////////////////////////////////////////////////////////////////////////
	
	ESoundSystemErrorCode Initialize(CSoundBuffer* pInfoBuf, const unsigned int nFlags);
	void	StartPlayback(CListener* pListener, float &fDistanceToListener, float &fDistSquared);
	bool	CanBeCulled(CListener* pListener, float &fDistanceToListener, float &fDistSquared);

	void Update();														// central call to update changed of a sound to the platform sound library

	void Reset(); // completely resets the sound, all information is lost
	void SetId(tSoundID SoundID);
	
	void              SetState(ESoundActiveState eState) { m_eState = eState; }
	ESoundActiveState GetState() const									 { return (m_eState); }

	void              SetSoundBufferPtr(CSoundBuffer* pNewSoundBuffer);
	CSoundBuffer*	    GetSoundBufferPtr() const														{ return (m_pSoundBuffer); }

	void							SetPlatformSoundPtr(IPlatformSound* pPlatformSound)	{ m_pPlatformSound = pPlatformSound; }
	IPlatformSound*		GetPlatformSoundPtr()	const													{ return (m_pPlatformSound); }

	CSoundSystem*			GetSoundSystemPtr() const														{ return (m_pSSys); }
	
	//! set soundinfo struct for voice sounds
	void				SetVoiceSpecs( SLocalizedSoundInfoGame *pSoundInfo);
	VoiceSpecs*	GetVoiceSpecs() { return (&m_VoiceSpecs); }

	//! Frees sound library allocation, also stop hear able sound playback
	void Deactivate();
	
	// Add/remove sounds. // This is used in the smart_ptr
	int	AddRef() { return ++m_refCount; };
	int	Release();
	
	
	
	//////////////////////////////////////////////////////////////////////////
	// CSOUND PLAYBACK INFORMATION
	//////////////////////////////////////////////////////////////////////////
	
	bool IsPlayingOnChannel() const;								// returns true if a channel is used to play the sound and have channel allocated.
	  																							//it might be too far (clipped) so it will not use a physical channel nor is it audible
																									// TODO get rid of loop-dependecy. An inactive sound can started playback virtually.

	bool  IsActive() const;										// returns true if a sound is allocated in the sound library's "audio device"

	bool  IsPlayLengthExpired( float fCurrTime  ) const;
	
	bool	GetPausedState() const { return m_IsPaused; }

	bool	GetLoopMode() const { return (GetFlags() & FLAG_SOUND_LOOP); }

	float GetRatioByDistance(const float fDistance);			// return ratio

	float GetActiveVolume() const	{ return (m_fActiveVolume); }

	float GetRatio() const { return (m_fRatio); }

	Vec3 GetPlaybackPosition() const { return (m_vPlaybackPos); }	

	//////////////////////////////////////////////////////////////////////////
	// CSOUND DATA INFORMATION
	//////////////////////////////////////////////////////////////////////////

	float GetSquardedMaxRadius() const			{ return (m_RadiusSpecs.fMaxRadius2); }
	float GetSquardedPreloadRadius() const	{ return (m_RadiusSpecs.fPreloadRadius2); }
	void  GetMemoryUsage(class ICrySizer* pSizer) const;

	IVisArea *GetSoundVisArea() const { return (m_pVisArea); }
	void			SetSoundVisArea(IVisArea *pVisArea) { m_pVisArea = pVisArea; }
	void			InvalidateVisArea() { m_pVisArea = NULL; m_bRecomputeVisArea = true; }

	//////////////////////////////////////////////////////////////////////////
	// CSOUND EVENTHANDLING
	//////////////////////////////////////////////////////////////////////////

	//! Fires event for all listeners to this sound.
	void  OnEvent( ESoundCallbackEvent event );

	void  OnBufferLoaded();
	void  OnBufferLoadFailed();
	void	OnAssetUnloaded();
	void	OnBufferDelete();

	//////////////////////////////////////////////////////////////////////////
	// CSOUND WORKINPROGRESS
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// CSOUND DEPRECATED
	//////////////////////////////////////////////////////////////////////////

	//! Return frequency of sound.
	int	 GetBaseFrequency() const;

	void SetFrequency(int nFreq);	

	float GetMinRadius() const							{ return (m_RadiusSpecs.fMinRadius); }
	float GetSquardedMinRadius() const			{ return (m_RadiusSpecs.fMinRadius2); }

	void  SetAttrib(float fVolume, float fRatio, int nFreq=1000, bool bSetRatio=true);
	void  SetAttrib(const Vec3 &pos, const Vec3 &speed);
	

private:
	//! Update the position (called when directional-attenuation-parameters change)
	void UpdatePositionAndVelocity(); // just calls 	SetPosition(m_vPosition);

	//! Sets the ActiveVolume to the PlatformSound
	void	UpdateActiveVolume();

	//! updates the obstruction level of that sound, will be called from within Update()
	void ProcessObstruction(CListener *pListener, bool bImmediatly);
	void CalculateObstruction(); // evaluates the RWIs and changes the obstruction value

	// group stuff
	float	CalcSoundVolume(float fSoundVolume, float fVolumeScale) const;
	
	bool  UpdateSoundProperties();
	void	UpdateVoiceProperties(bool bSoundEnded);

	void SetMinMaxDistance_i(float fMinDist, float fMaxDist);

private:

	tSoundID										m_nSoundID;
	_smart_ptr<CSoundBuffer>		m_pSoundBuffer;	
	_smart_ptr<IPlatformSound>	m_pPlatformSound;

	ESoundActiveState		m_eState;
	EFadeState					m_eFadeState;
	ESoundSemantic			m_eSemantic;
	
	uint32	m_nFlags;
	int			m_refCount;

	public:

		static CSoundSystem		*m_pSSys;
		EntityId							m_SoundProxyEntityId;
		_smart_ptr<CSoundBuffer>		m_pEventGroupBuffer;	
		ESoundStopMode m_eStopMode;
		
private:

	// old group stuff
	unsigned int				m_nSoundScaleGroups;

	int		m_nFxChannel;	
	int		m_nStartOffset;
	bool	m_bOffsetInSamples;
	
	CTimeValue	m_tPlayTime;
	float				m_fSyncTimeout;
	float				m_fLastObstructionUpdate;

	// Volume related
	float	m_fMaxVolume;			// theoretical maximum volume
	float	m_fActiveVolume;	// volume that is currently set to the platform sound
	float m_fRatio;					// additional scale factor used for software distance rollof
	float m_fFadeGoal;
	float m_fFadeCurrent;
	int   m_nFadeTimeInMS;
	
	float	m_fPan;
	int		m_nCurrPitch;
	int		m_nRelativeFreq;

	float m_HDRA;		// additional attenuation based on louder sounds nearby
	float	m_fPitching;	// relative random pitching of sound (to avoid flanging if multiple sounds are played simultaneously)
	float	m_fPostLoadRatio;

	// Radius Specs
	RadiusSpecs m_RadiusSpecs;

	// Direction
	Matrix34	m_mTransformation;
	float m_fInnerAngle;
	float m_fOuterAngle;

	// Voice Specs
	VoiceSpecs m_VoiceSpecs;

	// Vectors
	Vec3	m_vPosition;		// position of the sound object, maybe relative coordinates
	Vec3	m_vPlaybackPos;	// this is the playback position (may differ from m_position on binocular/portals/relative sounds)
	Vec3	m_vSpeed;

	//Line Sound quick feature
	Lineseg m_Line;
	bool  m_bLineSound;

	//Sphere Sound quick feature
	Vec3 m_vCenter;
	float m_fSphereRadius;
	bool  m_bSphereSound;
	
	//short		m_nSectorId,m_nBuildingId;
	IVisArea	*m_pVisArea;
	
	// update flags // TODO put these in a struct or bitfield
	bool  m_bVolumeHasChanged;
	bool  m_bPositionHasChanged;
	bool  m_bPlayerPositionHasChanged;
	bool	m_bLoopModeHasChanged;
	bool  m_bFlagsChanged;

	bool	m_bPlaying; // set the logical state of the Sound, eg a looping Sound is always playing until stopped
	bool	m_IsPaused;
	bool	m_bSentOnStopEvent; // guard to prevent the OnStop Event is not sent out more than once
	bool	m_bStopCalled;
	bool	m_bPlayAfterLoad;
	bool	m_bPostLoadForceActiveState;
	bool	m_bPostLoadSetRatio;
	bool	m_bRecomputeVisArea;
	bool  m_bRefCountGuard;
	bool  m_bInitialized;

	int		m_nSoundPriority;

		
	typedef std::vector<SSoundEventListenerInfo> TEventListenerInfoVector;
	TEventListenerInfoVector m_listeners;
	TEventListenerInfoVector m_listenersToBeRemoved;
	TEventListenerInfoVector m_listenersTemp;

	string m_sSoundName;	


	public:
		SObstruction m_Obstruction;

};

#endif // CRYSOUND_SOUND_H
