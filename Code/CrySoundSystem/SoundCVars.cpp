#include "StdAfx.h"
#include "SoundCVars.h"
#include "SoundSystem.h"
#include "CryCommon/CrySoundSystem/IReverbManager.h"

//////////////////////////////////////////////////////////////////////////
void OnCVarChangedCallback(ICVar* pCVar)
{
	// This is called from the main thread
	// on tear down we get our soundsystem swapped with the null implementation.. fixes crash on quit
	if (gEnv->pSoundSystem && !gEnv->pSoundSystem->GetInterfaceExtended()->IsNullImplementation())
	{
		CSoundSystem* const pSoundSystem = static_cast<CSoundSystem* const>(gEnv->pSoundSystem);

		pSoundSystem->OnCVarChanged(pCVar);
	}
}

//////////////////////////////////////////////////////////////////////////
CSoundCVars::CSoundCVars()
: g_nAllowNotCachedAccess(0),
  g_nAllowSoundBankPatching(0),
	g_nAllowMusicFileCaching(0),
	g_nAllowStreamFileCaching(0),
	g_nGameReverbManagerPause(0),
	g_nNumLoadingThreadsToUse(0),
	g_nEffectsAffectReverb(0),
	g_nDistanceSphereSemantic(0),
	g_autoMpSoundNameFixup(0),
	g_nReverbDebugDraw(0),
	g_nReverbType(0),
	g_nReverbEchoDSP(0),
	g_fLoadRequestsDisplayTime(0.0f),
	g_fGameMasterVolume(0.0f),
	g_fNoFocusVolume(0.0f),
	g_fReverbDynamic(0.0f),
	g_fReverbReflectionDelay(0.0f),
	g_fReverbDelay(0.0f),
	g_fSFXVolume(0.0f),
	g_fGameSFXVolume(0.0f),
	g_fMusicVolume(0.0f),
	g_fGameMusicVolume(0.0f),
	g_fDialogVolume(0.0f),
	g_fGameDialogVolume(0.0f),
#ifdef INCLUDE_AUDIO_PRODUCTION_CODE
	g_nFileCacheManagerDebugFilter(0),
	g_nFileCacheManagerDebugLog(0),
#endif // INCLUDE_AUDIO_PRODUCTION_CODE
	g_sAudioPreloadsFile(NULL)
{
}

//////////////////////////////////////////////////////////////////////////
CSoundCVars::~CSoundCVars()
{
}

