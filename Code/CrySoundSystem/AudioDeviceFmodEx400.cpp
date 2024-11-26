////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   AudioDeviceFmodEx400.cpp
//  Version:     v1.00
//  Created:     8/6/2005 by Tomas
//  Compilers:   Visual Studio.NET
//  Description: FmodEx 4.00 Implementation of a Audio Device
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#ifdef SOUNDSYSTEM_USE_FMODEX400

#include <CryThread.h>
#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/CrySystem/ICryPak.h" 
#include "CryCommon/CryCore/CrySizer.h"
#include "CryCommon/CryRenderer/IRenderer.h"
#include "CryCommon/CrySystem/ITimer.h"
#include "IAudioDevice.h"
#include "AudioDeviceFmodEx400.h"
#include "SoundBufferFmodEx400.h"
#include "PlatformSoundFmodEx400.h"
#include "SoundBufferFmodEx400Event.h"
#include "PlatformSoundFmodEx400Event.h"
#include "SoundBufferFmodEx400Micro.h"
#include "SoundBufferFmodEx400Network.h"
#include "CryCommon/CrySoundSystem/ISound.h"
#include "Sound.h"
#include "SoundSystem.h"
#include "FmodEx/inc/fmod_errors.h"
#include "FmodEx/inc/fmod_dsp.h"
#include "FmodEx/inc/fmod.hpp"
#include "FmodEx/inc/fmod_event.hpp"
#include "FmodEx/inc/fmod_event_net.hpp"
#include "SoundSystemCommon.h"
#include "CryCommon/CrySoundSystem/ISoundMoodManager.h"
#include "CryCommon/CrySoundSystem/IReverbManager.h"
//#include "CryMemoryManager_impl.h"
//static uint32 nSoundMainThreadId = ~0;
static int32 s_nFileOpenCount = 0; // open file handle counting through FMOD filesystem callback
unsigned int CAudioDeviceFmodEx400::s_SoundFileIndex = 0;
tSoundFileMap CAudioDeviceFmodEx400::s_SoundFileMap;
//
//#if defined(PS3)
//#include <sysutil/sysutil_sysparam.h>
//#endif

#if defined(USE_RSX_MEM)
	unsigned int PS3_MEM_FLAG = 0;
#endif






//#include <windows.h> // used for get currentthread streaming
//////////////////////////////////////////////////////////////////////////
#define IS_FMODERROR (m_ExResult != FMOD_OK )
// this starts off not active
#define GETMEMORYSTATSEVERY 1000
// ****OPTIONAL**** set the size of the mixing buffer in milliSeconds 
#define FMOD_MIXING_BUFF_SIZE 200 // mixing buffer size in millaseconds ** NOT USED AT THIS TIME ******
 
CFrameProfiler* CAudioDeviceFmodEx400::s_pFrameProfiler_API_Info;
CFrameProfiler* CAudioDeviceFmodEx400::s_pFrameProfiler_API_Other;
CFrameProfiler* CAudioDeviceFmodEx400::s_pFrameProfiler_Buffer_Other; 
CFrameProfiler* CAudioDeviceFmodEx400::s_pFrameProfiler_Buffer_Release; 
CFrameProfiler* CAudioDeviceFmodEx400::s_pFrameProfiler_Event_Other;

// debugging memory crash

#ifdef FMOD_MEMORY_DEBUGGING
CAudioDeviceFmodEx400::tMemMap CAudioDeviceFmodEx400::MemMap;
CDebugLogger CAudioDeviceFmodEx400::MemLogger("FMOD_Memory.log");
char	CAudioDeviceFmodEx400::MemString[128];
static void *m_CS = NULL; // Critical section pointer.

class CSmartCriticalSection
{
private:
	void *m_pCS;
public:
	CSmartCriticalSection( void *cs ) { m_pCS = cs; CryEnterCriticalSection(m_pCS); }
	~CSmartCriticalSection() { CryLeaveCriticalSection(m_pCS); }
};

void CAudioDeviceFmodEx400::TraceMemory(void *ptr, int nSize, int nMemMode)
{

	CSmartCriticalSection SmartCriticalSection(m_CS);

	tMemMap::iterator It = MemMap.find(ptr);

	switch(nMemMode)
	{
	case 1:	// alloc
				 {
					 if (It == MemMap.end())
					 {
						 sprintf_s(MemString,"Alloc   : %d Size: %d", ptr, nSize);
						 MemLogger.LogString(MemString,true);
						 MemMap[ptr] = nSize;
					 }
					 else
					 {
						 sprintf_s(MemString,"Alloc   : %d Size: %d - Already found!", ptr, nSize);
						 MemLogger.LogString(MemString,true);
					 }
				 }
				 break;
	case 2: 	// free
				 {
					 if (It == MemMap.end())
					 {
						 sprintf(MemString,"Free    : %d Size: %d - Not found!", ptr, nSize);
						 MemLogger.LogString(MemString,true);
					 }
					 else
					 {
						 sprintf(MemString,"Free    : %d Size: %d", ptr, nSize);
						 MemLogger.LogString(MemString,true);
						 MemMap.erase(It);			
					 }
				 }
				 break;
	default:
		break;
	}
}

#endif

#ifdef FMOD_MEMORY_DEBUGGING_SIMPLE
uint32 CAudioDeviceFmodEx400::nFMODMemUsage = 0;
uint32 CAudioDeviceFmodEx400::nFMODMemTotalSmallAllocCount = 0;
uint32 CAudioDeviceFmodEx400::nFMODMemTotalLargeAllocCount = 0;
uint32 CAudioDeviceFmodEx400::nFMODMemActiveAllocCount = 0;
#endif

#ifdef FMOD_STREAMING_DEBUGGING
//uint32 CAudioDeviceFmodEx400::nFMODFileOpenCount = 0;
uint32 CAudioDeviceFmodEx400::s_nFMODFileSeekCount = 0;
uint32 CAudioDeviceFmodEx400::s_nFMODFileReadBandwidth = 0;
#endif

#include "CryMemoryAllocator.h" // [saschad] Required in case NOT_USE_CRY_MEMORY_MANAGER is defined

// [Alexey] I found biggest problems with WriteLocks in CryMalloc because of tons of FMOD allocations
// Ive decided made a special fmod allocator (thread safe) and after that this problem disappeared
// We still have a statistics because node_allocator uses CryMalloc with proper statistics counting
extern "C" 
{
#if !defined(PS3)
	node_alloc<eCryDefaultMalloc, true, 524288> g_Allocator;
#endif
	//////////////////////////////////////////////////////////////////////////
	static void * F_CALLBACK CrySound_Alloc (unsigned int size, FMOD_MEMORY_TYPE memtype)
	{

#ifdef FMOD_MEMORY_DEBUGGING
		CAudioDeviceFmodEx400::TraceMemory(ptr, size, 1);
#endif

		// On PS3, malloc is wrapped and forwards to a global node allocator
		// so use this one to prevent allocated but unused chunks in both node allocators



		void* ptr = g_Allocator.alloc(size);


#ifdef FMOD_MEMORY_DEBUGGING_SIMPLE




		size_t nSize = g_Allocator.getSizeEx(ptr); 


		if (!nSize)
		{



			nSize = CryCrtSize(ptr);

			++CAudioDeviceFmodEx400::nFMODMemTotalLargeAllocCount;
		}
		else
		{
			++CAudioDeviceFmodEx400::nFMODMemTotalSmallAllocCount;
		}

		CAudioDeviceFmodEx400::nFMODMemUsage += nSize;
		
		++CAudioDeviceFmodEx400::nFMODMemActiveAllocCount;
#endif

		return ptr;
	}

	//////////////////////////////////////////////////////////////////////////
	static void F_CALLBACK CrySound_Free (void* ptr, FMOD_MEMORY_TYPE memtype)
	{

#ifdef FMOD_MEMORY_DEBUGGING_SIMPLE




		size_t nSize = g_Allocator.getSizeEx(ptr); 

		if (!nSize) 
			nSize = CryCrtSize(ptr);


		CAudioDeviceFmodEx400::nFMODMemUsage -= nSize;
		--CAudioDeviceFmodEx400::nFMODMemActiveAllocCount;
#endif

		// On PS3, malloc is wrapped and forwards to a global node allocator
		// so use this one to prevent allocated but unused chunks in both node allocators



		unsigned size = g_Allocator.deallocate(ptr);


#ifdef FMOD_MEMORY_DEBUGGING
		CAudioDeviceFmodEx400::TraceMemory(ptr, size, 2);
#endif

	}

	//////////////////////////////////////////////////////////////////////////
	static void * F_CALLBACK CrySound_Realloc (void *ptr, unsigned int size, FMOD_MEMORY_TYPE memtype)
	{

#ifdef FMOD_MEMORY_DEBUGGING
		void *pRealloc = NULL;
		unsigned int nOldsize = _msize( ptr );
		CAudioDeviceFmodEx400::TraceMemory(ptr, nOldsize, 2);
#endif

		// On PS3, malloc is wrapped and forwards to a global node allocator
		// so use this one to prevent allocated but unused chunks in both node allocators






		void * np = g_Allocator.alloc(size);
		size_t oldsize = g_Allocator.getSize(ptr);
		memmove(np, ptr, oldsize);
		g_Allocator.dealloc(ptr);


#ifdef FMOD_MEMORY_DEBUGGING_SIMPLE
		CAudioDeviceFmodEx400::nFMODMemUsage = CAudioDeviceFmodEx400::nFMODMemUsage - oldsize + size;
		
		if (size > 512)
			++CAudioDeviceFmodEx400::nFMODMemTotalLargeAllocCount;
		else
			++CAudioDeviceFmodEx400::nFMODMemTotalSmallAllocCount;
#endif

#ifdef FMOD_MEMORY_DEBUGGING
		CAudioDeviceFmodEx400::TraceMemory(pRealloc, size, 1);
#endif

		return np;
	}
}


//////////////////////////////////////////////////////////////////////////
// File callbacks for FMODEX.
//////////////////////////////////////////////////////////////////////////
	//typedef FMOD_RESULT (F_CALLBACK *FMOD_FILE_OPENCALLBACK)     (const char *name, int unicode, unsigned int *filesize, void **handle, void **userdata);
	//typedef FMOD_RESULT (F_CALLBACK *FMOD_FILE_CLOSECALLBACK)    (void *handle, void *userdata);
	//typedef FMOD_RESULT (F_CALLBACK *FMOD_FILE_READCALLBACK)     (void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata);
	//typedef FMOD_RESULT (F_CALLBACK *FMOD_FILE_SEEKCALLBACK)     (void *handle, unsigned int pos, void *userdata);

void CFMODFileStreamCallback::StreamAsyncOnComplete(IReadStream *pStream, unsigned nError)
{
	void* nFile = (void*) pStream->GetUserData();
	tSoundFileMap::iterator It = CAudioDeviceFmodEx400::s_SoundFileMap.find(nFile);

	unsigned int nBytesRead = 0;

	if (It != CAudioDeviceFmodEx400::s_SoundFileMap.end())
	{
		if (nError)
		{
			if (nError != ERROR_USER_ABORT)
			{
				assert(!"FMOD file read failed!");
			}
		}
		else
			nBytesRead = pStream->GetBytesRead();

		(*It).second.pReadStream = NULL;
		(*It).second.nBytesReady = nBytesRead;
	}
}

void CFMODFileStreamCallback::StreamOnComplete (IReadStream* pStream, unsigned nError)
{
	// leave empty because FMOD Streaming thread is controlling this, not the Main thread
}

	FMOD_RESULT F_CALLBACK FMODFileOpenCallback(const char *name, int unicode, unsigned int *filesize, void **handle, void **userdata)
	{
		CSoundSystem* pSoundSystem = (CSoundSystem*) gEnv->pSoundSystem;

		assert(name);
		assert(filesize);
		assert(handle);

		char szPath[512];
		unsigned int nFileSize = gEnv->pCryPak->FGetSize(name);
		
		if (nFileSize == 0)
		{
			// FMOD workaround START
			// Iterate through all known Projects and create correct path on demand 
			// as long as FMOD can not track individual FEV Paths internaly if the file is loaded from memory
			CAudioDeviceFmodEx400* pAudioDevice = (CAudioDeviceFmodEx400*)gEnv->pSoundSystem->GetIAudioDevice();

			CAudioDeviceFmodEx400::VecProjectFilesIter IterEnd = pAudioDevice->m_VecLoadedProjectFiles.end();
			for (CAudioDeviceFmodEx400::VecProjectFilesIter Iter = pAudioDevice->m_VecLoadedProjectFiles.begin(); Iter!=IterEnd; ++Iter)
			{
				int nPosLastSlash = (*Iter).sProjectName.rfind('/', (*Iter).sProjectName.size());
				string sPath = (*Iter).sProjectName.substr(0, (*Iter).sProjectName.size() +2 - nPosLastSlash);

				sprintf_s(szPath, "%s/%s", sPath.c_str(), name);
				nFileSize = gEnv->pCryPak->FGetSize(szPath);

				// FMOD workaround START
				if (nFileSize > 0)
					break; // leave the loop
			}

			if (nFileSize == 0)
				return FMOD_ERR_FILE_NOTFOUND;	
		}

		// raw filesystem mapping
		if (pSoundSystem->g_nFileAccess == 0)
		{
			// File is opened for streaming.
			// rbx open flags, x is a hint to not cache whole file in memory.
			FILE *file = gEnv->pCryPak->FOpen( szPath,"rbx",ICryPak::FOPEN_HINT_DIRECT_OPERATION );

			if (!file)
				return FMOD_ERR_FILE_NOTFOUND;	

			*handle		= file;
			*filesize = nFileSize;
			++s_nFileOpenCount;

			return FMOD_OK;
		}

		// Streaming Engine mapping
		if (pSoundSystem->g_nFileAccess == 1)
		{
			++CAudioDeviceFmodEx400::s_SoundFileIndex;
			sSoundFileEntry NewFile;
			NewFile.sFilename = szPath;
			NewFile.nOffsetInFile = 0;
			NewFile.nFileSize = nFileSize;
			NewFile.nBytesReady = 0;
			NewFile.pReadStream = NULL;

			if (NewFile.nFileSize)
			{
				CAudioDeviceFmodEx400::s_SoundFileMap.insert(std::pair<void*, sSoundFileEntry>((void*)CAudioDeviceFmodEx400::s_SoundFileIndex, NewFile));
				*handle = (void*)CAudioDeviceFmodEx400::s_SoundFileIndex;
				*filesize = NewFile.nFileSize;
				return FMOD_OK;
			}
			else
				return FMOD_ERR_FILE_NOTFOUND;

		}
		return FMOD_ERR_FILE_NOTFOUND;
	}

	FMOD_RESULT F_CALLBACK FMODFileCloseCallback(void *nFile, void *userdata)
	{
		CSoundSystem* pSoundSystem = (CSoundSystem*) gEnv->pSoundSystem;

		// raw filesystem mapping
		if (pSoundSystem->g_nFileAccess == 0)
		{
			assert(nFile);
			if (nFile)
			{
				FILE *file = (FILE*)nFile;

				if (!gEnv->pCryPak->FClose( file ))
				{
					--s_nFileOpenCount;
					return FMOD_OK;
				}
			}

			return FMOD_ERR_FILE_NOTFOUND;
		}

		// Streaming Engine mapping
		if (pSoundSystem->g_nFileAccess == 1)
		{
			tSoundFileMap::iterator It = CAudioDeviceFmodEx400::s_SoundFileMap.find(nFile);

			if (It != CAudioDeviceFmodEx400::s_SoundFileMap.end())
			{
				if ((*It).second.pReadStream)
					(*It).second.pReadStream->Abort();

				(*It).second.pReadStream = NULL;
				CAudioDeviceFmodEx400::s_SoundFileMap.erase(It);
			}

			return FMOD_OK;
		}
		
		return FMOD_ERR_FILE_NOTFOUND;
	}

	//static FMOD_RESULT F_CALLBACK CrySoundEx_fread(void *nFile, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata)
	FMOD_RESULT F_CALLBACK FMODFileReadCallback(void *nFile, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata)
	{
		CSoundSystem* pSoundSystem = (CSoundSystem*) gEnv->pSoundSystem;

		// raw filesystem mapping
		if (pSoundSystem->g_nFileAccess == 0)
		{
			assert(nFile);
			assert(buffer);
			assert(sizebytes);
			assert(bytesread);
			FILE *file = (FILE*)nFile;

			uint32 nBytesRead = gEnv->pCryPak->FReadRaw( buffer,1,sizebytes,file );
			*bytesread = nBytesRead;

#ifdef FMOD_STREAMING_DEBUGGING
			CAudioDeviceFmodEx400::s_nFMODFileReadBandwidth += nBytesRead;
#endif

			if (nBytesRead != sizebytes)
				return FMOD_ERR_FILE_EOF;

			return FMOD_OK;
		}

		// Streaming Engine mapping
		if (pSoundSystem->g_nFileAccess == 1)
		{
			tSoundFileMap::iterator It = CAudioDeviceFmodEx400::s_SoundFileMap.find(nFile);

			if (It != CAudioDeviceFmodEx400::s_SoundFileMap.end())
			{
				sSoundFileEntry* pEntry = &(*It).second;
				if (pEntry->nBytesReady == 0)
				{
					StreamReadParams StrParams;
					StrParams.nOffset = pEntry->nOffsetInFile;
					StrParams.nFlags = 0;
					StrParams.dwUserData = (DWORD_PTR)nFile;
					StrParams.nLoadTime = 0;
					StrParams.nMaxLoadTime = 0;
					StrParams.nPriority = 0;
					StrParams.pBuffer = buffer;
					StrParams.nSize = min(pEntry->nFileSize - pEntry->nOffsetInFile, sizebytes);

					// catch EOF for debugging, will be removed
					if (StrParams.nSize < sizebytes)
						int Catchme = 0;

					pEntry->pReadStream = gEnv->pSystem->GetStreamEngine()->StartRead(eStreamTaskTypeSound, pEntry->sFilename.c_str(), &s_FMODFileCallback, &StrParams);

					if (true)
						pEntry->pReadStream->Wait();

				}

				if (pEntry->nBytesReady > 0)
				{
					unsigned int nBytesRead = pEntry->nBytesReady;
					*bytesread = nBytesRead;
					pEntry->nBytesReady = 0;
					pEntry->nOffsetInFile += nBytesRead;

#ifdef FMOD_STREAMING_DEBUGGING
					CAudioDeviceFmodEx400::s_nFMODFileReadBandwidth += nBytesRead;
#endif

					if (nBytesRead != sizebytes)
						return FMOD_ERR_FILE_EOF;

					return FMOD_OK;
				}
				return FMOD_ERR_FILE_EOF;
			}

		}
		return FMOD_ERR_FILE_NOTFOUND;
	}

	FMOD_RESULT F_CALLBACK FMODFileSeekCallback(void *nFile, unsigned int pos, void *userdata)
	{
		CSoundSystem* pSoundSystem = (CSoundSystem*) gEnv->pSoundSystem;

#ifdef FMOD_STREAMING_DEBUGGING
		++CAudioDeviceFmodEx400::s_nFMODFileSeekCount;
#endif

		// raw filesystem mapping
		if (pSoundSystem->g_nFileAccess == 0)
		{
			assert(nFile);
			FILE *file = (FILE*)nFile;
			// mode not supplied
			int mode = 0;

			if (gEnv->pCryPak->FSeek( file,pos,mode ))
				return FMOD_ERR_FILE_COULDNOTSEEK;
			else
				return FMOD_OK;
		}

		// Streaming Engine mapping
		if (pSoundSystem->g_nFileAccess == 1)
		{
			CAudioDeviceFmodEx400::s_SoundFileMap[nFile].nOffsetInFile = pos;
			return FMOD_OK;
		}

		return FMOD_ERR_FILE_COULDNOTSEEK;
	}

	FMOD_RESULT F_CALLBACK CAudioDeviceFmodEx400::FMODThreadCallback(FMOD_SYSTEM *system, FMOD_SYSTEM_CALLBACKTYPE type, void* commanddata1, void* commanddata2)
	{
		FMOD::System *sys = (FMOD::System *)system;

		switch (type)
		{
		case FMOD_SYSTEM_CALLBACKTYPE_DEVICELISTCHANGED:
			{
				int numdrivers;

				//printf("NOTE : FMOD_SYSTEM_CALLBACKTYPE_DEVICELISTCHANGED occured.\n");

				sys->getNumDrivers(&numdrivers);

				//printf("Numdevices = %d\n", numdrivers);
				if (gEnv && gEnv->pLog)
				{
					gEnv->pLog->Log("<Sound> Sound-FmodEx-AudioDevice Device List changed: (%d) \n", numdrivers);
				}
				break;
			}
		case FMOD_SYSTEM_CALLBACKTYPE_MEMORYALLOCATIONFAILED:
			{
				if (gEnv && gEnv->pLog)
				{
					const char* sMemInfo = (const char*)commanddata1;
					int nBytes = (int)commanddata2;
					gEnv->pLog->Log("<Sound> Sound-FmodEx-AudioDevice Memory Allocation failed: %s %d bytes \n", sMemInfo, nBytes);
				}
				//printf("ERROR : FMOD_SYSTEM_CALLBACKTYPE_MEMORYALLOCATIONFAILED occured.\n");
				//printf("%s.\n", commanddata1);
				//printf("%d bytes.\n", commanddata2);
				break;
			}
		case FMOD_SYSTEM_CALLBACKTYPE_THREADCREATED:
			{
				int nThreadID = (int)commanddata1;
				const char* sThreadName = (const char*)commanddata2;

				if (gEnv && gEnv->pSystem && gEnv->pLog)
				{
					gEnv->pSystem->GetIThreadTaskManager()->SetThreadName( nThreadID, sThreadName );
					gEnv->pLog->Log("<Sound> Sound-FmodEx-AudioDevice Thread created: %s (%d) \n", sThreadName, nThreadID);
				}

				//THREAD_HANDLE hThreadHandle = OpenThread(0, 0, nThreadID);
				//int threadPriority = GetThreadPriority( hThreadHandle );
				//CloseHandle(hThreadHandle);

				//SetThreadPriority( nThreadID, THREAD_PRIORITY_TIME_CRITICAL );
				break;
			}
		case FMOD_SYSTEM_CALLBACKTYPE_BADDSPCONNECTION:
			{
				FMOD::DSP *source = (FMOD::DSP *)commanddata1;
				FMOD::DSP *dest = (FMOD::DSP *)commanddata2;

				char sSource[256];
				char sDest[256];
				
				if (source)
					source->getInfo(sSource, 0,0,0,0);

				if (dest)
					dest->getInfo(sDest, 0,0,0,0);

				if (gEnv && gEnv->pLog)
					gEnv->pLog->Log("<Sound> Sound-FmodEx-AudioDevice bad DSP Connection occured: Source: %s Dest: %s \n", sSource, sDest);

				break;
			}
		}

		return FMOD_OK;



	}

