#include <stdlib.h>  // atoi

#include <tracy/Tracy.hpp>

#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/CryNetwork/INetwork.h"
#include "CryCommon/CryScriptSystem/IScriptSystem.h"
#include "CryCommon/Cry3DEngine/I3DEngine.h"
#include "CryGame/Game.h"
#include "CryMP/Common/Executor.h"
#include "CryMP/Common/GSMasterHook.h"
#include "CrySystem/GameWindow.h"
#include "CrySystem/RandomGenerator.h"
#include "Launcher/Resources.h"
#include "Library/StringTools.h"
#include "Library/Util.h"
#include "Library/WinAPI.h"

#include "Client.h"
#include "FileDownloader.h"
#include "FileCache.h"
#include "MapDownloader.h"
#include "ScriptCommands.h"
#include "ScriptCallbacks.h"
#include "ScriptBind_CPPAPI.h"
#include "ServerBrowser.h"
#include "ServerConnector.h"
#include "ServerPAK.h"
#include "EngineCache.h"
#include "ParticleManager.h"
#include "FlashFileHooks.h"
#include "DrawTools.h"
#include "FFontHooks.h"

#include "config.h"

void Client::InitMasters()
{
	std::string content;

	WinAPI::File file("masters.txt", WinAPI::FileAccess::READ_ONLY);  // Crysis main directory
	if (file)
	{
		CryLogAlways("$6[CryMP] Using local masters.txt as the master server list provider");

		try
		{
			content = file.Read();
		}
		catch (const std::exception& ex)
		{
			CryLogAlways("$4[CryMP] Failed to read the masters.txt file: %s", ex.what());
		}
	}
	else
	{
		content = WinAPI::GetDataResource(nullptr, RESOURCE_MASTERS);
	}

	for (const std::string_view & master : Util::SplitWhitespace(content))
	{
		m_masters.emplace_back(master);
	}

	if (m_masters.empty())
	{
		m_masters.emplace_back("crymp.org");
	}

	m_pScriptCallbacks->OnMasterResolved();
}

void Client::SetVersionInLua()
{
	gEnv->pScriptSystem->SetGlobalValue("CRYMP_CLIENT", CRYMP_CLIENT_VERSION);
	gEnv->pScriptSystem->SetGlobalValue("CRYMP_CLIENT_STRING", CRYMP_CLIENT_VERSION_STRING);
	gEnv->pScriptSystem->SetGlobalValue("CRYMP_CLIENT_BITS", CRYMP_CLIENT_BITS);
}

void Client::AddFlashFileHook(const std::string_view& path, int resourceID)
{
	m_pFlashFileHooks->Add(path, WinAPI::GetDataResource(nullptr, resourceID));
}

std::string Client::GenerateRandomCDKey()
{
	constexpr std::string_view TABLE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

	std::string key;
	key.resize(20);

	for (char& ch : key)
	{
		ch = TABLE[RandomGenerator::GenerateIndex(TABLE.size())];
	}

	return key;
}

void Client::SetRandomCDKey()
{
	const std::string key = GenerateRandomCDKey();

	CryLogAlways("[CryMP] Using random CD-Key '%s'", key.c_str());

	gEnv->pNetwork->SetCDKey(key.c_str());
}

void Client::OnConnectCmd(IConsoleCmdArgs *pArgs)
{
	IConsole *pConsole = gEnv->pConsole;

	const char *host = "";
	const char *port = "";

	if (pArgs->GetArgCount() > 1)
		host = pArgs->GetArg(1);
	else
		host = pConsole->GetCVar("cl_serveraddr")->GetString();

	if (pArgs->GetArgCount() > 2)
		port = pArgs->GetArg(2);
	else
		port = pConsole->GetCVar("cl_serverport")->GetString();

	ServerInfo server;
	server.master = gClient->m_masters[0];
	server.public_host = host;
	server.public_port = atoi(port);
	server.SplitPublicPort();

	gClient->GetServerConnector()->Connect(server);
}

