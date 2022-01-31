#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/ICryPak.h"
#include "CryCommon/CryEntitySystem/IEntitySystem.h"
#include "CryCommon/CryScriptSystem/IScriptSystem.h"
#include "CryCommon/CryNetwork/INetwork.h"
#include "CryCommon/CryAction/IVehicleSystem.h"
#include "CryCommon/CryAction/IItemSystem.h"
#include "CryCommon/CryAction/IMaterialEffects.h"
#include "Library/WinAPI.h"

#include "ServerPAK.h"
#include "Client/Client.h"

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

		//Trigger a subsystem reset
		m_bResetRequired = true;
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

void ServerPAK::OnLoadingStart(ILevelInfo* pLevel)
{
	m_bResetRequired = true;
}

void ServerPAK::OnConnect()
{
	if (!m_bResetRequired)
		return;

	ResetSubSystems();
}

void ServerPAK::OnDisconnect(int reason, const char* message)
{
	gEnv->pScriptSystem->ResetTimers();

	const bool unloaded = Unload();
	if (unloaded)
	{
		m_bResetRequired = true;
	}
}

void ServerPAK::ResetSubSystems()
{
	IGameFramework* pGameFrameWork = gEnv->pGame->GetIGameFramework();

	//Reset a bunch of subsystems
	gEnv->p3DEngine->ResetPostEffects();
	gEnv->p3DEngine->ResetParticlesAndDecals();
	pGameFrameWork->ResetBrokenGameObjects();
	gEnv->pPhysicalWorld->ResetDynamicEntities();
	//gEnv->pFlowSystem->Reset();
	pGameFrameWork->GetIItemSystem()->Reset();
	//gEnv->pDialogSystem->Reset();
	pGameFrameWork->GetIMaterialEffects()->Reset();
	pGameFrameWork->GetIVehicleSystem()->Reset();

	//Reset scripts to avoid disconnect bugs caused by possible new rmis in PAK
	IEntityClassRegistry* pClassRegistry = gEnv->pEntitySystem->GetClassRegistry();

	//Parent classes: BasicEntity and Chickens are parent classes and have to be loaded before others
	IEntityClass *pBasicEntityClass = pClassRegistry->FindClass("BasicEntity");
	if (pBasicEntityClass)
	{
		pBasicEntityClass->LoadScript(true);
	}
	IEntityClass* pChickensClass = pClassRegistry->FindClass("Chickens");
	if (pChickensClass)
	{
		pChickensClass->LoadScript(true);
	}

	IEntityClass* pGUI = pClassRegistry->FindClass("GUI");

	std::vector<IEntityClass*> classes;
	pClassRegistry->IteratorMoveFirst();
	IEntityClass* pEntityClass = nullptr;
	int counter = 0;
	while (pEntityClass = pClassRegistry->IteratorNext())
	{
		const char* file = pEntityClass->GetScriptFile();
		if (strlen(file) > 0)
		{
			if (pEntityClass == pBasicEntityClass || pEntityClass == pChickensClass)
				continue;

			//GUI corrupts Lua environment :S Hope there aren't any others..
			if (pEntityClass == pGUI)
				continue;
			
			SmartScriptTable entityTable;
			if (!gEnv->pScriptSystem->GetGlobalValue(pEntityClass->GetName(), entityTable))
				continue;

			classes.push_back(pEntityClass);
		}
	}

	for (IEntityClass* pClass : classes)
	{
		const bool bReloaded = pClass->LoadScript(true);
		if (bReloaded)
		{
			++counter;
		}

	}

	if (counter)
	{
		CryLogAlways("$3[CryMP] Reloaded %d scripts", counter);
		m_bResetRequired = false;
	}
}
