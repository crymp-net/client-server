#include "StdAfx.h"
#include "FileCacheManager.h"
#include "SoundCVars.h"
#include <IRenderer.h>
#include <IRenderAuxGeom.h>
#include "../CrySystem/CustomMemoryHeap.h"
#include "ISerialize.h"
#include "SoundSystem.h"
#include <IJobManager_JobDelegator.h>

#if defined SOUNDSYSTEM_USE_FMODEX400
#include "AudioDeviceFmodEx400.h"
#else
#include "AudioDeviceFmodEx400_NULL.h"
#endif // SOUNDSYSTEM_USE_FMODEX400
#include <FmodEx/inc/fmod_errors.h>

DECLARE_JOB("RunQueuedRead", TRunQueuedReadJob, CFileCacheManager::RunQueuedRead);

// Static member initialization
size_t CFileCacheManager::m_nCurrentByteTotal  = 0;
size_t CFileCacheManager::m_nMaxByteTotal      = 0;
size_t CFileCacheManager::m_nMaxByteMain       = 0;
size_t CFileCacheManager::m_nMaxByteExtended   = 0;
size_t CFileCacheManager::m_nMaxByteAdditional = 512 << 10; // To account for alignment and fragmentation we request 0.5 MiB more.

//////////////////////////////////////////////////////////////////////////
CFileCacheManager::CFileCacheManager()
: m_bCanCacheEntries(true)
{
	m_mapKnownFiles.reserve(256);
	m_mapQueuedStreams.reserve(32);
	m_aoQueuedRequestInfos.reserve(4);
	m_aoQueuedUnloadEntries.reserve(32);
	m_aoQueuedPreloadEntries.reserve(32);
	m_mapQueuedCacheRequests.reserve(2);
}

