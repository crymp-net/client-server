////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ReverbManager.cpp
//  Version:     v1.00
//  Created:     22/2/2006 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: Base implementation of a ReverbManager.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "ReverbManager.h"
#include "CryCommon/CryRenderer/IRenderer.h"
#include "CryCommon/CryRenderer/IRenderAuxGeom.h"
#include "IAudioDevice.h"
#include "CryCommon/CryEntitySystem/IEntitySystem.h"
#include "CryCommon/CryNetwork/ISerialize.h"
#include "SoundSystem.h"
#include "Sound.h"

#include "CryCommon/CryPhysics/IPhysics.h"

//////////////////////////////////////////////////////////////////////////
// Initialization
//////////////////////////////////////////////////////////////////////////

float				CReverbManager::m_fObstructionTestResults[DIRECTIONS_COUNT] = {0.0f};
int					CReverbManager::m_nSentIdx = 0;

CReverbManager::CReverbManager()
{
	m_pAudioDevice			= NULL;
	m_pSoundSystem			= NULL;
	m_bForceUnderwater	= false;
	//m_bAreaManagerListener = false;
	
	string sFullFileName;
	sFullFileName = REVERB_PRESETS_FILENAME_XML;

	LoadFromXML(sFullFileName.c_str(), false);
	m_ReverbEnable = false;

	m_nLastEax=REVERB_PRESET_OFF;	

	CRYSOUND_REVERB_PROPERTIES PropsIn = CRYSOUND_REVERB_PRESET_GENERIC;
	m_EAXIndoorPreset = PropsIn;

	CRYSOUND_REVERB_PROPERTIES pPropsOut = CRYSOUND_REVERB_PRESET_OUTDOOR;
	m_EAXOutdoorPreset		= pPropsOut;

//	CRYSOUND_REVERB_PROPERTIES pPropsOff = CRYSOUND_REVERB_PRESET_OFF;
	m_EAXOffPreset		= pPropsOut;

	m_sTempPreset.EAX =	m_EAXOutdoorPreset;
	m_sTempPreset.sPresetName = "Mix";

	m_sTempEaxPreset.pReverbPreset = &m_sTempPreset;

	m_pLastEAXProps = NULL;
	m_vecActiveReverbPresetAreas.reserve(4);
	m_vecActiveReverbPresetAreas.clear();
	m_bInside = false;
	m_bNeedUpdate = true;

	m_sIndoorMix	= "IndoorMix";
	m_sOutdoorMix = "OutdoorMix";
	
	m_v3ObstructionTestVecs[0] = Vec3(0.0f, 0.0f, 1.0f),	// Top vector
	m_v3ObstructionTestVecs[1] = Vec3(0.0f, 1.0f, 0.0f),	// Front vector
	m_v3ObstructionTestVecs[2] = Vec3(0.0f, -1.0f, 0.0f),	// Rear vector
	m_v3ObstructionTestVecs[3] = Vec3(1.0f, 0.0f, 0.0f),	// Right vector
	m_v3ObstructionTestVecs[4] = Vec3(-1.0f, 0.0f, 0.0f),	// Left vector

	m_v3Origin.zero();

	if(gEnv->pPhysicalWorld)
		gEnv->pPhysicalWorld->AddEventClient(EventPhysRWIResult::id, OnObstructionTest, 1);
}

CReverbManager::~CReverbManager()
{
	m_ReverbPresets.clear();

	//gEnv->pEntitySystem->GetAreaManager()->RemoveEventListener(this);
}

void CReverbManager::Init(IAudioDevice *pAudioDevice, int nInstanceNumber, CSoundSystem* pSoundSystem)
{
	assert (pAudioDevice);
	assert (pSoundSystem);
	m_pAudioDevice = pAudioDevice;
	m_pSoundSystem = pSoundSystem;
}

bool CReverbManager::SelectReverb(int nReverbType) 
{ 
	m_bNeedUpdate = true;
	if (nReverbType == 0)
	{
		m_ReverbEnable = false;
		m_vecActiveReverbPresetAreas.clear();
		Update(false);
	}
	else
		m_ReverbEnable = true;
	
	return true; 
}

void CReverbManager::Reset()
{
	m_vecActiveReverbPresetAreas.clear();
}

void CReverbManager::Release()
{
	Reset();
}

bool CReverbManager::SetData(SReverbInfo::Data *pReverbData)
{

	m_ReverbPresets.clear();
	m_vecActiveReverbPresetAreas.clear();

	if (!pReverbData)
		return false;

	int i;

	//////////////////////////////////////////////////////////////////////////
	// Set Presets.

	for (i = 0; i < pReverbData->nPresets; i++)
	{
		SReverbInfo::SReverbPreset NewPreset;
		NewPreset.sPresetName = pReverbData->pPresets[i].sPresetName;
		NewPreset.EAX = pReverbData->pPresets[i].EAX;
		m_ReverbPresets.push_back(NewPreset);
	}
	return true;
	
}

