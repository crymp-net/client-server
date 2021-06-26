#pragma once

#include <string>
#include <vector>

#include "CryCommon/CryNetwork/INetworkService.h"

#include "HTTPClient.h"

class ServerBrowser : public IServerBrowser
{
	struct Server
	{
		unsigned int ip = 0;
		unsigned short port = 0;
	};

	std::vector<Server> m_servers;
	IServerListener *m_pListener = nullptr;

	Server *GetServer(int id)
	{
		return (id >= 0 && id < m_servers.size()) ? &m_servers[id] : nullptr;
	}

	void OnServerList(HTTPClient::Result & result);
	void OnServerInfo(HTTPClient::Result & result, int serverID);

public:
	ServerBrowser();
	~ServerBrowser();

	// INetworkInterface
	bool IsAvailable() const override;

	// IServerBrowser
	void Start(bool browseLAN) override;
	void SetListener(IServerListener *pListener) override;
	void Stop() override;
	void Update() override;
	void UpdateServerInfo(int id) override;
	void BrowseForServer(unsigned int ip, unsigned short port) override;
	void BrowseForServer(const char *address, unsigned short port) override;
	void SendNatCookie(unsigned int ip, unsigned short port, int cookie) override;
	void CheckDirectConnect(int id, unsigned short port) override;
	int GetServerCount() override;
	int GetPendingQueryCount() override;
};
