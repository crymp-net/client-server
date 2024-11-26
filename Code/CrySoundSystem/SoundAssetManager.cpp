////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   SoundAssetManager.cpp
//  Version:     v1.00
//  Created:     26/4/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: SoundAssetManager is responsible for memory allocation of
//							 assets and answer queries about their state and priority
// -------------------------------------------------------------------------
//  History:
//  8/6/05   Nick changed createBuffer routine so precache could call it
//                also put warning note in update buffers so precache buffers wouldn't be released
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "SoundAssetManager.h"
#include <IRenderer.h>
#include "IAudioDevice.h"
#include "SoundSystem.h" // CSoundSystem should be ISoundSystem
#include "Sound.h"
#include "ITimer.h"
#include <CrySizer.h>
#include <StlUtils.h>

CSoundAssetManager::CSoundAssetManager(CSoundSystem *pSoundSystem)
{
	m_pSoundSystem			= pSoundSystem;
	m_bUseMemoryBlock		= false;	// toggles if internal Memory Block should be used or not
	m_MemoryBlockStart	= NULL;	
	m_nMemoryBlockSize	= 0;
	m_nUsedMemorySize		= 0;
	m_fTotalSoundMemory = 0;
	m_fTotalSecondaryMemory = 0;
	m_nUsingOldFMODStatsCounter = RENEW_FMOD_STATS_LIMIT;
	m_fBudgetRatio = 1.0f;
	m_fMainThreshold = 0.0f;
	m_fSecondaryThreshold = 0.0f;
	m_nLastSoundBuffersIndex = 0;
}

CSoundAssetManager::~CSoundAssetManager(void)
{
	ShutDown();
}

//////////////////////////////////////////////////////////////////////////
// Management
//////////////////////////////////////////////////////////////////////////

// gives a big memory block in Bytes to the Manager
bool CSoundAssetManager::SetMemoryBlock( void* pBlockStart, unsigned int nBlockSize)
{
	assert (pBlockStart);

	// delete the old Memory Block
	if (m_bUseMemoryBlock)
	{
		delete[] (char*)m_MemoryBlockStart;
		m_nMemoryBlockSize = 0;
		m_bUseMemoryBlock = false;
	}

	// set the new one
	if (pBlockStart && nBlockSize)
	{
		m_MemoryBlockStart	= pBlockStart;
		m_nMemoryBlockSize	= nBlockSize;
		m_bUseMemoryBlock		= true;
	}

	return (m_bUseMemoryBlock);
}

// Let the Manager allocate a big memory block in Bytes to deal with
bool CSoundAssetManager::AllocMemoryBlock(unsigned int nBlockSize)
{
	// delete the old Memory Block
	if (m_bUseMemoryBlock)
	{
		delete[] (char*)m_MemoryBlockStart;
		m_nMemoryBlockSize = 0;
		m_bUseMemoryBlock = false;
	}
	
	// create the new one
	if (nBlockSize)
	{
		m_MemoryBlockStart = new int8[ nBlockSize ];
		m_nMemoryBlockSize	= nBlockSize;
		m_bUseMemoryBlock		= true;
	}

	return (m_bUseMemoryBlock);
}

// returns a Ptr to a free chunk of memory to be used
void* CSoundAssetManager::GetMemoryChunk(SSoundBufferProps BufferProps, unsigned int nChunkSize)
{
	// only alloc memory for a SoundBuffer that already exist
	SoundBufferPropsMapItorConst It = m_soundBuffers.find(BufferProps);
	if (It == m_soundBuffers.end())
		return (NULL);

	CSoundBuffer*	pBuf = It->second;

	// remove all old data
	if (pBuf->m_BufferData)
	{
		delete[] (char*)pBuf->m_BufferData;
		pBuf->m_BufferData = NULL;
	}

	void* pMemChunk = new int8[ nChunkSize ];
	
	// out of memory
	if (!pMemChunk)
		return NULL;

	m_nUsedMemorySize += nChunkSize;
	memset(pMemChunk, 0, nChunkSize);
	
	return (pMemChunk);
}

