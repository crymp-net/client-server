#include "StdAfx.h"
#include <ISystem.h>
#include <IConsole.h>
#include "SoundSystemCommon.h"
#include "IAudioDevice.h"
#include <IReverbManager.h>

CSoundSystemCommon::CSoundSystemCommon()
{
	IConsole* pConsole = gEnv->pConsole;

	assert(pConsole);

	// init this one just to be save
	g_nDebugSound = 0;

	//System
	REGISTER_CVAR2("s_DummySound", &g_nDummySound, 0, VF_DUMPTODISK|VF_REQUIRE_APP_RESTART,
		"Toggles dummy(NULL) sound system.");	
	
	REGISTER_CVAR2("s_SoundEnable", &g_nSoundEnable, 1, VF_DUMPTODISK,
		"Toggles sound on and off.\n"
		"Usage: s_SoundEnable [0/1]\n"
		"Default is 1 (on). Set to 0 to disable sound.");	
	
	REGISTER_CVAR2("s_SpeakerConfig", &g_nSpeakerConfig, 0, VF_DUMPTODISK|VF_REQUIRE_APP_RESTART,
		"Sets up the preferred speaker configuration.\n"
		"Usage: s_SpeakerConfig #\n"
		"where # is a number between 0 and 7 representing\n"
		" 0: Control Panel Settings\n"
		"	1: Mono\n"
		"	2: Stereo\n"
		"	3: Headphone\n"
		"	4: 4Point1\n"
		"	5: 5Point1\n"
		"	6: Prologic\n"
		"	7: 7Point1\n"
		"Default is 0 (Control Panel Setting).");
	
	REGISTER_CVAR2("s_OutputConfig", &g_nOutputConfig, 0, VF_DUMPTODISK|VF_REQUIRE_APP_RESTART,
		"Sets up the preferred output configuration.\n"
		"Usage: s_OutputConfig #\n"
		"where # is a number between 0 and 3 representing\n"
		" 0: AutoDetect\n"
		"	1: DirectSound\n"
		"	2: WAV-Writer\n"
		"	3: WAV-Writer-NRT\n"
		//"	4: OpenAL\n"
		"Default is 0 (AutoDetect).");

	REGISTER_CVAR2("s_RecordConfig", &g_nRecordConfig, 0, VF_DUMPTODISK|VF_REQUIRE_APP_RESTART,
		"Sets up the preferred record configuration.\n"
		"Usage: s_RecordConfig #\n"
		"where # is a number from 0 to x representing\n"
		" 0: AutoDetect (Control Panel Setting)\n"
		" x: The index of a listed record driver\n"
		"Default is 0 (AutoDetect).");
	
int nReverbType = REVERB_TYPE_SOFTWARE_HIGH;




	REGISTER_CVAR2("s_ReverbDebugDraw", &g_nReverbDebugDraw, 0, VF_CHEAT,
		"Enables reverb ray casting debug drawing. Needs s_ReverbDynamic to be not 0!\n"
		"Usage: s_ReverbDebugDraw [0/1]\n" 
		"Default: 0\n");

	REGISTER_CVAR2("s_ReverbType", &g_nReverbType, nReverbType, VF_REQUIRE_APP_RESTART,
		"Toggles type of reverb effect.\n"
		"Usage: s_ReverbType [0..7]\n" 
		"Default PC/Xbox360: 3, PS3: 2.\n"
		"0: Disables reverb completely.\n"
		"1: Enables HW I3DL2 reverb (EAX).\n"
		"2: Enables SW I3DL2 reverb (low quality).\n"
		"3: Enables SW I3DL2 reverb (high quality).\n"
		"4: Enables DSP based FreeVerb Reverb.\n"
		"5: Enables DSP based VST plugin: Classic Reverb.\n"
		"6: Enables DSP based Princeton2016 (disabled).\n"
		"7: Enables DSP based RoomMachine844.");

	REGISTER_CVAR2("s_ReverbInstances", &g_nReverbInstances, 1, VF_CHEAT,
		"Number of reverb instances.\n"
		"Usage: s_ReverbInstances [1]\n" 
		"Default is 1\n"
		"1: One reverb instance is created.\n"
		"(multiple reverb in development)\n");

	REGISTER_CVAR2("s_ReverbEchoDSP", &g_nReverbEchoDSP, 0, VF_CHEAT,
		"Toggles Echo DSP effect. Works only with s_ReverbType set to either 2 or 3!\n"
		"Usage: s_ReverbEchoDSP [0/1]\n" 
		"Default: 0\n"
		"0: Bypasses Echo DSP effect.\n"
		"1: Enables Echo DSP effect. (~~~>Reverb~>Echo~>~~)\n");

	REGISTER_CVAR2("s_ReverbDynamic", &g_fReverbDynamic, 0.0f, VF_CHEAT,
		"If greater than 0 enables dynamic reverb-delay and reverb-reflection-delay calculation dependent on the surrounding geometry!\n"
		"The number than indicates a multiplier for the reverb-delay and reverb-reflection-delay effect.(1.0 manipulates nothing)\n"
		"Usage: s_ReverbDynamic [0/...]\n" 
		"Default: 0\n"
		"0: Uses the static values set within the reverb preset.\n");

	REGISTER_CVAR2("s_ReverbReflectionDelay", &g_fReverbReflectionDelay, -1.0f, VF_CHEAT,
		"Sets the current reverb's early reflection in seconds! (overrides dynamic values!)\n"
		"Usage: s_ReverbReflectionDelay [0/0.3]\n" 
		"Default: -1\n"
		"-1: Uses the value set within the reverb preset.\n");

	REGISTER_CVAR2("s_ReverbDelay", &g_fReverbDelay, -1.0f, VF_CHEAT,
		"Sets the current reverb's late reflection in seconds! (overrides dynamic values!)\n"
		"Usage: s_ReverbDelay [0/0.1]\n" 
		"Default: -1\n"
		"-1: Uses the value set within the reverb preset.\n");
	
	REGISTER_CVAR2("s_MinHWChannels", &g_nMinHWChannels, 0, VF_DUMPTODISK|VF_REQUIRE_APP_RESTART,
		"Sets the minimum number of sound hardware channels used.\n"
		"Usage: s_MinHWChannels 16\n"
		"Default value is 0. This is the number of hardware channels\n"
		"used to play back game sound. If the hardware does not have\n"
		"the required channels, the system will use software mixing,\n"
		"resulting in reduced performance (no eax).\n"
		"0 forces software mixing.\n"); 
	 
	REGISTER_CVAR2("s_MaxHWChannels", &g_nMaxHWChannels, 0, VF_DUMPTODISK|VF_REQUIRE_APP_RESTART,
		"Sets the maximum number of sound hardware channels used./n"
		"So even if you have 256 channels only max of them will be used.\n"
		"Default is 0.");

	REGISTER_CVAR2("s_SoftwareChannels", &g_nSoftwareChannels, 256, VF_DUMPTODISK|VF_REQUIRE_APP_RESTART,
		"Sets the maximum number of software sound channels.\n"
		"Default is 256.");








	// set auto values for PC
	g_nInternCompression = 2; 


	REGISTER_CVAR2("s_Compression", &g_nCompression, 0, VF_DUMPTODISK,
		"Toggles which decoder should be preferred: <auto>, FSB, MP2, MP3, XMA, or WAV.\n"
		"0:	<auto>, PC:2, PS3:3, X360:4\n"
		"1: Prefers FSB\n"
		"2: Prefers MP2\n"
		"3: Prefers MP3\n"
		"4: Prefers XMA\n"
		"5: Prefers WAV\n"
		"Default is 0 (auto)");

	//Debug
	REGISTER_CVAR2("s_DebugSound",&g_nDebugSound,0,VF_CHEAT,
		"Toggles sound debugging mode.\n"
		"Usage: s_DebugSound [0/4]\n" 
		"0: Disables debugging.\n" 
		"1: Enables simple sound debugging.\n" 
		"2: Enables full sound debugging.\n" 
		"3: Enables sound event listener debugging.\n" 
		"4: Enables sound syncpoint debugging.\n" 
		"5: Enables AudioDevice debugging.\n" 
		"6: Enables more AudioDevice debugging.\n" 
		"7: Enables all AudioDevice debugging.\n" 
		"8: Enables AudioDevice command logging to SoundCommands.xml\n" 
		"Default is 0 (off).");

	REGISTER_CVAR2("s_SpamFilterTimeout", &g_fSpamFilterTimeout, 5, VF_CHEAT,
		"Sets the time in sec for entries in spam filter to time out.\n"
		"Usage: s_SpamFilterTimeout 0 means spam filter is disables\n" 
		"Usage: s_SpamFilterTimeout [0..]\n" 
		"Default is 5 (on).");

	REGISTER_CVAR2("s_ErrorSound", &g_nErrorSound, 1, VF_CHEAT,
		"Toggles error sound playback.\n"
		"Usage: s_ErrorSound [0/1]\n" 
		"Default is 1 (on).");

	REGISTER_CVAR2("s_DrawSounds", &g_nDrawSounds, 0, VF_CHEAT,
		"Toggles drawing of a small red ball at the sound's position and additional information.\n"
		"Usage: s_DrawSounds [0..4]\n" 
		"Default is 0 (off).\n"
		"1: Draws the ball, filename and the current volume of the used channel for all active sounds.\n"
		"2: Draws the ball, used channel, static volume, current volume, SoundID of the used channel for all active sounds.\n"
		"3: Draws the ball, all information for all active sounds.\n"
		"4: Draws the ball, and information for all sounds (also unactive).");

	REGISTER_CVAR2("s_DrawObstruction", &g_nDrawObstruction, 0, VF_CHEAT,
		"Toggles drawing of a blue radius around the sound's position and rays casted to test obstruction.\n"
		"Usage: s_DrawObstruction [0/1]\n" 
		"Default is 0 (off).\n"
		"1: Draws the ball and casts rays to show the obstruction tests.");
	
	REGISTER_CVAR2("s_SoundInfo",&g_nSoundInfo,0,VF_CHEAT,
		"Toggles onscreen sound statistics.\n"
		"Usage: s_SoundInfo [0..9]\n"
		"1: simple list of playing sounds.\n"
		"2: extended list of looping sounds.\n"
		"3: extended list of oneshot sounds.\n"
		"4: list of lost sounds.\n"
		"5: list of event soundbuffers.\n"
		"6: list of sample soundbuffers (dialog).\n"
		"7: list of wavebanks.\n"
		"8: displays music information.\n"
		"9: displays sound moods.\n"
		"Default is 0 (off).");

	REGISTER_CVAR2("s_NetworkAudition", &g_nNetworkAudition, 0, VF_CHEAT|VF_REQUIRE_APP_RESTART,
		"Toggles network audition (opens port/needs restart).\n"
		"Usage: s_NetworkAudition [0/1]\n" 
		"Default is 0 (off).");

	REGISTER_CVAR2("s_Profiling", &g_nProfiling, 0, VF_CHEAT,
		"Toggles profiling of some sound calls.\n"
		"Usage: s_Profiling [0/1]\n"
		"Default is 0 (off).");
	
	REGISTER_CVAR2("s_PlaybackFilter", &g_nPlaybackFilter, 0, VF_CHEAT | VF_BITFIELD,
		"Toggles filter to select certain sounds to be played only.\n"
		"Default is 0 (off).\n"
		"1 : Only Voice Sounds\n"
		"2 : No Voice Sounds\n"
		"+a : Ambience\n"
		"+b : Ambience Oneshots\n"
		"+c : Collisions\n"
		"+d : Dialog\n"
		"+e : MP Chat\n"
		"+f : Footsteps\n"
		"+g : General Physics\n"
		"+h : HUD\n"
		"+i : Unused\n"
		"+j : FlowGraph\n"
		"+k : Unused\n"
		"+l : Living Entity\n"
		"+m : Mechanic Entity\n"
		"+n : NanoSuit\n"
		"+o : SoundSpot\n"
		"+p : Particles\n"
		"+q : AI Pain/Death\n"
		"+r : AI Readability\n"
		"+s : AI Readability Response\n"
		"+t : TrackView\n"
		"+u : Projectile\n"
		"+v : Vehicle\n"
		"+w : Weapon\n"
		"+x : Explosion\n"
		"+y : Player Foley\n"
		"+z : Animation\n"
		"Usage: s_PlaybackFilter [1,2,a..z].");
	
	REGISTER_CVAR2("s_StopSoundsImmediately", &g_nStopSoundsImmediately, 0, VF_CHEAT,
		"Toggles to stop sounds without internal fadeout.\n"
		"Default is 0 (off).\n"
		"0: Stops sound with fadeout.\n"
		"1: Stops sounds without fadeout.\n"
		"Usage: s_StopSoundsImmediately [0/1].");
	
	REGISTER_CVAR2("s_DumpEventStructure", &g_nDumpEventStructure, 0, VF_CHEAT,
		"Toggles to save a file of event structure.\n"
		"Default is 0 (off).\n"
		"1: dumps event structure to eventstructure.txt.\n"
		"Usage: s_DumpEventStructure [0/1].");
	
	REGISTER_CVAR2("s_FindLostEvents", &g_nFindLostEvents, 0, VF_CHEAT,
		"Toggles to find and stop lost events to prevent them from endlessly looping.\n"
		"Default is 0 (off).\n"
		"1: finds and stops lost events.\n"
		"Usage: s_FindLostEvents [0/1].");

	// Volume
	REGISTER_CVAR2("s_SFXVolume", &g_fSFXVolume, 1.0f, VF_DUMPTODISK,
		"Sets the percentile volume of the sound effects.\n"
		"Usage: s_SFXVolume 0.5\n"
		"Default is 1, which is full volume.");	

	REGISTER_CVAR2("s_GameSFXVolume", &g_fGameSFXVolume, 1.0f, 0,
		"Controls the sfx volume for game use.\n"
		"Usage: s_GameSFXVolume 0.5\n"
		"Default is 1, which is full volume.");	

	REGISTER_CVAR2("s_MusicVolume", &g_fMusicVolume, 1.0f, VF_DUMPTODISK, 
		"Sets the music volume from 0 to 1 in the games option.\n"
		"Usage: s_MusicVolume 0.2\n"
		"Default is 1.0");	

	REGISTER_CVAR2("s_GameMusicVolume", &g_fGameMusicVolume, 1.0f, 0, 
		"Controls the music volume for game use.\n"
		"Usage: s_GameMusicVolume 0.2\n"
		"Default is 1.0");	

	REGISTER_CVAR2("s_DialogVolume", &g_fDialogVolume, 1.0f, VF_DUMPTODISK,
		"Sets the volume of all dialog sounds.\n"
		"Usage: s_DialogVolume 0.5\n"
		"Default is 1, which is full volume.");	

	REGISTER_CVAR2("s_GameDialogVolume", &g_fGameDialogVolume, 1.0f, 0,
		"Controls the dialog volume for game use.\n"
		"Usage: s_GameDialogVolume 0.5\n"
		"Default is 1, which is full volume.");	

	// Format
	REGISTER_CVAR2("s_FormatSampleRate", &g_nFormatSampleRate, 48000, VF_REQUIRE_APP_RESTART,
		"Sets the output sample rate.\n"
		"Usage: s_FormatSampleRate 44100\n"
		"Default is 48000. Sets the rate, in samples per second,\n"
		"at which the output of the sound system is played.");

	REGISTER_CVAR2("s_FormatResampler", &g_nFormatResampler, 1, VF_REQUIRE_APP_RESTART,
		"Toggles  internal resampling method.\n"
		"Usage: s_FormatResampler [0..3]\n" 
		"0: none\n"
		"1: linear\n"
		"2: cubic\n"
		"3: spline\n"
		"Default is 1 (linear).");






	int nFormatType = 2;


	REGISTER_CVAR2("s_FormatType", &g_nFormatType, nFormatType, VF_REQUIRE_APP_RESTART,
		"Sets the format data type.\n"
		"Usage: s_FormatType [0..5]\n" 
		"0: none\n"
		"1: PCM 8bit\n"
		"2: PCM 16bit\n"
		"3: PCM 24bit\n"
		"4: PCM 32bit\n"
		"5: PCM 32bit float\n"
		"Default is 2 (PCM 16bit).");

	//m_pCVarMaxSoundDist= REGISTER_FLOAT("s_MaxSoundDist",500.0f,VF_DUMPTODISK,
	//	"Sets the maximum sound distance, in meters. Default is 500 meters.\n"
	//	"Usage: s_MaxSoundDist 500");	


	// Features
	REGISTER_CVAR2("s_Doppler", &g_nDoppler, 1, VF_DUMPTODISK,
		"Toggles Doppler effect on and off.\n"
		"Usage: s_Doppler [0/1]\n"
		"Default is 1 (on).");	
	
	REGISTER_CVAR2("s_DopplerScale", &g_fDopplerScale, 1.0f, VF_DUMPTODISK,
		"Sets the strength of the Doppler effect.\n"
		"Usage: s_DopplerValue 1.0"
		"Default is 1.0. This multiplier affects the sound velocity.");		

	//g_npCVarEnableSoundFX = REGISTER_INT("s_EnableSoundFX",1,VF_DUMPTODISK,
	//	"Toggles sound effects on and off.\n"
	//	"Usage: s_EnableSoundFX [0/1]\n"
	//	"Default is 1 (on).");
	//m_pCVarEnableHDRA = REGISTER_INT("s_EnableHDRA",0,VF_DUMPTODISK,
	//	"Toggles hig dynamic range acoustic effect on and off.\n"
	//	"Usage: s_EnableHDRA [0/1]\n"
	//	"Default (for now) is 0 (off).");

	REGISTER_CVAR2("s_Obstruction", &g_nObstruction, 1, VF_REQUIRE_APP_RESTART,
		"Toggles sound obstruction effect.\n"
		"Usage: s_Obstruction [0..2]\n" 
		"0: off\n"
		"1: DSP based obstruction\n"
		"2: volume based obstruction\n"
		"Default is 1 (DSP).");

	REGISTER_CVAR2("s_ObstructionMaxValue", &g_fObstructionMaxValue, 0.95f, VF_CHEAT,
		"Toggles maximum obstruction effect.\n"
		"Usage: s_ObstructionMaxValue [0..1.0f]\n" 
		"0:		 off\n"
		"1.0f: total maximum obstruction\n"
		"Default is 0.85f.");

	REGISTER_CVAR2("s_ObstructionAccuracy", &g_nObstructionAccuracy, 1, 0,
		"Toggles maximum obstruction effect.\n"
		"Usage: s_ObstructionAccuracy [1..5]\n" 
		"1:		 test Direct\n"
		"2:		 also test left side\n"
		"3:		 also test right side\n"
		"4:		 also test up\n"
		"5:		 also test down\n"
		"Default is 1 (for now)");

	REGISTER_CVAR2("s_ObstructionUpdate", &g_fObstructionUpdate, 0.1f, 0,
		"Controls how fast obstruction is re-evaluated in seconds.\n"
		"Usage: s_ObstructionUpdate [0..]\n" 
		"0:		 every frame\n"
		"0.5:	 every half seconds\n"
		"Default is 0.5 (for now)");

	REGISTER_CVAR2("s_ObstructionVisArea", &g_fObstructionVisArea, 0.15f, 0,
		"Controls the effect of additional obstruction per VisArea/portal step.\n"
		"Usage: s_ObstructionVisArea [0..1]\n" 
		"0:		 none\n"
		"0.15: obstruction per step\n"
		"Default is 0.15");

	REGISTER_CVAR2("s_ObstructionMaxRadius", &g_fObstructionMaxRadius, 500.0f, 0,
		"Controls how much loud sounds are affected by obstruction.\n"
		"Usage: s_ObstructionMaxRadius [0..]\n" 
		"0:		 none\n"
		"500:	 a sound with a radius of 500m is not affected by obstruction\n"
		"Default is 500m");

	REGISTER_CVAR2("s_ObstructionMaxPierecability", &g_fObstructionMaxPierecability, 100.0f, 0,
		"Normalizes accumulated pierecability value for obstruction.\n"
		"Usage: s_ObstructionMaxPierecability [0..]\n" 
		"0:		 none\n"
		"100:	 a sound that is obstructed by accumulated material pierecability of 50 gets half obstruction\n"
		"Default is 100");
	

	//m_pCVarReverbPresetMix = REGISTER_FLOAT("s_ReverbPresetMix",0.5f,VF_CHEAT,
	//	"Toggles wet mix effect on reverb preview sound.\n"
	//	"Usage: s_ReverbPresetMix [0..1.0f]\n" 
	//	"0:		 off\n"
	//	"1.0f: total reverb\n"
	//	"Default is 0.5f.");

	// Tweaking

	REGISTER_CVAR2("s_InactiveSoundIterationTimeout", &g_fInactiveSoundIterationTimeout, 1.0f, VF_DUMPTODISK,
		"This variable is for internal use only.");

	REGISTER_CVAR2("s_MaxChannels", &g_nMaxChannels, 64, VF_DUMPTODISK,
		"Sets the maximum number of sound channels.\n"
		"Default is 64.");

	REGISTER_CVAR2("s_VisAreasPropagation", &g_nVisAreaPropagation, 5, VF_DUMPTODISK,
		"Sets the vis area propagation number.\n"
		"Usage: s_VisAreasPropagation 5\n"
		"Default is 5. This number defines how far sound will\n"
		"propagate, in vis areas from the player's position. A\n"
		"value of 1 means the sound is only heard in the current\n"
		"vis area, while 3 means sound is heard in adjacent areas\n"
		"which are connected by one portal."); 

	REGISTER_CVAR2("s_MaxActiveSounds", &g_nMaxActiveSounds, 100, VF_DUMPTODISK,
		"Sets the maximum number of active sounds.\n");

	REGISTER_CVAR2("s_MinRepeatSoundTimeout", &g_fMinRepeatSoundTimeout, 200.0f, VF_DUMPTODISK,
		"Prevents playback of a sound within time range in MS.\n"
		"Usage: s_MinRepeatSoundTimeout [0..]\n"
		"Default is 200.0.");
	
	//m_pCVarCapsCheck= REGISTER_INT("s_CapsCheck",0,VF_DUMPTODISK,
		//"Checks for sound CAPS on startup and write them into the log.\n");	

	REGISTER_CVAR2("s_PrecacheData",&g_nPrecacheData, 0, VF_CHEAT,
		"Toggles precaching of static sounds on level loading.\n"
		"Usage: s_PrecacheData [0/1]\n" 
		"Default is 0.");	
	
	REGISTER_CVAR2("s_UnloadData", &g_nUnloadData, 1, VF_CHEAT,
		"Toggles unloading of sound data by the AssetManager.\n"
		"Usage: s_UnloadData [0/1]\n" 
		"Default is 1.");	
	
		REGISTER_CVAR2("s_UnloadProjects", &g_nUnloadProjects, 1, VF_CHEAT,
		"Toggles automatic unloading of a project if it was not in use for 20 secs.\n"
		"Usage: s_UnloadProjects [0/1]\n" 
		"Default is 1.");

	REGISTER_CVAR2("s_LoadNonBlocking", &g_nLoadNonBlocking, 1, VF_CHEAT,
		"Toggles loading data non-blocking.\n"
		"Usage: s_LoadNonBlocking [0/1]\n" 
		"Default is 1.");	
	
	REGISTER_CVAR2("s_FileAccess", &g_nFileAccess, 0, VF_CHEAT,
		"Toggles disk access on reading files.\n"
		"0:		 direct file access via CryPak\n"
		"1:		 indirect file access via StreamingSystem with wait\n"
		"Default is 0.");	

	REGISTER_CVAR2("s_CacheFillRatio", &g_fCacheFillRatio, 0.85f, VF_CHEAT,
		"Controls at what fill ratio sound data is unloaded.\n"
		"Usage: s_CacheFillRatio [0/1]\n" 
		"Default is 0.85.");	

	REGISTER_CVAR2("s_CompressedDialog", &g_nCompressedDialog, 1, VF_CHEAT,
		"toggles if dialog data are stored compressed in memory.\n"
		"Usage: s_CompressedDialog [0/1]\n" 
		"Default is 1 (on).");	

	REGISTER_CVAR2("s_PriorityThreshold", &g_nPriorityThreshold, 45, VF_CHEAT,
		"Controls rejection of sounds lower(higher) than this priority if 80% of voices are used.\n"
		"Usage: s_PriorityThreshold [0..128]\n"
		"Default is 45, value of 0 disables priority rejection.");

	REGISTER_CVAR2("s_LanguagesConversion", &g_nLanguagesConversion, 1, VF_CHEAT,
		"Controls conversion of legacy event name to direct wav files (only for languages).\n"
		"Usage: s_LanguagesConversion [0/1]\n"
		"Default is 0 (off).");
	
	REGISTER_CVAR2("s_SoundMoods", &g_nSoundMoods, 1, VF_CHEAT,
		"Controls using sound moods for mixing.\n"
		"Usage: s_SoundMoods [0/1]\n"
		"Default is 1 (on).");
	
	REGISTER_CVAR2("s_SoundMoodsDSP", &g_nSoundMoodsDSP, 1, 0,
		"enables DSP effects being used in soundmoods.\n"
		"Usage: s_SoundMoodsDSP [0/1]\n"
		"Default is 1 (on).");
	
	REGISTER_CVAR2("s_MPEGDecoders", &g_nMPEGDecoders, 0, VF_REQUIRE_APP_RESTART,
		"Sets maximum number of MPEG Decoder.\n"
		"Usage: s_MPEGDecoders 16\n"
		"0:			<auto>, PC:32, PS3:32, X360:0\n"
		"Default is 0 <auto>.");

	REGISTER_CVAR2("s_XMADecoders", &g_nXMADecoders, 0, VF_CHEAT|VF_REQUIRE_APP_RESTART,
		"Sets maximum number of XMA Decoder.\n"
		"Usage: s_XMADecoders 32\n"
		"0:			<auto>, PC:0, PS3:0, X360:48\n"
		"Default is 0 <auto>.");
	
	REGISTER_CVAR2("s_ADPCMDecoders", &g_nADPCMDecoders, 0, VF_CHEAT|VF_REQUIRE_APP_RESTART,
		"Sets maximum number of ADPCM Decoder.\n"
		"Usage: s_ADPCMDecoders 32\n"
		"0:			<auto>, PC:32, PS3:0, X360:0\n"
		"Default is 0 <auto>.");

	











	// set auto values for PC
	g_nInternMPEGDecoders = 32; 
	g_nInternXMADecoders = 0;
	g_nInternADPCMDecoders = 32;


	REGISTER_CVAR2("s_VariationLimiter", &g_fVariationLimiter, 1.0f, VF_REQUIRE_APP_RESTART,
		"Sets limiter to control sound variation.\n"
		"Usage: s_VariationLimiter [0..1]\n"
		"Default is 1.0.");
	
	int nOpenFileHandleMaxDefault = 30;




		REGISTER_CVAR2("s_FileOpenHandleMax", &g_nOpenFileHandleMax, nOpenFileHandleMaxDefault, VF_CHEAT,
		"Sets max of open file handles.\n"
		"Usage: s_FileOpenHandleMax [1..]\n"
		"Default PC: 30, PS3: 10.");
	
	REGISTER_CVAR2("s_HRTF_DSP", &g_nHRTF_DSP, 0, VF_CHEAT|VF_REQUIRE_APP_RESTART,
		"Toggles extra lowpass filter for enhanced HRTF.\n"
		"Usage: s_HRTF_DSP [0/1]\n"
		"Default is 0 (on).");

	REGISTER_CVAR2("s_Vol0TurnsVirtual", &g_nVol0TurnsVirtual, 1, VF_CHEAT|VF_REQUIRE_APP_RESTART,
		"Toggles if sounds with zero volume force to go virtual.\n"
		"Usage: s_Vol0TurnsVirtual [0/1]\n"
		"Default is 1 (on).");

	int nStreamDialogIntoMemory = 1;






	REGISTER_CVAR2("s_StreamDialogIntoMemory", &g_nStreamDialogIntoMemory, nStreamDialogIntoMemory, VF_CHEAT,
		"Toggles if dialog are streamed into main memory or FMOD makes a copy for itself.\n"
		"Usage: s_StreamDialogIntoMemory [0/1]\n"
		"Default is 1 (on) on PC, and 0 (off) on XBox.");

	REGISTER_CVAR2("s_StreamProjectFiles", &g_StreamProjectFiles, 1, VF_CHEAT,
		"Toggles if project files are streamed into memory or middleware loads them blocking.\n"
		"Usage: s_StreamProjectFiles [0/1]\n"
		"Default is 1 (on).");


}