//static int F_CALLBACKAPI CrySound_ftell( void * nFile )
//  {
//  FILE *file = (FILE*)nFile;
//  return gEnv->pCryPak->FTell( file );
//  }


	void CAudioDeviceFmodEx400::FmodErrorOutput(const char * sDescription, ESoundLogType LogType) const
	{
		gEnv->pSoundSystem->Log(LogType, "<Sound> FmodEx-AudioDevice: %s (%d) %s\n", sDescription, m_ExResult, FMOD_ErrorString(m_ExResult));
	}

void AddPhysicalBlock(long size);

CAudioDeviceFmodEx400::CAudioDeviceFmodEx400(void * hWnd)
{
  //GUARD_HEAP;

#ifdef FMOD_MEMORY_DEBUGGING
	m_CS = CryCreateCriticalSection();
#endif

	m_ExResult					= FMOD_OK;
	m_nSpeakerMode			= FMOD_SPEAKERMODE_RAW;
	m_pEventSystem			= NULL;
	m_pSoundSystem			= NULL;
	m_pAPISystem				= NULL;
	m_pAPISystemDSound	= NULL;
	m_pEventDumpLogger	= NULL;
	m_pCacheStaticMemBlock = NULL;
	m_nCacheStaticMemBlockSize = 0;




	m_tLastUpdate				= gEnv->pTimer->GetFrameStartTime();

#ifdef FMOD_STREAMING_DEBUGGING
	m_tLastBandwidthTime = gEnv->pTimer->GetFrameStartTime();
	//g_nFMODFileSeekCount = 0;
	m_fStreamKBPerSecond = 0.0f;
	//m_nFMODFileSeekCount = 0;
	//g_nFMODFileReadBandwidth = 0;
#endif
	//g_SoundFileIndex = 0;

//	nSoundMainThreadId = CryGetCurrentThreadId();

	s_pFrameProfiler_API_Info = new CFrameProfiler( gEnv->pSystem, "FMOD API Info", PROFILE_SOUND ); 
	s_pFrameProfiler_API_Other = new CFrameProfiler( gEnv->pSystem, "FMOD API Other", PROFILE_SOUND ); 
	s_pFrameProfiler_Buffer_Other = new CFrameProfiler( gEnv->pSystem, "FMOD Buffer Other", PROFILE_SOUND ); 
	s_pFrameProfiler_Buffer_Release = new CFrameProfiler( gEnv->pSystem, "FMOD Buffer Release", PROFILE_SOUND ); 
	s_pFrameProfiler_Event_Other = new CFrameProfiler( gEnv->pSystem, "FMOD Event Other", PROFILE_SOUND ); 


	int nFMODCurrent = 0;
	int nMax = 0;
	m_ExResult = FMOD::Memory_GetStats(&nFMODCurrent, &nMax);











	// set auto values for PC
	g_nInternCacheStatic = 0; // off on PC by default
	g_fInternCacheSize = 60;


	//m_ExResult = FMOD::System_SetDebugMode(FMOD::DEBUG_FILE);
	//m_ExResult = FMOD::System_SetDebugLevel(FMOD::LOG_NONE);

	ICVar* pCacheStatic = gEnv->pConsole->GetCVar("s_CacheStatic");
	ICVar* pCacheSize = gEnv->pConsole->GetCVar("s_CacheSize");

	float fCacheSizeCVar = pCacheSize ? pCacheSize->GetFVal() : 0.0f;
	bool bCacheStaticCVar = pCacheStatic ? (pCacheStatic->GetIVal() == 1 ) : false;

	m_InitSettings.fCacheSize = (fCacheSizeCVar > 0) ? fCacheSizeCVar : g_fInternCacheSize;
	m_InitSettings.bCacheStatic = (bCacheStaticCVar) ? bCacheStaticCVar : (g_nInternCacheStatic == 1);

	// Creating static memory blocks for XBOX360, PS3 or PC
	if (m_InitSettings.bCacheStatic && !m_pCacheStaticMemBlock)
	{
		MEMSTAT_CONTEXT(EMemStatContextTypes::MSC_SoundProject, 0, "Sound cache");
		m_nCacheStaticMemBlockSize = (uint32)(m_InitSettings.fCacheSize*1024*1024);




		// PC or PS3 static main memory block
		m_pCacheStaticMemBlock = new int8[ m_nCacheStaticMemBlockSize ];


		//memset(m_pCacheStaticMemBlock, 0, m_nCacheStaticMemBlockSize);

		m_ExResult = FMOD::Memory_Initialize(m_pCacheStaticMemBlock, m_nCacheStaticMemBlockSize, 0, 0, 0);

		// Added statistics for static memory block
		AddPhysicalBlock(m_nCacheStaticMemBlockSize);
		CryLogAlways("<Sound> Initialize FMOD with static memory block of %.0f\n", m_InitSettings.fCacheSize);
	}
	else
	{
		CryLogAlways("<Sound> Initialize FMOD with dynamic memory callbacks\n");
		m_ExResult = FMOD::Memory_Initialize(0, 0, CrySound_Alloc, CrySound_Realloc, CrySound_Free);
	}

	if (IS_FMODERROR)
	{
		FmodErrorOutput("memory initialization failed! ", eSLT_Error);
	}

	m_ExResult = FMOD::Memory_GetStats(&nFMODCurrent, &nMax);

	m_InitSettings.nMaxHWchannels = 0;
	m_InitSettings.nMinHWChannels = 0;
	m_InitSettings.nSoftwareChannels = 0;
	m_InitSettings.nMPEGDecoders = 0;
	m_InitSettings.nXMADecoders = 0;
	m_InitSettings.nADPCMDecoders = 0;

  //CS_SetHWND(hWnd);
	m_nCurrentMemAlloc = 0;
	m_nMaxMemAlloc		 = 0;
//	m_nEaxStatusFMOD	 = 0;
	
	m_nHardware2DChannels = 0;
	m_nHardware3DChannels = 0;
	m_nTotalHardwareChannelsAvail = 0;

  // 0 - 1, 1 being max volume
  m_fMasterVolume = 1.0f;
	m_nFMODDriverCaps = 0;
	m_nCountProject = 0;
	m_nCountEvent = 0;
	m_nCountGroup = 0;
	s_nFileOpenCount = 0;

  // store active window
  m_pHWnd = hWnd; 

	m_ExResult = FMOD::EventSystem_Create(&m_pEventSystem);
	if (IS_FMODERROR)
	{
		FmodErrorOutput("event system failed! ", eSLT_Error);
		return;
	}

	// have to do it again
	FMOD_DEBUGLEVEL nLevel = FMOD_DEBUG_LEVEL_NONE;
	//nLevel = SOUNDSYSTEM_DEBUG_FMOD_ALL;
	m_ExResult = FMOD::Debug_SetLevel(nLevel);

	// Get FMOD-Ex System Ptr from EventSystem
	m_ExResult = m_pEventSystem->getSystemObject(&m_pAPISystem);
	if (IS_FMODERROR)
	{
		FmodErrorOutput("get system object failed! ", eSLT_Error);
		return;
	}

}








































































CAudioDeviceFmodEx400::~CAudioDeviceFmodEx400()
{
	ShutDownDevice();

	IWavebank *pWavebank = NULL;
	tmapWavebanks::iterator ItEnd = m_Wavebanks.end();
	for (tmapWavebanks::const_iterator It = m_Wavebanks.begin(); It!=ItEnd; ++It)
	{
		pWavebank = (*It).second;
		delete pWavebank;
	}

	int nEnd = m_UnusedPlatformSounds.size();
	for (int i=0; i<nEnd; ++i)
		delete m_UnusedPlatformSounds[i];

	nEnd = m_UnusedPlatformSoundEvents.size();
	for (int i=0; i<nEnd; ++i)
		delete m_UnusedPlatformSoundEvents[i];

	delete s_pFrameProfiler_API_Info; 
	delete s_pFrameProfiler_API_Other; 
	delete s_pFrameProfiler_Buffer_Other; 
	delete s_pFrameProfiler_Buffer_Release; 
	delete s_pFrameProfiler_Event_Other; 

#ifdef FMOD_MEMORY_DEBUGGING
	CryDeleteCriticalSection(m_CS);
	m_CS = NULL;
#endif

}

