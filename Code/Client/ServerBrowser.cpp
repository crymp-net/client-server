#include <stdio.h>
#include <stdint.h>
#include <string_view>

#include "CryCommon/CrySystem/ISystem.h"
#include "Library/External/nlohmann/json.hpp"
#include "Library/Format.h"

#include "ServerBrowser.h"
#include "Client.h"

using json = nlohmann::json;

namespace
{
	bool GetBool(const json & value)
	{
		return value.get<bool>();
	}

	int GetInt(const json & value)
	{
		return value.get<int>();
	}

	const std::string & GetString(const json & value)
	{
		return value.get_ref<const std::string&>();
	}

	const char *GetCString(const json & value)
	{
		return value.get_ref<const std::string&>().c_str();
	}

	std::string IPToString(uint32_t ip)
	{
		const uint8_t byte0 = ip;
		const uint8_t byte1 = ip >> 8;
		const uint8_t byte2 = ip >> 16;
		const uint8_t byte3 = ip >> 24;

		return Format("%hhu.%hhu.%hhu.%hhu", byte0, byte1, byte2, byte3);
	}

	uint32_t IPFromString(const std::string & ip)
	{
		uint8_t byte0 = 0;
		uint8_t byte1 = 0;
		uint8_t byte2 = 0;
		uint8_t byte3 = 0;

		sscanf(ip.c_str(), "%hhu.%hhu.%hhu.%hhu", &byte0, &byte1, &byte2, &byte3);

		return (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
	}

	const char *GameTypeFromMap(const std::string_view & map)
	{
		constexpr std::string_view PREFIX_IA = "multiplayer/ia/";
		constexpr std::string_view PREFIX_PS = "multiplayer/ps/";

		if (map.length() > PREFIX_IA.length() && std::string_view(map.data(), PREFIX_IA.length()) == PREFIX_IA)
			return "InstantAction";

		if (map.length() > PREFIX_PS.length() && std::string_view(map.data(), PREFIX_PS.length()) == PREFIX_PS)
			return "PowerStruggle";

		return "";
	}

	int GetTimeLeft(const json & server)
	{
		int minutes = 0;
		int seconds = 0;

		sscanf(GetCString(server["timel"]), "%d:%d", &minutes, &seconds);

		return (minutes * 60) + seconds;
	}
}

bool ServerBrowser::OnServerList(HTTPClient::Result & result)
{
	CryLog("[CryMP] Server list (%d): %s", result.code, result.response.c_str());

	try
	{
		const json data = json::parse(result.response);

		if (data.contains("error"))
		{
			CryLogAlways("$4[CryMP] Server list update error: %s", GetCString(data["error"]));
			return false;
		}

		for (const json & server : data)
		{
			const int serverID = m_servers.size();

			SBasicServerInfo info = {};

			info.m_hostName   =                GetCString(server["name"]);
			info.m_mapName    =                GetCString(server["mapnm"]);
			info.m_gameType   = GameTypeFromMap(GetString(server["map"]));
			info.m_numPlayers =                    GetInt(server["numpl"]);
			info.m_maxPlayers =                    GetInt(server["maxpl"]);
			info.m_publicIP   =    IPFromString(GetString(server["public_ip"]));
			info.m_privateIP  =    IPFromString(GetString(server["local_ip"]));
			info.m_publicPort =                    GetInt(server["public_port"]);
			info.m_hostPort   =                    GetInt(server["local_port"]);
			info.m_official   =                    GetInt(server["ranked"]) != 0;
			info.m_private    =                 GetString(server["pass"]) != "0";

			const std::string version = "1.1.1." + std::to_string(GetInt(server["ver"]));
			info.m_gameVersion = version.c_str();

			info.m_anticheat    = GetBool(server["anticheat"]);
			info.m_dedicated    = GetBool(server["dedicated"]);
			info.m_dx10         = GetBool(server["dx10"]);
			info.m_friendlyfire = GetBool(server["friendlyfire"]);
			info.m_gamepadsonly = GetBool(server["gamepadsonly"]);
			info.m_voicecomm    = GetBool(server["voicecomm"]);

			// not used
			info.m_country = "";

			// TODO: add optional SSM info?
			info.m_modName = "";
			info.m_modVersion = "";

			m_servers.emplace_back(Server{
				IPFromString(GetString(server["ip"])),
				static_cast<unsigned short>(GetInt(server["port"]))
			});

			m_pListener->NewServer(serverID, &info);

			// custom stuff
			m_pListener->UpdateValue(serverID, "connectable", GetInt(server["available"]) ? "1" : "0");
		}
	}
	catch (const json::exception & ex)
	{
		CryLogAlways("$4[CryMP] Server list parse error: %s", ex.what());
		return false;
	}

	return true;
}

bool ServerBrowser::OnServerInfo(HTTPClient::Result & result, int serverID)
{
	CryLog("[CryMP] Server info (%d): %s", result.code, result.response.c_str());

	try
	{
		const json data = json::parse(result.response);

		if (data.contains("error"))
		{
			CryLogAlways("$4[CryMP] Server update error: %s", GetCString(data["error"]));
			return false;
		}

		const int timeLeft = GetTimeLeft(data);
		const int numPlayers = GetInt(data["numpl"]);
		const int maxPlayers = GetInt(data["maxpl"]);
		const std::string version = "1.1.1." + std::to_string(GetInt(data["ver"]));

		m_pListener->UpdateValue(serverID, "hostname", GetCString(data["name"]));
		m_pListener->UpdateValue(serverID, "mapname", GetCString(data["mapnm"]));
		m_pListener->UpdateValue(serverID, "numplayers", std::to_string(numPlayers).c_str());
		m_pListener->UpdateValue(serverID, "maxplayers", std::to_string(maxPlayers).c_str());
		m_pListener->UpdateValue(serverID, "gametype", GameTypeFromMap(GetString(data["map"])));
		m_pListener->UpdateValue(serverID, "password", GetCString(data["pass"]));
		m_pListener->UpdateValue(serverID, "anticheat", GetBool(data["anticheat"]) ? "1" : "0");
		m_pListener->UpdateValue(serverID, "friendlyfire", GetBool(data["friendlyfire"]) ? "1" : "0");
		m_pListener->UpdateValue(serverID, "gamepadsonly", GetBool(data["gamepadsonly"]) ? "1" : "0");
		m_pListener->UpdateValue(serverID, "voicecomm", GetBool(data["voicecomm"]) ? "1" : "0");
		m_pListener->UpdateValue(serverID, "dedicated", GetBool(data["dedicated"]) ? "1" : "0");
		m_pListener->UpdateValue(serverID, "official", GetInt(data["ranked"]) ? "1" : "0");
		m_pListener->UpdateValue(serverID, "gamever", version.c_str());
		m_pListener->UpdateValue(serverID, "timelimit", timeLeft ? "1" : "0");
		m_pListener->UpdateValue(serverID, "timeleft", std::to_string(timeLeft).c_str());
		m_pListener->UpdateValue(serverID, "dx10", GetBool(data["dx10"]) ? "1" : "0");

		// custom stuff
		m_pListener->UpdateValue(serverID, "connectable", GetInt(data["available"]) ? "1" : "0");

		int playerID = 0;

		for (const json & player : data["players"])
		{
			m_pListener->UpdatePlayerValue(serverID, playerID, "player", GetCString(player["name"]));

			const int team = GetInt(player["team"]);
			const int rank = GetInt(player["rank"]);
			const int kills = GetInt(player["kills"]);
			const int deaths = GetInt(player["deaths"]);

			m_pListener->UpdatePlayerValue(serverID, playerID, "team", std::to_string(team).c_str());
			m_pListener->UpdatePlayerValue(serverID, playerID, "rank", std::to_string(rank).c_str());
			m_pListener->UpdatePlayerValue(serverID, playerID, "kills", std::to_string(kills).c_str());
			m_pListener->UpdatePlayerValue(serverID, playerID, "deaths", std::to_string(deaths).c_str());

			playerID++;
		}
	}
	catch (const json::exception & ex)
	{
		CryLogAlways("$4[CryMP] Server info parse error: %s", ex.what());
		return false;
	}

	return true;
}

ServerBrowser::ServerBrowser()
{
}

ServerBrowser::~ServerBrowser()
{
}

bool ServerBrowser::IsAvailable() const
{
	return true;
}

void ServerBrowser::Start(bool browseLAN)
{
}

void ServerBrowser::SetListener(IServerListener *pListener)
{
	m_pListener = pListener;
}

void ServerBrowser::Stop()
{
	m_servers.clear();
}

void ServerBrowser::Update()
{
	m_servers.clear();

	const std::string api = gClient->GetMasterServerAPI();
	if (api.empty())
		return;

	const std::string url = api + "/servers?all&detailed&json";

	gClient->GetHTTPClient()->GET(url, [this](HTTPClient::Result & result)
	{
		if (m_pListener)
		{
			if (result.error)
			{
				CryLogAlways("$4[CryMP] Server list update failed: %s", result.error.what());
			}
			else
			{
				OnServerList(result);
			}

			m_pListener->UpdateComplete(false);
		}
	});
}

void ServerBrowser::UpdateServerInfo(int id)
{
	Server *pServer = GetServer(id);
	if (!pServer)
		return;

	const std::string api = gClient->GetMasterServerAPI();
	if (api.empty())
		return;

	const std::string ip = IPToString(pServer->ip);
	const std::string port = std::to_string(pServer->port);

	const std::string url = api + "/server?ip=" + ip + "&port=" + port + "&json";

	gClient->GetHTTPClient()->GET(url, [id, this](HTTPClient::Result & result)
	{
		if (m_pListener)
		{
			if (result.error)
			{
				CryLogAlways("$4[CryMP] Server update failed: %s", result.error.what());
			}
			else
			{
				if (OnServerInfo(result, id))
					m_pListener->ServerUpdateComplete(id);
				else
					m_pListener->ServerUpdateFailed(id);
			}
		}
	});
}

void ServerBrowser::BrowseForServer(unsigned int ip, unsigned short port)
{
}

void ServerBrowser::BrowseForServer(const char *address, unsigned short port)
{
}

void ServerBrowser::SendNatCookie(unsigned int ip, unsigned short port, int cookie)
{
}

void ServerBrowser::CheckDirectConnect(int id, unsigned short port)
{
	if (!m_pListener)
		return;

	Server *pServer = GetServer(id);
	if (!pServer)
		return;

	m_pListener->ServerDirectConnect(false, pServer->ip, pServer->port);
}

int ServerBrowser::GetServerCount()
{
	return m_servers.size();
}

int ServerBrowser::GetPendingQueryCount()
{
	return 0;
}