void Client::OnReconnectCmd(IConsoleCmdArgs *pArgs)
{
	gClient->GetServerConnector()->Reconnect();
}

void Client::OnDisconnectCmd(IConsoleCmdArgs *pArgs)
{
	gClient->GetServerConnector()->Disconnect();
}

void Client::OnKeyBindCmd(IConsoleCmdArgs* pArgs)
{
	const int argCount = pArgs->GetArgCount();

	if (argCount > 2)
	{
		std::string command;

		for (int i = 2; i < argCount; i++)
		{
			command += pArgs->GetArg(i);
			command += ' ';
		}

		gClient->AddKeyBind(pArgs->GetArg(1), command);
	}
}

void Client::OnDumpKeyBindsCmd(IConsoleCmdArgs* pArgs)
{
	struct Sink : public IKeyBindDumpSink
	{
		void OnKeyBindFound(const char* key, const char* command) override
		{
			CryLogAlways("$8%10s : %s", key, command);
		}
	};

	Sink sink;
	gEnv->pConsole->DumpKeyBinds(&sink);

	for (const KeyBind& bind : gClient->m_keyBinds)
	{
		CryLogAlways("%10s : %s", bind.key.c_str(), bind.command.c_str());
	}
}

Client::Client()
{
	m_hwid = GetHWID("idsvc");
	m_locale = WinAPI::GetLocale();
	m_timezone = std::to_string(WinAPI::GetTimeZoneBias());
}

Client::~Client()
{
}