bool CAudioDeviceFmodEx400::InitDevice(CSoundSystem* pSoundSystem) 
{
  //GUARD_HEAP;

	if (!pSoundSystem) // check for valid system
		return (false);













	m_pSoundSystem = pSoundSystem;
  bool bTemp = true;
  m_nMemoryStatInc = 0;

  // system starts out not muted
  m_bMuteStatus = false;

  // make true when have new attributes for listener
  m_bHaveListenerAttributes = false;

  m_bSystemPaused = false; // starts out not paused
    
  // gets system stats when turned on
  m_bGetSystemStats = false;

	if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
	{
		if (!m_CommandPlayer.IsRecording())
			m_CommandPlayer.RecordFile("SoundCommands.xml");

		m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::EVENTSYSTEM_CREATE, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds());
	}

	// 0 means <auto> values should be used
	m_InitSettings.nMPEGDecoders	= m_pSoundSystem->g_nMPEGDecoders ? m_pSoundSystem->g_nMPEGDecoders : m_pSoundSystem->g_nInternMPEGDecoders;
	m_InitSettings.nADPCMDecoders = m_pSoundSystem->g_nADPCMDecoders ? m_pSoundSystem->g_nADPCMDecoders : m_pSoundSystem->g_nInternADPCMDecoders;
	m_InitSettings.nXMADecoders		= m_pSoundSystem->g_nXMADecoders ? m_pSoundSystem->g_nXMADecoders : m_pSoundSystem->g_nInternXMADecoders;

	//m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::EVENTSYSTEM_GETSYSTEMOBJECT, gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), m_pEventSystem, m_pAPISystem);

	// memory debugging
	//m_ExResult = m_pEventSystem->release();
	//if (IS_FMODERROR)
	//{
	//	FmodErrorOutput("event system release failed! ", eSLT_Error);
	//	bTemp = false;
	//}


	//m_ExResult = m_pEventSystem->release();
	//if (IS_FMODERROR)
	//{
	//	FmodErrorOutput("event system release failed! ", eSLT_Error);
	//	bTemp = false;
	//}

	// if not already there, create again
	if (!m_pEventSystem)
	{
		int nFMODCurrent = 0;
		int nMax = 0;
		m_ExResult = FMOD::Memory_GetStats(&nFMODCurrent, &nMax);

		ICVar* pCacheStatic = gEnv->pConsole->GetCVar("s_CacheStatic");
		ICVar* pCacheSize = gEnv->pConsole->GetCVar("s_CacheSize");

		float fCacheSizeCVar = pCacheSize ? pCacheSize->GetFVal() : 0.0f;
		bool bCacheStaticCVar = pCacheStatic ? (pCacheStatic->GetIVal() == 1 ) : false;

		m_InitSettings.fCacheSize = (fCacheSizeCVar > 0) ? fCacheSizeCVar : g_fInternCacheSize;
		m_InitSettings.bCacheStatic = (bCacheStaticCVar) ? bCacheStaticCVar : (g_nInternCacheStatic == 1);

		if (m_InitSettings.bCacheStatic && !m_pCacheStaticMemBlock)
		{
			m_nCacheStaticMemBlockSize = (uint32)(m_InitSettings.fCacheSize*1024*1024);




			m_pCacheStaticMemBlock = new int8[ m_nCacheStaticMemBlockSize ];


			m_ExResult = FMOD::Memory_Initialize(m_pCacheStaticMemBlock, m_nCacheStaticMemBlockSize, 0, 0, 0);

			// Added statistics for static memory block
			AddPhysicalBlock(m_nCacheStaticMemBlockSize);
			CryLogAlways("<Sound> Initialize FMOD with static memory block of %.0f\n", m_InitSettings.fCacheSize);
		}
		else
		{
			CryLogAlways("<Sound> Initialize FMOD with dynamic memory callbacks\n");
			m_ExResult = FMOD::Memory_Initialize(0, 0, CrySound_Alloc, CrySound_Realloc, CrySound_Free);
		}

		if (IS_FMODERROR)
		{
			FmodErrorOutput("memory initialization failed! ", eSLT_Error);
		}


		//m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::EVENTSYSTEM_CREATE, gEnv->pTimer->GetFrameStartTime().GetMilliSeconds());
		m_ExResult = FMOD::EventSystem_Create(&m_pEventSystem);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("event system failed! ", eSLT_Error);
			return false;
		}
	}

	// and again..
	FMOD_DEBUGLEVEL nLevel = FMOD_DEBUG_LEVEL_NONE;
	//nLevel = SOUNDSYSTEM_DEBUG_FMOD_ALL;
	m_ExResult = FMOD::Debug_SetLevel(nLevel);

	if (pSoundSystem->g_nNetworkAudition)
	{
		m_ExResult = FMOD::NetEventSystem_Init(m_pEventSystem);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("event system network audition init failed! ", eSLT_Error);
			return false;
		}
	}

	// Get FMOD-Ex System Ptr from EventSystem
	if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
		m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::EVENTSYSTEM_GETSYSTEMOBJECT, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds());

	m_ExResult = m_pEventSystem->getSystemObject(&m_pAPISystem);
	if (IS_FMODERROR)
	{
		FmodErrorOutput("get system object failed! ", eSLT_Error);
		return false;
	}

	m_ExResult = m_pAPISystem->setCallback(FMODThreadCallback);
	if (IS_FMODERROR)
	{
		FmodErrorOutput("set thread callback failed! ", eSLT_Error);
		//return false;
	}

	// changing number of Mpeg codec instances
	FMOD_ADVANCEDSETTINGS settings = {0};
	settings.cbsize = sizeof(FMOD_ADVANCEDSETTINGS);

	m_ExResult = m_pAPISystem->getAdvancedSettings(&settings);
	if (IS_FMODERROR)
	{
		FmodErrorOutput("get system advanced settings failed! ", eSLT_Error);
		return false;
	}

	settings.cbsize = sizeof(FMOD_ADVANCEDSETTINGS);
	settings.maxMPEGcodecs	= m_InitSettings.nMPEGDecoders;
	settings.maxADPCMcodecs = m_InitSettings.nADPCMDecoders;
	settings.maxXMAcodecs		= m_InitSettings.nXMADecoders;
	//settings.minHRTFAngle		= 90;
	////settings.maxHRTFAngle   = 0;
	//settings.minHRTFFreq    = 4000;
	//settings.maxHRTFFreq		= 22050;

	m_ExResult = m_pAPISystem->setAdvancedSettings(&settings);
	if (IS_FMODERROR)
	{
		FmodErrorOutput("set system advanced settings failed! ", eSLT_Error);
		return false;
	}

	// output types
	FMOD_OUTPUTTYPE nOutput = FMOD_OUTPUTTYPE_AUTODETECT;

	if (pSoundSystem->g_nOutputConfig == 1)
	{
		nOutput = FMOD_OUTPUTTYPE_DSOUND;
		CryLogAlways("<Sound> Trying to initialize DirectSound output! \n");
	}
	if (pSoundSystem->g_nOutputConfig == 2)
	{
		nOutput = FMOD_OUTPUTTYPE_WAVWRITER;
		CryLogAlways("<Sound> Trying to initialize Wav-Writer output! \n");
	}
	if (pSoundSystem->g_nOutputConfig == 3)
	{
		nOutput = FMOD_OUTPUTTYPE_WAVWRITER_NRT;
		CryLogAlways("<Sound> Trying to initialize Wav-Writer_NRT output! \n");
	}
	if (pSoundSystem->g_nOutputConfig == 4)
	{
		nOutput = FMOD_OUTPUTTYPE_OPENAL;
		//pSoundSystem->g_nFormatType = FMOD_SOUND_FORMAT_PCMFLOAT; // enforce FLOAT Format
		CryLogAlways("<Sound> Trying to initialize OpenAL output! \n");
	}

	// software format
	int nSampleRate = 0;
	FMOD_SOUND_FORMAT Format = FMOD_SOUND_FORMAT_NONE;
	int nOutputChannels = 0;
	int nInputChannels = 0;
	FMOD_DSP_RESAMPLER Resampler = FMOD_DSP_RESAMPLER_NOINTERP;
	int nBits = 0;
	m_ExResult = m_pAPISystem->getSoftwareFormat(&nSampleRate, &Format, &nOutputChannels, &nInputChannels, &Resampler, &nBits);

	if (IS_FMODERROR)
	{
		FmodErrorOutput("get software format failed! ", eSLT_Error);
		return false;
	}

	if (pSoundSystem->g_nFormatSampleRate > 0 && pSoundSystem->g_nFormatSampleRate <= 96000)
		nSampleRate = pSoundSystem->g_nFormatSampleRate;

	if (pSoundSystem->g_nFormatType >= 0 && (FMOD_SOUND_FORMAT) pSoundSystem->g_nFormatType < FMOD_SOUND_FORMAT_MAX)
		Format = (FMOD_SOUND_FORMAT) pSoundSystem->g_nFormatType;

	if (pSoundSystem->g_nFormatResampler >= 0 && (FMOD_DSP_RESAMPLER) pSoundSystem->g_nFormatResampler < FMOD_DSP_RESAMPLER_MAX)
		Resampler = (FMOD_DSP_RESAMPLER) pSoundSystem->g_nFormatResampler;

	m_ExResult = m_pAPISystem->setSoftwareFormat(nSampleRate, Format, 0, 0, Resampler);

	if (IS_FMODERROR)
	{
		FmodErrorOutput("set software format failed! ", eSLT_Error);
		return false;
	}

	if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
		m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::SYSTEM_SETOUTPUT, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*)nOutput);

  m_ExResult = m_pAPISystem->setOutput(nOutput);
	if (IS_FMODERROR)
	{
		FmodErrorOutput("set sound output failed! ", eSLT_Error);
		return false;
	}

	// Create DSound System object on Vista for SoftDec audio support
	m_ExResult = m_pAPISystem->getOutput(&nOutput);
	if (IS_FMODERROR)
	{
		FmodErrorOutput("get sound output failed! ", eSLT_Error);
		return false;
	}

	switch (nOutput) 
	{
	case FMOD_OUTPUTTYPE_DSOUND:
		CryLogAlways("<Sound> Starting to initialize DirectSound output! \n");
		break;
	case FMOD_OUTPUTTYPE_WASAPI:
		CryLogAlways("<Sound> Starting to initialize Windows Audio Session API output! \n");
		break;
	case FMOD_OUTPUTTYPE_WINMM:
		CryLogAlways("<Sound> Starting to initialize Windows Multimedia output! \n");
		break;
	case FMOD_OUTPUTTYPE_OPENAL:
		CryLogAlways("<Sound> Starting to initialize OpenAL output! \n");
		break;
	case FMOD_OUTPUTTYPE_XBOX360:
		CryLogAlways("<Sound> Starting to initialize XBox360 output! \n");
		break;
	case FMOD_OUTPUTTYPE_PS3:
		CryLogAlways("<Sound> Starting to initialize PS3 output! \n");
		break;
	default:
		break;
		}

#if !defined(PS3) && !defined(XENON) 
	if (nOutput != FMOD_OUTPUTTYPE_DSOUND)
	{
		m_ExResult = FMOD::System_Create(&m_pAPISystemDSound);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("2nd system object failed! ", eSLT_Error);
			m_pAPISystemDSound = 0;
		}
		else
		{
			FMOD_DEBUGLEVEL nLevel = FMOD_DEBUG_LEVEL_NONE;
			//nLevel = SOUNDSYSTEM_DEBUG_FMOD_ALL;
			m_ExResult = FMOD::Debug_SetLevel(nLevel);

			m_ExResult = m_pAPISystemDSound->setOutput(FMOD_OUTPUTTYPE_DSOUND);
			if (IS_FMODERROR)
			{
				FmodErrorOutput("set DSound on 2nd system object failed! ", eSLT_Error);
				m_pAPISystemDSound = 0;
			}
			else
			{
				if (m_pSoundSystem->g_nOutputConfig == 2 || m_pSoundSystem->g_nOutputConfig == 3)
					m_ExResult = m_pAPISystemDSound->init(5, FMOD_INIT_NORMAL, (void*)"audio_capture_dsound.wav");
				else
					m_ExResult = m_pAPISystemDSound->init(5, FMOD_INIT_NORMAL, m_pSoundSystem->g_nNetworkAudition?NULL:m_pHWnd);

				if (IS_FMODERROR)
				{
					FmodErrorOutput("init 2nd system object failed! ", eSLT_Error);
					m_pAPISystemDSound = NULL;
				}
			}
		}
	}
#endif

	int nNumDrivers = 0;
	m_ExResult = m_pAPISystem->getNumDrivers(&nNumDrivers);
	if (IS_FMODERROR)
	{
		FmodErrorOutput("get number of drivers failed! ", eSLT_Error);
		//return false;

#if !defined(PS3) && !defined(XENON) 
		// enforce DSound
		m_ExResult = m_pAPISystem->setOutput(FMOD_OUTPUTTYPE_DSOUND);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("set sound output (enforce DSound) failed! ", eSLT_Error);
			return false;
		}

		m_ExResult = m_pAPISystem->getNumDrivers(&nNumDrivers);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("get number of drivers failed again! ", eSLT_Error);
			return false;
		}


#endif
	}
	
	{
		CryLogAlways("<Sound> Drivers found: %d\n", nNumDrivers);
		string pDriversString("<Sound> Available drivers:");

		// prints out valid drivers
		for (unsigned int i=0; m_ExResult==FMOD_OK; ++i)
		{
			char sDriverName[MAXCHARBUFFERSIZE] = {'\0'};
			m_ExResult = m_pAPISystem->getDriverInfo(i, sDriverName, MAXCHARBUFFERSIZE, 0);

			if (m_ExResult == FMOD_OK)
			{
				char cTemp[4] = {'\0'}; // There won't be more than 999 drivers
				sprintf_s(cTemp, 4, "%d", i+1);
				pDriversString +=  "\n\t";
				pDriversString +=  cTemp;
				pDriversString +=  ".\t";
				pDriversString +=  sDriverName;
			}
		}

		CryLogAlways(pDriversString.c_str());
	}

	// setting driver
	if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
		m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::SYSTEM_SETDRIVER, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds());

	m_ExResult = m_pAPISystem->setDriver(0);
	if (IS_FMODERROR)
	{
		FmodErrorOutput("set driver to default failed! ", eSLT_Error);
		return false;
	}

	// querying which driver
	int nDriver = 0;
	m_ExResult = m_pAPISystem->getDriver(&nDriver);
	if (IS_FMODERROR)
	{
		FmodErrorOutput("get driver failed! ", eSLT_Error);
		return false;
	}

	// querying which driver
	char sDriverName[MAXCHARBUFFERSIZE];
	m_ExResult = m_pAPISystem->getDriverInfo(nDriver, sDriverName, MAXCHARBUFFERSIZE, 0);
	if (IS_FMODERROR)
	{
		FmodErrorOutput("get driver name failed! ", eSLT_Error);
		return false;
	}

#if !defined(PS3) && !defined(XENON) 
	// Set HW and Softwarevoices
	m_ExResult = m_pAPISystem->setHardwareChannels(m_InitSettings.nMinHWChannels, m_InitSettings.nMaxHWchannels, m_InitSettings.nMinHWChannels, m_InitSettings.nMaxHWchannels);
	
	if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
		m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::SYSTEM_SETHARDWARECHANNELS, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*) 0, (void*) 0, (void*) 0, (void*) 0);
	
	//m_ExResult = m_pAPISystem->setHardwareChannels(0, 0, 0, 0);
	if (IS_FMODERROR)
	{
		FmodErrorOutput("set hardware voices limit failed! ", eSLT_Error);
		return false;
	}
#endif

	// overwrite file system
	m_ExResult = m_pAPISystem->setFileSystem(&FMODFileOpenCallback, &FMODFileCloseCallback, &FMODFileReadCallback, &FMODFileSeekCallback, -1);
	if (IS_FMODERROR)
	{
		FmodErrorOutput("set file system callbacks failed! ", eSLT_Error);
		return false;
	}

	// need to call iseax before initt to set it up correctly (also speaker mode)
	IsEax();

	if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
		m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::SYSTEM_SETSPEAKERMODE, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*) m_nSpeakerMode);




	m_ExResult = m_pAPISystem->setSpeakerMode(m_nSpeakerMode);
	if (IS_FMODERROR)
	{
		FmodErrorOutput("set speaker mode failed! ", eSLT_Error);
		return false;
	}

	// TOMAS
	m_ExResult = m_pAPISystem->setSoftwareChannels(64);

	CryLogAlways("<Sound> Initializing FMOD-EX now!\n");
	
	if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
		m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::EVENTSYSTEM_INIT, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*) m_InitSettings.nSoftwareChannels, (void*)FMOD_INIT_NORMAL, (void*) 0, (void*) FMOD_EVENT_INIT_NORMAL);

	FMOD_INITFLAGS InitFlags = FMOD_INIT_NORMAL;
	FMOD_EVENT_INITFLAGS EventInitFlags = FMOD_EVENT_INIT_NORMAL; //|FMOD_EVENT_INIT_DONTUSENAMES; // TODO:Enable NoName-Support

	if (m_pSoundSystem->g_nObstruction == 1)
		InitFlags |= FMOD_INIT_SOFTWARE_OCCLUSION;

	if (m_pSoundSystem->g_nHRTF_DSP == 1)
		InitFlags |= FMOD_INIT_SOFTWARE_HRTF;

	if (m_pSoundSystem->g_nVol0TurnsVirtual == 1)
		InitFlags |= FMOD_INIT_VOL0_BECOMES_VIRTUAL;

	if (m_pSoundSystem->g_nNetworkAudition == 1)
		InitFlags |= FMOD_INIT_ENABLE_PROFILE;

	if (m_pSoundSystem->g_nReverbType == REVERB_TYPE_SOFTWARE_LOW)
		InitFlags |= FMOD_INIT_SOFTWARE_REVERB_LOWMEM;

	// GUID conflicts should not cause any problems
	//EventInitFlags |= FMOD_EVENT_INIT_USE_GUIDS; // lets not use this for now

	// no names should be loaded
	//EventInitFlags |= FMOD_EVENT_INIT_DONTUSENAMES; // would break all name-based sound calls.

	unsigned int nBlocksize = 0;
	int nNumblocks = 0;
	m_ExResult = m_pAPISystem->getDSPBufferSize(&nBlocksize, &nNumblocks);

#if !defined(PS3)
	if (m_pSoundSystem->g_nOutputConfig == 2 || m_pSoundSystem->g_nOutputConfig == 3)
	{
		m_ExResult = m_pEventSystem->init(m_InitSettings.nSoftwareChannels, InitFlags, (void*)"audio_capture.wav", EventInitFlags);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("event system init failed pipeing into audio_capture.wav! ", eSLT_Error);
			return false;
		}
	}
	else
#endif
	{
#if !defined(PS3)
		// test for windows advanced performance hardware acceleration slider
		if (m_nFMODDriverCaps & FMOD_CAPS_HARDWARE_EMULATED) // This is really bad for latency!. 
		{                                                   /* You might want to warn the user about this. */
			m_ExResult = m_pAPISystem->setDSPBufferSize(1024, 10);    /* At 48khz, the latency between issuing an fmod command and hearing it will now be about 213ms. */
			if (IS_FMODERROR)
			{
				FmodErrorOutput("system set DSP Buffer Size failed! ", eSLT_Error);
				//return false;
			}
		} 



		m_ExResult = m_pEventSystem->init(m_InitSettings.nSoftwareChannels, InitFlags, m_pSoundSystem->g_nNetworkAudition?NULL:m_pHWnd, EventInitFlags);



#endif
		if (IS_FMODERROR)
		{
			FmodErrorOutput("event system init failed! ", eSLT_Error);
			if (m_ExResult == FMOD_ERR_OUTPUT_CREATEBUFFER)
			{
				// this might be an invalid speaker mode (5.0 or 7.1), so report an error if possible and fallback to stereo
				m_ExResult = m_pAPISystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
				
				if (IS_FMODERROR)
				{
					FmodErrorOutput("set fallback stereo speaker mode failed! ", eSLT_Error);
					return false;
				}

				m_ExResult = m_pEventSystem->init(m_InitSettings.nSoftwareChannels, InitFlags, m_pSoundSystem->g_nNetworkAudition?NULL:m_pHWnd, FMOD_EVENT_INIT_NORMAL);
				
				if (IS_FMODERROR)
				{
					FmodErrorOutput("event system init with fallback stereo speaker mode failed! ", eSLT_Error);
					return false;
				}
				else
					CryLogAlways("<Sound> Initialized FMOD-EX with stereo fallback\n");

			}
			else
				return false;
		}
		else
			CryLogAlways("<Sound> Initialized FMOD-EX\n");
	}



	// Record driver query
	int nNumRecordDrivers = 0;
	m_ExResult = m_pAPISystem->getRecordNumDrivers(&nNumRecordDrivers);

	for (int i=0; i<nNumRecordDrivers; ++i)
	{
		char sRecordDriverName[MAXCHARBUFFERSIZE];
		m_ExResult = m_pAPISystem->getRecordDriverInfo(i, sRecordDriverName, MAXCHARBUFFERSIZE, 0);

		if (m_ExResult == FMOD_OK)
			CryLogAlways("<Sound> Available record drivers: %d %s !\n", i, sRecordDriverName);
	}

	if (m_pSoundSystem->g_nRecordConfig == 0)
		CryLogAlways("<Sound> Record config is set to autodetect using record driver 0\n");
	else
		CryLogAlways("<Sound> Record config is set to explicitly use record driver %d\n", m_pSoundSystem->g_nRecordConfig-1); // decrease because FMOD index starts at 0

	// set Record Driver for Microphone first
	if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
		m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::SYSTEM_SETRECORDDRIVER, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*) 0);

	m_ExResult = FMOD_OK;
	//m_ExResult = m_pAPISystem->setRecordDriver(0);
	if (IS_FMODERROR)
	{
		FmodErrorOutput("init of microphone recording failed! ", eSLT_Error);
	}
	
	uint32 nVersion;
	m_ExResult = m_pAPISystem->getVersion(&nVersion);
	CryLogAlways("<Sound> Using FMOD version: %08X and internal %08X!\n", nVersion, FMOD_VERSION);

	if (IS_FMODERROR)
	{
		FmodErrorOutput("Version number not available!", eSLT_Error);
		return false;
	}

	if (nVersion < FMOD_VERSION)
	{
		m_ExResult = FMOD_ERR_VERSION;
		if (IS_FMODERROR)
		{
			FmodErrorOutput("Version number conflict!", eSLT_Error);
			return false;
		}
	}

	// deactive SetMediaPath as long as FMOD can not track individual
	// FEV Paths internaly if the file is loaded from memory
	//char szPath[512];
	//sprintf_s(szPath, "/%s/Sounds/", PathUtil::GetGameFolder().c_str());
	////sprintf_s(szPath, "/Game/Sounds/");
	//
	//if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
	//	m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::EVENTSYSTEM_SETMEDIAPATH, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*) szPath);

	//m_ExResult = m_pEventSystem->setMediaPath(szPath);
	//if (IS_FMODERROR)
	//{
	//	FmodErrorOutput("invalid media path! ", eSLT_Warning);
	//}

	//m_ExResult = m_pFMODEX->setOutput(FMOD_OUTPUTTYPE_AUTODETECT);
	//m_ExResult = m_pFMODEX->setDriver(0);
	
	/*
	Set the distance units. (meters/feet etc).
	*/
	const float DISTANCEFACTOR = 1.0f;   // Units per meter.  I.e feet would = 3.28.  centimeters would = 100.
	// workaround for FMOD bug: distance attenuation was 1.0f, lowered to fix
	m_ExResult = m_pAPISystem->set3DSettings(1.0, DISTANCEFACTOR, 0.01f);
	if (IS_FMODERROR)
	{
		FmodErrorOutput("distance factor failed! ", eSLT_Warning);
	}

  // Set memory system to crytek mem manager
  //CS_SetMemorySystem(NULL,NULL,CrySound_Alloc,CrySound_Realloc,CrySound_Free);

  // Assign file access callbacks to fmod to our pak file system.
  //CS_File_SetCallbacks( CrySound_fopen,CrySound_fclose,CrySound_fread,CrySound_fseek,CrySound_ftell );
	
  // store the amount of software channels
  //ptParamINT32 softwareChannels(softwareChan);
  //bTemp = SetParam(adpSOFTWARECHANNELS ,&softwareChannels);
  
  //******  Init fmod system setting mixing/playing speed (44100)
  // also requesting 256 software channels
  // flags are -0- for now

  // After init called this sets up hardware channel info for 3d and 2d channels
  GetNumberSoundsPlaying();

  // report info to log file
	//int32 nNuMDriver;
	//char sDriverName[MAXCHARBUFFERSIZE];

	//m_ExResult = m_pCSEX->getDriver(&nNuMDriver);
	//m_ExResult = m_pCSEX->getDriverName(nNuMDriver, sDriverName, MAXCHARBUFFERSIZE);

	int nHW2D = 0;
	int nHW3D = 0;

	m_pSoundSystem->Log(eSLT_Always, "<Sound> -------------CRYSOUND-EX VERSION =   %08X ------- \n", FMOD_VERSION);
	m_ExResult = m_pAPISystem->getHardwareChannels(&nHW2D, &nHW3D, &m_nTotalHardwareChannelsAvail);
	m_pSoundSystem->Log(eSLT_Always, "<Sound> Total number of 2D hardware channels available: %d\n", nHW2D);
	m_pSoundSystem->Log(eSLT_Always, "<Sound> Total number of 3D hardware channels available: %d\n", nHW3D);
	m_pSoundSystem->Log(eSLT_Always, "<Sound> Total number of all hardware channels available: %d\n", m_nTotalHardwareChannelsAvail);

	//m_nHardware2DChannels = 5;
	//m_nHardware3DChannels = 5;
	//m_nTotalHardwareChannelsAvail = 10;

	IReverbManager *pReverbManager = m_pSoundSystem->GetIReverbManager();
	if (pReverbManager)
	{
		pReverbManager->Init(this, m_pSoundSystem->g_nReverbInstances, m_pSoundSystem);
		pReverbManager->SelectReverb(m_pSoundSystem->g_nReverbType);
	}
	
	/* After eventsystem create in your code */
	//FMOD::gDebugMode  = FMOD::DEBUG_STDOUT;
	//FMOD::gDebugLevel = FMOD::LOG_NONE;


	// DSP Debugging
	//char sDSPName[512];
	//int nNums = 0;
	//FMOD::DSP* pSoundCard = NULL;
	//m_ExResult = m_pAPISystem->getDSPHead(&pSoundCard);
	//m_ExResult = pSoundCard->getInfo(sDSPName, 0,0,0,0);
	//m_ExResult = pSoundCard->getNumInputs(&nNums);


	//FMOD::DSP* pTargetUnit = NULL;
	//m_ExResult = pSoundCard->getInput(0, &pTargetUnit);
	//m_ExResult = pTargetUnit->getInfo(sDSPName, 0,0,0,0);
	//m_ExResult = pTargetUnit->getNumInputs(&nNums);

	//FMOD::DSP* pFMODMaster = NULL;
	//FMOD::DSP* pMaster = NULL;
	//FMOD::DSP* pReverb = NULL;
	//		

	//m_ExResult = pTargetUnit->getNumInputs(&nNums);

	//m_ExResult = pTargetUnit->getInput(0, &pFMODMaster);
	//if (pFMODMaster)
	//{
	//	m_ExResult = pFMODMaster->getNumInputs(&nNums);
	//	m_ExResult = pFMODMaster->getInfo(sDSPName, 0,0,0,0);
	//}

	//m_ExResult = pTargetUnit->getInput(1, &pMaster);
	//if (pMaster)
	//{
	//	FMOD::DSP* pMusic = NULL;		
	//	m_ExResult = pMaster->getInfo(sDSPName, 0,0,0,0);
	//	m_ExResult = pMaster->getNumInputs(&nNums);
	//	m_ExResult = pMaster->getInput(0, &pMusic);

	//	if (pMusic)
	//	{
	//		m_ExResult = pMusic->getNumInputs(&nNums);
	//		m_ExResult = pMusic->getInfo(sDSPName, 0,0,0,0);
	//	}
	//}	




  return bTemp;
}


