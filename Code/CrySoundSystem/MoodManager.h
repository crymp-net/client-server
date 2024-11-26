////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   MoodManager.h
//  Version:     v1.00
//  Created:     25/10/2005 by Tomas
//  Compilers:   Visual Studio.NET
//  Description: MoodManager is responsible for organising categories into 
//							 moods and blending these together
////////////////////////////////////////////////////////////////////////////

#ifndef __SOUND_MOOD_MANAGER__
#define __SOUND_MOOD_MANAGER__
#pragma once

#include "CryCommon/CrySoundSystem/ISoundMoodManager.h"

#if defined(SOUNDSYSTEM_USE_FMODEX400)
#include "FmodEx/inc/fmod_event.hpp"
#endif

enum SAVETYPE
{
	eDONOTSAVE,
	eRESTORE_AT_START,
	eRESTORE_AT_SAVED_LOCATION
};

#define FLAG_SOUNDMOOD_VOLUME								0x000001	 
#define FLAG_SOUNDMOOD_PITCH								0x000002	
#define FLAG_SOUNDMOOD_LOWPASS							0x000004	 
#define FLAG_SOUNDMOOD_HIGHPASS							0x000008	 
#define FLAG_SOUNDMOOD_COMPRESSOR						0x000010	 
#define FLAG_SOUNDMOOD_PARAMEQ1							0x000020	 
#define FLAG_SOUNDMOOD_PARAMEQ2							0x000040	 


typedef struct Prop
{
	enumGroupParamSemantics eSemantics;
	ptParam									*pParam; // TODO ptr, besser reference

}Prop;

// forward declaration
class CCategory;
class CMoodManager;
struct ISoundAssetManager;
class CMood;

// Sound Mood
typedef struct SSoundMoodProp
{
	TFixedSoundMoodName	sFixMoodName;
	float		fFadeCurrent;
	float		fFadeTarget;
	int32	  nFadeTimeInMS;
	bool    bUnregisterOnFadeOut;
	bool		bForceUpdate;
	bool    bReadyToRemove;

	void GetMemoryUsage(ICrySizer *pSizer) const{}

}SSoundMoodProp;


//////////////////////////////////////////////////////////////////////////
class CCategory : public ICategory
{
public:
	CCategory(CMoodManager *pMoodManager, ICategory* pParent, CMood* pMood);
	~CCategory();

	// Name
	VIRTUAL const char*	GetName() const;
	VIRTUAL void				SetName(const char* sCategoryName) { m_sName = sCategoryName; }
	VIRTUAL const char*	GetMoodName() const;				

	// Platform Category
	VIRTUAL void*				GetPlatformCategory() const;
	VIRTUAL void				SetPlatformCategory(void *pPlatformCategory);

	// Muted
	VIRTUAL bool				GetMuted() const									{ return (m_nFlags & FLAG_SOUNDMOOD_MUTED) != 0; }	
	VIRTUAL void				SetMuted(bool bMuted); 

	// Parent
	VIRTUAL ICategory*	GetParentCategory() const		{ return m_ParentCategory; }
	VIRTUAL void				SetParentCategory(ICategory* pParent) { m_ParentCategory = pParent; }

	// Children Categories
	VIRTUAL uint32			GetChildCount();
	VIRTUAL ICategory*	GetCategoryByIndex(uint32 nCategoryCount) const;
	VIRTUAL ICategory*	GetCategory(const char *sCategoryName) const;
	VIRTUAL ICategory*	GetCategoryByPtr(void *pPlatformCategory) const;
	VIRTUAL ICategory*	AddCategory(const char *sCategoryName);
	VIRTUAL bool				RemoveCategory(const char *sCategoryName);

	// Attributes
	VIRTUAL uint32 GetAttributes() const { return m_nAttributes; }

	// Parameter
	VIRTUAL bool				GetParam(enumGroupParamSemantics eSemantics, ptParam* pParam) const;
	VIRTUAL bool				SetParam(enumGroupParamSemantics eSemantics, ptParam* pParam);

