////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   CommandPlayerFmodEx400.h
//  Version:     v1.00
//  Created:     8/11/2006 by Tomas
//  Compilers:   Visual Studio.NET
//  Description: FmodEx 4.00 Implementation of a Command Player
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __COMMANDPLAYERFMODEX400_H__
#define __COMMANDPLAYERFMODEX400_H__

#include "FmodEx/inc/fmod.hpp"
#include "FmodEx/inc/fmod_event.hpp"

class CCommandPlayerFmodEx400
{
	
public:	
	
	CCommandPlayerFmodEx400();
	~CCommandPlayerFmodEx400();

	enum eCOMMANDS
	{ 
		NONE,
		SYSTEM_,
		SYSTEM_SETOUTPUT,
		SYSTEM_SETDRIVER,
		SYSTEM_SETRECORDDRIVER,
		SYSTEM_SETHARDWARECHANNELS,
		SYSTEM_SETSPEAKERMODE,
		SYSTEM_SET3DSETTINGS,
		EVENTSYSTEM_CREATE,
		EVENTSYSTEM_INIT,
		EVENTSYSTEM_UPDATE,
		EVENTSYSTEM_SET3DNUMLISTENERS,
		EVENTSYSTEM_SET3DLISTENERSATTRIBUTES,
		EVENTSYSTEM_LOAD,
		EVENTSYSTEM_UNLOAD,
		EVENTSYSTEM_RELEASE,
		EVENTSYSTEM_GETSYSTEMOBJECT,
		EVENTSYSTEM_SETMEDIAPATH,
		PROJECT_,
		PROJECT_GETGROUP,
		GROUP_,
		GROUP_GETEVENT,
		GROUP_FREEDATA,
		GROUP_FREEEVENTDATA,
		EVENT_,
		EVENT_PLAY,
		EVENT_STOP,
		EVENT_SETPAUSED,
		EVENT_SETVOLUME,
		EVENT_SET3DATTRIBUTES,
		EVENT_GETPARAMETER,
		PARAMETER_,
		PARAMETER_SETVALUE,
		CATEGORY_

	}; 


void RecordFile(const char* sFilename);
bool IsRecording() { return m_bRecording; }
void StopRecording() { m_bRecording = false; }

void LogCommand(const eCOMMANDS command, const int nMilliSecs, void* pParam1=0, void* pParam2=0, void* pParam3=0, void* pParam4=0, void* pParam5=0);
//void PlayCommand(const eCOMMANDS command, const int nMilliSecs, void* pParam1=0, void* pParam2=0, void* pParam3=0, void* pParam4=0);

private:
	FMOD::System*				m_pFMODEX;
	FMOD::EventSystem*	m_pEventSystem;
	FMOD_RESULT					m_ExResult;

		
	FILE	*m_pFile;
	bool m_bRecording;

	#define	COMMAND_SIZE	256
	static char	m_sCommand[COMMAND_SIZE];

int					 m_nfModDriverType;
int					 m_nCurrentMemAlloc;
int					 m_nMaxMemAlloc;
unsigned int m_nMemoryStatInc;
FMOD_CAPS		m_nFMODDriverCaps;
//int					m_nEaxStatusFMOD;

// init vars
FMOD_SPEAKERMODE m_nSpeakerMode;


//float m_fpctCpuUsed;

//void FmodErrorOutput(const char * sDescription, IMiniLog::ELogType LogType = IMiniLog::eMessage);

};

#endif

