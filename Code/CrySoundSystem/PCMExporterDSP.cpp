#include "StdAfx.h"
#include "PCMExporterDSP.h"
#include "ISoundMoodManager.h"

#if defined SOUNDSYSTEM_USE_FMODEX400
	#include <FmodEx/inc/fmod_event.hpp>
	#include <FmodEx/inc/fmod_dsp.h>
	#include "ISound.h"
	#include "IAudioDevice.h"
	#define IS_FMODERROR (m_ExResult != FMOD_OK ) 
#endif // SOUNDSYSTEM_USE_FMODEX400

/*
    Location of embedded dsp read callback SPU code.
*/




//////////////////////////////////////////////////////////////////////////
CPCMExporterDSP::CPCMExporterDSP()
#if defined SOUNDSYSTEM_USE_FMODEX400
:	m_pDSPUnit(NULL)
#endif // SOUNDSYSTEM_USE_FMODEX400
{
}

//////////////////////////////////////////////////////////////////////////
CPCMExporterDSP::~CPCMExporterDSP()
{
#if defined SOUNDSYSTEM_USE_FMODEX400
	if (m_pDSPUnit)
	{
		// Prevent the read callback from accessing a dangling user data pointer by first deactivating the unit.
		m_pDSPUnit->setBypass(true);
		m_pDSPUnit->setActive(false);
		m_pDSPUnit->setUserData(NULL);
		m_pDSPUnit->release();
	}
#endif // SOUNDSYSTEM_USE_FMODEX400
}

//////////////////////////////////////////////////////////////////////////
bool const CPCMExporterDSP::Create()
{
#if defined SOUNDSYSTEM_USE_FMODEX400

#if !defined(PS3)
	FMOD_DSP_DESCRIPTION oDSPDescription =
	{
		"PCM Exporter DSP Unit",												// Name of the unit to be displayed in the network.
		1,																							// Plugin writer's version number.
		0,																							// Number of channels. Use 0 to process whatever number of channels is currently in the network. >0 would be mostly used if the unit is a unit that only generates sound.
		&CPCMExporterDSP::FMOD_DSP_CREATECALLBACK_OWN,	// Create callback. This is called when DSP unit is created. Can be null.
		0,																							// Release callback. This is called just before the unit is freed so the user can do any cleanup needed for the unit. Can be null.
		0,																							// Reset callback. This is called by the user to reset any history buffers that may need resetting for a filter, when it is to be used or re-used for the first time to its initial clean state. Use to avoid clicks or artifacts.
		&CPCMExporterDSP::FMOD_DSP_READCALLBACK_OWN,		// Read callback. Processing is done here. Can be null.
		0,																							// Set position callback. This is called if the unit wants to update its position info but not process data, or reset a cursor position internally if it is reading data from a certain source. Can be null.
		0,																							// Number of parameters used in this filter. The user finds this with DSP::getNumParameters
		0,																							// Variable number of parameter structures.
		0,																							// This is called when the user calls DSP::setParameter. Can be null.
		0,																							// This is called when the user calls DSP::getParameter. Can be null.
		0,																							// This is called when the user calls DSP::showConfigDialog. Can be used to display a dialog to configure the filter. Can be null.
		0,																							// Width of config dialog graphic if there is one. 0 otherwise.
		0,																							// Height of config dialog graphic if there is one. 0 otherwise.
		(void*)this																			// Optional. Specify 0 to ignore. This is user data to be attached to the DSP unit during creation. Access via DSP::getUserData.
	};
















#endif

	ISoundSystem const* const pSoundSystem = (ISoundSystem*)gEnv->pSoundSystem;
	if (pSoundSystem)
	{
		IAudioDevice* const pAudioDevice = pSoundSystem->GetIAudioDevice();
		if (pAudioDevice)
		{
			FMOD::System* const pFMODSystem = (FMOD::System*)pAudioDevice->GetSoundLibrary();
			if (pFMODSystem)
			{
				m_ExResult = pFMODSystem->createDSP(&oDSPDescription, &m_pDSPUnit);
				if (m_ExResult == FMOD_OK)
				{
					m_ExResult = m_pDSPUnit->setActive(true);

					assert(m_ExResult == FMOD_OK);
					return m_ExResult == FMOD_OK;
				}
			}
		}
	}
#endif // SOUNDSYSTEM_USE_FMODEX400

	return false;
}