	VIRTUAL bool				InterpolateCategories(ICategory *pCat1, float fFade1, ICategory *pCat2, float fFade2, bool bForceDefault);

	// Import Export
	VIRTUAL bool				Serialize(XmlNodeRef &node, bool bLoading);
	VIRTUAL bool				RefreshCategories(const bool bAddNew);

	// Functionality
	VIRTUAL ISoundMoodManager* GetIMoodManager() { return (ISoundMoodManager*)m_pMoodManager; }

	// Other
	VIRTUAL bool ApplyCategory(float fFade);

	// writes output to screen in debug
	VIRTUAL	void DrawInformation(IRenderer* pRenderer, float &xpos, float &ypos, bool bAppliedValues); 

	VIRTUAL void GetMemoryUsage( ICrySizer *pSizer ) const;

	void*			GetDSP(const int nDSPType);
	void			SetDSP(const int nDSPType, void* pDSP);

	void			GetDSPFromAll(const int nDSPType);
	void			SetDSPForAll(const int nDSPType, void* pDSP);	

private:

	bool			ImportProps(XmlNodeRef &xmlNode);
	bool			ExportProps(XmlNodeRef &xmlNode);

	ICategory*						m_ParentCategory;
	int32									m_nFlags;	
	CMoodManager*					m_pMoodManager;
	CMood*								m_pMood;

#if defined(SOUNDSYSTEM_USE_FMODEX400)
	FMOD::EventCategory*	m_pPlatformCategory;
	FMOD_RESULT						m_ExResult;	
	FMOD::DSP*						m_pDSPLowPass;
	FMOD::DSP*						m_pDSPHighPass;
	FMOD::DSP*						m_pDSPCompressor;
	FMOD::DSP*						m_pDSPParamEQ1;
	FMOD::DSP*						m_pDSPParamEQ2;
	float									m_fMaxCategoryVolume;

	void FmodErrorOutput(const char * sDescription, ESoundLogType LogType = eSLT_Message);
#endif

	// Attributes
	string	m_sName;	
	uint32	m_nAttributes;
	float		m_fVolume;
	float		m_fPitch;
	// LowPass DSP
	float		m_fLowPassCutoff;					// Lowpass cutoff frequency in hz. 10.0 to 22000.0. Default = 5000.0. 
	// HighPass DSP
	float		m_fHighPassCutoff;				// Highpass cutoff frequency in hz. 1.0 to output 22000.0. Default = 5000.0. 
	float		m_fHighPassResonance;			// Highpass resonance Q value. 1.0 to 10.0. Default = 1.0. 
	// Compressor DSP
	float		m_fCompressorThreshold;		// Threshold level (dB) in the range from -60 through 0. The default value is 0. 
	float		m_fCompressorAttack;			// Gain reduction attack time (milliseconds), in the range from 10 through 200. The default value is 50. 
	float		m_fCompressorRelease;			// Gain reduction release time (milliseconds), in the range from 20 through 1000. The default value is 50. 
	float		m_fCompressorGainMakeup;	// Make-up gain (dB) applied after limiting, in the range from 0 through 30. The default value is 0. 
	// Param EQ1 DSP
	float		m_fParamEQ1Center;				// Frequency center. 20.0 to 22000.0. Default = 8000.0. 
	float		m_fParamEQ1Bandwidth;			// Octave range around the center frequency to filter. 0.2 to 5.0. Default = 1.0. 
	float		m_fParamEQ1Gain;					// Frequency Gain. 0.05 to 3.0. Default = 1.0. 
	// Param EQ2 DSP
	float		m_fParamEQ2Center;				// Frequency center. 20.0 to 22000.0. Default = 8000.0. 
	float		m_fParamEQ2Bandwidth;			// Octave range around the center frequency to filter. 0.2 to 5.0. Default = 1.0. 
	float		m_fParamEQ2Gain;					// Frequency Gain. 0.05 to 3.0. Default = 1.0. 



};


