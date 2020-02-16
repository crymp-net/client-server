/**
 * @file
 * @brief Implementation of CryMP Client.
 */

#include "CryCommon/ISystem.h"
#include "CryCommon/IConsole.h"
#include "CryCommon/I3DEngine.h"  // required by IGameRulesSystem.h
#include "CryAction/IGameRulesSystem.h"
#include "Library/StringBuffer.h"

#include "Client.h"
#include "Log.h"

Client *Client::s_pInstance;

static void SendChatMessageToSelf(const char *message)
{
	IGameFramework *pGameFramework = gEnv->pGame->GetIGameFramework();

	IGameRules *pGameRules = pGameFramework->GetIGameRulesSystem()->GetCurrentGameRules();
	EntityId localActorID = pGameFramework->GetClientActorId();
	if (!pGameRules || !localActorID)
	{
		LogError("Unable to send chat message!");
		return;
	}

	pGameRules->SendChatMessage(eChatToTarget, localActorID, localActorID, message);
}

static void SendValidate()
{
	Profile & profile = Client::GetProfile();

	if (profile.isLoggedIn())
	{
		StringBuffer<1024> msg;

		msg.append_f("!validate %d ", profile.getID());

		msg += profile.getToken();
		msg += ' ';
		msg += profile.getName();

		SendChatMessageToSelf(msg.get());
	}
}

static void CmdSay(IConsoleCmdArgs *args)
{
	if (args->GetArgCount() <= 1)
	{
		LogInfo("$9Usage: say <text>");
		return;
	}

	const char *text = args->GetCommandLine() + 4;  // skip command name and space after it

	SendChatMessageToSelf(text);
}

static void CmdLogin(IConsoleCmdArgs *args)
{
	if (args->GetArgCount() <= 2)
	{
		LogInfo("$9Usage: login <name or e-mail> <password>");
		return;
	}

	const char *name = args->GetArg(1);
	const char *pass = args->GetArg(2);

	Client::GetProfile().login(name, pass);

	LogWarning("The \'login\' command is not secure. Consider using \'secu_login\' command instead.");
}

static void CmdSecuLogin(IConsoleCmdArgs *args)
{
	if (args->GetArgCount() <= 2)
	{
		LogInfo("$9Usage: secu_login <name> <key>");
		return;
	}

	const char *name = args->GetArg(1);
	const char *key  = args->GetArg(2);

	Client::GetProfile().login(name, key);
}

static void CmdAuthLogin(IConsoleCmdArgs *args)
{
	if (!Client::GetProfile().isLoggedIn())
	{
		LogError("No profile is logged-in!");
		return;
	}

	SendValidate();
}

static void CmdLogout(IConsoleCmdArgs *args)
{
	Profile & profile = Client::GetProfile();

	if (!profile.isLoggedIn())
	{
		LogError("No profile is logged-in!");
		return;
	}

	profile.logout();
}

static void CmdShowProfile(IConsoleCmdArgs *args)
{
	Profile & profile = Client::GetProfile();

	if (!profile.isLoggedIn())
	{
		LogError("No profile is logged-in!");
		return;
	}

	LogInfo("$8Profile Name: %s", profile.getName().c_str());
	LogInfo("$8Profile ID: %d", profile.getID());

	LogDebug("$8Profile Token: %s", profile.getToken().c_str());
}

bool Client::init()
{
	if (!m_GSMaster.init())
	{
		return false;
	}

	IConsole *pConsole = gEnv->pConsole;

	pConsole->AddCommand("say", CmdSay, VF_RESTRICTEDMODE, "Say something.");
	pConsole->AddCommand("login", CmdLogin, VF_RESTRICTEDMODE, "Log-in to CryMP profile.");
	pConsole->AddCommand("secu_login", CmdSecuLogin, VF_RESTRICTEDMODE, "Log-in to CryMP profile.");
	pConsole->AddCommand("auth_login", CmdAuthLogin, VF_RESTRICTEDMODE, "Authenticate CryMP profile on server.");
	pConsole->AddCommand("logout", CmdLogout, VF_RESTRICTEDMODE, "Sign out current CryMP profile.");
	pConsole->AddCommand("show_profile", CmdShowProfile, VF_RESTRICTEDMODE, "Show information about active CryMP profile.");

	m_pClAutoValidateCVar = pConsole->RegisterInt("cl_autoValidate", 1, VF_RESTRICTEDMODE | VF_NOT_NET_SYNCED,
	                                              "Enables automatic CryMP profile authentication.");

	IGameFramework *pGameFramework = gEnv->pGame->GetIGameFramework();

	pGameFramework->RegisterListener(this, "Client", FRAMEWORKLISTENERPRIORITY_DEFAULT);
	pGameFramework->GetILevelSystem()->AddListener(this);

	return true;
}

void Client::OnPostUpdate(float deltaTime)
{
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
		case eAE_resetEnd:
		case eAE_inGame:
		{
			if (gEnv->bMultiplayer && m_pClAutoValidateCVar->GetIVal())
			{
				SendValidate();
			}

			break;
		}
		case eAE_channelCreated:
		case eAE_channelDestroyed:
		case eAE_connectFailed:
		case eAE_connected:
		case eAE_disconnected:
		case eAE_clientDisconnected:
		case eAE_resetBegin:
		case eAE_resetProgress:
		case eAE_preSaveGame:
		case eAE_postSaveGame:
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
	if (gEnv->bMultiplayer && m_pClAutoValidateCVar->GetIVal())
	{
		m_profile.refreshToken();
	}
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