//////////////////////////////////////////////////////////////////////////
void CReverbManager::LoadPresetFromXML( XmlNodeRef &node, SReverbInfo::SReverbPreset *pPreset )
{
	// Loading.
	pPreset->sPresetName = node->getTag();
	//pPreset->sDefaultMood = node->getAttr( "DefaultMood" );
	//node->getAttr( "DefaultMoodTimeout",pPreset->fDefaultMoodTimeout );



	//XmlNodeRef nodeProps = node->findChild("Moods");
	//if (nodeProps)
	//{
	//	for (int i = 0; i < node->getChildCount(); i++)
	//	{
			XmlNodeRef childNode = node->findChild("nEnvironment");
			childNode->getAttr( "value",pPreset->EAX.Environment );

			childNode = node->findChild("fEnvSize");
			childNode->getAttr( "value",pPreset->EAX.EnvSize );

			childNode = node->findChild("fEnvDiffusion");
			childNode->getAttr( "value",pPreset->EAX.EnvDiffusion );

			childNode = node->findChild("nRoom");
			childNode->getAttr( "value",pPreset->EAX.Room );

			childNode = node->findChild("nRoomHF");
			childNode->getAttr( "value",pPreset->EAX.RoomHF );

			childNode = node->findChild("nRoomLF");
			childNode->getAttr( "value",pPreset->EAX.RoomLF );

			childNode = node->findChild("fDecayTime");
			childNode->getAttr( "value",pPreset->EAX.DecayTime );

			childNode = node->findChild("fDecayHFRatio");
			childNode->getAttr( "value",pPreset->EAX.DecayHFRatio );

			childNode = node->findChild("fDecayLFRatio");
			childNode->getAttr( "value",pPreset->EAX.DecayLFRatio );

			childNode = node->findChild("nReflections");
			childNode->getAttr( "value",pPreset->EAX.Reflections );

			childNode = node->findChild("fReflectionsDelay");
			childNode->getAttr( "value",pPreset->EAX.ReflectionsDelay );

			childNode = node->findChild("fReflectionsPan");
			XmlNodeRef vectorChild = childNode->findChild("x");
			vectorChild->getAttr( "value",pPreset->EAX.ReflectionsPan[0] );
			vectorChild = childNode->findChild("y");
			vectorChild->getAttr( "value",pPreset->EAX.ReflectionsPan[1] );
			vectorChild = childNode->findChild("z");
			vectorChild->getAttr( "value",pPreset->EAX.ReflectionsPan[2] );

			childNode = node->findChild("nReverb");
			childNode->getAttr( "value",pPreset->EAX.Reverb );

			childNode = node->findChild("fReverbDelay");
			childNode->getAttr( "value",pPreset->EAX.ReverbDelay );

			childNode = node->findChild("fReverbPan");
			vectorChild = childNode->findChild("x");
			vectorChild->getAttr( "value",pPreset->EAX.ReverbPan[0] );
			vectorChild = childNode->findChild("y");
			vectorChild->getAttr( "value",pPreset->EAX.ReverbPan[1] );
			vectorChild = childNode->findChild("z");
			vectorChild->getAttr( "value",pPreset->EAX.ReverbPan[2] );

			childNode = node->findChild("fEchoTime");
			childNode->getAttr( "value",pPreset->EAX.EchoTime );

			childNode = node->findChild("fEchoDepth");
			childNode->getAttr( "value",pPreset->EAX.EchoDepth );

			childNode = node->findChild("fModulationTime");
			childNode->getAttr( "value",pPreset->EAX.ModulationTime );

			childNode = node->findChild("fModulationDepth");
			childNode->getAttr( "value",pPreset->EAX.ModulationDepth );

			childNode = node->findChild("fAirAbsorptionHF");
			childNode->getAttr( "value",pPreset->EAX.AirAbsorptionHF );

			childNode = node->findChild("fHFReference");
			childNode->getAttr( "value",pPreset->EAX.HFReference );

			childNode = node->findChild("fLFReference");
			childNode->getAttr( "value",pPreset->EAX.LFReference );

			childNode = node->findChild("fRoomRolloffFactor");
			childNode->getAttr( "value",pPreset->EAX.RoomRolloffFactor );

			childNode = node->findChild("fDiffusion");
			childNode->getAttr( "value",pPreset->EAX.Diffusion );

			childNode = node->findChild("fDensity");
			childNode->getAttr( "value",pPreset->EAX.Density );

			childNode = node->findChild("nFlags");
			childNode->getAttr( "value",pPreset->EAX.Flags );

			childNode = node->findChild("EchoDSP");
			if(childNode)
			{
				XmlNodeRef oEchoDSPChildNode = childNode->findChild("fDelay");
				oEchoDSPChildNode->getAttr( "value", pPreset->EchoDSP.fDelay );
				oEchoDSPChildNode = childNode->findChild("fDecayRatio");
				oEchoDSPChildNode->getAttr( "value", pPreset->EchoDSP.fDecayRatio);
				oEchoDSPChildNode = childNode->findChild("fDryMix");
				oEchoDSPChildNode->getAttr( "value", pPreset->EchoDSP.fDryMix );
				oEchoDSPChildNode = childNode->findChild("fWetMix");
				oEchoDSPChildNode->getAttr( "value", pPreset->EchoDSP.fWetMix );
			}
			
	//		
	//		if (childNode->isTag("Preset"))
	//		{
	//			SReverbInfo::SReverbPreset NewPreset;
	//			LoadPresetFromXML( nodeTheme, &NewPreset );
	//		}
	//		childnode
	//		SMusicMood *pMood = new SMusicMood;
	//		LoadMoodFromXML( childNode,pMood );
	//		pPreset->EAX.AirAbsorptionHF
	//		pPreset->mapMoods[pMood->sName.c_str()] = pMood;
	//	}
	//}

	//XmlNodeRef nodeBridges = node->findChild( "Bridges" );
	//if (nodeBridges)
	//{
	//	for (int i = 0; i < nodeBridges->getChildCount(); i++)
	//	{
	//		XmlNodeRef nodeBridge = nodeBridges->getChild(i);
	//		const char *sPattern = nodeBridge->getAttr( "Pattern" );
	//		if (strlen(sPattern) > 0)
	//		{
	//			pPreset->mapBridges[nodeBridges->getTag()] = (const char*)sPattern;
	//		}
	//	}
	//}

	// Add this pattern to music data.
	//m_mapThemes[pPreset->sName] = pTheme;

}

//////////////////////////////////////////////////////////////////////////
bool CReverbManager::LoadFromXML( const char *sFilename,bool bAddData )
{
	// Loading.
	if (!bAddData)
	{
		//Unload();
	}

	XmlNodeRef root = GetISystem()->LoadXmlFile( sFilename );

	if (!root)
	{
		GetISystem()->Warning( VALIDATOR_MODULE_SYSTEM,VALIDATOR_WARNING,VALIDATOR_FLAG_SOUND,sFilename,
			"Reverb description XML file %s failed to load",sFilename );
		return false;
	}

	// Iterate reverb presets
	for (int i = 0; i < root->getChildCount(); i++)
	{
		XmlNodeRef nodeTheme = root->getChild(i);
		
		//if (nodeTheme->isTag("Preset"))
		{
			SReverbInfo::SReverbPreset NewPreset;
			LoadPresetFromXML( nodeTheme, &NewPreset );
			m_ReverbPresets.push_back(NewPreset);

		}
	}

	//m_bDataLoaded = true;

	return true;
}



//////////////////////////////////////////////////////////////////////////
// Information
//////////////////////////////////////////////////////////////////////////

// writes output to screen in debug
void CReverbManager::DrawInformation(IRenderer* pRenderer, float xpos, float ypos)
{
	//ActiveEaxPresetAreasIter It=m_vecActiveEaxPresetAreas.begin();
	//pRenderer->Draw2dLabel(xpos, ypos, 1.5, fColorGreen, false, "EAX - %s",(*It).sPresetName);
	float fColorGreen[4] ={0.0f, 1.0f, 0.0f, 0.7f};
	float fColorGray[4]	 ={0.3f, 0.3f, 0.3f, 0.7f};
	float fColorTemp[4]	 ={0.0f, 0.0f, 0.0f, 0.7f};
	float fColorRed[4]	 ={1.0f, 0.0f, 0.0f, 0.7f};

	if (!m_ReverbEnable)
	{
		pRenderer->Draw2dLabel(xpos, ypos, 1.35f, fColorGray, false, "Reverb disabled");
		ypos+=11;
		return;
	}


	ActiveReverbPresetAreasIterConst  IterEnd = m_vecActiveReverbPresetAreas.end();
	for (ActiveReverbPresetAreasIterConst  It=m_vecActiveReverbPresetAreas.begin(); It!=IterEnd; ++It)
	{
		float *pCurColor = fColorGreen;

		if ((*It).bFullEffectWhenInside)
			pCurColor = fColorRed;

		fColorTemp[0] = (*It).fWeight * pCurColor[0] + (1.0f - (*It).fWeight) * fColorGray[0];
		fColorTemp[1] = (*It).fWeight * pCurColor[1] + (1.0f - (*It).fWeight) * fColorGray[1];
		fColorTemp[2] = (*It).fWeight * pCurColor[2] + (1.0f - (*It).fWeight) * fColorGray[2];

		pRenderer->Draw2dLabel(xpos, ypos, 1.35f, fColorTemp, false, "EAX - %s",(*It).pReverbPreset->sPresetName.c_str());

		ypos+=11;
	}
}


//////////////////////////////////////////////////////////////////////////
// Management
//////////////////////////////////////////////////////////////////////////

