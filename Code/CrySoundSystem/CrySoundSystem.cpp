// CrySoundSystem.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include "platform_impl.h"

#include <IEngineModule.h>
#include <CryExtension/ICryFactory.h>
#include <CryExtension/Impl/ClassWeaver.h>

#include "Sound.h"
#include "SoundSystem.h"
#include "IAudioDevice.h"

#ifdef SOUNDSYSTEM_USE_FMODEX400
	#include "AudioDeviceFmodEx400.h"
#endif






//////////////////////////////////////////////////////////////////////////
struct CSystemEventListner_Sound : public ISystemEventListener
{
public:
	virtual void OnSystemEvent( ESystemEvent event,UINT_PTR wparam,UINT_PTR lparam )
	{
		switch (event)
		{
		case ESYSTEM_EVENT_LEVEL_POST_UNLOAD:
			{
				STLALLOCATOR_CLEANUP;
				break;
			}
		}
	}
};

static CSystemEventListner_Sound g_system_event_listener_sound;

//////////////////////////////////////////////////////////////////////////////////////////////
// dll interface	

  
void AddPhysicalBlock(long size) {
	// Added statistics for XENON
#ifndef _LIB
	_CryMemoryManagerPoolHelper::FakeAllocation( size );
#else
	GetISystem()->GetIMemoryManager()->FakeAllocation( size );
	//_CryMemoryManagerPoolHelper::FakeAllocation( size,eCryModule );
#endif
}


//////////////////////////////////////////////////////////////////////////
class CEngineModule_CrySoundSystem : public IEngineModule
{
	CRYINTERFACE_SIMPLE(IEngineModule)
	CRYGENERATE_SINGLETONCLASS(CEngineModule_CrySoundSystem, "EngineModule_CrySoundSystem", 0xec73cf4362ca4a7f, 0x8b451076dc6fdb8b)

	//////////////////////////////////////////////////////////////////////////
	virtual const char *GetName() { return "CrySoundSystem"; };
	virtual const char *GetCategory() { return "CryEngine"; };

	//////////////////////////////////////////////////////////////////////////
	virtual bool Initialize( SSystemGlobalEnvironment &env,const SSystemInitParams &initParams )
	{
		ISystem* pSystem = env.pSystem;

		void *hWnd = initParams.hWnd;

		CSoundSystem* pSoundSystem = NULL;
		IAudioDevice *pAudioDevice = NULL;
		bool bInitDeviceResult = false;
		bool bSoundSystemResult = false;


#ifdef SOUNDSYSTEM_USE_FMODEX400
		// creating a FMOD AudioDevice for a Win32/64 platform and hook it into the SoundSystem
		pAudioDevice	= new CAudioDeviceFmodEx400(hWnd);
#endif






		if (pAudioDevice)
		{
			// We have created the audio device now lets create sound system
			pSoundSystem	= new CSoundSystem(hWnd, pAudioDevice);

			if (pSoundSystem)
			{
				// time to init device if everything ok with soundsystem
				if (pSoundSystem->IsOK())
				{
					CryLogAlways("<Sound> Initializing AudioDevice now!\n");
					bInitDeviceResult = pAudioDevice->InitDevice(pSoundSystem);
				}

				if (bInitDeviceResult)
				{
					CryLogAlways("<Sound> Initializing SoundSystem now!\n");
					bSoundSystemResult = pSoundSystem->Init();
				}
				else
					CryLogAlways("<Sound> Error: Cannot initialize SoundSystem!\n");
			}
			else
				CryLogAlways("<Sound> Error: Cannot create SoundSystem!\n");
		}

		if (!pSoundSystem || !bSoundSystemResult || !pAudioDevice || !bInitDeviceResult)
		{
			//if the sound system cannot be created or initialized,
			// or the console variable s_DummySound = 1 then
			//create the dummy sound system (NULL sound system, same as for
			//dedicated server)

			// Cleanup what was created
			if (pSoundSystem)
			{
				pSoundSystem->Release();
				pSoundSystem = NULL;
				pAudioDevice = NULL;
			}

			if (pAudioDevice)
			{
				pAudioDevice->ShutDownDevice();
				delete pAudioDevice;
				pAudioDevice = NULL;
			}
		}

		pSystem->GetISystemEventDispatcher()->RegisterListener(&g_system_event_listener_sound);

		env.pSoundSystem = pSoundSystem;
	
		return true;
	}
};

CRYREGISTER_CLASS(CEngineModule_CrySoundSystem)

CEngineModule_CrySoundSystem::CEngineModule_CrySoundSystem()
{
};

CEngineModule_CrySoundSystem::~CEngineModule_CrySoundSystem()
{
};

#include <CrtDebugStats.h>
