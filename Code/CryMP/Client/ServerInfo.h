#pragma once

#include <string>

struct ServerInfo
{
	std::string master;
	std::string local_host;
	std::string public_host;
	unsigned short local_port = 0;
	unsigned short public_port = 0;
	bool is_local = false;

	std::string name;
	std::string map;
	std::string map_version;
	std::string map_url;
	std::string pak_url;

	const std::string& GetPreferredHost() const
	{
		return this->is_local ? this->local_host : this->public_host;
	}

	unsigned short GetPreferredPort() const
	{
		return this->is_local ? this->local_port : this->public_port;
	}

	std::string CreateEndpointString() const;

	void SplitLocalPort();
	void SplitPublicPort();
	void SplitMapVersion();

	void FixMapUrl();
	void FixPakUrl();
};
