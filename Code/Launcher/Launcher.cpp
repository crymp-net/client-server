/**
 * @file
 * @brief Implementation of CryMP Client launcher.
 */

#include "CryCommon/IGameFramework.h"
#include "CryCommon/IConsole.h"
#include "CrySystem/GameWindow.h"
#include "CryGame/Game.h"

#include "Launcher.h"
#include "Patch.h"
#include "CPU.h"
#include "Util.h"

#include "config.h"

#ifdef BUILD_64BIT
#define BIN_DIRECTORY "Bin64"
#else
#define BIN_DIRECTORY "Bin32"
#endif

Launcher *Launcher::s_pInstance;

static bool CheckGameVersion(void *pDLL)
{
	int gameVersion = Util::GetCrysisGameVersion(pDLL);
	if (gameVersion < 0)
	{
		Util::ErrorBox("Failed to obtain game version!");
		return false;
	}

	if (gameVersion != 6156)
	{
		Util::ErrorBox("Unsupported version of the game!");
		return false;
	}

	return true;
}

static bool InstallMemoryPatches(void *pCryAction, void *pCryNetwork, void *pCrySystem)
{
	// CryAction

	if (!Patch::AllowDX9ImmersiveMultiplayer(pCryAction))
		return false;

	// CryNetwork

	if (!Patch::AllowSameCDKeys(pCryNetwork))  // useful for non-dedicated servers
		return false;

	// CrySystem

	if (!Patch::RemoveSecuROM(pCrySystem))
		return false;

	if (!Patch::AllowDX9VeryHighSpec(pCrySystem))
		return false;

	if (!Patch::AllowMultipleInstances(pCrySystem))
		return false;

	if (!Patch::UnhandledExceptions(pCrySystem))
		return false;

	if (CPU::IsAMD() && !CPU::Has3DNow())
	{
		if (!Patch::Disable3DNow(pCrySystem))
			return false;
	}

	return true;
}

static bool InitWorkingDirectory()
{
	// get absolute path to the directory with our executable
	Path path = Util::GetApplicationDirectory();
	if (path.isEmpty())
	{
		Util::ErrorBox("Failed to get application directory!");
		return false;
	}

	if ((path.getSize() == 0 || path.getLast() != BIN_DIRECTORY) && Util::DirectoryExists(path + "Game"))
	{
		// we are in Crysis root directory
		if (!Util::AddDLLDirectory(path + BIN_DIRECTORY))
		{
			Util::ErrorBox("Failed to add " BIN_DIRECTORY " directory to DLL search path!");
			return false;
		}
	}
	else
	{
		// we are in Bin32 or Bin64 directory
		path.pop();  // remove Bin32 or Bin64 from the path

		// set Crysis root directory as working directory
		if (!Util::SetWorkingDirectory(path))
		{
			Util::ErrorBox("Failed to set working directory!");
			return false;
		}
	}

	return true;
}

bool Launcher::run(SSystemInitParams & params)
{
	m_pInitParams = &params;

	if (!InitWorkingDirectory())
	{
		return false;
	}

	if (!m_libCrySystem.load("CrySystem.dll"))
	{
		Util::ErrorBox("Failed to load the CrySystem DLL!");
		return false;
	}

	if (!m_libCryAction.load("CryAction.dll"))
	{
		Util::ErrorBox("Failed to load the CryAction DLL!");
		return false;
	}

	if (!m_libCryNetwork.load("CryNetwork.dll"))
	{
		Util::ErrorBox("Failed to load the CryNetwork DLL!");
		return false;
	}

	if (!CheckGameVersion(m_libCrySystem.getHandle()))
	{
		return false;
	}

	if (Util::GetApplicationFileName().find(CRYMP_CLIENT_EXE_NAME) != 0)
	{
		Util::ErrorBox("Invalid name of the executable!");
		return false;
	}

	if (Util::FindStringNoCase(params.szSystemCmdLine, "-mod"))
	{
		Util::ErrorBox("Mods are not supported!");
		return false;
	}

	if (params.bDedicatedServer || Util::FindStringNoCase(params.szSystemCmdLine, "-dedicated"))
	{
		Util::ErrorBox("Running as dedicated server is not supported!");
		return false;
	}

	if (!InstallMemoryPatches(m_libCryAction.getHandle(), m_libCryNetwork.getHandle(), m_libCrySystem.getHandle()))
	{
		Util::ErrorBox("Failed to apply memory patch!");
		return false;
	}

	IGameFramework::TEntryFunction pEntry = m_libCryAction.getSymbol<IGameFramework::TEntryFunction>("CreateGameFramework");
	if (!pEntry)
	{
		Util::ErrorBox("The CryAction DLL is not valid!");
		return false;
	}

	m_pGameFramework = pEntry();
	if (!m_pGameFramework)
	{
		Util::ErrorBox("Failed to create the GameFramework Interface!");
		return false;
	}

	GameWindow window;
	if (!window.init())
	{
		return false;
	}

	params.pUserCallback = this;

	// initialize CryEngine
	if (!m_pGameFramework->Init(params))
	{
		Util::ErrorBox("CryENGINE initialization failed!");
		return false;
	}

	params.pSystem = m_pSystem;  // Launcher::OnInit

	CGame game;
	if (!game.Init(m_pGameFramework))
	{
		Util::ErrorBox("Game initialization failed!");
		return false;
	}

	if (!m_pGameFramework->CompleteInit())
	{
		Util::ErrorBox("CryENGINE post-initialization failed!");
		return false;
	}

	m_pSystem->ExecuteCommandLine();
	m_pSystem->GetIConsole()->ExecuteString("exec autoexec.cfg");

	// game update loop
	while (true)
	{
		if (!window.update())
		{
			break;
		}

		if (!game.Update(true, 0))
		{
			window.onQuit();
			break;
		}
	}

	return true;
}

bool Launcher::OnError(const char *msg)
{
	// quit
	return true;
}

void Launcher::OnSaveDocument()
{
}

void Launcher::OnProcessSwitch()
{
}

void Launcher::OnInitProgress(const char *msg)
{
}

void Launcher::OnInit(ISystem *pSystem)
{
	m_pSystem = pSystem;

	// CryEngine global environment
	ModuleInitISystem(pSystem);
}

void Launcher::OnShutdown()
{
}

void Launcher::OnUpdate()
{
}

void Launcher::GetMemoryUsage(ICrySizer *pSizer)
{
}