// frees a chunk of memory that was used in a SoundBuffer
bool CSoundAssetManager::FreeMemoryChunk(SSoundBufferProps BufferProps)
{
	// only free memory of a SoundBuffer that already exist
	SoundBufferPropsMapItorConst It = m_soundBuffers.find(BufferProps);
	if (It == m_soundBuffers.end())
		return (false);

	CSoundBuffer*	pBuf = It->second;
	uint32 nMemorySize = pBuf->GetInfo()->nLengthInBytes;

	// remove all old data
	if (pBuf->m_BufferData)
	{
		delete[] (char*)pBuf->m_BufferData;
		pBuf->m_BufferData = NULL;
		m_nUsedMemorySize -= nMemorySize;
	}

	return (true);
}

// gives a big memory block to the Manager
//bool CSoundAssetManager::FreeMemoryBlock()
//{
	//return (false);
//}

// frees all SoundBuffers, deletes all data;
bool CSoundAssetManager::ShutDown()
{
	//SoundBufferPropsMapItor It = m_soundBuffers.begin();
	//while (It!=m_soundBuffers.end())
	//{			
	//	CSoundBuffer *pBuf = It->second;
	//	//if (pBuf->Loaded())
	//	if (pBuf)
	//	{
	//		RemoveSoundBuffer(pBuf->GetProps());
	//	}
	//	else
	//		++It;
	//} //it

	m_lockedResources.resize(0);
	m_soundBuffers.clear();
	m_nUsedMemorySize = 0;

	// delete the old Memory Block
	if (m_bUseMemoryBlock)
	{
		delete[] (char*)m_MemoryBlockStart;
		m_nMemoryBlockSize	= 0;
		m_bUseMemoryBlock		= false;
	}

	return (true);
}


// Returns current Memory Usage
unsigned int CSoundAssetManager::GetMemUsage()
{
	uint32 nSize = 0;
	SoundBufferPropsMapItorConst ItEnd = m_soundBuffers.end();
	for (SoundBufferPropsMapItorConst It=m_soundBuffers.begin(); It!=ItEnd; ++It)
	{			
		CSoundBuffer *pBuf = It->second;
		nSize += pBuf->GetMemoryUsed();
	} //it

	return nSize;
}

// compute memory-consumption, returns rough estimate in MB
void CSoundAssetManager::GetMemoryUsage(class ICrySizer* pSizer) const
{	
	pSizer->AddObject(this, sizeof(*this));
	pSizer->AddObject(m_soundBuffers);			
}
	

unsigned int CSoundAssetManager::GetNumberBuffersLoaded()
{
	return (m_soundBuffers.size());
}