CSoundSystemCommon::~CSoundSystemCommon()
{
	IConsole* pConsole = gEnv->pConsole;

	assert(pConsole);


	pConsole->UnregisterVariable("s_DummySound", true);
	pConsole->UnregisterVariable("s_SoundEnable", true);
	pConsole->UnregisterVariable("s_SpeakerConfig", true);
	pConsole->UnregisterVariable("s_OutputConfig", true);
	pConsole->UnregisterVariable("s_RecordConfig", true);
	pConsole->UnregisterVariable("s_ReverbDebugDraw", true);
	pConsole->UnregisterVariable("s_ReverbType", true);
	pConsole->UnregisterVariable("s_ReverbInstances", true);
	pConsole->UnregisterVariable("s_ReverbEchoDSP", true);
	pConsole->UnregisterVariable("s_ReverbDynamic", true);
	pConsole->UnregisterVariable("s_ReverbDelay", true);
	pConsole->UnregisterVariable("s_ReverbReflectionDelay", true);


	//debug
	pConsole->UnregisterVariable("s_SoundInfo", true);
	pConsole->UnregisterVariable("s_DebugSound", true);
	pConsole->UnregisterVariable("s_SpamFilterTimeout", true);


	//m_pCVARDummySound->Release();
	//m_pCVARSoundEnable->Release();
	//m_pCVarMaxSoundDist->Release();	
	pConsole->UnregisterVariable("s_Doppler");
	pConsole->UnregisterVariable("s_DopplerScale");
	
	pConsole->UnregisterVariable("s_SFXVolume");
	pConsole->UnregisterVariable("s_MusicVolume");
	pConsole->UnregisterVariable("s_GameSFXVolume");
	pConsole->UnregisterVariable("s_GameMusicVolume");
	pConsole->UnregisterVariable("s_DialogVolume");
	pConsole->UnregisterVariable("s_GameDialogVolume");

	pConsole->UnregisterVariable("s_FormatSampleRate");
	pConsole->UnregisterVariable("s_FormatResampler");
	pConsole->UnregisterVariable("s_FormatType");

//	m_pCVarSpeakerConfig->Release();
//	m_pCVarOutputConfig->Release();
	//m_pCVarEnableSoundFX->Release();
	//m_pCVarDebugSound->Release();


	pConsole->UnregisterVariable("s_PlaybackFilter");
	pConsole->UnregisterVariable("s_ErrorSound");
	pConsole->UnregisterVariable("s_DrawSounds");
	pConsole->UnregisterVariable("s_DrawObstruction");

//	m_pCVarReverbType->Release();
	//m_pCVarReverbPresetMix->Release();
	//m_pCVarSoundInfo->Release();
	pConsole->UnregisterVariable("s_InactiveSoundIterationTimeout");
	
	pConsole->UnregisterVariable("s_MinHWChannels");
	pConsole->UnregisterVariable("s_MaxHWChannels");
	pConsole->UnregisterVariable("s_SoftwareChannels");
	pConsole->UnregisterVariable("s_MaxChannels");
	pConsole->UnregisterVariable("s_MaxActiveSounds");
	pConsole->UnregisterVariable("s_CompressedDialog");
	pConsole->UnregisterVariable("s_PriorityThereshold");
	pConsole->UnregisterVariable("s_VisAreasPropagation");

	
	pConsole->UnregisterVariable("s_Obstruction");
	pConsole->UnregisterVariable("s_ObstructionMaxValue");
	pConsole->UnregisterVariable("s_ObstructionAccuracy");
	pConsole->UnregisterVariable("s_ObstructionUpdate");
	pConsole->UnregisterVariable("s_ObstructionVisArea");
	pConsole->UnregisterVariable("s_ObstructionMaxRadius");
	pConsole->UnregisterVariable("s_ObstructionMaxPierecability");

	//m_pCVarCapsCheck->Release();

	pConsole->UnregisterVariable("s_MinRepeatSoundTimeout");

	pConsole->UnregisterVariable("s_NetworkAudition");
	
	pConsole->UnregisterVariable("s_PrecacheData");
	pConsole->UnregisterVariable("s_UnloadData");
	pConsole->UnregisterVariable("s_LoadNonBlocking");
	pConsole->UnregisterVariable("s_FileAccess");
	pConsole->UnregisterVariable("s_CacheFillRatio");
	

	pConsole->UnregisterVariable("s_LanguagesConversion");

	pConsole->UnregisterVariable("s_SoundMoods");
	pConsole->UnregisterVariable("s_SoundMoodsDSP");
	pConsole->UnregisterVariable("s_Compression");

	pConsole->UnregisterVariable("s_MPEGDecoders");
	pConsole->UnregisterVariable("s_XMADecoders");
	pConsole->UnregisterVariable("s_ADPCMDecoders");
	pConsole->UnregisterVariable("s_VariationLimiter");
	pConsole->UnregisterVariable("s_FileOpenHandleMax");
	
	pConsole->UnregisterVariable("s_Profiling");
	pConsole->UnregisterVariable("s_DumpEventStructure");
	pConsole->UnregisterVariable("s_FindLostEvents");

	pConsole->UnregisterVariable("s_HRTF_DSP");
	pConsole->UnregisterVariable("s_Vol0TurnsVirtual");
	pConsole->UnregisterVariable("s_StreamDialogIntoMemory");
	pConsole->UnregisterVariable("s_StreamProjectFiles");
	
}

bool CSoundSystemCommon::DebuggingSound()
{
	return (g_nDebugSound != 0);
}