// needs to be called regularly
bool CReverbManager::Update(bool bInside)
{
	if (!m_bNeedUpdate || !m_ReverbEnable)
		return true;

	FUNCTION_PROFILER( GetISystem(),PROFILE_SOUND );

	//////////////////////////////////////////////////////////////////////////
	// new code by Tomas
	// go through the vector of active EAX Presets, calculate the resulting Interpolation and set it
	//CRYSOUND_REVERB_PROPERTIES GenericEAX = CS_PRESET_GENERIC;

	/*
	if (bWasInside!=m_bInside)
	{
	if (m_bInside)
	SetEaxListenerEnvironment(m_EAXIndoor.nPreset, (m_EAXIndoor.nPreset==-1) ? &m_EAXIndoor.EAX : NULL, FLAG_SOUND_INDOOR);
	else
	SetEaxListenerEnvironment(m_EAXOutdoor.nPreset, (m_EAXOutdoor.nPreset==-1) ? &m_EAXOutdoor.EAX : NULL, FLAG_SOUND_OUTDOOR);
	}
	*/

	if (m_bForceUnderwater)
	{
		CRYSOUND_REVERB_PROPERTIES pPropsOut = CRYSOUND_REVERB_UNDERWATER;
		m_sTempEaxPreset.pReverbPreset->EAX = pPropsOut;
	}
	else
	{


		// Verifying Reverb Values enabled/disables
		bool Checkresults = true;
		float SumOfWeight = 0;
		m_bInside = bInside;

		// if there is only one Preset just interpolate directly with its weight
		if (m_vecActiveReverbPresetAreas.size()==1)
		{
			ActiveReverbPresetAreasIterConst It= m_vecActiveReverbPresetAreas.begin();
			SumOfWeight = (*It).fWeight;
			m_sTempEaxPreset = (*It);
		}
		else
		{
			ActiveReverbPresetAreasIter ItEnd = m_vecActiveReverbPresetAreas.end();
			for (ActiveReverbPresetAreasIter It=m_vecActiveReverbPresetAreas.begin(); It!=ItEnd; ++It)
			{
				// Copy first element to the temp preset
				if (It==m_vecActiveReverbPresetAreas.begin())
				{
					m_sTempEaxPreset = (*It);
				}
				else // start interpolating at the 2nd element
				{
					// calculate the sum of the weight
					SumOfWeight = m_sTempEaxPreset.fWeight + (*It).fWeight;
					if (SumOfWeight>0) 
					{
						float BlendRatio = 0;
						//float iteratorweight=(*It).fWeight;
						BlendRatio = (*It).fWeight / SumOfWeight;
						//EAX3ListenerInterpolate(&m_sTempEaxPreset.EAX, &((*It).EAX), BlendRatio, &m_sTempEaxPreset.EAX, Checkresults);
						EAX3ListenerInterpolate(&m_sTempEaxPreset.pReverbPreset->EAX, &((*It).pReverbPreset->EAX), BlendRatio, &m_sTempEaxPreset.pReverbPreset->EAX, Checkresults);

						m_sTempEaxPreset.fWeight = BlendRatio;
						//gEnv->pLog->LogToConsole("setting Weight %f Interpolate", BlendRatio);
					}
				}
			}
		}

		// if the weighted sum of effect has not reached 100% then "fill up" with basic preset
		if (SumOfWeight < 1) 
		{
			if (!m_bInside) 
				EAX3ListenerInterpolate(&m_EAXOutdoorPreset, &m_sTempEaxPreset.pReverbPreset->EAX, SumOfWeight, &m_sTempEaxPreset.pReverbPreset->EAX, Checkresults);
				//EAX3ListenerInterpolate(&m_EAXOutdoorPreset, &m_sTempEaxPreset.EAX, SumOfWeight, &m_sTempEaxPreset.EAX, Checkresults);
			else 
				EAX3ListenerInterpolate(&m_EAXIndoorPreset, &m_sTempEaxPreset.pReverbPreset->EAX, SumOfWeight, &m_sTempEaxPreset.pReverbPreset->EAX, Checkresults);
				//EAX3ListenerInterpolate(&m_EAXIndoorPreset, &m_sTempEaxPreset.EAX, SumOfWeight, &m_sTempEaxPreset.EAX, Checkresults);

		}
	}


	/*
	if (m_nLastEax!=EAX_PRESET_UNDERWATER)
	{			
	// if not underwater, check if we now went to outside
	if (bWasInside!=m_bInside)
	{
	if (!m_bInside)
	{
	// if outside,set the global EAX outdoor environment
	// disabled by Tomas
	SetEaxListenerEnvironment(m_EAXOutdoor.nPreset, (m_EAXOutdoor.nPreset==-1) ? &m_EAXOutdoor.EAX : NULL);
	m_sTempEaxPreset.EAX = m_EAXOutdoor.EAX;
	}
	else m_sTempEaxPreset.EAX = m_EAXIndoor.EAX;
	}
	else m_sTempEaxPreset.EAX=m_EAXOutdoor.EAX;
	}
	
	*/
	
	
	 // TODO reactivate weather airabsorption
	// Add the weather effect
	//m_sTempEaxPreset.EAX.AirAbsorptionHF = m_fWeatherAirAbsorptionMultiplyer*m_sTempEaxPreset.EAX.AirAbsorptionHF;
	// TODO take the Weather Inversion into effect

	//TODO Reverb_Properties are different in FmodEX
	if (m_pAudioDevice)
	{
		//FMOD::System *pEx = (FMOD::System*)m_pAudioDevice->GetSoundLibrary();
		//FMOD_REVERB_PROPERTIES *pProp = (FMOD_REVERB_PROPERTIES*)&(m_sTempEaxPreset.EAX);
		//pProp->Instance = 0;
		//FMOD_RESULT Result = pEx->setReverbProperties(pProp);
		//Result = Result;
	}

	m_bNeedUpdate = false;
	return true;
}


bool CReverbManager::SetListenerReverb(SOUND_REVERB_PRESETS nPreset, CRYSOUND_REVERB_PROPERTIES *tpProps, uint32 nFlags)
{
	//GUARD_HEAP;
	//FUNCTION_PROFILER( GetSystem(),PROFILE_SOUND );

	if (!m_ReverbEnable)
		return false;

	m_bNeedUpdate = true;

	if (nFlags==FLAG_SOUND_OUTDOOR)
	{
		m_bForceUnderwater = false;
		//m_EAXOutdoor.nPreset=nPreset;
		m_EAXOutdoor.EAX = *tpProps;
		
		gEnv->pSoundSystem->Log(eSLT_Message, "<Sound> Setting global EAX outdoor");
		
		if (m_bInside)
			return (false); // set only when outside
	}
	if (nPreset==REVERB_PRESET_UNDERWATER)
	{
		m_bForceUnderwater = true;
	}
	else
		m_bForceUnderwater = false;

	return true;

}

// Preset management
bool CReverbManager::RegisterReverbPreset(char const* const pcPresetName/* =NULL */, CRYSOUND_REVERB_PROPERTIES const* const pReverbProperties/* =NULL */, CRYSOUND_ECHO_PROPERTIES const* const pEchoProperties/* =NULL */)
{
	//GUARD_HEAP;
	//FUNCTION_PROFILER( GetSystem(),PROFILE_SOUND );

	if(!m_ReverbEnable)
		return false;

	m_bNeedUpdate = true;

	//gEnv->pLog->LogToConsole("Registered Preset %s", sPresetName);

	assert(pcPresetName && pcPresetName[0]);
	assert(pReverbProperties && pEchoProperties);

	// add this Preset to the vector of Presets or update it if its already in there
	bool bPresetfound = false;
	for(ReverbPresetsIter It=m_ReverbPresets.begin(); It!=m_ReverbPresets.end(); ++It)
	{
		// update
		if((*It).sPresetName == pcPresetName)
		{
			(*It).EAX				= *pReverbProperties;
			(*It).EchoDSP		= *pEchoProperties;
			bPresetfound		= true;
			break;
		}
	}

	if(!bPresetfound)
	{
		SReverbInfo::SReverbPreset NewPreset;
		NewPreset.EAX					= *pReverbProperties;
		NewPreset.EchoDSP			= *pEchoProperties;
		NewPreset.sPresetName = pcPresetName;

		m_ReverbPresets.push_back(NewPreset);
	}

	return true;
}