// Cleans up dead bufffers, defrags or kicks out low priority Buffers
bool CSoundAssetManager::Update()
{
	FUNCTION_PROFILER( gEnv->pSystem, PROFILE_SOUND );

	if (!m_pSoundSystem->IsEnabled())
		return true;

	// Lets see how much memory is used and if the cache is too full, remove some buffers.
	if (m_nUsingOldFMODStatsCounter == RENEW_FMOD_STATS_LIMIT)
	{
		uint32 nTotal = 0;
		uint32 nSecondary = 0;
		uint32 nMainCacheSize = 0;

		m_pSoundSystem->GetIAudioDevice()->GetMemoryStats(admqALL, &nTotal);
		m_pSoundSystem->GetIAudioDevice()->GetMemoryStats(admqSOUNDSECONDARY, &nSecondary);
		m_pSoundSystem->GetIAudioDevice()->GetMemoryStats(admqCACHESIZEMAIN, &nMainCacheSize);
		
		m_fTotalSoundMemory = (float)(nTotal/1024)/1024;
		m_fTotalSecondaryMemory = (float)(nSecondary/1024)/1024;
		float fCacheMainSize = (float)(nMainCacheSize/1024)/1024;
		float fCacheFillRation = m_pSoundSystem->g_fCacheFillRatio;

		if (m_fTotalSecondaryMemory)
		{
			ICVar* pRSXCacheSize = gEnv->pConsole->GetCVar("s_CacheRSXSize");
			m_fSecondaryThreshold = pRSXCacheSize ? pRSXCacheSize->GetFVal() * fCacheFillRation : 0;
		}

		m_fMainThreshold = fCacheMainSize * fCacheFillRation;

		if (m_fTotalSoundMemory > 0)
			m_fBudgetRatio = m_fMainThreshold / m_fTotalSoundMemory;
			
		m_nUsingOldFMODStatsCounter = 0;
	}
	else
	{
		++m_nUsingOldFMODStatsCounter;
	}

	bool bTooManyOpenFiles = (m_pSoundSystem->GetIAudioDevice()->GetNumOpenFileHandle() > m_pSoundSystem->g_nOpenFileHandleMax);
	assert(!bTooManyOpenFiles);
	bool bInBudget = (m_fTotalSoundMemory < m_fMainThreshold) && (m_fTotalSecondaryMemory <= m_fSecondaryThreshold) && !bTooManyOpenFiles;
	bool bRemoved = false;

	size_t const nBuffersCommands = m_BufferCommands.size();
	if (nBuffersCommands)
	{
		tvecBufferCommandsIter			It		= m_BufferCommands.begin();
		tvecBufferCommandsConstIter	ItEnd	= m_BufferCommands.end();
		for (; It != ItEnd;)
		{
			CSoundBuffer*	const pBuf = (*It).pBuffer;
			bool bRemoveSuccess = false;
			if((*It).bRemoveBuffer)
			{
				bRemoveSuccess = pBuf->UnloadData(sbUNLOAD_ALL_DATA);

				// refcount is 2 because of m_ToBeRemoved and m_soundBuffers
				if (pBuf->m_nRef == 2)
					RemoveSoundBuffer(*(pBuf->GetProps()));
			}
			else
				bRemoveSuccess = pBuf->UnloadData((*It).eUnloadOption);

			if(bRemoveSuccess)
			{
				It		= m_BufferCommands.erase(It);
				ItEnd	= m_BufferCommands.end();
			}
			else
			{
				++It;
			}
		}
	}

	// BufferCommands maybe not be empty if some buffers couldnt unloaded out of MT reasons
	// on the next iteration it will be tried again
	//assert(!m_BufferCommands.size() && "BufferCommands not empty!");

	size_t const nSoundBuffersCount = m_soundBuffers.size();
	if (nSoundBuffersCount)
	{
		int nSoundBufferItCount = (int)cry_ceilf((gEnv->pTimer->GetFrameTime()/1.0f) * (float)nSoundBuffersCount);	

		// make sure we at least process one
		nSoundBufferItCount = max(nSoundBufferItCount, 1);

		SoundBufferPropsMapItor				It		= m_soundBuffers.begin();
		SoundBufferPropsMapItorConst	ItEnd	= m_soundBuffers.end();

		// continue where we stopped last time if index is within the range
		if (nSoundBuffersCount > m_nLastSoundBuffersIndex)
			std::advance(It, m_nLastSoundBuffersIndex);
		else
			m_nLastSoundBuffersIndex = 0;

		for (int i=0; i<nSoundBufferItCount; ++i)
		{
			bRemoved = false;
			CSoundBuffer*	const pBuf = It->second;

			if (pBuf) 
			{
				// pBuf is valid
				if (pBuf->LoadFailure() && !pBuf->Loading())
					stl::push_back_unique(m_BufferCommands, SBufferCommand(pBuf, sbUNLOAD_NONE, true));
				else
				{
					float const fBufTime	= pBuf->GetTimer().GetSeconds();
					float const fNowTime	= gEnv->pTimer->GetFrameStartTime().GetSeconds();
					float const fDiff			= fNowTime - fBufTime;

					if (fDiff > (20.0f * m_fBudgetRatio) || !bInBudget || pBuf->GetProps()->nPrecacheFlags & FLAG_SOUND_PRECACHE_UNLOAD_NOW)
					{
						++pBuf->GetInfo()->nTimedOut;

						float const fNeeded = AssetStillNeeded(pBuf) * m_fBudgetRatio; // Calls the evaluation Algorithm

						if (pBuf->Loaded() || pBuf->Loading())
							pBuf->UpdateTimer(gEnv->pTimer->GetFrameStartTime());

						if (fNeeded < 1.0f || !bInBudget)  
						{
							// remove buffers which hold no data and are not loading and have been unloaded for 60 seconds
							if (!pBuf->Loaded() && !pBuf->Loading() && fDiff > 60.0f)
							{
								stl::push_back_unique(m_BufferCommands, SBufferCommand(pBuf, sbUNLOAD_NONE, true));
							}
							else
							{
								if (pBuf->GetProps()->nPrecacheFlags & FLAG_SOUND_PRECACHE_UNLOAD_NOW && m_pSoundSystem->g_nUnloadData != 0)
								{
									if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_DETAIL)
										m_pSoundSystem->Log(eSLT_Message, "<Sound> unloading (now) data on: %s", pBuf->GetName());

									stl::push_back_unique(m_BufferCommands, SBufferCommand(pBuf, sbUNLOAD_ALL_DATA, false));
									pBuf->GetProps()->nPrecacheFlags = 0; // reset the Precache flags to prevent endless unloading
								}

								// if needed, maybe we can unload some data partially
								// disable until FMOD bug is fixed
								if (!bInBudget && m_pSoundSystem->g_nUnloadData != 0)
								{
									if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_DETAIL)
										m_pSoundSystem->Log(eSLT_Message, "<Sound> unloading data on: %s", pBuf->GetName());

									stl::push_back_unique(m_BufferCommands, SBufferCommand(pBuf, sbUNLOAD_UNNEEDED_DATA, false));
								}

							}
						}
					}
				}
			}

			// next unprocessed element
			++It;
			++m_nLastSoundBuffersIndex;

			// if current It is the last sound buffer then reset It to begin
			if (It == ItEnd)
			{
				It = m_soundBuffers.begin();
				m_nLastSoundBuffersIndex = 0;
			}
		}
	}

	return (false);
}


