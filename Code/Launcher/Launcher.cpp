#include <cstdlib>
#include <cstring>

#include "Cry3DEngine/TimeOfDay.h"
#include "CryAction/GameFramework.h"
#include "CryCommon/CryAction/IGameFramework.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/CrySystem/ICryPak.h"
#include "CryMP/Client/Client.h"
#include "CryScriptSystem/ScriptSystem.h"
#include "CrySystem/CPUInfo.h"
#include "CrySystem/CryMemoryManager.h"
#include "CrySystem/GameWindow.h"
#include "CrySystem/LocalizationManager.h"
#include "CrySystem/Logger.h"
#include "CrySystem/RandomGenerator.h"
#include "Library/CrashLogger.h"
#include "Library/StringTools.h"
#include "Library/WinAPI.h"

#include "Launcher.h"
#include "MemoryPatch.h"

#include "config.h"

static bool InitCrySystem(void* pCrySystem, SSystemInitParams& params, bool oldAction)
{
	using CrySystemEntry = ISystem* (*)(SSystemInitParams&);

	if (oldAction)
	{
		// original CryAction initializes CrySystem on its own
		return true;
	}

	auto entry = static_cast<CrySystemEntry>(WinAPI::DLL::GetSymbol(pCrySystem, "CreateSystemInterface"));
	if (!entry)
	{
		throw StringTools::ErrorFormat("The CrySystem DLL is not valid!");
	}

	if (!entry(params))
	{
		throw StringTools::ErrorFormat("CrySystem initialization failed!");
	}

	return true;
}

static IScriptSystem* CreateNewScriptSystem(ISystem* pSystem, bool)
{
	CryLogAlways("$3[CryMP] Initializing Script System");

	ScriptSystem* pScriptSystem = new ScriptSystem();
	pScriptSystem->Init();

	return pScriptSystem;
}

static void ReplaceScriptSystem(void* pCrySystem)
{
	void* pNewFunc = CreateNewScriptSystem;

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

	WinAPI::FillMem(WinAPI::RVA(pCrySystem, codeOffset), code, sizeof code);
	WinAPI::FillNOP(WinAPI::RVA(pCrySystem, codeOffset + sizeof code), codeSize - sizeof code);
}

static bool LanguagePakExists(std::string_view language)
{
	std::filesystem::path path = "Game";
	path /= "Localized";
	path /= language;
	path += ".pak";

	return std::filesystem::exists(path);
}

static std::string_view ChooseLanguage(std::string_view defaultLanguage, ICVar* pLanguageCVar)
{
	std::string_view language = WinAPI::CmdLine::GetArgValue("-language");

	if (!language.empty())
	{
		return language;
	}

	if (pLanguageCVar)
	{
		const std::string_view value = pLanguageCVar->GetString();

		if (!value.empty())
		{
			return value;
		}
	}

	language = defaultLanguage;

	if (language.empty())
	{
		CryLogAlways("$4[CryMP] Missing or invalid Game/Localized/Default.lng file!");
		CryLogAlways("$4[CryMP] Trying to guess language from the system!");
		language = LocalizationManager::GetLanguageFromSystem();

		if (language.empty())
		{
			CryLogAlways("$4[CryMP] Failed to guess language from the system!");
			CryLogAlways("$4[CryMP] Falling back to English language!");
			language = "English";
		}
	}

	bool exists = LanguagePakExists(language);
	if (!exists)
	{
		if (language == "English")
		{
			CryLogAlways("$4[CryMP] Not even English language exists!");
		}
		else
		{
			CryLogAlways("$4[CryMP] %s language does not exist!", language.data());
			CryLogAlways("$4[CryMP] Falling back to English language!");
			language = "English";

			exists = LanguagePakExists(language);
			if (!exists)
			{
				CryLogAlways("$4[CryMP] Not even English language exists!");
			}
		}
	}

	if (!exists)
	{
		CryLogAlways("$4[CryMP] No suitable language found!");

		WinAPI::ErrorBox(
			"No suitable language found!\n"
			"\n"
			"Localization files are incomplete!\n"
			"This is a known issue in the Steam version of Crysis.\n"
			"\n"
			"You can try the following:\n"
			"    1. Go to Game/Localized\n"
			"    2. Choose a suitable *.lng file\n"
			"    3. Make a copy of that file\n"
			"    4. Rename the copy to Default.lng\n"
			"\n"
			"One or more *.pak files of the chosen language must exist!"
		);

		// throwing an exception through the engine is undefined behavior
		std::exit(1);
	}

	return language;
}

