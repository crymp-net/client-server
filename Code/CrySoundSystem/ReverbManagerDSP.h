////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ReverbManagerDSP.h
//  Version:     v1.00
//  Created:     23/9/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: DSP implementation of a ReverbManager.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __REVERBMANAGERDSP_H__
#define __REVERBMANAGERDSP_H__

#include "ReverbManager.h"
#pragma once

#include "eax.h"

struct IAudioDevice;
struct IReverbInstance;

class CReverbManagerDSP : public CReverbManager
{

public:
	CReverbManagerDSP(void);
	~CReverbManagerDSP(void);

	//////////////////////////////////////////////////////////////////////////
	// Initialization
	//////////////////////////////////////////////////////////////////////////

	void Init(IAudioDevice *pAudioDevice, int nInstanceNumber, CSoundSystem* pSoundSystem);
	bool SelectReverb(int nReverbType);

	void Release();

	//////////////////////////////////////////////////////////////////////////
	// Information
	//////////////////////////////////////////////////////////////////////////

	// writes output to screen in debug
	void DrawInformation(IRenderer* pRenderer, float xpos, float ypos);

	//////////////////////////////////////////////////////////////////////////
	// Management
	//////////////////////////////////////////////////////////////////////////

	// needs to be called regularly
	virtual bool Update(bool bInside);

	//! returns boolean if hardware reverb (EAX) is used or not
	bool UseHardwareVoices() {return false;}


private:

	//SWeightedEAXPreset					m_sTempEaxPreset;
	//bool												m_bInside;

	IReverbInstance							*m_pReverbInstance;

};
#endif