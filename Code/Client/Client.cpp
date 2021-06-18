#include "CryCommon/ISystem.h"
#include "CryCommon/IConsole.h"
#include "CryCommon/IScriptSystem.h"
#include "CryCommon/INetwork.h"
#include "CryCommon/ICryPak.h"
#include "Launcher/Resources.h"
#include "Library/RandomSeeder.h"
#include "Library/WinAPI.h"
#include "Library/External/nlohmann/json.hpp"

#include "Client.h"
#include "Executor.h"
#include "HTTPClient.h"
#include "GSMasterHook.h"
#include "ScriptCommands.h"
#include "ScriptCallbacks.h"
#include "ScriptBind_CPPAPI.h"

void Client::OnConnectCmd(IConsoleCmdArgs *pArgs)
{
	IGameFramework *pGameFramework = gClient->getGameFramework();

	if (!gEnv->bServer)
	{
		INetChannel *pClientChannel = pGameFramework->GetClientChannel();

		if (pClientChannel)
		{
			pClientChannel->Disconnect(eDC_UserRequested, "User left the game");
		}
	}

	pGameFramework->EndGameContext();

	IConsole *pConsole = gEnv->pConsole;

	if (pArgs->GetArgCount() > 1)
	{
		pConsole->GetCVar("cl_serveraddr")->Set(pArgs->GetArg(1));
	}

	if (pArgs->GetArgCount() > 2)
	{
		pConsole->GetCVar("cl_serverport")->Set(pArgs->GetArg(2));
	}

	// TODO: refactoring
	unsigned int contractId = gClient->nextContract();
	const char* endpoint = 0;
	if (gEnv->pScriptSystem->GetGlobalValue("SFWCL_ENDPOINT", endpoint)) {
		SGameStartParams params;
		params.flags = eGSF_Client | eGSF_NoDelayedStart | eGSF_NoQueries | eGSF_ImmersiveMultiplayer;
		params.hostname = pConsole->GetCVar("cl_serveraddr")->GetString();
		params.port = pConsole->GetCVar("cl_serverport")->GetIVal();
		std::string hostname = params.hostname;
		std::string ip = hostname;
		int port = params.port;
		auto colon = hostname.find(":");
		if (colon != std::string::npos) {
			ip = hostname.substr(0, colon);
			port = atoi(hostname.substr(colon + 1).c_str());
		}
		CryLogAlways("$6Checking server %s:%d", ip.c_str(), port);
		gClient->getHTTPClient()->GET(
			std::string(endpoint) + "/server?ip=" + ip + "&port=" + std::to_string(port) + "&json",
			[contractId, params](int code, const std::string& response, Error& err) -> void {
				if (gClient->getContract() != contractId) return;
				IGameFramework* pGameFramework = gClient->getGameFramework();
				if (err || code != 200) {
					CryLogAlways("$4Server check error: %s", err.what());
					pGameFramework->StartGameContext(&params);
					return;
				}
				try {
					auto body = nlohmann::json::parse(response);
					if (body.contains("error")) {
						CryLogAlways("$4Server check failed with: %s", body["error"].get<std::string>().c_str());
						pGameFramework->StartGameContext(&params);
						return;
					}
					std::string serverMap = body["map"].get<std::string>();
					std::string downloadLink, serverPak;
					bool mapMissing = false;

					if (body.contains("mapdl")) {
						downloadLink = body["mapdl"].get<std::string>();
					}
					if (body.contains("pak")) {
						serverPak = body["pak"].get<std::string>();
					}

					CryLogAlways("$3Server successfuly checked, map: %s", serverMap.c_str());

					auto downloadMapTask = [contractId](const std::string& url, std::function<void()> callback) -> void {
						if (contractId != gClient->getContract()) return;
						CryLogAlways("$4Failed to download map");
						callback();
					};

					auto downloadPakTask = [contractId](const std::string& url, std::function<void()> callback) -> void {
						if (contractId != gClient->getContract()) return;
						CryLogAlways("$6Downloading pak located at: %s", url.c_str());
						gClient->getHTTPClient()->Request("GET", url, {}, "", [contractId, callback, url](int code, const std::string& response, Error& err) -> void {
							if (contractId != gClient->getContract()) return;
							if (err || code >= 400) {
								CryLogAlways("$4Failed to download server pak at URL: %s", url.c_str());
								if (err) CryLogAlways("$4Error description: %s", err.what());
								else CryLogAlways("$4HTTP status code: %d", code);
								return;
							}
							// TODO:
							gClient->loadServerPak(response);
							callback();
						}, 4000, true, true);
					};

					auto runGameTask = [contractId, params]() -> void {
						if (contractId != gClient->getContract()) return;
						CryLogAlways("$3Joining server...");
						gClient->getGameFramework()->StartGameContext(&params);
					};

					// Welcome to hell without promises... :D

					if (serverPak.length() > 0) {
						downloadPakTask(serverPak, [contractId, mapMissing, downloadLink, downloadMapTask, runGameTask]() -> void {
							if (mapMissing && downloadLink.length() > 0) {
								downloadMapTask(downloadLink, [contractId, runGameTask]() -> void {
									runGameTask();
								});
							} else {
								runGameTask();
							}
						});
					} else {
						if (mapMissing && downloadLink.length() > 0) {
							downloadMapTask(downloadLink, [contractId, runGameTask]() -> void {
								runGameTask();
							});
						} else {
							runGameTask();
						}
					}
				} catch (std::exception & ex) {
					CryLogAlways("$4Received corrupted JSON: %s", response.c_str());
					pGameFramework->StartGameContext(&params);
				}
			}
		);
	} else {
		CryLogAlways("$4Lua client not initialized yet");
	}
}

