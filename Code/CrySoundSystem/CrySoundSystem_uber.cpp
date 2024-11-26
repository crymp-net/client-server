//file acts as Uber-file
#ifdef _DEVIRTUALIZE_
	#include <CrySoundSystem_devirt_defines.h>
#endif

#if defined(LINUX)
#include "AudioDeviceFmodEx400_NULL.cpp"
#else
#include "AudioDeviceFmodEx400.cpp"
#endif

#include "AudioDeviceXenon.cpp"
#include "CommandPlayerFmodEx400.cpp"
#include "CrySoundSystem.cpp"
#include "DebugLogger.cpp"
#include "Microphone.cpp"
#include "MicrophoneStream.cpp"
#include "MoodManager.cpp"
#include "MusicSystem/Decoder/ADPCMDecoder.cpp"
#include "MusicSystem/Decoder/FMODBankDecoder.cpp"
#include "MusicSystem/Decoder/OGGDecoder.cpp"
#include "MusicSystem/Decoder/PCMDecoder.cpp"
#include "MusicSystem/MusicCVars.cpp"
#include "MusicSystem/MusicSystem.cpp"
#include "MusicSystem/Pattern/MusicPattern.cpp"
#include "MusicSystem/Pattern/MusicPatternInstance.cpp"
#include "PlatformSoundFmodEx400.cpp"
#include "PlatformSoundFmodEx400Event.cpp"
#include "ReverbManager.cpp"
#include "ReverbManagerEAX.cpp"
#include "Sound.cpp"
#include "SoundAssetManager.cpp"
#include "SoundBuffer.cpp"
#include "SoundBufferFmodEx400.cpp"
#include "SoundBufferFmodEx400Event.cpp"
#include "SoundBufferFmodEx400Micro.cpp"
#include "SoundBufferFmodEx400Network.cpp"
#include "SoundSystem.cpp"
#include "SoundSystemCommon.cpp"
#include "PCMExporterDSP.cpp"
#include "FileCacheManager.cpp"
#include "SoundCVars.cpp"

#ifdef _DEVIRTUALIZE_
	#include <CrySoundSystem_wrapper_includes.h>
#endif