//////////////////////////////////////////////////////////////////////////
CFileCacheManager::~CFileCacheManager()
{
	m_pMemoryHeapMain     = NULL;
	m_pMemoryHeapExtended = NULL;
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::Init()
{
	// Pass NULL to cache globals and game hints only
	ParsePreloadsXML(NULL);
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::Update()
{
	if (m_oKnownFilesCS.TryLock())
	{
		// First finish queued requests.
		ProcessQueuedRequests();

		// Then handle the pre/un-loading.
		UpdatePatchRequestsInternal();

		if (m_nCurrentByteTotal > m_nMaxByteTotal)
		{
			// Remove any removable file entry.
			tMapFileCacheEntries::const_iterator const IterEntriesEnd(m_mapKnownFiles.end());

			for (tMapFileCacheEntries::iterator Iter(m_mapKnownFiles.begin()); Iter != IterEntriesEnd; ++Iter)
			{
				SFileCacheEntry& rFileCacheEntry = Iter->second;

				if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_REMOVABLE) != 0)
				{
					UncacheFileCacheEntryInternal(rFileCacheEntry, true);
				}
			}
		}

		if (!m_mapQueuedStreams.empty() && m_oQueuedStreamsCS.TryLock())
		{
			tMapQueuedStreams::const_iterator const IterReadsEnd(m_mapQueuedStreams.end());

			for (tMapQueuedStreams::iterator IterReads(m_mapQueuedStreams.begin()); IterReads != IterReadsEnd; ++IterReads)
			{
				FinishStreamInternal(IterReads->first, IterReads->second, false);
			}

			m_mapQueuedStreams.clear();

			m_oQueuedStreamsCS.Unlock();
		}

		if (!m_mapQueuedCacheRequests.empty() && m_oQueuedCacheRequestsCS.TryLock())
		{
			// Handle queued cache requests, use a local copy as CacheFileInternal removes directly.
			tMapQueuedCacheRequests const mapQueuedCacheRequests(m_mapQueuedCacheRequests);
			tMapQueuedCacheRequests::const_iterator const IterQueuedCacheRequestsEnd(mapQueuedCacheRequests.end());

			for (tMapQueuedCacheRequests::const_iterator Iter(mapQueuedCacheRequests.begin()); Iter != IterQueuedCacheRequestsEnd; ++Iter)
			{
				CacheFileInternal(Iter->first, Iter->second.m_nRequestFileOffset, Iter->second.m_nRequestBytes, true);
			}

			m_oQueuedCacheRequestsCS.Unlock();
		}

		m_oKnownFilesCS.Unlock();
	}
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::UpdatePatchRequestsInternal()
{
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SOUND);

	if ((!m_aoQueuedPreloadEntries.empty() || !m_aoQueuedUnloadEntries.empty()) && m_oQueuedPreloadUnloadEntriesCS.TryLock())
	{
		tVecFileCacheEntryCRCs aoRequestPatchFileEntries(m_aoQueuedPreloadEntries);
		tVecFileCacheEntryCRCs::const_iterator IterEnd(aoRequestPatchFileEntries.end());

		for (tVecFileCacheEntryCRCs::iterator Iter(aoRequestPatchFileEntries.begin()); Iter != IterEnd; ++Iter)
		{
			if (PreloadEntryIntoFMODInternal(*Iter))
			{
				// Only remove from the queue if the preloading was actually successful.
				stl::find_and_erase(m_aoQueuedPreloadEntries, *Iter);
			}
		}

		tVecFileCacheEntryCRCs aoRequestUnpatchFileEntries(m_aoQueuedUnloadEntries);
		IterEnd = aoRequestUnpatchFileEntries.end();

		for (tVecFileCacheEntryCRCs::iterator Iter(aoRequestUnpatchFileEntries.begin()); Iter != IterEnd; ++Iter)
		{
			if (UnloadEntryFromFMODInternal(*Iter))
			{
				// Only remove from the queue if the unloading was actually successful.
				stl::find_and_erase(m_aoQueuedUnloadEntries, *Iter);
			}
		}

		m_oQueuedPreloadUnloadEntriesCS.Unlock();
	}
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::AllocateHeap(size_t const nSize, char const* const sUsage)
{
	if (nSize > 0)
	{
		m_pMemoryHeapMain = static_cast<CCustomMemoryHeap*>(gEnv->pSystem->GetIMemoryManager()->CreateCustomMemoryHeapInstance(true));

		if (m_pMemoryHeapMain != NULL)
		{
			m_nMaxByteMain = nSize << 10;
			m_pMemoryHeapMain->AllocateHeap(m_nMaxByteMain + m_nMaxByteAdditional, sUsage);
			m_nMaxByteTotal += m_nMaxByteMain; // Add in case there's extended memory allocated.
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::Defrag()
{
	if (m_pMemoryHeapMain)
	{
		m_pMemoryHeapMain->Defrag();
	}

	if (m_pMemoryHeapExtended)
	{
		m_pMemoryHeapExtended->Defrag();
	}
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::ParsePreloadsXML(char const* const sLevelName)
{
	// Parse the xml file.
	char const* const sFileName = g_SoundCVars.g_sAudioPreloadsFile->GetString();

	if (sFileName && sFileName[0])
	{
		stack_string sPathAndName;
		sPathAndName.Format("%s%s.xml", AUDIO_PRELOADS_PATH, sFileName);

		XmlNodeRef const pNodeRoot(gEnv->pSystem->GetXmlUtils()->LoadXmlFromFile(sPathAndName.c_str()));

		if (pNodeRoot)
		{
			CryAutoCriticalSection oCS(m_oKnownFilesCS);

			char const* sPlatform = NULL;

#if defined(WIN32) || defined(WIN64)
			sPlatform = "pc";







#endif

			// The known-files-map must be empty if no level is passed.
			// It means that all Global and GameHint entries get reparsed therefore we need a clean state.
			if (sLevelName == NULL)
			{
				assert(m_mapKnownFiles.empty());
			}

			// Then load the globals for this platform and find the right platform node.
			stack_string sName;
			size_t const nNodeCount = (size_t)pNodeRoot->getChildCount();

			for (size_t i = 0; i < nNodeCount; ++i)
			{
				XmlNodeRef const pNode(pNodeRoot->getChild(i));

				if (pNode)
				{
					if (_stricmp(pNode->getTag(), "Platform") == 0)
					{
						bool bLoad = false;
						pNode->getAttr(sPlatform, bLoad);

						if (bLoad)
						{
							size_t const nChildNodeCount = (size_t)pNode->getChildCount();

							for (size_t j = 0; j < nChildNodeCount; ++j)
							{
								XmlNodeRef const pNodeChild(pNode->getChild(j));

								if (pNodeChild)
								{
									if (sLevelName && sLevelName[0] && _stricmp(pNodeChild->getTag(), "Level") == 0)
									{
										// Find the passed level.
										sName = pNodeChild->getAttr("name");

										if (_stricmp(sName.c_str(), sLevelName) == 0)
										{
											// Now cache the level specifics.
											size_t const nFileCount = (size_t)pNodeChild->getChildCount();

											for (size_t k = 0; k < nFileCount; ++k)
											{
												XmlNodeRef const pNodeFile(pNodeChild->getChild(k));

												if (pNodeFile && _stricmp(pNodeFile->getTag(), "File") == 0)
												{
													// Check if this file is already cached.
													char const* const sFullPath = pNodeFile->getAttr("name");
													CacheFile(sFullPath, sFullPath, eAFCT_LEVEL_SPECIFIC);
												}
											}
										}
									}
									else if (_stricmp(pNodeChild->getTag(), "Global") == 0)
									{
										// Cache the globals.
										size_t const nFileCount = (size_t)pNodeChild->getChildCount();

										for (size_t k = 0; k < nFileCount; ++k)
										{
											XmlNodeRef const pNodeFile(pNodeChild->getChild(k));

											if (pNodeFile && _stricmp(pNodeFile->getTag(), "File") == 0)
											{
												// Check if this file is already cached.
												char const* const sFullPath = pNodeFile->getAttr("name");
												CacheFile(sFullPath, sFullPath, eAFCT_GLOBAL);
											}
										}
									}
								}
							}
						}
					}
					else if (sLevelName == NULL && _stricmp(pNode->getTag(), "GameHints") == 0)
					{
						// Note: Loading level (first time load or changing between different preload xmls).
						ParseForGameHintsInternal(pNode);
					}
					else if (_stricmp(pNode->getTag(), "ExtendedMemory") == 0)
					{
						// Check if the user wants more memory!
						char const* const sExtendedMemory = pNode->getAttr(sPlatform);
						size_t const nExtendedSize = static_cast<size_t>(atoi(sExtendedMemory)) << 10;

						// We go ahead only if there's been a change in size.
						if (nExtendedSize != m_nMaxByteExtended)
						{
							if (m_pMemoryHeapExtended != NULL)
							{
								// First make sure to deallocate all blocks, that should have been done by the user already!
								m_pMemoryHeapExtended = NULL;
								m_nMaxByteTotal -= m_nMaxByteExtended;
							}

							if (nExtendedSize > 0)
							{
								m_pMemoryHeapExtended = static_cast<CCustomMemoryHeap*>(gEnv->pSystem->GetIMemoryManager()->CreateCustomMemoryHeapInstance(true));
								assert(m_pMemoryHeapExtended != NULL);

								m_pMemoryHeapExtended->AllocateHeap(nExtendedSize + m_nMaxByteAdditional, "AudioFileCacheManager extended heap");
								m_nMaxByteTotal += nExtendedSize;
							}

							// Store the new extended size.
							m_nMaxByteExtended = nExtendedSize;
						}
					}
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
bool CFileCacheManager::CreateEventSlot(CCryNameCRC const& rNameCRC, size_t const nEventOpenID)
{
	CryAutoCriticalSection oCS(m_oKnownFilesCS);

	bool bSuccess = false;
	tMapFileCacheEntries::iterator const Iter(m_mapKnownFiles.find(rNameCRC));

	if (Iter != m_mapKnownFiles.end())
	{
		Iter->second.m_mapReadStreams.insert(std::make_pair(nEventOpenID, NULL));
		bSuccess = true;
	}

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////////
bool CFileCacheManager::RemoveEventSlot(void* const pHandle, size_t const nEventOpenID)
{
	CryAutoCriticalSection oCS(m_oKnownFilesCS);

	bool bSuccess = false;
	CCryNameCRC const& rNameCRC(reinterpret_cast<uint32>(pHandle));
	tMapFileCacheEntries::iterator const Iter(m_mapKnownFiles.find(rNameCRC));

	if (Iter != m_mapKnownFiles.end())
	{
		bSuccess = Iter->second.AbortReadStreams(nEventOpenID);
	}

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////////
bool CFileCacheManager::CreateFileCacheEntry(char const* const sName, EAudioFileCacheType const eAudioFileCacheType)
{
	bool bSuccess = false;

	if (sName && sName[0])
	{
		CryAutoCriticalSection oCS(m_oKnownFilesCS);

		bSuccess = true;
		CCryNameCRC const oNameCRC(sName);
		tMapFileCacheEntries::iterator const Iter(m_mapKnownFiles.find(oNameCRC));

		if (Iter == m_mapKnownFiles.end())
		{
			// File entry does not yet exist.
			CryFixedStringT<AFCM_FILE_NAME_LENGTH> sNameLowerCase(sName);
			sNameLowerCase.MakeLower();

			// We also create entries for files that don't seem to exist!
			// This is communicated to the users so they can straighten things out.
			size_t const nFileSize = gEnv->pCryPak->FGetSize(sNameLowerCase.c_str());
			bSuccess = (nFileSize > 0);

			// A resize of this container will call Reset() on all entries.
			// Make sure to not abort in-flight streams and reads during resize!
			SFileCacheEntry& rFileCacheEntry       = m_mapKnownFiles.insert(std::make_pair(oNameCRC, SFileCacheEntry())).first->second;
			rFileCacheEntry.m_sFileName            = sNameLowerCase;
			rFileCacheEntry.m_nSizeFile            = nFileSize;
			rFileCacheEntry.m_eAudioFileCacheType  = eAudioFileCacheType;
			rFileCacheEntry.m_eAudioFileCacheState = bSuccess ? eAFCS_NOTCACHED : eAFCS_NOTFOUND;

			switch (eAudioFileCacheType)
			{
			case eAFCT_FSB_HEADER:
			case eAFCT_MUSIC:
				{
					rFileCacheEntry.m_eStreamTaskType = eStreamTaskTypeMusic;

					break;
				}
			case eAFCT_NOTCACHED:
			case eAFCT_GLOBAL:
			case eAFCT_LEVEL_SPECIFIC:
			case eAFCT_GAME_HINT:
				{
					// For these types we first assume eStreamTaskTypeSound.
					// It will be adjusted once the file gets cached.
					rFileCacheEntry.m_eStreamTaskType = eStreamTaskTypeSound;

					break;
				}
			}
		}
		else
		{
			SFileCacheEntry& rFileCacheEntry = Iter->second;

			if (rFileCacheEntry.m_nSizeFile == 0)
			{
				// Try to re-get the size in case it failed earlier.
				rFileCacheEntry.m_nSizeFile = gEnv->pCryPak->FGetSize(rFileCacheEntry.m_sFileName.c_str());
				
				if (rFileCacheEntry.m_nSizeFile > 0)
				{
					// Looks like this file was found after all, update its state flags.
					rFileCacheEntry.m_eAudioFileCacheState = eAFCS_NOTCACHED;
				}
				else
				{
					// File was still not found.
					bSuccess = false;
				}
			}

			// Only allow GameHints and uncached type entries to be changed.
			if (rFileCacheEntry.m_eAudioFileCacheType != eAudioFileCacheType &&
				 (rFileCacheEntry.m_eAudioFileCacheType == eAFCT_GAME_HINT ||
				  rFileCacheEntry.m_eAudioFileCacheType == eAFCT_NOTCACHED))
			{
				gEnv->pSoundSystem->Log(eSLT_Always, "<Sound> AFCM: changed type on (%s) from %d to %d!", rFileCacheEntry.m_sFileName.c_str(), static_cast<int>(rFileCacheEntry.m_eAudioFileCacheType), static_cast<int>(eAudioFileCacheType));

				// This kind of allows for "upgrading" entries to become either global or level specific entries.
				// These files will then be non-uncachable during game play.
				rFileCacheEntry.m_eAudioFileCacheType = eAudioFileCacheType;
			}
		}
	}

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::CacheFile(char const* const sName, CCryNameCRC const& rNameCRC, EAudioFileCacheType const eAudioFileCacheType, size_t const nRequestFileOffset /* = 0 */, int const nRequestBytes /* = -1 */)
{
	switch (eAudioFileCacheType)
	{
	case eAFCT_GAME_HINT:
		{
			CacheGameHintInternal(rNameCRC);

			break;
		}
	case eAFCT_GLOBAL:
	case eAFCT_LEVEL_SPECIFIC:
	case eAFCT_MUSIC:
	case eAFCT_FSB_HEADER:
		{
			if (CreateFileCacheEntry(sName, eAudioFileCacheType))
			{
				CacheFileInternal(rNameCRC, nRequestFileOffset, nRequestBytes);
			}

			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::CacheFileInternal(CCryNameCRC const& rNameCRC, size_t const nRequestFileOffset /* = 0 */, int const nRequestBytes /* = -1 */, bool const bRetriedCacheRequest /* = false */)
{
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SOUND);
	
	if (m_oKnownFilesCS.TryLock())
	{
		tMapFileCacheEntries::iterator Iter(m_mapKnownFiles.find(rNameCRC));

		// File must exist and mustn't be cached already.
		if (Iter != m_mapKnownFiles.end())
		{
			SFileCacheEntry& rFileCacheEntry = Iter->second;

#ifdef INCLUDE_AUDIO_PRODUCTION_CODE
			if (g_SoundCVars.g_nFileCacheManagerDebugLog >= 1)
			{
				CryLogAlways("<Sound> AFCM: CacheFileInternal '%s', %d, %d, %d", rFileCacheEntry.m_sFileName.c_str(), rFileCacheEntry.m_eAudioFileCacheType, nRequestFileOffset, nRequestBytes);
			}
#endif // INCLUDE_AUDIO_PRODUCTION_CODE

			MEMSTAT_CONTEXT(EMemStatContextTypes::MSC_Other, EMemStatContextFlags::MSF_None, "CacheFileInternal");
			MEMSTAT_CONTEXT_FMT(EMemStatContextTypes::MSC_Other, EMemStatContextFlags::MSF_None, "CacheFileInternal %s", rFileCacheEntry.m_sFileName.c_str());

			if ((rFileCacheEntry.m_eAudioFileCacheState & (eAFCS_CACHED|eAFCS_LOADING|eAFCS_VIRTUAL)) == 0)
			{
				// Assume to cache the entire file.
				rFileCacheEntry.m_nSizeCached = rFileCacheEntry.m_nSizeFile;

				if (nRequestBytes > 0 || nRequestFileOffset > 0)
				{
					rFileCacheEntry.m_nFileOffset = nRequestFileOffset;
					rFileCacheEntry.m_nSizeCached = nRequestBytes; // It's just a chunk of the file.

					if (rFileCacheEntry.m_nFileOffset + rFileCacheEntry.m_nSizeCached > rFileCacheEntry.m_nSizeFile)
					{
						rFileCacheEntry.m_nSizeCached = rFileCacheEntry.m_nSizeFile - rFileCacheEntry.m_nFileOffset;
					}
				}

				// Only try to allocate if it fits into our budget.
				if (DoesRequestFitInternal(rFileCacheEntry.m_nSizeCached) && AllocateMemoryBlockInternal(rFileCacheEntry))
				{
					{
						// Remove a potentially queued up cache request.
						CryAutoCriticalSection const oCS(m_oQueuedCacheRequestsCS);

						tMapQueuedCacheRequests::iterator const IterQueued(m_mapQueuedCacheRequests.find(rNameCRC));

						if (IterQueued != m_mapQueuedCacheRequests.end())
						{
							m_mapQueuedCacheRequests.erase(IterQueued);
						}
					}

					rFileCacheEntry.m_eAudioFileCacheState = m_bCanCacheEntries ? eAFCS_LOADING : eAFCS_VIRTUAL;

					if (m_bCanCacheEntries)
					{
						// Now start the asynchronous read stream to copy the data into the memory block.
						StreamReadParams oStreamReadParams;
						oStreamReadParams.nOffset      = rFileCacheEntry.m_nFileOffset;
						oStreamReadParams.nFlags       = IStreamEngine::FLAGS_NO_SYNC_CALLBACK;



						oStreamReadParams.dwUserData   = static_cast<DWORD_PTR>(rNameCRC.get());

						oStreamReadParams.nLoadTime    = 0;
						oStreamReadParams.nMaxLoadTime = 0;
						oStreamReadParams.ePriority    = estpUrgent;
						oStreamReadParams.pBuffer      = rFileCacheEntry.m_pMemoryBlock->GetData();
						oStreamReadParams.nSize        = static_cast<int unsigned>(rFileCacheEntry.m_nSizeCached);

						rFileCacheEntry.m_pReadStream = gEnv->pSystem->GetStreamEngine()->StartRead(eStreamTaskTypeFSBCache, rFileCacheEntry.m_sFileName.c_str(), this, &oStreamReadParams);
					}
					else
					{
						// In case we cannot cache this entry yet make sure to free its allocated memory again.
						rFileCacheEntry.m_pMemoryBlock = NULL;
					}

					// Always add to the total size.
					m_nCurrentByteTotal += rFileCacheEntry.m_nSizeCached;
				}
				else
				{
					if (!m_aoQueuedUnloadEntries.empty() && (rFileCacheEntry.m_eAudioFileCacheState & eAFCS_QUEUED_FOR_CACHING) == 0)
					{
						// If we couldn't allocate memory and there are still unloads in flight and the entry hasn't been queued for caching already, go ahead and queue it up.
						CryAutoCriticalSection const oCS(m_oQueuedCacheRequestsCS);

						rFileCacheEntry.m_eAudioFileCacheState = eAFCS_QUEUED_FOR_CACHING;
						m_mapQueuedCacheRequests.insert(std::make_pair(rNameCRC, SQueuedCacheRequest(nRequestFileOffset, nRequestBytes))); // Inserts uniquely.

						// Let the users know that we queued their cache request.
						gEnv->pSoundSystem->Log(eSLT_Warning, "<Sound> AFCM: Could not cache %s but put it into the caching queue!", rFileCacheEntry.m_sFileName.c_str());
					}
					else if (m_aoQueuedUnloadEntries.empty())
					{
						// Note: This entry might not have eAFCS_QUEUED_FOR_CACHING set in case it got queued up because m_oKnownFilesCS couldn't get locked.
						CryAutoCriticalSection const oCS(m_oQueuedCacheRequestsCS);

						tMapQueuedCacheRequests::iterator const IterQueued(m_mapQueuedCacheRequests.find(rNameCRC));

						if (IterQueued != m_mapQueuedCacheRequests.end())
						{
							// If there are no unloads in flight anymore and the entry is still queued for caching make sure to remove it from the list.
							m_mapQueuedCacheRequests.erase(IterQueued);
						}

						// Cannot have a valid memory block!
						assert(rFileCacheEntry.m_pMemoryBlock == NULL);

						// This unfortunately is a total memory allocation fail.
						rFileCacheEntry.m_eAudioFileCacheState = eAFCS_MEMALLOCFAIL;

						// The user should be made aware of it.
						gEnv->pSoundSystem->Log(eSLT_Warning, "<Sound> AFCM: Could not cache %s as we are out of memory!", rFileCacheEntry.m_sFileName.c_str());

            // Finally, in case this entry got reads queued up make sure to finish those.
            FinishQueuedReads(rFileCacheEntry);
					}

					// In any case reset the cached size member as caching didn't succeed.
					rFileCacheEntry.m_nSizeCached = 0;
				}
			}

			// In case this entry is queued up for unload make sure to remove it from the list.
			if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_QUEUED_FOR_UNLOAD) != 0)
			{
				bool const bFoundAndErased = stl::find_and_erase(m_aoQueuedUnloadEntries, rNameCRC);
				assert(bFoundAndErased);
			}

			// Increment the used count on GameHints.
			if (rFileCacheEntry.m_eAudioFileCacheType == eAFCT_GAME_HINT && (rFileCacheEntry.m_eAudioFileCacheState & (eAFCS_CACHED|eAFCS_LOADING|eAFCS_VIRTUAL|eAFCS_QUEUED_FOR_CACHING)) != 0)
			{
				if (!bRetriedCacheRequest)
				{
					++rFileCacheEntry.m_nUsedCount;
				}

				// Make sure to handle the eAFCS_REMOVABLE flag according to the m_nUsedCount count.
				if (rFileCacheEntry.m_nUsedCount != 0)
				{
					rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState & ~eAFCS_REMOVABLE);
				}
				else
				{
					rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState | eAFCS_REMOVABLE);
				}
			}

			// And finally reset some flags.
			rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState & ~(eAFCS_QUEUED_FOR_UNLOAD|eAFCS_UNCACHE_AFTER_UNLOAD|eAFCS_UNLOAD_WHEN_SOUND_READY|eAFCS_UNCACHE_WHEN_SOUND_READY));
		}

		m_oKnownFilesCS.Unlock();
	}
	else
	{
		// Queue this cache request and try next update cycle to get a lock.
		CryAutoCriticalSection const oCS(m_oQueuedCacheRequestsCS);
		m_mapQueuedCacheRequests.insert(std::make_pair(rNameCRC, SQueuedCacheRequest(nRequestFileOffset, nRequestBytes))); // Adds uniquely.
	}
}

//////////////////////////////////////////////////////////////////////////
bool CFileCacheManager::RemoveEntryOrHint(CCryNameCRC const& rNameCRC, bool const bNow)
{
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SOUND);

	tMapAllGameHints::const_iterator const Iter(m_mapAllFoundGameHints.find(rNameCRC));

	if (Iter != m_mapAllFoundGameHints.end())
	{
		// It's a GameHint.
		tVecFileCacheEntryCRCs::const_iterator IterFiles(Iter->second.begin());
		tVecFileCacheEntryCRCs::const_iterator const IterFilesEnd(Iter->second.end());

		for (; IterFiles != IterFilesEnd; ++IterFiles)
		{
			UncacheFileCacheEntryByCRCInternal(*IterFiles, bNow);
		}

		return true;
	}

	// Maybe it's a single file?
	return UncacheFileCacheEntryByCRCInternal(rNameCRC, bNow);
}

//////////////////////////////////////////////////////////////////////////
bool CFileCacheManager::UncacheFileCacheEntryByCRCInternal(CCryNameCRC const& rNameCRC, bool const bNow, bool const bIgnoreUsedCount /* = false */)
{
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SOUND);

	CryAutoCriticalSection oCS(m_oKnownFilesCS);

	bool bSuccess = false;

	tMapFileCacheEntries::iterator const Iter(m_mapKnownFiles.find(rNameCRC));

	if (Iter != m_mapKnownFiles.end())
	{
		bSuccess = UncacheFileCacheEntryInternal(Iter->second, bNow, bIgnoreUsedCount);
	}

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////////
bool CFileCacheManager::UncacheFileCacheEntryInternal(SFileCacheEntry& rFileCacheEntry, bool const bNow, bool const bIgnoreUsedCount /* = false */)
{
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SOUND);

	bool bSuccess = false;

	// In any case decrement the used count.
	if (rFileCacheEntry.m_nUsedCount > 0)
	{
		--rFileCacheEntry.m_nUsedCount;
	}

	if (rFileCacheEntry.m_nUsedCount < 1 || bIgnoreUsedCount)
	{
		// The file is always removable in this case.
		if (rFileCacheEntry.m_eAudioFileCacheType != eAFCT_GLOBAL && rFileCacheEntry.m_eAudioFileCacheType != eAFCT_LEVEL_SPECIFIC)
		{
			rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState | eAFCS_REMOVABLE);
		}

		// Must be cached to proceed.
		if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_CACHED) != 0)
		{
			if (bNow || bIgnoreUsedCount)
			{
				// An FMOD preloaded file must be queued up for unload.
				if (!QueueFileCacheEntryForUnloadInternal(rFileCacheEntry, true))
				{
					// Only uncache right away if this file is not preloaded.
          CryAutoCriticalSection oCS(m_oFileReadCS);
					rFileCacheEntry.Uncache();
				}
			}
		}
		else if ((rFileCacheEntry.m_eAudioFileCacheState & (eAFCS_LOADING|eAFCS_MEMALLOCFAIL)) != 0)
		{
			// Reset the entry in case it's still loading or was a memory allocation fail.
      CryAutoCriticalSection oCS(m_oFileReadCS);
			rFileCacheEntry.Uncache();
		}

		// The file was properly uncached or queued for uncache.
		bSuccess = true;
	}

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::ParseForGameHintsInternal(XmlNodeRef const pNodeGameHints)
{
	assert(pNodeGameHints != NULL);
	assert(_stricmp(pNodeGameHints->getTag(), "GameHints")== 0);

	stl::free_container(m_mapAllFoundGameHints);
	size_t const nNumGameHints = static_cast<size_t>(pNodeGameHints->getChildCount());
	m_mapAllFoundGameHints.reserve(nNumGameHints);
	size_t nNumGameHintChildren = 0;
	CryFixedStringT<64> sGameHintName;
	XmlNodeRef pNodeGameHint;
	XmlNodeRef pNodeGameHintFile;
	char const* sFullPath = NULL;

	for (size_t i = 0; i < nNumGameHints; ++i)
	{
		pNodeGameHint = pNodeGameHints->getChild(i);
		sGameHintName = pNodeGameHint->getTag();
		assert(!sGameHintName.empty());

		nNumGameHintChildren = static_cast<size_t>(pNodeGameHint->getChildCount());
		tVecFileCacheEntryCRCs aoFilePathCRCs;
		aoFilePathCRCs.reserve(nNumGameHintChildren);

		for (size_t j = 0; j < nNumGameHintChildren; ++j)
		{
			pNodeGameHintFile = pNodeGameHint->getChild(j);

			if (pNodeGameHintFile && _stricmp(pNodeGameHintFile->getTag(), "File") == 0)
			{
				sFullPath = pNodeGameHintFile->getAttr("name");

				if (sFullPath && sFullPath[0])
				{
					aoFilePathCRCs.push_back(sFullPath);

					// Create the entry.
					CreateFileCacheEntry(sFullPath, eAFCT_GAME_HINT);
				}
			}
		}

		m_mapAllFoundGameHints.insert(std::make_pair(sGameHintName.c_str(), aoFilePathCRCs));
	}
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::CacheGameHintInternal(CCryNameCRC const& rNameCRC)
{
	tMapAllGameHints::const_iterator const IterGameHints(m_mapAllFoundGameHints.find(rNameCRC));

	if (IterGameHints != m_mapAllFoundGameHints.end())
	{
		CryAutoCriticalSection oCS(m_oKnownFilesCS);

		// This GameHint is known, now go ahead and cache its associated files.
		tVecFileCacheEntryCRCs::const_iterator const IterFilesEnd(IterGameHints->second.end());

		for (tVecFileCacheEntryCRCs::const_iterator IterFiles(IterGameHints->second.begin()); IterFiles != IterFilesEnd; ++IterFiles)
		{
			tMapFileCacheEntries::iterator const Iter(m_mapKnownFiles.find(*IterFiles));

			if (Iter != m_mapKnownFiles.end())
			{
				SFileCacheEntry& rFileCacheEntry = Iter->second;

				// Cache only files that exist.
				if (rFileCacheEntry.m_nSizeFile > 0)
				{
					CacheFileInternal(*IterFiles);
				}
			}
			else
			{
				// Investigate why this entry doesn't exist!
				assert(0);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::Clear(EAudioFileCacheType const eAudioFileCacheType /* = eAFCT_ALL */)
{
	CryAutoCriticalSection const oKnownFilesCS(m_oKnownFilesCS);
	
	{
		// First clean up queued requests.
		CryAutoCriticalSection const oQueuedCacheRequestsCS(m_oQueuedCacheRequestsCS);

		if (!m_mapQueuedCacheRequests.empty())
		{
			// Reset the queued cache request entries.
			tMapQueuedCacheRequests const mapQueuedCacheRequests(m_mapQueuedCacheRequests);
			tMapQueuedCacheRequests::const_iterator const IterQueuedCacheRequestsEnd(mapQueuedCacheRequests.end());

			for (tMapQueuedCacheRequests::const_iterator IterQueuedCacheRequests(mapQueuedCacheRequests.begin()); IterQueuedCacheRequests != IterQueuedCacheRequestsEnd; ++IterQueuedCacheRequests)
			{
				tMapFileCacheEntries::iterator IterKnownFile(m_mapKnownFiles.find(IterQueuedCacheRequests->first));

				if (IterKnownFile != m_mapKnownFiles.end())
				{
					SFileCacheEntry& rFileCacheEntry = IterKnownFile->second;

					if (rFileCacheEntry.m_eAudioFileCacheType == eAudioFileCacheType || eAudioFileCacheType == eAFCT_ALL)
					{
						// Even though the file isn't chached Uncache() will reset it to a default state.
						rFileCacheEntry.Uncache();
						m_mapQueuedCacheRequests.erase(m_mapQueuedCacheRequests.find(IterQueuedCacheRequests->first));
					}
				}
			}
		}
	}

	{
		CryAutoCriticalSection const oQueuedStreamsCS(m_oQueuedStreamsCS);

		tMapFileCacheEntries::const_iterator IterEnd(m_mapKnownFiles.end());

		for (tMapFileCacheEntries::iterator Iter(m_mapKnownFiles.begin()); Iter != IterEnd; ++Iter)
		{
			SFileCacheEntry& rFileCacheEntry = Iter->second;

			// Clear out everything on type "eAFCT_ALL", all GameHints on "eAFCT_GAME_HINT_ALL" or the actual passed type.
			if (eAudioFileCacheType == eAFCT_ALL || rFileCacheEntry.m_eAudioFileCacheType == eAudioFileCacheType)
			{
				// In case this entry has a caching read queued up remove it from the queue.
				tMapQueuedStreams::iterator const IterQueuedStream(m_mapQueuedStreams.find(rFileCacheEntry.m_pReadStream));

				if (IterQueuedStream != m_mapQueuedStreams.end())
				{
					m_mapQueuedStreams.erase(IterQueuedStream);
				}

				// This will be a forceful removal regardless of "m_nUsedCount".
				UncacheFileCacheEntryInternal(rFileCacheEntry, true, true);
			}
		}

		UpdatePatchRequestsInternal();

		if (eAudioFileCacheType == eAFCT_ALL)
		{
			// Clear the known files map except for the music files.
			m_mapKnownFiles.erase_if(IsNotMusicFile);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
bool CFileCacheManager::IsFileCacheEntryCached(CCryNameCRC const& rNameCRC)
{
	CryAutoCriticalSection oCS(m_oKnownFilesCS);

	tMapFileCacheEntries::const_iterator const Iter(m_mapKnownFiles.find(rNameCRC));

	if (Iter != m_mapKnownFiles.end())
	{
		SFileCacheEntry const& rFileCacheEntry = Iter->second;

		return (rFileCacheEntry.m_eAudioFileCacheState & (eAFCS_CACHED|eAFCS_LOADING|eAFCS_QUEUED_FOR_CACHING|eAFCS_VIRTUAL)) != 0;
	}

	// File must be known!
	assert(0);
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool CFileCacheManager::GetFileCacheEntryState(CCryNameCRC const& rNameCRC, EAudioFileCacheState& eAudioFileCacheState)
{
	CryAutoCriticalSection oCS(m_oKnownFilesCS);

	tMapFileCacheEntries::const_iterator const Iter(m_mapKnownFiles.find(rNameCRC));

	if (Iter != m_mapKnownFiles.end())
	{
		eAudioFileCacheState = Iter->second.m_eAudioFileCacheState;

		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
bool CFileCacheManager::FindFileCacheEntry(CCryNameCRC const& rNameCRC, void** const ppHandle, int unsigned* const pFileSize)
{
	CryAutoCriticalSection oCS(m_oKnownFilesCS);

	tMapFileCacheEntries::const_iterator const Iter(m_mapKnownFiles.find(rNameCRC));

	if (Iter != m_mapKnownFiles.end())
	{
		if (ppHandle)
		{
			*ppHandle = reinterpret_cast<void*>(Iter->first.get());
		}

		if (pFileSize)
		{
			*pFileSize = static_cast<int unsigned>(Iter->second.m_nSizeFile);
		}

		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::ReadFileCacheEntry(void* const pReadInfo, size_t const nEventOpenID)
{
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SOUND);

	FMOD_ASYNCREADINFO* const pAsyncReadInfo = static_cast<FMOD_ASYNCREADINFO* const>(pReadInfo);
	CCryNameCRC const& rNameCRC(reinterpret_cast<uint32>(pAsyncReadInfo->handle));
	AFCM_RESULT eResult     = AFCM_FILE_NOT_FOUND;
	size_t nBytesToRead     = 0;
	size_t nOffset          = static_cast<size_t>(pAsyncReadInfo->offset);
	size_t const nSizeBytes = static_cast<size_t>(pAsyncReadInfo->sizebytes);
	
	_smart_ptr<ICustomMemoryBlock> pMemoryBlock = NULL;
	CryStackStringT<char,512> sFileName = "";

	{
		CryAutoCriticalSection oCS(m_oKnownFilesCS);
		SFileCacheEntry* pFileCacheEntry = NULL;

		tMapFileCacheEntries::iterator const Iter(m_mapKnownFiles.find(rNameCRC));

		if (Iter != m_mapKnownFiles.end())
		{
			pFileCacheEntry = &Iter->second;
			SFileCacheEntry& rFileCacheEntry = Iter->second;

			// collect information to be used outside the critical section in stack variables
			pMemoryBlock = pFileCacheEntry->m_pMemoryBlock;
			sFileName = pFileCacheEntry->m_sFileName.c_str();

      // Always remove queued reads here, if the entry is still queued in some form the read will get added again.
      if (!rFileCacheEntry.m_mapQueuedReads.empty())
      {
        rFileCacheEntry.RemoveQueuedRead(nEventOpenID, pReadInfo);
      }

			// We mustn't read from entries that are preloaded or about to get preloaded!
			assert((rFileCacheEntry.m_eAudioFileCacheState & (eAFCS_PRELOADED|eAFCS_QUEUED_FOR_PRELOAD)) == 0);
			bool bReadData = (rFileCacheEntry.m_eAudioFileCacheState & eAFCS_CACHED) != 0;			

			// Make sure this entry has been fully delivered by the streaming engine!
			if (bReadData)
			{
				// Is this a partially cached file?
				if (rFileCacheEntry.m_nSizeCached > 0 && rFileCacheEntry.m_nSizeFile != rFileCacheEntry.m_nSizeCached)
				{
					if (nOffset >= rFileCacheEntry.m_nFileOffset && ((nOffset + nSizeBytes) <= (rFileCacheEntry.m_nFileOffset + rFileCacheEntry.m_nSizeCached)))
					{
						nOffset -= rFileCacheEntry.m_nFileOffset;
					}
					else
					{
						// We are not reading within the cached region.
						bReadData = false;
						eResult   = AFCM_FILE_OUTSIDE_REGION;
					}
				}

				if (bReadData)
				{
					// Now copy the requested memory.
					if (nOffset < rFileCacheEntry.m_nSizeCached)
					{
						nBytesToRead = min(rFileCacheEntry.m_nSizeCached - nOffset, nSizeBytes);
					}
					else
					{
						// File is available, but the request is for data beyond the end of the file.
						// This mustn't happen!
						assert(0);
						eResult = AFCM_ERROR;

						gEnv->pSoundSystem->Log(eSLT_Error, "<Sound> AFCM: AFCM_ERROR beyond data (%s)", sFileName.c_str());
					}
				}
			}
			else if ((rFileCacheEntry.m_eAudioFileCacheState & (eAFCS_NOTCACHED|eAFCS_MEMALLOCFAIL)) != 0)
			{
				// Forward to the streaming engine.
				eResult = AFCM_FILE_NOT_CACHED;
			}
			else if ((rFileCacheEntry.m_eAudioFileCacheState & (eAFCS_QUEUED_FOR_CACHING|eAFCS_LOADING)) != 0)
			{
				// In case a soundbank is still loading in queue the read request.
				rFileCacheEntry.AddQueuedRead(nEventOpenID, pReadInfo);
				eResult = AFCM_FILE_NOT_READY;
				gEnv->pSoundSystem->Log(eSLT_Message, "<Sound> AFCM: added queued read for (%s) (count: %d)", sFileName.c_str(), static_cast<int>(rFileCacheEntry.m_mapQueuedReads.size()));
			}
			else if ((rFileCacheEntry.m_eAudioFileCacheState & (eAFCS_PRELOADED|eAFCS_QUEUED_FOR_PRELOAD)) != 0)
			{
				// If this file is preloaded or about to get preloaded we need to fail this read request.
				assert(0);
				eResult = AFCM_ERROR;
				gEnv->pSoundSystem->Log(eSLT_Error, "<Sound> AFCM: aborted async read due to soundbank being preloaded or getting preloaded! (%s)", sFileName.c_str());
			}
			else if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_VIRTUAL) != 0)
			{
				// Read request to a virtual file, that's totally fine just make sure to fail the request below.
				eResult = AFCM_FILE_FLAGGED_VIRTUAL;
			}
			else
			{
				// Mustn't come here!
				assert(0);
			}

#if defined(SOUNDSYSTEM_USE_FMODEX400)
			// Do this inside the scope of the lock.
			if (eResult == AFCM_FILE_NOT_CACHED || eResult == AFCM_FILE_OUTSIDE_REGION)
			{
#ifdef INCLUDE_AUDIO_PRODUCTION_CODE
				++rFileCacheEntry.m_nRequestedCount;
#endif // INCLUDE_AUDIO_PRODUCTION_CODE

				// This read request is either outside of the file's cached region or not cached at all.
				// In this case we need to forward this request to the streaming engine.
				// Note: This stream request is currently finished in AudioDeviceFmodEx400.cpp by CFMODFileStreamCallback::StreamAsyncOnComplete.
				if (g_SoundCVars.g_nAllowNotCachedAccess == 1 || rFileCacheEntry.m_eStreamTaskType != eStreamTaskTypeSound)
				{
					StreamReadParams StrParams;
					StrParams.nOffset          = pAsyncReadInfo->offset;
					StrParams.nFlags           = IStreamEngine::FLAGS_NO_SYNC_CALLBACK | IStreamEngine::FLAGS_IGNORE_TMP_OUT_OF_MEM;
					StrParams.dwUserData       = reinterpret_cast<DWORD_PTR>(pAsyncReadInfo);
					StrParams.nLoadTime        = 0;
					StrParams.nMaxLoadTime     = 0;
					StrParams.ePriority        = estpNormal;
					StrParams.pBuffer          = pAsyncReadInfo->buffer;
					StrParams.nSize            = min((rFileCacheEntry.m_nSizeFile - nOffset), nSizeBytes);
					IReadStreamPtr pReadStream = gEnv->pSystem->GetStreamEngine()->StartRead(rFileCacheEntry.m_eStreamTaskType, sFileName.c_str(), &s_FMODFileCallback, &StrParams);

					if (pReadStream) // Getting NULL here means we are in shutdown phase.
					{
						rFileCacheEntry.AddReadStream(nEventOpenID, pReadStream);
					}
					else
					{
						assert(0);
						pAsyncReadInfo->result = FMOD_ERR_FILE_BAD;
						gEnv->pSoundSystem->Log(eSLT_Error, "<Sound> AFCM: FMOD_ERR_FILE_BAD no pReadStream (%s)", sFileName.c_str());

					}
				}
				else
				{
					gEnv->pSoundSystem->Log(eSLT_Error, "<Sound> AFCM: FMOD_ERR_FILE_BAD no g_nAllowNotCachedAccess (%s)", sFileName.c_str());
					pAsyncReadInfo->result = FMOD_ERR_FILE_BAD;
				}
			}
#endif
		}
		else
		{
			// Investigate why this entry doesn't exist!
			assert(0);
		}

		// modify pFileCacheEntry while we are in it's critical section
#ifdef INCLUDE_AUDIO_PRODUCTION_CODE
		if(nBytesToRead > 0 && pMemoryBlock)
			++pFileCacheEntry->m_nRequestedCount;
#endif // INCLUDE_AUDIO_PRODUCTION_CODE
	} // end of critical section m_oKnownFilesCS

	if (nBytesToRead > 0)
	{
    // Copy outside the scope of the "m_oKnownFilesCS" lock to avoid contention with main thread.
    // However, make sure to still protect it with its unique lock.
    CryAutoCriticalSection oCS(m_oFileReadCS);

    if (pMemoryBlock)
    {
      pMemoryBlock->CopyMemoryRegion(pAsyncReadInfo->buffer, nOffset, nBytesToRead);
      pAsyncReadInfo->bytesread = static_cast<int unsigned>(nBytesToRead);

      pAsyncReadInfo->result = (nBytesToRead < nSizeBytes) ? FMOD_ERR_FILE_EOF : FMOD_OK;
    }
    else
    {
      // This file got uncached while trying to read from it.
      assert(0);
      pAsyncReadInfo->result = FMOD_ERR_FILE_BAD;
      gEnv->pSoundSystem->Log(eSLT_Error, "<Sound> AFCM: FMOD_ERR_FILE_BAD uncache during read (%s)", sFileName.c_str());
    }
	}
	else
	{
		switch (eResult)
		{
		case AFCM_FILE_NOT_READY:
			{
				pAsyncReadInfo->result = FMOD_ERR_NOTREADY;

				break;
			}
		case AFCM_FILE_FLAGGED_VIRTUAL:
			{
				pAsyncReadInfo->result = FMOD_ERR_FILE_BAD;

				break;
			}
		case AFCM_ERROR:
		case AFCM_FILE_NOT_FOUND:
			{
				assert(0);
				pAsyncReadInfo->result = FMOD_ERR_FILE_BAD;
				gEnv->pSoundSystem->Log(eSLT_Error, "<Sound> AFCM: FMOD_ERR_FILE_BAD error&file not found");

				break;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::RunQueuedRead(void* const pReadInfo, size_t const nEventOpenID)
{
	ReadFileCacheEntry(pReadInfo, nEventOpenID);
}

//////////////////////////////////////////////////////////////////////////
bool CFileCacheManager::PreloadEntryIntoFMODInternal(CCryNameCRC const& rNameCRC)
{
#ifndef PS3
#if defined(SOUNDSYSTEM_USE_FMODEX400)
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SOUND);

	bool bSuccess = false;

	if (m_oKnownFilesCS.TryLock())
	{
		tMapFileCacheEntries::iterator const Iter(m_mapKnownFiles.find(rNameCRC));

		if (Iter != m_mapKnownFiles.end())
		{
			SFileCacheEntry& rFileCacheEntry = Iter->second;

			if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_PRELOADED) == 0)
			{
				FMOD::System* const pSystem           = static_cast<FMOD::System* const>(gEnv->pSoundSystem->GetIAudioDevice()->GetSoundLibrary());
				FMOD::EventSystem* const pEventSystem = static_cast<FMOD::EventSystem* const>(gEnv->pSoundSystem->GetIAudioDevice()->GetEventSystem());

				/* We need to create a FMOD::Sound object to use with preloadFSB */
				FMOD_RESULT	eResult = ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_SOUND_NOT_READY) == 0) ? FMOD_OK : FMOD_ERR_NOTREADY;
				FMOD::Sound* pSound = NULL;

				if (eResult == FMOD_OK)
				{
					FMOD_CREATESOUNDEXINFO oInfo = {0};
					oInfo.cbsize                 = sizeof(FMOD_CREATESOUNDEXINFO);
					oInfo.length                 = static_cast<int unsigned>(rFileCacheEntry.m_nSizeCached);
					oInfo.suggestedsoundtype     = FMOD_SOUND_TYPE_FSB;
					FMOD_MODE const Modes        = FMOD_OPENMEMORY_POINT | FMOD_CREATECOMPRESSEDSAMPLE | FMOD_SOFTWARE | FMOD_NONBLOCKING; // We use FMOD_NONBLOCKING to prevent createSound from hanging where data is still being loaded asynchronously from this soundbank. (same thread is used)

					char const* const pData = static_cast<char const* const>(rFileCacheEntry.m_pMemoryBlock->GetData());
					eResult                 = pSystem->createSound(pData, Modes, &oInfo, &pSound);
				}
				else
				{
					// The sound instance is still loading, let's check for its state.
					assert(rFileCacheEntry.m_pSoundBufferData != NULL);

					if (rFileCacheEntry.m_pSoundBufferData)
					{
						pSound = static_cast<FMOD::Sound*>(rFileCacheEntry.m_pSoundBufferData);

						FMOD_OPENSTATE eOpenState;
						FMOD_RESULT const eTempResult = pSound->getOpenState(&eOpenState, NULL, NULL, NULL);
						assert(eTempResult == FMOD_OK);

						if (eOpenState != FMOD_OPENSTATE_LOADING)
						{
							// Proceed with the preloading below.
							eResult = FMOD_OK;

							// The sound is ready, remove the flag.
							rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState & ~eAFCS_SOUND_NOT_READY);

							if ((rFileCacheEntry.m_eAudioFileCacheState & (eAFCS_UNLOAD_WHEN_SOUND_READY|eAFCS_UNCACHE_WHEN_SOUND_READY)) != 0)
							{
								// In case this entry got told to unload or uncache while we were waiting for the sound instance to become ready,
								// make sure to not proceed with the preloading and uncache the file if asked for.
								eResult = pSound->release();
								assert(eResult == FMOD_OK);

								if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_UNCACHE_WHEN_SOUND_READY) != 0)
								{
									rFileCacheEntry.Uncache();
								}
								else
								{
									rFileCacheEntry.m_pSoundBufferData     = NULL;
									rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState & ~(eAFCS_QUEUED_FOR_PRELOAD|eAFCS_UNLOAD_WHEN_SOUND_READY|eAFCS_UNCACHE_WHEN_SOUND_READY));

									// Finish reads that might have gotten queued up in between.
									FinishQueuedReads(rFileCacheEntry);
								}

								// So this entry gets removed from m_aoQueuedPreloadEntries.
								bSuccess = true;

								// To skip the preloading below.
								eResult = FMOD_ERR_NOTREADY;
							}
						}
					}
				}

				if (eResult == FMOD_OK)
				{
#if defined(SOUNDSYSTEM_USE_FMODEX400)
					eResult = pEventSystem->preloadFSB(rFileCacheEntry.m_sFileName.c_str(), 0, pSound);

					switch (eResult)
					{
					case FMOD_OK:
						{
							rFileCacheEntry.m_pSoundBufferData     = static_cast<void*>(pSound);
							rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState & ~eAFCS_QUEUED_FOR_PRELOAD);
							rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState | eAFCS_PRELOADED);

							// Entry might have queued reads. Failing those here will kill the very first event but allow us to safely preload the soundbank.
							// To prevent this make sure to preload soundbanks well ahead before requesting data from them.
							if (!rFileCacheEntry.m_mapQueuedReads.empty())
							{
								assert(0);
								rFileCacheEntry.FailQueuedReads(0);

								gEnv->pSoundSystem->Log(eSLT_Error, "<Sound> AFCM: FMOD_ERR_FILE_BAD queued reads on preload (%s)", rFileCacheEntry.m_sFileName.c_str());
							}

							bSuccess = true;

							break;
						}
					case FMOD_ERR_EVENT_ALREADY_LOADED:
						{
							// This return code indicates that the soundbank to preload has already asynchronously loaded events sitting around.
							// Those need to get unloaded first before the preloading can be successful.
							eResult = pSound->release();
							assert(eResult == FMOD_OK);
							rFileCacheEntry.m_pSoundBufferData     = NULL;
							rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState & ~(eAFCS_QUEUED_FOR_PRELOAD|eAFCS_PRELOADED));

							// Finish reads that might have gotten queued up in between.
							FinishQueuedReads(rFileCacheEntry);

							gEnv->pSoundSystem->Log(eSLT_Warning, "<Sound> AFCM: preloadFSB failed due to FMOD_ERR_EVENT_ALREADY_LOADED! (%s)", rFileCacheEntry.m_sFileName.c_str());

							// Let's indicate success for now so the entry is removed from the queue.
							bSuccess = true;

							break;
						}
					case FMOD_ERR_NOTREADY:
						{
							// The sound instance isn't ready yet due to the FMOD_NONBLOCKING flag.
							rFileCacheEntry.m_pSoundBufferData     = static_cast<void*>(pSound);
							rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState | eAFCS_SOUND_NOT_READY);

							break;
						}
					default:
						{
							eResult = pSound->release();
							assert(eResult == FMOD_OK);
							rFileCacheEntry.m_pSoundBufferData     = NULL;
							rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState & ~(eAFCS_QUEUED_FOR_PRELOAD|eAFCS_PRELOADED));

							// Finish reads that might have gotten queued up in between.
							FinishQueuedReads(rFileCacheEntry);

							gEnv->pSoundSystem->Log(eSLT_Error, "<Sound> AFCM: failed to carry out pEventSystem->preloadFSB! (result: %d)", static_cast<int>(eResult));

							break;
						}
					}
#endif
				}
				else if (eResult != FMOD_ERR_NOTREADY)
				{
					rFileCacheEntry.m_pSoundBufferData     = NULL;
					rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState & ~(eAFCS_QUEUED_FOR_PRELOAD|eAFCS_PRELOADED));
					rFileCacheEntry.FailQueuedReads(0);

					gEnv->pSoundSystem->Log(eSLT_Error, "<Sound> AFCM: failed to call pSystem->createSound! (result: %d)", static_cast<int>(eResult));
				}
			}
			else if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_QUEUED_FOR_UNLOAD) != 0)
			{
				// This case needs investigation!
				assert(0);
			}
			else
			{
				// File is already preloaded.
				bSuccess = true;
			}
		}

		m_oKnownFilesCS.Unlock();
	}

	return bSuccess;
#endif
#endif // PS3
}

//////////////////////////////////////////////////////////////////////////
bool CFileCacheManager::UnloadEntryFromFMODInternal(CCryNameCRC const& rNameCRC)
{
#ifndef PS3
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SOUND);

	bool bSuccess = false;

	if (m_oKnownFilesCS.TryLock())
	{
		tMapFileCacheEntries::iterator const Iter(m_mapKnownFiles.find(rNameCRC));

		if (Iter != m_mapKnownFiles.end())
		{
			SFileCacheEntry& rFileCacheEntry = Iter->second;

			if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_PRELOADED) != 0)
			{
#if defined(SOUNDSYSTEM_USE_FMODEX400)
				FMOD::EventSystem* const pEventSystem = static_cast<FMOD::EventSystem* const>(gEnv->pSoundSystem->GetIAudioDevice()->GetEventSystem());
				FMOD_RESULT	eResult                   = pEventSystem->unloadFSB(rFileCacheEntry.m_sFileName.c_str(), 0);

				if (eResult == FMOD_OK)
				{
					FMOD::Sound* const pSound = static_cast<FMOD::Sound* const>(rFileCacheEntry.m_pSoundBufferData);
					eResult                   = pSound->release();
					bSuccess                  = (eResult == FMOD_OK);
					assert(bSuccess);
					rFileCacheEntry.m_pSoundBufferData     = NULL;
					rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState & ~(eAFCS_QUEUED_FOR_UNLOAD|eAFCS_PRELOADED));

					// In case this entry has bit eAFCS_VIRTUAL set make sure to also free its memory
					if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_VIRTUAL) != 0)
					{
						rFileCacheEntry.m_pMemoryBlock = NULL;
					}

					if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_UNCACHE_AFTER_UNLOAD) != 0)
					{
						// Entries that get "unpatch" called on them will be removed regardless of their "m_nUsedCount" count.
						UncacheFileCacheEntryInternal(rFileCacheEntry, true, true);
					}
				}
				else
				{
					gEnv->pSoundSystem->Log(eSLT_Error, "<Sound> AFCM: failed to carry out pEventSystem->unloadFSB  :-(");
				}