static void ReplaceLocalizationManager(void* pCrySystem)
{
	struct DummyCSystem
	{
		ILocalizationManager* GetLocalizationManager()
		{
			return &LocalizationManager::GetInstance();
		}

		static void InitLocalizationManager(const char* defaultLanguage)
		{
			CryLogAlways("$3[CryMP] Initializing Localization Manager");

			ICVar* pLanguageCVar = gEnv->pConsole->GetCVar("g_language");

			const std::string_view language = ChooseLanguage(defaultLanguage, pLanguageCVar);
			CryLogAlways("$3[CryMP] Using %s language", language.data());

			if (pLanguageCVar)
			{
				pLanguageCVar->Set(language.data());
			}

			LocalizationManager::GetInstance().SetLanguage(language.data());
		}
	};

	using TGetLocalizationManager = decltype(&DummyCSystem::GetLocalizationManager);

	void* pNewFunc = &DummyCSystem::InitLocalizationManager;

#ifdef BUILD_64BIT
	const std::size_t vtableOffset = 0x26ACF8;

	unsigned char code[] = {
		0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // mov rax, 0x0
		0x48, 0x8B, 0x4C, 0x24, 0x30,                                // mov rcx, qword ptr ss:[rsp+0x30]
		0xFF, 0xD0                                                   // call rax
	};

	std::memcpy(&code[2], &pNewFunc, 8);

	WinAPI::FillNOP(WinAPI::RVA(pCrySystem, 0x453A7), 0x1A8);
	WinAPI::FillMem(WinAPI::RVA(pCrySystem, 0x453A7), code, sizeof code);
	WinAPI::FillNOP(WinAPI::RVA(pCrySystem, 0x50A5C), 0x28);
#else
	const std::size_t vtableOffset = 0x1BC5F8;

	unsigned char code[] = {
		0xB8, 0x00, 0x00, 0x00, 0x00,  // mov eax, 0x0
		0xFF, 0x74, 0x24, 0x1C,        // push dword ptr ss:[esp+0x1C]
		0xFF, 0xD0,                    // call eax
		0x83, 0xC4, 0x04               // add esp, 0x4
	};

	std::memcpy(&code[1], &pNewFunc, 4);

	WinAPI::FillNOP(WinAPI::RVA(pCrySystem, 0x56B1D), 0xA4);
	WinAPI::FillMem(WinAPI::RVA(pCrySystem, 0x56B1D), code, sizeof code);
	WinAPI::FillNOP(WinAPI::RVA(pCrySystem, 0x624E1), 0x23);
#endif
	void** vtable = static_cast<void**>(WinAPI::RVA(pCrySystem, vtableOffset));

	// vtable hook
	TGetLocalizationManager newFunc = &DummyCSystem::GetLocalizationManager;
	WinAPI::FillMem(&vtable[105], &reinterpret_cast<void*&>(newFunc), sizeof(void*));
}

static ITimeOfDay* CreateTimeOfDay()
{
	CryLogAlways("$3[CryMP] Initializing Time Of Day");

	return new TimeOfDay(gLauncher->GetDLLs().pCry3DEngine);
}

static void DestroyTimeOfDay(void* pTimeOfDay)
{
	delete static_cast<TimeOfDay*>(pTimeOfDay);
}

static float GetTimeOfDayHDRMultiplier()
{
	return static_cast<TimeOfDay*>(gEnv->p3DEngine->GetTimeOfDay())->GetHDRMultiplier();
}

