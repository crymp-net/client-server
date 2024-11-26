////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ReverbManagerEAX.cpp
//  Version:     v1.00
//  Created:     15/8/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: EAX implementation of a ReverbManager.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#ifdef SOUNDSYSTEM_USE_FMODEX400

#include "ReverbManagerEAX.h"
#include "CryCommon/CryRenderer/IRenderer.h"
#include "IAudioDevice.h"
#include "FmodEx/inc/fmod.hpp"
#include "SoundSystem.h"
#include "Sound.h" // TODO remove this and clean up include conflict in CSoundSystem and CSound

//////////////////////////////////////////////////////////////////////////
// Initialization
//////////////////////////////////////////////////////////////////////////

CReverbManagerEAX::CReverbManagerEAX()
:	m_bHardwareReverb(false),
	m_pSoundSystem(NULL),
	m_ExResult(FMOD_OK),
	m_pDSPEchoUnit(NULL)
{
}

CReverbManagerEAX::~CReverbManagerEAX()
{
}

//////////////////////////////////////////////////////////////////////////
void CReverbManagerEAX::Init(IAudioDevice* pAudioDevice, int nInstanceNumber, CSoundSystem* pSoundSystem)
{
	// Call the base class' implementation first
	CReverbManager::Init(pAudioDevice, nInstanceNumber, pSoundSystem);

	assert(pAudioDevice);
	assert(pSoundSystem);

	m_pAudioDevice = pAudioDevice;
	m_pSoundSystem = pSoundSystem;
}

//////////////////////////////////////////////////////////////////////////
bool CReverbManagerEAX::SelectReverb(int nReverbType)
{
	if (!m_pAudioDevice)
		return false;

	m_bNeedUpdate = true;
	if (nReverbType == REVERB_TYPE_NONE)
	{
		m_ReverbEnable = false;
		m_vecActiveReverbPresetAreas.clear();
		Update(false);
	}
	else
		m_ReverbEnable = true;

	switch(nReverbType) 
	{
	case REVERB_TYPE_HARDWARE:
		{
			m_bHardwareReverb = true;
			if(m_pDSPEchoUnit)
				m_pDSPEchoUnit->setBypass(true);
		}
		break;
	case REVERB_TYPE_SOFTWARE_LOW:
		{
			m_bHardwareReverb = false;

			if(m_pDSPEchoUnit && m_pSoundSystem && m_pSoundSystem->g_nReverbEchoDSP == 1)
				m_pDSPEchoUnit->setBypass(false);
		}
		break;
	case REVERB_TYPE_SOFTWARE_HIGH:
		{
			m_bHardwareReverb = false;

			if(m_pDSPEchoUnit && m_pSoundSystem && m_pSoundSystem->g_nReverbEchoDSP == 1)
				m_pDSPEchoUnit->setBypass(false);
		}
		break;
	default:
		return false;
	}

	return true;

}

void CReverbManagerEAX::Release()
{
	m_vecActiveReverbPresetAreas.clear();
}

//////////////////////////////////////////////////////////////////////////
// Information
//////////////////////////////////////////////////////////////////////////