bool CReverbManager::UnregisterReverbPreset(const char *sPresetName)
{
	//GUARD_HEAP;
	//	FUNCTION_PROFILER( GetSystem(),PROFILE_SOUND );

	if (!m_ReverbEnable)
		return false;

	m_bNeedUpdate = true;

	//gEnv->pLog->Log("Unregistered Preset %s", sPresetName);

	assert( sPresetName[0] );

	bool bPresetfound = false;
	do 
	{
		bPresetfound = UnregisterWeightedReverbEnvironment(sPresetName);
	} while (bPresetfound);

	bool bWeightedfound = false;
	for (ReverbPresetsIter It=m_ReverbPresets.begin(); It!=m_ReverbPresets.end(); ++It)
	{
		if ((*It).sPresetName == sPresetName)
		{
			// Delete the active Preset from the vector
			m_ReverbPresets.erase(It);
			bWeightedfound = true;
			break;
		}
	}
	return bWeightedfound;

}


//! Registers an EAX Preset Area with the current blending weight (0-1) as being active
//! morphing of several EAX Preset Areas is done internally
//! Registering the same Preset multiple time will only overwrite the old one
//bool CReverbManager::RegisterWeightedReverbEnvironment(const char *sPresetName, CRYSOUND_REVERB_PROPERTIES *pProps, bool bFullEffectWhenInside, int nFlags)
bool CReverbManager::RegisterWeightedReverbEnvironment(const char *sPresetName, EntityId RegID, bool bFullEffectWhenInside, uint32 nFlags)
{
	//GUARD_HEAP;
	//FUNCTION_PROFILER( GetSystem(),PROFILE_SOUND );

	if (!m_ReverbEnable)
		return false;

	m_bNeedUpdate = true;

	//gEnv->pLog->LogToConsole("Registered Preset %s", sPresetName);

	//assert(sPresetName != "");
	assert( sPresetName[0] );

	// add this Preset to the vector of active Presets or update it if its already in there
	SReverbInfo::SReverbPreset* pPreset = NULL;

	for (ReverbPresetsIter It=m_ReverbPresets.begin(); It!=m_ReverbPresets.end(); ++It)
	{
		if ((*It).sPresetName == sPresetName)
		{
			// Delete the active Preset from the vector
			pPreset = &(*It);
			break;
		}
	}

	// couldnt find Preset
	if (!pPreset)
		return false;

	bool bWeightedfound = false;

	for (ActiveReverbPresetAreasIter  It=m_vecActiveReverbPresetAreas.begin(); It!=m_vecActiveReverbPresetAreas.end(); ++It)
	{
		if ((*It).pReverbPreset->sPresetName == sPresetName && (*It).RegID == RegID)
		{
			(*It).bFullEffectWhenInside = bFullEffectWhenInside;
			(*It).pReverbPreset				  = pPreset;
			bWeightedfound							= true;
			break;
		}
	}

	//assert(!Presetfound);

	if (!bWeightedfound) 
	{
		SReverbInfo::SWeightedReverbPreset NewWeighted;

		NewWeighted.pReverbPreset = pPreset;
		NewWeighted.fWeight = 0; // always start with 0
		NewWeighted.bFullEffectWhenInside = bFullEffectWhenInside;
		NewWeighted.RegID = RegID;

		m_vecActiveReverbPresetAreas.push_back(NewWeighted);
	}
	return true;
}


//! Updates an EAX Preset Area with the current blending ratio (0-1)
bool CReverbManager::UpdateWeightedReverbEnvironment(const char *sPresetName, EntityId RegID, float fWeight)
{
	//GUARD_HEAP;
	//FUNCTION_PROFILER( GetISystem(),PROFILE_SOUND );

	if (!m_ReverbEnable)
		return false;

	m_bNeedUpdate = true;

	//gEnv->pLog->Log("Update Preset %s", sPresetName);

	//assert(sPresetName != "");
	assert( sPresetName[0] );
	assert(fWeight >= 0 && fWeight <= 1);

	// update this Preset to the vector of active Presets or add if its not in there yet
	bool bWeightedfound = false;
	for (ActiveReverbPresetAreasIter It=m_vecActiveReverbPresetAreas.begin(); It!=m_vecActiveReverbPresetAreas.end(); ++It)
	{
		if ((*It).pReverbPreset->sPresetName == sPresetName && (*It).RegID == RegID)
		{
			(*It).fWeight = fWeight;
			bWeightedfound = true;
			break;
		}
	}
	//assert(Presetfound);

	return bWeightedfound;
}


//! Unregisters an active EAX Preset Area 
bool CReverbManager::UnregisterWeightedReverbEnvironment(const char *sPresetName, EntityId RegID)
{
	//GUARD_HEAP;
//	FUNCTION_PROFILER( GetSystem(),PROFILE_SOUND );

	if (!m_ReverbEnable)
		return false;

	m_bNeedUpdate = true;

	//gEnv->pLog->Log("Unregistered Preset %s", sPresetName);

	//assert(sPresetName != "");
	assert( sPresetName[0] );

	bool bWeightedfound = false;
	for (ActiveReverbPresetAreasIter It=m_vecActiveReverbPresetAreas.begin(); It!=m_vecActiveReverbPresetAreas.end(); ++It)
	{
		// remove all occurences if RedID == 0, because then the whole preset gets removed
		if ((*It).pReverbPreset->sPresetName == sPresetName && ((*It).RegID == RegID || RegID == 0 || (*It).RegID == 0))
		{
			// Delete the active Preset from the vector
			m_vecActiveReverbPresetAreas.erase(It);
			bWeightedfound = true;
			break;
		}
	}
	//assert(Presetfound);

	return bWeightedfound;
}


//! Gets current EAX listener environment.
bool CReverbManager::GetCurrentReverbEnvironment(SOUND_REVERB_PRESETS &nPreset, CRYSOUND_REVERB_PROPERTIES &Props)
{
	nPreset=m_nLastEax;
	if (m_pLastEAXProps)
		memcpy(&Props,m_pLastEAXProps,sizeof(CRYSOUND_REVERB_PROPERTIES));
	else
		memset(&Props,0,sizeof(CRYSOUND_REVERB_PROPERTIES));

	return (true);
}

//////////////////////////////////////////////////////////////////////////
//ISound* CReverbManager::GetSound( tSoundID nSoundId )
//{
//	SRegisteredSound *pSoundSlot = FindRegisteredSound(nSoundId);
//	if (pSoundSlot)
//		return pSoundSlot->pSound;
//	return 0;
//}

