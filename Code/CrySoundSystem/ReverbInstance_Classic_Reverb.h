////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ReverbInstance_Classic_Reverb.h
//  Version:     v1.00
//  Created:     12/10/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: Classic_Reverb implementation of IReverbInstance
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __REVERBINSTANCE_CLASSIC_REVERB_H__
#define __REVERBINSTANCE_CLASSICREVERBB_H__

#pragma once
#include "IReverbInstance.h"
#include "FmodEx/inc/fmod.hpp"

struct IAudioDevice;

class CReverbInstance_Classic_Reverb : public IReverbInstance
{

public:
	CReverbInstance_Classic_Reverb(IAudioDevice *pAudioDevice);
	~CReverbInstance_Classic_Reverb(void);


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

	FMOD::System		*m_pCSEX;
	FMOD::DSP				*m_DSPReverb;
	FMOD_RESULT			m_ExResult;
	unsigned int		m_nHandle;

	void FmodErrorOutput(const char * sDescription, ESoundLogType LogType = eSLT_Message);


};
#endif