void Client::OnDisconnectCmd(IConsoleCmdArgs *pArgs)
{
	IGameFramework *pGameFramework = gClient->getGameFramework();

	if (!gEnv->bServer)
	{
		INetChannel *pClientChannel = pGameFramework->GetClientChannel();

		if (pClientChannel)
		{
			pClientChannel->Disconnect(eDC_UserRequested, "User left the game");
		}
	}

	pGameFramework->EndGameContext();
}

Client::Client()
{
	RandomSeeder seeder;
	m_randomEngine.seed(seeder);
}

Client::~Client()
{
}

void Client::init(IGameFramework *pGameFramework)
{
	IConsole *pConsole = gEnv->pConsole;
	IScriptSystem *pScriptSystem = gEnv->pScriptSystem;
	IEntitySystem *pEntitySystem = gEnv->pEntitySystem;

	m_pGameFramework = pGameFramework;

	// initialize client components
	m_pExecutor          = std::make_unique<Executor>();
	m_pHTTPClient        = std::make_unique<HTTPClient>();
	m_pGSMasterHook      = std::make_unique<GSMasterHook>();
	m_pScriptCommands    = std::make_unique<ScriptCommands>();
	m_pScriptCallbacks   = std::make_unique<ScriptCallbacks>();
	m_pScriptBind_CPPAPI = std::make_unique<ScriptBind_CPPAPI>();

	// prepare Lua scripts
	m_scriptMain      = WinAPI::GetDataResource(nullptr, RESOURCE_SCRIPT_MAIN);
	m_scriptGameRules = WinAPI::GetDataResource(nullptr, RESOURCE_SCRIPT_GAME_RULES);
	m_scriptJSON      = WinAPI::GetDataResource(nullptr, RESOURCE_SCRIPT_JSON);
	m_scriptRPC       = WinAPI::GetDataResource(nullptr, RESOURCE_SCRIPT_RPC);

	// register engine listeners
	pGameFramework->RegisterListener(this, "crymp-client", FRAMEWORKLISTENERPRIORITY_DEFAULT);
	pGameFramework->GetILevelSystem()->AddListener(this);
	pEntitySystem->AddSink(this);

	// replace connect and disconnect console commands
	pConsole->RemoveCommand("connect");
	pConsole->RemoveCommand("disconnect");
	pConsole->AddCommand("connect", OnConnectCmd, VF_RESTRICTEDMODE, "Usage: connect [HOST] [PORT]");
	pConsole->AddCommand("disconnect", OnDisconnectCmd, VF_RESTRICTEDMODE, "Usage: disconnect");

	// execute Lua scripts
	pScriptSystem->ExecuteBuffer(m_scriptJSON.data(), m_scriptJSON.length(), "JSON.lua");
	pScriptSystem->ExecuteBuffer(m_scriptRPC.data(),  m_scriptRPC.length(),  "RPC.lua");
	pScriptSystem->ExecuteBuffer(m_scriptMain.data(), m_scriptMain.length(), "Main.lua");
}

bool Client::loadServerPak(const std::string& path)
{
	if (m_activePak.length() > 0)
	{
		unloadServerPak();
	}

	if (gEnv->pCryPak->OpenPack("game\\", path.c_str(), ICryPak::FLAGS_PATH_REAL))
	{
		CryLogAlways("$6[sfwcl] Loaded server pak at %s", path.c_str());
		m_activePak = path;
		return true;
	}
	else
	{
		CryLogAlways("$6[sfwcl] Loading server pak %s failed", path.c_str());
		m_activePak = "";
		return false;
	}
}

bool Client::unloadServerPak()
{
	if (m_activePak.length() == 0)
	{
		return true;
	}

	if (gEnv->pCryPak->ClosePack(m_activePak.c_str()))
	{
		CryLogAlways("$6[sfwcl] Unloaded server pak at %s", m_activePak.c_str());
		m_activePak.clear();
		return true;
	}
	else
	{
		CryLogAlways("$6[sfwcl] Unloading server pak %s failed", m_activePak.c_str());
		return false;
	}
}

void Client::OnPostUpdate(float deltaTime)
{
	m_pExecutor->onUpdate();
	m_pScriptCallbacks->onUpdate(deltaTime);
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
			unloadServerPak();
			nextContract();
			m_pScriptCallbacks->onDisconnect(event.m_value, event.m_description);
			break;
		}
		case eAE_channelCreated:
		case eAE_channelDestroyed:
		case eAE_connectFailed:
		case eAE_connected:
		case eAE_clientDisconnected:
		case eAE_resetBegin:
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
	gEnv->pScriptSystem->ExecuteBuffer(m_scriptGameRules.data(), m_scriptGameRules.length(), "GameRules.lua");
}

void Client::OnLoadingComplete(ILevel *pLevel)
{
}

void Client::OnLoadingError(ILevelInfo *pLevel, const char *error)
{
}

void Client::OnLoadingProgress(ILevelInfo *pLevel, int progressAmount)
{
}

bool Client::OnBeforeSpawn(SEntitySpawnParams & params)
{
	return true;
}

void Client::OnSpawn(IEntity *pEntity, SEntitySpawnParams & params)
{
	m_pScriptCallbacks->onSpawn(pEntity);
}

bool Client::OnRemove(IEntity *pEntity)
{
	return true;
}

void Client::OnEvent(IEntity *pEntity, SEntityEvent & event)
{
}
