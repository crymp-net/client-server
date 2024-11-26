////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ISoundBuffer.h
//  Version:     v1.00
//  Created:     5/4/2005 by Tomas
//  Compilers:   Visual Studio.NET
//  Description: Interface of a SoundBuffer
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#include DEVIRTUALIZE_HEADER_FIX(ISoundBuffer.h)

#ifndef __ISOUNDBUFFER_H__
#define __ISOUNDBUFFER_H__

// TODO remove this again (DebugOutputstring)
#include <ISystem.h>			// CryLogAlways
#include <CrySizer.h>
#include "CryFixedString.h"
#include "IAudioDevice.h"

#define MAXCHARBUFFERSIZE 512

typedef void* ProjectHandle;

#pragma once

enum enumBufferType
{
	btNONE,
	btSAMPLE,
	btSTREAM,
	btEVENT,
	btMICRO,
	btNETWORK
};


enum enumAssetLoadingStates
{
	alsSTILLLOADING,
	alsFINISHEDLOADING,
	alsNOTLOADED,
	alsLOADFAILURE
};

typedef CryFixedStringT<MAXCHARBUFFERSIZE>	TFixedResourceName;

// active properties with copy constructor and overloaded operators for comparing sName
struct SSoundBufferProps
{
	SSoundBufferProps()
	{
		nFlags = 0;
		nPrecacheFlags = 0;
		eBufferType = btNONE;
		nProjectIndex = PROJECTINDEX_INVALID;
	}

	SSoundBufferProps(const char *_pszName, uint32 _nFlags, enumBufferType _eBufferType, uint32 _nPrecacheFlags, ProjectIndex _nProjectIndex)
	{
		sName.assign(_pszName);
		
		nFlags = _nFlags; 
		eBufferType = _eBufferType;
		nPrecacheFlags = _nPrecacheFlags;
		nProjectIndex = _nProjectIndex;
	}
	SSoundBufferProps(const SSoundBufferProps &Props)
	{
		sName = Props.sName;
		sUserDataText = Props.sUserDataText;

		nFlags = Props.nFlags;
		eBufferType = Props.eBufferType;
		nPrecacheFlags = Props.nPrecacheFlags;
		nProjectIndex = Props.nProjectIndex;
	}
	
	bool operator<(const SSoundBufferProps &Props) const
	{
		if ((uint32)eBufferType != (uint32)Props.eBufferType)
			return ((uint32)eBufferType < (uint32)Props.eBufferType);

		return sName.compare(Props.sName) < 0;
	}

	void GetMemoryUsage( ICrySizer *pSizer ) const 
	{
 		pSizer->AddObject(sUserDataText);
	}

	// actual properties
	TFixedResourceName sName;
	string						 sUserDataText;
	uint32						 nFlags;
	uint32						 nPrecacheFlags;
	enumBufferType		 eBufferType;
	ProjectIndex			 nProjectIndex;
};

// information of that asset and the loading state
struct SSoundBufferInfo
{
	int32						nBaseFreq;
	int32						nBitsPerSample;
	int32						nChannels;
	uint32					nTimesUsed;
	uint32					nTimedOut;
	uint32					nLengthInBytes;
	uint32					nLengthInMs;
	uint32					nLengthInSamples;
	bool						bLoadFailure;
};

class CSound;
class CSoundSystem;
class CSoundBuffer;
struct IMicrophoneStream;

typedef void*												tAssetHandle;
typedef _smart_ptr<CSoundBuffer>		CSoundBufferPtr;

typedef std::vector<CSound*>							TBufferLoadReqVec;
typedef TBufferLoadReqVec::iterator				TBufferLoadReqVecIt;
typedef TBufferLoadReqVec::const_iterator	TBufferLoadReqVecItConst;

typedef std::vector<_smart_ptr<CSound> >		tvecLoadReqSoundPtr;


// special asset ParamSemantics
enum enumAssetParamSemantics
{
	apASSETNAME,
	apASSETTYPE,
	apASSETFREQUENCY,
	apLENGTHINBYTES,
	apLENGTHINMS,
	apLENGTHINSAMPLES,
	apBYTEPOSITION,
	apTIMEPOSITION,
	apLOADINGSTATE,
	apNUMREFERENCEDSOUNDS,
	apNUMSIMPLESOUNDS
};


//////////////////////////////
// A single microphone stream
UNIQUE_IFACE struct IMicrophoneStream
{
	// ask for butter to be read
	virtual bool ReadDataBuffer(const void* ptr1,const unsigned int len1,const void* ptr2,const unsigned int len2) = 0;
	virtual void OnError(const char*) = 0;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// SoundBuffer interface
//////////////////////////////////////////////////////////////////////////////////////////////
struct ISoundBuffer
{
	//////////////////////////////////////////////////////////////////////////
	// Management
	//////////////////////////////////////////////////////////////////////////
	virtual void SetSoundSystemPtr(CSoundSystem* pSoundSystem) = 0;
	virtual int  AddRef() = 0;
	virtual int	 Release() = 0;
	virtual void RemoveFromLoadReqList(CSound *pSound) = 0;
	virtual void AddToLoadReqList(CSound *pSound) = 0;
	virtual bool Load(CSound *pSound) = 0;
	virtual bool WaitForLoad() = 0;
	virtual void AbortLoading() = 0;
	virtual void SetAssetHandle(tAssetHandle AssetHandle, enumBufferType eBufferType) = 0;
	virtual void SetProps(SSoundBufferProps NewProps) = 0;
	virtual void Update() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Information
	//////////////////////////////////////////////////////////////////////////

	// Gets and Sets Parameter defined in the enumAssetParam list
	virtual bool							GetParam(enumAssetParamSemantics eSemantics, ptParam* pParam) const = 0;
	virtual bool							SetParam(enumAssetParamSemantics eSemantics, ptParam* pParam) = 0;

	// TODO Clean up here!
	virtual tAssetHandle				GetAssetHandle() const = 0;	
	virtual SSoundBufferProps*	GetProps() = 0;
	virtual SSoundBufferInfo*		GetInfo() = 0;
	virtual const char*					GetName() const = 0;
	virtual bool								NotLoaded() const = 0;
	virtual bool								Loaded() const = 0;
	virtual bool								Loading() const = 0;
	virtual bool								LoadFailure() const = 0;
	virtual uint32							GetMemoryUsed() = 0;
	virtual void								GetMemoryUsage( ICrySizer *pSizer ) const=0;		// compute memory-consumption, returns size in Bytes
	virtual void								LogDependencies() = 0;

	//////////////////////////////////////////////////////////////////////////
	//	platform dependent calls
	//////////////////////////////////////////////////////////////////////////
	virtual void* LoadAsSample(const char *AssetName, int nLength) = 0;
	virtual void* LoadAsStream(const char *AssetName, int nLength) = 0;
	virtual void* LoadAsEvent (const char *AssetName) = 0;
	virtual void* LoadAsMicro (IMicrophoneStream *pIMicrophoneStream) = 0;

	virtual bool	UnloadData(const eUnloadDataOptions UnloadOption) = 0;

};
#endif