// writes output to screen in debug
void CReverbManagerEAX::DrawInformation(IRenderer* pRenderer, float xpos, float ypos)
{
	//ActiveEaxPresetAreasIter It=m_vecActiveEaxPresetAreas.begin();
	//pRenderer->Draw2dLabel(xpos, ypos, 1.5, fColorGreen, false, "EAX - %s",(*It).sPresetName);
	float fColorGreen[4] ={0.0f, 1.0f, 0.0f, 0.7f};
	float fColorGray[4]	 ={0.3f, 0.3f, 0.3f, 0.7f};
	float fColorTemp[4]	 ={0.0f, 0.0f, 0.0f, 0.7f};
	float fColorBlue[4]	 ={0.4f, 0.4f, 1.0f, 0.7f};
	float fColorRed[4]	 ={1.0f, 0.0f, 0.0f, 0.7f};

	if (!m_ReverbEnable)
	{
		pRenderer->Draw2dLabel(xpos, ypos, 1.1f, fColorGray, false, "Reverb disabled");
		ypos += 11;
		return;
	}

	pRenderer->Draw2dLabel(xpos, ypos, 1.1f, fColorBlue, false, "EAX - %s ", m_sTempEaxPreset.pReverbPreset->sPresetName.c_str());
	ypos += 11;

	ActiveReverbPresetAreasIterConst  IterEnd = m_vecActiveReverbPresetAreas.end();
	for (ActiveReverbPresetAreasIterConst  It=m_vecActiveReverbPresetAreas.begin(); It!=IterEnd; ++It)
	{
		float *pCurColor = fColorGreen;

		if ((*It).bFullEffectWhenInside)
			pCurColor = fColorRed;

		fColorTemp[0] = (*It).fWeight * pCurColor[0] + (1.0f - (*It).fWeight) * fColorGray[0];
		fColorTemp[1] = (*It).fWeight * pCurColor[1] + (1.0f - (*It).fWeight) * fColorGray[1];
		fColorTemp[2] = (*It).fWeight * pCurColor[2] + (1.0f - (*It).fWeight) * fColorGray[2];

		stack_string sTemp;
		sTemp.Format("Reverb - %s", (*It).pReverbPreset->sPresetName.c_str());
		if(m_pSoundSystem && m_pSoundSystem->g_fReverbDynamic)
			sTemp.Format("%s ReverbReflectionDelay: %f ReverbDelay: %f", sTemp.c_str(), m_sTempEaxPreset.pReverbPreset->EAX.ReflectionsDelay, m_sTempEaxPreset.pReverbPreset->EAX.ReverbDelay);

		if(m_pDSPEchoUnit)
		{
			float fValues[4] = {0.0f};
			m_pDSPEchoUnit->getParameter(FMOD_DSP_ECHO_DELAY,				&fValues[0], NULL, 0);
			m_pDSPEchoUnit->getParameter(FMOD_DSP_ECHO_DECAYRATIO,	&fValues[1], NULL, 0);
			m_pDSPEchoUnit->getParameter(FMOD_DSP_ECHO_DRYMIX,			&fValues[2], NULL, 0);
			m_pDSPEchoUnit->getParameter(FMOD_DSP_ECHO_WETMIX,			&fValues[3], NULL, 0);
			sTemp.Format("%s Echo - Delay: %.2f DecayRatio: %.2f DryMix: %.2f WetMix: %.2f", sTemp.c_str(), fValues[0], fValues[1], fValues[2], fValues[3]);
		}

		pRenderer->Draw2dLabel(xpos, ypos, 1.1f, fColorTemp, false, sTemp.c_str());
		ypos += 11;
	}
}


//////////////////////////////////////////////////////////////////////////
// Management
//////////////////////////////////////////////////////////////////////////

