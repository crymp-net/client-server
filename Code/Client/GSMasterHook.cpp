#include <winsock2.h>

#include "CryCommon/ISystem.h"
#include "CryCommon/IConsole.h"
#include "Launcher/Launcher.h"
#include "Library/WinAPI.h"

#include "GSMasterHook.h"

#define GS_MASTER_HOOK_HOSTNAME_CVAR_NAME "cl_master"
#define GS_MASTER_HOOK_DEFAULT_HOSTNAME "m.crymp.net"

namespace
{
	void* __stdcall CryNetwork_gethostbyname_hook(const char *name)
	{
		// replace gamespy.com suffix with custom hostname
		if (const char *suffix = strstr(name, "gamespy.com"))
		{
			const size_t prefixLength = suffix - name;

			ICVar *pHostnameCVar = gEnv->pConsole->GetCVar(GS_MASTER_HOOK_HOSTNAME_CVAR_NAME);

			std::string buffer;
			buffer.append(name, prefixLength);
			buffer.append(pHostnameCVar->GetString());

			CryLog("GSMasterHook: %s => %s", name, buffer.c_str());

			return gethostbyname(buffer.c_str());
		}
		else
		{
			return gethostbyname(name);
		}
	}
}

GSMasterHook::GSMasterHook()
{
	m_pHostnameCVar = gEnv->pConsole->RegisterString(GS_MASTER_HOOK_HOSTNAME_CVAR_NAME,
	                                                 GS_MASTER_HOOK_DEFAULT_HOSTNAME,
	                                                 VF_NOT_NET_SYNCED,
	                                                 "GameSpy master server hostname.");

	void *pCryNetwork = gLauncher->getCryNetwork().getHandle();

	WinAPI::HookIATByAddress(pCryNetwork, gethostbyname, CryNetwork_gethostbyname_hook);
}

GSMasterHook::~GSMasterHook()
{
}
