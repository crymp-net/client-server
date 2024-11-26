////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   IPlatformSound.h
//  Version:     v1.00
//  Created:     6/4/2005 by Tomas
//  Compilers:   Visual Studio.NET
//  Description: Interface of a platform dependent Sound
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __IPLATFORMSOUND_H__
#define __IPLATFORMSOUND_H__

#pragma once
enum enumPlatformSoundParamSemantics
{
	pspNONE,
	pspSOUNDTYPE,
	pspSOUNDHANDLE,
	pspSAMPLETYPE,
	pspFREQUENCY,
	pspVOLUME,
	pspIS3D,
	pspISPLAYING,
	pspISSIMPLE,
	pspISSTREAM,
	pspLOOPMODE,
	pspPAUSEMODE,
	pspSPEAKERPAN,
	pspSPEAKER3DPAN,
	pspSAMPLEPOSITION,
	pspTIMEPOSITION,
	pspMAXRADIUS,
	pspDISTANCE,
	psp3DPOSITION,
	psp3DVELOCITY,
	psp3DDOPPLERSCALE,
	pspMINRADIUS,
	pspPRIORITY,
	pspFXEFFECT,
	pspAMPLITUDE,
	pspEVENTNAME,
	pspLOUDNESSINDB,
	pspCONEINNERANGLE,
	pspCONEOUTERANGLE,
	pspCONEOUTERVOLUME,
	pspREVERBWET,
	pspREVERBDRY,
	pspSPREAD,
	pspFADEOUT,
	pspLENGTHINMS,
	pspLENGTHINBYTES,
	pspSPAWNEDINSTANCES,
	pspCHANNELSUSED,
	pspSTEALBEHAVIOUR,
};

enum enumPlatformSoundStates
{
	pssNONE,
	pssLOADING,
	pssREADY,
	pssPLAYING,
	pssPAUSED,
	pssENDING,
	pssSTOPPED,
	pssERROR,
	pssSTOLEN,
	pssINFOONLY,
	pssWAITINGFORSYNC,
	pssWAITINGTOFADEOUT,
	pssSYNCARRIVED,
};

enum enumPlatformSoundErrorCodes
{
	pssOK,
	pssCHANNEL_INVALID,
	pssEVENT_UNKNOWN,
	pssEVENT_INVALID,
	pssEVENT_NOT_AVAILIBLE,
	pssPARAMETER_UNKNOWN,
	pssPARAMETER_INVALID,
	pssPROPERTY_INVALID,
	pssBUFFER_INVALID,
	pssBUFFER_NOT_AVAILIBLE,
	pssBUFFER_HANDLE_INVALID,
	pssBUFFER_BAD_TYPE,
	pssPROJECT_INVALID,
	pssMEMORY_ERROR,
};

enum enumPlatformSoundClass
{
	pscNONE,
	pscSOUND,
	pscEVENT,

};

// 3D Settings of the sound
struct SSoundSettings3D
{
	bool						bRelativetoListener; // TODO which Listener?
	Vec3						vPosition;
	Vec3						vVelocity;
	bool						bUseVelocity;
};

// Settings of the sound
struct SSoundSettings
{
	int								Frequency;
	int								SpeakerPan;
	SSoundSettings3D* SoundSettings3D;
	bool							is3D;
	bool							bLoadData;
	bool							bLoadNonBlocking;
};

// TODO Review, Platform dependency by #ifdefs ?
typedef void*			tSoundHandle;
typedef void*			tAssetHandle;

class CSound;

//////////////////////////////////////////////////////////////////////////////////////////////
// PlatformSound interface
//////////////////////////////////////////////////////////////////////////////////////////////
struct IPlatformSound
{
	//////////////////////////////////////////////////////////////////////////
	// Management
	//////////////////////////////////////////////////////////////////////////
	virtual void	SetSoundPtr(CSound* pSound) = 0;
	virtual int32	AddRef() = 0;
	virtual int32	Release() = 0;
	virtual enumPlatformSoundErrorCodes	CreateSound(tAssetHandle AssetHandle, SSoundSettings SoundSettings) = 0;
	virtual bool	PlaySound(bool bStartPaused) = 0; // should or could be done through SetParam PlayModes
	virtual bool	StopSound() = 0; // should or could be done through SetParam PlayModes
	virtual bool	FreeSoundHandle() = 0;
	virtual bool	Set3DPosition(Vec3 *pvPosition, Vec3 *pvVelocity, Vec3* pvOrientation) = 0;
	virtual void  SetObstruction(const SObstruction *pObstruction) = 0;
	virtual void  SetPlatformSoundName( const char * sPlatformSoundName) = 0;

	virtual enumPlatformSoundStates  GetState() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Information
	//////////////////////////////////////////////////////////////////////////

	virtual enumPlatformSoundClass GetClass() const = 0;
	virtual void					Reset(CSound*	pSound, const char *sEventName) = 0;

	// Gets and Sets Parameter defined in the enumAssetParam list
	virtual bool					GetParamByType(enumPlatformSoundParamSemantics eSemantics, ptParam* pParam) = 0;
	virtual bool					SetParamByType(enumPlatformSoundParamSemantics eSemantics, ptParam* pParam) = 0;

	// Gets and Sets Parameter defined by string and float value, returns the index of that parameter
	virtual int						GetParamByName(const char *sParameter, float *fValue, bool bOutputWarning=true) = 0;
	virtual int						SetParamByName(const char *sParameter, float fValue, bool bOutputWarning=true) = 0;

	// Gets and Sets Parameter defined by index and float value
	virtual int						GetEventIndexByNameIndex(int const nNameIndex) = 0;
	virtual bool					GetParamByIndex(int nEventIndex, float* const pfValue, float* const pfRangeMin=NULL, float* const pfRangeMax=NULL, char const** const ppcParameterName=NULL, bool const bOutputWarning=true) = 0;
	virtual bool					SetParamByIndex(int nEventIndex, float fValue, bool bOutputWarning=true) = 0;

	virtual tSoundHandle	GetSoundHandle() const = 0;	// returns a valid SoundHandle or 0 if failed

	virtual bool					IsInCategory(const char* sCategory) = 0;

	// Memory
	virtual void					GetMemoryUsage(ICrySizer* pSizer) const = 0;
};
#endif