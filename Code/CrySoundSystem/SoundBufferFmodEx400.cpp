////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   SoundBufferFmodEx400.cpp
//  Version:     v1.00
//  Created:     8/6/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: FMODEX 4.00 Implementation of SoundBuffer
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#ifdef SOUNDSYSTEM_USE_FMODEX400

#include "SoundBufferFmodEx400.h"
#include "AudioDeviceFmodEx400.h"
#include "Sound.h"
#include "CryCommon/CrySoundSystem/IReverbManager.h"
#include "FmodEx/inc/fmod_errors.h"
#include "CryCommon/CryCore/CrySizer.h"
#include "CryCommon/CrySystem/ITimer.h"

//////////////////////////////////////////////////////////////////////////
#define IS_FMODERROR (m_ExResult != FMOD_OK )

CSoundBufferFmodEx400::CSoundBufferFmodEx400(const SSoundBufferProps &Props, FMOD::System* pCSEX) : CSoundBuffer(Props)
{
	m_pCSEX = pCSEX;
	m_ExResult = FMOD_OK;
	m_Props.nFlags = Props.nFlags;
	m_Props.nPrecacheFlags = Props.nPrecacheFlags;
}

CSoundBufferFmodEx400::~CSoundBufferFmodEx400()
{
	//gEnv->pSoundSystem->Log(eSLT_Always, "<Sound> SB-Destructor-start: %s \n", m_Props.sName.c_str());
	UnloadData(sbUNLOAD_ALL_DATA_NOW);

	// Tell Sounds that their soundbuffers are invalid now
	for (TBufferLoadReqVecIt It=m_vecLoadReq.begin(); It!=m_vecLoadReq.end(); ++It) 
		(*It)->OnBufferDelete(); 

	//gEnv->pSoundSystem->Log(eSLT_Always, "<Sound> SB-Destructor-end: %s \n", m_Props.sName.c_str());
}

void CSoundBufferFmodEx400::FmodErrorOutput(const char * sDescription, ESoundLogType LogType)
{
	gEnv->pSoundSystem->Log(LogType, "<Sound> SB: %s (%d) %s\n", sDescription, m_ExResult, FMOD_ErrorString(m_ExResult));
}

uint32 CSoundBufferFmodEx400::GetMemoryUsed()
{
	if (Loaded())
		return m_Info.nLengthInBytes;
	else
		return sizeof(*this);
}

// compute memory-consumption, returns size in Bytes
void CSoundBufferFmodEx400::GetMemoryUsage(class ICrySizer* pSizer) const
{
	pSizer->AddObject( this, sizeof(*this) );	
	CSoundBuffer::GetMemoryUsage(pSizer);
}

