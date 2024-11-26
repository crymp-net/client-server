////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ReverbInstance_FreeVerb.cpp
//  Version:     v1.00
//  Created:     12/10/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: FreeVerb implementation of IReverbInstance
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#ifdef SOUNDSYSTEM_USE_FMODEX400

#include "ReverbInstance_FreeVerb.h"
//#include "IRenderer.h"
#include "IAudioDevice.h"
#include "FmodEx/inc/fmod_errors.h"


void CReverbInstance_FreeVerb::FmodErrorOutput(const char * sDescription, ESoundLogType LogType)
{
	gEnv->pSoundSystem->Log(LogType, "<Sound> Sound-FmodEx-Reverb: %s (%d) %s\n", sDescription, m_ExResult, FMOD_ErrorString(m_ExResult));
}


//////////////////////////////////////////////////////////////////////////
// Initialization
//////////////////////////////////////////////////////////////////////////

CReverbInstance_FreeVerb::CReverbInstance_FreeVerb(IAudioDevice *pAudioDevice)
{
	m_pCSEX = 0;

	if (pAudioDevice)
		m_pCSEX = (FMOD::System*) pAudioDevice->GetSoundLibrary();

	m_DSPReverb = 0;
	m_ExResult = FMOD_OK;

}

CReverbInstance_FreeVerb::~CReverbInstance_FreeVerb()
{
	Deactivate();
}

//////////////////////////////////////////////////////////////////////////
// Usage
//////////////////////////////////////////////////////////////////////////
bool CReverbInstance_FreeVerb::Activate()
{

	if (m_pCSEX && !m_DSPReverb)
	{
		bool bActive = false;
		m_ExResult = m_pCSEX->createDSPByType(FMOD_DSP_TYPE_REVERB, &m_DSPReverb);
		
		if (m_ExResult != FMOD_OK)
		{
			FmodErrorOutput("create freeverb reverb failed! ", eSLT_Warning);
			return false;
		}

		if (m_DSPReverb)
		{
			m_ExResult = m_pCSEX->addDSP(m_DSPReverb, 0);
			if (m_ExResult != FMOD_OK)
			{
				FmodErrorOutput("add freeverb to DSP chain failed! ", eSLT_Warning);
				return false;
			}
			
			m_ExResult = m_DSPReverb->getActive(&bActive);
			if (m_ExResult != FMOD_OK)
			{
				FmodErrorOutput("activate freeverb failed! ", eSLT_Warning);
				return false;
			}

			bActive = bActive;
		}
	}

	int i = 0;
	while (m_ExResult == FMOD_OK && m_DSPReverb)
	{
		char sName[MAXCHARBUFFERSIZE];
		char sLabel[MAXCHARBUFFERSIZE];
		char sDescription[MAXCHARBUFFERSIZE];
		int nDescLen = MAXCHARBUFFERSIZE;

		float fMin = 0.0f;
		float fMax = 0.0f;

		m_ExResult = m_DSPReverb->getParameterInfo(i, sName, sLabel, sDescription, nDescLen, &fMin, &fMax);
		++i;

	}

	return (true);

}

bool CReverbInstance_FreeVerb::Deactivate()
{

	if (m_DSPReverb)
		m_ExResult = m_DSPReverb->remove();

	m_DSPReverb = 0;
	// unload Plugin

	return (m_ExResult == FMOD_OK);
}

//////////////////////////////////////////////////////////////////////////
// Management
//////////////////////////////////////////////////////////////////////////

// needs to be called regularly
bool CReverbInstance_FreeVerb::Update(CRYSOUND_REVERB_PROPERTIES *pProps)
{
	//FMOD_DSP_REVERB_ROOMSIZE
	//	Roomsize. 0.0 to 1.0. Default = 0.5 
	//FMOD_DSP_REVERB_DAMP
	//	Damp. 0.0 to 1.0. Default = 0.5 
	//FMOD_DSP_REVERB_WETMIX
	//	Wet mix. 0.0 to 1.0. Default = 0.33 
	//FMOD_DSP_REVERB_DRYMIX
	//	Dry mix. 0.0 to 1.0. Default = 0.66 
	//FMOD_DSP_REVERB_WIDTH
	//	Width. 0.0 to 1.0. Default = 1.0 
	//FMOD_DSP_REVERB_MODE
	//	Mode. 0 (normal), 1 (freeze). Default = 0 


	m_ExResult = m_DSPReverb->setParameter(FMOD_DSP_REVERB_ROOMSIZE, pProps->EnvSize/50.0f);
	if (m_ExResult != FMOD_OK)
		m_ExResult = m_ExResult;

	m_ExResult = m_DSPReverb->setParameter(FMOD_DSP_REVERB_DAMP, pProps->DecayTime/20.0f);
	if (m_ExResult != FMOD_OK)
		m_ExResult = m_ExResult;

	return (m_ExResult == FMOD_OK);

}


//////////////////////////////////////////////////////////////////////////
// Information
//////////////////////////////////////////////////////////////////////////

// writes output to screen in debug
//void CReverbManagerDSP::DrawInformation(IRenderer* pRenderer, float xpos, float ypos)
//{
	//ActiveEaxPresetAreasIter It=m_vecActiveEaxPresetAreas.begin();
	//pRenderer->Draw2dLabel(xpos, ypos, 1.5, fColorGreen, false, "EAX - %s",(*It).sPresetName);
//	float fColorGreen[4]	={0.0f, 1.0f, 0.0f, 0.7f};
//	for (ActiveEaxPresetAreasIter It=m_vecActiveEaxPresetAreas.begin(); It!=m_vecActiveEaxPresetAreas.end(); ++It)
//	{
//		pRenderer->Draw2dLabel(xpos, ypos, 1.5, fColorGreen, false, "EAX - %s",(*It).sPresetName);
//		ypos+=15;
//	}
//}

#endif