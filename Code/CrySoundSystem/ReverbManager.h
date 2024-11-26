////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ReverbManager.h
//  Version:     v1.00
//  Created:     22/2/2006 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: Base implementation of a ReverbManager.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __REVERBMANAGER_H__
#define __REVERBMANAGER_H__

#include "CryCommon/CrySoundSystem/IReverbManager.h"
#include "CryCommon/CryEntitySystem/IEntitySystem.h"
#include "CryCommon/CryCore/CrySizer.h"
#pragma once

#include "eax.h"

#define DIRECTIONS_COUNT 5

struct IAudioDevice;



class CReverbManager : public IReverbManager //, IAreaManagerEventListener
{

	friend class CReverbManagerDSP;
	friend class CReverbManagerEAX;

	//struct SReverbProps
	//{
	//	CRYSOUND_REVERB_PROPERTIES EAX;
	//	int nPreset;
	//};

	//struct SReverbPreset
	//{
	//	string sPresetName;
	//	CRYSOUND_REVERB_PROPERTIES EAX;
	//};

	typedef std::vector<SReverbInfo::SReverbPreset>		ReverbPresets;
	typedef ReverbPresets::iterator										ReverbPresetsIter;
	typedef ReverbPresets::const_iterator							ReverbPresetsIterConst;

	//struct SWeightedReverbPreset
	//{
	//	SReverbPreset* pReverbPreset;
	//	float fWeight;
	//	bool bFullEffectWhenInside;
	//};

	typedef std::vector<SReverbInfo::SWeightedReverbPreset>	ActiveReverbPresetAreas;
	typedef ActiveReverbPresetAreas::iterator								ActiveReverbPresetAreasIter;
	typedef ActiveReverbPresetAreas::const_iterator					ActiveReverbPresetAreasIterConst;


	//////////////////////////////////////////////////////////////////////////
	struct SRegisteredSound
	{
		tSoundID nSoundID;
		_smart_ptr<ISound> pSound;
		//Vec3 vOffset;
		//Vec3 vDirection;
		//bool bOffset;
		//bool bStatic;
		void GetMemoryUsage( ICrySizer *pSizer ) const
		{
			pSizer->AddObject(pSound);
		}

	};

	typedef std::vector<SRegisteredSound>		RegisteredSoundsVec;

public:
	CReverbManager(void);
	~CReverbManager(void);

	//////////////////////////////////////////////////////////////////////////
	// Initialization
	//////////////////////////////////////////////////////////////////////////

	virtual void GetMemoryUsage( ICrySizer *pSizer ) const;
	virtual void Init(IAudioDevice *pAudioDevice, int nInstanceNumber, CSoundSystem* pSoundSystem);
	virtual bool SelectReverb(int nReverbType);

	virtual void Reset();
	virtual void Release();

	virtual bool SetData( SReverbInfo::Data *pReverbData );

	//////////////////////////////////////////////////////////////////////////
	// Information
	//////////////////////////////////////////////////////////////////////////

	// writes output to screen in debug
	virtual void DrawInformation(IRenderer* pRenderer, float xpos, float ypos);

	// Draws the ray cast lines plus spheres at the collision points
	virtual void	DrawDynamicReverbDebugInfo(IRenderer* const pRenderer);

	//////////////////////////////////////////////////////////////////////////
	// Management
	//////////////////////////////////////////////////////////////////////////

	// needs to be called regularly
	virtual bool Update(bool bInside);

	virtual bool SetListenerReverb(SOUND_REVERB_PRESETS nPreset, CRYSOUND_REVERB_PROPERTIES *tpProps, uint32 nFlags);

	// Preset management
	virtual bool RegisterReverbPreset(char const* const pcPresetName=NULL, CRYSOUND_REVERB_PROPERTIES const* const pReverbProperties=NULL, CRYSOUND_ECHO_PROPERTIES const* const pEchoProperties=NULL);
	virtual bool UnregisterReverbPreset(const char *sPresetName=NULL);

	//! Registers an EAX Preset Area with the current blending weight (0-1) as being active
	//! morphing of several EAX Preset Areas is done internally
	//! Registering the same Preset multiple time will only overwrite the old one
	//virtual bool RegisterWeightedReverbEnvironment(const char *sPreset=NULL, CRYSOUND_REVERB_PROPERTIES *pProps=NULL, bool bFullEffectWhenInside=false, int nFlags=0);
	bool RegisterWeightedReverbEnvironment(const char *sPreset=NULL, EntityId RegID=0, bool bFullEffectWhenInside=false, uint32 nFlags=0);

	//virtual bool RegisterWeightedReverbEnvironment(const char *sPreset=NULL, SOUND_REVERB_PRESETS nPreset, bool bFullEffectWhenInside=false, int nFlags=0);

	//! Updates an EAX Preset Area with the current blending ratio (0-1)
	virtual bool UpdateWeightedReverbEnvironment(const char *sPreset=NULL, EntityId RegID=0, float fWeight=0.0);

	//! Unregisters an active EAX Preset Area 
	virtual bool UnregisterWeightedReverbEnvironment(const char *sPreset=NULL, EntityId RegID=0);