// needs to be called regularly
bool CReverbManagerEAX::Update(bool bInside)
{
	if (!m_bNeedUpdate)
		return true;

	m_bNeedUpdate = false;

	//if (!m_bAreaManagerListener && gEnv->pEntitySystem)
	//{
	//	gEnv->pEntitySystem->GetAreaManager()->AddEventListener(this);
	//	m_bAreaManagerListener = true;
	//}

	// Once this method is called send a new ray cast
	SendRay();

	if (!m_ReverbEnable)
	{
		m_sTempEaxPreset.pReverbPreset->EAX = m_EAXOffPreset;
		m_sTempEaxPreset.pReverbPreset->sPresetName = "Off";
		return false;
	}
	else
	{

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
			CRYSOUND_REVERB_PROPERTIES pPropsUnderWater = CRYSOUND_REVERB_UNDERWATER;
			m_sTempEaxPreset.pReverbPreset->EAX = pPropsUnderWater;
			m_sTempEaxPreset.pReverbPreset->sPresetName = "UnderWater";
		}
		else
		{
			float SumOfWeight = 0;

			// get sum of weights
			ActiveReverbPresetAreasIterConst  ItEnd = m_vecActiveReverbPresetAreas.end();
			for (ActiveReverbPresetAreasIterConst It=m_vecActiveReverbPresetAreas.begin(); It!=ItEnd; ++It)
			{
				SumOfWeight += (*It).fWeight;
			}

			// Verifying Reverb Values enabled/disables
			bool Checkresults = true;
			m_bInside = bInside;

			// if there is only one Preset just interpolate directly with its weight
			if (m_vecActiveReverbPresetAreas.size()==1)
			{
				ActiveReverbPresetAreasIterConst  It = m_vecActiveReverbPresetAreas.begin();
				m_sTempEaxPreset.fWeight										= (*It).fWeight;
				m_sTempEaxPreset.pReverbPreset->EAX					= (*It).pReverbPreset->EAX;
				m_sTempEaxPreset.pReverbPreset->EchoDSP			= (*It).pReverbPreset->EchoDSP;
				m_sTempEaxPreset.pReverbPreset->sPresetName = (*It).pReverbPreset->sPresetName;
			}
			else
			{
				for (ActiveReverbPresetAreasIterConst It=m_vecActiveReverbPresetAreas.begin(); It!=ItEnd; ++It)
				{
					// Copy first element to the temp preset
					if (It==m_vecActiveReverbPresetAreas.begin())
					{
						m_sTempEaxPreset.fWeight										= (*It).fWeight;
						m_sTempEaxPreset.pReverbPreset->EAX					= (*It).pReverbPreset->EAX;
						m_sTempEaxPreset.pReverbPreset->EchoDSP			= (*It).pReverbPreset->EchoDSP;
						m_sTempEaxPreset.pReverbPreset->sPresetName	= "Reverb_Mix";
					}
					else // start interpolating at the 2nd element
					{
						// only blend if needed
						if ((*It).fWeight > 0) 
						{
							float BlendRatio = 0;
							BlendRatio = (*It).fWeight / SumOfWeight;
							EAX3ListenerInterpolate(&m_sTempEaxPreset.pReverbPreset->EAX, &((*It).pReverbPreset->EAX), BlendRatio, &m_sTempEaxPreset.pReverbPreset->EAX, Checkresults);
							EchoDSPInterpolate(m_sTempEaxPreset.pReverbPreset->EchoDSP, (*It).pReverbPreset->EchoDSP, BlendRatio, m_sTempEaxPreset.pReverbPreset->EchoDSP);
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
				{
					EAX3ListenerInterpolate(&m_EAXOutdoorPreset, &m_sTempEaxPreset.pReverbPreset->EAX, SumOfWeight, &m_sTempEaxPreset.pReverbPreset->EAX, Checkresults);
					//EAX3ListenerInterpolate(&m_EAXOutdoorPreset, &m_sTempEaxPreset.EAX, SumOfWeight, &m_sTempEaxPreset.EAX, Checkresults);
					m_sTempEaxPreset.pReverbPreset->sPresetName = m_sOutdoorMix;
				}
				else 
				{
					EAX3ListenerInterpolate(&m_EAXIndoorPreset, &m_sTempEaxPreset.pReverbPreset->EAX, SumOfWeight, &m_sTempEaxPreset.pReverbPreset->EAX, Checkresults);
					//EAX3ListenerInterpolate(&m_EAXIndoorPreset, &m_sTempEaxPreset.EAX, SumOfWeight, &m_sTempEaxPreset.EAX, Checkresults);
					m_sTempEaxPreset.pReverbPreset->sPresetName = m_sIndoorMix;
				}

			}
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
#ifdef SOUNDSYSTEM_USE_FMODEX400

	//TODO Reverb_Properties are different in FmodEX
	if (m_pAudioDevice)
	{
		m_sTempEaxPreset.pReverbPreset->EAX.Instance = 0;
		FMOD::System *pEx = (FMOD::System*)m_pAudioDevice->GetSoundLibrary();
		//FMOD_REVERB_PROPERTIES *pProp = (FMOD_REVERB_PROPERTIES*)&(m_sTempEaxPreset.EAX);
		FMOD_REVERB_PROPERTIES *pProp = (FMOD_REVERB_PROPERTIES*)&(m_sTempEaxPreset.pReverbPreset->EAX);
		pProp->Instance = 0;
		pProp->Flags = FMOD_REVERB_FLAGS_DEFAULT;
		// TODO evaluate if test against old value is cheaper (pEx->getReverbProperties())

		if (m_pSoundSystem && m_pSoundSystem->g_fReverbDynamic)
		{
			// Determine the shortest distance
			float fShortestDistance = FLT_MAX;
			for (unsigned int i = 0; i < DIRECTIONS_COUNT; ++i)
				if (m_fObstructionTestResults[i] < fShortestDistance)
					fShortestDistance	= m_fObstructionTestResults[i];

			if (fShortestDistance > 0.0f && fShortestDistance < FLT_MAX)
			{
				// If the shortest distance is within the max value for ReverbReflection (0.3 seconds)
				// set the new reflection value accordingly
				float fNewReflectionValue = SONIC_SPEED_METER_PER_SEC * 0.15f;
				if (fShortestDistance <= fNewReflectionValue)
				{
					fNewReflectionValue				= ((fShortestDistance / fNewReflectionValue) * 0.3f) * m_pSoundSystem->g_fReverbDynamic;
					pProp->ReflectionsDelay		= min(fNewReflectionValue, 0.3f);
				}
				else
					pProp->ReflectionsDelay	= 0.3f; // Max

				//pProp->ReverbDelay				=; // TODO: Implement this!	
			}
			else
				pProp->ReflectionsDelay	= 0.3f; // Max
		}

		// Overrides the early and late reverb delays in case the user supplied custom values
		float const fReverbDelay = m_pSoundSystem->g_fReverbDelay;
		float const fReverbReflectionDelay = m_pSoundSystem->g_fReverbReflectionDelay;
		if (fReverbDelay >= 0.0f)
			m_sTempEaxPreset.pReverbPreset->EAX.ReverbDelay = fReverbDelay;
		if (fReverbReflectionDelay >= 0.0f)
			m_sTempEaxPreset.pReverbPreset->EAX.ReflectionsDelay = fReverbReflectionDelay;

		FMOD_RESULT Result = pEx->setReverbProperties(pProp);

		if(m_pDSPEchoUnit)
		{
			m_ExResult = m_pDSPEchoUnit->setParameter(FMOD_DSP_ECHO_DELAY,			m_sTempEaxPreset.pReverbPreset->EchoDSP.fDelay);
			assert(m_ExResult == FMOD_OK);
			m_ExResult = m_pDSPEchoUnit->setParameter(FMOD_DSP_ECHO_DECAYRATIO,	m_sTempEaxPreset.pReverbPreset->EchoDSP.fDecayRatio);
			assert(m_ExResult == FMOD_OK);
			m_ExResult = m_pDSPEchoUnit->setParameter(FMOD_DSP_ECHO_DRYMIX,			m_sTempEaxPreset.pReverbPreset->EchoDSP.fDryMix);
			assert(m_ExResult == FMOD_OK);
			m_ExResult = m_pDSPEchoUnit->setParameter(FMOD_DSP_ECHO_WETMIX,			m_sTempEaxPreset.pReverbPreset->EchoDSP.fWetMix);
			assert(m_ExResult == FMOD_OK);
		}

		//if (Result != FMOD_OK)
			//int a = 0;

	}
#endif

	if(m_pSoundSystem && !m_bHardwareReverb)
	{
		switch(m_pSoundSystem->g_nReverbEchoDSP)
		{
		case 0:
			{
				bool bEchoUnitBypassing = false;
				if(m_pDSPEchoUnit)
				{
					// Turn off the echo unit i.e. bypass it
					m_ExResult = m_pDSPEchoUnit->setBypass(true);
					assert(m_ExResult == FMOD_OK);
				}
			}
			break;
		case 1:
			{
				if(!m_pDSPEchoUnit)
					CreateEchoDSP();
				else
				{
					// Turn on the echo unit i.e. include it in the process chain again
					m_ExResult = m_pDSPEchoUnit->setBypass(false);
					assert(m_ExResult == FMOD_OK);
				}
			}
			break;
		}
	}

	m_bNeedUpdate = false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
void CReverbManagerEAX::CreateEchoDSP()
{
	if(m_pAudioDevice)
	{
		FMOD::System* pFMODSystem = (FMOD::System*)m_pAudioDevice->GetSoundLibrary();
		if(pFMODSystem)
		{
			FMOD::DSP* pDSPHeadUnit = NULL;
			m_ExResult = pFMODSystem->getDSPHead(&pDSPHeadUnit);
			if(pDSPHeadUnit)
			{
				FMOD::DSP* pDSPTargetUnit = NULL;
				m_ExResult = pDSPHeadUnit->getInput(0, &pDSPTargetUnit, NULL);
				if(pDSPTargetUnit)
				{
					int iNumInputs = 0;
					m_ExResult = pDSPTargetUnit->getNumInputs(&iNumInputs);

					for(int i = 0; i < iNumInputs; ++i)
					{
						FMOD::DSP*	pDSPReverbUnit = NULL;
						m_ExResult = pDSPTargetUnit->getInput(i, &pDSPReverbUnit, NULL);

						FMOD_DSP_TYPE eDSPType = FMOD_DSP_TYPE_UNKNOWN;
						if(pDSPReverbUnit)
						{
							m_ExResult = pDSPReverbUnit->getType(&eDSPType);
							if(eDSPType == FMOD_DSP_TYPE_SFXREVERB)
							{
								m_ExResult = pFMODSystem->createDSPByType(FMOD_DSP_TYPE_ECHO, &m_pDSPEchoUnit);
								if(m_ExResult == FMOD_OK)
								{
									m_ExResult = pDSPTargetUnit->disconnectFrom(pDSPReverbUnit);
									assert(m_ExResult == FMOD_OK);
									m_ExResult = m_pDSPEchoUnit->addInput(pDSPReverbUnit, NULL);
									assert(m_ExResult == FMOD_OK);
									m_ExResult = pDSPTargetUnit->addInput(m_pDSPEchoUnit, NULL);
									assert(m_ExResult == FMOD_OK);
									m_ExResult = m_pDSPEchoUnit->setActive(true);
									assert(m_ExResult == FMOD_OK);

									break;
								}
							}
						}
					}
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CReverbManagerEAX::DrawDynamicReverbDebugInfo(IRenderer* const pRenderer)
{
	CReverbManager::DrawDynamicReverbDebugInfo(pRenderer);

	float const fDarkGreen[4] = {0.0f, 0.5f, 0.2f, 1.0f};
	pRenderer->Draw2dLabel(500.0f, 400.0f, 2.0f, fDarkGreen, false, "ReflectionDelay: %f", m_sTempEaxPreset.pReverbPreset->EAX.ReflectionsDelay);
}

#endif // SOUNDSYSTEM_USE_FMODEX400
