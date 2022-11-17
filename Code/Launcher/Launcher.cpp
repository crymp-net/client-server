#include <cstring>

#include "CryCommon/CryAction/IGameFramework.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/CrySystem/ICryPak.h"
#include "CryGame/Game.h"
#include "CryMP/Client/Client.h"
#include "CryScriptSystem/ScriptSystem.h"
#include "CrySystem/CryMemoryManager.h"
#include "CrySystem/GameWindow.h"
#include "CrySystem/LocalizationManager.h"
#include "CrySystem/Logger.h"
#include "Library/CmdLine.h"
#include "Library/Error.h"
#include "Library/Format.h"
#include "Library/WinAPI.h"

#include "Launcher.h"
#include "Patch.h"
#include "CPUInfo.h"
#include "CrashLogger.h"

#include "config.h"

#define BUILD_DATE_TIME __DATE__ " " __TIME__

namespace
{
	IScriptSystem *CreateNewScriptSystem(ISystem *pSystem, bool bStdLibs)
	{
		CryLogAlways("$3[CryMP] Using the new Script System");

		ScriptSystem *pScriptSystem = new ScriptSystem();
		pScriptSystem->Init();

		return pScriptSystem;
	}

	void ReplaceScriptSystem(const DLL & CrySystem)
	{
		using WinAPI::RVA;
		using WinAPI::FillNOP;
		using WinAPI::FillMem;

		void *pCrySystem = CrySystem.GetHandle();
		void *pNewFunc = CreateNewScriptSystem;

#ifdef BUILD_64BIT
		const size_t codeOffset = 0x445A2;
		const size_t codeSize = 0x4E;

		unsigned char code[] = {
			0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // mov rax, 0x0
			0x48, 0x8B, 0xCB                                             // mov rcx, rbx
		};

		std::memcpy(&code[2], &pNewFunc, 8);
#else
		const size_t codeOffset = 0x56409;
		const size_t codeSize = 0x3C;

		unsigned char code[] = {
			0xB8, 0x00, 0x00, 0x00, 0x00  // mov eax, 0x0
		};

		std::memcpy(&code[1], &pNewFunc, 4);
#endif

		FillMem(RVA(pCrySystem, codeOffset), code, sizeof code);
		FillNOP(RVA(pCrySystem, codeOffset + sizeof code), codeSize - sizeof code);
	}

	void ReplaceLocalizationManager(const DLL & CrySystem)
	{
		struct DummyCSystem
		{
			ILocalizationManager* GetLocalizationManager()
			{
				return &LocalizationManager::GetInstance();
			}

			static void InitLocalizationManager()
			{
				CryLogAlways("$3[CryMP] Using the new Localization Manager");
			}
		};

		using TGetLocalizationManager = decltype(&DummyCSystem::GetLocalizationManager);

		using WinAPI::RVA;
		using WinAPI::FillNOP;
		using WinAPI::FillMem;

		void* pCrySystem = CrySystem.GetHandle();
		void* pNewFunc = &DummyCSystem::InitLocalizationManager;

#ifdef BUILD_64BIT
		const std::size_t vtableOffset = 0x26ACF8;

		unsigned char code[] = {
			0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // mov rax, 0x0
			0xFF, 0xD0                                                   // call rax
		};

		std::memcpy(&code[2], &pNewFunc, 8);

		FillNOP(RVA(pCrySystem, 0x453A7), 0x62);
		FillMem(RVA(pCrySystem, 0x453A7), code, sizeof code);
		FillNOP(RVA(pCrySystem, 0x50A5C), 0x28);
#else
		const std::size_t vtableOffset = 0x1BC5F8;

		unsigned char code[] = {
			0xB8, 0x00, 0x00, 0x00, 0x00,  // mov eax, 0x0
			0xFF, 0xD0                     // call eax
		};

		std::memcpy(&code[1], &pNewFunc, 4);

		FillNOP(RVA(pCrySystem, 0x56B1D), 0x29);
		FillMem(RVA(pCrySystem, 0x56B1D), code, sizeof code);
		FillNOP(RVA(pCrySystem, 0x624E1), 0x23);
#endif
		void** vtable = static_cast<void**>(RVA(pCrySystem, vtableOffset));

		// vtable hook
		TGetLocalizationManager newFunc = &DummyCSystem::GetLocalizationManager;
		FillMem(&vtable[105], &reinterpret_cast<void*&>(newFunc), sizeof(void*));
	}
}