class CMood : public IMood
{

	typedef std::vector<_smart_ptr<ICategory> >				tvecCategory;
	typedef tvecCategory::iterator										tvecCategoryIter;
	typedef tvecCategory::const_iterator							tvecCategoryIterConst;

public:
	CMood(CMoodManager *pMoodManager);
	~CMood();

	// Name
	VIRTUAL const char*	GetName() const;
	VIRTUAL void SetName(const char* sMoodName) { m_sMoodName = sMoodName; }

	// Master Category
	VIRTUAL ICategory*	GetMasterCategory() const;

	// Categories
	VIRTUAL uint32			GetCategoryCount() const;
	VIRTUAL ICategory*	GetCategoryByIndex(uint32 nCategoryCount)const;
	VIRTUAL ICategory*	GetCategory(const char *sCategoryName) const;
	VIRTUAL ICategory*	GetCategoryByPtr(void* pPlatformCategory) const;
	VIRTUAL ICategory*	AddCategory(const char *sCategoryName);
	VIRTUAL bool				RemoveCategory(const char *sCategoryName);

	// Priority
	VIRTUAL float           GetPriority() const { return m_fPriority; }
	VIRTUAL void            SetPriority(float fPriority) { m_fPriority = fPriority; }
	// Music Volume
	VIRTUAL float           GetMusicVolume() const { return m_fMusicVolume; }
	VIRTUAL void            SetMusicVolume(float fMusicVolume) { m_fMusicVolume = fMusicVolume; }

	// Import Export
	VIRTUAL bool Serialize(XmlNodeRef &node, bool bLoading);
	VIRTUAL void SetIsMixMood(const bool bIsMixMood) { m_bIsMixMood = bIsMixMood; }
	VIRTUAL bool GetIsMixMood() { return m_bIsMixMood; }

	// Functionality
	VIRTUAL bool InterpolateMoods(const IMood *pMood1, float fFade1, const IMood *pMood2, float fFade2, bool bForceDefault);
	VIRTUAL bool ApplyMood(float fFade);

	// Other
	VIRTUAL ISoundMoodManager* GetIMoodManager() { return (ISoundMoodManager*)m_pMoodManager; }

	// writes output to screen in debug
	VIRTUAL void DrawInformation(IRenderer* pRenderer, float &xpos, float &ypos, bool bAppliedValues); 

	// collect memory informations
	void GetMemoryUsage(ICrySizer *pSizer) const;

	tvecCategory m_vecCategories;

private:

	CMoodManager				*m_pMoodManager;
	string							m_sMoodName;
	float								m_fPriority;
	float								m_fMusicVolume;
	bool								m_bIsMixMood;

#if defined(SOUNDSYSTEM_USE_FMODEX400)
	FMOD_RESULT	        m_ExResult;
#endif
	
};


//////////////////////////////////////////////////////////////////////////
class CMoodManager : ISoundMoodManager
{
	typedef std::vector<_smart_ptr<IMood> >						tvecMood;
	typedef tvecMood::iterator												tvecMoodIter;
	typedef tvecMood::const_iterator									tvecMoodIterConst;

	typedef std::vector<SSoundMoodProp>									tvecActiveMoods;
	typedef tvecActiveMoods::iterator										tvecActiveMoodsIter;
	typedef tvecActiveMoods::const_iterator							tvecActiveMoodsIterConst;
	typedef std::vector<tvecActiveMoodsIter>						tvecRemoveMoodIter;
	typedef tvecRemoveMoodIter::iterator								tvecRemoveMoodIterIter;
	typedef tvecRemoveMoodIter::const_reverse_iterator	tvecRemoveMoodIterIterConstReverse;

	friend class CCategory;
	friend class CMood;

public:

	//////////////////////////////////////////////////////////////////////////
	// Initialization
	//////////////////////////////////////////////////////////////////////////

	CMoodManager();
	~CMoodManager();

