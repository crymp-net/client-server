////////////////////////////////////////////////////////////////////////////
//
//  CryEngine Source File.
//  Copyright (C), Crytek, 1999-2010.
//
// -------------------------------------------------------------------------
//  File name:   FileCacheManager.h
//  Version:     v1.00
//  Created:     19/10/2010 by Thomas Wollenzin
//  Description: This is the implementation of the FileCacheManager class
//
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef FILE_CACHE_MANAGER_H_INCLUDED
#define FILE_CACHE_MANAGER_H_INCLUDED

#include "CryCommon/CrySystem/IStreamEngine.h"
#include <VectorMap.h>
#include <CryName.h>
#include <IJobManager.h>

#define AFCM_FILE_NAME_LENGTH 128

// Forward declarations
class CCustomMemoryHeap;

// Return results
typedef enum
{
	AFCM_ERROR,
	AFCM_FILE_NOT_FOUND,
	AFCM_FILE_NOT_READY,
	AFCM_FILE_NOT_CACHED,
	AFCM_FILE_OUTSIDE_REGION,
	AFCM_FILE_FLAGGED_VIRTUAL,
} AFCM_RESULT;

// Filter for drawing debug info to the screen
enum EAudioFileCacheManagerDebugFilter
{
	eAFCMDF_ALL              = 0,
	eAFCMDF_GLOBALS          = BIT(6),  // a
	eAFCMDF_LEVEL_SPECIFICS  = BIT(7),  // b
	eAFCMDF_GAME_HINTS       = BIT(8),  // c
	eAFCMDF_PARTIALLY_CACHED = BIT(9),  // d
	eAFCMDF_MUSIC            = BIT(10), // e
	eAFCMDF_NOT_CACHED       = BIT(11), // f
	eAFCMDF_FRAGMENTATION    = BIT(12), // g
};

// Types of queued requests.
enum EAudioFileCacheManagerQueudRequestType 
{
	eAFCMQRT_NONE,
	eAFCMQRT_REMOVE_READ_STREAM_ON_ENTRY,
};

// Info struct of a queued request.
struct SAudioFileCacheManagerQueuedRequestInfo 
{
	SAudioFileCacheManagerQueuedRequestInfo()
	: eType(eAFCMQRT_NONE),
	  pHandle(NULL)
	{}

	EAudioFileCacheManagerQueudRequestType eType;
	void* pHandle;
	IReadStreamPtr pReadStream;
};

class CFileCacheManager : public IStreamCallback
{
public:

	CFileCacheManager();
	~CFileCacheManager();

	// Public methods
	void Init();
	void Update();
	void AllocateHeap(size_t const nSize, char const* const sUsage);
	void ParsePreloadsXML(char const* const sLevelName);
	bool CreateEventSlot(CCryNameCRC const& rNameCRC, size_t const nEventOpenID);
	bool RemoveEventSlot(void* const pHandle, size_t const nEventOpenID);
	bool CreateFileCacheEntry(char const* const sName, EAudioFileCacheType const eAudioFileCacheType);
	void CacheFile(char const* const sName, CCryNameCRC const& rNameCRC, EAudioFileCacheType const eAudioFileCacheType, size_t const nRequestFileOffset = 0, int const nRequestBytes = -1);
	bool RemoveEntryOrHint(CCryNameCRC const& rNameCRC, bool const bNow);
	void Clear(EAudioFileCacheType const eAudioFileCacheType = eAFCT_ALL);
	bool IsFileCacheEntryCached(CCryNameCRC const& rNameCRC);
	bool GetFileCacheEntryState(CCryNameCRC const& rNameCRC, EAudioFileCacheState& eAudioFileCacheState);
	bool FindFileCacheEntry(CCryNameCRC const& rNameCRC, void** const ppHandle, int unsigned* const pFileSize);
	void ReadFileCacheEntry(void* const pReadInfo, size_t const nEventOpenID);
	void SerializeState(TSerialize ser);
	void DrawInformation(IRenderer* const pRenderer, float const fPosX, float const fPosY, int const nSoundInfo);
	void ResetRequestedCounter();
	void Defrag();
	bool EntryHasStateFlags(CCryNameCRC const& rNameCRC, const EAudioFileCacheState eFlags);
	void ChangeCacheStatus(bool const bCache);
	void OnCVarChanged(ICVar* const pCVar);
	void RunQueuedRead(void* const pReadInfo, size_t const nEventOpenID);
	void AbortAllReadStreams();
	bool RemoveReadStreamOnEntry(void* const pHandle, IReadStreamPtr const pReadStream);

private:

	CFileCacheManager(CFileCacheManager const&);						// Copy protection
	CFileCacheManager& operator=(CFileCacheManager const&);	// Copy protection

	struct SFileCacheEntry
	{
		SFileCacheEntry()
			:	m_nSizeFile(0),
				m_nSizeCached(0),
				m_nFileOffset(0),
				m_nUsedCount(0),
				m_pMemoryBlock(NULL),
				m_eAudioFileCacheType(eAFCT_ALL),
				m_eAudioFileCacheState(eAFCS_NOTFOUND),
				m_eStreamTaskType(eStreamTaskTypeCount),
				m_pSoundBufferData(NULL),
				m_pReadStream(NULL)
		{
			m_mapReadStreams.reserve(2);
			m_mapQueuedReads.reserve(2);

#ifdef INCLUDE_AUDIO_PRODUCTION_CODE
			m_nRequestedCount = 0;
			m_oTimeCached.SetValue(0);
#endif // INCLUDE_AUDIO_PRODUCTION_CODE
		}

		SFileCacheEntry(SFileCacheEntry const& rOther)
		{
			m_sFileName            = rOther.m_sFileName;
			m_nSizeFile            = rOther.m_nSizeFile;
			m_nSizeCached          = rOther.m_nSizeCached;
			m_nFileOffset          = rOther.m_nFileOffset;
			m_nUsedCount           = rOther.m_nUsedCount;
			m_pMemoryBlock         = rOther.m_pMemoryBlock;
			m_eAudioFileCacheType  = rOther.m_eAudioFileCacheType;
			m_eAudioFileCacheState = rOther.m_eAudioFileCacheState;
			m_eStreamTaskType      = rOther.m_eStreamTaskType;
			m_pSoundBufferData     = rOther.m_pSoundBufferData;
			m_pReadStream          = rOther.m_pReadStream;
			m_mapReadStreams       = rOther.m_mapReadStreams;
			m_mapQueuedReads       = rOther.m_mapQueuedReads;

#ifdef INCLUDE_AUDIO_PRODUCTION_CODE
			m_nRequestedCount = rOther.m_nRequestedCount;
			m_oTimeCached     = rOther.m_oTimeCached;
#endif // INCLUDE_AUDIO_PRODUCTION_CODE
		}

		~SFileCacheEntry()
		{
			Reset();
		}

		//////////////////////////////////////////////////////////////////////////
		bool AddQueuedRead(size_t const nEventOpenID, void* const pQueuedRead)
		{
			bool bSuccess = false;
			tMapQueuedReads::iterator const IterOpenID(m_mapQueuedReads.find(nEventOpenID));

			if (IterOpenID != m_mapQueuedReads.end())
			{
				bSuccess = stl::push_back_unique(IterOpenID->second, pQueuedRead);
			}
			else
			{
				tVecQueuedReads apQueuedReads;
				apQueuedReads.push_back(pQueuedRead);
				m_mapQueuedReads.insert(std::make_pair(nEventOpenID, apQueuedReads));
				bSuccess = true;
			}

			return bSuccess;
		}

		//////////////////////////////////////////////////////////////////////////
		bool RemoveQueuedRead(size_t const nEventOpenID, void* const pQueuedRead)
		{
			bool bSuccess = false;
			tMapQueuedReads::iterator const IterOpenID(m_mapQueuedReads.find(nEventOpenID));

			if (IterOpenID != m_mapQueuedReads.end())
			{
				bSuccess = stl::find_and_erase(IterOpenID->second, pQueuedRead);

				if (IterOpenID->second.empty())
				{
					m_mapQueuedReads.erase(IterOpenID);
				}
			}

			return bSuccess;
		}

