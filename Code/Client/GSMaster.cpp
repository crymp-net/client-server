/**
 * @file
 * @brief Implementation of GameSpy master server hostname hook.
 */

#include "CryCommon/ISystem.h"
#include "CryCommon/IConsole.h"
#include "Launcher/Launcher.h"
#include "Library/StringBuffer.h"
#include "Library/Hook/IAT.h"

#include "GSMaster.h"
#include "Client.h"

#define GS_MASTER_DEFAULT_HOSTNAME "m.crymp.net"

void* __stdcall GSMaster::CryNetwork_gethostbyname_hook(const char *name)
{
	GSMaster & self = Client::GetGSMaster();

	// replace gamespy.com suffix with custom hostname
	// TODO: maybe edited CryNetwork should be supported as well
	if (const char *gamespy = std::strstr(name, "gamespy.com"))
	{
		const size_t prefixLength = gamespy - name;

		StringBuffer<1024> buffer;
		buffer.append(name, prefixLength);
		buffer.append(self.getHostname());

		CryLog("GSMaster::CryNetwork_gethostbyname_hook: %s => %s", name, buffer.get());

		return self.m_pGetHostByName(buffer.get());
	}

	CryLog("GSMaster::CryNetwork_gethostbyname_hook: %s", name);

	return self.m_pGetHostByName(name);
}

void GSMaster::OnHostnameCVarChanged(ICVar *pCVar)
{
	GSMaster & self = Client::GetGSMaster();

	self.m_hostname = pCVar->GetString();
}

bool GSMaster::init()
{
	m_hostname = GS_MASTER_DEFAULT_HOSTNAME;

	m_pHostnameCVar = gEnv->pConsole->RegisterString("cl_master", GS_MASTER_DEFAULT_HOSTNAME, VF_NOT_NET_SYNCED,
	                                                 "GameSpy master server hostname.", GSMaster::OnHostnameCVarChanged);

	m_libGetHostByName.load("ws2_32.dll", DLL::NO_LOAD);

	m_pGetHostByName = m_libGetHostByName.getSymbol<TGetHostByName>("gethostbyname");

	void *pCryNetwork = Launcher::GetCryNetworkDLL().getHandle();

	if (!IATHookByAddress(pCryNetwork, m_pGetHostByName, GSMaster::CryNetwork_gethostbyname_hook))
	{
		return false;
	}

	return true;
}
