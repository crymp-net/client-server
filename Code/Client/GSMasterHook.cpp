#include <winsock2.h>

#include <array>
#include <algorithm>
#include <string_view>

#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "Launcher/Launcher.h"
#include "Library/StringBuffer.h"
#include "Library/WinAPI.h"

#include "GSMasterHook.h"

#define GS_MASTER_HOOK_HOSTNAME_CVAR_NAME "cl_master"
#define GS_MASTER_HOOK_DEFAULT_HOSTNAME "m.crymp.net"

namespace
{
	constexpr std::array<std::string_view, 3> PREFIXES = {
		"crysis.available.",
		"crysis.master.",
		"crysis.ms5."
	};

	std::string_view FindPrefix(const std::string_view & name)
	{
		auto it = std::find_if(PREFIXES.begin(), PREFIXES.end(), [name](const std::string_view & prefix)
		{
			if (prefix.length() < name.length())
				return prefix == std::string_view(name.data(), prefix.length());
			else
				return false;
		});

		return (it != PREFIXES.end()) ? *it : std::string_view();
	}

	void* __stdcall CryNetwork_gethostbyname_hook(const char *name)
	{
		const std::string_view prefix = FindPrefix(name);

		if (!prefix.empty())
		{
			ICVar *pHostnameCVar = gEnv->pConsole->GetCVar(GS_MASTER_HOOK_HOSTNAME_CVAR_NAME);
			if (pHostnameCVar)
			{
				StringBuffer<64> buffer;
				buffer += prefix;
				buffer += pHostnameCVar->GetString();

				CryLog("GSMasterHook: %s => %s", name, buffer.c_str());

				return gethostbyname(buffer.c_str());
			}
			else
			{
				CryLogError("GSMasterHook: " GS_MASTER_HOOK_HOSTNAME_CVAR_NAME " cvar is missing!");
			}
		}

		CryLog("GSMasterHook: %s", name);

		return gethostbyname(name);
	}
}

GSMasterHook::GSMasterHook()
{
	m_pHostnameCVar = gEnv->pConsole->RegisterString(GS_MASTER_HOOK_HOSTNAME_CVAR_NAME,
	                                                 GS_MASTER_HOOK_DEFAULT_HOSTNAME,
	                                                 VF_NOT_NET_SYNCED,
	                                                 "GameSpy master server hostname.");

	void *pCryNetwork = gLauncher->GetCryNetwork().GetHandle();

	WinAPI::HookIATByAddress(pCryNetwork, gethostbyname, CryNetwork_gethostbyname_hook);
}

GSMasterHook::~GSMasterHook()
{
}