// returns ptr to the output device if possible, else NULL
// this might be a pointer to DirectX LPDIRECTSOUND or a WINMM handle
void CAudioDeviceFmodEx400::GetOutputHandle( void **pHandle, void **pHandle2, EOutputHandle *HandleType)
{
	if (!pHandle || !HandleType || !m_pAPISystem)
		return; 

	if (m_pAPISystem)
		m_ExResult = m_pAPISystem->getOutputHandle(pHandle);





	
	FMOD_OUTPUTTYPE OutputType;

	if (m_pAPISystem)
		m_ExResult = m_pAPISystem->getOutput(&OutputType);

	switch(OutputType) 
	{
	case FMOD_OUTPUTTYPE_DSOUND:
		*HandleType = eOUTPUT_DSOUND;

		// prevent CRI video codec from not play intro/tutorial videos, instead play them but without any sound
		if (m_nFMODDriverCaps & FMOD_CAPS_HARDWARE_EMULATED)
			*HandleType = eOUTPUT_NOSOUND;
		break;
	case FMOD_OUTPUTTYPE_WINMM:
		*HandleType = eOUTPUT_WINMM;
		break;
	case FMOD_OUTPUTTYPE_WASAPI:
		*HandleType = eOUTPUT_WASAPI;
		break;
	case FMOD_OUTPUTTYPE_OPENAL:
		*HandleType = eOUTPUT_OPENAL;
		break;
	case FMOD_OUTPUTTYPE_ASIO:
		*HandleType = eOUTPUT_ASIO;
		break;
	case FMOD_OUTPUTTYPE_OSS:
		*HandleType = eOUTPUT_OSS;
		break;
	case FMOD_OUTPUTTYPE_ESD:
		*HandleType = eOUTPUT_ESD;
		break;
	case FMOD_OUTPUTTYPE_ALSA:
		*HandleType = eOUTPUT_ALSA;
		break;	
	//case FMOD_OUTPUTTYPE_MAC:
	//	HandleType = eOUTPUT_MAC;
	//	break;	
	//case FMOD_OUTPUTTYPE_XBOX:
	//	*HandleType = eOUTPUT_Xbox;
	//	break;
	case FMOD_OUTPUTTYPE_XBOX360:
		*HandleType = eOUTPUT_Xbox360;
		break;
	case FMOD_OUTPUTTYPE_PS2:
		*HandleType = eOUTPUT_PS2;
		break;
	case FMOD_OUTPUTTYPE_PS3:
		*HandleType = eOUTPUT_PS3;
		break;
	case FMOD_OUTPUTTYPE_PSP:
		*HandleType = eOUTPUT_PSP;
		break;
	//case FMOD_OUTPUTTYPE_GC:
	//	*HandleType = eOUTPUT_GC;
	//	break;
	case FMOD_OUTPUTTYPE_WII:
		*HandleType = eOUTPUT_WII;
		break;
	case FMOD_OUTPUTTYPE_NOSOUND:
		*HandleType = eOUTPUT_NOSOUND;
		break;
	case FMOD_OUTPUTTYPE_WAVWRITER:
		*HandleType = eOUTPUT_WAVWRITER;
		break;
default:
	*HandleType = eOUTPUT_MAX;
	}

	// use secondary DSound system
	if (m_pAPISystemDSound)
	{
		*HandleType = eOUTPUT_DSOUND;
		m_ExResult = m_pAPISystemDSound->getOutputHandle(pHandle);
	}
}

void CAudioDeviceFmodEx400::GetInitSettings(AudioDeviceSettings *InitSettings)
{
	*InitSettings = m_InitSettings;
}

void CAudioDeviceFmodEx400::SetInitSettings(AudioDeviceSettings *InitSettings)
{
	m_InitSettings = *InitSettings;
}

bool CAudioDeviceFmodEx400::ShutDownDevice(void)
{
	// in this function m_pSoundSystem might be NULL if initialization of SoundSystem failed

  bool bTemp=true;
  // this one dosn't need a param
  bTemp = SetParam(adpSTOPALL_CHANNELS, NULL);
	
	if (m_pEventSystem)
	{
		if (m_pSoundSystem && m_pSoundSystem->g_nNetworkAudition)
		{
			m_ExResult = FMOD::NetEventSystem_Shutdown();
			if (IS_FMODERROR)
			{
				FmodErrorOutput("event system network audition shutdown failed! ", eSLT_Error);
				bTemp = false;
			}
		}		
		
		m_ExResult = m_pEventSystem->unload();
		if (IS_FMODERROR)
		{
			FmodErrorOutput("event system unload failed! ", eSLT_Warning);
			bTemp = false;
		}

		m_ExResult = m_pEventSystem->release();
		if (IS_FMODERROR)
		{
			FmodErrorOutput("event system release failed! ", eSLT_Error);
			bTemp = false;
		}

		m_pEventSystem = NULL;
		m_pAPISystem = NULL;

	}

	if (m_pAPISystemDSound)
	{
		m_ExResult = m_pAPISystemDSound->close();
		if (IS_FMODERROR)
		{
			FmodErrorOutput("2nd DSound system release failed! ", eSLT_Error);
			bTemp = false;
		}

		m_pAPISystemDSound = NULL;

	}

	int nFMODCurrent = 0;
	int nMax = 0;
	m_ExResult = FMOD::Memory_GetStats(&nFMODCurrent, &nMax);

	m_pSoundSystem->GetISoundAssetManager()->RemoveProjectDependency();
	m_VecLoadedProjectFiles.clear();

	// refresh categories after unloading projects
	if (m_pSoundSystem && m_pSoundSystem->GetIMoodManager())
		m_pSoundSystem->GetIMoodManager()->RefreshCategories();

	// free static memory block in RSX on PS3








	// free static memory block in main memory
	if (m_pCacheStaticMemBlock)
	{



		delete[] (uint8*)(m_pCacheStaticMemBlock);

		m_pCacheStaticMemBlock = NULL;
		m_nCacheStaticMemBlockSize = 0;
	}

	return bTemp;
}

bool CAudioDeviceFmodEx400::ResetAudio(void)
{
  bool bTemp = true;
  //GUARD_HEAP;

	assert (m_pEventSystem);

	if (m_pSoundSystem->g_nUnloadData && m_pEventSystem)
	{
		m_pSoundSystem->GetISoundAssetManager()->RemoveProjectDependency();
		for (unsigned int i = 0; i < m_VecLoadedProjectFiles.size(); ++i)
		{
			UnloadProjectFile(i);
		}

		m_VecLoadedProjectFiles.clear();

		//unload Projects *important* all handles turn invalid now!
		m_ExResult = m_pEventSystem->unload();
		if (IS_FMODERROR)
		{
			FmodErrorOutput("projects couldnt be unloaded! ", eSLT_Error);
			bTemp = false;
		}

		// update group and event counting
		UpdateGroupEventCount();
	}

	// refresh categories after unloading projects
	if (m_pSoundSystem->GetIMoodManager())
		m_pSoundSystem->GetIMoodManager()->RefreshCategories();

  //m_nMemoryStatInc = 0;

	//ShutDownDevice();
	//m_ExResult = FMOD::System_Create(&m_pFMODEX);
	//if (IS_FMODERROR)
	//{
	//	FmodErrorOutput("system object create failed! ", eSLT_Error);
	//	bTemp = false;
	//}


	//InitAudio(m_pSoundSystem, m_nSoftwareChannels);

  //if (!CS_Init(m_nSystemFrequency , m_nSoftwareChannels, 0))
	//m_ExResult = m_pFMODEX->init(100, FMOD_INIT_NORMAL, m_pHWnd);

	//if (!InitDevice(m_pSoundSystem, m_nSoftwareChannels))
	//{
	//	m_pSoundSystem->Log("System re-init of CRYSOUND FAILED\n");
	//	ShutDownDevice();
	//	return false;
	//}
  //m_pSoundSystem->Log("--------------  RE-INIT  --------------------------CRYSOUND VERSION = %f\n",CS_GetVersion());
	//int32 nSoftwareChannels = 0;
	//m_pFMODEX->getSoftwareChannels(&nSoftwareChannels);
	//m_pSoundSystem->Log("Total number of channels available: %d\n", nSoftwareChannels);


  //bTemp = (CS_SetHWND(m_pHWnd) ? true : false); // re_set active window

  return bTemp;
}

// this is called every frame to update all listeners and must be called *before* SubSystemUpdate()
bool CAudioDeviceFmodEx400::UpdateListeners(void)
{
	if (!m_pAPISystem || !m_pEventSystem)
		return false;

	bool bResult = true;
	uint32 nNumActiveListeners = m_pSoundSystem->GetNumActiveListeners();
	FMOD_VECTOR vExPos;
	FMOD_VECTOR vExVel;
	FMOD_VECTOR vExForward;
	FMOD_VECTOR vExTop;

	if (nNumActiveListeners > 4)
	{
		m_ExResult = FMOD_ERR_INVALID_PARAM;
		FmodErrorOutput("more than 4 listeners! ", eSLT_Warning);
		nNumActiveListeners = 4;
	}


	CListener* pListener = NULL;
	ListenerID nListenerID = LISTENERID_INVALID;
	int nListenerIndex = 0;
	do 
	{
		pListener = (CListener*)m_pSoundSystem->GetNextListener(nListenerID);
		if (pListener)
		{
			nListenerID = pListener->GetID();

			if (pListener->GetActive())// && pListener->bRotated)
			{
				Vec3 vPos = pListener->GetPosition();
				Vec3 vForward = pListener->GetForward();
				Vec3 vTop = pListener->GetTop();
				vExPos.x = vPos.x;
				vExPos.y = vPos.z;
				vExPos.z = vPos.y;

				if (m_pSoundSystem->g_nDoppler)
				{
					vExVel.x = clamp(pListener->GetVelocity().x, -200.0f, 200.0f);
					vExVel.y = clamp(pListener->GetVelocity().z, -200.0f, 200.0f);
					vExVel.z = clamp(pListener->GetVelocity().y, -200.0f, 200.0f);
				}
				else
				{
					vExVel.x = 0;
					vExVel.y = 0;
					vExVel.z = 0;
				}

				vExForward.x = vForward.x;
				vExForward.y = vForward.z;
				vExForward.z = vForward.y;
				vExTop.x = vTop.x;
				vExTop.y = vTop.z;
				vExTop.z = vTop.y;

				// FMOD-EX
				//m_ExResult = m_pFMODEX->set3DNumListeners(nNumActiveListeners);
				//if (IS_FMODERROR)
				//{
				//	FmodErrorOutput("invalid number of listeners! ", eSLT_Warning);
				//	bResult = false;
				//}

				//m_ExResult = m_pFMODEX->set3DListenerAttributes(nListenerID, &vExPos, &vExVel, &vExForward, &vExTop);
				//if (IS_FMODERROR)
				//{
				//	FmodErrorOutput("listener 3d update failed! ", eSLT_Warning);
				//	bResult = false;
				//}

				// Event System

				{
					if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
						m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::EVENTSYSTEM_SET3DNUMLISTENERS, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*)nNumActiveListeners);

					CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_API_OTHER) );
					m_ExResult = m_pEventSystem->set3DNumListeners(nNumActiveListeners);
				}

				if (IS_FMODERROR)
				{
					FmodErrorOutput("event system invalid number of listeners! ", eSLT_Warning);
					bResult = false;
				}

				//m_pSoundSystem->Log("FMOD Update Listener Pos: %.2f,%.2f,%.2f", vPos.x, vPos.y, vPos.z);

				{	
					if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
						m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::EVENTSYSTEM_SET3DLISTENERSATTRIBUTES, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*)nListenerID, (void*)&vExPos, (void*)&vExVel, (void*)&vExForward, (void*)&vExTop);

					CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_API_OTHER) );
					m_ExResult = m_pEventSystem->set3DListenerAttributes(nListenerIndex, &vExPos, &vExVel, &vExForward, &vExTop);
				}

				if (IS_FMODERROR)
				{
					FmodErrorOutput("event system listener 3d update failed! ", eSLT_Warning);
					bResult = false;
				}

				pListener->MarkAsSet();
				++nListenerIndex; // increase FMOD index
			}
		}

	}	while (pListener);

	return bResult;
}

// must be called every game frame
bool CAudioDeviceFmodEx400::Update(void)
{
	//float position[3];
	//float velocity[3];
	bool bResult = true;

	//CTimeValue tTimeDiff = gEnv->pTimer->GetAsyncTime() - m_pSoundSystem->m_tUpdateAudioDevice;
	//float fMilliSecs = tTimeDiff.GetMilliSeconds();
	//if (fMilliSecs == 0)
	//	return bResult;

#ifdef FMOD_MEMORY_DEBUGGING_SIMPLE
	if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_MEMORY)
	{
		int nFMODCurrent = 0;
		m_ExResult = FMOD::Memory_GetStats(&nFMODCurrent, 0);
		m_pSoundSystem->Log(eSLT_Message, "<Sound> AD: Memory Allocs Active %d Total-small %d Total-large %d FMOD: %d Alloc-Control: %d\n", nFMODMemActiveAllocCount, nFMODMemTotalSmallAllocCount, nFMODMemTotalLargeAllocCount, nFMODCurrent, nFMODMemUsage);
	}
	