void Client::Init(IGameFramework *pGameFramework)
{
	IConsole *pConsole = gEnv->pConsole;
	IScriptSystem *pScriptSystem = gEnv->pScriptSystem;
	IEntitySystem *pEntitySystem = gEnv->pEntitySystem;

	m_pGameFramework = pGameFramework;

	// initialize client components
	m_pExecutor          = std::make_unique<Executor>();
	m_pHTTPClient        = std::make_unique<HTTPClient>(*m_pExecutor);
	m_pFileDownloader    = std::make_unique<FileDownloader>();
	m_pFileCache         = std::make_unique<FileCache>();
	m_pMapDownloader     = std::make_unique<MapDownloader>();
	m_pGSMasterHook      = std::make_unique<GSMasterHook>();
	m_pScriptCommands    = std::make_unique<ScriptCommands>();
	m_pScriptCallbacks   = std::make_unique<ScriptCallbacks>();
	m_pScriptBind_CPPAPI = std::make_unique<ScriptBind_CPPAPI>();
	m_pServerBrowser     = std::make_unique<ServerBrowser>();
	m_pServerConnector   = std::make_unique<ServerConnector>();
	m_pServerPAK         = std::make_unique<ServerPAK>();
	m_pEngineCache       = std::make_unique<EngineCache>();
	m_pParticleManager   = std::make_unique<ParticleManager>();
	m_pFlashFileHooks    = std::make_unique<FlashFileHooks>();
	m_pDrawTools         = std::make_unique<DrawTools>();

	// prepare Lua scripts
	m_scriptMain         = WinAPI::GetDataResource(nullptr, RESOURCE_SCRIPT_MAIN);
	m_scriptJSON         = WinAPI::GetDataResource(nullptr, RESOURCE_SCRIPT_JSON);
	m_scriptRPC          = WinAPI::GetDataResource(nullptr, RESOURCE_SCRIPT_RPC);
	m_scriptLocalization = WinAPI::GetDataResource(nullptr, RESOURCE_SCRIPT_LOCALIZATION);

	AddFlashFileHook("Libs/UI/HUD_ChatSystem.gfx", RESOURCE_HUD_CHAT_SYSTEM_GFX);
	AddFlashFileHook("Libs/UI/HUD_VehicleStats.gfx", RESOURCE_HUD_VEHICLE_STATS_GFX);
	AddFlashFileHook("Libs/UI/HUD_PDA_Buy.gfx", RESOURCE_HUD_PDA_BUY_GFX);
	AddFlashFileHook("Libs/UI/Menus_Loading_MP.gfx", RESOURCE_MENUS_LOADING_MP_GFX);
	AddFlashFileHook("Libs/UI/HUD_HitIndicator.gfx", RESOURCE_HUD_HIT_INDICATOR_GFX);
	AddFlashFileHook("Libs/UI/HUD_Spectate.gfx", RESOURCE_HUD_SPECTATE_GFX);
	AddFlashFileHook("Libs/UI/HUD_MP_Radio_Buttons.gfx", RESOURCE_HUD_MP_RADIO_BUTTONS_GFX);
	AddFlashFileHook("Libs/UI/HUD_ChatSystem_HR.gfx", RESOURCE_HUD_CHAT_SYSTEM_HR_GFX);
	AddFlashFileHook("Libs/UI/HUD_KillLog.gfx", RESOURCE_HUD_KILL_LOG_GFX);

	PatchCryFont();

	// register engine listeners
	pGameFramework->RegisterListener(this, "crymp-client", FRAMEWORKLISTENERPRIORITY_DEFAULT);
	pGameFramework->GetILevelSystem()->AddListener(this);
	pEntitySystem->AddSink(this);

	// replace connect, disconnect, and bind console commands
	pConsole->RemoveCommand("connect");
	pConsole->RemoveCommand("disconnect");
	pConsole->RemoveCommand("bind");
	pConsole->AddCommand("connect", OnConnectCmd, VF_RESTRICTEDMODE, "Usage: connect [HOST] [PORT]");
	pConsole->AddCommand("reconnect", OnReconnectCmd, VF_RESTRICTEDMODE, "Usage: reconnect");
	pConsole->AddCommand("disconnect", OnDisconnectCmd, VF_RESTRICTEDMODE, "Usage: disconnect");
	pConsole->AddCommand("bind", OnKeyBindCmd, VF_NOT_NET_SYNCED, "Usage: bind key command");
	pConsole->AddCommand("dumpbinds", OnDumpKeyBindsCmd, VF_RESTRICTEDMODE, "Usage: dumpbinds");

	if (!WinAPI::CmdLine::HasArg("-keepcdkey"))
	{
		// workaround for legacy servers with CD-Key check
		SetRandomCDKey();
	}

	SetVersionInLua();

	// execute Lua scripts
	pScriptSystem->ExecuteBuffer(m_scriptJSON.data(), m_scriptJSON.length(), "JSON.lua");
	pScriptSystem->ExecuteBuffer(m_scriptRPC.data(), m_scriptRPC.length(), "RPC.lua");
	pScriptSystem->ExecuteBuffer(m_scriptMain.data(), m_scriptMain.length(), "Main.lua");
	pScriptSystem->ExecuteBuffer(m_scriptLocalization.data(), m_scriptLocalization.length(), "Localization.lua");

	InitMasters();

	m_pServerBrowser->QueryClientPublicAddress();

	if (!WinAPI::CmdLine::HasArg("-oldgame"))
	{
		m_pGame = new CGame();
	}
	else
	{
		void* pCryGame = WinAPI::DLL::Load("CryGame.dll");
		if (!pCryGame)
		{
			throw StringTools::SysErrorFormat("Failed to load the CryGame DLL!");
		}

		auto entry = static_cast<IGame::TEntryFunction>(WinAPI::DLL::GetSymbol(pCryGame, "CreateGame"));
		if (!entry)
		{
			throw StringTools::ErrorFormat("The CryGame DLL is not valid!");
		}

		m_pGame = entry(pGameFramework);
	}

	// initialize the game
	// mods are not supported
	m_pGame->Init(pGameFramework);
}

