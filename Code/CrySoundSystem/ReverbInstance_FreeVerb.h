////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ReverbInstance_FreeVerb.h
//  Version:     v1.00
//  Created:     12/10/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: FreeVerb implementation of IReverbInstance
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __REVERBINSTANCE_FREEVERB_H__
#define __REVERBINSTANCE_FREEVERB_H__

#pragma once
#include "IReverbInstance.h"
#include "FmodEx/inc/fmod.hpp"

struct IAudioDevice;

class CReverbInstance_FreeVerb : public IReverbInstance
{

public:
	CReverbInstance_FreeVerb(IAudioDevice *pAudioDevice);
	~CReverbInstance_FreeVerb(void);


	//////////////////////////////////////////////////////////////////////////
	// Usage
	//////////////////////////////////////////////////////////////////////////
	bool Activate();
	bool Deactivate();

	//////////////////////////////////////////////////////////////////////////
	// Management
	//////////////////////////////////////////////////////////////////////////

	// needs to be called regularly
	bool Update(CRYSOUND_REVERB_PROPERTIES *pProps);

	//////////////////////////////////////////////////////////////////////////
	// Information
	//////////////////////////////////////////////////////////////////////////

	// writes output to screen in debug
	//void DrawInformation(IRenderer* pRenderer, float xpos, float ypos) = 0;

private:

	FMOD::System	*m_pCSEX;
	FMOD::DSP			*m_DSPReverb;
	FMOD_RESULT		m_ExResult;

	void FmodErrorOutput(const char * sDescription, ESoundLogType LogType = eSLT_Message);

};
#endif