void Launcher::SetCmdLine()
{
	const std::string_view cmdLine = WinAPI::GetCmdLine();

	if (cmdLine.length() >= sizeof m_params.cmdLine)
	{
		throw Error("Command line is too long!");
	}

	memcpy(m_params.cmdLine, cmdLine.data(), cmdLine.length());

	m_params.cmdLine[cmdLine.length()] = '\0';
}

void Launcher::InitWorkingDirectory()
{
	std::filesystem::path dir;

	const std::string dirArg = CmdLine::GetArgValue("-dir");
	if (!dirArg.empty())
	{
		dir = dirArg;
	}
	else
	{
		dir = WinAPI::GetApplicationPath().remove_filename();
	}

	// sanitize the path
	dir = std::filesystem::canonical(dir);

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

void Launcher::LoadEngine()
{
	if (!m_CrySystem.Load("CrySystem.dll", DLL::NO_RELEASE))  // unloading Crysis DLLs is not safe
	{
		if (SystemError::GetCurrentCode() == 193)  // ERROR_BAD_EXE_FORMAT
		{
			throw Error("Failed to load the CrySystem DLL!\n\n"
#ifdef BUILD_64BIT
				"It seems you have 32-bit DLLs in your Bin64 directory! Please fix it."
#else
				"It seems you have 64-bit DLLs in your Bin32 directory! Please fix it."
#endif
			);
		}
		else
		{
			throw SystemError("Failed to load the CrySystem DLL!");
		}
	}

	const int gameVersion = WinAPI::GetCrysisGameBuild(m_CrySystem.GetHandle());
	if (gameVersion < 0)
	{
		throw SystemError("Failed to get the game version!");
	}

	CryMemoryManager::Init(m_CrySystem);

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

	if (!m_CryAction.Load("CryAction.dll", DLL::NO_RELEASE))
	{
		throw SystemError("Failed to load the CryAction DLL!");
	}

	if (!m_CryNetwork.Load("CryNetwork.dll", DLL::NO_RELEASE))
	{
		throw SystemError("Failed to load the CryNetwork DLL!");
	}

	const bool isDX10 = !CmdLine::HasArg("-dx9") && (CmdLine::HasArg("-dx10") || WinAPI::IsVistaOrLater());

	if (isDX10)
	{
		if (!m_CryRenderD3D10.Load("CryRenderD3D10.dll", DLL::NO_RELEASE))
		{
			throw SystemError("Failed to load the CryRenderD3D10 DLL!");
		}
	}
}

void Launcher::PatchEngine()
{
	if (m_CryAction)
	{
		Patch::AllowDX9ImmersiveMultiplayer(m_CryAction);
		Patch::DisableBreakLog(m_CryAction);
	}

	if (m_CryNetwork)
	{
		Patch::FixFileCheckCrash(m_CryNetwork);
		Patch::FixInternetConnect(m_CryNetwork);
		Patch::EnablePreordered(m_CryNetwork);
		Patch::AllowSameCDKeys(m_CryNetwork);
	}

	if (m_CrySystem)
	{
		Patch::RemoveSecuROM(m_CrySystem);
		//Patch::MakeDX9Default(m_CrySystem);
		Patch::AllowDX9VeryHighSpec(m_CrySystem);
		Patch::AllowMultipleInstances(m_CrySystem);
		Patch::UnhandledExceptions(m_CrySystem);
		Patch::DisableIOErrorLog(m_CrySystem);
		Patch::HookCPUDetect(m_CrySystem, &CPUInfo::Detect);

		if (!CmdLine::HasArg("-oldss"))
		{
			ReplaceScriptSystem(m_CrySystem);
		}

		ReplaceLocalizationManager(m_CrySystem);
	}

	if (m_CryRenderD3D10)
	{
		Patch::FixLowRefreshRateBug(m_CryRenderD3D10);
	}
}

void Launcher::StartEngine()
{
	IGameFramework::TEntryFunction entry = m_CryAction.GetSymbol<IGameFramework::TEntryFunction>("CreateGameFramework");
	if (!entry)
	{
		throw Error("The CryAction DLL is not valid!");
	}

	IGameFramework *pGameFramework = entry();
	if (!pGameFramework)
	{
		throw Error("Failed to create the GameFramework Interface!");
	}

	GameWindow::GetInstance().Init();

	// initialize CryEngine
	// Launcher::OnInit is called here
	if (!pGameFramework->Init(m_params))
	{
		throw Error("CryENGINE initialization failed!");
	}

	// initialize the multiplayer client
	gClient->Init(pGameFramework);

	// initialize the game
	// mods are not supported
	m_pGame = new CGame();
	if (!m_pGame->Init(pGameFramework))
	{
		throw Error("Game initialization failed!");
	}

	if (!pGameFramework->CompleteInit())
	{
		throw Error("CryENGINE post-initialization failed!");
	}

	gEnv->pSystem->ExecuteCommandLine();
}

void Launcher::UpdateLoop()
{
	gEnv->pConsole->ExecuteString("exec autoexec.cfg");

	while (true)
	{
		if (!GameWindow::GetInstance().OnUpdate())
		{
			break;
		}

		if (!m_pGame->Update(true, 0))
		{
			GameWindow::GetInstance().OnQuit();
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

	// open the log file
	Logger::GetInstance().Init(m_params.logFileName);

	// crash logger requires the log file
	CrashLogger::Init();

	const WinAPI::DateTime dateTime = WinAPI::GetCurrentDateTimeLocal();
	const std::string timeZone = WinAPI::GetTimeZoneOffsetString();

	CryLogAlways("Log begins at %4hu-%02hu-%02hu %02hu:%02hu:%02hu%s",
		dateTime.year,
		dateTime.month,
		dateTime.day,
		dateTime.hour,
		dateTime.minute,
		dateTime.second,
		timeZone.c_str()
	);

	const std::string mainDir = std::filesystem::current_path().string();
	const std::string userDir = std::filesystem::canonical(gEnv->pCryPak->GetAlias("%USER%")).string();
	const std::string rootDir = std::filesystem::canonical(gEnv->pSystem->GetRootFolder()).string();

	CryLogAlways("Executable: %s", WinAPI::GetApplicationPath().string().c_str());
	CryLogAlways("Main directory: %s", mainDir.c_str());
	CryLogAlways("User directory: %s", userDir.c_str());
	CryLogAlways("Root directory: %s", rootDir.c_str());
	CryLogAlways("");

	const SFileVersion & version = gEnv->pSystem->GetProductVersion();

	CryLogAlways("Crysis %d.%d.%d.%d " CRYMP_CLIENT_BITS, version[3], version[2], version[1], version[0]);
	CryLogAlways("CryMP Client " CRYMP_CLIENT_VERSION_STRING " " CRYMP_CLIENT_BITS);
	CryLogAlways("Compiled by " CRYMP_CLIENT_COMPILER " at " BUILD_DATE_TIME " [" CRYMP_CLIENT_BUILD_TYPE "]");
	CryLogAlways("Copyright (C) 2001-2008 Crytek GmbH");
	CryLogAlways("Copyright (C) 2014-2022 CryMP Network");
	CryLogAlways("");
}

void Launcher::OnShutdown()
{
}

void Launcher::OnUpdate()
{
	Logger::GetInstance().OnUpdate();
}

void Launcher::GetMemoryUsage(ICrySizer *pSizer)
{
}

void Launcher::Run()
{
	m_params.logFileName = "CryMP-Client.log";

	m_params.hInstance = WinAPI::DLL_Get(nullptr);  // EXE handle
	m_params.pUserCallback = this;
	m_params.pLog = &Logger::GetInstance();

	SetCmdLine();

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
	InitWorkingDirectory();

	// load and patch Crysis DLLs
	LoadEngine();
	PatchEngine();

	// the multiplayer client
	Client client;
	gClient = &client;

	StartEngine();

	UpdateLoop();
}