#if defined SOUNDSYSTEM_USE_FMODEX400
//////////////////////////////////////////////////////////////////////////
FMOD_RESULT F_CALLBACK CPCMExporterDSP::FMOD_DSP_CREATECALLBACK_OWN(FMOD_DSP_STATE* pDSPState)
{




	return FMOD_OK;
}

//////////////////////////////////////////////////////////////////////////
FMOD_RESULT F_CALLBACK CPCMExporterDSP::FMOD_DSP_READCALLBACK_OWN(FMOD_DSP_STATE* pDSPState,
																																	float* pfInBuffer,
																																	float* pfOutBuffer,
																																	int unsigned nLength,
																																	int nInChannels,
																																	int nOutChannels)
{
	// Just pass the data through to the output buffer without any modification!
	int unsigned const nTotalSize = nLength*nInChannels*sizeof(float);
	memcpy(pfOutBuffer, pfInBuffer, nTotalSize);

#if !defined(PS3)
	// Now notify our listeners
	FMOD::DSP* const pInstance = (FMOD::DSP*)pDSPState->instance;
	void* pUserData;
	pInstance->getUserData(&pUserData);
	if (pUserData)
	{
		// TODO: Make sure pUserData is truly of type CPCMExporterDSP
		CPCMExporterDSP* const pPCMExporterDSP = (CPCMExporterDSP*)pUserData;
		if (pPCMExporterDSP)
			pPCMExporterDSP->UpdateListeners(pfInBuffer, nLength, nInChannels);
	}


#endif

	return FMOD_OK;
}
#endif // SOUNDSYSTEM_USE_FMODEX400

//////////////////////////////////////////////////////////////////////////
void CPCMExporterDSP::AddListener(IReadPCMDataListener* const pListener, void const* pUserData /* = NULL */)
{
	// Protect access to m_apListeners as it can be accessed by several threads.
	CryAutoCriticalSection oCS(m_oListenersCS);

	TPCMDataListenerVector::const_iterator ItEnd = m_apListeners.end();
	for (TPCMDataListenerVector::iterator it = m_apListeners.begin(); it != ItEnd; ++it)
	{
		if ((*it).pListener == pListener)
			return;
	}

	SListenerData oListenerData;
	oListenerData.pListener	= pListener;
	oListenerData.pUserData	= pUserData;
	m_apListeners.push_back(oListenerData);
}

//////////////////////////////////////////////////////////////////////////
void CPCMExporterDSP::RemoveListener(IReadPCMDataListener const* const pListener)
{
	// Protect access to m_apListeners as it can be accessed by several threads.
	CryAutoCriticalSection oCS(m_oListenersCS);

	TPCMDataListenerVector::const_iterator ItEnd = m_apListeners.end();
	for (TPCMDataListenerVector::iterator it = m_apListeners.begin(); it != ItEnd; ++it)
	{
		if ((*it).pListener == pListener)
		{
			m_apListeners.erase(it);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CPCMExporterDSP::UpdateListeners(float const* const pfBuffer, int unsigned const nLengthInSamples, int const nChannels)
{
	// Protect access to m_apListeners as it can be accessed by several threads.
	CryAutoCriticalSection oCS(m_oListenersCS);

	int unsigned const nCount = m_apListeners.size();
	for (unsigned int i = 0; i < nCount; ++i)
		m_apListeners[i].pListener->OnReadPCMData(pfBuffer, nLengthInSamples, nChannels, m_apListeners[i].pUserData);
}
