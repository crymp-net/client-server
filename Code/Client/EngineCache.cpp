#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/Cry3DEngine/I3DEngine.h"
#include "CryCommon/CryAnimation/ICryAnimation.h"

#include "EngineCache.h"

EngineCache::EngineCache()
{
	IConsole* pConsole = gEnv->pConsole;
	pConsole->Register("cl_engineCacheLevel", &cl_engineCacheLevel, 1, VF_NOT_NET_SYNCED, "0 - off, 4 - maximum level");
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

	ICryPak* pPak = gEnv->pSystem->GetIPak();

	_finddata_t fd;
	const intptr_t handle = pPak->FindFirst(search.c_str(), &fd);

	int index = 0;

	if (handle > -1)
	{
		do
		{
			if (!strcmp(fd.name, ".") || !strcmp(fd.name, ".."))
				continue;

			if (fd.attrib & _A_SUBDIR)
			{
				string subName = folder + "/" + fd.name;
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

			const char* ext = PathUtil::GetExt(fd.name);

			//supported file ext
			if (stricmp(ext, "cdf") && stricmp(ext, "cgf") && stricmp(ext, "cga") && stricmp(ext, "chr"))
				continue;

			//skip folders
			if (folder == "Objects/Characters/Human/asian/infantry/camp" || folder == "Objects/Characters/Human/asian/infantry/jungle" || 
				folder == "Objects/Characters/Human/asian/infantry/elite")
				continue;

			if (string(fd.name) == "nanosuit_us_parachute.cdf" || string(fd.name) == "nanosuit_us_with_weapon.cdf" ||
			    string(fd.name) == "rifleman_light.chr" || string(fd.name) == "rifleman_heavy.chr")
				continue;

			string cdfFile = folder + string("/") + string(fd.name);

			if (Cache(folder, cdfFile))
				++counter;

		} while (pPak->FindNext(handle, &fd) >= 0);
	}

	return counter;
}

bool EngineCache::Cache(string folder, string file)
{
	float color[] = { 1.0, 1.0, 1.0, 1.0 };
	//CryLogAlways("Caching %s... (%s)", file.c_str(), folder.c_str());

	const char* ext = PathUtil::GetExt(file.c_str());
	if (!stricmp(ext, "cdf") || !stricmp(ext, "chr") || !stricmp(ext, "cga"))
	{
		ICharacterInstance* pChar = gEnv->pCharacterManager->CreateInstance(file.c_str());
		if (pChar && pChar->GetFilePath())
		{
			pChar->AddRef();
			return true;
		}
	}
	else if (!stricmp(ext, "cgf"))
	{
		IStatObj* pStatObj = gEnv->p3DEngine->LoadStatObj(file.c_str());
		if (pStatObj && pStatObj->GetFilePath())
		{
			pStatObj->AddRef();
			return true;
		}
	}
	else if (!stricmp(ext, "mtl"))
	{
		IMaterial* pMat = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial(file.c_str());
		if (pMat)
		{
			pMat->GetNumRefs();
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

