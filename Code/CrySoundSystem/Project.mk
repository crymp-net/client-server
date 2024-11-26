#############################################################################
## Crytek Source File
## Copyright (C) 2006, Crytek Studios
##
## Creator: Sascha Demetrio
## Date: Jul 31, 2006
## Description: GNU-make based build system
#############################################################################

PROJECT_TYPE := module
PROJECT_VCPROJ := CrySoundSystem.vcproj

-include $(PROJECT_CODE)/Project_override.mk

PROJECT_CPPFLAGS_COMMON += \
	-I$(CODE_ROOT)/CryEngine/CryCommon \
	-I$(CODE_ROOT)/SDKs/boost \
	-I$(CODE_ROOT)/SDKs/Audio/

fmodex_libdir := $(CODE_ROOT)/SDKs/Audio/FmodEx/lib/ps3

ifeq ($(MKOPTION_UNITYBUILD),1)
PROJECT_SOURCES_CPP_REMOVE := StdAfx.cpp\
	AudioDeviceFmodEx400.cpp \
	AudioDeviceXenon.cpp \
	CommandPlayerFmodEx400.cpp \
	CrySoundSystem.cpp \
	DebugLogger.cpp \
	FileCacheManager.cpp \
	SoundCVars.cpp \
	Microphone.cpp \
	MicrophoneStream.cpp \
	MoodManager.cpp \
	MusicSystem/Decoder/ADPCMDecoder.cpp \
	MusicSystem/Decoder/FMODBankDecoder.cpp \
	MusicSystem/Decoder/OGGDecoder.cpp \
	MusicSystem/Decoder/PCMDecoder.cpp \
	MusicSystem/MusicCVars.cpp \
	MusicSystem/MusicSystem.cpp \
	MusicSystem/Pattern/MusicPattern.cpp \
	MusicSystem/Pattern/MusicPatternInstance.cpp \
	PlatformSoundFmodEx400.cpp \
	PlatformSoundFmodEx400Event.cpp \
	PlatformSoundXenon.cpp \
	ReverbManager.cpp \
	ReverbManagerEAX.cpp \
	Sound.cpp \
	SoundAssetManager.cpp \
	SoundBuffer.cpp \
	SoundBufferFmodEx400.cpp \
	SoundBufferFmodEx400Event.cpp \
	SoundBufferFmodEx400Micro.cpp \
	SoundBufferFmodEx400Network.cpp \
	SoundBufferXenon.cpp \
	SoundSystem.cpp \
	SoundSystemCommon.cpp \
	PCMExporterDSP.cpp \
	XenonUtil/AtgApp.cpp \
	XenonUtil/AtgAudio.cpp \
	XenonUtil/AtgUtil.cpp

PROJECT_SOURCES_CPP_ADD += CrySoundSystem_uber.cpp

else

ifeq ($(ARCH_OS),Linux)
PROJECT_SOURCES_CPP_ADD += AudioDeviceFmodEx400_NULL.cpp
endif

endif


PROJECT_SCAN_CPP := MusicSystem/MusicSystem.cpp \
					SPU_Fmod.cpp


# vim:ts=8:sw=8
