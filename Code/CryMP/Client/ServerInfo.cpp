#include <cstdlib>  // std::atoi

#include "ServerInfo.h"

std::string ServerInfo::CreateEndpointString() const
{
	// TODO: IPv6 support

	return this->GetPreferredHost() + ':' + std::to_string(this->GetPreferredPort());
}

static void SplitPort(std::string& host, unsigned short& port)
{
	// TODO: IPv6 support

	if (const auto pos = host.find(':'); pos != std::string::npos)
	{
		port = std::atoi(host.c_str() + pos + 1);
		host.erase(host.begin() + pos, host.end());
	}
}

void ServerInfo::SplitLocalPort()
{
	SplitPort(this->local_host, this->local_port);
}

void ServerInfo::SplitPublicPort()
{
	SplitPort(this->public_host, this->public_port);
}

void ServerInfo::SplitMapVersion()
{
	this->map_version.clear();

	if (const auto pos = this->map.find('|'); pos != std::string::npos)
	{
		const auto begin = this->map.begin() + pos;
		const auto end = this->map.end();

		this->map_version.assign(begin + 1, end);
		this->map.erase(begin, end);
	}
}

static void FixUrl(std::string& url)
{
	if (!url.empty() && !url.starts_with("http"))
	{
		url.insert(0, "http://");
	}
}

void ServerInfo::FixMapUrl()
{
	FixUrl(this->map_url);
}

void ServerInfo::FixPakUrl()
{
	FixUrl(this->pak_url);
}
