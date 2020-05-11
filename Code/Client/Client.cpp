/**
 * @file
 * @brief Implementation of CryMP Client.
 */

#include "CryCommon/ISystem.h"
#include "CryCommon/IConsole.h"
#include "CryCommon/IHardwareMouse.h"
#include "CryCommon/I3DEngine.h"  // required by IGameRulesSystem.h
#include "CryAction/IGameRulesSystem.h"
#include "Library/StringBuffer.h"
#include "Launcher/Launcher.h"

#include "Client.h"
#include "DisplayInfo.h"

Client *Client::s_pInstance;

static void SendChatMessageToSelf(const char *message)
{
	IGameFramework *pGameFramework = gEnv->pGame->GetIGameFramework();

	IGameRules *pGameRules = pGameFramework->GetIGameRulesSystem()->GetCurrentGameRules();
	EntityId localActorID = pGameFramework->GetClientActorId();
	if (!pGameRules || !localActorID)
	{
		CryLogErrorAlways("Unable to send chat message!");
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

		msg += "!validate ";
		msg += std::to_string(profile.getID());
		msg += ' ';
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
		CryLogAlways("$9Usage: say <text>");
		return;
	}

	const char *text = args->GetCommandLine() + 4;  // skip command name and space after it

	SendChatMessageToSelf(text);
}

static void CmdLogin(IConsoleCmdArgs *args)
{
	if (args->GetArgCount() <= 2)
	{
		CryLogAlways("$9Usage: login <name or e-mail> <password>");
		return;
	}

	const char *name = args->GetArg(1);
	const char *pass = args->GetArg(2);

	Client::GetProfile(EProfile::USER).login(name, pass);

	CryLogWarningAlways("The \'login\' command is not secure. Consider using \'secu_login\' command instead.");
}

static void CmdSecuLogin(IConsoleCmdArgs *args)
{
	if (args->GetArgCount() <= 2)
	{
		CryLogAlways("$9Usage: secu_login <name> <key>");
		return;
	}

	const char *name = args->GetArg(1);
	const char *key  = args->GetArg(2);

	Client::GetProfile(EProfile::USER).login(name, key);
}

static void CmdAuthLogin(IConsoleCmdArgs *args)
{
	SendValidate();
}

static void CmdLogout(IConsoleCmdArgs *args)
{
	Profile & profile = Client::GetProfile(EProfile::USER);

	if (!profile.isLoggedIn())
	{
		CryLogErrorAlways("No profile is logged-in!");
		return;
	}

	profile.logout();
}

static void CmdShowProfile(IConsoleCmdArgs *args)
{
	Profile & profile = Client::GetProfile(EProfile::USER);

	if (!profile.isLoggedIn())
	{
		CryLogErrorAlways("No profile is logged-in!");
		return;
	}

	CryLogAlways("$8Profile Name: %s", profile.getName().c_str());
	CryLogAlways("$8Profile ID: %d", profile.getID());

	CryLog("$8Profile Token: %s", profile.getToken().c_str());
}

static void CmdShowTelemetry(IConsoleCmdArgs *args)
{
	Telemetry & telemetry = Client::GetTelemetry();

	CryLogAlways("$8HWID: %s", telemetry.getHWID().c_str());
	CryLogAlways("$8Locale: %s", telemetry.getLocale().c_str());
	CryLogAlways("$8Time zone: %+d", -telemetry.getTimeZoneBias());
}

static void CmdCursorIncrement(IConsoleCmdArgs *args)
{
	gEnv->pHardwareMouse->IncrementCounter();
}

static void CmdCursorDecrement(IConsoleCmdArgs *args)
{
	gEnv->pHardwareMouse->DecrementCounter();
}

struct CVarDumpListener : public ICVarDumpSink
{
	using Buffer = StringBuffer<256>;

	int filterFlags = -1;  // cvars without these flags are not listed at all
	int skippedFlags = 0;  // cvars with these flags are not listed at all
	int ignoredFlags = 0;  // these flags are not listed in the dump

	void addFlag(Buffer & buffer, bool & isNextFlag, int flags, int flag, const char *name)
	{
		if (!(flag & flags) || flag & ignoredFlags)
		{
			return;
		}

		if (isNextFlag)
		{
			buffer += ", ";
		}
		else
		{
			isNextFlag = true;
		}

		buffer += name;
	}

	void OnElementFound(ICVar *pCVar) override
	{
		const int flags = pCVar->GetFlags();

		if (!(filterFlags & flags) || skippedFlags & flags)
		{
			return;
		}

		Buffer buffer;

		buffer += "$3";  // green
		buffer += pCVar->GetName();

		switch (pCVar->GetType())
		{
			case CVAR_INT:
			{
				buffer += " = $6";  // yellow
				buffer += std::to_string(pCVar->GetIVal());
				break;
			}
			case CVAR_FLOAT:
			{
				buffer += " = $6";  // yellow
				buffer += std::to_string(pCVar->GetFVal());
				break;
			}
			case CVAR_STRING:
			{
				buffer += " = $6\"";  // yellow
				buffer += pCVar->GetString();
				buffer += "\"";
				break;
			}
		}

		buffer += " $5[";  // cyan

		bool isNextFlag = false;
		addFlag(buffer, isNextFlag, flags, VF_CHEAT, "CHEAT");
		addFlag(buffer, isNextFlag, flags, VF_NOT_NET_SYNCED, "NOT_NET_SYNCED");
		addFlag(buffer, isNextFlag, flags, VF_READONLY, "READONLY");
		addFlag(buffer, isNextFlag, flags, VF_BITFIELD, "BITFIELD");
		addFlag(buffer, isNextFlag, flags, VF_RESTRICTEDMODE, "RESTRICTEDMODE");

		buffer += "]";

		CryLogAlways("%s", buffer.get());
	}
};

static void CmdDumpClientCVars(IConsoleCmdArgs *args)
{
	CVarDumpListener listener;
	listener.filterFlags  = VF_NOT_NET_SYNCED;
	listener.ignoredFlags = VF_NOT_NET_SYNCED;
	gEnv->pConsole->DumpCVars(&listener);
}

bool Client::init()
{
	if (!m_GSMaster.init() || !m_telemetry.init() || !DisplayInfo::Init())
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
	pConsole->AddCommand("show_telemetry", CmdShowTelemetry, VF_RESTRICTEDMODE, "Show all telemetry information.");
	pConsole->AddCommand("cursor_increment", CmdCursorIncrement, VF_RESTRICTEDMODE);
	pConsole->AddCommand("cursor_decrement", CmdCursorDecrement, VF_RESTRICTEDMODE);
	pConsole->AddCommand("dump_client_cvars", CmdDumpClientCVars, VF_RESTRICTEDMODE, "Show all client-side cvars.");

	IGameFramework *pGameFramework = Launcher::GetIGameFramework();  // gEnv->pGame is not initialized yet

	pGameFramework->RegisterListener(this, "Client", FRAMEWORKLISTENERPRIORITY_DEFAULT);
	pGameFramework->GetILevelSystem()->AddListener(this);

	m_staticProfile.initStatic();

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
			if (gEnv->bMultiplayer)
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
	if (gEnv->bMultiplayer)
	{
		Client::GetProfile().refreshToken();
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