static void ReplaceTimeOfDay(void* pCry3DEngine)
{
	void* ctorFunc = &CreateTimeOfDay;
	void* dtorFunc = &DestroyTimeOfDay;
	void* getHDRMultiplierFunc = &GetTimeOfDayHDRMultiplier;

#ifdef BUILD_64BIT
	unsigned char ctorCode[] = {
		0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // mov rax, 0x0
		0xFF, 0xD0,                                                  // call rax
		0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,              // nop...
		0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
	};

	unsigned char dtorCode[] = {
		0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // mov rax, 0x0
		0xFF, 0xD0,                                                  // call rax
		0x90, 0x90, 0x90, 0x90                                       // nop...
	};

	unsigned char getHDRMultiplierCode[] = {
		0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // mov rax, 0x0
		0xFF, 0xD0,                                                  // call rax
		0x0F, 0x28, 0xD0,                                            // movaps xmm2, xmm0
		0x48, 0x8D, 0x4C, 0x24, 0x20,                                // lea rcx, qword ptr ss:[rsp+0x20]
		0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90         // nop...
	};

	std::memcpy(&ctorCode[2], &ctorFunc, 8);
	std::memcpy(&dtorCode[2], &dtorFunc, 8);
	std::memcpy(&getHDRMultiplierCode[2], &getHDRMultiplierFunc, 8);

	WinAPI::FillMem(WinAPI::RVA(pCry3DEngine, 0xFB81A), ctorCode, sizeof ctorCode);
	WinAPI::FillMem(WinAPI::RVA(pCry3DEngine, 0xFC505), dtorCode, sizeof dtorCode);
	WinAPI::FillMem(WinAPI::RVA(pCry3DEngine, 0xF38CC), getHDRMultiplierCode, sizeof getHDRMultiplierCode);
#else
	unsigned char ctorCode[] = {
		0xB8, 0x00, 0x00, 0x00, 0x00,  // mov eax, 0x0
		0xFF, 0xD0,                    // call eax
		0x90, 0x90, 0x90, 0x90,        // nop...
		0x90, 0x90, 0x90, 0x90,
		0x90, 0x90, 0x90, 0x90,
		0x90, 0x90, 0x90, 0x90,
		0x90, 0x90
	};

	unsigned char dtorCode[] = {
		0xB8, 0x00, 0x00, 0x00, 0x00,  // mov eax, 0x0
		0x57,                          // push edi
		0xFF, 0xD0,                    // call eax
		0x83, 0xC4, 0x04,              // add esp, 0x4
		0x90, 0x90, 0x90, 0x90, 0x90   // nop...
	};

	unsigned char getHDRMultiplierCode[] = {
		0xB8, 0x00, 0x00, 0x00, 0x00,        // mov eax, 0x0
		0xFF, 0xD0,                          // call eax
		0xD9, 0x5C, 0x24, 0xFC,              // fstp dword ptr ss:[esp-0x4], st(0)
		0xF3, 0x0F, 0x10, 0x44, 0x24, 0xFC,  // movss xmm0, dword ptr ss:[esp-0x4]
		0x90, 0x90, 0x90, 0x90               // nop...
	};

	std::memcpy(&ctorCode[1], &ctorFunc, 4);
	std::memcpy(&dtorCode[1], &dtorFunc, 4);
	std::memcpy(&getHDRMultiplierCode[1], &getHDRMultiplierFunc, 4);

	WinAPI::FillMem(WinAPI::RVA(pCry3DEngine, 0xBE70B), ctorCode, sizeof ctorCode);
	WinAPI::FillMem(WinAPI::RVA(pCry3DEngine, 0xBF0D6), dtorCode, sizeof dtorCode);
	WinAPI::FillMem(WinAPI::RVA(pCry3DEngine, 0xB8D7D), getHDRMultiplierCode, sizeof getHDRMultiplierCode);
#endif
}

static void EnableHiddenProfilerSubsystems(ISystem* pSystem)
{
	struct Subsystem
	{
		const char* name;
		float reserved;
	};

#ifdef BUILD_64BIT
	static_assert(sizeof(Subsystem) == 16);
#else
	static_assert(sizeof(Subsystem) == 8);
#endif

	IFrameProfileSystem* pProfiler = pSystem->GetIProfileSystem();

#ifdef BUILD_64BIT
	void* subsystemsBegin = reinterpret_cast<unsigned char*>(pProfiler) + 0xAC8;
#else
	void* subsystemsBegin = reinterpret_cast<unsigned char*>(pProfiler) + 0x634;
#endif
	Subsystem* subsystems = static_cast<Subsystem*>(subsystemsBegin);

	subsystems[PROFILE_ANY].name = "Unknown";
	subsystems[PROFILE_MOVIE].name = "Movie";
	subsystems[PROFILE_FONT].name = "Font";
	subsystems[PROFILE_SCRIPT].name = "Script";
}

