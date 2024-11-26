////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   CommandPlayerFmodEx400.cpp
//  Version:     v1.00
//  Created:     8/11/2006 by Tomas
//  Compilers:   Visual Studio.NET
//  Description: FmodEx 4.00 Implementation of a CommandPlayer
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"

#ifdef SOUNDSYSTEM_USE_FMODEX400

#include "CommandPlayerFmodEx400.h"

#include "FmodEx/inc/fmod.hpp"
#include "FmodEx/inc/fmod_errors.h"
#include "FmodEx/inc/fmod_dsp.h"
#include "FmodEx/inc/fmod_event_net.h"

char	CCommandPlayerFmodEx400::m_sCommand[COMMAND_SIZE];

//#include <windows.h> // used for get currentthread streaming
//////////////////////////////////////////////////////////////////////////
#define IS_FMODERROR (m_ExResult != FMOD_OK )

// debugging memory crash

//CAudioDeviceFmodEx400::tMemPair	CAudioDeviceFmodEx400::m_Malloc[1000];
//int		CAudioDeviceFmodEx400::m_MallocIndex = 0;
//
//CAudioDeviceFmodEx400::tMemPair CAudioDeviceFmodEx400::m_Free[1000];
//int		CAudioDeviceFmodEx400::m_FreeIndex = 0;
//
//void CAudioDeviceFmodEx400::TraceMemory(void *ptr, int nSize, bool bAlloc)
//{
//	if (bAlloc)
//	{
//		CAudioDeviceFmodEx400::m_Malloc[m_MallocIndex].first = ptr;
//		CAudioDeviceFmodEx400::m_Malloc[m_MallocIndex].second = nSize;
//		++m_MallocIndex;
//
//	}
//	else
//	{
//		int i = 0;
//		bool bFound = false;
//		while (CAudioDeviceFmodEx400::m_Malloc[i].first != NULL)
//		{
//			if (CAudioDeviceFmodEx400::m_Malloc[i].first == ptr)
//			{
//				CAudioDeviceFmodEx400::m_Malloc[i].second += 10000;
//				bFound = true;
//				break;
//			}
//			++i;
//		}
//
//		if (!bFound)
//			int a =1; // never happen!
//
//		CAudioDeviceFmodEx400::m_Free[m_FreeIndex].first = ptr;
//		CAudioDeviceFmodEx400::m_Free[m_FreeIndex].second = nSize;
//		++m_FreeIndex;
//
//	}
//}