	//! Gets current EAX listener environment.
	virtual bool GetCurrentReverbEnvironment(SOUND_REVERB_PRESETS &nPreset, CRYSOUND_REVERB_PROPERTIES &Props);

	//! returns boolean if hardware reverb (EAX) is used or not
	virtual bool UseHardwareVoices() {return true;}

	//virtual bool RegisterSound(ISound *pSound);
	//ISound* GetSound( tSoundID nSoundId );
	
	//virtual int  GetReverbInstance(ISound *pSound) const;
	virtual float GetEnvironment(ISound *pSound);

	//virtual bool RegisterPlayer(EntityId PlayerID);
	//virtual bool UnregisterPlayer(EntityId PlayerID);

	//bool EvaluateSound(ISound *pSound);

	//////////////////////////////////////////////////////////////////////////
	// ISoundEventListener implementation
	//////////////////////////////////////////////////////////////////////////
	virtual void OnSoundEvent( ESoundCallbackEvent event,ISound *pSound );
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
	//! EventListener interface for the AreaManager.
	//////////////////////////////////////////////////////////////////////////
	//virtual void OnAreaManagerEvent( EEntityEvent event, EntityId TriggerEntityID, IArea *pArea );

	//////////////////////////////////////////////////////////////////////////
	//! Load reverb data from XML.
	//! @param bAddAdata if true data from XML will be added to currently loaded reverb data.
	bool LoadFromXML( const char *sFilename,bool bAddData );

	virtual void SerializeState(TSerialize ser);

	static int		OnObstructionTest(const EventPhys* pEvent);

protected:
	
	void	SendRay();

private:

	IAudioDevice*								m_pAudioDevice; // AudioDevice
	CSoundSystem*								m_pSoundSystem;
	ReverbPresets								m_ReverbPresets;
	ActiveReverbPresetAreas			m_vecActiveReverbPresetAreas;

	SOUND_REVERB_PRESETS				m_nLastEax; // will be deleted soon
	SReverbInfo::SReverbPreset	m_EAXIndoor;  // will be deleted soon
	SReverbInfo::SReverbPreset	m_EAXOutdoor; // will be deleted soon
	CRYSOUND_REVERB_PROPERTIES* m_pLastEAXProps; // will be deleted soon
	CRYSOUND_REVERB_PROPERTIES  m_EAXIndoorPreset;
	CRYSOUND_REVERB_PROPERTIES  m_EAXOutdoorPreset;
	CRYSOUND_REVERB_PROPERTIES  m_EAXOffPreset;
	SReverbInfo::SReverbPreset								m_sTempPreset;
	SReverbInfo::SWeightedReverbPreset				m_sTempEaxPreset;

	TFixedResourceName					m_sIndoorMix;
	TFixedResourceName					m_sOutdoorMix;

	// sound management
	RegisteredSoundsVec					m_Sounds;
	bool												m_ReverbEnable;
	bool												m_bInside;
	bool												m_bNeedUpdate;
	bool												m_bForceUnderwater;
	//bool												m_bAreaManagerListener;

	static int									m_nSentIdx;									// Index of sent direction vector
	static float								m_fObstructionTestResults[DIRECTIONS_COUNT];
	
	Vec3												m_v3ObstructionTestVecs[DIRECTIONS_COUNT];
	Vec3												m_v3Origin;

	/***********************************************************************************************
	* Function	:	EAX3ListenerInterpolate
	* Params	:	lpStart			- Initial EAX 3 Listener parameters
	*			:	lpFinish		- Final EAX 3 Listener parameters
	*			:	flRatio			- Ratio Destination : Source (0.0 == Source, 1.0 == Destination)
	*			:	lpResult		- Interpolated EAX 3 Listener parameters
	*			:	bCheckValues	- Check EAX 3.0 parameters are in range,
	- default == false (no checking)
	************************************************************************************************/
	bool    EAX3ListenerInterpolate(CRYSOUND_REVERB_PROPERTIES *lpStartEAX3LP, CRYSOUND_REVERB_PROPERTIES *lpFinishEAX3LP,
																	float flRatio, CRYSOUND_REVERB_PROPERTIES *lpResultEAX3LP, bool bCheckValues = false);

	bool    EchoDSPInterpolate(	CRYSOUND_ECHO_PROPERTIES const& rEchoPropertiesStart, CRYSOUND_ECHO_PROPERTIES const& rEchoPropertiesFinish,
															float fBlendRatio, CRYSOUND_ECHO_PROPERTIES& rEchoPropertiesResult);

	//Checks that the parameters in the EAX 3 Listener Properties structure are in-range
	bool CheckEAX3LP(CRYSOUND_REVERB_PROPERTIES *lpEAX3LP);

	//Clamps the length of the vector to 1.0f
	void Clamp(EAXVECTOR *eaxVector);

	//defines a distance function on ReverbProperties 
	float Distance(CRYSOUND_REVERB_PROPERTIES *lpProp1, CRYSOUND_REVERB_PROPERTIES *lpProp2);

	SRegisteredSound* FindRegisteredSound( tSoundID nSoundId );
	bool RemoveRegisteredSound( tSoundID nSoundId );

	void LoadPresetFromXML(	XmlNodeRef &node,	SReverbInfo::SReverbPreset *pPreset );

	void DrawDebugInfo();
};
#endif