// writes output to screen in debug
void CSoundAssetManager::DrawInformation(IRenderer* pRenderer, float xpos, float ypos, enumAssetManagerDrawInformation DrawMode)
{
	uint32 nUsedMem = m_nUsedMemorySize;
	int nBufferCount = 0;

	if (!(m_soundBuffers.empty()) && m_nUsedMemorySize == 0)
	{
		SoundBufferPropsMapItorConst ItEnd = m_soundBuffers.end();
		for (SoundBufferPropsMapItorConst It=m_soundBuffers.begin(); It!=ItEnd; ++It)
		{			
			++nBufferCount;
			CSoundBuffer *pBuf = It->second;

			if (pBuf->m_BufferData != NULL && pBuf->GetProps()->eBufferType != btSTREAM)
				nUsedMem += pBuf->GetMemoryUsed();
				//nUsedMem += pBuf->GetInfo()->nLengthInBytes;
		}
	}

	float fColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	float fWhite[4] = {1.0f, 1.0f, 1.0f, 0.7f};
//	float fBlue[4] = {0.0f, 0.0f, 1.0f, 0.7f};
	float fCyan[4] = {0.0f, 1.0f, 1.0f, 0.7f};
	float fRed[4] = {1.0f, 0.0f, 0.0f, 1.0f};

	TFixedResourceName sMemUnit = "(KB)";
	//string sMemUnit = "(KB)";
	uint32 fUsedMem = (nUsedMem/1024);
	if (fUsedMem > 1024)
	{
		sMemUnit = "(MB)";
		fUsedMem /= 1024;
	}

	pRenderer->Draw2dLabel(xpos, ypos, 1, fColor, false, "Total Buffers: %d", m_soundBuffers.size());
	ypos += 10;
	
	pRenderer->Draw2dLabel(xpos, ypos, 1, fColor, false, "Used Memory: %d %s", fUsedMem, sMemUnit.c_str());
	ypos += 10;

	SoundBufferPropsMapItorConst ItEnd = m_soundBuffers.end();
	for (SoundBufferPropsMapItorConst It=m_soundBuffers.begin(); It!=ItEnd; ++It)
	{			
		CSoundBuffer *pBuf = It->second;
		memcpy(fColor,fWhite,4*sizeof(float));

		if (pBuf->GetProps()->nPrecacheFlags & FLAG_SOUND_PRECACHE_STAY_IN_MEMORY)
			memcpy(fColor,fCyan,4*sizeof(float));

		int32 nNumSimple = 0;
		ptParamINT32 ptTemp(nNumSimple);
		if (pBuf->GetParam(apNUMSIMPLESOUNDS, &ptTemp))
			ptTemp.GetValue(nNumSimple);

		//if (pBuf->GetSample())
		//if (pBuf->GetAssetHandle()) 
//		if (!pBuf->Loaded())
		{
			//float nThreshold = m_pSoundSystem->g_nCacheSize * 0.9f;
			//if (m_nTotalSoundMemory < nThreshold)
			//{
				//memcpy(fColor,fWhite,4*sizeof(float));
				
			//}
			//else
			//{
//				if (pBuf->GetProps()->nPrecacheFlags & FLAG_SOUND_PRECACHE_STAY_IN_MEMORY)
//					memcpy(fColor,fCyan,4*sizeof(float));

				if (!pBuf->Loaded())
					memcpy(fColor,fRed,4*sizeof(float));
				else
					fColor[3] = AssetStillNeeded(pBuf) + 0.3f;
			//}

				if (DrawMode == amdiALLBUFFERS || 
					(DrawMode == amdiONLYEVENTBUFFERS && pBuf->GetProps()->eBufferType == btEVENT) || 
					(DrawMode == amdiONLYSAMPLEBUFFERS && pBuf->GetProps()->eBufferType == btSAMPLE))
				{
					stack_string oTemp;
					if (nNumSimple > 0)
						oTemp.Format("%s Mem: %dKB Ref: %d Simple: %d", pBuf->GetName(), pBuf->GetMemoryUsed()/1024, pBuf->m_nRef, nNumSimple);
					else
						oTemp.Format("%s Mem: %dKB Ref: %d", pBuf->GetName(), pBuf->GetMemoryUsed()/1024, pBuf->m_nRef);

					pRenderer->Draw2dLabel(xpos, ypos, 1, fColor, false, oTemp);
					ypos += 10;
				}
		}
	} //it
}