/*
extern "C"
{
	//////////////////////////////////////////////////////////////////////////
	static void * F_CALLBACK CrySound_Alloc (unsigned int size)
	{
		void* ptr = malloc(size);

		//	 gEnv->pSoundSystem->TraceMemoryUsage(size);

		//CAudioDeviceFmodEx400::TraceMemory(ptr, size, true);

		return ptr;

	}

	//////////////////////////////////////////////////////////////////////////
	static void F_CALLBACK CrySound_Free (void* ptr)
	{
		//unsigned int size = _msize( ptr );
		
		//	gEnv->pSoundSystem->TraceMemoryUsage(0 - size);
	
		//CAudioDeviceFmodEx400::TraceMemory(ptr, size, false);

		free (ptr);
	}

	//////////////////////////////////////////////////////////////////////////
	static void * F_CALLBACK CrySound_Realloc (void *ptr, unsigned int size)
	{
		//unsigned int oldsize = _msize( ptr );
		//	gEnv->pSoundSystem->TraceMemoryUsage(size - oldsize);

		return realloc (ptr, size);
		//return CryRealloc(ptr, size);
	}
}

//////////////////////////////////////////////////////////////////////////
// File callbacks for FMODEX.
//////////////////////////////////////////////////////////////////////////
	//typedef FMOD_RESULT (F_CALLBACK *FMOD_FILE_OPENCALLBACK)     (const char *name, int unicode, unsigned int *filesize, void **handle, void **userdata);
	//typedef FMOD_RESULT (F_CALLBACK *FMOD_FILE_CLOSECALLBACK)    (void *handle, void *userdata);
	//typedef FMOD_RESULT (F_CALLBACK *FMOD_FILE_READCALLBACK)     (void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata);
	//typedef FMOD_RESULT (F_CALLBACK *FMOD_FILE_SEEKCALLBACK)     (void *handle, unsigned int pos, void *userdata);

	static FMOD_RESULT F_CALLBACK CrySoundEx_fopen(const char *name, int unicode, unsigned int *filesize, void **handle, void **userdata)
	{
		assert(name);
		assert(filesize);
		assert(handle);
				
		//DWORD threadid = GetCurrentThreadId();

#ifdef WIN32
		static bool bThreadNamed = false;

		DWORD threadid = GetCurrentThreadId();

		if (!bThreadNamed)
		{
			if (GetCurrentThreadId() != nMainThreadId)
			{
				CryThreadSetName( -1,"FMOD-Streaming" );
				bThreadNamed = true;
			}
			bThreadNamed = false;
		}
#endif

		//FILE *file = gEnv->pCryPak->FOpen( name,"rb", );
		// rbx open flags, x is a hint to not cache whole file in memory.
		FILE *file = gEnv->pCryPak->FOpen( name,"rbx",ICryPak::FOPEN_HINT_DIRECT_OPERATION );
		if (!file)
		{			
			GetISystem()->Warning( VALIDATOR_MODULE_SOUNDSYSTEM,VALIDATOR_WARNING,VALIDATOR_FLAG_SOUND, name, "File %s failed to load", name );
			return FMOD_ERR_FILE_NOTFOUND;	
		}
		// File is opened for streaming.
		//uint32 nSize = gEnv->pCryPak->GetFileSize(file);
		uint32 nSize = gEnv->pCryPak->FGetSize(file);
		*handle		= file;
		*filesize = nSize;

		CSoundSystem* pSoundSystem = (CSoundSystem*) gEnv->pSoundSystem;
		//unsigned int nTemp = nSize/1024;
		string sTemp = "FMOD opens: ";
		//string sSize = sTemp + nTemp;
		sTemp += name;
		//sTemp += " (" + sSize;
		//sTemp += "KB :";
		pSoundSystem->m_pDebugLogger->LogString(sTemp);
		pSoundSystem->m_FMODFilesMap[file] = name;

		return FMOD_OK;

	}

	static FMOD_RESULT F_CALLBACK CrySoundEx_fclose(void *nFile, void *userdata)
	{
		assert(nFile);
		if (nFile)
		{
			FILE *file = (FILE*)nFile;

			string sTemp = "FMOD closes: ";
			gEnv->pSoundSystem->m_pDebugLogger->LogString(sTemp + pSoundSystem->m_FMODFilesMap[file]);
			gEnv->pSoundSystem->m_FMODFilesMap.erase(file);

			if (!gEnv->pCryPak->FClose( file ))
				return FMOD_OK;
		}

		return FMOD_ERR_FILE_NOTFOUND;
	}

	static FMOD_RESULT F_CALLBACK CrySoundEx_fread(void *nFile, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata)
	{
		assert(nFile);
		assert(buffer);
		assert(sizebytes);
		assert(bytesread);
		FILE *file = (FILE*)nFile;

		uint32 nBytesRead = gEnv->pCryPak->FReadRaw( buffer,1,sizebytes,file );
		*bytesread = nBytesRead;
		
		if (nBytesRead != sizebytes)
			return FMOD_ERR_FILE_EOF;
			
//	string sTemp = "FMOD reads: ";
	//gEnv->pSoundSystem->m_DebugLogger.LogString(sTemp + file->_tmpfname);

		return FMOD_OK;
	}

	static FMOD_RESULT F_CALLBACK CrySoundEx_fseek(void *nFile, unsigned int pos, void *userdata)
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

//static int F_CALLBACKAPI CrySound_ftell( void * nFile )
//  {
//  FILE *file = (FILE*)nFile;
//  return gEnv->pCryPak->FTell( file );
//  }

*/