//////////////////////////////////////////////////////////////////////////
tAssetHandle CSoundBufferFmodEx400::LoadAsSample(const char *AssetDataPtrOrName, int nLength)
{
	FUNCTION_PROFILER( gEnv->pSystem,PROFILE_SOUND );

	FMOD::Sound            *pEXSound = 0;
	FMOD_CREATESOUNDEXINFO	exinfo = {0};
	exinfo.cbsize							= sizeof(FMOD_CREATESOUNDEXINFO);
	exinfo.length							= nLength;
	FMOD_MODE Modes = 0;

	if (m_pSoundSystem->g_nCompressedDialog)
		Modes |= FMOD_CREATECOMPRESSEDSAMPLE;

	int nTryThisCompression = m_pSoundSystem->g_nCompression ? m_pSoundSystem->g_nCompression : m_pSoundSystem->g_nInternCompression;

	switch(nTryThisCompression)
	{
	case 0:
		{
			exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_WAV;
		}
		break;
	case 1:
		{



			exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_MPEG;

		}
		break;
	case 2:
		{
			exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_MPEG;
		}
		break;
	case 3:
		{
			exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_MPEG;
		}
		break;
	case 4:
		{
			exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_XMA;
		}
		break;
	default:
		{
			exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_UNKNOWN;
		}
		break;
	}

	IReverbManager* pReverb = m_pSoundSystem->GetIReverbManager();
	if (pReverb && pReverb->UseHardwareVoices()) 
		Modes |= FMOD_HARDWARE;
	else
		Modes |= FMOD_SOFTWARE;

	if (m_pSoundSystem->g_nStreamDialogIntoMemory)
		Modes |= FMOD_OPENMEMORY_POINT;
	else
		Modes |= FMOD_OPENMEMORY;

	if (m_Props.nFlags & FLAG_SOUND_VOICE || !(m_Props.nFlags & FLAG_SOUND_LOOP))
		Modes |= FMOD_LOOP_OFF;

	//float f0 = gEnv->pTimer->GetAsyncCurTime();

	if (m_Props.nFlags & FLAG_SOUND_3D)
		m_ExResult = m_pCSEX->createSound(AssetDataPtrOrName, (FMOD_MODE)(FMOD_DEFAULT | FMOD_3D |  Modes | PS3_MEM_FLAG), &exinfo, &pEXSound);
	else
		m_ExResult = m_pCSEX->createSound(AssetDataPtrOrName, (FMOD_MODE)(FMOD_DEFAULT |  Modes | PS3_MEM_FLAG), &exinfo, &pEXSound);

	//float f1 = gEnv->pTimer->GetAsyncCurTime();
	//CryLog("<Sound Profile FMOD create sound> %s %.2f", m_Props.sName.c_str(), (f1-f0)*1000.0f);


	// if the suggested codec failed, just try the other one to make sure before we give up
	if (m_ExResult == FMOD_ERR_FORMAT)
	{
		TFixedResourceName sTemp = "suggested codec not available! Trying fallback..." + m_Props.sName;
		FmodErrorOutput(sTemp.c_str(), eSLT_Warning);

		exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_UNKNOWN; // new code, will be activated soon
		Modes &= ~FMOD_OPENMEMORY_POINT;
		Modes |= FMOD_OPENMEMORY;

		if (m_Props.nFlags & FLAG_SOUND_3D)
			m_ExResult = m_pCSEX->createSound(AssetDataPtrOrName, (FMOD_MODE)(FMOD_DEFAULT | FMOD_3D |  Modes | PS3_MEM_FLAG), &exinfo, &pEXSound);
		else
			m_ExResult = m_pCSEX->createSound(AssetDataPtrOrName, (FMOD_MODE)(FMOD_DEFAULT |  Modes | PS3_MEM_FLAG), &exinfo, &pEXSound);
	}

	if (IS_FMODERROR)
	{
		TFixedResourceName sTemp = "create sample sound failed! " + m_Props.sName;
		FmodErrorOutput(sTemp.c_str(), eSLT_Warning);
		return (NULL);
	}

	if (pEXSound)
	{
		//m_Info.nChannels = exinfo.numchannels;
		float fFreq;

		m_ExResult = pEXSound->getLength(&(m_Info.nLengthInMs), FMOD_TIMEUNIT_MS);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("get length ms failed! ", eSLT_Warning);
			return (NULL);
		}

		m_ExResult = pEXSound->getLength(&(m_Info.nLengthInSamples), FMOD_TIMEUNIT_PCM);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("get length pcm failed! ", eSLT_Warning);
			return (NULL);
		}
		
		m_ExResult = pEXSound->getDefaults(&fFreq, 0, 0, 0);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("get defaults failed! ", eSLT_Warning);
			return (NULL);
		}

		m_Info.nBaseFreq = (uint32) fFreq;
		m_ExResult = pEXSound->getFormat(0, 0, &(m_Info.nChannels), &(m_Info.nBitsPerSample));
		if (IS_FMODERROR)
		{
			FmodErrorOutput("get format failed! ", eSLT_Warning);
			return (NULL);
		}

		//m_ExResult = pEXSound->getMode(&SoundMode);
		//m_ExResult = pEXSound->setMode(FMOD_LOOP_NORMAL);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("set mode failed! ", eSLT_Warning);
			return (NULL);
		}

		// TODO review this, what will happen if compressed sound data is used?
		//m_Info.nLengthInBytes		= (int32) m_Info.nLengthInSamples*m_Info.nChannels*(m_Info.nBitsPerSample/8.0f);
		m_Info.nLengthInBytes = exinfo.length;
	}

	return (pEXSound);
}


