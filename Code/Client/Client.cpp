#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/CryNetwork/INetwork.h"
#include "CryCommon/CryScriptSystem/IScriptSystem.h"
#include "Launcher/Resources.h"
#include "Library/RandomSeeder.h"
#include "Library/WinAPI.h"

#include "Client.h"
#include "Executor.h"
#include "HTTPClient.h"
#include "GSMasterHook.h"
#include "ScriptCommands.h"
#include "ScriptCallbacks.h"
#include "ScriptBind_CPPAPI.h"
#include "ServerConnector.h"
#include "ServerPAK.h"

void Client::OnConnectCmd(IConsoleCmdArgs *pArgs)
{
	IGameFramework *pGameFramework = gClient->GetGameFramework();

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

	gClient->GetServerConnector()->Connect(
		pConsole->GetCVar("cl_serveraddr")->GetString(),
		pConsole->GetCVar("cl_serverport")->GetIVal()
	);
}

void Client::OnDisconnectCmd(IConsoleCmdArgs *pArgs)
{
	IGameFramework *pGameFramework = gClient->GetGameFramework();

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

void Client::Init(IGameFramework *pGameFramework)
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
	m_pServerConnector   = std::make_unique<ServerConnector>();
	m_pServerPAK         = std::make_unique<ServerPAK>();

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

void Client::OnPostUpdate(float deltaTime)
{
	m_pExecutor->OnUpdate();
	m_pScriptCallbacks->OnUpdate(deltaTime);
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
			m_pScriptCallbacks->OnDisconnect(event.m_value, event.m_description);
			m_pServerPAK->Unload();
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
	m_pScriptCallbacks->OnSpawn(pEntity);
}

bool Client::OnRemove(IEntity *pEntity)
{
	return true;
}

void Client::OnEvent(IEntity *pEntity, SEntityEvent & event)
{
}