//void CCommandPlayerFmodEx400::FmodErrorOutput(const char * sDescription, IMiniLog::ELogType LogType)
//	{
//		switch(LogType) 
//		{
//		case IMiniLog::eWarning: 
//			gEnv->pLog->LogWarning("[Sound]-Warning: Sound-FmodEx-AudioDevice: %s (%d) %s\n", sDescription, m_ExResult, FMOD_ErrorString(m_ExResult));
//			break;
//		case IMiniLog::eError: 
//			gEnv->pLog->LogError("[Sound]-Error: Sound-FmodEx-AudioDevice: %s (%d) %s\n", sDescription, m_ExResult, FMOD_ErrorString(m_ExResult));
//			break;
//		case IMiniLog::eMessage: 
//			gEnv->pLog->Log("[Sound]: Sound-FmodEx-AudioDevice: %s (%d) %s\n", sDescription, m_ExResult, FMOD_ErrorString(m_ExResult));
//			break;
//		default:
//			break;
//		}
//	}

CCommandPlayerFmodEx400::CCommandPlayerFmodEx400()
{
	m_pFile = NULL;

	m_ExResult				= FMOD_OK;
	m_nSpeakerMode		= FMOD_SPEAKERMODE_RAW;
	m_pEventSystem		= NULL;
	m_pFMODEX					= NULL;
	m_bRecording			= false;

	//m_ExResult = FMOD::Memory_Initialize(0, 0, CrySound_Alloc, CrySound_Realloc, CrySound_Free);
	//m_ExResult = FMOD::EventSystem_Create(&m_pEventSystem);
	//m_ExResult = m_pEventSystem->getSystemObject(&m_pFMODEX);

}

CCommandPlayerFmodEx400::~CCommandPlayerFmodEx400()
{
	if (m_bRecording)
	{
		sprintf(m_sCommand,"</CommandPlayer>\n");
		fputs(m_sCommand, m_pFile);
		fflush(m_pFile);
	}

	if(m_pFile)
		fclose(m_pFile);
}


void CCommandPlayerFmodEx400::RecordFile(const char* sFilename)
{
	if (*sFilename!=0)
		m_pFile = fopen(sFilename,"wt");

	if (m_pFile)
	{
		sprintf(m_sCommand,"<CommandPlayer>\n");
		fputs(m_sCommand, m_pFile);
		fflush(m_pFile);
		m_bRecording = true;
	}
}