#endif
			}
			else if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_QUEUED_FOR_PRELOAD) != 0)
			{
				// This case needs investigation!
				assert(0);
			}
			else
			{
				// File is already unloaded.
				bSuccess = true;
			}
		}

		m_oKnownFilesCS.Unlock();
	}

	return bSuccess;
#endif // PS3
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::StreamAsyncOnComplete(IReadStream* pStream, unsigned int nError)
{
	// We "user abort" quite frequently so this is not something we want to assert on.
	assert(nError == 0 || nError == ERROR_USER_ABORT);

	// Mustn't get this callback when we're not allowed to cache.
	assert(m_bCanCacheEntries);

	if (m_oKnownFilesCS.TryLock())
	{
		FinishStreamInternal(pStream, nError, true);

		m_oKnownFilesCS.Unlock();
	}
	else
	{
		CryAutoCriticalSection oCS(m_oQueuedStreamsCS);

		m_mapQueuedStreams.insert(std::make_pair(pStream, nError)); // Adds uniquely.
	}
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::SerializeState(TSerialize ser)
{
	// Note: The AFCM cannot be responsible for managing GameHints during serialization!
	// This needs to be handled exclusively by the game itself!
	if (!ser.IsWriting())
	{
		// Clear out all game hints when a game state is reloaded before the new cache requests come in.
		// For that we need to flush the SoundSystem and EventSystem of any remains.
		CSoundSystem const* const pSoundSystem = static_cast<CSoundSystem const* const>(gEnv->pSoundSystem);

		if (pSoundSystem)
		{
			pSoundSystem->GetISoundAssetManager()->UnloadAllSoundBuffers();
			pSoundSystem->GetIAudioDevice()->ResetAudio();
		}		

		Clear(eAFCT_GAME_HINT);
	}
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::DrawInformation(IRenderer* const pRenderer, float const fPosX, float const fPosY, int const nSoundInfo)
{
#ifdef INCLUDE_AUDIO_PRODUCTION_CODE

	if (pRenderer)
	{
		// TODO: optimize this method to not iterate over the entire map for each entry type!
		CTimeValue const tFrameTime = gEnv->pTimer->GetAsyncTime();

		stack_string sTemp;
		float const fHeaderDrawSize = 1.3f;
		float const fEntryDrawSize  = 1.1f;
		float const fEntryStepSize  = 10.0f;
		float fPositionY            = fPosY;
		float fHeaderPos            = 0.0f;
		float fTime                 = 0.0f;
		float fRatio                = 0.0f;
		float fOriginalAlpha        = 0.7f;
		size_t nMemSize             = 0;
		float* pfColor              = NULL;

		// The colors.
		float fWhite[4]   = {1.0f, 1.0f, 1.0f, fOriginalAlpha};
		float fCyan[4]    = {0.0f, 1.0f, 1.0f, fOriginalAlpha};
		float fOrange[4]  = {1.0f, 0.5f, 0.0f, fOriginalAlpha};
		float fGreen[4]   = {0.0f, 1.0f, 0.0f, fOriginalAlpha};
		float fRed[4]     = {1.0f, 0.0f, 0.0f, fOriginalAlpha};
		float fBlue[4]    = {0.1f, 0.2f, 0.8f, fOriginalAlpha};
		float fYellow[4]  = {1.0f, 1.0f, 0.0f, fOriginalAlpha};
		float fBluish[4]  = {0.3f, 0.7f, 1.0f, fOriginalAlpha};
		float fDarkish[4] = {0.3f, 0.3f, 0.3f, fOriginalAlpha};

		pRenderer->Draw2dLabel(fPosX, fPosY, 1.6f, fOrange, false, "FileCacheManager (%d of %d KiB) [Entries: %d]", static_cast<int>(m_nCurrentByteTotal >> 10), static_cast<int>(m_nMaxByteTotal >> 10), static_cast<int>(m_mapKnownFiles.size()));

		tMapFileCacheEntries::const_iterator Iter(m_mapKnownFiles.begin());
		tMapFileCacheEntries::const_iterator const IterEnd(m_mapKnownFiles.end());

		// Store the globals header position.
		fPositionY  += 20.0f;
		fHeaderPos  = fPositionY;
		fPositionY  += 15.0f;

		for (; Iter != IterEnd; ++Iter)
		{
			SFileCacheEntry const& rFileCacheEntry = Iter->second;

			if (rFileCacheEntry.m_eAudioFileCacheType == eAFCT_GLOBAL &&
				 (rFileCacheEntry.m_eAudioFileCacheState & (eAFCS_CACHED|eAFCS_LOADING|eAFCS_VIRTUAL)) != 0)
			{
				if (g_SoundCVars.g_nFileCacheManagerDebugFilter == eAFCMDF_ALL ||
					 (g_SoundCVars.g_nFileCacheManagerDebugFilter & eAFCMDF_GLOBALS) != 0)
				{
					pfColor        = (rFileCacheEntry.m_eAudioFileCacheState & eAFCS_VIRTUAL) ? fDarkish : (rFileCacheEntry.m_eAudioFileCacheState & eAFCS_LOADING) ? fRed : ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_REMOVABLE) ? fWhite : fCyan);
					fTime          = (tFrameTime - rFileCacheEntry.m_oTimeCached).GetSeconds();
					fRatio         = fTime / 5.0f;
					fOriginalAlpha = pfColor[3];
					pfColor[3]     *= clamp_tpl(fRatio, 0.2f, 1.0f);

					sTemp.clear();

					if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_PRELOADED) == 0)
					{
						sTemp.Format("(%d)", static_cast<int>(rFileCacheEntry.m_nRequestedCount));
					}

					sTemp.Format("\t\t" + sTemp + "%s (%d KiB)", rFileCacheEntry.m_sFileName.c_str(), static_cast<int>(rFileCacheEntry.m_nSizeFile >> 10));
					pRenderer->Draw2dLabel(fPosX, fPositionY, fEntryDrawSize, pfColor, false, sTemp.c_str());
					pfColor[3] = fOriginalAlpha;
					fPositionY += fEntryStepSize;
				}

				nMemSize += rFileCacheEntry.m_nSizeCached;
			}
		}

		// The globals header.
		pRenderer->Draw2dLabel(fPosX, fHeaderPos, fHeaderDrawSize, fWhite, false, "\tGlobals (%d KiB)", static_cast<int>(nMemSize >> 10));
		fPositionY  += 20.0f;
		fHeaderPos  = fPositionY;
		fPositionY  += 15.0f;
		nMemSize    = 0;

		// Level specific files.
		Iter = m_mapKnownFiles.begin();

		for (; Iter != IterEnd; ++Iter)
		{
			SFileCacheEntry const& rFileCacheEntry = Iter->second;

			if (rFileCacheEntry.m_eAudioFileCacheType == eAFCT_LEVEL_SPECIFIC &&
				 (rFileCacheEntry.m_eAudioFileCacheState & (eAFCS_CACHED|eAFCS_LOADING|eAFCS_VIRTUAL)) != 0)
			{
				if (g_SoundCVars.g_nFileCacheManagerDebugFilter == eAFCMDF_ALL ||
					 (g_SoundCVars.g_nFileCacheManagerDebugFilter & eAFCMDF_LEVEL_SPECIFICS) != 0)
				{
					pfColor        = (rFileCacheEntry.m_eAudioFileCacheState & eAFCS_VIRTUAL) ? fDarkish : (rFileCacheEntry.m_eAudioFileCacheState & eAFCS_LOADING) ? fRed : ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_REMOVABLE) ? fWhite : fYellow);
					fTime          = (tFrameTime - rFileCacheEntry.m_oTimeCached).GetSeconds();
					fRatio         = fTime / 5.0f;
					fOriginalAlpha = pfColor[3];
					pfColor[3]     *= clamp_tpl(fRatio, 0.2f, 1.0f);

					sTemp.clear();
					
					if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_PRELOADED) == 0)
					{
						sTemp.Format("(%d)", static_cast<int>(rFileCacheEntry.m_nRequestedCount));
					}

					sTemp.Format("\t\t" + sTemp + "%s (%d KiB)", rFileCacheEntry.m_sFileName.c_str(), static_cast<int>(rFileCacheEntry.m_nSizeFile >> 10));
					pRenderer->Draw2dLabel(fPosX, fPositionY, fEntryDrawSize, pfColor, false, sTemp.c_str());
					pfColor[3] = fOriginalAlpha;
					fPositionY += fEntryStepSize;
				}

				nMemSize += rFileCacheEntry.m_nSizeCached;
			}
		}

		// The level specifics header.
		pRenderer->Draw2dLabel(fPosX, fHeaderPos, fHeaderDrawSize, fWhite, false, "\tLevel Specifics (%d KiB)", static_cast<int>(nMemSize >> 10));
		fPositionY  += 20.0f;
		fHeaderPos  = fPositionY;
		fPositionY  += 15.0f;
		nMemSize    = 0;

		// Game hinted files.
		Iter = m_mapKnownFiles.begin();

		for (; Iter != IterEnd; ++Iter)
		{
			SFileCacheEntry const& rFileCacheEntry = Iter->second;

			if (rFileCacheEntry.m_eAudioFileCacheType == eAFCT_GAME_HINT &&
				 (rFileCacheEntry.m_eAudioFileCacheState & (eAFCS_CACHED|eAFCS_LOADING|eAFCS_VIRTUAL)) != 0)
			{
				if (g_SoundCVars.g_nFileCacheManagerDebugFilter == eAFCMDF_ALL ||
					 (g_SoundCVars.g_nFileCacheManagerDebugFilter & eAFCMDF_GAME_HINTS) != 0)
				{
					pfColor        = (rFileCacheEntry.m_eAudioFileCacheState & eAFCS_VIRTUAL) ? fDarkish : (rFileCacheEntry.m_eAudioFileCacheState & eAFCS_LOADING) ? fRed : ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_REMOVABLE) ? fWhite : fGreen);
					fTime          = (tFrameTime - rFileCacheEntry.m_oTimeCached).GetSeconds();
					fRatio         = fTime / 5.0f;
					fOriginalAlpha = pfColor[3];
					pfColor[3]     *= clamp_tpl(fRatio, 0.2f, 1.0f);

					sTemp.clear();

					if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_PRELOADED) == 0)
					{
						sTemp.Format("(%d)", static_cast<int>(rFileCacheEntry.m_nRequestedCount));
					}

					sTemp.Format("\t\t" + sTemp + "%s (%d KiB) [%d]", rFileCacheEntry.m_sFileName.c_str(), static_cast<int>(rFileCacheEntry.m_nSizeFile >> 10), static_cast<int>(rFileCacheEntry.m_nUsedCount));
					pRenderer->Draw2dLabel(fPosX, fPositionY, fEntryDrawSize, pfColor, false, sTemp.c_str());
					pfColor[3] = fOriginalAlpha;
					fPositionY += fEntryStepSize;
				}

				nMemSize += rFileCacheEntry.m_nSizeCached;
			}
		}

		// The game hints header.
		pRenderer->Draw2dLabel(fPosX, fHeaderPos, fHeaderDrawSize, fWhite, false, "\tGame Hints (%d KiB)", static_cast<int>(nMemSize >> 10));
		fPositionY  += 20.0f;
		fHeaderPos  = fPositionY;
		fPositionY  += 15.0f;
		nMemSize    = 0;

		// Partially cached files.
		Iter = m_mapKnownFiles.begin();

		for (; Iter != IterEnd; ++Iter)
		{
			SFileCacheEntry const& rFileCacheEntry = Iter->second;

			if (rFileCacheEntry.m_eAudioFileCacheType == eAFCT_FSB_HEADER &&
				 (rFileCacheEntry.m_eAudioFileCacheState & (eAFCS_CACHED|eAFCS_LOADING|eAFCS_VIRTUAL)) != 0)
			{
				if (g_SoundCVars.g_nFileCacheManagerDebugFilter == eAFCMDF_ALL ||
					 (g_SoundCVars.g_nFileCacheManagerDebugFilter & eAFCMDF_PARTIALLY_CACHED) != 0)
				{
					pfColor        = (rFileCacheEntry.m_eAudioFileCacheState & eAFCS_VIRTUAL) ? fDarkish : (rFileCacheEntry.m_eAudioFileCacheState & eAFCS_LOADING) ? fRed : ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_REMOVABLE) ? fWhite : fOrange);
					fTime          = (tFrameTime - rFileCacheEntry.m_oTimeCached).GetSeconds();
					fRatio         = fTime / 5.0f;
					fOriginalAlpha = pfColor[3];
					pfColor[3]     *= clamp_tpl(fRatio, 0.2f, 1.0f);

					sTemp.clear();

					if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_PRELOADED) == 0)
					{
						sTemp.Format("(%d)", static_cast<int>(rFileCacheEntry.m_nRequestedCount));
					}

					sTemp.Format("\t\t" + sTemp + "%s (cached: %d KiB) (file: %d KiB)", rFileCacheEntry.m_sFileName.c_str(), static_cast<int>(rFileCacheEntry.m_nSizeCached >> 10), static_cast<int>(rFileCacheEntry.m_nSizeFile >> 10));
					pRenderer->Draw2dLabel(fPosX, fPositionY, fEntryDrawSize, pfColor, false, sTemp.c_str());
					pfColor[3] = fOriginalAlpha;
					fPositionY += fEntryStepSize;
				}

				nMemSize += rFileCacheEntry.m_nSizeCached;
			}
		}

		// The partially cached file's header.
		pRenderer->Draw2dLabel(fPosX, fHeaderPos, fHeaderDrawSize, fWhite, false, "\tPartially Cached (%d KiB)", static_cast<int>(nMemSize >> 10));
		fPositionY  += 20.0f;
		fHeaderPos  = fPositionY;
		fPositionY  += 15.0f;
		nMemSize    = 0;

		// Music files.
		Iter = m_mapKnownFiles.begin();

		for (; Iter != IterEnd; ++Iter)
		{
			SFileCacheEntry const& rFileCacheEntry = Iter->second;

			if (rFileCacheEntry.m_eAudioFileCacheType == eAFCT_MUSIC &&
				 (rFileCacheEntry.m_eAudioFileCacheState & (eAFCS_CACHED|eAFCS_LOADING|eAFCS_VIRTUAL)) != 0)
			{
				if (g_SoundCVars.g_nFileCacheManagerDebugFilter == eAFCMDF_ALL ||
					 (g_SoundCVars.g_nFileCacheManagerDebugFilter & eAFCMDF_MUSIC) != 0)
				{
					pfColor        = (rFileCacheEntry.m_eAudioFileCacheState & eAFCS_VIRTUAL) ? fDarkish : (rFileCacheEntry.m_eAudioFileCacheState & eAFCS_LOADING) ? fRed : fBluish;
					fTime          = (tFrameTime - rFileCacheEntry.m_oTimeCached).GetSeconds();
					fRatio         = fTime / 5.0f;
					fOriginalAlpha = pfColor[3];
					pfColor[3]     *= clamp_tpl(fRatio, 0.2f, 1.0f);
					pRenderer->Draw2dLabel(fPosX, fPositionY, fEntryDrawSize, pfColor, false, "\t\t(%d)%s (%d KiB)", static_cast<int>(rFileCacheEntry.m_nRequestedCount), rFileCacheEntry.m_sFileName.c_str(), static_cast<int>(rFileCacheEntry.m_nSizeFile >> 10));
					pfColor[3] = fOriginalAlpha;
					fPositionY += fEntryStepSize;
				}

				nMemSize += rFileCacheEntry.m_nSizeCached;
			}
		}

		// The music header.
		pRenderer->Draw2dLabel(fPosX, fHeaderPos, fHeaderDrawSize, fWhite, false, "\tMusic (%d KiB)", static_cast<int>(nMemSize >> 10));

		// Entries that were requested but not found.
		pRenderer->Draw2dLabel(fPosX + 500.0f, fPosY, 1.6f, fOrange, false, "Not Cached Requests");

		fPositionY = fPosY + 20.0f;

		if (g_SoundCVars.g_nFileCacheManagerDebugFilter == eAFCMDF_ALL ||
			 (g_SoundCVars.g_nFileCacheManagerDebugFilter & eAFCMDF_NOT_CACHED) != 0)
		{
			Iter = m_mapKnownFiles.begin();

			for (; Iter != IterEnd; ++Iter)
			{
				SFileCacheEntry const& rFileCacheEntry = Iter->second;

				pfColor = NULL;

				if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_NOTCACHED) != 0)
				{
					pfColor = fRed;
				}
				else if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_NOTFOUND) != 0)
				{
					pfColor = fWhite;
				}

				// This flag can be combined with eAFCS_NOTCACHED but not with eAFCS_NOTFOUND
				if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_MEMALLOCFAIL) != 0)
				{
					pfColor = fBlue;
				}

				if (pfColor != NULL &&
					 (rFileCacheEntry.m_nRequestedCount > 0 ||
					 (rFileCacheEntry.m_eAudioFileCacheState & eAFCS_NOTFOUND) != 0 ||
					 (rFileCacheEntry.m_eAudioFileCacheState & eAFCS_MEMALLOCFAIL) != 0))
				{
					fTime          = (tFrameTime - rFileCacheEntry.m_oTimeCached).GetSeconds();
					fRatio         = fTime / 5.0f;
					fOriginalAlpha = pfColor[3];
					pfColor[3]     *= clamp_tpl(fRatio, 0.2f, 1.0f);
					pRenderer->Draw2dLabel(fPosX + 500.0f, fPositionY, fEntryDrawSize, pfColor, false, "\t(%d)%s (%d KiB)", static_cast<int>(rFileCacheEntry.m_nRequestedCount), rFileCacheEntry.m_sFileName.c_str(), static_cast<int>(rFileCacheEntry.m_nSizeFile >> 10));
					pfColor[3] = fOriginalAlpha;
					fPositionY += fEntryStepSize;
				}
			}
		}

		// Memory fragmentation for PS3 only at the moment as only the PS3 uses a contiguous memory block.









































































































































	}