//////////////////////////////////////////////////////////////////////////
tAssetHandle CSoundBufferFmodEx400::LoadAsStream(const char *AssetName, int nLength)
{
	FMOD::Sound	*pEXStream = 0;
	FMOD_CREATESOUNDEXINFO	exinfo;

	memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
	exinfo.cbsize           = sizeof(FMOD_CREATESOUNDEXINFO);
	exinfo.length           = nLength;
	
	if (m_Props.nFlags & FLAG_SOUND_3D)
		m_ExResult = m_pCSEX->createStream(AssetName, (FMOD_MODE)( FMOD_CREATESTREAM |FMOD_DEFAULT | FMOD_3D ), 0, &pEXStream);
	else
		m_ExResult = m_pCSEX->createStream(AssetName, (FMOD_MODE)( FMOD_CREATESTREAM |FMOD_DEFAULT ), 0, &pEXStream);

	if (IS_FMODERROR)
	{
		TFixedResourceName sTemp = "create stream sound failed! " + m_Props.sName;
		FmodErrorOutput(sTemp.c_str(), eSLT_Warning);
		return (NULL);
	}

	if (pEXStream)
	{
		float fFreq;
		m_ExResult = pEXStream->getLength(&(m_Info.nLengthInMs), FMOD_TIMEUNIT_MS);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("get length ms failed! ", eSLT_Warning);
			return (NULL);
		}

		m_ExResult = pEXStream->getLength(&(m_Info.nLengthInSamples), FMOD_TIMEUNIT_PCM);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("get length pcm failed! ", eSLT_Warning);
			return (NULL);
		}

		m_ExResult = pEXStream->getDefaults(&fFreq, 0, 0, 0);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("get defaults failed! ", eSLT_Warning);
			return (NULL);
		}
		m_Info.nBaseFreq = (uint32) fFreq;

		m_ExResult = pEXStream->getFormat(0, 0, &(m_Info.nChannels), &(m_Info.nBitsPerSample));
		if (IS_FMODERROR)
		{
			FmodErrorOutput("get format failed! ", eSLT_Warning);
			return (NULL);
		}

		//m_ExResult = pEXSound->getMode(&SoundMode);
		//m_ExResult = pEXStream->setMode(FMOD_LOOP_NORMAL);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("set mode failed! ", eSLT_Warning);
			return (NULL);
		}
		m_Info.nLengthInBytes = exinfo.length;
	}

	return (pEXStream);
}

