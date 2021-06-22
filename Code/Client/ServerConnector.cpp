#include <stdlib.h>  // atoi

#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CryAction/IGameFramework.h"
#include "CryCommon/CryScriptSystem/IScriptSystem.h"
#include "Library/External/nlohmann/json.hpp"

#include "ServerPAK.h"
#include "MapDownloader.h"
#include "ServerConnector.h"
#include "Client.h"
#include "Executor.h"

#include "CryCommon/CryAction/ILevelSystem.h"

// TODO: needs refactor maybe? :D
#ifdef WIN32
#define StrICmp _stricmp
#else
#define StrICmp strcasecmp
#endif

using json = nlohmann::json;

namespace
{
	std::string GetMasterServerAPI()
	{
		const char *endpoint = "";
		gEnv->pScriptSystem->GetGlobalValue("SFWCL_ENDPOINT", endpoint);

		return endpoint;
	}
}

void ServerConnector::RequestServerInfo()
{
	const unsigned int contractID = m_contractID;
	CryLogAlways("$3[CryMP] Checking server at $6%s:%u$3", m_host.c_str(), m_port);

	const std::string api = GetMasterServerAPI();
	if (api.empty())
	{
		CryLogAlways("$4[CryMP] Failed to get the master server API!");
		TryConnect(contractID);
		return;
	}

	const std::string url = api + "/server?ip=" + m_host + "&port=" + std::to_string(m_port) + "&json";

	gClient->GetHTTPClient()->GET(url, [contractID, this](HTTPClient::Result & result)
	{
		if (contractID == m_contractID)
		{
			// next step
			OnServerInfo(contractID, result);
		}
	});
}

void ServerConnector::OnServerInfo(int contractID, HTTPClient::Result & result)
{
	if (result.error)
	{
		CryLogAlways("$4[CryMP] Server check failed: %s", result.error.what());
		TryConnect(contractID);
		return;
	}

	CryLog("[CryMP] Server info (%d): %s", result.code, result.response.c_str());

	try
	{
		const json info = json::parse(result.response);

		if (info.contains("error"))
		{
			CryLogAlways("$4[CryMP] Server check error: %s", info["error"].get<std::string>().c_str());
			TryConnect(contractID);
			return;
		}

		m_serverName = info["name"].get<std::string>();
		m_serverMapName = info["map"].get<std::string>();

		if (info.contains("mapdl"))
			m_serverMapLink = info["mapdl"].get<std::string>();
		else m_serverMapLink = "";

		if (info.contains("pak"))
			m_serverPAKLink = info["pak"].get<std::string>();
		else m_serverPAKLink = "";

		// For testing purposes
		// m_serverPAKLink = "https://crymp.net/client/ServerPak.pak";
	}
	catch (const json::exception & ex)
	{
		CryLogAlways("$4[CryMP] Server check info parse error: %s", ex.what());
		TryConnect(contractID);
		return;
	}

	CryLogAlways("$3[CryMP] Server check done, map: $6%s$3", m_serverMapName.c_str());

	// next step
	DownloadMap(contractID);
}

void ServerConnector::DownloadMap(int contractID)
{
	if (contractID != m_contractID) return;
	if (m_serverMapLink.length() == 0) {
		CryLogAlways("$3[CryMP] This server doesn't use custom map");
		DownloadPAK(contractID);
		return;
	}
	if (m_serverMapLink.find("http") != 0) {
		m_serverMapLink = "https://" + m_serverMapLink;
	}
	bool mapMissing = true;
	
	ILevelSystem* pLevelSystem = gClient->GetGameFramework()->GetILevelSystem();
	pLevelSystem->Rescan();
	for (int l = 0; l < pLevelSystem->GetLevelCount(); ++l) {
		ILevelInfo* pLevelInfo = pLevelSystem->GetLevelInfo(l);
		if (!pLevelInfo) continue;
		if (!StrICmp(pLevelInfo->GetName(), m_serverMapName.c_str())) {
			mapMissing = false;
			break;
		}
	}

	if (!mapMissing) {
		CryLogAlways("$3[CryMP] This server uses custom map, but map was already found");
		DownloadPAK(contractID);
		return;
	}

	CryLogAlways("$3[CryMP] This server uses custom map located at: %s", m_serverMapLink.c_str());
	gClient->GetExecutor()->RunAsync([contractID, this]() {
		if (contractID != m_contractID) return;
		auto [result, message] = m_mapDownloder.execute(m_serverMapLink, [](unsigned int progress, unsigned int progressMax, const std::string& message) {
			// ...
			CryLogAlways("$5[CryMP] Map download progress: %d / %d, %s", progress, progressMax, message.c_str());
		});
		if (!SUCCEEDED(result)) {
			CryLogAlways("$4[CryMP] Map download failed with error code: %x, message: %s", result, message.c_str());
			DownloadPAK(contractID);
		} else {
			CryLogAlways("$3[CryMP] Map download succeeded with message: %s", message.c_str());
			DownloadPAK(contractID);
		}
	});
}

void ServerConnector::DownloadPAK(int contractID)
{
	if (contractID != m_contractID) return;
	if (m_serverPAKLink.length() == 0) {
		CryLogAlways("$3[CryMP] This server doesn't use server pak feature");
		gClient->GetExecutor()->RunOnMainThread([this, contractID]() {
			TryConnect(contractID);
		});
		return;
	}
	CryLogAlways("$3[CryMP] This server uses server pak located at: $6%s", m_serverPAKLink.c_str());
	gClient->GetHTTPClient()->Request("GET", m_serverPAKLink, "", {}, [contractID, this](HTTPClient::Result& result) -> void {
		if (m_contractID != contractID) return;

		if (result.error || result.code >= 400) {
			CryLogAlways("$4[CryMP] Failed to download server pak at: %s", m_serverPAKLink.c_str());
			if (result.error) CryLogAlways("$4[CryMP] Server pak download error description: %s", result.error.what());
			else CryLogAlways("$4[CryMP] Server pak download status code: %d", result.code);
			return;
		}

		gClient->GetServerPAK()->Load(result.response);
		gClient->GetExecutor()->RunOnMainThread([this, contractID]() {
			TryConnect(contractID);
		});
	}, 4000, true, true);
}

void ServerConnector::TryConnect(int contractID)
{
	if (contractID != m_contractID) return;
	ILevelSystem* pLevelSystem = gClient->GetGameFramework()->GetILevelSystem();
	pLevelSystem->Rescan();
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

void ServerConnector::Disconnect() {
	++m_contractID;
	m_mapDownloder.cancel();
}