		//////////////////////////////////////////////////////////////////////////
		bool AddReadStream(size_t const nEventOpenID, IReadStreamPtr const pReadStream)
		{
			bool bSuccess = false;
			tMapReadStreams::iterator const IterOpenID(m_mapReadStreams.find(nEventOpenID));

			if (IterOpenID != m_mapReadStreams.end())
			{
				bSuccess = stl::push_back_unique(IterOpenID->second, pReadStream);
			}
			else
			{
				tVecReadStreams apReadStreams;
				apReadStreams.push_back(pReadStream);
				m_mapReadStreams.insert(std::make_pair(nEventOpenID, apReadStreams));
				bSuccess = true;
			}

			return bSuccess;
		}

		//////////////////////////////////////////////////////////////////////////
		bool RemoveReadStream(IReadStreamPtr const pReadStream)
		{
			bool bSuccess = false;

			tMapReadStreams::const_iterator const IterOpenIDEnd(m_mapReadStreams.end());

			for (tMapReadStreams::iterator IterOpenID(m_mapReadStreams.begin()); IterOpenID != IterOpenIDEnd; ++IterOpenID)
			{
				bSuccess = stl::find_and_erase(IterOpenID->second, pReadStream);

				if (bSuccess)
				{
					break;
				}
			}

			return bSuccess;
		}

		//////////////////////////////////////////////////////////////////////////
		bool AbortReadStreams(size_t const nEventOpenID)
		{
			bool bSuccess = false;

			if (nEventOpenID != 0)
			{
				tMapReadStreams::iterator const IterOpenID(m_mapReadStreams.find(nEventOpenID));

				if (IterOpenID != m_mapReadStreams.end())
				{
					tVecReadStreams::const_iterator const IterStreamsEnd(IterOpenID->second.end());

					for (tVecReadStreams::const_iterator IterStreams(IterOpenID->second.begin()); IterStreams != IterStreamsEnd; ++IterStreams)
					{
						(*IterStreams)->Abort();
					}

					m_mapReadStreams.erase(IterOpenID);
					bSuccess = true;
				}
			}
			else
			{
				tMapReadStreams::const_iterator const IterOpenIDEnd(m_mapReadStreams.end());

				for (tMapReadStreams::const_iterator IterOpenID(m_mapReadStreams.begin()); IterOpenID != IterOpenIDEnd; ++IterOpenID)
				{
					tVecReadStreams::const_iterator const IterStreamsEnd(IterOpenID->second.end());

					for (tVecReadStreams::const_iterator IterStreams(IterOpenID->second.begin()); IterStreams != IterStreamsEnd; ++IterStreams)
					{
						(*IterStreams)->Abort();
					}
				}

				// This is always a success.
				m_mapReadStreams.clear();
				bSuccess = true;
			}

			// Fail possible queued reads.
			FailQueuedReads(nEventOpenID);

			return bSuccess;
		}

		//////////////////////////////////////////////////////////////////////////
		void FailQueuedReads(size_t const nEventOpenID);

		//////////////////////////////////////////////////////////////////////////
		void Uncache()
		{
			// This instance must be unloaded from FMOD when getting uncached.
			assert((m_eAudioFileCacheState & eAFCS_PRELOADED) == 0);

			if (m_pReadStream)
			{
				m_pReadStream->Abort();
				m_pReadStream = NULL;
			}

			AbortReadStreams(0);

			m_pMemoryBlock         = NULL;
			m_pSoundBufferData     = NULL;
			m_eAudioFileCacheState = eAFCS_NOTCACHED;

			CFileCacheManager::m_nCurrentByteTotal -= m_nSizeCached;
			m_nSizeCached = 0;

			assert(m_nUsedCount == 0);
			m_nUsedCount = 0;

#ifdef INCLUDE_AUDIO_PRODUCTION_CODE
			m_nRequestedCount = 0;
			m_oTimeCached.SetValue(0);
#endif // INCLUDE_AUDIO_PRODUCTION_CODE
		}