//////////////////////////////////////////////////////////////////////////
void CSoundBufferFmodEx400::StreamOnComplete(IReadStream *pStream, unsigned nError)
{
	// pStream should be the same as m_StreamBuffer now, if s_StreamDialogIntoMemory was used
	
	FUNCTION_PROFILER( gEnv->pSystem,PROFILE_SOUND );
	
	// Dont need stream anymore.
	m_pReadStream = NULL;

	if (nError)
	{
		if (nError == ERROR_USER_ABORT)
			m_Info.bLoadFailure = false;
		else
			m_Info.bLoadFailure = true;

		if (m_StreamBuffer)
		{
			gEnv->pSoundSystem->Log(eSLT_Always, "<Sound> SB-StreamOnComplete-BufDel: %s Error:%d Buffer:%d\n", m_Props.sName.c_str(), nError, m_StreamBuffer);
			delete[] ((uint8*)m_StreamBuffer);
			m_StreamBuffer = NULL;
		}

		return;
	}

	FMOD::Sound* pExSound = (FMOD::Sound*)LoadAsSample((const char*)m_StreamBuffer, pStream->GetBytesRead());

	// remove buffer if FMOD holds a copy
	if (m_StreamBuffer && !m_pSoundSystem->g_nStreamDialogIntoMemory)
	{
		gEnv->pSoundSystem->Log(eSLT_Always, "<Sound> SB-StreamOnComplete-BufDel: %s Error:%d Buffer:%d\n", m_Props.sName.c_str(), nError, m_StreamBuffer);
		delete[] ((uint8*)m_StreamBuffer);
		m_StreamBuffer = NULL;
	}

	if (!pExSound)
	{
		m_pSoundSystem->Log(eSLT_Error, "<Sound> Cannot load sample sound %s\n", m_Props.sName.c_str());
		m_Info.bLoadFailure = true;
		LoadFailed();
		return;
	}		
	//set the sound source
	SetAssetHandle(pExSound, btSAMPLE);	
	SoundBufferLoaded();
	//TRACE("Sound-Streaming for %s finished.", m_Props.sName.c_str());
}

// Gets and Sets Parameter defined in the enumAssetParam list
//////////////////////////////////////////////////////////////////////////
bool CSoundBufferFmodEx400::GetParam(enumAssetParamSemantics eSemantics, ptParam* pParam) const
{
	switch (eSemantics)
	{	
	case apASSETNAME:	
		{
			string sTemp;

			if (!(pParam->GetValue(sTemp))) 
				return (false);
			
			sTemp = m_Props.sName.c_str();
			pParam->SetValue(sTemp);
		}
		break;
	case apASSETTYPE:	
		{
			int32 nTemp;
			if (!(pParam->GetValue(nTemp))) return (false);
			nTemp = m_Props.eBufferType;
			pParam->SetValue(nTemp);
			break;
		}	
	case apASSETFREQUENCY:	
		{
			int32 nTemp;
			if (!(pParam->GetValue(nTemp))) return (false);
				nTemp = m_Info.nBaseFreq;
				pParam->SetValue(nTemp);
			break;
		}
	case apLENGTHINBYTES:	
		{
			int32 nTemp;
			if (!(pParam->GetValue(nTemp))) return (false);
			switch (m_Props.eBufferType)
			{
			case btSAMPLE:
				nTemp = m_Info.nLengthInBytes;
				pParam->SetValue(nTemp);
				break;
			case btSTREAM: // Stream does not support Byte Length
				nTemp = m_Info.nLengthInBytes;
				pParam->SetValue(nTemp);
				break;
			default:
				return (false);
			}
			break;
		}
	case apLENGTHINMS:	
		{
			int32 nTemp;
			if (!(pParam->GetValue(nTemp))) 
				return (false);
			nTemp = m_Info.nLengthInMs;
			pParam->SetValue(nTemp);
			break;
		}
	case apLENGTHINSAMPLES:	
		{
			int32 nTemp;
			if (!(pParam->GetValue(nTemp))) 
				return (false);
			nTemp = m_Info.nLengthInSamples;
			pParam->SetValue(nTemp);
			break;
		}	
	case apBYTEPOSITION:	
		{
			int32 nTemp;
			switch (m_Props.eBufferType)
			{
			case btSAMPLE:
				return (false);
				break;
			case btSTREAM: 
				if (!(pParam->GetValue(nTemp))) 
					return (false);
				//nTemp = CS_Stream_GetPosition((CS_STREAM*)m_BufferData);
				pParam->SetValue(nTemp);
				break;
			default:
				return (false);
			}
			return (true);
		}	
	case apTIMEPOSITION:	
		{
			int32 nTemp;
			switch (m_Props.eBufferType)
			{
			case btSAMPLE:
				return (false);
				break;
			case btSTREAM: 
				if (!(pParam->GetValue(nTemp))) 
					return (false);
				//nTemp = CS_Stream_GetTime((CS_STREAM*)m_BufferData);
				pParam->SetValue(nTemp);
				break;
			default:
				return (false);
			}
			return (true);
		}	
	case apLOADINGSTATE:	
		{
			int32 nTemp;
			if (!(pParam->GetValue(nTemp))) 
				return (false);
			nTemp = m_Info.bLoadFailure;
			pParam->SetValue(nTemp);
			break;
		}
	case 	apNUMREFERENCEDSOUNDS:
		{
			int32 nTemp;
			if (!(pParam->GetValue(nTemp))) 
				return (false);
			nTemp = m_vecLoadReq.size();
			pParam->SetValue(nTemp);
		}
	default:	
    return (false);
		break;
	}
	return (true);
}