#endif // INCLUDE_AUDIO_PRODUCTION_CODE
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::ResetRequestedCounter()
{
#ifdef INCLUDE_AUDIO_PRODUCTION_CODE
	
	CryAutoCriticalSection oCS(m_oKnownFilesCS);

	tMapFileCacheEntries::const_iterator const IterEnd(m_mapKnownFiles.end());

	for (tMapFileCacheEntries::iterator Iter(m_mapKnownFiles.begin()); Iter != IterEnd; ++Iter)
	{
		(Iter->second).m_nRequestedCount = 0;
	}

#endif // INCLUDE_AUDIO_PRODUCTION_CODE
}

//////////////////////////////////////////////////////////////////////////
bool CFileCacheManager::EntryHasStateFlags(CCryNameCRC const& rNameCRC, const EAudioFileCacheState eFlags)
{
	CryAutoCriticalSection oCS(m_oKnownFilesCS);

	tMapFileCacheEntries::const_iterator const IterEntries(m_mapKnownFiles.find(rNameCRC));

	if (IterEntries != m_mapKnownFiles.end())
	{
		return (IterEntries->second.m_eAudioFileCacheState & eFlags) != 0;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
bool CFileCacheManager::DoesRequestFitInternal(size_t const nRequestSize)
{
	// Make sure these unsigned values don't flip around.
	assert(m_nCurrentByteTotal <= m_nMaxByteTotal);
	bool bSuccess = false;

	if (nRequestSize <= (m_nMaxByteTotal - m_nCurrentByteTotal))
	{
		// Here the requested size is available without the need of first cleaning up.
		bSuccess = true;
	}
	else
	{
		// Determine how much memory would get freed if all eAFCS_REMOVABLE files get thrown out.
		// We however skip files that are already queued for unload. The request will get queued up int that case.
		size_t nPossibleMemoryGain = 0;

		// Check the single file entries for removability.
		tMapFileCacheEntries::const_iterator const IterEnd(m_mapKnownFiles.end());

		for (tMapFileCacheEntries::const_iterator Iter(m_mapKnownFiles.begin()); Iter != IterEnd; ++Iter)
		{
			SFileCacheEntry const& rFileCacheEntry = Iter->second;

			if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_CACHED) != 0 &&
				  (rFileCacheEntry.m_eAudioFileCacheState & eAFCS_REMOVABLE) != 0 &&
					(rFileCacheEntry.m_eAudioFileCacheState & eAFCS_QUEUED_FOR_UNLOAD) == 0)
			{
				nPossibleMemoryGain += rFileCacheEntry.m_nSizeCached;
			}
		}

		size_t const nMaxAvailableSize = (m_nMaxByteTotal - (m_nCurrentByteTotal - nPossibleMemoryGain));

		if (nRequestSize <= nMaxAvailableSize)
		{
			// Here we need to cleanup first before allowing the new request to be allocated.
			tMapFileCacheEntries::const_iterator const IterEntriesEnd(m_mapKnownFiles.end());

			for (tMapFileCacheEntries::iterator Iter(m_mapKnownFiles.begin()); Iter != IterEntriesEnd; ++Iter)
			{
				SFileCacheEntry& rFileCacheEntry = Iter->second;

				if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_CACHED) != 0 &&
					  (rFileCacheEntry.m_eAudioFileCacheState & eAFCS_REMOVABLE) != 0 &&
						(rFileCacheEntry.m_eAudioFileCacheState & eAFCS_QUEUED_FOR_UNLOAD) == 0)
				{
					UncacheFileCacheEntryInternal(rFileCacheEntry, true);
				}
			}

			// We should only indicate success if there's actually really enough room for the new entry!
			bSuccess = (m_nMaxByteTotal - m_nCurrentByteTotal) >= nRequestSize;
		}
	}

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////////
bool CFileCacheManager::QueueFileCacheEntryForPreloadInternal(SFileCacheEntry& rFileCacheEntry)
{
	bool bSuccess = false;

	// Make sure this entry is basically preloadable.
	if (IsPreloadableInternal(rFileCacheEntry))
	{
		CryAutoCriticalSection oCS(m_oQueuedPreloadUnloadEntriesCS);

		CCryNameCRC const oNameCRC(rFileCacheEntry.m_sFileName.c_str());

		// Must be unloaded and not already queued for preload.
		if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_PRELOADED) == 0 &&
			  (rFileCacheEntry.m_eAudioFileCacheState & eAFCS_QUEUED_FOR_PRELOAD) == 0)
		{
			assert(rFileCacheEntry.m_pMemoryBlock != NULL); // Must have valid memory.
			rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState & ~eAFCS_QUEUED_FOR_UNLOAD);
			rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState | eAFCS_QUEUED_FOR_PRELOAD);
			stl::push_back_unique(m_aoQueuedPreloadEntries, oNameCRC);

			bSuccess = true;