void CCommandPlayerFmodEx400::LogCommand(const eCOMMANDS command, const int nMilliSecs, void* pParam1, void* pParam2, void* pParam3, void* pParam4, void* pParam5)
{
	if (!m_pFile || !m_bRecording)
		return;

	int nTime = nMilliSecs;
	switch (command)
	{
	case SYSTEM_:
		break;
	case SYSTEM_SETOUTPUT:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" Output=\"%d\" />\n", command, nTime, *(uint32*)pParam1);
		break;
	case SYSTEM_SETDRIVER:
		break;
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" Driver=\"%d\" />\n", command, nTime, *(uint32*)pParam1);
	case SYSTEM_SETRECORDDRIVER:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" Record=\"%d\" />\n", command, nTime, *(uint32*)pParam1);
		break;
	case SYSTEM_SETHARDWARECHANNELS:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" Min2D=\"%d\" Max2D=\"%d\" Min3D=\"%d\" Max3D=\"%d\" />\n", command, nTime, *(uint32*)pParam1, *(uint32*)pParam2, *(uint32*)pParam3, *(uint32*)pParam4);
		break;
	case SYSTEM_SETSPEAKERMODE:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" Speaker=\"%d\" />\n", command, nTime, *(uint32*)pParam1);
		break;
	case SYSTEM_SET3DSETTINGS:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" Doppler=\"%f\" Distance=\"%f\" Rolloff=\"%f\" />\n", command, nTime, *(float*)pParam1, *(float*)pParam2, *(float*)pParam3);
		break;
	case EVENTSYSTEM_CREATE: 
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" />\n", command, nTime);
		break;
	case EVENTSYSTEM_INIT: 
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" Channels=\"%d\" Flags=\"%d\" Extra=\"%d\" EventFlags=\"%d\" />\n", command, nTime, *(uint32*)pParam1, *(uint32*)pParam2, *(uint32*)pParam3, *(uint32*)pParam4);
		break;	
	case EVENTSYSTEM_UPDATE:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" />\n", command, nTime);
		break;
	case EVENTSYSTEM_SET3DNUMLISTENERS: 
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" NumList=\"%d\" />\n", command, nTime, *(uint32*)pParam1);
		break;	
	case EVENTSYSTEM_SET3DLISTENERSATTRIBUTES: 
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" ListID=\"%d\" PosX=\"%f\" PosY=\"%f\" PosZ=\"%f\" VelX=\"%f\" VelY=\"%f\" VelZ=\"%f\" ForwardX=\"%f\" ForwardY=\"%f\" ForwardZ=\"%f\" TopX=\"%f\" TopY=\"%f\" TopZ=\"%f\"  />\n", command, nTime, *(int*)pParam1, 
			(*(FMOD_VECTOR*)pParam2).x, (*(FMOD_VECTOR*)pParam2).y, (*(FMOD_VECTOR*)pParam2).z, 
			(*(FMOD_VECTOR*)pParam3).x, (*(FMOD_VECTOR*)pParam3).y, (*(FMOD_VECTOR*)pParam3).z,
			(*(FMOD_VECTOR*)pParam4).x, (*(FMOD_VECTOR*)pParam4).y, (*(FMOD_VECTOR*)pParam4).z,
			(*(FMOD_VECTOR*)pParam5).x, (*(FMOD_VECTOR*)pParam5).y, (*(FMOD_VECTOR*)pParam5).z);
		break;	
	case EVENTSYSTEM_LOAD:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" sProject=\"%s\" iProject=\"%d\" />\n", command, nTime, (char*)pParam1, *(uint32*)pParam2);
		break;
	case EVENTSYSTEM_UNLOAD:
		break;
	case EVENTSYSTEM_RELEASE:
		break;
	case EVENTSYSTEM_GETSYSTEMOBJECT:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" />\n", command, nTime);
		break;
	case EVENTSYSTEM_SETMEDIAPATH:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" sMediaPath=\"%s\" />\n", command, nTime, (char*)pParam1);
		break;	
	case PROJECT_GETGROUP:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" iProject=\"%d\" sGroup=\"%s\" Cache=\"%d\" iGroup=\"%d\" />\n", command, nTime, *(uint32*)pParam1, (char*)pParam2, *(uint32*)pParam3, *(uint32*)pParam4);
		break;
	case GROUP_GETEVENT:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" iGroup=\"%d\" sEvent=\"%s\" Mode=\"%d\" iEvent=\"%d\" />\n", command, nTime, *(uint32*)pParam1, (char*)pParam2, *(uint32*)pParam3, *(uint32*)pParam4);
		break;
	case GROUP_FREEDATA:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" iGroup=\"%d\" />\n", command, nTime, *(uint32*)pParam1);
		break;
	case GROUP_FREEEVENTDATA:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" iGroup=\"%d\" Idx=\"%d\" />\n", command, nTime, *(uint32*)pParam1, *(uint32*)pParam2);
		break;
	case EVENT_PLAY:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" iEvent=\"%d\" />\n", command, nTime, *(uint32*)pParam1);
		break;
	case EVENT_STOP:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" iEvent=\"%d\" />\n", command, nTime, *(uint32*)pParam1);
		break;
	case EVENT_SETPAUSED:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" iEvent=\"%d\" Paused=\"%d\" />\n", command, nTime, *(uint32*)pParam1, *(uint32*)pParam2);
		break;
	case EVENT_SETVOLUME:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" iEvent=\"%d\" Val=\"%f\" />\n", command, nTime, *(uint32*)pParam1, *(float*)pParam2);
		break;
	case EVENT_SET3DATTRIBUTES:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" iEvent=\"%d\" PosX=\"%f\" PosY=\"%f\" PosZ=\"%f\" VelX=\"%f\" VelY=\"%f\" VelZ=\"%f\" />\n", command, nTime, *(uint32*)pParam1, (*(FMOD_VECTOR*)pParam2).x, (*(FMOD_VECTOR*)pParam2).y, (*(FMOD_VECTOR*)pParam2).z, (*(FMOD_VECTOR*)pParam3).x, (*(FMOD_VECTOR*)pParam3).y, (*(FMOD_VECTOR*)pParam3).z);
		break;
	case EVENT_GETPARAMETER:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" iEvent=\"%d\" sParam=\"%s\" iParam=\"%d\" />\n", command, nTime, *(uint32*)pParam1, (char*)pParam2, *(uint32*)pParam3);
		break;
	case PARAMETER_SETVALUE:
		sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" iParam=\"%d\" Val=\"%f\" />\n", command, nTime, *(uint32*)pParam1, *(float*)pParam2);
		break;
	default:
		{
			int a = 0;
			(void)a;
			break;
		}
	}

	fputs(m_sCommand, m_pFile);
	fflush(m_pFile);

}
/*
void CCommandPlayerFmodEx400::PlayCommand(const eCOMMANDS command, const int nMilliSecs, void* pParam1, void* pParam2, void* pParam3, void* pParam4)
{
	if (!m_pFile || !m_bRecording)
		return;

	int nTime = nMilliSecs;
	switch (command)
	{
	case SYSTEM_:
		break;
	case SYSTEM_SETOUTPUT:
		//printf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" Output=\"%d\" />\n", command, nTime, pParam1);
		break;
	case SYSTEM_SETDRIVER:
		break;
		//sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" Driver=\"%d\" />\n", command, nTime, pParam1);
	case SYSTEM_SETRECORDDRIVER:
		//sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" Record=\"%d\" />\n", command, nTime, pParam1);
		break;
	case SYSTEM_SETHARDWARECHANNELS:
		//sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" Min2D=\"%d\" Max2D=\"%d\" Min3D=\"%d\" Max3D=\"%d\" />\n", command, nTime, pParam1, pParam2, pParam3, pParam4);
		break;
	case SYSTEM_SETSPEAKERMODE:
		//sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" Speaker=\"%d\" />\n", command, nTime, pParam1);
		break;
	case SYSTEM_SET3DSETTINGS:
		//sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" Doppler=\"%f\" Distance=\"%f\" Rolloff=\"%f\" />\n", command, nTime, pParam1, pParam2, pParam3);
		break;
	case EVENTSYSTEM_CREATE: 
		//sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" />\n", command, nTime);
		break;
	case EVENTSYSTEM_INIT: 
		//sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" Channels=\"%d\" Flags=\"%d\" Extra=\"%d\" EventFlags=\"%d\" />\n", command, nTime, pParam1, pParam2, pParam3, pParam4);
		break;	
	case EVENTSYSTEM_UPDATE:
		//sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" />\n", command, nTime);
		//sprintf(m_sCommand,"");
		break;
	case EVENTSYSTEM_LOAD:
		//sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" sProject=\"%s\" />\n", command, nTime, pParam1);
		break;
	case EVENTSYSTEM_UNLOAD:
		break;
	case EVENTSYSTEM_RELEASE:
		break;
	case EVENTSYSTEM_GETSYSTEMOBJECT:
		break;
	case EVENTSYSTEM_SETMEDIAPATH:
		//sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" sMediaPath=\"%s\" />\n", command, nTime, pParam1);
		break;	
	case PROJECT_GETGROUP:
		//sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" pProject=\"%d\" sGroup=\"%s\" Cache=\"%d\" />\n", command, nTime, pParam1, pParam2, pParam3);
		break;
	case GROUP_GETEVENT:
		//sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" pGroup=\"%d\" sEvent=\"%s\" Mode=\"%d\" />\n", command, nTime, pParam1, pParam2, pParam3);
		break;
	case EVENT_PLAY:
		//sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" pEvent=\"%d\" />\n", command, nTime, pParam1);
		break;
	case EVENT_STOP:
		//sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" pEvent=\"%d\" />\n", command, nTime, pParam1);
		break;
	case EVENT_SETPAUSED:
		//sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" pEvent=\"%d\" Paused=\"%d\" />\n", command, nTime, pParam1, pParam2);
		break;
	case EVENT_SETVOLUME:
		//sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" pEvent=\"%d\" Val=\"%f\" />\n", command, nTime, pParam1, *(float*)pParam2);
		break;
	case EVENT_SET3DATTRIBUTES:
		//sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" pEvent=\"%d\" PosX=\"%f\" PosY=\"%f\" PosZ=\"%f\" VelX=\"%f\" VelY=\"%f\" VelZ=\"%f\" />\n", command, nTime, pParam1, (*(FMOD_VECTOR*)pParam2).x, (*(FMOD_VECTOR*)pParam2).y, (*(FMOD_VECTOR*)pParam2).z, (*(FMOD_VECTOR*)pParam3).x, (*(FMOD_VECTOR*)pParam3).y, (*(FMOD_VECTOR*)pParam3).z);
		break;
	case EVENT_GETPARAMETER:
		//sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" pEvent=\"%d\" sParam=\"%s\" />\n", command, nTime, pParam1, pParam2);
		break;
	case PARAMETER_SETVALUE:
		//sprintf(m_sCommand,"<Command ID=\"%d\" T=\"%d\" pParam=\"%d\" Val=\"%f\" />\n", command, nTime, pParam1, *(float*)pParam2);
		break;
	default:
		{
			int a = 0;
			break;
		}
	}

	//fputs(m_sCommand, m_pFile);
	//fflush(m_pFile);

}
*/