void Launcher::SetCmdLine()
{
	const std::string_view cmdLine = WinAPI::CmdLine::GetFull();

	if (cmdLine.length() >= sizeof m_params.cmdLine)
	{
		throw StringTools::ErrorFormat("Command line is too long!");
	}

	std::memcpy(m_params.cmdLine, cmdLine.data(), cmdLine.length());

	m_params.cmdLine[cmdLine.length()] = '\0';
}

void Launcher::InitWorkingDirectory()
{
	std::filesystem::path dir;

	if (const std::string_view dirArgValue = WinAPI::CmdLine::GetArgValue("-dir"); !dirArgValue.empty())
	{
		dir = dirArgValue;
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
		WinAPI::DLL::AddSearchDirectory(dir / BIN_DIR);
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
	m_dlls.pCrySystem = WinAPI::DLL::Load("CrySystem.dll");
	if (!m_dlls.pCrySystem)
	{
		if (WinAPI::GetCurrentErrorCode() == 193)  // ERROR_BAD_EXE_FORMAT
		{
			throw StringTools::ErrorFormat("Failed to load the CrySystem DLL!\n\n"
#ifdef BUILD_64BIT
				"It seems you have 32-bit DLLs in your Bin64 directory! Please fix it."
#else
				"It seems you have 64-bit DLLs in your Bin32 directory! Please fix it."
#endif
			);
		}
		else
		{
			throw StringTools::SysErrorFormat("Failed to load the CrySystem DLL!");
		}
	}

	const int gameVersion = WinAPI::GetCrysisGameBuild(m_dlls.pCrySystem);
	if (gameVersion < 0)
	{
		throw StringTools::SysErrorFormat("Failed to get the game version!");
	}

	CryMemoryManager::Init(m_dlls.pCrySystem);

	switch (gameVersion)
	{
		case 5767:
		{
			throw StringTools::ErrorFormat(
				"Crysis 1.0 is not supported!\n\n"
				"Install 1.2 and 1.2.1 official patch."
			);
		}
		case 5879:
		{
			throw StringTools::ErrorFormat(
				"Crysis 1.1 is not supported!\n\n"
				"Install 1.2 and 1.2.1 official patch."
			);
		}
		case 6115:
		{
			throw StringTools::ErrorFormat(
				"Crysis 1.2 is not supported!\n\n"
				"Install 1.2.1 official patch."
			);
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
			throw StringTools::ErrorFormat("Crysis Wars is not supported!");
		}
		case 687:
		case 710:
		case 711:
		{
			throw StringTools::ErrorFormat("Crysis Warhead is not supported!");
		}
		default:
		{
			throw StringTools::ErrorFormat("Unknown game version %d!", gameVersion);
		}
	}

	m_dlls.pCryAction = WinAPI::DLL::Load("CryAction.dll");
	if (!m_dlls.pCryAction)
	{
		throw StringTools::SysErrorFormat("Failed to load the CryAction DLL!");
	}

	m_dlls.pCryNetwork = WinAPI::DLL::Load("CryNetwork.dll");
	if (!m_dlls.pCryNetwork)
	{
		throw StringTools::SysErrorFormat("Failed to load the CryNetwork DLL!");
	}

	m_dlls.pCry3DEngine = WinAPI::DLL::Load("Cry3DEngine.dll");
	if (!m_dlls.pCry3DEngine)
	{
		throw StringTools::SysErrorFormat("Failed to load the Cry3DEngine DLL!");
	}

	const bool isDX10 = !WinAPI::CmdLine::HasArg("-dx9") && (WinAPI::CmdLine::HasArg("-dx10") || WinAPI::IsVistaOrLater());

	if (isDX10)
	{
		m_dlls.pCryRenderD3D10 = WinAPI::DLL::Load("CryRenderD3D10.dll");
		if (!m_dlls.pCryRenderD3D10)
		{
			throw StringTools::SysErrorFormat("Failed to load the CryRenderD3D10 DLL!");
		}
	}
	else
	{
		m_dlls.pCryRenderD3D9 = WinAPI::DLL::Load("CryRenderD3D9.dll");
		if (!m_dlls.pCryRenderD3D9)
		{
			throw StringTools::SysErrorFormat("Failed to load the CryRenderD3D9 DLL!");
		}
	}
}

void Launcher::PatchEngine()
{
	if (m_dlls.pCryAction)
	{
		MemoryPatch::CryAction::AllowDX9ImmersiveMultiplayer(m_dlls.pCryAction);
		MemoryPatch::CryAction::DisableBreakLog(m_dlls.pCryAction);
	}

	if (m_dlls.pCryNetwork)
	{
		MemoryPatch::CryNetwork::AllowSameCDKeys(m_dlls.pCryNetwork);
		MemoryPatch::CryNetwork::EnablePreordered(m_dlls.pCryNetwork);
		MemoryPatch::CryNetwork::FixFileCheckCrash(m_dlls.pCryNetwork);
		MemoryPatch::CryNetwork::FixInternetConnect(m_dlls.pCryNetwork);
	}

	if (m_dlls.pCrySystem)
	{
		MemoryPatch::CrySystem::AllowDX9VeryHighSpec(m_dlls.pCrySystem);
		MemoryPatch::CrySystem::AllowMultipleInstances(m_dlls.pCrySystem);
		MemoryPatch::CrySystem::DisableIOErrorLog(m_dlls.pCrySystem);
		MemoryPatch::CrySystem::HookCPUDetect(m_dlls.pCrySystem, &CPUInfo::Detect);
		MemoryPatch::CrySystem::HookError(m_dlls.pCrySystem, &CrashLogger::OnEngineError);
		//MemoryPatch::CrySystem::MakeDX9Default(m_dlls.pCrySystem);
		MemoryPatch::CrySystem::RemoveSecuROM(m_dlls.pCrySystem);
		MemoryPatch::CrySystem::UnhandledExceptions(m_dlls.pCrySystem);

		if (!WinAPI::CmdLine::HasArg("-oldss"))
		{
			ReplaceScriptSystem(m_dlls.pCrySystem);
		}

		ReplaceLocalizationManager(m_dlls.pCrySystem);
	}

	if (m_dlls.pCry3DEngine)
	{
		if (WinAPI::CmdLine::HasArg("-newtod"))
		{
			ReplaceTimeOfDay(m_dlls.pCry3DEngine);
		}
	}

	const char* GAME_WINDOW_NAME = "CryMP Client " CRYMP_CLIENT_VERSION_STRING;

	if (m_dlls.pCryRenderD3D9)
	{
		MemoryPatch::CryRenderD3D9::HookWindowNameD3D9(m_dlls.pCryRenderD3D9, GAME_WINDOW_NAME);
	}

	if (m_dlls.pCryRenderD3D10)
	{
		MemoryPatch::CryRenderD3D10::FixLowRefreshRateBug(m_dlls.pCryRenderD3D10);
		MemoryPatch::CryRenderD3D10::HookWindowNameD3D10(m_dlls.pCryRenderD3D10, GAME_WINDOW_NAME);
	}
}

void Launcher::StartEngine()
{
	const bool oldAction = WinAPI::CmdLine::HasArg("-oldaction");

	IGameFramework* pGameFramework = nullptr;

	if (oldAction)
	{
		using CryActionEntry = IGameFramework::TEntryFunction;

		auto entry = static_cast<CryActionEntry>(WinAPI::DLL::GetSymbol(m_dlls.pCryAction, "CreateGameFramework"));
		if (!entry)
		{
			throw StringTools::ErrorFormat("The CryAction DLL is not valid!");
		}

		pGameFramework = entry();
		if (!pGameFramework)
		{
			throw StringTools::ErrorFormat("Failed to create the GameFramework Interface!");
		}
	}
	else
	{
		pGameFramework = GameFramework::GetInstance();
	}

	GameWindow::GetInstance().Init();

	// initialize CryEngine
	// Launcher::OnInit is called here
	if (!InitCrySystem(m_dlls.pCrySystem, m_params, oldAction) || !pGameFramework->Init(m_params))
	{
		throw StringTools::ErrorFormat("CryENGINE initialization failed!");
	}

	// initialize CryMP and CryGame
	gClient->Init(pGameFramework);

	if (!pGameFramework->CompleteInit())
	{
		throw StringTools::ErrorFormat("CryENGINE post-initialization failed!");
	}

	gEnv->pSystem->ExecuteCommandLine();
}

Launcher::Launcher()
{
	std::memset(&m_params, 0, sizeof m_params);
}

Launcher::~Launcher()
{
}

bool Launcher::OnError(const char* error)
{
	return true;
}

void Launcher::OnSaveDocument()
{
}

void Launcher::OnProcessSwitch()
{
}

void Launcher::OnInitProgress(const char* message)
{
}

void Launcher::OnInit(ISystem* pSystem)
{
	gEnv = pSystem->GetGlobalEnvironment();

	const std::filesystem::path mainDirPath = std::filesystem::current_path();
	const std::filesystem::path userDirPath = std::filesystem::canonical(gEnv->pCryPak->GetAlias("%USER%"));
	const std::filesystem::path rootDirPath = std::filesystem::canonical(gEnv->pSystem->GetRootFolder());

	const char* defaultVerbosity = "0";
	const char* defaultLogFileName = "CryMP-Client.log";
	const char* defaultLogPrefix = "";

	const int verbosity = std::atoi(WinAPI::CmdLine::GetArgValue("-verbosity", defaultVerbosity));
	const char* logFileName = WinAPI::CmdLine::GetArgValue("-logfile", defaultLogFileName);
	const char* logPrefix = WinAPI::CmdLine::GetArgValue("-logprefix", defaultLogPrefix);

	Logger& logger = Logger::GetInstance();

	logger.SetVerbosity(verbosity);
	logger.OpenFile((rootDirPath.empty() ? userDirPath : rootDirPath) / logFileName);

	CrashLogger::Enable([]() -> std::FILE* { return Logger::GetInstance().GetFileHandle(); });

	logger.LogAlways("Log begins at %s", Logger::FormatPrefix("%F %T%z").c_str());

	const std::string exe = WinAPI::GetApplicationPath().string();
	const std::string mainDir = mainDirPath.string();
	const std::string rootDir = rootDirPath.string();
	const std::string userDir = userDirPath.string();

	logger.LogAlways("Executable: %s", exe.c_str());
	logger.LogAlways("Main directory: %s", mainDir.c_str());
	logger.LogAlways("Root directory: %s", rootDir.empty() ? mainDir.c_str() : rootDir.c_str());
	logger.LogAlways("User directory: %s", userDir.c_str());
	logger.LogAlways("");

	const SFileVersion& version = gEnv->pSystem->GetProductVersion();

	logger.LogAlways("Crysis %d.%d.%d.%d " CRYMP_CLIENT_BITS, version[3], version[2], version[1], version[0]);
	logger.LogAlways("CryMP Client " CRYMP_CLIENT_VERSION_STRING " " CRYMP_CLIENT_BITS " " CRYMP_CLIENT_BUILD_TYPE);
	logger.LogAlways("Compiled by " CRYMP_CLIENT_COMPILER);
	logger.LogAlways("Copyright (C) 2001-2008 Crytek GmbH");
	logger.LogAlways("Copyright (C) 2014-2022 CryMP Network");
	logger.LogAlways("");

	logger.SetPrefix(logPrefix);

	EnableHiddenProfilerSubsystems(pSystem);
}

void Launcher::OnShutdown()
{
}

void Launcher::OnUpdate()
{
	Logger::GetInstance().OnUpdate();
}

void Launcher::GetMemoryUsage(ICrySizer* pSizer)
{
}

void Launcher::Run()
{
	m_params.hInstance = WinAPI::DLL::Get(nullptr);  // EXE handle
	m_params.pUserCallback = this;
	m_params.pLog = &Logger::GetInstance();

	this->SetCmdLine();

	if (WinAPI::GetApplicationPath().filename().string().find(CRYMP_CLIENT_EXE_NAME) != 0)
	{
		throw StringTools::ErrorFormat("Invalid name of the executable!");
	}

	if (WinAPI::CmdLine::HasArg("-mod"))
	{
		throw StringTools::ErrorFormat("Mods are not supported!");
	}

	if (WinAPI::CmdLine::HasArg("-dedicated") || m_params.isDedicatedServer)
	{
		throw StringTools::ErrorFormat("Running as a dedicated server is not supported!");
	}

	this->InitWorkingDirectory();

	this->LoadEngine();
	this->PatchEngine();

	RandomGenerator::Init();

	Client client;
	gClient = &client;

	this->StartEngine();

	gClient->UpdateLoop();
}