//////////////////////////////////////////////////////////////////////////
// Asset Management
//////////////////////////////////////////////////////////////////////////

// // gets or creates a SoundBuffer for a referred sound on a specific platform and returns the pointer to it
// NICK moved pRefSound test for NULL further down so precache routine can call this
CSoundBuffer* CSoundAssetManager::GetOrCreateSoundBuffer(const SSoundBufferProps &BufferProps, CSound* pRefSound)
{
	CSoundBuffer* pBuf = GetSoundBufferPtr(BufferProps);

	if (!pBuf)
	{
		if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_SIMPLE)
			m_pSoundSystem->Log(eSLT_Message, "<Sound> AssetManager: Create new Buffer %s ", BufferProps.sName.c_str());

    pBuf = m_pSoundSystem->GetIAudioDevice()->CreateSoundBuffer(BufferProps);
	}

	if (pBuf)
	{
		m_soundBuffers[BufferProps] = pBuf;
		//pBuf->GetProps()->nPrecacheFlags = BufferProps.nPrecacheFlags;
		//pBuf->GetProps()->nFlags = BufferProps.nFlags;
		//m_pSoundSystem->Log(eSLT_Message, "Sound-AssetManager: SoundBuffer %s %s \n", BufferProps.sName.c_str(), "created successfully.");

    // moved this test to here so pSound ptr can be null when precache called
    if (pRefSound)
		  pRefSound->SetSoundBufferPtr(pBuf);

		//pBuf->Load(pRefSound);
	}
	else
	{
		m_pSoundSystem->Log(eSLT_Warning, "<Sound> AssetManager: Could not create %s!", BufferProps.sName.c_str());
	}

	return (pBuf);
}

// removes a SoundBuffer from the AssetManager
bool CSoundAssetManager::RemoveSoundBuffer(const SSoundBufferProps &BufferProps)
{
	SoundBufferPropsMapItor itor = m_soundBuffers.find(BufferProps);

	if (itor != m_soundBuffers.end())
	{
		CSoundBuffer *pBuf = (itor->second);

		if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_DETAIL)
			m_pSoundSystem->Log(eSLT_Message, "<Sound> AssetManager: Removing sound [%s]", itor->first.sName.c_str());

		m_soundBuffers.erase(itor);
		return true;
	}
	else
	{
		if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_DETAIL)
			m_pSoundSystem->Log(eSLT_Warning, "<Sound> AssetManager: Trying to remove sound [%s] NOT FOUND!!!", BufferProps.sName.c_str());
	}

	return (false);
}

