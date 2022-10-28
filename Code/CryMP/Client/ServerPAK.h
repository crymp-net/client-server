#pragma once

#include <string>

struct ILevelInfo;

class ServerPAK
{
	std::string m_path;
	bool m_bResetRequired = false;

public:
	ServerPAK();
	~ServerPAK();

	bool IsZipFile(const std::string & path);
	bool Load(const std::string & path);
	bool Unload();
	void OnLoadingStart(ILevelInfo* pLevel);
	void OnConnect();
	void OnDisconnect(int reason, const char* message);
	void ResetSubSystems();
};