#ifdef INCLUDE_AUDIO_PRODUCTION_CODE
			if (stl::find(m_aoQueuedUnloadEntries, oNameCRC))
			{
				// If this cache entry was also found in the "unload" list something is quite nervous here.
				assert(0);
			}
#endif // INCLUDE_AUDIO_PRODUCTION_CODE
		}
		else
		{
			// If we get here the entry is either already loaded, queued for unload or already queued for preload.
			// Make sure it does not stay in m_aoQueuedUnloadEntries.
			rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState & ~eAFCS_QUEUED_FOR_UNLOAD);
			stl::find_and_erase(m_aoQueuedUnloadEntries, oNameCRC);

			// Indicate as well success if this entry is already queued for preload.
			bSuccess = (rFileCacheEntry.m_eAudioFileCacheState & eAFCS_QUEUED_FOR_PRELOAD) != 0;
		}
	}

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////////
bool CFileCacheManager::QueueFileCacheEntryForUnloadInternal(SFileCacheEntry& rFileCacheEntry, bool const bUncache)
{
	CryAutoCriticalSection oCS(m_oQueuedPreloadUnloadEntriesCS);

	bool bSuccess = false;
	CCryNameCRC const oNameCRC(rFileCacheEntry.m_sFileName.c_str());

	// Must be loaded and not already queued for unload.
	if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_PRELOADED) != 0 &&
			(rFileCacheEntry.m_eAudioFileCacheState & eAFCS_QUEUED_FOR_UNLOAD) == 0)
	{
		assert(rFileCacheEntry.m_pMemoryBlock != NULL); // Must have valid memory.

		if (bUncache)
		{
			rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState | eAFCS_UNCACHE_AFTER_UNLOAD);
		}

		rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState & ~eAFCS_QUEUED_FOR_PRELOAD);
		rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState | eAFCS_QUEUED_FOR_UNLOAD);
		stl::push_back_unique(m_aoQueuedUnloadEntries, oNameCRC);

		bSuccess = true;