#endif

	if (m_pSoundSystem)
	{
		int nDebugLevel = m_pSoundSystem->g_nDebugSound;

		FMOD_DEBUGLEVEL nLevel = FMOD_DEBUG_LEVEL_NONE;

		if (nDebugLevel == SOUNDSYSTEM_DEBUG_FMOD_SIMPLE)
			nLevel = (FMOD_DEBUG_TYPE_EVENT | 
								FMOD_DEBUG_DISPLAY_TIMESTAMPS | 
								FMOD_DEBUG_LEVEL_ERROR);

		if (nDebugLevel == SOUNDSYSTEM_DEBUG_FMOD_COMPLEX)
			nLevel = (FMOD_DEBUG_TYPE_EVENT | 
								FMOD_DEBUG_DISPLAY_TIMESTAMPS | 
								FMOD_DEBUG_LEVEL_ERROR | 
								FMOD_DEBUG_LEVEL_WARNING);

		if (nDebugLevel == SOUNDSYSTEM_DEBUG_FMOD_ALL)
			nLevel = (FMOD_DEBUG_TYPE_EVENT | 
								FMOD_DEBUG_DISPLAY_TIMESTAMPS | 
								FMOD_DEBUG_LEVEL_ERROR | 
								FMOD_DEBUG_LEVEL_WARNING |
								FMOD_DEBUG_LEVEL_HINT | 
								FMOD_DEBUG_TYPE_MEMORY);

		m_ExResult = FMOD::Debug_SetLevel(nLevel);

		if (nDebugLevel == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
		{
			if (!m_CommandPlayer.IsRecording())
				m_CommandPlayer.RecordFile("SoundCommands.xml");
		}
		else
		{
			m_CommandPlayer.StopRecording();
		}

	}
	
	
	//DWORD threadid = GetCurrentThreadId();

	// every ? calls get stats (memory and cpu usage pct)
	//if (m_bGetSystemStats)
	//{
	//	if (++m_nMemoryStatInc >= GETMEMORYSTATSEVERY)
	//	{
	//		FRAME_PROFILER( "FMOD-GetMem_CPU", GetISystem(), PROFILE_SOUND );	
	//		GetCpuPctUsage();
	//		GetMemoryStats();
	//		m_nMemoryStatInc = 0;
	//	}
	//}

	int nUpdateLoops = 0;

	if (m_pSoundSystem->g_nOutputConfig == 3)
	{
		CTimeValue tCurrent = gEnv->pTimer->GetFrameStartTime();
		CTimeValue tTimeDiff = tCurrent - m_tLastUpdate;
		m_tLastUpdate = tCurrent;

		//CTimeValue tTimeDiff = (gEnv->pTimer->GetAsyncTime() - gEnv->pTimer->GetFrameStartTime()); //m_pSoundSystem->m_tUpdateAudioDevice;
		float fMS = tTimeDiff.GetMilliSeconds();

		if (fMS < 5000.0f) // prevent long pause (5 sec) of silence when activated at runtime
		{
			while (abs(fMS) > 21.33f)
			{
				++nUpdateLoops;
				fMS -= 21.33f;
			}
			tTimeDiff.SetMilliSeconds((int64)fMS);
			m_tLastUpdate -= tTimeDiff;
		}

	}
	else
		nUpdateLoops = 1;

	// update crysound must be called every frame
	while (m_pEventSystem && nUpdateLoops>0)
	{	
			--nUpdateLoops;

			{
				if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
					m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::EVENTSYSTEM_UPDATE, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds());

				FRAME_PROFILER( "SoundSystem::FMOD-EventUpdate", GetISystem(), PROFILE_SOUND );	
				m_ExResult = m_pEventSystem->update();
			}	

			if (IS_FMODERROR) 
			{
				// disable output for VS2
				FmodErrorOutput("event system update failed! ", eSLT_Warning);
				bResult = false;
			}

			if (m_pSoundSystem->g_nNetworkAudition)
			{
				CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_API_OTHER) );
				m_ExResult = FMOD::NetEventSystem_Update();
				if (IS_FMODERROR)
				{
					FmodErrorOutput("event system network audition update failed! ", eSLT_Warning);
					bResult = false;
				}
			}

			if (m_pSoundSystem->g_nProfiling > 0)
			{
				FMOD_EVENT_SYSTEMINFO SystemInfo;

				// first get the number of wavebanks
				{
					CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_API_INFO) );
					m_ExResult = m_pEventSystem->getInfo(&SystemInfo);
				}
				if (IS_FMODERROR)
				{
					FmodErrorOutput("event system get system info failed! ", eSLT_Warning);
					bResult = false;
				}

				FMOD_EVENT_WAVEBANKINFO* pWavebankInfo =  NULL;

				if (SystemInfo.maxwavebanks)
				{
					// now repeat to fill the array of wavebank infos
					pWavebankInfo = new FMOD_EVENT_WAVEBANKINFO[SystemInfo.maxwavebanks];

					if (!pWavebankInfo)
						return false;

					SystemInfo.wavebankinfo = pWavebankInfo;

					{
						CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_API_INFO) );
						m_ExResult = m_pEventSystem->getInfo(&SystemInfo);
					}
					if (IS_FMODERROR)
					{
						FmodErrorOutput("event system get system info with wavebanks failed! ", eSLT_Warning);
						bResult = false;
					}
				}

				//nSizeInMB += Info.instancememory;

				for (int i=0; i<SystemInfo.maxwavebanks; ++i)
				{
					tmapWavebanks::iterator It = m_Wavebanks.find(CONST_TEMP_STRING(pWavebankInfo[i].name));

					IWavebank *pWavebank = NULL;
					if (It == m_Wavebanks.end())
					{
						pWavebank = new CWavebankFmodEx400(pWavebankInfo[i].name);

						if (!pWavebank)
						{
							delete[] pWavebankInfo; // remove wavebankinfo array
							return false;
						}

						m_Wavebanks[pWavebankInfo[i].name] = pWavebank;
					}
					else
						pWavebank = (*It).second;

					IWavebank::SWavebankInfo BankInfo;
					BankInfo.nMemCurrentlyInByte = pWavebankInfo[i].streammemory + pWavebankInfo[i].samplememory;
					BankInfo.nMemPeakInByte = pWavebankInfo[i].streammemory + pWavebankInfo[i].samplememory;

					if (*pWavebank->GetPath())
					{
						CCryFile file;

						m_sFullWaveBankName = "";

						int nPathLength = strlen(pWavebank->GetPath());
						int nNameLength = strlen(pWavebank->GetName());
						if (nPathLength + nNameLength + 4 < 512)
						{
							m_sFullWaveBankName = pWavebank->GetPath();
							m_sFullWaveBankName += pWavebank->GetName();
							m_sFullWaveBankName += ".fsb";

							if (file.Open( m_sFullWaveBankName, "rb" ))
								BankInfo.nFileSize = file.GetLength();
						}

					}
					pWavebank->AddInfo(BankInfo);

					//nSizeInMB += Info.wavebankinfo[i].streammemory + Info.wavebankinfo[i].samplememory;
				}

				delete[] pWavebankInfo; // remove wavebankinfo array
				//nSizeInMB = nSizeInMB/(1024*1024);

				//m_Wavebanks


			}

			if (m_pSoundSystem->g_nFindLostEvents)
				FindLostEvent();
	}

	// go through Projects and unload those which are not referenced
	if (m_pSoundSystem->g_nUnloadProjects)
	{
		bool bProjectWasRemoved = false;
		VecProjectFilesIter IterEnd = m_VecLoadedProjectFiles.end();
		for (VecProjectFilesIter Iter = m_VecLoadedProjectFiles.begin(); Iter!=IterEnd; ++Iter)
		{
			if ((*Iter).pProjectHandle != PROJECTHANDLE_INVALID && (*Iter).pReadStream == 0 && (*Iter).nRefCount == 0)
			{
				CTimeValue tDeltaTime = gEnv->pTimer->GetFrameStartTime() - (*Iter).tLastTimeUsed;
				float fDiff = tDeltaTime.GetSeconds();

				if (fDiff > 20.0f)
				{
					FMOD::EventProject* pProject = (FMOD::EventProject*)(*Iter).pProjectHandle;

					if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_SIMPLE)
						m_pSoundSystem->Log(eSLT_Message, "<Sound> AD: Project released: %s",(*Iter).sProjectName.c_str());

					m_ExResult = pProject->release();
					(*Iter).pProjectHandle = PROJECTHANDLE_INVALID;
					bProjectWasRemoved = true;
				}
			}
		}

		// update group and event counting
		if (bProjectWasRemoved)
			UpdateGroupEventCount();
	}

	return bResult;
}

void CAudioDeviceFmodEx400::GetMemoryStats(enumAudioDeviceMemoryQuery eMemoryQuery, unsigned int *nMemoryUsed)
{

	FRAME_PROFILER( "SoundSystem::FMOD-MemoryStats", GetISystem(), PROFILE_SOUND );	

	/*
#ifdef FMOD_MEMORY_DEBUGGING_SIMPLE
	if (nMemoryUsed)
		*nMemoryUsed = nFMODMemUsage;

	//if (nMaxOrCacheSize)
	//	*nMaxOrCacheSize = 0;

	//return nFMODMemUsage; // use own traced memory for now
	return;
#endif
	*/

	unsigned int nAllocated = 0;
	unsigned int nAllocated2 = 0;
	unsigned int nAllocatedSecondary = 0;
	int nAllocatedCurrent = 0;
	unsigned int nMemoryUsedArray[FMOD_MEMTYPE_MAX] = {0};





	m_ExResult = FMOD::Memory_GetStats(&m_nCurrentMemAlloc, &m_nMaxMemAlloc);

	if (m_pAPISystem && m_pEventSystem)
	{

		switch (eMemoryQuery)
		{
		case admqALL:
			// use the cheaper version for now
			nAllocated = m_nCurrentMemAlloc;

			// if callback allocaltion was tracked, then this more accurate number is used
#ifdef FMOD_MEMORY_DEBUGGING_SIMPLE
			if (nFMODMemUsage)
				nAllocated = nFMODMemUsage;
#endif
			//m_ExResult = m_pEventSystem->getMemoryInfo(FMOD_MEMBITS_ALL, FMOD_EVENT_MEMBITS_ALL, &nAllocated, nMemoryUsedArray);
			break;

		case admqSOUNDALL:
			
			if (nAllocatedSecondary)
			{
				// query all sound data, we dont know if this is main or rsx
				m_pAPISystem->getSoundRAM(&nAllocatedCurrent, 0, 0); // use faster but a bit inaccurate function
				nAllocated = nAllocatedCurrent;
			}
			else
				m_ExResult = m_pEventSystem->getMemoryInfo(FMOD_MEMBITS_SOUND, 0, &nAllocated, nMemoryUsedArray);
			break;

		case admqSOUNDMAIN:
			if (nAllocatedSecondary)
			{
				nAllocated = 0;
			}
			else
			{
				m_ExResult = m_pEventSystem->getMemoryInfo(FMOD_MEMBITS_SOUND, 0, &nAllocated, nMemoryUsedArray);
			}

			break;

		case admqSOUNDSECONDARY:
			if (nAllocatedSecondary)
			{
				// query all sound data, we dont know if this is main or rsx
				m_pAPISystem->getSoundRAM(&nAllocatedCurrent, 0, 0); // use faster but a bit inaccurate function
				nAllocated = nAllocatedCurrent;
			}
			else
			{
				nAllocated = 0;
			}

			break;

		case admqSYSTEM:

			m_ExResult = m_pEventSystem->getMemoryInfo(FMOD_MEMBITS_ALL, FMOD_EVENT_MEMBITS_ALL, &nAllocated, nMemoryUsedArray);
			m_ExResult = m_pEventSystem->getMemoryInfo(FMOD_MEMBITS_SOUND, 0, &nAllocated2, nMemoryUsedArray);

			// everything except audio data
			if (nAllocated > nAllocated2)
				nAllocated = nAllocated - nAllocated2;

			break;
		case admqCACHEFILLALL:
			if (nAllocatedSecondary)
			{
				// query all sound data on PS3
				m_pAPISystem->getSoundRAM(&nAllocatedCurrent, 0, 0); // use faster but a bit inaccurate function
				nAllocated = nAllocatedCurrent;
			}
			nAllocated += m_nCurrentMemAlloc;
			break;
		case admqCACHEFILLMAIN:
			{
				nAllocated = m_nCurrentMemAlloc;
			}
			break;
		case admqCACHEFILLSECONDARY:
			if (nAllocatedSecondary)
			{
				// query all sound data on PS3
				m_pAPISystem->getSoundRAM(&nAllocatedCurrent, 0, 0); // use faster but a bit inaccurate function
				nAllocated = nAllocatedCurrent;
			}
			else
			{
				nAllocated = 0;
			}
			break;
		case admqCACHESIZEALL:
			if (m_InitSettings.bCacheStatic)
			{
				nAllocated = (unsigned int)(m_InitSettings.fCacheSize * 1024 * 1024) + nAllocatedSecondary;
			}
			else
			{
				ICVar* pCacheSize = gEnv->pConsole->GetCVar("s_CacheSize");
				float fCacheSizeCVar = pCacheSize ? pCacheSize->GetFVal() : 0.0f;
				fCacheSizeCVar = (fCacheSizeCVar > 0) ? fCacheSizeCVar : m_InitSettings.fCacheSize;
				nAllocated = (unsigned int)(fCacheSizeCVar * 1024 * 1024) + nAllocatedSecondary;
			}
			break;
		case admqCACHESIZEMAIN:
			if (m_InitSettings.bCacheStatic)
			{
				nAllocated = (unsigned int)(m_InitSettings.fCacheSize * 1024 * 1024);
			}
			else
			{
				ICVar* pCacheSize = gEnv->pConsole->GetCVar("s_CacheSize");
				float fCacheSizeCVar = pCacheSize ? pCacheSize->GetFVal() : 0.0f;
				fCacheSizeCVar = (fCacheSizeCVar > 0) ? fCacheSizeCVar : m_InitSettings.fCacheSize;
				nAllocated = (unsigned int)(fCacheSizeCVar * 1024 * 1024);
			}
			break;
		case admqCACHESIZESECONDARY:
			{
				nAllocated = nAllocatedSecondary;
			}
			break;
		}

	}

	if (IS_FMODERROR)
	{
		FmodErrorOutput("system get memory stats failed! ", eSLT_Warning);
		return;
	}
	
	if (nMemoryUsed)
	{
		*nMemoryUsed = nAllocated;
	}

	//if (nMaxOrCacheSize)
	//{
	//	// add static memblock
	//	if (m_pCacheStaticMemBlock)
	//		*nMaxOrCacheSize = m_nCacheStaticMemBlockSize;
	//	else
	//	{
	//		ICVar* pCacheSize = gEnv->pConsole->GetCVar("s_CacheSize");
	//		*nMaxOrCacheSize = pCacheSize ? pCacheSize->GetFVal()*1024*1024 : 0;
	//	}
	//}

	return;// m_nCurrentMemAlloc;
}

bool CAudioDeviceFmodEx400::IsEax(void)
{
	if (m_pAPISystem && !m_nFMODDriverCaps && m_pSoundSystem)
	{
		//int nDriver = 0;

		if (m_nSpeakerMode != FMOD_SPEAKERMODE_MAX)
			//m_ExResult = m_pFMODEX->getDriver(&nDriver);
			m_ExResult = m_pAPISystem->getDriverCaps(0, &m_nFMODDriverCaps, 0, 0, &m_nSpeakerMode);
		else
			m_ExResult = m_pAPISystem->getDriverCaps(0, &m_nFMODDriverCaps, 0, 0, 0);

		if (m_nFMODDriverCaps & FMOD_CAPS_HARDWARE_EMULATED)
			m_pSoundSystem->Log(eSLT_Always, " *WARNING* Hardware acceleration turned off sound performance settings!");

		if (m_nFMODDriverCaps & FMOD_CAPS_HARDWARE)
			m_pSoundSystem->Log(eSLT_Always, " Driver supports hardware 3D sound");
		
		if (m_nFMODDriverCaps & FMOD_CAPS_REVERB_EAX2)
			m_pSoundSystem->Log(eSLT_Always, " Driver supports EAX 2.0 reverb");

		if (m_nFMODDriverCaps & FMOD_CAPS_REVERB_EAX3)
			m_pSoundSystem->Log(eSLT_Always, " Driver supports EAX 3.0 reverb");

		if (m_nFMODDriverCaps & FMOD_CAPS_REVERB_EAX4)
			m_pSoundSystem->Log(eSLT_Always, " Driver supports EAX 4.0 reverb");
	}

	return (m_nFMODDriverCaps & (FMOD_CAPS_REVERB_EAX2 | FMOD_CAPS_REVERB_EAX3 | FMOD_CAPS_REVERB_EAX4))?true:false;
}

int  CAudioDeviceFmodEx400::GetNumberSoundsPlaying(void)
{
	int nTemp = 0;
	ptParamINT32 Param(nTemp);
	GetParam(adpCHANNELS_PLAYING, &Param);
	Param.GetValue(nTemp);

	return nTemp;
}

// percent of cpu usage by CrySound mixer,dsound, and streams
bool CAudioDeviceFmodEx400::GetCpuUsage(float *fDSP, float *fStream, float* fGeometry, float* fUpdate, float *fTotal)
{
	if (m_pAPISystem)
	{
		CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_API_INFO) );
		m_ExResult = m_pAPISystem->getCPUUsage(fDSP, fStream, fGeometry, fUpdate, fTotal);
	}

	if (IS_FMODERROR)
	{
		FmodErrorOutput("system get CPU usage failed! ", eSLT_Warning);
		return false;
	}

	return true;
}

// sets whole sound system's frequency
bool CAudioDeviceFmodEx400::SetFrequency(int newFreq)
{
  bool bTemp = true;

  m_nSystemFrequency = newFreq;

  //bTemp = (CS_SetFrequency(CS_ALL,newFreq) ? true : false);
  return bTemp;
}

