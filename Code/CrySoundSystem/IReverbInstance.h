////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   IReverbInstancer.h
//  Version:     v1.00
//  Created:     12/10/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: IReverbInstance offers the abstraction interface to control
//							 a single reverb effect. It will translate a set of EAX
//              properties to the parameter it is using internally
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __IREVERBINSTANCE_H__
#define __IREVERBINSTANCE_H__

#pragma once

#include "CryCommon/CrySoundSystem/ISound.h"
#include "CryCommon/CrySoundSystem/IReverbManager.h"

struct IReverbInstance
{
	//////////////////////////////////////////////////////////////////////////
	// Usage
	//////////////////////////////////////////////////////////////////////////
	virtual bool Activate() = 0;
	virtual bool Deactivate() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Management
	//////////////////////////////////////////////////////////////////////////

	// needs to be called regularly
	virtual bool Update(CRYSOUND_REVERB_PROPERTIES *pProps) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Information
	//////////////////////////////////////////////////////////////////////////

	// writes output to screen in debug
	//virtual void DrawInformation(IRenderer* pRenderer, float xpos, float ypos) = 0;



};
#endif