#ifdef INCLUDE_AUDIO_PRODUCTION_CODE
		if (stl::find(m_aoQueuedPreloadEntries, oNameCRC))
		{
			// If this cache entry was also found in the "preload" list something is quite nervous here.
			assert(0);
		}
#endif // INCLUDE_AUDIO_PRODUCTION_CODE
	}
	else
	{
		// If we get here the entry is either not loaded at all, queued for preload or already queued for unload.
		// Make sure it does not stay in m_aoQueuedPreloadEntries.
		if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_SOUND_NOT_READY) == 0)
		{
			rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState & ~eAFCS_QUEUED_FOR_PRELOAD);
			stl::find_and_erase(m_aoQueuedPreloadEntries, oNameCRC);
		}
		else
		{
			// In case this entry is still waiting for its sound instance to preload set the corresponding flags.
			if (bUncache)
			{
				rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState | eAFCS_UNCACHE_WHEN_SOUND_READY);
			}
			else
			{
				rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState | eAFCS_UNLOAD_WHEN_SOUND_READY);
			}
		}

		// Indicate as well success if this entry is already queued for unload or is still waiting on its sound instance.
		bSuccess = (rFileCacheEntry.m_eAudioFileCacheState & (eAFCS_QUEUED_FOR_UNLOAD|eAFCS_UNLOAD_WHEN_SOUND_READY|eAFCS_UNCACHE_WHEN_SOUND_READY)) != 0;
	}

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::ChangeCacheStatus(bool const bCache)
{
	CryAutoCriticalSection oCS1(m_oKnownFilesCS);

	m_bCanCacheEntries = bCache;
	tMapFileCacheEntries::const_iterator const IterEnd(m_mapKnownFiles.end());

	for (tMapFileCacheEntries::iterator Iter(m_mapKnownFiles.begin()); Iter != IterEnd; ++Iter)
	{
		SFileCacheEntry& rFileCacheEntry = Iter->second;

		if (bCache)
		{
			// Entries must not be in eAFCS_CACHED state.
			assert((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_CACHED) == 0);

			// Must be flagged virtual in order to get cached again.
			if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_VIRTUAL) != 0)
			{
				// Set the state to eAFCS_LOADING and remove the eAFCS_VIRTUAL flag.
				rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState & ~eAFCS_VIRTUAL);
				rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState | eAFCS_LOADING);

				if (AllocateMemoryBlockInternal(rFileCacheEntry))
				{
					// Start the asynchronous read stream to copy the data into the memory block.
					StreamReadParams oStreamReadParams;
					oStreamReadParams.nOffset      = rFileCacheEntry.m_nFileOffset;
					oStreamReadParams.nFlags       = IStreamEngine::FLAGS_NO_SYNC_CALLBACK;



					oStreamReadParams.dwUserData   = static_cast<DWORD_PTR>(Iter->first.get());

					oStreamReadParams.nLoadTime    = 0;
					oStreamReadParams.nMaxLoadTime = 0;
					oStreamReadParams.ePriority    = estpUrgent;
					oStreamReadParams.pBuffer      = rFileCacheEntry.m_pMemoryBlock->GetData();
					oStreamReadParams.nSize        = static_cast<int unsigned>(rFileCacheEntry.m_nSizeCached);

					rFileCacheEntry.m_pReadStream = gEnv->pSystem->GetStreamEngine()->StartRead(eStreamTaskTypeFSBCache, rFileCacheEntry.m_sFileName.c_str(), this, &oStreamReadParams);
				}
			}
		}
		else
		{
			// Entries mustn't be in eAFCS_VIRTUAL state.
			assert((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_VIRTUAL) == 0);

			// Must be cached.
			if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_CACHED) != 0)
			{
				// Set to state eAFCS_VIRTUAL and remove state flag eAFCS_CACHED.
				rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState & ~eAFCS_CACHED);
				rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState | eAFCS_VIRTUAL);

				if (!QueueFileCacheEntryForUnloadInternal(rFileCacheEntry, false))
				{
					// The entry wasn't queued so uncache immediately.
          CryAutoCriticalSection oCS2(m_oFileReadCS);
					rFileCacheEntry.m_pMemoryBlock = NULL;
				}
			}
		}
	}

	if (!bCache)
	{
		// As ChangeCacheStatus() is called from the Main thread we can safely call UpdatePatchRequests() here right away.
		// This call is necessary during unloading as the sound system does not update anymore once it's disabled hence entries will not get unloaded.
		UpdatePatchRequestsInternal();
	}
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::OnCVarChanged(ICVar* const pCVar)
{
	CryAutoCriticalSection oCS(m_oKnownFilesCS);

	stack_string const sTemp(pCVar->GetName());

	if (sTemp.compareNoCase("s_AllowSoundBankPatching") == 0)
	{
		// Handle the preloading/unloading to/from FMOD depending on the "s_AllowSoundBankPatching" cvar.
		tMapFileCacheEntries::const_iterator const IterFilesEnd(m_mapKnownFiles.end());

		switch (pCVar->GetIVal())
		{
		case 0:
			{
				for (tMapFileCacheEntries::iterator IterFiles(m_mapKnownFiles.begin()); IterFiles != IterFilesEnd; ++IterFiles)
				{
					QueueFileCacheEntryForUnloadInternal(IterFiles->second, false);
				}

				break;
			}
		case 1:
			{
				for (tMapFileCacheEntries::iterator IterFiles(m_mapKnownFiles.begin()); IterFiles != IterFilesEnd; ++IterFiles)
				{
					SFileCacheEntry& rFileCacheEntry = IterFiles->second;

					if (!QueueFileCacheEntryForPreloadInternal(rFileCacheEntry))
					{
						// This file hasn't been queued so check for queued reads.
						SFileCacheEntry::tMapQueuedReads::const_iterator const IterQueuedReadsEnd(rFileCacheEntry.m_mapQueuedReads.end());

						for (SFileCacheEntry::tMapQueuedReads::const_iterator IterQueuedReads(rFileCacheEntry.m_mapQueuedReads.begin()); IterQueuedReads != IterQueuedReadsEnd; ++IterQueuedReads)
						{
							SFileCacheEntry::tVecQueuedReads::const_iterator const IterReadEnd(IterQueuedReads->second.end());

							for (SFileCacheEntry::tVecQueuedReads::const_iterator IterRead(IterQueuedReads->second.begin()); IterRead != IterReadEnd; ++IterRead)
							{
								// This will assure that the read is not issued from the main thread on PS3.
								// TODO: Double check if there are several jobs issued that need syncing!
								TRunQueuedReadJob job(*IterRead, IterQueuedReads->first);
								job.SetBlocking();
								job.SetClassInstance(this);
								job.Run();
							}
						}
					}
				}

				break;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
bool CFileCacheManager::IsPreloadableInternal(SFileCacheEntry const& rFileCacheEntry) const
{
#ifndef PS3
	return (g_SoundCVars.g_nAllowSoundBankPatching == 1 &&
		     (rFileCacheEntry.m_eAudioFileCacheState & eAFCS_CACHED) != 0 &&
		     (rFileCacheEntry.m_eAudioFileCacheType == eAFCT_GLOBAL ||
					rFileCacheEntry.m_eAudioFileCacheType == eAFCT_LEVEL_SPECIFIC ||
					rFileCacheEntry.m_eAudioFileCacheType == eAFCT_GAME_HINT));


#endif // PS3
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::AbortAllReadStreams()
{
	CryAutoCriticalSection oCS(m_oKnownFilesCS);

	tMapFileCacheEntries::const_iterator const IterEntriesEnd(m_mapKnownFiles.end());

	for (tMapFileCacheEntries::iterator Iter(m_mapKnownFiles.begin()); Iter != IterEntriesEnd; ++Iter)
	{
		Iter->second.AbortReadStreams(0);
	}
}

//////////////////////////////////////////////////////////////////////////
bool CFileCacheManager::RemoveReadStreamOnEntry(void* const pHandle, IReadStreamPtr const pReadStream)
{
	bool bSuccess = true;

	if (m_oKnownFilesCS.TryLock())
	{
		CCryNameCRC const& rNameCRC(reinterpret_cast<uint32>(pHandle));
		tMapFileCacheEntries::iterator const Iter(m_mapKnownFiles.find(rNameCRC));

		if (Iter != m_mapKnownFiles.end())
		{
			bSuccess = Iter->second.RemoveReadStream(pReadStream);
		}

		m_oKnownFilesCS.Unlock();
	}
	else
	{
		CryAutoCriticalSection oCS(m_oQueuedRequestInfosCS);

		SAudioFileCacheManagerQueuedRequestInfo oQueuedRequestInfo;
		oQueuedRequestInfo.eType       = eAFCMQRT_REMOVE_READ_STREAM_ON_ENTRY;
		oQueuedRequestInfo.pHandle     = pHandle;
		oQueuedRequestInfo.pReadStream = pReadStream;
		m_aoQueuedRequestInfos.push_back(oQueuedRequestInfo);
	}

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////////
bool CFileCacheManager::FinishStreamInternal(IReadStreamPtr const pStream, int unsigned const nError, bool const bReadDirectly)
{
	bool bSuccess = false;




	CCryNameCRC const& rNameCRC(static_cast<uint32>(pStream->GetUserData()));

	tMapFileCacheEntries::iterator const Iter(m_mapKnownFiles.find(rNameCRC));

	if (Iter != m_mapKnownFiles.end())
	{
		SFileCacheEntry& rFileCacheEntry = Iter->second;

		// Must be loading in to proceed.
		if ((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_LOADING) != 0)
		{
			if (nError == 0)
			{
				rFileCacheEntry.m_pReadStream          = NULL;
				rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState | eAFCS_CACHED);
				rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState & ~(eAFCS_LOADING|eAFCS_NOTCACHED));

#ifdef INCLUDE_AUDIO_PRODUCTION_CODE
				rFileCacheEntry.m_oTimeCached = gEnv->pTimer->GetAsyncTime();
#endif // INCLUDE_AUDIO_PRODUCTION_CODE

				if (rFileCacheEntry.m_eAudioFileCacheType == eAFCT_MUSIC)
				{
					// Music files are always removable as they're seen volatile.
					rFileCacheEntry.m_eAudioFileCacheState = (EAudioFileCacheState)((int)rFileCacheEntry.m_eAudioFileCacheState | eAFCS_REMOVABLE);
				}

				// Make sure the eAFCS_NOTCACHED flag has been removed for state and type.
				assert((rFileCacheEntry.m_eAudioFileCacheState & eAFCS_NOTCACHED) == 0 && (rFileCacheEntry.m_eAudioFileCacheType != eAFCT_NOTCACHED));

				if (!QueueFileCacheEntryForPreloadInternal(rFileCacheEntry) && !rFileCacheEntry.m_mapQueuedReads.empty())
				{
					// This file hasn't been queued and got queued reads.
					// We need a local copy as ReadFileCacheEntry removes from the map.
					SFileCacheEntry::tMapQueuedReads const mapQueuedReads(rFileCacheEntry.m_mapQueuedReads);
					SFileCacheEntry::tMapQueuedReads::const_iterator const IterQueuedReadsEnd(mapQueuedReads.end());

					for (SFileCacheEntry::tMapQueuedReads::const_iterator IterQueuedReads(mapQueuedReads.begin()); IterQueuedReads != IterQueuedReadsEnd; ++IterQueuedReads)
					{
						SFileCacheEntry::tVecQueuedReads::const_iterator const IterReadEnd(IterQueuedReads->second.end());

						for (SFileCacheEntry::tVecQueuedReads::const_iterator IterRead(IterQueuedReads->second.begin()); IterRead != IterReadEnd; ++IterRead)
						{
							if (bReadDirectly)
							{
								// We're not on the main thread therefore issue this read directly.
								ReadFileCacheEntry(*IterRead, IterQueuedReads->first);
							}
							else
							{
								// This will assure that the read is not issued from the main thread on PS3.
								// TODO: Double check if there are several jobs issued that need syncing!
								TRunQueuedReadJob job(*IterRead, IterQueuedReads->first);
								job.SetBlocking();
								job.SetClassInstance(this);
								job.Run();
							}
						}
					}
				}

				bSuccess = true;
			}
			else if (nError == ERROR_USER_ABORT)
			{
				// We abort this stream only during entry Uncache().
				// Therefore there's no need to call Uncache() during stream abort with error code ERROR_USER_ABORT.
				gEnv->pSoundSystem->Log(eSLT_Always, "<Sound> AFCM: user aborted stream for file %s (error: %d)", rFileCacheEntry.m_sFileName.c_str(), nError);
			}
			else
			{
				UncacheFileCacheEntryInternal(rFileCacheEntry, true, true);
				gEnv->pSoundSystem->Log(eSLT_Error, "<Sound> AFCM: failed to stream in file %s (error: %d)", rFileCacheEntry.m_sFileName.c_str(), nError);
			}
		}
#ifdef INCLUDE_AUDIO_PRODUCTION_CODE
		else
		{
			// Can happen since FinishStreamInternal can be called delayed. This is not bad, let's just get informed during production.
			gEnv->pSoundSystem->Log(eSLT_Always, "<Sound> AFCM: FinishStreamInternal but entry not loading! (state: %d) %s (error: %d)", static_cast<int>(rFileCacheEntry.m_eAudioFileCacheState), rFileCacheEntry.m_sFileName.c_str(), nError);
		}
#endif // INCLUDE_AUDIO_PRODUCTION_CODE
	}
	else
	{
		// Investigate why this entry doesn't exist!
		assert(0);
	}

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::SFileCacheEntry::FailQueuedReads(size_t const nEventOpenID)
{
	if (nEventOpenID != 0)
	{
		SFileCacheEntry::tMapQueuedReads::iterator const IterQueuedReads(m_mapQueuedReads.find(nEventOpenID));

		if (IterQueuedReads != m_mapQueuedReads.end())
		{
			SFileCacheEntry::tVecQueuedReads::const_iterator const IterReadEnd(IterQueuedReads->second.end());

			for (SFileCacheEntry::tVecQueuedReads::iterator IterRead(IterQueuedReads->second.begin()); IterRead != IterReadEnd; ++IterRead)
			{
				static_cast<FMOD_ASYNCREADINFO* const>(*IterRead)->result = FMOD_ERR_FILE_BAD;
			}

			IterQueuedReads->second.clear();

			m_mapQueuedReads.erase(IterQueuedReads);
		}
	}
	else
	{
		SFileCacheEntry::tMapQueuedReads::const_iterator const IterQueuedReadsEnd(m_mapQueuedReads.end());

		for (SFileCacheEntry::tMapQueuedReads::iterator IterQueuedReads(m_mapQueuedReads.begin()); IterQueuedReads != IterQueuedReadsEnd; ++IterQueuedReads)
		{
			SFileCacheEntry::tVecQueuedReads::const_iterator const IterReadEnd(IterQueuedReads->second.end());

			for (SFileCacheEntry::tVecQueuedReads::iterator IterRead(IterQueuedReads->second.begin()); IterRead != IterReadEnd; ++IterRead)
			{
				static_cast<FMOD_ASYNCREADINFO* const>(*IterRead)->result = FMOD_ERR_FILE_BAD;
			}

			IterQueuedReads->second.clear();
		}

		m_mapQueuedReads.clear();
	}
}

//////////////////////////////////////////////////////////////////////////
bool CFileCacheManager::AllocateMemoryBlockInternal(SFileCacheEntry& rFileCacheEntry)
{
	bool bSuccess = false;

	// Mustn't not have valid memory yet.
	assert(rFileCacheEntry.m_pMemoryBlock == NULL);

	if (m_pMemoryHeapMain != NULL)
	{
		rFileCacheEntry.m_pMemoryBlock = m_pMemoryHeapMain->AllocateBlock(rFileCacheEntry.m_nSizeCached, rFileCacheEntry.m_sFileName.c_str());
	}

	if (rFileCacheEntry.m_pMemoryBlock == NULL)
	{
		// The main heap is either full or too fragmented, let's try to allocate on the extended heap.
		if (m_pMemoryHeapExtended != NULL)
		{
			rFileCacheEntry.m_pMemoryBlock = m_pMemoryHeapExtended->AllocateBlock(rFileCacheEntry.m_nSizeCached, rFileCacheEntry.m_sFileName.c_str());
		}

		if (rFileCacheEntry.m_pMemoryBlock == NULL)
		{
			// Now both memory blocks are either full or too fragmented, let's try to throw everything out that can be removed and allocate again.
			tMapFileCacheEntries::iterator Iter(m_mapKnownFiles.begin());
			tMapFileCacheEntries::const_iterator const IterEnd(m_mapKnownFiles.end());

			for (Iter = m_mapKnownFiles.begin(); Iter != IterEnd; ++Iter)
			{
				SFileCacheEntry& rTempFileCacheEntry = Iter->second;

				if ((rTempFileCacheEntry.m_eAudioFileCacheState & eAFCS_REMOVABLE) != 0)
				{
					UncacheFileCacheEntryInternal(rTempFileCacheEntry, true);
				}
			}

			// First try the main block.
			if (m_pMemoryHeapMain != NULL)
			{
				rFileCacheEntry.m_pMemoryBlock = m_pMemoryHeapMain->AllocateBlock(rFileCacheEntry.m_nSizeCached, rFileCacheEntry.m_sFileName.c_str());
			}

			if (rFileCacheEntry.m_pMemoryBlock == NULL)
			{
				// If that failed try the extended block as a last resort.
				if (m_pMemoryHeapExtended != NULL)
				{
					rFileCacheEntry.m_pMemoryBlock = m_pMemoryHeapExtended->AllocateBlock(rFileCacheEntry.m_nSizeCached, rFileCacheEntry.m_sFileName.c_str());
				}
			}
		}
	}

	return rFileCacheEntry.m_pMemoryBlock != NULL;
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::ProcessQueuedRequests()
{
	if (!m_aoQueuedRequestInfos.empty())
	{
		CryAutoCriticalSection oCS(m_oQueuedRequestInfosCS);

		TVecQueuedRequestInfos::const_iterator IterEnd(m_aoQueuedRequestInfos.end());

		for (TVecQueuedRequestInfos::iterator Iter(m_aoQueuedRequestInfos.begin()); Iter != IterEnd; ++Iter)
		{
			SAudioFileCacheManagerQueuedRequestInfo const& rQueuedRequestInfo(*Iter);

			switch (rQueuedRequestInfo.eType)
			{
			case eAFCMQRT_REMOVE_READ_STREAM_ON_ENTRY:
				{
					bool const bSuccess = RemoveReadStreamOnEntry(rQueuedRequestInfo.pHandle, rQueuedRequestInfo.pReadStream);
					assert(bSuccess);

					break;
				}
			default:
				{
					// Mustn't happen, we need to handle this type!
					assert(0);

					break;
				}
			}
		}

		m_aoQueuedRequestInfos.clear();
	}
}

//////////////////////////////////////////////////////////////////////////
void CFileCacheManager::FinishQueuedReads(SFileCacheEntry const& rFileCacheEntry)
{
	if (!rFileCacheEntry.m_mapQueuedReads.empty())
	{
		// We need a local copy as ReadFileCacheEntry removes from the map.
		SFileCacheEntry::tMapQueuedReads const mapQueuedReads(rFileCacheEntry.m_mapQueuedReads);
		SFileCacheEntry::tMapQueuedReads::const_iterator const IterQueuedReadsEnd(mapQueuedReads.end());

		for (SFileCacheEntry::tMapQueuedReads::const_iterator IterQueuedReads(mapQueuedReads.begin()); IterQueuedReads != IterQueuedReadsEnd; ++IterQueuedReads)
		{
			SFileCacheEntry::tVecQueuedReads::const_iterator const IterReadEnd(IterQueuedReads->second.end());

			for (SFileCacheEntry::tVecQueuedReads::const_iterator IterRead(IterQueuedReads->second.begin()); IterRead != IterReadEnd; ++IterRead)
			{
				// TODO: Verify if running a read job is feasible.
				ReadFileCacheEntry(*IterRead, IterQueuedReads->first);
			}
		}
	}
}