void Client::UpdateLoop()
{
	gEnv->pConsole->ExecuteString("exec autoexec.cfg");

	const bool haveFocus = true;
	const unsigned int updateFlags = 0;

	while (GameWindow::GetInstance().OnUpdate() && m_pGame->Update(haveFocus, updateFlags))
	{
		FrameMark;
	}

	GameWindow::GetInstance().OnQuit();
}

void Client::HttpGet(const std::string_view& url, std::function<void(HTTPClientResult&)> callback)
{
	HTTPClientRequest request;
	request.method = "GET";
	request.url = url;
	request.callback = std::move(callback);

	HttpRequest(std::move(request));
}

void Client::HttpRequest(HTTPClientRequest&& request)
{
	for (const std::string& master : m_masters)
	{
		if (Util::StartsWith(request.url, GetMasterServerAPI(master)))
		{
			request.headers["X-Sfwcl-HWID"] = m_hwid;
			request.headers["X-Sfwcl-Locale"] = m_locale;
			request.headers["X-Sfwcl-Tz"] = m_timezone;
			break;
		}
	}

	m_pHTTPClient->Request(std::move(request));
}

std::string Client::GetMasterServerAPI(const std::string & master)
{
	if (master.empty())
	{
		return "https://" + m_masters[0] + "/api";
	}
	else
	{
		int a = 0, b = 0, c = 0, d = 0;
		// in case it is IP, don't use HTTPS
		if (sscanf(master.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d) == 4 || master == "localhost")
			return "http://" + master + "/api";
		else
			return "https://" + master + "/api";
	}
}

std::string Client::GetHWID(const std::string_view & salt)
{
	std::string hwid = Util::SHA256(WinAPI::GetMachineGUID());

	if (!hwid.empty())
		hwid += ':' + Util::SHA256(hwid + std::string(salt));

	return hwid;
}

void Client::AddKeyBind(const std::string_view& key, const std::string_view& command)
{
	for (KeyBind& bind : m_keyBinds)
	{
		if (bind.key == key)
		{
			bind.command = command;
			return;
		}
	}

	KeyBind& bind = m_keyBinds.emplace_back();
	bind.key = key;
	bind.command = command;

	if (m_pGameFramework->GetClientActor())
	{
		bind.createdInGame = true;
	}
}

void Client::AddKeyBind(const std::string_view& key, HSCRIPTFUNCTION function)
{
	for (KeyBind& bind : m_keyBinds)
	{
		if (bind.key == key)
		{
			bind.function = SmartScriptFunction(gEnv->pScriptSystem, function);
			return;
		}
	}

	KeyBind& bind = m_keyBinds.emplace_back();
	bind.key = key;
	bind.function = SmartScriptFunction(gEnv->pScriptSystem, function);

	if (m_pGameFramework->GetClientActor())
	{
		bind.createdInGame = true;
	}
}

void Client::OnKeyPress(const std::string_view& key)
{
	for (const KeyBind& bind : m_keyBinds)
	{
		if (bind.key == key)
		{
			if (!bind.command.empty())
			{
				gEnv->pConsole->ExecuteString(bind.command.c_str());
			}
			if (bind.function)
			{
				IScriptSystem* pSS = gEnv->pScriptSystem;
				if (pSS->BeginCall(bind.function))
				{
					pSS->PushFuncParam(1); //press
					pSS->EndCall();
				}
			}
		}
	}
}

void Client::OnKeyRelease(const std::string_view& key)
{
	for (const KeyBind& bind : m_keyBinds)
	{
		if (bind.key == key)
		{
			if (bind.function)
			{
				IScriptSystem* pSS = gEnv->pScriptSystem;
				if (pSS->BeginCall(bind.function))
				{
					pSS->PushFuncParam(2); //release
					pSS->EndCall();
				}
			}
		}
	}
}

