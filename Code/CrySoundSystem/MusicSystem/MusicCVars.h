////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   MusicCVars.h
//  Version:     v1.00
//  Created:     19/3/2007 by Tomas.
//  Compilers:   Visual Studio.NET 2005
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __MusicCVars_h__
#define __MusicCVars_h__
#pragma once

//////////////////////////////////////////////////////////////////////////
// Console variables local to MusicSystem.
//////////////////////////////////////////////////////////////////////////
class CMusicCVars
{
public:

	CMusicCVars();
	virtual ~CMusicCVars();

	int			g_nDebugMusic;
	int			g_nMusicEnable;
	int			g_nMusicMaxPatterns;
	int			g_nMusicFormat;
	int			g_nMusicCategory;
	int			g_nMusicProfiling;
	float		g_fMusicSpeakerFrontVolume;
	float		g_fMusicSpeakerBackVolume;
	float		g_fMusicSpeakerCenterVolume;
	float		g_fMusicSpeakerSideVolume;
	float		g_fMusicSpeakerLFEVolume;
	
	static void Init( struct IConsole *pConsole );

};

#endif // __EntityCVars_h__