// compute memory-consumption, returns rough estimate in MB
int CAudioDeviceFmodEx400::GetMemoryUsage(class ICrySizer* pSizer) const
{
	// TODO MAJOR REVIEW HERE !

	int nSizerSizeInByte = 0;
	int nWavebank = 0;

	if (pSizer)
	{		
		pSizer->AddObject(this, sizeof(*this));			
		pSizer->AddObject(m_UnusedPlatformSoundEvents);			
		pSizer->AddObject(m_UnusedPlatformSounds);	
	}
	

	// unused debug
	unsigned int nMemoryUsedEvent = 0;
	//unsigned int nMemoryUsedAPI = 0;
	unsigned int nMemoryUsedArray[FMOD_MEMTYPE_MAX] = {0};
	m_ExResult = m_pEventSystem->getMemoryInfo(FMOD_MEMBITS_ALL, FMOD_EVENT_MEMBITS_ALL, &nMemoryUsedEvent, nMemoryUsedArray);
	//m_ExResult = m_pAPISystem->getMemoryInfo(FMOD_MEMBITS_ALL, FMOD_EVENT_MEMBITS_ALL, &nMemoryUsedAPI, nMemoryUsedArray);
	// unused debug

	// API and Eventsystem both report the same numbers of sound data loaded
	//unsigned int nRSXMemoryUsedAPI = 0;
	//m_ExResult = m_pAPISystem->getMemoryInfo(FMOD_MEMBITS_SOUND, 0, &nRSXMemoryUsedAPI, 0);
	unsigned int nSoundMemoryUsedEvent = 0;
	m_ExResult = m_pEventSystem->getMemoryInfo(FMOD_MEMBITS_SOUND, 0, &nSoundMemoryUsedEvent, 0);

	//CSoundBuffer *pBuffer=m_pSoundBuffer;
	int nFMODCurrent = 0;
	int nMax = 0;
	m_ExResult = FMOD::Memory_GetStats(&nFMODCurrent, &nMax);

	if (IS_FMODERROR)
	{
		FmodErrorOutput("system get memory stats failed! ", eSLT_Warning);
		return 0;
	}

	// substract sound data stored in RSX memory









	// add static memblock
	if (m_pCacheStaticMemBlock)
	{
		// if we have a memblock, then FMODCurrent is part of the memblock
		nFMODCurrent = m_nCacheStaticMemBlockSize;
	}

	int nTempMemory = 0;

	if (pSizer)
	{
		SIZER_SUBCOMPONENT_NAME(pSizer, "FMOD Heap");

//#ifdef FMOD_MEMORY_DEBUGGING_SIMPLE
//		nSizerSizeInByte += nFMODMemUsage;
//		pSizer->AddObject(m_pAPISystem, nFMODMemUsage);
//#else
//		nSizerSizeInByte += nFMODCurrent;
//		pSizer->AddObject(m_pAPISystem, nFMODCurrent);
//#endif

		static const char* memTypes[FMOD_MEMTYPE_MAX] = {
			"FMOD_MEMTYPE_OTHER",
			"FMOD_MEMTYPE_STRING",
			"FMOD_MEMTYPE_SYSTEM",
			"FMOD_MEMTYPE_PLUGINS",
			"FMOD_MEMTYPE_OUTPUT",
			"FMOD_MEMTYPE_CHANNEL",
			"FMOD_MEMTYPE_CHANNELGROUP",
			"FMOD_MEMTYPE_CODEC",
			"FMOD_MEMTYPE_FILE",
			"FMOD_MEMTYPE_SOUND",
			"FMOD_MEMTYPE_SOUNDGROUP",
			"FMOD_MEMTYPE_STREAMBUFFER",
			"FMOD_MEMTYPE_DSPCONNECTION",
			"FMOD_MEMTYPE_DSP",
			"FMOD_MEMTYPE_DSPCODEC",
			"FMOD_MEMTYPE_PROFILE",
			"FMOD_MEMTYPE_RECORDBUFFER",
			"FMOD_MEMTYPE_REVERB",
			"FMOD_MEMTYPE_REVERBCHANNELPROPS",
			"FMOD_MEMTYPE_GEOMETRY",
			"FMOD_MEMTYPE_SYNCPOINT",
			"FMOD_MEMTYPE_EVENTSYSTEM",
			"FMOD_MEMTYPE_MUSICSYSTEM",
			"FMOD_MEMTYPE_FEV",
			"FMOD_MEMTYPE_MEMORYFSB",
			"FMOD_MEMTYPE_EVENTPROJECT",
			"FMOD_MEMTYPE_EVENTGROUPI",
			"FMOD_MEMTYPE_SOUNDBANKCLASS",
			"FMOD_MEMTYPE_SOUNDBANKLIST",
			"FMOD_MEMTYPE_STREAMINSTANCE",
			"FMOD_MEMTYPE_SOUNDDEFCLASS",
			"FMOD_MEMTYPE_SOUNDDEFDEFCLASS",
			"FMOD_MEMTYPE_SOUNDDEFPOOL",
			"FMOD_MEMTYPE_REVERBDEF",
			"FMOD_MEMTYPE_EVENTREVERB",
			"FMOD_MEMTYPE_USERPROPERTY",
			"FMOD_MEMTYPE_EVENTINSTANCE",
			"FMOD_MEMTYPE_EVENTINSTANCE_COMPLEX",
			"FMOD_MEMTYPE_EVENTINSTANCE_SIMPLE",
			"FMOD_MEMTYPE_EVENTINSTANCE_LAYER",
			"FMOD_MEMTYPE_EVENTINSTANCE_SOUND",
			"FMOD_MEMTYPE_EVENTENVELOPE",
			"FMOD_MEMTYPE_EVENTENVELOPEDEF",
			"FMOD_MEMTYPE_EVENTPARAMETER",
			"FMOD_MEMTYPE_EVENTCATEGORY",
			"FMOD_MEMTYPE_EVENTENVELOPEPOINT",
			"FMOD_MEMTYPE_EVENTINSTANCEPOOL",
			""
		};

		for (int mtIdx = 0; mtIdx != static_cast<int>(FMOD_MEMTYPE_MAX); ++ mtIdx)
		{




			{
				SIZER_SUBCOMPONENT_NAME(pSizer, memTypes[mtIdx]);
				pSizer->AddObject(m_pAPISystem + mtIdx, nMemoryUsedArray[mtIdx]);
				nSizerSizeInByte += nMemoryUsedArray[mtIdx];
			}
		}

		//pSizer->AddObject(m_pAPISystem, nFMODCurrent); // use own traced memory
		//pSizer->AddObject(m_pEventSystem, Info.instancememory);
	}

	FMOD_EVENT_SYSTEMINFO SystemInfo;

	// first get the number of wavebanks
	m_ExResult = m_pEventSystem->getInfo(&SystemInfo);
	if (IS_FMODERROR)
	{
		FmodErrorOutput("event system get system info failed! ", eSLT_Warning);
		return 0;
	}

	FMOD_EVENT_WAVEBANKINFO* pWavebankInfo = NULL;

	if (SystemInfo.maxwavebanks)
	{
		// now repeat to fill the array of wavebank infos
		pWavebankInfo = new FMOD_EVENT_WAVEBANKINFO[SystemInfo.maxwavebanks];

		if (!pWavebankInfo)
			return false;

		SystemInfo.wavebankinfo = pWavebankInfo;

		m_ExResult = m_pEventSystem->getInfo(&SystemInfo);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("event system get system info with wavebanks failed! ", eSLT_Warning);
			delete[] pWavebankInfo; // remove wavebankinfo array
			return 0;
		}
	}

	//nTempMemory = SystemInfo.eventmemory + SystemInfo.instancememory + SystemInfo.dspmemory;

	for (int i=0; i<SystemInfo.maxwavebanks; ++i)
	{
		nWavebank += pWavebankInfo[i].streammemory + pWavebankInfo[i].samplememory;
		nTempMemory += pWavebankInfo[i].streammemory + pWavebankInfo[i].samplememory;
		//if (pSizer)
		//pSizer->AddObject(&(Info.wavebankinfo[i]), Info.wavebankinfo[i].streammemory + Info.wavebankinfo[i].samplememory);
	}

	delete[] pWavebankInfo; // remove wavebankinfo array

#ifdef FMOD_MEMORY_DEBUGGING
	int nNum = MemMap.size();
	int nSum = 0;
	tMemMap::iterator ItEnd = MemMap.end();
	for (tMemMap::iterator It = MemMap.begin(); It!=ItEnd; ++It)
	{
		nSum += (*It).second;
	}
#endif



	return nSizerSizeInByte;

}

// accesses wavebanks
IWavebank* CAudioDeviceFmodEx400::GetWavebank(int nIndex)
{
	IWavebank *pWavebank = NULL;

	int nIdx=0;
	tmapWavebanks::iterator ItEnd = m_Wavebanks.end();
	for (tmapWavebanks::const_iterator It = m_Wavebanks.begin(); It!=ItEnd; ++It)
	{
		if (nIndex == nIdx)
			pWavebank = (*It).second;
		++nIdx;
	}

	return pWavebank;
}

// accesses wavebanks
IWavebank* CAudioDeviceFmodEx400::GetWavebank(const char* sWavebankName)
{
	IWavebank *pWavebank = NULL;

	tmapWavebanks::iterator It = m_Wavebanks.find(CONST_TEMP_STRING(sWavebankName));
	if (It != m_Wavebanks.end())
	{
			pWavebank = (*It).second;
	}

	return pWavebank;
}

//MVD
// creates a new platform dependent SoundBuffer
CSoundBuffer* CAudioDeviceFmodEx400::CreateSoundBuffer(const SSoundBufferProps &BufferProps)
{
	CSoundBuffer* pBuf = NULL;

	if (BufferProps.eBufferType == btEVENT)
		pBuf = new CSoundBufferFmodEx400Event(BufferProps, m_pAPISystem);

	if (BufferProps.eBufferType == btMICRO)
	{
		if (m_pSoundSystem->g_nRecordConfig == 0)
			pBuf = new CSoundBufferFmodEx400Micro(BufferProps, m_pAPISystem, 0); // use default == 0 index
		else
			pBuf = new CSoundBufferFmodEx400Micro(BufferProps, m_pAPISystem, m_pSoundSystem->g_nRecordConfig-1); // decrease recordconfig because FMOD index starts with 0
	}

	if (BufferProps.eBufferType == btNETWORK)
		pBuf = new CSoundBufferFmodEx400Network(BufferProps, m_pAPISystem);

	if (!pBuf)
		pBuf = new CSoundBufferFmodEx400(BufferProps, m_pAPISystem);

	return pBuf;
}

// creates a new platform dependent PlatformSound
IPlatformSound* CAudioDeviceFmodEx400::CreatePlatformSound(CSound* pSound, const char *sEventName)
{
	IPlatformSound* pPlatform = NULL;

	if (sEventName[0])
	{
		if (m_UnusedPlatformSoundEvents.empty())
			pPlatform = new CPlatformSoundFmodEx400Event(pSound, m_pAPISystem, sEventName);
		else
		{
			pPlatform = m_UnusedPlatformSoundEvents[m_UnusedPlatformSoundEvents.size()-1];
			pPlatform->Reset(pSound, sEventName);
			m_UnusedPlatformSoundEvents.pop_back();
		}
	}
	else
	{
		if (m_UnusedPlatformSounds.empty())
			pPlatform = new CPlatformSoundFmodEx400(pSound, m_pAPISystem);
		else
		{
			pPlatform = m_UnusedPlatformSounds[m_UnusedPlatformSounds.size()-1];
			pPlatform->Reset(pSound, "");
			m_UnusedPlatformSounds.pop_back();
		}
	}

	return pPlatform;
}

bool CAudioDeviceFmodEx400::RemovePlatformSound(IPlatformSound* pPlatformSound)
{
	if (pPlatformSound)
	{
		if (pPlatformSound->GetClass() == pscEVENT)
			m_UnusedPlatformSoundEvents.push_back((CPlatformSoundFmodEx400Event*)pPlatformSound);
		else
			m_UnusedPlatformSounds.push_back((CPlatformSoundFmodEx400*)pPlatformSound);
	}

	return true;
}

// check for multiple record devices and expose them, write name to sName pointer
bool CAudioDeviceFmodEx400::GetRecordDeviceInfo(const int nRecordDevice, char* sName, const int nNameLength)
{
	if (m_pAPISystem)
	{
		CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_API_OTHER) );
		m_pAPISystem->getRecordDriverInfo(nRecordDevice, sName, nNameLength, 0);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("retrieve microphone record information failed! ", eSLT_Warning);
			return false;
		}
	}
	return true;
}


