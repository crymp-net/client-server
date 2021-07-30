#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/ICryPak.h"
#include "CryCommon/CryEntitySystem/IEntitySystem.h"
#include "CryCommon/CryScriptSystem/IScriptSystem.h"
#include "CryCommon/CryNetwork/INetwork.h"
#include "CryCommon/CryAction/IVehicleSystem.h"
#include "CryCommon/CryAction/IItemSystem.h"
#include "Library/WinAPI.h"

#include "ServerPAK.h"

ServerPAK::ServerPAK()
{
}

ServerPAK::~ServerPAK()
{
}

bool ServerPAK::IsZipFile(const std::string & path)
{
	try
	{
		WinAPI::File file(path, WinAPI::FileAccess::READ_ONLY);

		if (file && file.Read(2) == "PK")
		{
			// file magic number verified
			return true;
		}
	}
	catch (const std::exception & ex)
	{
	}

	return false;
}

bool ServerPAK::Load(const std::string & path)
{
	Unload();

	// Crytek's CryPak crashes when it tries to load something that's not a zip file
	if (!IsZipFile(path))
	{
		CryLogAlways("$4[CryMP] Invalid PAK file $6%s$4", path.c_str());
		return false;
	}
	
	const bool opened = gEnv->pCryPak->OpenPack("game\\", path.c_str());
	if (opened)
	{
		CryLogAlways("$3[CryMP] Loaded server PAK $6%s$3", path.c_str());
		m_path = path;

		ReloadEntityScripts();
	}
	else
	{
		CryLogAlways("$4[CryMP] Loading server PAK $6%s$4 failed", path.c_str());
	}
	
	return opened;
}

bool ServerPAK::Unload()
{
	if (m_path.empty())
	{
		return false;
	}

	const bool closed = gEnv->pCryPak->ClosePack(m_path.c_str());
	if (closed)
	{
		CryLogAlways("$3[CryMP] Unloaded server PAK $6%s$3", m_path.c_str());
	}
	else
	{
		CryLogAlways("$4[CryMP] Unloading server PAK $6%s$4 failed", m_path.c_str());
	}

	m_path.clear();

	return closed;
}

void ServerPAK::OnDisconnect(int reason, const char* message)
{
	gEnv->pScriptSystem->ResetTimers();

	const bool unloaded = Unload();
	if (unloaded || (reason != eDC_ServerFull && reason != eDC_AuthenticationFailed))
	{
		ReloadEntityScripts();
	}
}

void ServerPAK::ReloadEntityScripts()
{
	//CryMP reset scripts to avoid disconnect bugs caused by possible new rmis in PAK
	IEntityClassRegistry* pClassRegistry = gEnv->pEntitySystem->GetClassRegistry();
	pClassRegistry->IteratorMoveFirst();

	IEntityClass* pEntityClass = nullptr;
	int counter = 0;
	bool itemLuaLoaded = false;
	bool vehicleLuaLoaded = false;
	while (pEntityClass = pClassRegistry->IteratorNext())
	{
		const char* file = pEntityClass->GetScriptFile();
		if (strlen(file) > 0)
		{
			const bool isVehicleClass = gEnv->pGame->GetIGameFramework()->GetIVehicleSystem()->IsVehicleClass(pEntityClass->GetName());
			const bool isItemClass = gEnv->pGame->GetIGameFramework()->GetIItemSystem()->IsItemClass(pEntityClass->GetName());
			
			if (isVehicleClass)
			{
				if (vehicleLuaLoaded)
					continue;
			}
			else if (isItemClass)
			{
				if (itemLuaLoaded)
					continue;
			}
			else
			{
				SmartScriptTable entityTable;
				if (!gEnv->pScriptSystem->GetGlobalValue(pEntityClass->GetName(), entityTable))
					continue;
			}

			gEnv->pScriptSystem->SetGlobalToNull(pEntityClass->GetName());

			const bool ok = pEntityClass->LoadScript(true);
			if (ok)
			{
				if (isItemClass)
					itemLuaLoaded = true;
				else if (isVehicleClass)
					vehicleLuaLoaded = true;

				++counter;
			}
			
			/*if (ok)
				CryLogAlways("$3[CryMP] Loaded Script file %s successfully (%s)", pEntityClass->GetName(), pEntityClass->GetScriptFile());
			else
				CryLogAlways("$4[CryMP] Failed to load %s script", pEntityClass->GetName()); 
			*/
		}
	}

	if (counter)
	{
		CryLogAlways("$3[CryMP] Reloaded %d scripts", counter);
	}
}
