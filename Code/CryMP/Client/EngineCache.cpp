#include <string>
#include <vector>

#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/CryFind.h"
#include "CryCommon/CrySystem/CryPath.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/Cry3DEngine/I3DEngine.h"
#include "CryCommon/CryAnimation/ICryAnimation.h"

#include "EngineCache.h"

static void CacheObjectsInfo(IConsoleCmdArgs* pArgs)
{
	int vCount = -1;
	gEnv->pCharacterManager->GetLoadedModels(nullptr, vCount);
	CryLogAlways("$3Loaded Models: %d", vCount);

	int sCount = -1;
	gEnv->p3DEngine->GetLoadedStatObjArray(nullptr, sCount);
	CryLogAlways("$3Loaded StatObj: %d", sCount);

	int rCount = -1;
	gEnv->p3DEngine->GetVoxelRenderNodes(nullptr, rCount);
	CryLogAlways("$3Loaded VoxelRenderNodes: %d", rCount);

	uint32 mCount = -1;
	gEnv->p3DEngine->GetMaterialManager()->GetLoadedMaterials(nullptr, mCount);
	CryLogAlways("$3Loaded Materials: %d", mCount);
}

EngineCache::EngineCache()
{
	IConsole* pConsole = gEnv->pConsole;
	pConsole->Register("cl_engineCacheLevel", &cl_engineCacheLevel, 1, VF_NOT_NET_SYNCED, "0 - off, 4 - maximum level");

	pConsole->AddCommand("cacheInfo", CacheObjectsInfo, 0, "Get cached object info");
}

EngineCache::~EngineCache()
{
	IConsole* pConsole = gEnv->pConsole;
	pConsole->UnregisterVariable("cl_engineCacheLevel", true);
}

int EngineCache::ScanFolder(const char* folderName)
{
	int counter = 0;
	const string folder = folderName;
	string search = folder;
	search += "/*.*";

	for (auto& entry : CryFind(search.c_str()))
	{
		if (entry.IsDirectory())
		{
			string subName = folder + "/" + entry.name;
			if (m_recursing)
			{
				int c = ScanFolder(subName.c_str());
				counter += c;
			}
			else
			{
				m_recursing = true;
				int c = ScanFolder(subName.c_str());
				counter += c;
				m_recursing = false;
			}
			continue;
		}

		const char* ext = CryPath::GetExt(entry.name);

		//supported file ext
		if (_stricmp(ext, "cdf") && _stricmp(ext, "cgf") && _stricmp(ext, "cga") && _stricmp(ext, "chr"))
			continue;

		//skip folders
		if (folder == "Objects/Characters/Human/asian/infantry/camp" || folder == "Objects/Characters/Human/asian/infantry/jungle" ||
			folder == "Objects/Characters/Human/asian/infantry/elite")
			continue;

		if (string(entry.name) == "nanosuit_us_parachute.cdf" || string(entry.name) == "nanosuit_us_with_weapon.cdf" ||
			string(entry.name) == "rifleman_light.chr" || string(entry.name) == "rifleman_heavy.chr")
			continue;

		string cdfFile = folder + string("/") + string(entry.name);

		if (Cache(folder, cdfFile))
			++counter;
	}

	return counter;
}

bool EngineCache::Cache(string folder, string file)
{
	float color[] = { 1.0, 1.0, 1.0, 1.0 };
	//CryLogAlways("Caching %s... (%s)", file.c_str(), folder.c_str());

	const char* ext = CryPath::GetExt(file.c_str());
	if (!_stricmp(ext, "cdf") || !_stricmp(ext, "chr") || !_stricmp(ext, "cga"))
	{
		ICharacterInstance* pChar = gEnv->pCharacterManager->CreateInstance(file.c_str());
		if (pChar && pChar->GetFilePath())
		{
			pChar->AddRef();
			return true;
		}
	}
	else if (!_stricmp(ext, "cgf"))
	{
		IStatObj* pStatObj = gEnv->p3DEngine->LoadStatObj(file.c_str());
		if (pStatObj && pStatObj->GetFilePath())
		{
			pStatObj->AddRef();
			return true;
		}
	}
	else if (!_stricmp(ext, "mtl"))
	{
		IMaterial* pMat = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial(file.c_str());
		if (pMat)
		{
			pMat->AddRef();
		}
	}
	return false;
}

void EngineCache::OnLoadingStart(ILevelInfo* pLevel)
{
	m_isCaching = false;
}

void EngineCache::OnLoadingProgress(ILevelInfo* pLevel, int progressAmount)
{
	if (pLevel && !m_isCaching && cl_engineCacheLevel)
	{
		m_isCaching = true;

		Start();
	}
}

void EngineCache::Start()
{
	if (!cl_engineCacheLevel || cl_engineCacheLevel <= static_cast<int>(m_cacheStatus))
		return;

	int counter = 0;

	std::vector<std::string> folders = {};

	if (cl_engineCacheLevel == MINIMUM)
	{
		folders = {
			//"Objects/Characters/Human/story",
			"Objects/Characters/Human/Asian/Nanosuit",
			"Objects/Characters/Human/US/NanoSuit",
			"Objects/Vehicles/US_Vtol"
		};
	}
	else if (cl_engineCacheLevel == RECOMMENDED)
	{
		folders = {
			"Objects/Characters/Human/story",
			"Objects/Vehicles",
			"Materials"
		};
	}
	else if (cl_engineCacheLevel == HIGH)
	{
		folders = {
			"Objects/Characters/Human",
			"Objects/Weapons",
			"Objects/Vehicles",
			"Materials"
		};
	}
	else if (cl_engineCacheLevel >= LUDICROUS)
	{
		folders = {
			"Objects",
			"Materials"
		};
	}

	for (const std::string& folder : folders)
	{
		int c = ScanFolder(folder.c_str());
		counter += c;
	}

	m_cacheStatus = static_cast<ECacheLevel>(cl_engineCacheLevel);

	if (counter)
		CryLogAlways("$3[CryMP] Successfully cached %d objects", counter);
}

