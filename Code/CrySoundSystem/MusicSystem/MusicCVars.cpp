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

#include "CrySoundSystem/StdAfx.h"
#include "MusicCVars.h"

CMusicCVars::CMusicCVars()
{
	REGISTER_CVAR2("s_DebugMusic", &g_nDebugMusic, 0, VF_DUMPTODISK,
		"Changes music-debugging verbosity level.\n"
		"Usage: s_DebugMusic [0/4]\n"
		"Default is 0 (off). Set to 1 (up to 4) to debug music.");	




	REGISTER_CVAR2("s_MusicEnable", &g_nMusicEnable, 1, VF_DUMPTODISK,"enable/disable music");


	REGISTER_CVAR2("s_MusicMaxPatterns", &g_nMusicMaxPatterns, 12, VF_DUMPTODISK,
		"Max simultaneously playing music patterns.");






	int nDefaultMusicFormat = 1;


	REGISTER_CVAR2( "s_MusicFormat", &g_nMusicFormat, nDefaultMusicFormat, VF_DUMPTODISK,
		"Format used for streaming music data."
		"Usage: s_MusicFormat 1.0\n"
		"	0: (AD)PCM\n"
		"	1: OGG\n"
		"	4: FSB\n"
		"Default is PC: 1 (OGG), Xbox360: 4 (FSB), PS3: 4 (FSB)");	

	REGISTER_CVAR2("s_MusicCategory", &g_nMusicCategory, 1, VF_CHEAT,
		"Toggles adding the music sound under the music category of the eventsystem.\n"
		"Usage: s_MusicCategory [0/1]\n"
		"Default is 1 (on).");

	REGISTER_CVAR2("s_MusicProfiling", &g_nMusicProfiling, 0, VF_CHEAT,
		"Toggles profiling of music calls.\n"
		"Usage: s_MusicProfiling [0/1]\n"
		"Default is 0 (off).");

	REGISTER_CVAR2("s_MusicSpeakerFrontVolume", &g_fMusicSpeakerFrontVolume, 1.0f, VF_DUMPTODISK,
		"Sets the volume of the front speakers.\n"
		"Usage: s_MusicSpeakerFrontVolume 1.0"
		"Default is 1.0.");	
	
	REGISTER_CVAR2("s_MusicSpeakerBackVolume", &g_fMusicSpeakerBackVolume, 0.0f,VF_DUMPTODISK,
		"Sets the volume of the back speakers.\n"
		"Usage: s_MusicSpeakerBackVolume 0.3"
		"Default is 0.0.");	
	
	REGISTER_CVAR2("s_MusicSpeakerCenterVolume", &g_fMusicSpeakerCenterVolume, 0.0f, VF_DUMPTODISK,
		"Sets the volume of the center speakers (front and back).\n"
		"Usage: s_MusicSpeakerCenterVolume 0.0"
		"Default is 0.0.");	

	REGISTER_CVAR2("s_MusicSpeakerSideVolume", &g_fMusicSpeakerSideVolume, 0.5f, VF_DUMPTODISK,
		"Sets the volume of the side speakers.\n"
		"Usage: s_MusicSpeakerSideVolume 0.2"
		"Default is 0.5.");	
	
	REGISTER_CVAR2("s_MusicSpeakerLFEVolume", &g_fMusicSpeakerLFEVolume, 0.5f, VF_DUMPTODISK,
		"Sets the volume of the LFE speaker.\n"
		"Usage: s_MusicSpeakerLFEVolume 0.2"
		"Default is 0.5.");	

}

CMusicCVars::~CMusicCVars()
{
	IConsole* pConsole = gEnv->pConsole;

	assert(pConsole);

	pConsole->UnregisterVariable("s_DebugMusic");
	pConsole->UnregisterVariable("s_MusicEnable");
	pConsole->UnregisterVariable("s_MusicMaxPatterns");
	pConsole->UnregisterVariable("s_MusicFormat");
	pConsole->UnregisterVariable("s_MusicProfiling");
	pConsole->UnregisterVariable("s_MusicCategory");

	pConsole->UnregisterVariable("s_MusicSpeakerFrontVolume");
	pConsole->UnregisterVariable("s_MusicSpeakerBackVolume");
	pConsole->UnregisterVariable("s_MusicSpeakerCenterVolume");
	pConsole->UnregisterVariable("s_MusicSpeakerSideVolume");
	pConsole->UnregisterVariable("s_MusicSpeakerLFEVolume");


}