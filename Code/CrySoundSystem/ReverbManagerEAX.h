////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ReverbManagerEAX.h
//  Version:     v1.00
//  Created:     15/8/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: EAX implementation of a ReverbManager.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __REVERBMANAGEREAX_H__
#define __REVERBMANAGEREAX_H__

#include "ReverbManager.h"
#include "eax.h"

#ifdef SOUNDSYSTEM_USE_FMODEX400
#include "FmodEx/inc/fmod_event.hpp"
#endif // SOUNDSYSTEM_USE_FMODEX400

struct IAudioDevice;
class CSoundSystem;

class CReverbManagerEAX : public CReverbManager
{

public:
	CReverbManagerEAX(void);
	~CReverbManagerEAX(void);

	//////////////////////////////////////////////////////////////////////////
	// Initialization
	//////////////////////////////////////////////////////////////////////////

	void Init(IAudioDevice* pAudioDevice, int nInstanceNumber, CSoundSystem* pSoundSystem);
	bool SelectReverb(int nReverbType);

	void Release();

	//////////////////////////////////////////////////////////////////////////
	// Information
	//////////////////////////////////////////////////////////////////////////

	// writes output to screen in debug
	void DrawInformation(IRenderer* pRenderer, float xpos, float ypos);

	// Draws the ray cast lines plus spheres at the collision points
	virtual void	DrawDynamicReverbDebugInfo(IRenderer* const pRenderer);

	//////////////////////////////////////////////////////////////////////////
	// Management
	//////////////////////////////////////////////////////////////////////////

	// needs to be called regularly
	virtual bool Update(bool bInside);

	//! returns boolean if hardware reverb (EAX) is used or not
	bool UseHardwareVoices() {return m_bHardwareReverb;}

private:

	void				CreateEchoDSP();

	bool m_bHardwareReverb;
	//bool												m_bInside;
	//bool												m_bNeedUpdate;

	CSoundSystem*	m_pSoundSystem;

#ifdef SOUNDSYSTEM_USE_FMODEX400
	FMOD_RESULT		m_ExResult;
	FMOD::DSP*		m_pDSPEchoUnit;
#endif // SOUNDSYSTEM_USE_FMODEX400
};
#endif