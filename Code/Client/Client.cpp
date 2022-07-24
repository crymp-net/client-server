#include <stdlib.h>  // atoi

#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/CryNetwork/INetwork.h"
#include "CryCommon/CryScriptSystem/IScriptSystem.h"
#include "Launcher/Resources.h"
#include "Library/CmdLine.h"
#include "Library/RandomSeeder.h"
#include "Library/Util.h"
#include "Library/WinAPI.h"

#include "Client.h"
#include "Executor.h"
#include "HTTPClient.h"
#include "FileDownloader.h"
#include "FileRedirector.h"
#include "FileCache.h"
#include "MapDownloader.h"
#include "GSMasterHook.h"
#include "ScriptCommands.h"
#include "ScriptCallbacks.h"
#include "ScriptBind_CPPAPI.h"
#include "ServerBrowser.h"
#include "ServerConnector.h"
#include "ServerPAK.h"
#include "EngineCache.h"
#include "ParticleManager.h"
#include "FlashFileHooks.h"

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
		catch (const Error & error)
		{
			CryLogAlways("$4[CryMP] Failed to read the masters.txt file: %s", error.what());
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
		m_masters.emplace_back("crymp.net");
	}

	m_pScriptCallbacks->OnMasterResolved();
}

void Client::SetVersionInLua()
{
	gEnv->pScriptSystem->SetGlobalValue("CRYMP_CLIENT", CRYMP_CLIENT_VERSION);
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
		ch = TABLE[GetRandomNumber<std::string_view::size_type>(0, TABLE.length() - 1)];
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

	gClient->GetServerConnector()->Connect(gClient->m_masters[0], host, atoi(port));
}

void Client::OnDisconnectCmd(IConsoleCmdArgs *pArgs)
{
	gClient->GetServerConnector()->Disconnect();
}

void Client::OnAddKeyBind(IConsoleCmdArgs* pArgs)
{
	const int count = pArgs->GetArgCount();

	if (pArgs->GetArgCount() >= 3)
	{
		std::string arg;
		for (int i = 2; i < pArgs->GetArgCount(); ++i)
		{
			arg += pArgs->GetArg(i);
			arg += " ";
		}
		const auto [it, added] = gClient->m_keyBinds.emplace(pArgs->GetArg(1), arg.c_str());
	}
}

struct KeyBindDumpSink : public IKeyBindDumpSink
{
	virtual void OnKeyBindFound(const char* sBind, const char* sCommand)
	{
		CryLogAlways("$8%10s : %s", sBind, sCommand);
	}
};

void Client::OnDumpKeyBindings(IConsoleCmdArgs* pArgs)
{
	KeyBindDumpSink dump;
	gEnv->pConsole->DumpKeyBinds(&dump);

	for (const auto& [name, handler] : gClient->m_keyBinds)
	{
		CryLogAlways("%10s : %s", name.c_str(), handler.c_str());
	}
}

Client::Client()
{
	RandomSeeder seeder;
	m_randomEngine.seed(seeder);
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
	m_pHTTPClient        = std::make_unique<HTTPClient>();
	m_pFileDownloader    = std::make_unique<FileDownloader>();
	m_pFileRedirector    = std::make_unique<FileRedirector>();
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

	// prepare Lua scripts
	m_scriptMain = WinAPI::GetDataResource(nullptr, RESOURCE_SCRIPT_MAIN);
	m_scriptJSON = WinAPI::GetDataResource(nullptr, RESOURCE_SCRIPT_JSON);
	m_scriptRPC  = WinAPI::GetDataResource(nullptr, RESOURCE_SCRIPT_RPC);

	AddFlashFileHook("Libs/UI/HUD_ChatSystem.gfx", RESOURCE_HUD_CHAT_SYSTEM_GFX);
	AddFlashFileHook("Libs/UI/HUD_VehicleStats.gfx", RESOURCE_HUD_VEHICLE_STATS_GFX);

	// register engine listeners
	pGameFramework->RegisterListener(this, "crymp-client", FRAMEWORKLISTENERPRIORITY_DEFAULT);
	pGameFramework->GetILevelSystem()->AddListener(this);
	pEntitySystem->AddSink(this);

	// replace connect, disconnect, and bind console commands
	pConsole->RemoveCommand("connect");
	pConsole->RemoveCommand("disconnect");
	pConsole->RemoveCommand("bind");
	pConsole->AddCommand("connect", OnConnectCmd, VF_RESTRICTEDMODE, "Usage: connect [HOST] [PORT]");
	pConsole->AddCommand("disconnect", OnDisconnectCmd, VF_RESTRICTEDMODE, "Usage: disconnect");
	pConsole->AddCommand("bind", OnAddKeyBind, VF_NOT_NET_SYNCED, "Usage: bind key");
	pConsole->AddCommand("dumpbindings", OnDumpKeyBindings, VF_RESTRICTEDMODE, "Usage: bind key");

	if (!CmdLine::HasArg("-keepcdkey"))
	{
		// workaround for legacy servers with CD-Key check
		SetRandomCDKey();
	}

	SetVersionInLua();

	// execute Lua scripts
	pScriptSystem->ExecuteBuffer(m_scriptJSON.data(), m_scriptJSON.length(), "JSON.lua");
	pScriptSystem->ExecuteBuffer(m_scriptRPC.data(), m_scriptRPC.length(), "RPC.lua");
	pScriptSystem->ExecuteBuffer(m_scriptMain.data(), m_scriptMain.length(), "Main.lua");

	InitMasters();
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

void Client::OnKeyPress(const char* key)
{
	for (auto &m : m_keyBinds) 
	{
		if (!strcmp(key, m.first.c_str()))
		{
			gEnv->pConsole->ExecuteString(m.second.c_str());
		}
	}
}

void Client::OnTick() //each sec
{
}

void Client::OnPostUpdate(float deltaTime)
{
	m_pExecutor->OnUpdate();
	m_pScriptCallbacks->OnUpdate(deltaTime);

	m_FrameCounter += deltaTime;
	if (m_FrameCounter > 1.0f)
	{
		OnTick();
		m_FrameCounter = 0.0f;
	}
	
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
	switch (event.m_event)
	{
		case eAE_disconnected:
		{
			EDisconnectionCause reason = static_cast<EDisconnectionCause>(event.m_value);
			const char *message = event.m_description;

			m_pScriptCallbacks->OnDisconnect(reason, message);
			m_pServerPAK->OnDisconnect(reason, message);

			// prevent evil servers from changing the client version
			SetVersionInLua();

			// clear binds on disconnect
			m_keyBinds.clear();

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

bool Client::OnBeforeSpawn(SEntitySpawnParams & params)
{
	return true;
}

void Client::OnSpawn(IEntity *pEntity, SEntitySpawnParams & params)
{
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