//////////////////////////////////////////////////////////////////////////
void CSoundCVars::RegisterVariables()
{
	size_t nFileCacheManagerSize   = 0;
	size_t nAllowNotCachedAccess   = 0;
	size_t nAllowSoundBankPatching = 0;
	size_t nNumLoadingThreadsToUse = 0;

#if defined(WIN32) || defined(WIN64)
	nFileCacheManagerSize   = 80*1024; // 80 MiB on PC
	nAllowNotCachedAccess   = 1;
	nAllowSoundBankPatching = 1;
	nNumLoadingThreadsToUse = 2;















#endif

	REGISTER_CVAR2("s_FileCacheManagerSize", &g_nFileCacheManagerSize, nFileCacheManagerSize, VF_REQUIRE_APP_RESTART,
		"Sets the size in KiB the AFCM will allocate from the heap.\n"
		"Usage: s_FileCacheManagerSize [0/...]\n" 
		"Default PC: 81920 (80 MiB), PS3: 23552 (23 MiB), XBox360: 23552 (23 MiB)\n");

	REGISTER_CVAR2("s_AllowNotCachedAccess", &g_nAllowNotCachedAccess, nAllowNotCachedAccess, VF_NULL,
		"Controls whether to allow sound load requests that are not cached in the AFCM or not.\n"
		"Usage: s_AllowNotCachedAccess [0/1]\n"
		"0: OFF (disallowing access to uncached data)\n"
		"Default PC: 1, PS3: 1, XBox360: 1\n");

	REGISTER_CVAR2("s_AllowSoundBankPatching", &g_nAllowSoundBankPatching, nAllowSoundBankPatching, VF_NULL,
		"Controls whether to allow SoundBanks to be FMOD patched or not.\n"
		"This is only relevant for the PC and XBox360 platform! The PS3 naturally does not patch to FMOD!\n"
		"Usage: s_AllowSoundBankPatching [0/1]\n"
		"0: OFF (not allowing patching to FMOD)\n"
		"Default PC: 1, PS3: 0, XBox360: 1\n")->SetOnChangeCallback(&OnCVarChangedCallback);

	REGISTER_CVAR2("s_AllowMusicFileCaching", &g_nAllowMusicFileCaching, 1, VF_NULL,
		"Controls whether to allow music files to be AFCM cached or not.\n"
		"This is only relevant for consoles at the moment!\n"
		"Usage: s_AllowMusicFileCaching [0/1]\n"
		"0: OFF (not allowing caching of music files to the AFCM)\n"
		"Default: 1\n");

	REGISTER_CVAR2("s_AllowStreamFileCaching", &g_nAllowStreamFileCaching, 1, VF_NULL,
		"Controls whether to allow streamed files to be AFCM cached or not.\n"
		"This applies only to the first 256KiB of a streamed soundbank!\n"
		"Usage: s_AllowStreamFileCaching [0/1]\n"
		"0: OFF (not allowing caching of streamed files' headers to the AFCM)\n"
		"Default: 1\n");

	g_sAudioPreloadsFile = REGISTER_STRING("s_AudioPreloadsFile", "AudioPreloads", VF_NULL,
		"Sets the file name for the AudioFileCacheManager to parse.\n"
		"Usage: s_AudioPreloadsFile <file name without extension>\n" 
		"Default: AudioPreloads\n");

	REGISTER_CVAR2("s_GameReverbManagerPause", &g_nGameReverbManagerPause, 0, VF_NULL,
		"Allows for \"pausing\" the reverb manager, meaning the currently registered reverb preset(s)\n"
		"get saved and destroyed and recreated when unpaused again.\n"
		"Usage: s_GameReverbManagerPause [0/1]\n"
		"0: unpaused\n"
		"Default: 0\n");

	REGISTER_CVAR2("s_NumLoadingThreadsToUse", &g_nNumLoadingThreadsToUse, nNumLoadingThreadsToUse, VF_NULL,
		"Controls the maximum amount of concurrent loading threads. Can be changed during run time!\n"
		"Usage: s_NumMaxLoadingThreads [1...5]\n"
		"Default PC: 2, PS3: 2, XBox360: 2\n")->SetOnChangeCallback(&OnCVarChangedCallback);

	REGISTER_CVAR2("s_EffectsAffectReverb", &g_nEffectsAffectReverb, 1, VF_NULL,
		"Controls for alternating routing for reverb path of an event so it goes from the layer dsp unit instead of the channel if enabled.\n"
		"Usage: s_EffectsAffectReverb [0/1]\n"
		"Default PC: 1, PS3: 1, XBox360: 1\n");

	REGISTER_CVAR2("s_DistanceSphereSemantic", &g_nDistanceSphereSemantic, 0, VF_CHEAT|VF_CHEAT_NOCHECK|VF_BITFIELD,
		"Controls for which type of event to draw the distance sphere.\n"
		"Usage: s_DistanceSphereSemantic [0ab...] (flags can be combined)\n"
		"Default: 0 (all)\n"
		"1 : Only Voice Sounds\n"
		"2 : No Voice Sounds\n"
		"a : Ambience\n"
		"b : Ambience Oneshots\n"
		"c : Collisions\n"
		"d : Dialog\n"
		"e : MP Chat\n"
		"f : Footsteps\n"
		"g : General Physics\n"
		"h : HUD\n"
		"i : Unused\n"
		"j : FlowGraph\n"
		"k : Unused\n"
		"l : Living Entity\n"
		"m : Mechanic Entity\n"
		"n : NanoSuit\n"
		"o : SoundSpot\n"
		"p : Particles\n"
		"q : AI Pain/Death\n"
		"r : AI Readability\n"
		"s : AI Readability Response\n"
		"t : TrackView\n"
		"u : Projectile\n"
		"v : Vehicle\n"
		"w : Weapon\n"
		"x : Explosion\n"
		"y : Player Foley\n"
		"z : Animation\n");

	REGISTER_CVAR2("s_LoadRequestsDisplayTime", &g_fLoadRequestsDisplayTime, 5000.0f, VF_NULL,
		"Allows for setting the amount of time in milliseconds for how long load requests are being displayed on the screen.\n"
		"Usage: s_LoadRequestsDisplayTime [0...n]\n" 
		"Default is 5000.0 (5 seconds).\n");

	REGISTER_CVAR2("s_GameMasterVolume", &g_fGameMasterVolume, 1.0f, VF_NULL,
		"Allows for setting the application's master volume.\n"
		"Usage: s_GameMasterVolume [0...1]\n" 
		"Default is 1 (full volume).\n")->SetOnChangeCallback(&OnCVarChangedCallback);

	REGISTER_CVAR2("s_SFXVolume", &g_fSFXVolume, 1.0f, VF_DUMPTODISK,
		"Sets the percentile volume of the sound effects.\n"
		"Usage: s_SFXVolume 0.5\n"
		"Default is 1, which is full volume.");	

	REGISTER_CVAR2("s_GameSFXVolume", &g_fGameSFXVolume, 1.0f, 0,
		"Controls the sfx volume for game use.\n"
		"Usage: s_GameSFXVolume 0.5\n"
		"Default is 1, which is full volume.")->SetOnChangeCallback(&OnCVarChangedCallback);	

	REGISTER_CVAR2("s_MusicVolume", &g_fMusicVolume, 1.0f, VF_DUMPTODISK, 
		"Sets the music volume from 0 to 1 in the games option.\n"
		"Usage: s_MusicVolume 0.2\n"
		"Default is 1.0");	

	REGISTER_CVAR2("s_GameMusicVolume", &g_fGameMusicVolume, 1.0f, 0, 
		"Controls the music volume for game use.\n"
		"Usage: s_GameMusicVolume 0.2\n"
		"Default is 1.0")->SetOnChangeCallback(&OnCVarChangedCallback);	

	REGISTER_CVAR2("s_DialogVolume", &g_fDialogVolume, 1.0f, VF_DUMPTODISK,
		"Sets the volume of all dialog sounds.\n"
		"Usage: s_DialogVolume 0.5\n"
		"Default is 1, which is full volume.");	

	REGISTER_CVAR2("s_GameDialogVolume", &g_fGameDialogVolume, 1.0f, 0,
		"Controls the dialog volume for game use.\n"
		"Usage: s_GameDialogVolume 0.5\n"
		"Default is 1, which is full volume.")->SetOnChangeCallback(&OnCVarChangedCallback);

	REGISTER_CVAR2("s_NoFocusVolume", &g_fNoFocusVolume, 0.0f, VF_NULL,
		"Allows for overriding the application's master volume when the window lost focus.\n"
		"Usage: s_NoFocusVolume [0...1]\n" 
		"Default is 0 (total silence on no focus).\n");

	REGISTER_CVAR2("s_ReverbType", &g_nReverbType, REVERB_TYPE_SOFTWARE_HIGH, VF_REQUIRE_APP_RESTART,
		"Toggles type of reverb effect.\n"
		"Usage: s_ReverbType [0..2]\n" 
		"Default PC: 2, Xbox360: 2, PS3: 2.\n"
		"0: Disables reverb completely.\n"
		"1: Enables SW I3DL2 reverb (low quality).\n"
		"2: Enables SW I3DL2 reverb (high quality).")->SetOnChangeCallback(&OnCVarChangedCallback);

	REGISTER_CVAR2("s_ReverbDebugDraw", &g_nReverbDebugDraw, 0, VF_CHEAT|VF_CHEAT_NOCHECK,
		"Enables reverb ray casting debug drawing. Needs s_ReverbDynamic to be not 0!\n"
		"Usage: s_ReverbDebugDraw [0/1]\n" 
		"Default: 0\n");

	REGISTER_CVAR2("s_ReverbEchoDSP", &g_nReverbEchoDSP, 1, VF_CHEAT|VF_CHEAT_NOCHECK,
		"Toggles Echo DSP effect. Works only with s_ReverbType set to either 2 or 3!\n"
		"Usage: s_ReverbEchoDSP [0/1]\n" 
		"Default: 1\n"
		"0: Bypasses Echo DSP effect.\n"
		"1: Enables Echo DSP effect. (~~~>Reverb~>Echo~>~~)\n");

	REGISTER_CVAR2("s_ReverbDynamic", &g_fReverbDynamic, 0.0f, VF_CHEAT|VF_CHEAT_NOCHECK,
		"If greater than 0 enables dynamic reverb-delay and reverb-reflection-delay calculation dependent on the surrounding geometry!\n"
		"The number than indicates a multiplier for the reverb-delay and reverb-reflection-delay effect.(1.0 manipulates nothing)\n"
		"Usage: s_ReverbDynamic [0/...]\n" 
		"Default: 0\n"
		"0: Uses the static values set within the reverb preset.\n");

	REGISTER_CVAR2("s_ReverbReflectionDelay", &g_fReverbReflectionDelay, -1.0f, VF_CHEAT|VF_CHEAT_NOCHECK,
		"Sets the current reverb's early reflection in seconds! (overrides dynamic values!)\n"
		"Usage: s_ReverbReflectionDelay [0/0.3]\n" 
		"Default: -1\n"
		"-1: Uses the value set within the reverb preset.\n");

	REGISTER_CVAR2("s_ReverbDelay", &g_fReverbDelay, -1.0f, VF_CHEAT|VF_CHEAT_NOCHECK,
		"Sets the current reverb's late reflection in seconds! (overrides dynamic values!)\n"
		"Usage: s_ReverbDelay [0/0.1]\n" 
		"Default: -1\n"
		"-1: Uses the value set within the reverb preset.\n");

	REGISTER_CVAR2("s_autoMpSoundNameFixup", &g_autoMpSoundNameFixup, 0, VF_NULL,
		"Allows for overriding the sound file name on loading.\n"
		"If enabled, \"<sound>_mp\" file will be loaded instead of \"<sound>\" file\n"
		"Usage: s_autoMpSoundNameFixup [0/1]\n" 
		"Default is 0.\n");

#ifdef INCLUDE_AUDIO_PRODUCTION_CODE
	REGISTER_CVAR2("s_FileCacheManagerDebugFilter", &g_nFileCacheManagerDebugFilter, 0, VF_CHEAT|VF_CHEAT_NOCHECK|VF_BITFIELD,
		"Allows for filtered display of the different AFCM entries such as Globals, Level Specifics, Game Hints and so on.\n"
		"Additionally the memory fragmentation can be displayed as well.\n"
		"Usage: s_FileCacheManagerDebugFilter [0ab...] (flags can be combined)\n"
		"Default: 0 (all)\n"
		"a: Globals\n"
		"b: Level Specifics\n"
		"c: Game Hints\n"
		"d: Partially Cached\n"
		"e: Music\n"
		"f: Not Cached\n"
		"g: Fragmentation\n");

	REGISTER_CVAR2("s_FileCacheManagerDebug", &g_nFileCacheManagerDebugLog, 0, VF_NULL,
		"Toggles AFCM debug logging\n"
		"Usage: s_FileCacheManagerDebug [0-1]\n"
		"0: Disables debugging.\n"
		"1: Enables simple cache request debugging.\n");







#endif // INCLUDE_AUDIO_PRODUCTION_CODE
}