//////////////////////////////////////////////////////////////////////////
CReverbManager::SRegisteredSound* CReverbManager::FindRegisteredSound( tSoundID nSoundId )
{
	RegisteredSoundsVec::iterator ItEnd = m_Sounds.end();
	for (RegisteredSoundsVec::iterator It = m_Sounds.begin(); It != ItEnd; ++It)
	{
		SRegisteredSound &slot = (*It);
		if (slot.nSoundID == nSoundId)
			return &slot;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
bool CReverbManager::RemoveRegisteredSound( tSoundID nSoundId )
{
	RegisteredSoundsVec::iterator ItEnd = m_Sounds.end();
	for (RegisteredSoundsVec::iterator It = m_Sounds.begin(); It != ItEnd; ++It)
	{
		SRegisteredSound &slot = (*It);
		if (slot.nSoundID == nSoundId)
		{
			m_Sounds.erase(It);
			return true;
		}
	}
	return false;
}
/*
bool CReverbManager::RegisterSound(ISound *pSound)
{
	SRegisteredSound* pNewSound = FindRegisteredSound(pSound->GetId());

	if (!pNewSound)
	{
		SRegisteredSound NewSound;

		NewSound.pSound = pSound;
		NewSound.nSoundID = pSound->GetId();

		m_Sounds.push_back(NewSound);
	}


	//m_Sounds.push_back(NewSound);

	//gEnv->pEntitySystem->GetAreaManager()->TriggerSound(pSound);

	return true;
}
*/
//int  CReverbManager::GetReverbInstance(ISound *pSound) const
//{
//	return 0;
//}
	
float CReverbManager::GetEnvironment(ISound *pSound)
{
	assert (pSound);
	if (!pSound)
		return 0.0f;

	return 0.0f; //CryMP: fixme
	/*
	SAreaManagerResult Results[10];

	bool bResult = gEnv->pEntitySystem->GetAreaManager()->QueryAreas(pSound->GetPosition(), Results, 10);

	int nPriority = -1; 
	float fEnvironment = 0;
	//char *sReverbPreset = NULL;
	bool bQuery = false;

	// Iterate through all areas vPos is inside
	int AIndex = 0;
	while (Results[AIndex].pArea)
	{
		if (Results[AIndex].bInside)
		{
			int nEntityNum = Results[AIndex].pArea->GetEntityAmount();
			for (int i=0; i<nEntityNum; ++i)
			{
				EntityId EID = Results[AIndex].pArea->GetEntityByIdx(i);
				IEntity* pEntity = gEnv->pEntitySystem->GetEntity(EID);

				// is this a ReverbVolume
				if (pEntity && (strcmp(pEntity->GetClass()->GetName(),"ReverbVolume") == 0))
				{
					SmartScriptTable props;
					IScriptTable* pScriptTable = pEntity->GetScriptTable();

					if (pScriptTable)
					{
						if (pScriptTable->GetValue("Properties", props) )
						{
							int bEnable = 0;
							float fThisEnvironment = 0;
							bQuery = props->GetValue("bEnabled", bEnable);
							bQuery = props->GetValue("Environment", fThisEnvironment);
							//bQuery = props->GetValue("reverbpresetReverbPreset", sReverbPreset);

							if (bEnable != 0 && nPriority < Results[AIndex].pArea->GetPriority())
							{
								fEnvironment = fThisEnvironment;
								nPriority = Results[AIndex].pArea->GetPriority();;
							}
						}
					}
				}
			}
		}
		++AIndex;
	}
	(void)bQuery;

	return fEnvironment;
	*/
}

//bool CReverbManager::RegisterPlayer(EntityId PlayerID)
//{
//	return true;
//}
//
//bool CReverbManager::UnregisterPlayer(EntityId PlayerID)
//{
//	return true;
//}

//////////////////////////////////////////////////////////////////////////
void CReverbManager::OnSoundEvent( ESoundCallbackEvent event,ISound *pSound )
{
	switch (event) {
	case SOUND_EVENT_ON_STOP:
		{
			RemoveRegisteredSound(pSound->GetId());
			break;
		}

	case SOUND_EVENT_ON_LOAD_FAILED:
		break;
	}
}
/*
//////////////////////////////////////////////////////////////////////////
void CReverbManager::OnAreaManagerEvent( EEntityEvent event, EntityId TriggerEntityID, IArea *pArea )
{
	if (!pArea)
		return;

	IListener *pListener = gEnv->pSoundSystem->GetListener(TriggerEntityID);
	bool bIsCamera = false;
	
	if (!pListener)
	{
		IEntity* pTriggerEntity = NULL;
		pTriggerEntity = gEnv->pEntitySystem->GetEntity(TriggerEntityID);
		bIsCamera = strcmp(pTriggerEntity->GetClass()->GetName(), "CameraSource") == 0;
	}

	int nNum = pArea->GetEntityAmount();

	for (int i=0; i<nNum; ++i)
	{
		EntityId ID = pArea->GetEntityByIdx(i);
		IEntity* pEntity = gEnv->pEntitySystem->GetEntity(ID);
		
		if (pEntity)
		{
			pEntity->GetClass()->GetName();
		}

	}

	if (pListener || bIsCamera)
	{
		switch (event) 
		{
		case ENTITY_EVENT_ENTERAREA:
			break;
		case ENTITY_EVENT_LEAVEAREA:
			break;
		case ENTITY_EVENT_ENTERNEARAREA:
			break;
		case ENTITY_EVENT_LEAVENEARAREA:
			break;
		case ENTITY_EVENT_MOVEINSIDEAREA:
			break;
		case ENTITY_EVENT_MOVENEARAREA:
			break;

		default:
			break;
		}
	}


}
*/

/***********************************************************************************************\
*
* Definition of the EAXMorph function - EAX3ListenerInterpolate
*
\***********************************************************************************************/

/*
EAX3ListenerInterpolate
lpStart			- Initial EAX 3 Listener parameters
lpFinish		- Final EAX 3 Listener parameters
flRatio			- Ratio Destination : Source (0.0 == Source, 1.0 == Destination)
lpResult		- Interpolated EAX 3 Listener parameters
bCheckValues	- Check EAX 3.0 parameters are in range, default = false (no checking)
*/
bool CReverbManager::EAX3ListenerInterpolate(CRYSOUND_REVERB_PROPERTIES *lpStart, CRYSOUND_REVERB_PROPERTIES *lpFinish,
																					 float flRatio, CRYSOUND_REVERB_PROPERTIES *lpResult, bool bCheckValues)
{

	//typedef float EAXVECTOR[3];
	EAXVECTOR StartVector, FinalVector;

	float flInvRatio;

	if (bCheckValues)
	{
		if (!CheckEAX3LP(lpStart))
			return false;

		if (!CheckEAX3LP(lpFinish))
			return false;
	}

	if (flRatio >= 1.0f)
	{
		memcpy(lpResult, lpFinish, sizeof(CRYSOUND_REVERB_PROPERTIES));
		return true;
	}
	else if (flRatio <= 0.0f)
	{
		memcpy(lpResult, lpStart, sizeof(CRYSOUND_REVERB_PROPERTIES));
		return true;
	}

	flInvRatio = (1.0f - flRatio);

	// Environment
	if (flRatio < 0.5f)
		lpResult->Environment = lpStart->Environment;	// (use Start Environment value)
	else
		lpResult->Environment = lpFinish->Environment;	// (use Finish Environment value)

	// Environment Size
	if (lpStart->EnvSize == lpFinish->EnvSize)
		lpResult->EnvSize = lpStart->EnvSize;
	else
		lpResult->EnvSize = (float)expf( (logf(lpStart->EnvSize) * flInvRatio) + (logf(lpFinish->EnvSize) * flRatio) );

	// Environment Diffusion
	if (lpStart->EnvDiffusion == lpFinish->EnvDiffusion)
		lpResult->EnvDiffusion = lpStart->EnvDiffusion;
	else
		lpResult->EnvDiffusion = (lpStart->EnvDiffusion * flInvRatio) + (lpFinish->EnvDiffusion * flRatio);

	// Room
	if (lpStart->Room == lpFinish->Room)
		lpResult->Room = lpStart->Room;
	else
		lpResult->Room = (int)( ((float)lpStart->Room * flInvRatio) + ((float)lpFinish->Room * flRatio) );

	// Room HF
	if (lpStart->RoomHF == lpFinish->RoomHF)
		lpResult->RoomHF = lpStart->RoomHF;
	else
		lpResult->RoomHF = (int)( ((float)lpStart->RoomHF * flInvRatio) + ((float)lpFinish->RoomHF * flRatio) );

	// Room LF
	if (lpStart->RoomLF == lpFinish->RoomLF)
		lpResult->RoomLF = lpStart->RoomLF;
	else
		lpResult->RoomLF = (int)( ((float)lpStart->RoomLF * flInvRatio) + ((float)lpFinish->RoomLF * flRatio) );

	// Decay Time
	if (lpStart->DecayTime == lpFinish->DecayTime)
		lpResult->DecayTime = lpStart->DecayTime;
	else
		lpResult->DecayTime = (float)expf( (logf(lpStart->DecayTime) * flInvRatio) + (logf(lpFinish->DecayTime) * flRatio) );

	// Decay HF Ratio
	if (lpStart->DecayHFRatio == lpFinish->DecayHFRatio)
		lpResult->DecayHFRatio = lpStart->DecayHFRatio;
	else
		lpResult->DecayHFRatio = (float)expf( (logf(lpStart->DecayHFRatio) * flInvRatio) + (logf(lpFinish->DecayHFRatio) * flRatio) );

	// Decay LF Ratio
	if (lpStart->DecayLFRatio == lpFinish->DecayLFRatio)
		lpResult->DecayLFRatio = lpStart->DecayLFRatio;
	else
		lpResult->DecayLFRatio = (float)expf( (logf(lpStart->DecayLFRatio) * flInvRatio) + (logf(lpFinish->DecayLFRatio) * flRatio) );

	// Reflections
	if (lpStart->Reflections == lpFinish->Reflections)
		lpResult->Reflections = lpStart->Reflections;
	else
		lpResult->Reflections = (int)( ((float)lpStart->Reflections * flInvRatio) + ((float)lpFinish->Reflections * flRatio) );

	// Reflections Delay
	if (lpStart->ReflectionsDelay == lpFinish->ReflectionsDelay)
		lpResult->ReflectionsDelay = lpStart->ReflectionsDelay;
	else
		lpResult->ReflectionsDelay = (float)expf( (logf(lpStart->ReflectionsDelay+0.0001f) * flInvRatio) + (logf(lpFinish->ReflectionsDelay+0.0001f) * flRatio) );

	// Reflections Pan

	// To interpolate the vector correctly we need to ensure that both the initial and final vectors vectors are clamped to a length of 1.0f
	StartVector.x = lpStart->ReflectionsPan[0];
	StartVector.y = lpStart->ReflectionsPan[1];
	StartVector.z = lpStart->ReflectionsPan[2];
	FinalVector.x = lpFinish->ReflectionsPan[0];
	FinalVector.y = lpFinish->ReflectionsPan[1];
	FinalVector.z = lpFinish->ReflectionsPan[2];

	Clamp(&StartVector);
	Clamp(&FinalVector);

	if (lpStart->ReflectionsPan[0] == lpFinish->ReflectionsPan[0])
		lpResult->ReflectionsPan[0] = lpStart->ReflectionsPan[0];
	else
		lpResult->ReflectionsPan[0] = FinalVector.x + (flInvRatio * (StartVector.x - FinalVector.x));

	if (lpStart->ReflectionsPan[1] == lpFinish->ReflectionsPan[1])
		lpResult->ReflectionsPan[1] = lpStart->ReflectionsPan[1];
	else
		lpResult->ReflectionsPan[1] = FinalVector.y + (flInvRatio * (StartVector.y - FinalVector.y));

	if (lpStart->ReflectionsPan[2] == lpFinish->ReflectionsPan[2])
		lpResult->ReflectionsPan[2] = lpStart->ReflectionsPan[2];
	else
		lpResult->ReflectionsPan[2] = FinalVector.z + (flInvRatio * (StartVector.z - FinalVector.z));

	// Reverb
	if (lpStart->Reverb == lpFinish->Reverb)
		lpResult->Reverb = lpStart->Reverb;
	else
		lpResult->Reverb = (int)( ((float)lpStart->Reverb * flInvRatio) + ((float)lpFinish->Reverb * flRatio) );

	// Reverb Delay
	if (lpStart->ReverbDelay == lpFinish->ReverbDelay)
		lpResult->ReverbDelay = lpStart->ReverbDelay;
	else
		lpResult->ReverbDelay = (float)expf( (logf(lpStart->ReverbDelay+0.0001f) * flInvRatio) + (logf(lpFinish->ReverbDelay+0.0001f) * flRatio) );

	// Reverb Pan

	// To interpolate the vector correctly we need to ensure that both the initial and final vectors are clamped to a length of 1.0f	
	StartVector.x = lpStart->ReverbPan[0];
	StartVector.y = lpStart->ReverbPan[1];
	StartVector.z = lpStart->ReverbPan[2];
	FinalVector.x = lpFinish->ReverbPan[0];
	FinalVector.y = lpFinish->ReverbPan[1];
	FinalVector.z = lpFinish->ReverbPan[2];

	Clamp(&StartVector);
	Clamp(&FinalVector);

	if (lpStart->ReverbPan[0] == lpFinish->ReverbPan[0])
		lpResult->ReverbPan[0] = lpStart->ReverbPan[0];
	else
		lpResult->ReverbPan[0] = FinalVector.x + (flInvRatio * (StartVector.x - FinalVector.x));

	if (lpStart->ReverbPan[1] == lpFinish->ReverbPan[1])
		lpResult->ReverbPan[1] = lpStart->ReverbPan[1];
	else
		lpResult->ReverbPan[1] = FinalVector.y + (flInvRatio * (StartVector.y - FinalVector.y));

	if (lpStart->ReverbPan[2] == lpFinish->ReverbPan[2])
		lpResult->ReverbPan[2] = lpStart->ReverbPan[2];
	else
		lpResult->ReverbPan[2] = FinalVector.z + (flInvRatio * (StartVector.z - FinalVector.z));

	// Echo Time
	if (lpStart->EchoTime == lpFinish->EchoTime)
		lpResult->EchoTime = lpStart->EchoTime;
	else
		lpResult->EchoTime = (float)expf( (logf(lpStart->EchoTime) * flInvRatio) + (logf(lpFinish->EchoTime) * flRatio) );

	// Echo Depth
	if (lpStart->EchoDepth == lpFinish->EchoDepth)
		lpResult->EchoDepth = lpStart->EchoDepth;
	else
		lpResult->EchoDepth = (lpStart->EchoDepth * flInvRatio) + (lpFinish->EchoDepth * flRatio);

	// Modulation Time
	if (lpStart->ModulationTime == lpFinish->ModulationTime)
		lpResult->ModulationTime = lpStart->ModulationTime;
	else
		lpResult->ModulationTime = (float)expf( (logf(lpStart->ModulationTime) * flInvRatio) + (logf(lpFinish->ModulationTime) * flRatio) );

	// Modulation Depth
	if (lpStart->ModulationDepth == lpFinish->ModulationDepth)
		lpResult->ModulationDepth = lpStart->ModulationDepth;
	else
		lpResult->ModulationDepth = (lpStart->ModulationDepth * flInvRatio) + (lpFinish->ModulationDepth * flRatio);

	// Air Absorption HF
	if (lpStart->AirAbsorptionHF == lpFinish->AirAbsorptionHF)
		lpResult->AirAbsorptionHF = lpStart->AirAbsorptionHF;
	else
		lpResult->AirAbsorptionHF = (lpStart->AirAbsorptionHF * flInvRatio) + (lpFinish->AirAbsorptionHF * flRatio);

	// HF Reference
	if (lpStart->HFReference == lpFinish->HFReference)
		lpResult->HFReference = lpStart->HFReference;
	else
		lpResult->HFReference = (float)expf( (logf(lpStart->HFReference) * flInvRatio) + (logf(lpFinish->HFReference) * flRatio) );

	// LF Reference
	if (lpStart->LFReference == lpFinish->LFReference)
		lpResult->LFReference = lpStart->LFReference;
	else
		lpResult->LFReference = (float)expf( (logf(lpStart->LFReference) * flInvRatio) + (logf(lpFinish->LFReference) * flRatio) );

	// Room Rolloff Factor
	if (lpStart->RoomRolloffFactor == lpFinish->RoomRolloffFactor)
		lpResult->RoomRolloffFactor = lpStart->RoomRolloffFactor;
	else
		lpResult->RoomRolloffFactor = (lpStart->RoomRolloffFactor * flInvRatio) + (lpFinish->RoomRolloffFactor * flRatio);

	// Flags
	lpResult->Flags = (lpStart->Flags | lpFinish->Flags);

	// Clamp Delays
	if (lpResult->ReflectionsDelay > EAXLISTENER_MAXREFLECTIONSDELAY)
		lpResult->ReflectionsDelay = EAXLISTENER_MAXREFLECTIONSDELAY;

	if (lpResult->ReverbDelay > EAXLISTENER_MAXREVERBDELAY)
		lpResult->ReverbDelay = EAXLISTENER_MAXREVERBDELAY;

	return true;
}

/*
EchoDSPInterpolate
rEchoPropertiesStart	- Initial EAX 3 Listener parameters
rEchoPropertiesFinish	- Final EAX 3 Listener parameters
fBlendRatio						- Ratio Destination : Source (0.0 == Source, 1.0 == Destination)
rEchoPropertiesResult	- Interpolated EAX 3 Listener parameters
*/
bool CReverbManager::EchoDSPInterpolate(CRYSOUND_ECHO_PROPERTIES const& rEchoPropertiesStart,
																				CRYSOUND_ECHO_PROPERTIES const& rEchoPropertiesFinish,
																				float fBlendRatio,
																				CRYSOUND_ECHO_PROPERTIES& rEchoPropertiesResult)
{
	float fInvRatio = 0.0f;

	if(fBlendRatio >= 1.0f)
	{
		rEchoPropertiesResult = rEchoPropertiesFinish;
		return true;
	}
	else if (fBlendRatio <= 0.0f)
	{
		rEchoPropertiesResult = rEchoPropertiesStart;
		return true;
	}

	fInvRatio = (1.0f - fBlendRatio);

	// Delay
	if(rEchoPropertiesStart.fDelay == rEchoPropertiesFinish.fDelay)
		rEchoPropertiesResult.fDelay = rEchoPropertiesStart.fDelay;
	else
		rEchoPropertiesResult.fDelay = (rEchoPropertiesStart.fDelay * fInvRatio) + (rEchoPropertiesFinish.fDelay * fBlendRatio);

	// DecayRatio
	if(rEchoPropertiesStart.fDecayRatio == rEchoPropertiesFinish.fDecayRatio)
		rEchoPropertiesResult.fDecayRatio = rEchoPropertiesStart.fDecayRatio;
	else
		rEchoPropertiesResult.fDecayRatio = (rEchoPropertiesStart.fDecayRatio * fInvRatio) + (rEchoPropertiesFinish.fDecayRatio * fBlendRatio);

	// DryMix
	if(rEchoPropertiesStart.fDryMix == rEchoPropertiesFinish.fDryMix)
		rEchoPropertiesResult.fDryMix = rEchoPropertiesStart.fDryMix;
	else
		rEchoPropertiesResult.fDryMix = (rEchoPropertiesStart.fDryMix * fInvRatio) + (rEchoPropertiesFinish.fDryMix * fBlendRatio);

	// WetMix
	if(rEchoPropertiesStart.fWetMix == rEchoPropertiesFinish.fWetMix)
		rEchoPropertiesResult.fWetMix = rEchoPropertiesStart.fWetMix;
	else
		rEchoPropertiesResult.fWetMix = (rEchoPropertiesStart.fWetMix * fInvRatio) + (rEchoPropertiesFinish.fWetMix * fBlendRatio);

	return true;
}

/*
CheckEAX3LP
Checks that the parameters in the EAX 3 Listener Properties structure are in-range
*/
bool CReverbManager::CheckEAX3LP(CRYSOUND_REVERB_PROPERTIES *lpEAX3LP)
{
	if ( (lpEAX3LP->Room < EAXLISTENER_MINROOM) || (lpEAX3LP->Room > EAXLISTENER_MAXROOM) )
		return false;

	if ( (lpEAX3LP->RoomHF < EAXLISTENER_MINROOMHF) || (lpEAX3LP->RoomHF > EAXLISTENER_MAXROOMHF) )
		return false;

	if ( (lpEAX3LP->RoomLF < EAXLISTENER_MINROOMLF) || (lpEAX3LP->RoomLF > EAXLISTENER_MAXROOMLF) )
		return false;

	if ( ((lpEAX3LP->Environment < EAXLISTENER_MINENVIRONMENT) || (lpEAX3LP->Environment > EAXLISTENER_MAXENVIRONMENT)) && lpEAX3LP->Environment != -1)
		return false;

	if ( (lpEAX3LP->EnvSize < EAXLISTENER_MINENVIRONMENTSIZE) || (lpEAX3LP->EnvSize > EAXLISTENER_MAXENVIRONMENTSIZE) )
		return false;

	if ( (lpEAX3LP->EnvDiffusion < EAXLISTENER_MINENVIRONMENTDIFFUSION) || (lpEAX3LP->EnvDiffusion > EAXLISTENER_MAXENVIRONMENTDIFFUSION) )
		return false;

	if ( (lpEAX3LP->DecayTime < EAXLISTENER_MINDECAYTIME) || (lpEAX3LP->DecayTime > EAXLISTENER_MAXDECAYTIME) )
		return false;

	if ( (lpEAX3LP->DecayHFRatio < EAXLISTENER_MINDECAYHFRATIO) || (lpEAX3LP->DecayHFRatio > EAXLISTENER_MAXDECAYHFRATIO) )
		return false;

	if ( (lpEAX3LP->DecayLFRatio < EAXLISTENER_MINDECAYLFRATIO) || (lpEAX3LP->DecayLFRatio > EAXLISTENER_MAXDECAYLFRATIO) )
		return false;

	if ( (lpEAX3LP->Reflections < EAXLISTENER_MINREFLECTIONS) || (lpEAX3LP->Reflections > EAXLISTENER_MAXREFLECTIONS) )
		return false;

	if ( (lpEAX3LP->ReflectionsDelay < EAXLISTENER_MINREFLECTIONSDELAY) || (lpEAX3LP->ReflectionsDelay > EAXLISTENER_MAXREFLECTIONSDELAY) )
		return false;

	if ( (lpEAX3LP->Reverb < EAXLISTENER_MINREVERB) || (lpEAX3LP->Reverb > EAXLISTENER_MAXREVERB) )
		return false;

	if ( (lpEAX3LP->ReverbDelay < EAXLISTENER_MINREVERBDELAY) || (lpEAX3LP->ReverbDelay > EAXLISTENER_MAXREVERBDELAY) )
		return false;

	if ( (lpEAX3LP->EchoTime < EAXLISTENER_MINECHOTIME) || (lpEAX3LP->EchoTime > EAXLISTENER_MAXECHOTIME) )
		return false;

	if ( (lpEAX3LP->EchoDepth < EAXLISTENER_MINECHODEPTH) || (lpEAX3LP->EchoDepth > EAXLISTENER_MAXECHODEPTH) )
		return false;

	if ( (lpEAX3LP->ModulationTime < EAXLISTENER_MINMODULATIONTIME) || (lpEAX3LP->ModulationTime > EAXLISTENER_MAXMODULATIONTIME) )
		return false;

	if ( (lpEAX3LP->ModulationDepth < EAXLISTENER_MINMODULATIONDEPTH) || (lpEAX3LP->ModulationDepth > EAXLISTENER_MAXMODULATIONDEPTH) )
		return false;

	if ( (lpEAX3LP->AirAbsorptionHF < EAXLISTENER_MINAIRABSORPTIONHF) || (lpEAX3LP->AirAbsorptionHF > EAXLISTENER_MAXAIRABSORPTIONHF) )
		return false;

	if ( (lpEAX3LP->HFReference < EAXLISTENER_MINHFREFERENCE) || (lpEAX3LP->HFReference > EAXLISTENER_MAXHFREFERENCE) )
		return false;

	if ( (lpEAX3LP->LFReference < EAXLISTENER_MINLFREFERENCE) || (lpEAX3LP->LFReference > EAXLISTENER_MAXLFREFERENCE) )
		return false;

	if ( (lpEAX3LP->RoomRolloffFactor < EAXLISTENER_MINROOMROLLOFFFACTOR) || (lpEAX3LP->RoomRolloffFactor > EAXLISTENER_MAXROOMROLLOFFFACTOR) )
		return false;

	// TODO check thoise Flags
	//if (lpEAX3LP->Flags & EAXLISTENERFLAGS_RESERVED)
	//return false;

	// TODO Dissusion and Density (XBOX) not checked

	return true;
}

/*
Clamp
Clamps the length of the vector to 1.0f
*/
void CReverbManager::Clamp(EAXVECTOR *eaxVector)
{
	float flMagnitude;
	float flInvMagnitude;

	flMagnitude = (float)sqrt((eaxVector->x*eaxVector->x) + (eaxVector->y*eaxVector->y) + (eaxVector->z*eaxVector->z));

	if (flMagnitude <= 1.0f)
		return;

	flInvMagnitude = 1.0f / flMagnitude;

	eaxVector->x *= flInvMagnitude;
	eaxVector->y *= flInvMagnitude;
	eaxVector->z *= flInvMagnitude;
}

//defines a distance function on ReverbProperties 
float CReverbManager::Distance(CRYSOUND_REVERB_PROPERTIES *lpProp1, CRYSOUND_REVERB_PROPERTIES *lpProp2)
{
	return abs(lpProp1->DecayTime - lpProp2->DecayTime);
}

void CReverbManager::SerializeState( TSerialize ser )
{
	string sTempName;

	if (ser.IsWriting())
	{
		// active moods
		int nActiveReverbsNum = m_vecActiveReverbPresetAreas.size();
		ser.Value("ReverbCount",nActiveReverbsNum);

		ActiveReverbPresetAreasIter  ItEnd = m_vecActiveReverbPresetAreas.end();
		for (ActiveReverbPresetAreasIter  It=m_vecActiveReverbPresetAreas.begin(); It!=ItEnd; ++It)
		{
			ser.BeginGroup("ActiveReverb");

			sTempName = (*It).pReverbPreset->sPresetName.c_str();
			ser.Value("ReverbName", sTempName);
			ser.Value("Weight",(*It).fWeight);
			ser.Value("EffectInside",(*It).bFullEffectWhenInside);
			ser.Value("RegID", (*It).RegID);

			ser.EndGroup();
		}
	}
	else
	{
		int nActiveReverbsNum = 0;
		ser.Value("ReverbCount",nActiveReverbsNum);

		for (int i=0; i<nActiveReverbsNum; ++i)
		{
			ser.BeginGroup("ActiveReverb");

			ser.Value("ReverbName",sTempName);
			SReverbInfo::SReverbPreset* pPreset = NULL;

			ReverbPresetsIter ItEnd = m_ReverbPresets.end();
			for (ReverbPresetsIter It=m_ReverbPresets.begin(); It!=ItEnd; ++It)
			{
				if ((*It).sPresetName == sTempName.c_str() )
				{
					pPreset = &(*It);
					break;
				}
			}

			if (pPreset) // reverb exist
			{
				SReverbInfo::SWeightedReverbPreset NewWeighted;

				NewWeighted.pReverbPreset = pPreset;
				ser.Value("Weight",NewWeighted.fWeight);
				ser.Value("EffectInside",NewWeighted.bFullEffectWhenInside);
				ser.Value("RegID", NewWeighted.RegID);
				m_vecActiveReverbPresetAreas.push_back(NewWeighted);
			}
							
			ser.EndGroup();
		}

		m_bNeedUpdate = true;

	}

}

//////////////////////////////////////////////////////////////////////////
int CReverbManager::OnObstructionTest(const EventPhys* pEvent)
{
	EventPhysRWIResult const* const pRWIResult = (EventPhysRWIResult*)pEvent;
	if (pRWIResult->iForeignData == PHYS_FOREIGN_ID_SOUND_REVERB_OBSTRUCTION)
	{
		unsigned int const nIdx = (unsigned int)pRWIResult->pForeignData;
		if (nIdx < DIRECTIONS_COUNT)
		{
			if (pRWIResult->nHits)
				m_fObstructionTestResults[nIdx] = pRWIResult->pHits[0].dist;
			else
				m_fObstructionTestResults[nIdx] = FLT_MAX;
		}
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
// Description:
//	Every time a reverb manager gets updated this method is called.
//	Here we shoot only one ray of DIRECTIONS_COUNT and evaluate the distance in OnObstructionTest.
//	Once we reached the last vector we will start from the beginning
//	The length of the ray being shot is only the distance sound can travel within 0.4 seconds divided by 2
//	Early and late reflection together cover only 0.4 seconds
void CReverbManager::SendRay()
{
	if (m_pSoundSystem && m_pSoundSystem->g_fReverbDynamic)
	{
		uint32 const nActiveListenerCount = m_pSoundSystem->GetNumActiveListeners();
		if (nActiveListenerCount == 1)
		{
			// There is just 1 active listener
			IListener const* const pListener = m_pSoundSystem->GetListener(LISTENERID_STANDARD);
			if (pListener)
			{
				assert (pListener->GetActive());
				m_v3Origin = pListener->GetPosition();

				// Calculate the distance sound travels within 0.4 seconds
				float const fDistance = SONIC_SPEED_METER_PER_SEC * 0.2f; // Use only have the way, sound needs to travel back and forth
				gEnv->pPhysicalWorld->RayWorldIntersection(m_v3Origin, m_v3ObstructionTestVecs[m_nSentIdx]*fDistance, ent_static, rwi_pierceability0|rwi_stop_at_pierceable|rwi_queue, NULL, 1, NULL, 0, (void*)m_nSentIdx, PHYS_FOREIGN_ID_SOUND_REVERB_OBSTRUCTION);

				if(++m_nSentIdx == DIRECTIONS_COUNT)
					m_nSentIdx = 0;
			}
		}
		else if (nActiveListenerCount > 1)
		{
			// There are more than 1 active listeners
			// TODO: This case needs to be handled
			assert(0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CReverbManager::DrawDynamicReverbDebugInfo(IRenderer* const pRenderer)
{
	IRenderAuxGeom* const pAuxGeom = pRenderer->GetIRenderAuxGeom();
	pAuxGeom->SetRenderFlags(e_Def3DPublicRenderflags);
	ColorB oColor;
	float const fDistance = SONIC_SPEED_METER_PER_SEC * 0.2f;

	// Determine the shortest distance
	float fShortestDistance = FLT_MAX;
	unsigned int nShortestIdx = -1;
	for (unsigned int i = 0; i < DIRECTIONS_COUNT; ++i)
	{
		if (m_fObstructionTestResults[i] < fShortestDistance)
		{
			fShortestDistance	= m_fObstructionTestResults[i];
			nShortestIdx			= i;
		}
	}

	// Now draw all spheres+rays and the closest one red
	for (unsigned int i = 0; i < DIRECTIONS_COUNT; ++i)
	{
		if (i != nShortestIdx)
			oColor.set(255, 255, 0, 255); // Yellow
		else
			oColor.set(255, 0, 0, 255);		// Red

		// Do the actual drawing
		Vec3 const v3Temp1(m_v3ObstructionTestVecs[i]*m_fObstructionTestResults[i]);
		Vec3 const v3Temp2(m_v3Origin+v3Temp1);
		pAuxGeom->DrawSphere(v3Temp2, 0.2f, oColor);
		pAuxGeom->DrawLine(m_v3Origin, oColor, m_v3Origin+m_v3ObstructionTestVecs[i]*fDistance, oColor, 3.0f);
	}
}

void CReverbManager::GetMemoryUsage( ICrySizer *pSizer ) const
{
	SIZER_COMPONENT_NAME(pSizer, "ReverbManager");
	pSizer->AddObject(this, sizeof(*this));	
	pSizer->AddObject(m_ReverbPresets);	
	pSizer->AddObject(m_vecActiveReverbPresetAreas);	
	pSizer->AddObject(m_Sounds);		
}