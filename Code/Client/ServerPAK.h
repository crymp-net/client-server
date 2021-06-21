#pragma once

#include <string>

class ServerPAK
{
	std::string m_path;

public:
	ServerPAK();
	~ServerPAK();

	void Load(const std::string & path);
	void Unload();
};