/*
bool CCommandPlayerFmodEx400::InitDevice(CSoundSystem* pSoundSystem, int softwareChan) 
{
	if (!m_pEventSystem)
	{
		m_ExResult = FMOD::EventSystem_Create(&m_pEventSystem);
		if (IS_FMODERROR)
		{
			FmodErrorOutput("event system failed! ", IMiniLog::eError);
			return false;
		}
	}

	m_ExResult = m_pEventSystem->getSystemObject(&m_pFMODEX);

	// output types
	FMOD_OUTPUTTYPE nOutput = FMOD_OUTPUTTYPE_AUTODETECT;
  m_ExResult = m_pFMODEX->setOutput(nOutput);

	int nNumDrivers = 0;
	m_ExResult = m_pFMODEX->getNumDrivers(&nNumDrivers);

	// prints out valid drivers
	for (int i=0; m_ExResult==FMOD_OK; ++i)
	{
		char sDriverName[256];
		m_ExResult = m_pFMODEX->getDriverName(i, sDriverName, 256);
	}

	// setting driver
	m_ExResult = m_pFMODEX->setDriver(0);

	// querying which driver
	int nDriver = 0;
	m_ExResult = m_pFMODEX->getDriver(&nDriver);

	// querying which driver
	char sDriverName[256];
	m_ExResult = m_pFMODEX->getDriverName(nDriver, sDriverName, 256);

	//m_ExResult = m_pFMODEX->setHardwareChannels(m_nMinHardwareChannels, m_nMaxHardwareChannels, m_nMinHardwareChannels, m_nMaxHardwareChannels);
	m_ExResult = m_pFMODEX->setHardwareChannels(0, 0, 0, 0);

	m_ExResult = m_pFMODEX->setSpeakerMode(m_nSpeakerMode);

	m_ExResult = m_pEventSystem->init(m_nSoftwareChannels, FMOD_INIT_NORMAL, m_pSoundSystem->m_pCVarEnableNetworkAudition->GetIVal()?NULL:m_pHWnd, EVENT_INIT_NORMAL);

	// Record driver query
	for (int i=0; m_ExResult==FMOD_OK; ++i)
	{
		char sRecordDriverName[256];
		m_ExResult = m_pFMODEX->getRecordDriverName(i, sRecordDriverName, 256);

		if (m_ExResult == FMOD_OK)
			CryLogAlways("<Sound> Available record drivers: %d %s !\n", i, sRecordDriverName);
	}

	// set Record Driver for Microphone first
	m_ExResult = m_pFMODEX->setRecordDriver(0);
	
	uint32 nVersion;
	m_ExResult = m_pFMODEX->getVersion(&nVersion);

	char szPath[512];
	sprintf(szPath, "..\\%s\\Sounds", PathUtil::GetGameFolder().c_str());
	m_ExResult = m_pEventSystem->setMediaPath(szPath);

	const float DISTANCEFACTOR = 1.0f;   // Units per meter.  I.e feet would = 3.28.  centimeters would = 100.

	// workaround for FMOD bug: distance attenuation was 1.0f, lowered to fix
	m_ExResult = m_pFMODEX->set3DSettings(1.0, DISTANCEFACTOR, 0.01f);

	m_ExResult = m_pFMODEX->getHardwareChannels(&m_nHardware2DChannels, &m_nHardware3DChannels, &m_nTotalHardwareChannelsAvail);

	return bTemp;
}


	//m_ExResult = m_pFMODEX->getOutputHandle(pHandle);
	//m_ExResult = m_pFMODEX->getOutput(&OutputType);
	//	m_ExResult = m_pEventSystem->unload();
	//	m_ExResult = m_pEventSystem->release();
	//m_ExResult = m_pEventSystem->unload();
	//m_ExResult = m_pEventSystem->set3DNumListeners(2);
	//m_ExResult = m_pEventSystem->set3DListenerAttributes(nListenerID, &vExPos, &vExVel, &vExForward, &vExTop);
		//FMOD::System_SetDebugMode(nMode);
		//FMOD::System_SetDebugLevel(nLevel);
	
		//m_ExResult = m_pEventSystem->update();
		//m_ExResult = m_pEventSystem->getInfo(&Info);
		//m_ExResult = FMOD::Memory_GetStats(&m_nCurrentMemAlloc, &m_nMaxMemAlloc);

		//m_ExResult = m_pFMODEX->getCPUUsage(0,0,0,&fTemp);
		//m_ExResult = m_pFMODEX->getDriverCaps(0, &m_nFMODDriverCaps, 0, 0, &m_nSpeakerMode);
		//FMOD::Memory_GetStats(&nCurrent, &nMax);
		//m_pFMODEX->getRecordDriverName(nRecordDevice, sName, nNameLength);
		//m_ExResult = m_pFMODEX->getMasterChannelGroup(&MasterChannelGroup);
		//m_ExResult = m_pFMODEX->getChannelsPlaying(&nTemp);
		//m_ExResult = m_pEventSystem->setMediaPath((char*)sProjectPath.c_str());
		//m_ExResult = m_pEventSystem->load((char*)sProjectName.c_str(), 0, &NewProject.ProjectHandle);
		//m_ExResult = m_pEventSystem->getProjectByIndex(nProjectIndex, &pProject);

		//m_ExResult = pProject->getGroupByIndex(nGroupIndex, false, &pGroup);
		//m_ExResult = pParentGroup->getNumEvents( &nNumEvents);
*/
#endif
