#include <stdlib.h>  // atoi

#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CryAction/IGameFramework.h"
#include "Library/External/nlohmann/json.hpp"
#include "Library/StringBuffer.h"

#include "ServerConnector.h"
#include "Client.h"

using json = nlohmann::json;

void ServerConnector::RequestServerInfo()
{
	CryLogAlways("$3[CryMP] Checking server at $6%s:%u$3", m_host.c_str(), m_port);

	// get the master server API URL from Lua
	const char *endpoint = nullptr;
	if (gEnv->pScriptSystem->GetGlobalValue("SFWCL_ENDPOINT", endpoint))
	{
		StringBuffer<128> url;
		url += endpoint;
		url += "/server?ip=";
		url += m_host;
		url += "&port=";
		url.addUInt(m_port);
		url += "&json";

		const unsigned int contractID = m_contractID;

		gClient->GetHTTPClient()->GET(url, [contractID, this](HTTPClient::Result & result)
		{
			if (contractID == m_contractID)
			{
				// next step
				OnServerInfo(result);
			}
		});
	}
	else
	{
		CryLogAlways("$4[CryMP] Failed to get the master server API URL!");

		TryConnect();
	}
}

void ServerConnector::OnServerInfo(HTTPClient::Result & result)
{
	if (result.error)
	{
		CryLogAlways("$4[CryMP] Server check failed: %s", result.error.what());
		TryConnect();
		return;
	}

	CryLog("[CryMP] Server info (%d): %s", result.code, result.response.c_str());

	try
	{
		const json info = json::parse(result.response);

		if (info.contains("error"))
		{
			CryLogAlways("$4[CryMP] Server check error: %s", info["error"].get<std::string>().c_str());
			TryConnect();
			return;
		}

		m_serverName = info["name"].get<std::string>();
		m_serverMapName = info["map"].get<std::string>();

		if (info.contains("mapdl"))
			m_serverMapLink = info["mapdl"].get<std::string>();

		if (info.contains("pak"))
			m_serverPAKLink = info["pak"].get<std::string>();
	}
	catch (const json::exception & ex)
	{
		CryLogAlways("$4[CryMP] Server check info parse error: %s", ex.what());
		TryConnect();
		return;
	}

	CryLogAlways("$3[CryMP] Server check done, map: $6%s$3", m_serverMapName.c_str());

	// next step
	DownloadMap();
}

void ServerConnector::DownloadMap()
{
	// TODO

	// next step
	DownloadPAK();
}

void ServerConnector::DownloadPAK()
{
	// TODO

	// next step
	TryConnect();
}

void ServerConnector::TryConnect()
{
	if (m_serverName.empty())
		CryLogAlways("$3[CryMP] Joining unknown server at $6%s:%u$3", m_host.c_str(), m_port);
	else
		CryLogAlways("$3[CryMP] Joining $6%s$3 at $6%s:%u$3", m_serverName.c_str(), m_host.c_str(), m_port);

	SGameStartParams params;
	params.flags = eGSF_Client | eGSF_NoDelayedStart | eGSF_NoQueries | eGSF_ImmersiveMultiplayer;
	params.hostname = m_host.c_str();
	params.port = m_port;

	gClient->GetGameFramework()->StartGameContext(&params);
}

ServerConnector::ServerConnector()
{
}

ServerConnector::~ServerConnector()
{
}

void ServerConnector::Connect(const std::string_view & host, unsigned int port)
{
	// a new contract
	m_contractID++;

	m_host = host;
	m_port = port;

	// IP:PORT host workaround
	const size_t colonPos = m_host.find(':');
	if (colonPos != std::string::npos)
	{
		m_port = atoi(m_host.c_str() + colonPos + 1);
		m_host.resize(colonPos);
	}

	// the first step
	RequestServerInfo();
}