//////////////////////////////////////////////////////////////////////////
bool CSoundBufferFmodEx400::SetParam(enumAssetParamSemantics eSemantics, ptParam* pParam)
{
	switch (eSemantics)
	{
	case apASSETNAME:	// TODO Review this, Reset the Asset?
		return (false); // Setting this parameter not supported
		break;
	case apASSETTYPE:	// TODO Review this
		return (false); // Setting this parameter not supported
		break;
	case apASSETFREQUENCY:	
		{
			int32 nTemp;
			if (!(pParam->GetValue(nTemp))) return (false);
			switch (m_Props.eBufferType)
			{
			case btSAMPLE:
				//if (!CS_Sample_SetDefaults((CS_SAMPLE*)m_BufferData, nTemp, NULL, NULL, NULL)) 
				return (false);
				//m_Info.nBaseFreq = nTemp;
				break;
			case btSTREAM: // Stream does not support Frequency change
				return (false);
				break;
			default:
				return (false);
				break;
			}
		}
		break;
	case apLENGTHINBYTES:		
		return (false); // Setting this parameter not supported		break;
	case apLENGTHINMS:	
		return (false); // Setting this parameter not supported		break;
	case apBYTEPOSITION:	
		{
			int32 nTemp;
			if (!(pParam->GetValue(nTemp))) return (false);
			switch (m_Props.eBufferType)
			{
			case btSAMPLE:
				return (false); // Setting this parameter not supported
				break;
			case btSTREAM:
				//if (!CS_Stream_SetPosition((CS_STREAM*)m_BufferData, nTemp))
					return (false);
				break;
			default:
				return (false);
				break;
			}
		}
		break;
	case apTIMEPOSITION:	// TODO Review if seconds (float) or milliseconds (int) should be used
		{
			int32 nTemp;
			if (!(pParam->GetValue(nTemp))) 
				return (false);
			switch (m_Props.eBufferType)
			{
			case btSAMPLE:
				return (false); // Setting this parameter not supported
				break;
			case btSTREAM: 
				//if (!CS_Stream_SetTime((CS_STREAM*)m_BufferData, nTemp))
				return (false);
				break;
			default:
				return (false);
				break;
			}
		}
		break;
	case apLOADINGSTATE:		
		return (false); // Setting this parameter not supported		
		break;
	default:	
		return (false);
		break;
	}

}

