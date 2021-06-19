#include "Client/Client.h"
#include "CryCommon/CryAction/IGameFramework.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "CryGame/Game.h"
#include "Library/CmdLine.h"
#include "Library/CPU.h"
#include "Library/Error.h"
#include "Library/Format.h"
#include "Library/WinAPI.h"

#include "Launcher.h"
#include "Patch.h"
#include "CrashLogger.h"
#include "CryMemoryManager.h"

#include "config.h"

void Launcher::initWorkingDirectory()
{
	const std::string dirArg = CmdLine::GetArgValue("-dir");

	std::filesystem::path dir;

	if (!dirArg.empty())
	{
		// sanitize the path
		dir = std::filesystem::canonical(dirArg);
	}
	else
	{
		dir = std::filesystem::current_path();
	}

#ifdef BUILD_64BIT
	constexpr std::string_view BIN_DIR = "Bin64";
#else
	constexpr std::string_view BIN_DIR = "Bin32";
#endif

	if (dir.filename() != BIN_DIR && std::filesystem::is_directory(dir / "Game"))
	{
		// we are in Crysis directory, so add Bin32 or Bin64 to the DLL search path
		WinAPI::DLL_AddSearchDirectory(dir / BIN_DIR);
	}
	else
	{
		// we are in Crysis/Bin32 or Crysis/Bin64 directory, so remove Bin32 or Bin64 from the path
		dir = dir.parent_path();
	}

	WinAPI::SetWorkingDirectory(dir);
}

void Launcher::loadEngine()
{
	if (!m_CrySystem.load("CrySystem.dll", DLL::NO_RELEASE))  // unloading Crysis DLLs is not safe
	{
		throw SystemError("Failed to load the CrySystem DLL!");
	}

	const int gameVersion = WinAPI::GetCrysisGameBuild(m_CrySystem.getHandle());
	if (gameVersion < 0)
	{
		throw SystemError("Failed to get the game version!");
	}

	switch (gameVersion)
	{
		case 5767:
		{
			throw Error("Crysis 1.0 is not supported!\n\nInstall 1.2 and 1.2.1 official patch.");
		}
		case 5879:
		{
			throw Error("Crysis 1.1 is not supported!\n\nInstall 1.2 and 1.2.1 official patch.");
		}
		case 6115:
		{
			throw Error("Crysis 1.2 is not supported!\n\nInstall 1.2.1 official patch.");
		}
		case 6156:
		{
			// only the latest Crysis 1.2.1 is supported
			break;
		}
		case 6527:
		case 6566:
		case 6586:
		case 6627:
		case 6670:
		case 6729:
		{
			throw Error("Crysis Wars is not supported!");
		}
		case 687:
		case 710:
		case 711:
		{
			throw Error("Crysis Warhead is not supported!");
		}
		default:
		{
			throw Error(Format("Unknown game version %d!", gameVersion));
		}
	}

	if (!m_CryAction.load("CryAction.dll", DLL::NO_RELEASE))
	{
		throw SystemError("Failed to load the CryAction DLL!");
	}

	if (!m_CryNetwork.load("CryNetwork.dll", DLL::NO_RELEASE))
	{
		throw SystemError("Failed to load the CryNetwork DLL!");
	}
}

void Launcher::patchEngine()
{
	if (m_CryAction)
	{
		Patch::AllowDX9ImmersiveMultiplayer(m_CryAction);
	}

	if (m_CryNetwork)
	{
		Patch::FixInternetConnect(m_CryNetwork);
		Patch::EnablePreordered(m_CryNetwork);
		Patch::AllowSameCDKeys(m_CryNetwork);
	}

	if (m_CrySystem)
	{
		Patch::RemoveSecuROM(m_CrySystem);
		Patch::MakeDX9Default(m_CrySystem);
		Patch::AllowDX9VeryHighSpec(m_CrySystem);
		Patch::AllowMultipleInstances(m_CrySystem);
		Patch::UnhandledExceptions(m_CrySystem);

		if (CPU::IsAMD() && !CPU::Has3DNow())
		{
			Patch::Disable3DNow(m_CrySystem);
		}
	}
}

void Launcher::startEngine()
{
	IGameFramework::TEntryFunction entry = m_CryAction.getSymbol<IGameFramework::TEntryFunction>("CreateGameFramework");
	if (!entry)
	{
		throw Error("The CryAction DLL is not valid!");
	}

	IGameFramework *pGameFramework = entry();
	if (!pGameFramework)
	{
		throw Error("Failed to create the GameFramework Interface!");
	}

	m_gameWindow.init();

	m_params.pUserCallback = this;

	// initialize CryEngine
	if (!pGameFramework->Init(m_params))  // Launcher::OnInit is called here
	{
		throw Error("CryENGINE initialization failed!");
	}

	// initialize the multiplayer client
	gClient->init(pGameFramework);

	// initialize the game
	// mods are not supported
	m_pGame = new CGame();
	if (!m_pGame->Init(pGameFramework))
	{
		throw Error("Game initialization failed!");
	}

	if (!pGameFramework->CompleteInit())
	{
		throw Error("CryEngine post-initialization failed!");
	}

	gEnv->pSystem->ExecuteCommandLine();
}

void Launcher::updateLoop()
{
	gEnv->pConsole->ExecuteString("exec autoexec.cfg");

	while (true)
	{
		if (!m_gameWindow.update())
		{
			break;
		}

		if (!m_pGame->Update(true, 0))
		{
			m_gameWindow.onQuit();
			break;
		}
	}
}

Launcher::Launcher()
{
	memset(&m_params, 0, sizeof m_params);
}

Launcher::~Launcher()
{
}

bool Launcher::OnError(const char *error)
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

void Launcher::OnInitProgress(const char *message)
{
}

void Launcher::OnInit(ISystem *pSystem)
{
	gEnv = pSystem->GetGlobalEnvironment();
}

void Launcher::OnShutdown()
{
}

void Launcher::OnUpdate()
{
	m_log.OnUpdate();
}

void Launcher::GetMemoryUsage(ICrySizer *pSizer)
{
}

void Launcher::setCmdLine(const char *cmdLine)
{
	const size_t length = strlen(cmdLine);

	if (length >= sizeof m_params.cmdLine)
	{
		throw Error("Command line is too long!");
	}

	memcpy(m_params.cmdLine, cmdLine, length + 1);
}

void Launcher::run()
{
	if (WinAPI::GetApplicationPath().filename().string().find(CRYMP_CLIENT_EXE_NAME) != 0)
	{
		throw Error("Invalid name of the executable!");
	}

	if (CmdLine::HasArg("-mod"))
	{
		throw Error("Mods are not supported!");
	}

	if (CmdLine::HasArg("-dedicated") || m_params.isDedicatedServer)
	{
		throw Error("Running as a dedicated server is not supported!");
	}

	// the first step
	initWorkingDirectory();

	// open the log file
	m_log.Init(m_params);

	// crash logger requires the log file
	CrashLogger::Init();

	// load and patch Crysis DLLs
	loadEngine();
	patchEngine();

	CryMemoryManager::Init(m_CrySystem);

	// the multiplayer client
	Client client;
	gClient = &client;

	startEngine();

	updateLoop();
}
