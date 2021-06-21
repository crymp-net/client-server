#pragma once

#include <string>
#include <string_view>

#include "HTTPClient.h"

class ServerConnector
{
	std::string m_host;
	unsigned int m_port = 0;
	unsigned int m_contractID = 0;

	// server info
	std::string m_serverName;
	std::string m_serverMapName;
	std::string m_serverMapLink;
	std::string m_serverPAKLink;

	void RequestServerInfo();
	void OnServerInfo(HTTPClient::Result & result);
	void DownloadMap();
	void DownloadPAK();
	void TryConnect();

public:
	ServerConnector();
	~ServerConnector();

	void Connect(const std::string_view & host, unsigned int port);
};