//////////////////////////////////////////////////////////////////////////
bool CSoundBufferFmodEx400::UnloadData(const eUnloadDataOptions UnloadOption)
{
	FUNCTION_PROFILER( GetISystem(), PROFILE_SOUND );

	//GUARD_HEAP;
	//gEnv->pSoundSystem->Log(eSLT_Always, "<Sound> SB-UnloadData: %s %d\n", m_Props.sName.c_str(), (int) UnloadOption);
	bool bResult = true;
	bool bUnload = (UnloadOption == sbUNLOAD_ALL_DATA) || (UnloadOption == sbUNLOAD_ALL_DATA_NOW);

	// unneeded data can only be removed if its not used or not currently streamed in
	if (UnloadOption == sbUNLOAD_UNNEEDED_DATA && m_pReadStream==NULL)
	{
		bUnload = true;
		
		for (TBufferLoadReqVecIt It=m_vecLoadReq.begin(); It!=m_vecLoadReq.end(); ++It) 
			bUnload &= !(*It)->IsActive();
		for (TBufferLoadReqVecIt It=m_vecLoadReqToAdd.begin(); It!=m_vecLoadReqToAdd.end(); ++It) 
			bUnload &= !(*It)->IsActive(); 
	}

	if (bUnload)
	{
		if (m_BufferData)
		{
			// store processed sounds in a container to prevent racing condition and to delay the release() of the sound
			m_ProtectedSounds.reserve(m_vecLoadReq.size() + m_vecLoadReqToAdd.size());

			// Tell Sounds that their soundbuffers are unloaded now
			for (TBufferLoadReqVecIt It=m_vecLoadReq.begin(); It!=m_vecLoadReq.end(); ++It) 
			{
				CSound* pSound = (*It);
				m_ProtectedSounds.push_back(pSound);
				(*It)->OnAssetUnloaded(); 
			}
			for (TBufferLoadReqVecIt It=m_vecLoadReqToAdd.begin(); It!=m_vecLoadReqToAdd.end(); ++It) 
			{
				CSound* pSound = (*It);
				m_ProtectedSounds.push_back(pSound);
				(*It)->OnAssetUnloaded(); 
			}

			// release the FMOD Sound of this buffer
			FMOD::Sound* pExSound = (FMOD::Sound*)m_BufferData;
			{
				if (CSound::m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_PROGRAMMERSOUND)
					CSound::m_pSSys->Log(eSLT_Always, "----> Voice Buffer %s Before RELEASE", m_Props.sName.c_str());

				CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_BUFFER_RELEASE) );
				m_ExResult = pExSound->release();

				if (CSound::m_pSSys->g_nDebugSound == SOUNDSYSTEM_DEBUG_PROGRAMMERSOUND)
					CSound::m_pSSys->Log(eSLT_Always, "----> Voice Buffer %s After RELEASE", m_Props.sName.c_str());

				//gEnv->pSoundSystem->Log(eSLT_Always, "<Sound> SB-UnloadData-SoundRelease: %s \n", m_Props.sName.c_str());
			}
			if (IS_FMODERROR)
			{
				FmodErrorOutput("release sound failed! ", eSLT_Warning);
				bResult = false;
			}
			else
				CSoundSystem::g_nBuffersTrashed++;

			switch (m_Props.eBufferType)
			{
			case btSAMPLE:	
				SetAssetHandle(NULL, btSAMPLE);	
				break;
			case btSTREAM:	
				SetAssetHandle(NULL, btSTREAM);	
				break;
			}

			m_ProtectedSounds.resize(0);
		}

		if (m_StreamBuffer)
		{
			if (m_pReadStream)
			{
				// some bad problems occured when this code was hit. For now UnloadData should not be called while
				// streaming is still active, thats why there are checks in the SoundAssetManager
				//gEnv->pSoundSystem->Log(eSLT_Always, "<Sound> SB-Unload-StreamAbort: %s Stream:%d Buf:%d\n", m_Props.sName.c_str(), m_pReadStream, m_StreamBuffer);
				m_pReadStream->Abort();	
				m_pReadStream = NULL;
			}
			else
			{
				//gEnv->pSoundSystem->Log(eSLT_Always, "<Sound> SB-Unload-BufDel: %s Buf:%d\n", m_Props.sName.c_str(), m_StreamBuffer);
				delete[] ((uint8*)m_StreamBuffer);
				m_StreamBuffer = NULL;
			}
		}
	}
	//GUARD_HEAP;

	return bResult;
}

#endif