// locks the SoundBuffers so they don't get unloaded
void CSoundAssetManager::LockResources()
{
	m_lockedResources.resize(0);
	m_lockedResources.reserve(m_soundBuffers.size());

	SoundBufferPropsMapItorConst ItEnd = m_soundBuffers.end();
	for (SoundBufferPropsMapItorConst It=m_soundBuffers.begin(); It!=ItEnd; ++It)
	{			
		CSoundBuffer *pBuf = It->second;
		m_lockedResources.push_back( pBuf );
	}
}

// unlocks the SoundBuffers so they could be unload if needed
void CSoundAssetManager::UnlockResources()
{
	m_lockedResources.clear();
}

// unloads ALL SoundBuffers from the AssetManager
void CSoundAssetManager::UnloadAllSoundBuffers()
{
	//GUARD_HEAP;

	// first iterate through all buffers and unload events to prevent dialog event access invalid memory
	SoundBufferPropsMapItorConst IterEnd = m_soundBuffers.end();
	for (SoundBufferPropsMapItorConst Iter=m_soundBuffers.begin(); Iter!=IterEnd; ++Iter)
	{
		//m_pSoundSystem->GetSystem()->GetILog()->LogToFile("Removing sound [%s]", itor->first.sName.c_str());
		CSoundBuffer *pBuf = (Iter->second);
		if (pBuf && m_pSoundSystem->g_nUnloadData && pBuf->GetProps()->eBufferType == btEVENT)
			pBuf->UnloadData(sbUNLOAD_ALL_DATA_NOW);
	}

	// all other buffers 2nd iteration
	for (SoundBufferPropsMapItorConst Iter=m_soundBuffers.begin(); Iter!=IterEnd; ++Iter)
	{
		//m_pSoundSystem->GetSystem()->GetILog()->LogToFile("Removing sound [%s]", itor->first.sName.c_str());
		CSoundBuffer *pBuf = (Iter->second);
		if (pBuf && m_pSoundSystem->g_nUnloadData && pBuf->GetProps()->eBufferType != btEVENT)
			pBuf->UnloadData(sbUNLOAD_ALL_DATA_NOW);
	}

	// leave in for debugging
	//for (SoundBufferPropsMapItorConst Iter=m_soundBuffers.begin(); Iter!=IterEnd; ++Iter)
	//{
	//	CSoundBuffer *pBuf = (Iter->second);
	//	if (pBuf && (pBuf->GetAssetHandle() || pBuf->Loading()))
	//	{
	//		assert(0);
	//		int a = 0; // catch me
	//	}
	//}



}

// removes project dependency
void CSoundAssetManager::RemoveProjectDependency()
{
	SoundBufferPropsMapItorConst IterEnd = m_soundBuffers.end();
	for (SoundBufferPropsMapItorConst Iter=m_soundBuffers.begin(); Iter!=IterEnd; ++Iter)
	{
		//m_pSoundSystem->GetSystem()->GetILog()->LogToFile("Removing sound [%s]", itor->first.sName.c_str());
		CSoundBuffer *pBuf = (Iter->second);
		if (pBuf) 
			pBuf->GetProps()->nProjectIndex = PROJECTINDEX_INVALID;
	}
}

// informs about project availability
void CSoundAssetManager::ProjectWasLoaded(const ProjectIndex nProjectIndex)
{
	SoundBufferPropsMapItorConst IterEnd = m_soundBuffers.end();
	for (SoundBufferPropsMapItorConst Iter=m_soundBuffers.begin(); Iter!=IterEnd; ++Iter)
	{
		CSoundBuffer *pBuf = (Iter->second);
		if (pBuf && (pBuf->GetProps()->nProjectIndex == nProjectIndex))
			pBuf->Load(NULL);
	}
}