		//////////////////////////////////////////////////////////////////////////
		void Reset()
		{
			// This instance must be unloaded from FMOD when getting reset.
			assert((m_eAudioFileCacheState & eAFCS_PRELOADED) == 0);

			m_sFileName.clear();
			m_nSizeFile            = 0;
			m_nSizeCached          = 0;
			m_nFileOffset          = 0;
			m_nUsedCount           = 0;
			m_pMemoryBlock         = NULL;
			m_eAudioFileCacheType  = eAFCT_ALL;
			m_eAudioFileCacheState = eAFCS_NOTFOUND;
			m_eStreamTaskType      = eStreamTaskTypeCount;
			m_pSoundBufferData     = NULL;

			// Don't abort any of the streams or reads below during Reset()! (container resize!!)
			// Just dereference this object and it will automatically abort and release all associated resources.
			m_pReadStream = NULL;
			m_mapReadStreams.clear();
			m_mapQueuedReads.clear();

#ifdef INCLUDE_AUDIO_PRODUCTION_CODE
			m_nRequestedCount = 0;
			m_oTimeCached.SetValue(0);
#endif // INCLUDE_AUDIO_PRODUCTION_CODE
		}

		typedef std::vector<void*>                 tVecQueuedReads;
		typedef VectorMap<size_t, tVecQueuedReads> tMapQueuedReads;

		typedef std::vector<IReadStreamPtr>        tVecReadStreams;
		typedef VectorMap<size_t, tVecReadStreams> tMapReadStreams;

		CryFixedStringT<AFCM_FILE_NAME_LENGTH> m_sFileName;
		size_t                                 m_nSizeFile;
		size_t                                 m_nSizeCached;
		size_t                                 m_nFileOffset;
		size_t                                 m_nUsedCount;
		_smart_ptr<ICustomMemoryBlock>         m_pMemoryBlock;
		EAudioFileCacheType                    m_eAudioFileCacheType;
		EAudioFileCacheState                   m_eAudioFileCacheState;
		EStreamTaskType                        m_eStreamTaskType;
		void*                                  m_pSoundBufferData;
		IReadStreamPtr                         m_pReadStream;          // Used for streaming the entire file.
		tMapReadStreams                        m_mapReadStreams;       // Holds read streams on the file from different events.
		tMapQueuedReads                        m_mapQueuedReads;

#ifdef INCLUDE_AUDIO_PRODUCTION_CODE
		size_t                                 m_nRequestedCount;
		CTimeValue                             m_oTimeCached;
#endif // INCLUDE_AUDIO_PRODUCTION_CODE
	};

	// Queued cache request struct.
	struct SQueuedCacheRequest
	{
		SQueuedCacheRequest()
			: m_nRequestFileOffset(0),
			  m_nRequestBytes(0){}

		SQueuedCacheRequest(size_t const nRequestFileOffset, int const nRequestBytes)
			:	m_nRequestFileOffset(nRequestFileOffset),
		    m_nRequestBytes(nRequestBytes){}

		size_t m_nRequestFileOffset;
		int    m_nRequestBytes;
	};

	// Internal type definitions.
	typedef VectorMap<CCryNameCRC, SFileCacheEntry, std::less<CCryNameCRC>, stl::STLGlobalAllocator<std::pair<CCryNameCRC, SFileCacheEntry> > >               tMapFileCacheEntries;
	typedef VectorMap<CCryNameCRC, SQueuedCacheRequest, std::less<CCryNameCRC>, stl::STLGlobalAllocator<std::pair<CCryNameCRC, SQueuedCacheRequest> > >       tMapQueuedCacheRequests;
	typedef VectorMap<IReadStreamPtr, int unsigned, std::less<IReadStreamPtr>, stl::STLGlobalAllocator<std::pair<IReadStreamPtr, int unsigned> > >            tMapQueuedStreams;
	typedef std::vector<CCryNameCRC, stl::STLGlobalAllocator<CCryNameCRC> > tVecFileCacheEntryCRCs;
	typedef VectorMap<CCryNameCRC, tVecFileCacheEntryCRCs, std::less<CCryNameCRC>, stl::STLGlobalAllocator<std::pair<CCryNameCRC, tVecFileCacheEntryCRCs> > > tMapAllGameHints;
	typedef std::vector<SAudioFileCacheManagerQueuedRequestInfo, stl::STLGlobalAllocator<CCryNameCRC> > TVecQueuedRequestInfos;