//////////////////////////////////////////////////////////////////////////
void CSoundCVars::UnregisterVariables()
{
	IConsole* const pConsole = gEnv->pConsole;
	assert(pConsole);

	pConsole->UnregisterVariable("s_FileCacheManagerSize");
	pConsole->UnregisterVariable("s_AllowNotCachedAccess");
	pConsole->UnregisterVariable("s_AllowSoundBankPatching");
	pConsole->UnregisterVariable("s_AllowMusicFileCaching");
	pConsole->UnregisterVariable("s_AllowStreamFileCaching");
	pConsole->UnregisterVariable("s_AudioPreloadsFile");
	pConsole->UnregisterVariable("s_GameReverbManagerPause");
	pConsole->UnregisterVariable("s_NumLoadingThreadsToUse");
	pConsole->UnregisterVariable("s_EffectsAffectReverb");
	pConsole->UnregisterVariable("s_DistanceSphereSemantic");
	pConsole->UnregisterVariable("s_LoadRequestsDisplayTime");
	pConsole->UnregisterVariable("s_GameMasterVolume");
	pConsole->UnregisterVariable("s_SFXVolume");
	pConsole->UnregisterVariable("s_GameSFXVolume");
	pConsole->UnregisterVariable("s_MusicVolume");
	pConsole->UnregisterVariable("s_GameMusicVolume");
	pConsole->UnregisterVariable("s_DialogVolume");
	pConsole->UnregisterVariable("s_GameDialogVolume");
	pConsole->UnregisterVariable("s_NoFocusVolume");
	pConsole->UnregisterVariable("s_ReverbType");
	pConsole->UnregisterVariable("s_ReverbDebugDraw");
	pConsole->UnregisterVariable("s_ReverbEchoDSP");
	pConsole->UnregisterVariable("s_ReverbDynamic");
	pConsole->UnregisterVariable("s_ReverbReflectionDelay");
	pConsole->UnregisterVariable("s_ReverbDelay");
	pConsole->UnregisterVariable("s_autoMpSoundNameFixup");

#ifdef INCLUDE_AUDIO_PRODUCTION_CODE
	pConsole->UnregisterVariable("s_FileCacheManagerDebugFilter");
	pConsole->UnregisterVariable("s_FileCacheManagerDebug");



#endif // INCLUDE_AUDIO_PRODUCTION_CODE
}