// Checks and returns a Ptr to a loaded Asset or NULL if its not there
CSoundBuffer* CSoundAssetManager::GetSoundBufferPtr(const SSoundBufferProps &BufferProps) const
{	
	//GUARD_HEAP;
	CSoundBuffer *pBuf = NULL;

	//float f0 = 0.0f;
	//float f1 = 0.0f;
	//float f2 = 0.0f;
	//f0 = gEnv->pTimer->GetAsyncCurTime();

	SoundBufferPropsMapItorConst It = m_soundBuffers.find(BufferProps);
	if (It != m_soundBuffers.end())
	{
		pBuf = (It->second);
		pBuf->GetProps()->nProjectIndex = BufferProps.nProjectIndex;
		CTimeValue TimeNow = gEnv->pTimer->GetFrameStartTime();
		pBuf->UpdateTimer(TimeNow);
		//f1 = gEnv->pTimer->GetAsyncCurTime();
		
		pBuf->Load(NULL);
	}
	else
	{
		if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_SIMPLE)
			m_pSoundSystem->Log(eSLT_Warning, "<Sound> AssetManager: %s not found", BufferProps.sName.c_str());
	}

	//f2 = gEnv->pTimer->GetAsyncCurTime();

	// TOMAS Profiling
	//{
	//	float fStep1 = (f1-f0)*1000.0f;
	//	float fStep2 = (f2-f1)*1000.0f;
	//	float fTotal = fStep1+fStep2;
	//	CryLog("<---------> SoundSystem Profile GetOrCreateSoundBuffer> %s %.2f", BufferProps.sName.c_str(), fTotal);

	//	if (fTotal > 0.1f)
	//		int catchme = 0;

	//}

	//GUARD_HEAP;
	return pBuf;
}

bool CSoundAssetManager::IsLoaded(const SSoundBufferProps &BufferProps)
{
	return (GetSoundBufferPtr(BufferProps) != NULL);
}

float CSoundAssetManager::AssetStillNeeded(CSoundBuffer* pBuf)
{
	// pBuf was tested before and is valid!

	if (m_pSoundSystem->g_nUnloadData == 0)
		return (1.0f);

	if (pBuf->GetProps()->nPrecacheFlags & FLAG_SOUND_PRECACHE_STAY_IN_MEMORY && pBuf->Loaded())
		return (1.0f);

	int nNumReferencedSounds = 0;
	ptParamINT32 TempParam(nNumReferencedSounds);
	pBuf->GetParam(apNUMREFERENCEDSOUNDS, &TempParam);
	TempParam.GetValue(nNumReferencedSounds);

	if (pBuf->GetProps()->nPrecacheFlags & FLAG_SOUND_PRECACHE_UNLOAD_NOW)
		return (0.0f);

	if (nNumReferencedSounds)
		return (1.0f);

	if (pBuf->Loading())
		return (1.0f);

	CTimeValue tDeltaTime = gEnv->pTimer->GetFrameStartTime() - pBuf->GetTimer();
	
	// right now that would be sqrt(nTimesUsed) plus some minimal of 10, that divided through the
	// seconds of that last Update (+1 to rule out div(0)), and that multiplied by the squared number of references to the Buffer
	//float fCacheThreshold = ((sqrt((float)pBuf->GetInfo()->nTimesUsed)+20) / (1.0f + tDeltaTime.GetSeconds()));
	
	uint32 nUsedOverTime = 0;
	if (pBuf->GetInfo()->nTimesUsed > pBuf->GetInfo()->nTimedOut)
		nUsedOverTime = pBuf->GetInfo()->nTimesUsed - pBuf->GetInfo()->nTimedOut;
	
	float fCacheThreshold = nUsedOverTime / (1.0f + (tDeltaTime.GetSeconds()*tDeltaTime.GetSeconds() ));

	return (max(0.0f, fCacheThreshold - 1.0f));
}


//////////////////////////////////////////////////////////////////////////
// Information
//////////////////////////////////////////////////////////////////////////

// Gets and Sets Parameter defined in the enumAssetParam list
//bool CSoundAssetManager::GetParam(enumAssetManagerParamSemantics eSemantics, ptParam* pParam)
//{
//}
//
//bool CSoundAssetManager::SetParam(enumAssetManagerParamSemantics eSemantics, ptParam* pParam)
//{
//}

#include UNIQUE_VIRTUAL_WRAPPER(ISoundAssetManager)