	VIRTUAL bool RefreshCategories(IMood *pMood);
	VIRTUAL bool RefreshPlatformCategories();
	virtual void Release();
	VIRTUAL void Reset();

	//////////////////////////////////////////////////////////////////////////
	// Memory Usage
	//////////////////////////////////////////////////////////////////////////

	// Returns current Memory Usage
	VIRTUAL uint32 GetMemUsage(void) const {return m_nTotalMemUsedByManager;}

	//////////////////////////////////////////////////////////////////////////
	// Information
	//////////////////////////////////////////////////////////////////////////

	VIRTUAL uint32 GetNumberSoundsLoaded(void) const;

	// writes output to screen in debug
	VIRTUAL void DrawInformation(IRenderer* pRenderer, float &xpos, float &ypos, int nSoundInfo); 

	// collect memory informations
	VIRTUAL void GetMemoryUsage(ICrySizer *pSizer) const;

	//////////////////////////////////////////////////////////////////////////
	// Management
	//////////////////////////////////////////////////////////////////////////

	// adds a Mood to the MoodManager database
	VIRTUAL IMood*	AddMood(const char *sMoodName);
	// removes a Mood (and all Category settings) defined by sMoodName from the MoodManager
	VIRTUAL bool		RemoveMood(const char *sMoodName);

	//////////////////////////////////////////////////////////////////////////
	// Real Time Manipulation
	//////////////////////////////////////////////////////////////////////////

	// needs to be updated regularly
	VIRTUAL bool Update();

	// registers a Mood to be actively processes (starts with 0 fade value)
	VIRTUAL bool	RegisterSoundMood(const char *sMoodName);

	// updates the fade value of a registered mood
	VIRTUAL bool	UpdateSoundMood(const char *sMoodName, float fFade, uint32 nFadeTimeInMS=0, bool bUnregistedOnFadeOut=true);

	// get current fade value of a registered mood
	VIRTUAL float	GetSoundMoodFade(const char *sMoodName);

	// unregisters a Mood and removes it from the active ones
	VIRTUAL bool	UnregisterSoundMood(const char *sMoodName);

	//////////////////////////////////////////////////////////////////////////
	// Access
	//////////////////////////////////////////////////////////////////////////

	VIRTUAL IMood*			GetMoodPtr(uint32 nGroupCount) const;					// may be NULL
	VIRTUAL IMood*			GetMoodPtr(const char *sMoodName) const;			// may be NULL
	VIRTUAL IMood*			GetMixMoodPtr() const { return m_pFinalMix; }	// may be NULL

	//////////////////////////////////////////////////////////////////////////
	// Import/Export
	//////////////////////////////////////////////////////////////////////////

	VIRTUAL bool Serialize(XmlNodeRef &node, bool bLoading);
	VIRTUAL void SerializeState(TSerialize ser);

	VIRTUAL bool RefreshCategories();

	//////////////////////////////////////////////////////////////////////////
private:

	void		UpdateMemUsage(int nAddToTotal) {m_nTotalMemUsedByManager += nAddToTotal;}
	bool    InterpolateMoods(const IMood* pMood1, const float fFade1, const IMood* pMood2, const float fFade2, IMood* pResultMood, float fResultFade);
	bool			Load(const char* sFilename);

#if defined(SOUNDSYSTEM_USE_FMODEX400)
	FMOD_RESULT	      m_ExResult;
#endif

	uint32						m_nTotalMemUsedByManager;
	uint32						m_nTotalSoundsListed;

	tvecMood					m_MoodArray;								// array of all Mood-Ptrs.

	tvecActiveMoods    m_ActiveMoods;							// vec of all active Moods
	tvecRemoveMoodIter m_ToBeRemoved;							// vec of iter to active Moods which will be removed
	IMood*						m_pFinalMix;
	bool							m_bForceFinalMix;
	bool							m_bNeedUpdate;

	CTimeValue				m_tLastUpdate;
};
#endif
