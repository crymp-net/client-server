////////////////////////////////////////////////////////////////////////////
//
//  CryEngine Source File.
//  Copyright (C), Crytek, 1999-2010.
//
// -------------------------------------------------------------------------
//  File name:   PCMExporterDSP
//  Version:     v1.00
//  Created:     23/3/2010 by Thomas.
//  Description: This is the implementation of the PCMExporterDSP class
//
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef PCM_EXPORTER_DSP_H_INCLUDED
#define PCM_EXPORTER_DSP_H_INCLUDED

#include "PoolAllocSystem.h"

#if defined SOUNDSYSTEM_USE_FMODEX400
	#include <FmodEx/inc/fmod.hpp>
#endif // SOUNDSYSTEM_USE_FMODEX400





// Forward declarations
struct IReadPCMDataListener;

class CPCMExporterDSP : public CPoolAllocSystem
{
public:

	CPCMExporterDSP();
	~CPCMExporterDSP();

	bool const		Create();
	void					AddListener(IReadPCMDataListener* const pListener, void const* pUserData = NULL);
	void					RemoveListener(IReadPCMDataListener const* const pListener);
	size_t const	GetListenerCount() const { return m_apListeners.size(); }

#if defined SOUNDSYSTEM_USE_FMODEX400
	FMOD::DSP* const	GetDSPEffect() const { return m_pDSPUnit; }
#endif // SOUNDSYSTEM_USE_FMODEX400

private:

	CPCMExporterDSP(CPCMExporterDSP const&);						// Copy protection
	CPCMExporterDSP& operator=(CPCMExporterDSP const&);	// Copy protection

#if defined SOUNDSYSTEM_USE_FMODEX400
	static FMOD_RESULT		F_CALLBACK FMOD_DSP_CREATECALLBACK_OWN(FMOD_DSP_STATE* pDSPState);
	static FMOD_RESULT		F_CALLBACK FMOD_DSP_READCALLBACK_OWN(	FMOD_DSP_STATE* pDSPState,
																															float* pfInBuffer,
																															float* pfOutBuffer,
																															int unsigned nLength,
																															int nInChannels,
																															int nOutChannels);

	FMOD_RESULT		m_ExResult;
	FMOD::DSP*		m_pDSPUnit;
#endif // SOUNDSYSTEM_USE_FMODEX400

	void				UpdateListeners(float const* const pfBuffer, int unsigned const nLengthInSamples, int const nChannels);

	struct SListenerData
	{
		SListenerData():pListener(NULL), pUserData(NULL){}

		IReadPCMDataListener* pListener;
		void const* pUserData;
	};

	typedef std::vector<SListenerData> TPCMDataListenerVector;
	TPCMDataListenerVector  m_apListeners;
	CryCriticalSection      m_oListenersCS;
};

#endif // PCM_EXPORTER_DSP_H_INCLUDED
