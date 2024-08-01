#pragma once

#include <string>
#include <vector>

#include "CryCommon/CryNetwork/INetworkService.h"

#include "ServerInfo.h"

struct HTTPClientResult;

class ServerBrowser : public IServerBrowser
{
	std::string m_clientPublicAddress;

	std::vector<ServerInfo> m_servers;
	IServerListener *m_pListener = nullptr;

	unsigned int m_pendingQueryCount = 0;
	unsigned int m_contract = 0;
	bool m_lastRequestSucceeded = false;

	bool OnPublicAddress(HTTPClientResult & result);
	bool OnServerList(HTTPClientResult & result, const std::string & master);
	bool OnServerInfo(HTTPClientResult & result, int serverID);

public:
	ServerBrowser();
	~ServerBrowser();

	void QueryClientPublicAddress();

	bool LastRequestSucceeded() const { return m_lastRequestSucceeded; }

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