	// Unary predicate for conditional removing until our GCC supports C++11 to go lambda!
	static bool IsNotMusicFile(std::pair<CCryNameCRC, CFileCacheManager::SFileCacheEntry> const& rEntry)
	{
		return rEntry.second.m_eAudioFileCacheType != eAFCT_MUSIC;
	}

	// IStreamCallback class's overrides
	virtual void                  StreamAsyncOnComplete (IReadStream* pStream, unsigned int nError);
	virtual void                  StreamOnComplete (IReadStream* pStream, unsigned int nError){}

	// Internal methods
	void                          CacheFileInternal(CCryNameCRC const& rNameCRC, size_t const nRequestFileOffset = 0, int const nRequestBytes = -1, bool const bRetriedCacheRequest = false);
	bool                          UncacheFileCacheEntryInternal(SFileCacheEntry& rFileCacheEntry, bool const bNow, bool const bIgnoreUsedCount = false);
	bool                          UncacheFileCacheEntryByCRCInternal(CCryNameCRC const& rNameCRC, bool const bNow, bool const bIgnoreUsedCount = false);
	void                          ParseForGameHintsInternal(XmlNodeRef const pNodeGameHints);
	void                          CacheGameHintInternal(CCryNameCRC const& rNameCRC);
	bool                          DoesRequestFitInternal(size_t const nRequestSize);
	bool                          IsPreloadableInternal(SFileCacheEntry const& rFileCacheEntry) const;
	void                          UpdatePatchRequestsInternal();
	bool                          PreloadEntryIntoFMODInternal(CCryNameCRC const& rNameCRC);
	bool                          UnloadEntryFromFMODInternal(CCryNameCRC const& rNameCRC);
	bool                          QueueFileCacheEntryForPreloadInternal(SFileCacheEntry& rFileCacheEntry);
	bool                          QueueFileCacheEntryForUnloadInternal(SFileCacheEntry& rFileCacheEntry, bool const bUncache);
	bool                          FinishStreamInternal(IReadStreamPtr const pStream, int unsigned const nError, bool const bReadDirectly);
	bool                          AllocateMemoryBlockInternal(SFileCacheEntry& rFileCacheEntry);
	void                          ProcessQueuedRequests();
	void                          FinishQueuedReads(SFileCacheEntry const& rFileCacheEntry);

	// Internal members
	CryCriticalSection            m_oKnownFilesCS;
	CryCriticalSection            m_oQueuedPreloadUnloadEntriesCS;
	CryCriticalSection            m_oQueuedCacheRequestsCS;
	CryCriticalSection            m_oQueuedStreamsCS;
	CryCriticalSection            m_oQueuedRequestInfosCS;
  CryCriticalSection            m_oFileReadCS;
	tMapFileCacheEntries          m_mapKnownFiles;
	tMapAllGameHints              m_mapAllFoundGameHints;
	tMapQueuedCacheRequests       m_mapQueuedCacheRequests;
	tMapQueuedStreams             m_mapQueuedStreams;
	tVecFileCacheEntryCRCs        m_aoQueuedPreloadEntries;
	tVecFileCacheEntryCRCs        m_aoQueuedUnloadEntries;
	TVecQueuedRequestInfos        m_aoQueuedRequestInfos;
	_smart_ptr<CCustomMemoryHeap> m_pMemoryHeapMain;
	_smart_ptr<CCustomMemoryHeap> m_pMemoryHeapExtended;
	static size_t                 m_nCurrentByteTotal;
	static size_t                 m_nMaxByteTotal;
	static size_t                 m_nMaxByteMain;
	static size_t                 m_nMaxByteExtended;
	static size_t                 m_nMaxByteAdditional;
	bool                          m_bCanCacheEntries;
}; 

#endif // FILE_CACHE_MANAGER_H_INCLUDED