void Client::ClearKeyBinds()
{
	const auto createdInGame = [](KeyBind& bind) -> bool
	{
		return bind.createdInGame;
	};

	m_keyBinds.erase(std::remove_if(m_keyBinds.begin(), m_keyBinds.end(), createdInGame), m_keyBinds.end());

}

void Client::OnPostUpdate(float deltaTime)
{
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_GAME);

	m_pExecutor->OnUpdate();
	m_pScriptCallbacks->OnUpdate(deltaTime);
	m_pDrawTools->OnUpdate();
}

void Client::OnSaveGame(ISaveGame *pSaveGame)
{
}

void Client::OnLoadGame(ILoadGame *pLoadGame)
{
}

void Client::OnLevelEnd(const char *nextLevel)
{
}

void Client::OnActionEvent(const SActionEvent & event)
{
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_GAME);

	switch (event.m_event)
	{
		case eAE_disconnected:
		{
			EDisconnectionCause reason = static_cast<EDisconnectionCause>(event.m_value);
			const char *message = event.m_description;

			m_pScriptCallbacks->OnDisconnect(reason, message);
			m_pServerPAK->OnDisconnect(reason, message);
			m_pDrawTools->OnDisconnect(reason, message);

			// prevent evil servers from changing the client version
			SetVersionInLua();

			// clear binds on disconnect
			ClearKeyBinds();

			break;
		}
		case eAE_resetBegin:
		{
			m_pScriptCallbacks->OnLoadingStart();
			break;
		}
		case eAE_channelCreated:
		case eAE_channelDestroyed:
		case eAE_connectFailed:
		case eAE_connected:
		case eAE_clientDisconnected:
		case eAE_resetEnd:
		case eAE_resetProgress:
		case eAE_preSaveGame:
		case eAE_postSaveGame:
		case eAE_inGame:
		case eAE_serverName:
		case eAE_serverIp:
		case eAE_earlyPreUpdate:
		{
			break;
		}
	}
}

void Client::OnLevelNotFound(const char *levelName)
{
}

void Client::OnLoadingStart(ILevelInfo *pLevel)
{
	gEnv->pScriptSystem->ForceGarbageCollection();

	m_pServerPAK->OnLoadingStart(pLevel);
	m_pEngineCache->OnLoadingStart(pLevel);
	m_pScriptCallbacks->OnLoadingStart();
}

void Client::OnLoadingComplete(ILevel *pLevel)
{
}

void Client::OnLoadingError(ILevelInfo *pLevel, const char *error)
{
}

void Client::OnLoadingProgress(ILevelInfo *pLevel, int progressAmount)
{
	m_pEngineCache->OnLoadingProgress(pLevel, progressAmount);
}

bool Client::OnBeforeSpawn(SEntitySpawnParams& params)
{
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_GAME);

	//CryMP: Archetype Loader
	if (params.sName && params.sName[0] == '*')
	{
		const char* name = params.sName + 1;
		const char* archetypeEnd = strchr(name, '|');

		if (archetypeEnd)
		{
			const std::string archetypeName(name, archetypeEnd - name);

			if (!gEnv->bServer)
			{
				params.sName = archetypeEnd + 1;
			}
			params.pArchetype = gEnv->pEntitySystem->LoadEntityArchetype(archetypeName.c_str());

			if (!params.pArchetype && params.pClass)
			{
				CryLogWarningAlways("[CryMP] Archetype '%s' not found for entity %s",
					archetypeName.c_str(),
					params.pClass->GetName()
				);

				if (gEnv->bServer) //If archetype loading failed, don't proceed spawn on the server
					return false;
			}
		}
	}

	return true;
}

void Client::OnSpawn(IEntity *pEntity, SEntitySpawnParams & params)
{
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_GAME);

	m_pScriptCallbacks->OnSpawn(pEntity);

	m_lastSpawnId = pEntity->GetId();
}

bool Client::OnRemove(IEntity *pEntity)
{
	return true;
}

void Client::OnEvent(IEntity *pEntity, SEntityEvent & event)
{
}