/*
adpSOFTWARECHANNELS,
adpHARDWARECHANNELS,
adpTOTALCHANNELS,
adpEAX_STATUS,
adpMUTE_STATUS,
adpLISTENER_POSITION,
adpLISTENER_VELOCITY,
adpLISTENER_FORWARD,
adpLISTENER_TOP,
adpWINDOW_HANDLE,
adpSYSTEM_FREQUENCY,
adpMASTER_VOLUME,
adpMIXING_RATE,
adpSPEAKER_MODE,
adpPAUSED_STATUS,
adpSNDSYSTEM_MEM_USAGE_NOW,
adpSNDSYSTEM_MEM_USAGE_HIGHEST,
adpSNDSYSTEM_OUTPUTTYPE,
adpSNDSYSTEM_DRIVER
adpDOPPLER
*/
bool CAudioDeviceFmodEx400::GetParam(enumAudioDeviceParamSemantics enumtype, ptParam* pParam)
{
	int   nTemp = 0;
	float fTemp = 0.0f;
	bool  bTemp = false;;
	Vec3	vTemp;
	string sTemp;  
	void * vpTemp = NULL;
	float tempVect[3];
	bool result=true;


	switch(enumtype) 
	{
    case adpDOPPLER: // turn doppler on or off with a bool
      if (!(pParam->GetValue(bTemp))) 
				return (false);
      bTemp = m_bDopplerStatus;
      pParam->SetValue(bTemp);
      break;
    case adpHARDWARECHANNELS:// number of hardware channels available
      if (!(pParam->GetValue(nTemp))) 
				return (false);
      GetNumberSoundsPlaying();
      nTemp = m_nTotalHardwareChannelsAvail;
      pParam->SetValue(nTemp);
      break;
    case adpTOTALCHANNELS:// total number of channels available
      if (!(pParam->GetValue(nTemp))) 
				return (false);
      GetNumberSoundsPlaying();
      nTemp = m_nTotalHardwareChannelsAvail + m_InitSettings.nSoftwareChannels;
      pParam->SetValue(nTemp);
      break;
    case adpEAX_STATUS:// is eax available on this machine
      if (!(pParam->GetValue(bTemp))) 
				return (false);
      bTemp = IsEax();
      pParam->SetValue(bTemp);
      break;
    case adpMUTE_STATUS:// returns bool telling caller if system wide mute on=true or off=false
      if (!(pParam->GetValue(bTemp))) 
				return (false);
      bTemp = m_bMuteStatus;
      pParam->SetValue(bTemp);
      break;
    case adpLISTENER_POSITION:// gets active Listeners position
      if (!(pParam->GetValue(vTemp))) 
				return (false);
      tempVect[0] = tempVect[1] = tempVect[2] = 0.0f;
      //CS_3D_Listener_GetAttributes(tempVect, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
      vTemp(tempVect[0], tempVect[2], tempVect[1]);
      pParam->SetValue(vTemp);
      break;
    case adpLISTENER_VELOCITY:// gets active Listeners velocity
      if (!(pParam->GetValue(vTemp))) 
				return (false);
      tempVect[0] = tempVect[1] = tempVect[2] = 0.0f;
      //CS_3D_Listener_GetAttributes(NULL, tempVect, NULL, NULL, NULL, NULL, NULL, NULL);
      vTemp(tempVect[0], tempVect[2], tempVect[1]);
      pParam->SetValue(vTemp);
      break;
    case adpLISTENER_FORWARD:// gets active Listeners direction or forward
      if (!(pParam->GetValue(vTemp))) 
				return (false);
      tempVect[0] = tempVect[1] = tempVect[2] = 0.0f;
      //CS_3D_Listener_GetAttributes(NULL, NULL, &tempVect[0], &tempVect[1], &tempVect[2], NULL, NULL, NULL);
      vTemp(tempVect[0], tempVect[2], tempVect[1]);
      pParam->SetValue(vTemp);
      break;
    case adpLISTENER_TOP:// gets active Listeners top
      if (!(pParam->GetValue(vTemp))) 
				return (false);
      tempVect[0] = tempVect[1] = tempVect[2] = 0.0f;
      //CS_3D_Listener_GetAttributes(NULL, NULL, NULL, NULL, NULL, &tempVect[0], &tempVect[1], &tempVect[2]);
      vTemp(tempVect[0], tempVect[2], tempVect[1]);
      pParam->SetValue(vTemp);
      break;
    case adpWINDOW_HANDLE:// returns handle of active window which is void * type
      if (!(pParam->GetValue(vpTemp))) 
				return (false);
      vpTemp = m_pHWnd;
      pParam->SetValue(vpTemp);
      break;
    case adpSYSTEM_FREQUENCY:// gets the mixing speed
      if (!(pParam->GetValue(nTemp))) 
				return (false);
      //nTemp = CS_GetFrequency(CS_ALL);
      pParam->SetValue(nTemp);
			break;
		case adpMASTER_VOLUME: // value from 0-1, 1 being max fmod master volume
			{
				if (!(pParam->GetValue(fTemp))) 
					return (false);

				CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_API_OTHER) );
				FMOD::ChannelGroup *MasterChannelGroup = 0;
				m_ExResult = m_pAPISystem->getMasterChannelGroup(&MasterChannelGroup);
				if (!IS_FMODERROR)
				{
					m_ExResult = MasterChannelGroup->getVolume(&fTemp);
				}
				pParam->SetValue(nTemp);
				break;
			}
		case adpMASTER_PAUSE: // value true/false
			{
				if (!(pParam->GetValue(bTemp))) 
					return (false);

				CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_API_OTHER) );
				FMOD::ChannelGroup *MasterChannelGroup = 0;
				m_ExResult = m_pAPISystem->getMasterChannelGroup(&MasterChannelGroup);
				if (!IS_FMODERROR)
				{
					m_ExResult = MasterChannelGroup->getPaused(&bTemp);
				}
				pParam->SetValue(nTemp);
				break;
			}
    case adpSPEAKER_MODE:// gets active speaker mode
      if (!(pParam->GetValue(nTemp))) 
				return (false);
			switch (m_nSpeakerMode)
			{
			case FMOD_SPEAKERMODE_MONO :
				nTemp = 1;
				break;
			case FMOD_SPEAKERMODE_STEREO :
				nTemp = 2;
				break;
			case FMOD_SPEAKERMODE_QUAD :
				nTemp = 4;
				break;
			case FMOD_SPEAKERMODE_5POINT1 :
				nTemp = 5;
				break;
			case FMOD_SPEAKERMODE_7POINT1 :
				nTemp = 7;
				break;
			case FMOD_SPEAKERMODE_PROLOGIC :
				nTemp = 9;
				break;
			}
      pParam->SetValue(nTemp);
      break;
    case adpPAUSED_STATUS:// is fmod paused or not
      if (!(pParam->GetValue(bTemp))) 
				return (false);
      bTemp = m_bSystemPaused;
      pParam->SetValue(bTemp);
      break;
    case adpSNDSYSTEM_MEM_USAGE_NOW:// returns how much memory fmod is using RIGHT NOW
      if (!(pParam->GetValue(nTemp))) 
				return (false);
      GetMemoryStats(admqALL, 0);
      nTemp = m_nCurrentMemAlloc;
      pParam->SetValue(nTemp);
      break;
    case adpSNDSYSTEM_MEM_USAGE_HIGHEST:// returns the amount of memory fmod used at its PEEK
      if (!(pParam->GetValue(nTemp))) 
				return (false);
      GetMemoryStats(admqALL, 0);
      nTemp = m_nMaxMemAlloc;
      pParam->SetValue(nTemp);
      break;
    case adpSNDSYSTEM_OUTPUTTYPE:// returns a enum in int form of output type
      /*
      CS_OUTPUT_NOSOUND,     NoSound driver, all calls to this succeed but do nothing.
      CS_OUTPUT_WINMM,     Windows Multimedia driver. 
      CS_OUTPUT_DSOUND,    DirectSound driver.  You need this to get EAX2 or EAX3 support, or FX api support. 
      CS_OUTPUT_A3D,       A3D driver. 

      CS_OUTPUT_OSS,        Linux/Unix OSS (Open Sound System) driver, i.e. the kernel sound drivers. 
      CS_OUTPUT_ESD,        Linux/Unix ESD (Enlightment Sound Daemon) driver. 
      CS_OUTPUT_ALSA,       Linux Alsa driver. 

      CS_OUTPUT_ASIO,       Low latency ASIO driver 
      CS_OUTPUT_XBOX,       Xbox driver 
      CS_OUTPUT_PS2,        PlayStation 2 driver 
      CS_OUTPUT_MAC,        Mac SoundManager driver 
      CS_OUTPUT_GC,         Gamecube driver 
      CS_OUTPUT_NOSOUND_NONREALTIME   This is the same as nosound, but the sound generation is driven by CS_Update 
      */
      if (!(pParam->GetValue(nTemp))) 
				return (false);
      //nTemp = CS_GetOutput();
      pParam->SetValue(nTemp);
      break;
    case adpSNDSYSTEM_DRIVER:// sets a string name of driver
      if (!(pParam->GetValue(sTemp))) 
				return (false);
      //sTemp = CS_GetDriverName(CS_GetOutput());
      pParam->SetValue(sTemp);
      break;
    case adpMIXING_BUFFER_SIZE:// ****** OPTIONAL ****** if set returns mixing buffer size in milliseconds (fmod does automaticly)
      if (!(pParam->GetValue(nTemp))) 
				return (false);
      nTemp = m_nMixingBuffSize;
      pParam->SetValue(nTemp);
			break;
		case adpCHANNELS_PLAYING:// returns number of channels being used
			{
				if (!(pParam->GetValue(nTemp)) || !m_pAPISystem) 
					return (false);

				CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_API_INFO) );
				m_ExResult = m_pAPISystem->getChannelsPlaying(&nTemp);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("system get channels playing failed! ", eSLT_Warning);
					return false;
				}

				pParam->SetValue(nTemp);
				return false; // disable FMOD-EX polling because event system is ignored (BUG)
				break;
			}
		case adpSTOPALL_CHANNELS:// ******* NOT ACTIVE !!!!!
			break;
		case adpEVENTCOUNT:
			if (!(pParam->GetValue(nTemp))) 
				return (false);
			nTemp = m_nCountEvent;
			pParam->SetValue(nTemp);
			break;
		case adpGROUPCOUNT:
			if (!(pParam->GetValue(nTemp))) 
				return (false);
			nTemp = m_nCountGroup;
			pParam->SetValue(nTemp);
			break;
    default:
      break;
    }
    return result;
	}
	bool CAudioDeviceFmodEx400::SetParam(enumAudioDeviceParamSemantics enumtype, ptParam* pParam)
	{
		int   nTemp = 0;
		float fTemp = 0.0f;
		bool  bTemp = false;
		Vec3	vTemp;
		string sTemp;  
		void * vpTemp = NULL;
		bool result = true;

		switch(enumtype) 
		{
		case adpHARDWARECHANNELS:
		case adpTOTALCHANNELS:
		case adpEAX_STATUS:          // these values are not used at this time
    case adpCHANNELS_PLAYING:
    case adpSNDSYSTEM_MEM_USAGE_NOW:
    case adpSNDSYSTEM_MEM_USAGE_HIGHEST:
      result = false;
      break;
    case adpMIXING_BUFFER_SIZE:// ****OPTIONAL**** set the size of the mixing buffer in milliSeconds 
      if (!(pParam->GetValue(nTemp))) 
				return (false);
      m_nMixingBuffSize = nTemp;
      //result = (CS_SetBufferSize(nTemp) ? true : false);
      break;
    case adpMUTE_STATUS:// changes the system mute status ,bTemp true mutes the system false reactivates sound system
      if (!(pParam->GetValue(bTemp))) 
				return (false);
      m_bMuteStatus = bTemp;
      //result = (CS_SetMute(CS_ALL, (signed char)bTemp) ? true : false);
			break;

		case adpLISTENER_POSITION:// sets listener position
			return (false);

			break;
		case adpLISTENER_VELOCITY:// sets 3d listener velocity
			return (false);

			break;
		case adpLISTENER_FORWARD:// sets 3d listener forward value
			return (false);

			break;
		case adpLISTENER_TOP:// sets 3d listener top value
			return (false);

      break;
    case adpWINDOW_HANDLE:// sets fmod window output handle
      if (!(pParam->GetValue(vpTemp))) 
				return (false);
      //result = (CS_SetHWND(vpTemp) ? true : false);
      m_pHWnd = vpTemp;
      break;
    case adpSYSTEM_FREQUENCY:// sets fmod global mixing speed
      if (!(pParam->GetValue(nTemp))) 
				return (false);
      m_nSystemFrequency = nTemp;
			//result = (CS_SetFrequency(CS_ALL, nTemp) ? true : false);
			break;
		case adpMASTER_VOLUME:// sets the fmod master volume 0 - 1, 1 being highest volume
			{
				if (!(pParam->GetValue(fTemp)) || !m_pAPISystem) 
					return (false);

				if (fTemp < 0.0f)
					fTemp = 0.0f;
				else if (fTemp > 1.0f)
					fTemp = 1.0f;

				CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_API_OTHER) );
				FMOD::ChannelGroup *pChannelGroup = 0;
				m_ExResult = m_pAPISystem->getMasterChannelGroup(&pChannelGroup);

				if (IS_FMODERROR)
				{
					FmodErrorOutput("system get master channel group failed! ", eSLT_Warning);
					return false;
				}
					
				m_ExResult = pChannelGroup->setVolume(fTemp);
				
				if (IS_FMODERROR)
				{
					FmodErrorOutput("master channel group set volume failed! ", eSLT_Warning);
					return false;
				}

				break;
			}
		case adpMASTER_PAUSE: // value true/false
			{
				if (!(pParam->GetValue(bTemp))) 
					return (false);

				CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_API_OTHER) );
				FMOD::ChannelGroup *MasterChannelGroup = 0;
				m_ExResult = m_pAPISystem->getMasterChannelGroup(&MasterChannelGroup);
				
				if (IS_FMODERROR)
				{
					FmodErrorOutput("system get master channel group failed! ", eSLT_Warning);
					return false;
				}

				m_ExResult = MasterChannelGroup->setPaused(bTemp);
				if (IS_FMODERROR)
				{
					FmodErrorOutput("master channel group set pause failed! ", eSLT_Warning);
					return false;
				}

				if (m_pEventSystem)
				{
					FMOD::EventCategory *pMasterCategory = NULL;
					m_ExResult = m_pEventSystem->getCategory("master", &pMasterCategory);

					if (IS_FMODERROR)
					{
						FmodErrorOutput("get master category failed! ", eSLT_Warning);
						return false;
					}

					m_ExResult = pMasterCategory->setPaused(bTemp);
					if (IS_FMODERROR)
					{
						FmodErrorOutput("master category set pause failed! ", eSLT_Warning);
						return false;
					}
				}

				break;
			}
		case adpPITCH:
			{
				if (!(pParam->GetValue(fTemp)) || !m_pEventSystem) 
					return (false);

				if (fTemp < -4.0f)
					fTemp = -4.0f;
				else if (fTemp > 4.0f)
					fTemp = 4.0f;

				FMOD::EventCategory *pPlatformCategory = NULL;
				m_ExResult = m_pEventSystem->getCategory("master", &pPlatformCategory);

				if (IS_FMODERROR)
				{
					FmodErrorOutput("system get master category failed! ", eSLT_Warning);
					return false;
				}

				m_ExResult = pPlatformCategory->setPitch(fTemp);

				if (IS_FMODERROR)
				{
					FmodErrorOutput("master category set pitch failed! ", eSLT_Warning);
					return false;
				}

				break;
			}

		case adpSPEAKER_MODE:// sets fmod speaker enum
			
		//	typedef enum
		//	{
		//	FMOD_SPEAKERMODE_RAW,              /* There is no specific speakermode.  Sound channels are mapped in order of input to output.  See remarks for more information. */
		//	FMOD_SPEAKERMODE_MONO,             /* The speakers are monaural. */
		//	FMOD_SPEAKERMODE_STEREO,           /* The speakers are stereo (default value). */
		//	FMOD_SPEAKERMODE_4POINT1,          /* 4.1 speaker setup.  This includes front, center, left, rear and a subwoofer. Also known as a "quad" speaker configuration. */
		//	FMOD_SPEAKERMODE_5POINT1,          /* 5.1 speaker setup.  This includes front, center, left, rear left, rear right and a subwoofer. */
		//	FMOD_SPEAKERMODE_7POINT1,          /* 7.1 speaker setup.  This includes front, center, left, rear left, rear right, side left, side right and a subwoofer. */
		//	FMOD_SPEAKERMODE_PROLOGIC,         /* Stereo output, but data is encoded in a way that is picked up by a Prologic/Prologic2 decoder and split into a 5.1 speaker setup. */

		//	FMOD_SPEAKERMODE_MAX,              /* Maximum number of speaker modes supported. */
		//	FMOD_SPEAKERMODE_FORCEINT = 65536  /* Makes sure this enum is signed 32bit. */
		//} FMOD_SPEAKERMODE;
	
				//" 0: Control Panel Settings\n"
				//"	1: Mono\n"
				//"	2: Stereo\n"
				//"	3: Headphone\n"
				//"	4: 4Point1\n"
				//"	5: 5Point1\n"
				////"	6: Surround\n"
				//"	7: 7Point1\n"
				//"	9: Prologic\n"

      // ***** Setspeakermode also sets the PAN separation setting to -0- if set to mono and -1- if stereo
      if (!(pParam->GetValue(nTemp))) 
				return (false);
			switch (nTemp)
			{
			case 0 :
				IsEax(); // polls the state of the control panel;
			 break;
			case 1 :
				m_nSpeakerMode = FMOD_SPEAKERMODE_MONO;
				break;
			case 2 :
				m_nSpeakerMode = FMOD_SPEAKERMODE_STEREO;
				break;
			case 3 :
				m_nSpeakerMode = FMOD_SPEAKERMODE_STEREO;
				break;
			case 4 :
				m_nSpeakerMode = FMOD_SPEAKERMODE_QUAD;
				break;
			case 5 :
				m_nSpeakerMode = FMOD_SPEAKERMODE_5POINT1;
				break;
			case 6 :
				m_nSpeakerMode = FMOD_SPEAKERMODE_PROLOGIC;
				break;
			case 7 :
				m_nSpeakerMode = FMOD_SPEAKERMODE_7POINT1;
				break;
			}
			//ResetAudio();
			//m_ExResult = m_pFMODEX->setSpeakerMode(m_nSpeakerMode);
      break;
    case adpPAUSED_STATUS:// pauses or un pauses system
      if (!(pParam->GetValue(bTemp))) 
				return (false);
      m_bSystemPaused = bTemp;
      //result = (CS_SetPaused(CS_ALL, bTemp) ? true : false);
      break;
