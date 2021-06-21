#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/ICryPak.h"

#include "ServerPAK.h"

ServerPAK::ServerPAK()
{
}

ServerPAK::~ServerPAK()
{
}

void ServerPAK::Load(const std::string & path)
{
	Unload();

	if (gEnv->pCryPak->OpenPack("game\\", path.c_str()))
	{
		CryLogAlways("$3[CryMP] Loaded server PAK $6%s$3", path.c_str());
		m_path = path;
	}
	else
	{
		CryLogAlways("$4[CryMP] Loading server PAK $6%s$4 failed", path.c_str());
	}
}

void ServerPAK::Unload()
{
	if (m_path.empty())
	{
		return;
	}

	if (gEnv->pCryPak->ClosePack(m_path.c_str()))
	{
		CryLogAlways("$3[CryMP] Unloaded server PAK $6%s$3", m_path.c_str());
	}
	else
	{
		CryLogAlways("$4[CryMP] Unloading server PAK $6%s$4 failed", m_path.c_str());
	}

	m_path.clear();
}