/*
      CS_OUTPUT_NOSOUND,     NoSound driver, all calls to this succeed but do nothing.
        CS_OUTPUT_WINMM,     Windows Multimedia driver. 
        CS_OUTPUT_DSOUND,    DirectSound driver.  You need this to get EAX2 or EAX3 support, or FX api support. 
        CS_OUTPUT_A3D,       A3D driver. 

        CS_OUTPUT_OSS,        Linux/Unix OSS (Open Sound System) driver, i.e. the kernel sound drivers. 
        CS_OUTPUT_ESD,        Linux/Unix ESD (Enlightment Sound Daemon) driver. 
        CS_OUTPUT_ALSA,       Linux Alsa driver. 

        CS_OUTPUT_ASIO,       Low latency ASIO driver 
        CS_OUTPUT_XBOX,       Xbox driver 
        CS_OUTPUT_PS2,        PlayStation 2 driver 
        CS_OUTPUT_MAC,        Mac SoundManager driver 
        CS_OUTPUT_GC,         Gamecube driver 
        CS_OUTPUT_NOSOUND_NONREALTIME   This is the same as nosound, but the sound generation is driven by CS_Update 
        */
    case adpSNDSYSTEM_OUTPUTTYPE:// **** OPTIONAL ***** (-1 lets fmod pick best one) sets the fmod mixer driver type 
    case adpSNDSYSTEM_DRIVER: 
      if (!(pParam->GetValue(nTemp))) 
				return (false);
      
      // -1 tells the fmod system to pick the best one -- these are only valid choices
      if ((nTemp == -1) || (nTemp == FMOD_OUTPUTTYPE_NOSOUND) || (nTemp == FMOD_OUTPUTTYPE_WINMM) || (nTemp == FMOD_OUTPUTTYPE_DSOUND))
        //result = (CS_SetOutput(nTemp) ? true : false);
      break;

      case adpSTOPALL_CHANNELS:// stops all channels
				//m_pFMODEX->
        //result = (CS_StopSound(CS_ALL) ? true : false);  
        break;

      case adpDOPPLER: // turn doppler on or off with a bool
        if (!(pParam->GetValue(bTemp))) 
					return (false);
        m_bDopplerStatus = bTemp;
        break;

    default:
      break;
    }
  return result;
  }

	// return index of Project despite if it is available 
	ProjectIndex CAudioDeviceFmodEx400::LoadProjectFile(const string &sProjectPath, const string &sProjectName, bool bAsync)
	{
		MEMSTAT_CONTEXT_FMT(EMemStatContextTypes::MSC_SoundProject, 0, "%s (%s)", sProjectName.c_str(), sProjectPath.c_str());

		VecProjectFilesIter IterProject = m_VecLoadedProjectFiles.end();
		SProjectFile* pProjectFile = NULL;

		if (m_pEventSystem)
		{
			string sFull = sProjectPath + sProjectName;
			
			VecProjectFilesIter IterEnd = m_VecLoadedProjectFiles.end();
			for (VecProjectFilesIter Iter = m_VecLoadedProjectFiles.begin(); Iter!=IterEnd; ++Iter)
			{
				if (sFull == (*Iter).sProjectName)
				{
					if ((*Iter).pProjectHandle != PROJECTHANDLE_INVALID)
						return (*Iter).nProjectIndex;
					else
					{
						pProjectFile = &(*Iter);
						break;
					}
				}
			}

			if (!pProjectFile)
			{
				// This is a new Project and was not found yet
				SProjectFile NewProject;
				NewProject.nProjectIndex = m_VecLoadedProjectFiles.size();;
				NewProject.sProjectName = sFull;
				NewProject.pProjectHandle = PROJECTHANDLE_INVALID;
				NewProject.pProjectFileInMemory = 0;
				NewProject.pReadStream = 0;
				NewProject.nRefCount = 0;
				NewProject.tLastTimeUsed = gEnv->pTimer->GetFrameStartTime();

				m_VecLoadedProjectFiles.push_back(NewProject);

				pProjectFile = (SProjectFile*)&(m_VecLoadedProjectFiles[NewProject.nProjectIndex]);
			}
			
			if (pProjectFile)
			{
				if (pProjectFile->pProjectHandle != PROJECTHANDLE_INVALID || pProjectFile->pReadStream)
					return pProjectFile->nProjectIndex;

				// deactive SetMediaPath as long as FMOD can not track individual
				// FEV Paths internaly if the file is loaded from memory
				//m_ExResult = m_pEventSystem->setMediaPath((char*)sProjectPath.c_str());
				//if (IS_FMODERROR)
				//{
				//	FmodErrorOutput("set event media path failed! ", eSLT_Warning);
				//	return NULL;
				//}

				//{
				//{
				//	if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_RECORD_COMMANDS)
				//		m_CommandPlayer.LogCommand(CCommandPlayerFmodEx400::EVENTSYSTEM_SETMEDIAPATH, (int)gEnv->pTimer->GetFrameStartTime().GetMilliSeconds(), (void*) sProjectPath.c_str());

				//	CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_API_OTHER) );
				//	m_ExResult = m_pEventSystem->setMediaPath((char*)sProjectPath.c_str());
				//}
				//if (IS_FMODERROR)
				//{
				//	FmodErrorOutput("set event media path failed! ", eSLT_Warning);
				//	return NULL;
				//}

				TFixedResourceName ProjectPathAndName = sProjectPath.c_str();
				ProjectPathAndName += sProjectName.c_str();

				uint32 nSize = gEnv->pCryPak->FGetSize(ProjectPathAndName.c_str());

				if (!pProjectFile->pProjectFileInMemory && nSize)
				{
					pProjectFile->pProjectFileInMemory = new int8[ nSize ];
				}

				if (!pProjectFile->pProjectFileInMemory)
					return pProjectFile->nProjectIndex;

				StreamReadParams StrParams;
				StrParams.nFlags = 0;
				StrParams.dwUserData = pProjectFile->nProjectIndex;
				StrParams.nLoadTime = 0;
				StrParams.nMaxLoadTime = 0;
				StrParams.nPriority = 0;
				StrParams.pBuffer = pProjectFile->pProjectFileInMemory;
				StrParams.nOffset = 0;
				StrParams.nSize = nSize;
				pProjectFile->pReadStream = m_pSoundSystem->m_pStreamEngine->StartRead(eStreamTaskTypeSound, ProjectPathAndName.c_str(), this, &StrParams);

				if (!bAsync)
					pProjectFile->pReadStream->Wait();

				return pProjectFile->nProjectIndex;
			}
		}

		return PROJECTINDEX_INVALID;
	}

	//////////////////////////////////////////////////////////////////////////
	void CAudioDeviceFmodEx400::StreamOnComplete(IReadStream *pStream, unsigned nError)
	{
		FUNCTION_PROFILER( gEnv->pSystem,PROFILE_SOUND );

		unsigned int nProjectIndex = (unsigned int) pStream->GetUserData();

		VecProjectFilesIter IterEnd = m_VecLoadedProjectFiles.end();
		VecProjectFilesIter Iter;
		for (Iter = m_VecLoadedProjectFiles.begin(); Iter!=IterEnd; ++Iter)
		{
			if (nProjectIndex == (*Iter).nProjectIndex)
			{
				(*Iter).pReadStream = NULL;

				if (nError)
				{
					if (nError != ERROR_USER_ABORT)
					{
						FmodErrorOutput("load event project failed! " + (*Iter).sProjectName, eSLT_Error);
						assert(!"FMOD Project not found!");
					}

					// Remove the data
					if ((*Iter).pProjectFileInMemory)
					{
						delete[] (*Iter).pProjectFileInMemory;
						(*Iter).pProjectFileInMemory = NULL;
						(*Iter).pProjectHandle = PROJECTHANDLE_INVALID;
					}
				}
				else
				{
					if (m_pSoundSystem->g_nDebugSound == SOUNDSYSTEM_DEBUG_SIMPLE)
						m_pSoundSystem->Log(eSLT_Message, "<Sound> AD: Project loaded: %s", (*Iter).sProjectName.c_str());

					FMOD_EVENT_LOADINFO Info;
					FMOD::EventProject *pProject = NULL;

					Info.size = sizeof(Info);
					Info.loadfrommemory_length = pStream->GetBytesRead();
					Info.sounddefentrylimit = m_pSoundSystem->g_fVariationLimiter;
					//Info.sizeof_instancepool_simple = 50; //TODO find good value, if we have simple events

					m_ExResult = m_pEventSystem->load((char*)(*Iter).pProjectFileInMemory, &Info, &pProject);

					if (IS_FMODERROR)
					{
						FmodErrorOutput("load event project failed! " + (*Iter).sProjectName, eSLT_Error);
						assert(!"FMOD Project not found!");
						return;
					}

					// FMOD creates a internal copy, so memory can be freed again
					if ((*Iter).pProjectFileInMemory)
					{
						delete[] (*Iter).pProjectFileInMemory;
						(*Iter).pProjectFileInMemory = NULL;
					}

					(*Iter).pProjectHandle = pProject;

				// refresh categories after loading a new project
				if (m_pSoundSystem->GetIMoodManager())
					m_pSoundSystem->GetIMoodManager()->RefreshCategories();

				// informs about project availability
				if (m_pSoundSystem->GetISoundAssetManager())
					m_pSoundSystem->GetISoundAssetManager()->ProjectWasLoaded((*Iter).nProjectIndex);

				// update group and event counting
				UpdateGroupEventCount();

				}
			}
		} // for
					
		return;
	}

	// releases the data of a project but keeps it known to the system 
	bool CAudioDeviceFmodEx400::UnloadProjectFile(const ProjectIndex nProjectIndex)
	{
		VecProjectFilesIter IterEnd = m_VecLoadedProjectFiles.end();
		VecProjectFilesIter Iter;
		for (Iter = m_VecLoadedProjectFiles.begin(); Iter!=IterEnd; ++Iter)
		{
			if (nProjectIndex == (*Iter).nProjectIndex)
			{
				if ((*Iter).pProjectHandle != PROJECTHANDLE_INVALID)
				{
					FMOD::EventProject* pProject = (FMOD::EventProject*)(*Iter).pProjectHandle;
					m_ExResult = pProject->release();
				}

				if ((*Iter).pReadStream)
				{
					(*Iter).pReadStream->Abort();
					(*Iter).pReadStream = 0;
				}

				(*Iter).pProjectHandle = PROJECTHANDLE_INVALID;

				return true;
			}
		}

		return false;
	}


	// retrieves the project entry by the project index 
	SProjectFile* CAudioDeviceFmodEx400::GetProjectFile(const ProjectIndex nProjectIndex)
	{
		VecProjectFilesIter IterEnd = m_VecLoadedProjectFiles.end();
		VecProjectFilesIter Iter;
		for (Iter = m_VecLoadedProjectFiles.begin(); Iter!=IterEnd; ++Iter)
		{
			if (nProjectIndex == (*Iter).nProjectIndex)
			{
				SProjectFile* pProjectFile = &(*Iter);

				// renew the time
				(*Iter).tLastTimeUsed = gEnv->pTimer->GetFrameStartTime();

				return pProjectFile;
			}
		}

		return NULL;
	}

	void CAudioDeviceFmodEx400::UpdateGroupEventCount()
	{
		m_nCountProject = 0;
		m_nCountGroup = 0;
		m_nCountEvent = 0;
		int nProjectIndex = 0;
		int nGroupIndex = 0;
		m_ExResult = FMOD_OK;
		FMOD::EventProject* pProject = NULL;
		FMOD::EventGroup*		pGroup = NULL;

		TFixedResourceName sReferenceName;

		if (m_pSoundSystem->g_nDumpEventStructure)
		{
			if (m_pEventDumpLogger)
				delete m_pEventDumpLogger;

			m_pEventDumpLogger = (CDebugLogger*) new CDebugLogger("EventStructure.txt");
		}

		while(m_ExResult == FMOD_OK)
		{
			{
				CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_API_OTHER) );
				m_ExResult = m_pEventSystem->getProjectByIndex(nProjectIndex, &pProject);
			}
			if (IS_FMODERROR)
			{
				//FmodErrorOutput("event system get project by index failed! ", eSLT_Warning);
			}

			if (pProject && (m_ExResult == FMOD_OK))
			{
				if (m_pSoundSystem->g_nDumpEventStructure)
				{
					sReferenceName = "Sounds/";
					char *sProjectName = 0; 
					m_ExResult = pProject->getInfo(0, &sProjectName);
					sReferenceName += sProjectName;
					sReferenceName += ":";
				}
				
				while(m_ExResult == FMOD_OK)
				{
					{
						CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_API_OTHER) );
						m_ExResult = pProject->getGroupByIndex(nGroupIndex, false, &pGroup);
					}

					if (IS_FMODERROR)
					{
						//FmodErrorOutput("project get group by index failed! ", eSLT_Warning);
					}

					if (pGroup && (m_ExResult == FMOD_OK))
					{
						++nGroupIndex;
						++m_nCountGroup;
						GroupEventCount(pGroup, pProject, sReferenceName);
					}
				}
				nGroupIndex = 0;
				m_ExResult = FMOD_OK;
				++nProjectIndex;
				++m_nCountProject;
			}
		}
	}

	void CAudioDeviceFmodEx400::GroupEventCount(FMOD::EventGroup* pParentGroup, FMOD::EventProject* pProject, TFixedResourceName sReferenceName)
	{
		int nGroupIndex = 0;
		m_ExResult = FMOD_OK;
		FMOD::EventGroup* pGroup = NULL;
		FMOD::Event* pEvent = NULL;

		TFixedResourceName sLocalReferenceName = sReferenceName;

		if (m_pSoundSystem->g_nDumpEventStructure)
		{
			char *sGroupName = 0; 
			m_ExResult = pParentGroup->getInfo(0, &sGroupName);
			sLocalReferenceName += sGroupName;
		}

		// counting Groups
		while(m_ExResult == FMOD_OK)
		{
			{
				CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_API_OTHER) );
				m_ExResult = pParentGroup->getGroupByIndex(nGroupIndex, false, &pGroup);
			}
			if (IS_FMODERROR)
			{
				//FmodErrorOutput("group get group by index failed! ", eSLT_Warning);
			}

			if (pGroup && (m_ExResult == FMOD_OK))
			{
				TFixedResourceName sLocalReferenceNameSubGroup;

				if (m_pSoundSystem->g_nDumpEventStructure)
				{
					sLocalReferenceNameSubGroup = sLocalReferenceName;
					sLocalReferenceNameSubGroup += "/";
				}
				++nGroupIndex;
				++m_nCountGroup;
				GroupEventCount(pGroup, pProject, sLocalReferenceNameSubGroup);
			}
		}

		// counting Events
		int nNumEvents = 0;
		{
			CFrameProfilerSection SoundFrameProfilerSection( CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection_API_OTHER) );
			m_ExResult = pParentGroup->getNumEvents( &nNumEvents);
		}

		if (IS_FMODERROR)
		{
			//FmodErrorOutput("group get num. events failed! ", eSLT_Warning);
		}

		if (m_pSoundSystem->g_nDumpEventStructure)
		{
			TFixedResourceName sLocalReferenceNameEvent;

			for (int i=0; i<nNumEvents; ++i)
			{
				m_ExResult = pParentGroup->getEventByIndex(i, FMOD_EVENT_INFOONLY, &pEvent);
				
				if (IS_FMODERROR)
				{
					FmodErrorOutput("group get event by index failed! ", eSLT_Warning);
				}

				if (pEvent)
				{
					char *sEventName = 0;
					m_ExResult = pEvent->getInfo(0, &sEventName, 0);

					if (m_ExResult == FMOD_OK && m_pEventDumpLogger)
					{
						sLocalReferenceNameEvent = sLocalReferenceName;
						sLocalReferenceNameEvent += ":";
						sLocalReferenceNameEvent += sEventName;
						m_pEventDumpLogger->LogString(sLocalReferenceNameEvent.c_str());
					}


				}

			}
			
		}

		if (!IS_FMODERROR)
			m_nCountEvent += nNumEvents;

	}

	// writes output to screen in debug
	void CAudioDeviceFmodEx400::DrawInformation(IRenderer* pRenderer, float xpos, float ypos, int nSoundInfo)
	{
		float fColor[4] = {1.0f, 1.0f, 1.0f, 0.7f};
		float fWhite[4] = {1.0f, 1.0f, 1.0f, 0.7f};
		float fBlue[4] = {0.0f, 0.0f, 1.0f, 0.7f};
		float fCyan[4] = {0.0f, 1.0f, 1.0f, 0.7f};
		float fColorOrange[4]	={1.0f, 0.5f, 0.0f, 0.7f};
		(void)fWhite;		(void)fBlue;	(void)fCyan;

		if (nSoundInfo == 7)
		{
			uint32 nUsedMem = 0;

			IWavebank *pWavebank = NULL;

			ypos += 10;

			tmapWavebanks::iterator ItEnd = m_Wavebanks.end();
			for (tmapWavebanks::const_iterator It = m_Wavebanks.begin(); It!=ItEnd; ++It)
			{
				pWavebank = (*It).second;
				nUsedMem += pWavebank->GetInfo()->nMemCurrentlyInByte;
				if (pWavebank->GetInfo()->nMemCurrentlyInByte > 0)
				{
					pRenderer->Draw2dLabel(xpos, ypos, 1.5, fColor, false, "%s Mem: %dKB File: %dKB", pWavebank->GetName(), pWavebank->GetInfo()->nMemCurrentlyInByte/1024, pWavebank->GetInfo()->nFileSize/1024);
					ypos += 10;
				}
			}

			ypos += 10;
			pRenderer->Draw2dLabel(xpos, ypos, 1.5, fColor, false, "Total Wavebank Memory: %.1f MB", (nUsedMem/1024)/1024.0f);
			ypos += 10;
		}

		if (nSoundInfo == 1)
		{
			uint32 nFileSeeks = 0;

#ifdef FMOD_STREAMING_DEBUGGING
			nFileSeeks = s_nFMODFileSeekCount;
			CTimeValue const tTimeDif = gEnv->pTimer->GetFrameStartTime() - m_tLastBandwidthTime;
			float const fSecs	= tTimeDif.GetSeconds();

			if (abs(fSecs) > 1.0f)
			{
				m_fStreamKBPerSecond = (s_nFMODFileReadBandwidth / fSecs) / 1024.0f;
				s_nFMODFileReadBandwidth = 0;
				m_tLastBandwidthTime = gEnv->pTimer->GetFrameStartTime();
			}
#endif

			pRenderer->Draw2dLabel(xpos, ypos, 1.5, fColor, false, "Channel sounds: %d, Event sounds: %d", m_UnusedPlatformSounds.size(), m_UnusedPlatformSoundEvents.size() );
			ypos += 12;
			pRenderer->Draw2dLabel(xpos, ypos, 1.5, fColor, false, "Open file handles: %d", s_nFileOpenCount );
			ypos += 12;
			pRenderer->Draw2dLabel(xpos, ypos, 1.5, fColor, false, "Stream Bandwidth %.1f KBs/sec, File Seeks: %d", m_fStreamKBPerSecond, nFileSeeks );
			ypos += 12;

			if (m_pEventSystem)
			{
				int nNum = 64;
				FMOD_EVENT_SYSTEMINFO SystemInfo;
				memset(&SystemInfo, 0, sizeof(FMOD_EVENT_SYSTEMINFO));

				SystemInfo.numplayingevents = nNum;
				FMOD_EVENT* PlayingEvents[64];
				SystemInfo.playingevents = PlayingEvents;
				
				m_ExResult = m_pEventSystem->getInfo( &SystemInfo );
				if (IS_FMODERROR)
				{
					FmodErrorOutput("event system get system info failed! ", eSLT_Warning);
					return;
				}

				pRenderer->Draw2dLabel(xpos, ypos, 1, fColor, false, "----- Event Names %d -----", SystemInfo.numplayingevents );
				//pRenderer->Draw2dLabel(xpos, ypos, 1.5, fColor, false, "Playing Events: %d", );
				ypos += 10;

				for (int i=0; i<SystemInfo.numplayingevents; ++i)
				{
					char *sName; 
					FMOD::Event* pEvent = (FMOD::Event*)SystemInfo.playingevents[i];
					FMOD_EVENT_INFO EventInfo;
					FMOD_EVENT_WAVEBANKINFO WavebankInfo = {{0},0};
					EventInfo.maxwavebanks = 1;
					EventInfo.wavebankinfo = &WavebankInfo;

					m_ExResult = pEvent->getInfo(0, &sName, &EventInfo);

					if (IS_FMODERROR)
					{
						FmodErrorOutput("event get info failed! ", eSLT_Warning);
					}
					else
					{
						pRenderer->Draw2dLabel(xpos, ypos, 1.2f, WavebankInfo.type?fColor:fColorOrange, false, "%s in %s", sName, WavebankInfo.name);
						ypos += 10;
					}
				}
			}
		}
	}

	CFrameProfiler* CAudioDeviceFmodEx400::GetFMODFrameProfiler(adfpFMODProfileSection enumProfileSection)
	{
		switch (enumProfileSection)
		{
		case adfpFMODProfileSection_API_INFO:
			return s_pFrameProfiler_API_Info;
			break;
		case adfpFMODProfileSection_API_OTHER:
			return s_pFrameProfiler_API_Other;
			break;
		case adfpFMODProfileSection_BUFFER_OTHER:
			return s_pFrameProfiler_Buffer_Other;
			break;
		case adfpFMODProfileSection_BUFFER_RELEASE:
			return s_pFrameProfiler_Buffer_Release;
			break;
		case adfpFMODProfileSection_EVENT_OTHER:
			return s_pFrameProfiler_Event_Other;
			break;
		}

		return 0;
	}


	// workaround for looping weapon sound MP bug
	void CAudioDeviceFmodEx400::FindLostEvent()
	{
		if (m_pEventSystem)
		{
			int nNum = 64;
			FMOD_EVENT_SYSTEMINFO SystemInfo;
			//memset(&SystemInfo, 0, sizeof(FMOD_EVENT_SYSTEMINFO));

			SystemInfo.numplayingevents = nNum;
			FMOD_EVENT* PlayingEvents[64];
			SystemInfo.playingevents = PlayingEvents;

			m_ExResult = m_pEventSystem->getInfo( &SystemInfo );
			if (IS_FMODERROR)
			{
				FmodErrorOutput("event system get system info failed! ", eSLT_Warning);
				return;
			}

			for (int i=0; i<SystemInfo.numplayingevents; ++i)
			{
				char *sName; 
				FMOD::Event* pEvent = (FMOD::Event*)SystemInfo.playingevents[i];
				FMOD_EVENT_INFO EventInfo;
				//memset(&EventInfo, 0, sizeof(FMOD_EVENT_INFO));

				m_ExResult = pEvent->getInfo(0, &sName, &EventInfo);

				if (IS_FMODERROR)
				{
					FmodErrorOutput("event get info failed! ", eSLT_Warning);
				}
				else
				{
					// oneshot sounds will end eventually, 
					//so only check for looping sounds which are not controlled by the sound system
					bool bLost = (EventInfo.lengthms == -1) && !(m_pSoundSystem->IsEventUsedInPlatformSound((tSoundHandle)pEvent));
					
					if (bLost)
					{
						//m_pSoundSystem->Log("[Warning] <Sound> Lost event %s found and stopped. \n", sName );
						//m_ExResult = pEvent->stop(false);

						bool bPlaying = false;

						FMOD_EVENT_STATE EventState;
						m_ExResult = pEvent->getState(&EventState);
						if (EventState & FMOD_EVENT_STATE_PLAYING)
							bPlaying = true;
						(void)bPlaying;

						int catchme = 1;
						(void)catchme;
					}
				}

			}
		}

	}

	int CAudioDeviceFmodEx400::GetNumOpenFileHandle()
	{
		return s_nFileOpenCount;
	}


#endif
