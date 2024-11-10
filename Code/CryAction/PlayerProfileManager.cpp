#include <map>

#include "CryCommon/CryAction/IActionMapManager.h"
#include "CryCommon/CrySystem/CryFind.h"
#include "CryCommon/CrySystem/gEnv.h"
#include "CryCommon/CrySystem/ICryPak.h"
#include "CryCommon/CrySystem/ISystem.h"

#include "GameFramework.h"
#include "PlayerProfileManager.h"

class SaveGameThumbnail final : public ISaveGameThumbnail
{
	int m_refCount = 0;
	int m_width = 0;
	int m_height = 0;
	int m_depth = 0;
	std::string m_name;
	std::vector<std::uint8_t> m_data;

	explicit SaveGameThumbnail(std::string_view name) : m_name(name) {}

public:
	static SaveGameThumbnail* Create(std::string_view name)
	{
		return new SaveGameThumbnail(name);
	}

	// TODO: Init

	////////////////////////////////////////////////////////////////////////////////
	// ISaveGameThumbnail
	////////////////////////////////////////////////////////////////////////////////

	const char* GetSaveGameName() override
	{
		return m_name.c_str();
	}

	void GetImageInfo(int& width, int& height, int& depth) override
	{
		width = m_width;
		height = m_height;
		depth = m_depth;
	}

	int GetWidth() override
	{
		return m_width;
	}

	int GetHeight() override
	{
		return m_height;
	}

	int GetDepth() override
	{
		return m_depth;
	}

	const std::uint8_t* GetImageData() override
	{
		return m_data.data();
	}

	void AddRef() override
	{
		++m_refCount;
	}

	void Release() override
	{
		if (--m_refCount <= 0)
		{
			delete this;
		}
	}

	////////////////////////////////////////////////////////////////////////////////
};

class SaveGameEnumerator final : public ISaveGameEnumerator
{
	struct Entry
	{
		// TODO
		SGameDescription description = {};
		ISaveGameThumbnailPtr thumbnail;
	};

	int m_refCount = 0;
	std::vector<Entry> m_entries;

	SaveGameEnumerator() = default;

public:
	static SaveGameEnumerator* Create()
	{
		return new SaveGameEnumerator();
	}

	// TODO: Init

	////////////////////////////////////////////////////////////////////////////////
	// ISaveGameEnumerator
	////////////////////////////////////////////////////////////////////////////////

	int GetCount() override
	{
		return static_cast<int>(m_entries.size());
	}

	bool GetDescription(int index, SGameDescription& description) override
	{
		if (index < 0 || index >= static_cast<int>(m_entries.size()))
		{
			return false;
		}

		description = m_entries[index].description;

		return true;
	}

	ISaveGameThumbnailPtr GetThumbnail(int index) override
	{
		if (index < 0 || index >= static_cast<int>(m_entries.size()))
		{
			return {};
		}

		return m_entries[index].thumbnail;
	}

	ISaveGameThumbnailPtr GetThumbnail(const char* saveGameName) override
	{
		// TODO
		return {};
	}

	void AddRef() override
	{
		++m_refCount;
	}

	void Release() override
	{
		if (--m_refCount <= 0)
		{
			delete this;
		}
	}

	////////////////////////////////////////////////////////////////////////////////
};

using AttributeMap = std::map<std::string, TFlowInputData, std::less<void>>;

void SerializeAttributes(const AttributeMap& attributes, XmlNodeRef& xml)
{
	xml->setAttr("Version", 8);

	for (const auto& [name, value] : attributes)
	{
		CryStringT<char> stringValue;
		value.GetValueWithConversion(stringValue);

		XmlNodeRef attr = xml->newChild("Attr");
		attr->setAttr("name", name.c_str());
		attr->setAttr("value", stringValue.c_str());
	}
}

void DeserializeAttributes(const XmlNodeRef& xml, AttributeMap& attributes)
{
	const std::string_view attrTag = "Attr";
	const int attrCount = xml->getChildCount();
	for (int i = 0; i < attrCount; i++)
	{
		const XmlNodeRef attr = xml->getChild(i);
		if (!attr || attr->getTag() != attrTag)
		{
			continue;
		}

		const char* name = attr->getAttr("name");
		if (!name || !*name)
		{
			continue;
		}

		const char* value = attr->getAttr("value");
		if (!value)
		{
			continue;
		}

		// keep existing values
		attributes.try_emplace(name, CryStringT<char>(value));
	}
}

class AttributeEnumerator final : public IAttributeEnumerator
{
	int m_refCount = 0;
	AttributeMap m_attributes;
	AttributeMap::iterator m_iterator;

	explicit AttributeEnumerator(const AttributeMap& attributes, const AttributeMap& defaultAttributes)
	: m_attributes(attributes)
	{
		// fill in missing attributes with default ones
		m_attributes.insert(defaultAttributes.begin(), defaultAttributes.end());

		m_iterator = m_attributes.begin();
	}

public:
	static AttributeEnumerator* Create(const AttributeMap& attributes, const AttributeMap& defaultAttributes)
	{
		return new AttributeEnumerator(attributes, defaultAttributes);
	}

	////////////////////////////////////////////////////////////////////////////////
	// IAttributeEnumerator
	////////////////////////////////////////////////////////////////////////////////

	bool Next(SAttributeDescription& description) override
	{
		if (m_iterator == m_attributes.end())
		{
			return false;
		}

		description.name = m_iterator->first.c_str();
		++m_iterator;

		return true;
	}

	void AddRef() override
	{
		++m_refCount;
	}

	void Release() override
	{
		if (--m_refCount <= 0)
		{
			delete this;
		}
	}

	////////////////////////////////////////////////////////////////////////////////
};

class LevelRotationFile final : public ILevelRotationFile
{
	std::string m_filename;

public:
	explicit LevelRotationFile(std::string_view filename) : m_filename(filename) {}

	const std::string& GetFilename() const
	{
		return m_filename;
	}

	////////////////////////////////////////////////////////////////////////////////
	// ILevelRotationFile
	////////////////////////////////////////////////////////////////////////////////

	bool Save(XmlNodeRef xml) override
	{
		return xml->saveToFile(m_filename.c_str());
	}

	XmlNodeRef Load() override
	{
		if (gEnv->pCryPak->IsFileExist(m_filename.c_str()))
		{
			return gEnv->pSystem->LoadXmlFile(m_filename.c_str());
		}

		return gEnv->pSystem->LoadXmlFile("Libs/Config/Profiles/default/levelrotation/levelrotation.xml");
	}

	void Complete() override
	{
	}

	////////////////////////////////////////////////////////////////////////////////
};

class PlayerProfile final : public IPlayerProfile
{
	std::string m_name;
	AttributeMap m_attributes;
	AttributeMap m_defaultAttributes;
	std::vector<std::unique_ptr<LevelRotationFile>> m_levelRotationFiles;

public:
	explicit PlayerProfile(std::string_view name) : m_name(name)
	{
		this->LoadAttributes();
		this->LoadDefaultAttributes();
	}

	const std::string& GetNameString() const
	{
		return m_name;
	}

	void LoadActionMaps()
	{
		// TODO
	}

	////////////////////////////////////////////////////////////////////////////////
	// IPlayerProfile
	////////////////////////////////////////////////////////////////////////////////

	bool Reset() override
	{
		GameFramework::GetInstance()->GetIActionMapManager()->Reset();

		m_attributes.clear();

		return true;
	}

	bool IsDefault() const override
	{
		return m_name == "default";
	}

	const char* GetName() override
	{
		return m_name.c_str();
	}

	const char* GetUserId() override
	{
		return "";
	}

	IActionMap* GetActionMap(const char* name) override
	{
		return GameFramework::GetInstance()->GetIActionMapManager()->GetActionMap(name);
	}

	bool SetAttribute(const char* name, const TFlowInputData& value) override
	{
		m_attributes[name] = value;

		return true;
	}

	bool ResetAttribute(const char* name) override
	{
		return m_attributes.erase(name) > 0;
	}

	bool GetAttribute(const char* name, TFlowInputData& value, bool useDefaultFallback = true) const override
	{
		auto it = m_attributes.find(name);
		if (it == m_attributes.end())
		{
			if (!useDefaultFallback)
			{
				return false;
			}

			it = m_defaultAttributes.find(name);
			if (it == m_defaultAttributes.end())
			{
				return false;
			}
		}

		value = it->second;

		return true;
	}

	IAttributeEnumeratorPtr CreateAttributeEnumerator() override
	{
		return AttributeEnumerator::Create(m_attributes, m_defaultAttributes);
	}

	ISaveGameEnumeratorPtr CreateSaveGameEnumerator() override
	{
		// TODO
		return {};
	}

	ISaveGame* CreateSaveGame() override
	{
		// TODO
		return {};
	}

	ILoadGame* CreateLoadGame() override
	{
		// TODO
		return {};
	}

	bool DeleteSaveGame(const char* name) override
	{
		// TODO
		CryLogAlways("%s(\"%s\"): Not implemented!", __FUNCTION__, name);
		return {};
	}

	ILevelRotationFile* GetLevelRotationFile(const char* name) override
	{
		const std::string_view nameView(name);

		for (const std::unique_ptr<LevelRotationFile>& file : m_levelRotationFiles)
		{
			if (file->GetFilename() == nameView)
			{
				return file.get();
			}
		}

		return m_levelRotationFiles.emplace_back(std::make_unique<LevelRotationFile>(nameView)).get();
	}

	////////////////////////////////////////////////////////////////////////////////

private:
	std::string GetAttributesPath()
	{
		std::string path = "%USER%/Profiles/";
		path += m_name;
		path += "/attributes.xml";
		return path;
	}

	void SaveAttributes()
	{
		// TODO

		const std::string path = this->GetAttributesPath();
	}

	void LoadAttributes()
	{
		const std::string path = this->GetAttributesPath();

		XmlNodeRef xml = gEnv->pSystem->LoadXmlFile(path.c_str());
		if (!xml)
		{
			CryLogWarningAlways("%s: Failed to load '%s'", __FUNCTION__, path.c_str());
			return;
		}

		DeserializeAttributes(xml, m_attributes);
	}

	void LoadDefaultAttributes()
	{
		XmlNodeRef xml = gEnv->pSystem->LoadXmlFile("Libs/Config/Profiles/default/attributes.xml");
		if (!xml)
		{
			CryLogWarningAlways("%s: Failed to load default attributes", __FUNCTION__);
			return;
		}

		DeserializeAttributes(xml, m_attributes);
	}
};

PlayerProfileManager::PlayerProfileManager()
{
	IConsole* pConsole = gEnv->pConsole;

	m_pRichSaveGamesCVar = pConsole->RegisterInt("pp_RichSaveGames", 1, VF_NULL,
		"Enable RichSaveGame Format for SaveGames");
	m_pRSFDebugWriteCVar = pConsole->RegisterInt("pp_RSFDebugWrite", gEnv->pSystem->IsDevMode() ? 1 : 0, VF_NULL,
		"When RichSaveGames are enabled, save plain XML Data alongside for debugging");
	m_pRSFDebugWriteOnLoadCVar = pConsole->RegisterInt("pp_RSFDebugWriteOnLoad", 0, VF_NULL,
		"When RichSaveGames are enabled, save plain XML Data alongside for debugging when loading a savegame");

	pConsole->AddCommand("dump_action_maps", &PlayerProfileManager::DumpActionMapsCommand);
}

PlayerProfileManager::~PlayerProfileManager()
{
	IConsole* pConsole = gEnv->pConsole;

	m_pRichSaveGamesCVar->Release();
	m_pRSFDebugWriteCVar->Release();
	m_pRSFDebugWriteOnLoadCVar->Release();

	pConsole->RemoveCommand("dump_action_maps");
}

////////////////////////////////////////////////////////////////////////////////
// IPlayerProfileManager
////////////////////////////////////////////////////////////////////////////////

bool PlayerProfileManager::Initialize()
{
	return true;
}

bool PlayerProfileManager::Shutdown()
{
	return true;
}

void PlayerProfileManager::GetMemoryStatistics(ICrySizer*)
{
}

int PlayerProfileManager::GetUserCount()
{
	return 1;
}

bool PlayerProfileManager::GetUserInfo(int index, SUserInfo& userInfo)
{
	if (index != 1)
	{
		return false;
	}

	userInfo.userId = "";

	return true;
}

bool PlayerProfileManager::LoginUser(const char* userId, bool& firstTime)
{
	this->LogoutUser(nullptr);
	this->LoadProfileNames();

	if (m_profileNames.empty())
	{
		firstTime = true;
		m_profileNames.emplace_back("default");
	}

	return true;
}

bool PlayerProfileManager::LogoutUser(const char* userId)
{
	if (m_profileNames.empty())
	{
		return false;
	}

	// TODO: save current profile

	m_profileNames.clear();
	m_currentProfile = nullptr;
	m_previewProfile = nullptr;

	return true;
}

int PlayerProfileManager::GetProfileCount(const char* userId)
{
	return static_cast<int>(m_profileNames.size());
}

bool PlayerProfileManager::GetProfileInfo(const char* userId, int index, SProfileDescription& profileInfo)
{
	if (index < 0 || index >= static_cast<int>(m_profileNames.size()))
	{
		return false;
	}

	profileInfo.name = m_profileNames[index].c_str();

	return true;
}

bool PlayerProfileManager::CreateProfile(const char* userId, const char* profileName, bool overrideIfPresent, EProfileOperationResult& result)
{
	// TODO
	return {};
}

bool PlayerProfileManager::DeleteProfile(const char* userId, const char* profileName, EProfileOperationResult& result)
{
	// TODO
	return {};
}

bool PlayerProfileManager::RenameProfile(const char* userId, const char* newName, EProfileOperationResult& result)
{
	// TODO
	return {};
}

bool PlayerProfileManager::SaveProfile(const char* userId, EProfileOperationResult& result)
{
	// TODO
	return {};
}

IPlayerProfile* PlayerProfileManager::ActivateProfile(const char* userId, const char* profileName)
{
	const std::string_view profileNameView(profileName);

	if (m_currentProfile && m_currentProfile->GetNameString() == profileNameView)
	{
		// do nothing
	}
	else if (m_previewProfile && m_previewProfile->GetNameString() == profileNameView)
	{
		// TODO: save current profile
		m_currentProfile = std::move(m_previewProfile);
	}
	else
	{
		// TODO: check if it exists
		// TODO: save current profile
		m_currentProfile = std::make_unique<PlayerProfile>(profileNameView);
	}

	return m_currentProfile.get();
}

IPlayerProfile* PlayerProfileManager::GetCurrentProfile(const char* userId)
{
	return m_currentProfile.get();
}

const char* PlayerProfileManager::GetCurrentUser()
{
	return "";
}

bool PlayerProfileManager::ResetProfile(const char* userId)
{
	// TODO
	return {};
}

IPlayerProfile* PlayerProfileManager::GetDefaultProfile()
{
	// TODO
	return {};
}

const IPlayerProfile* PlayerProfileManager::PreviewProfile(const char* userId, const char* profileName)
{
	const std::string_view profileNameView(profileName);

	if (m_currentProfile && m_currentProfile->GetNameString() == profileNameView)
	{
		return m_currentProfile.get();
	}

	if (m_previewProfile && m_previewProfile->GetNameString() == profileNameView)
	{
		return m_previewProfile.get();
	}

	m_previewProfile = std::make_unique<PlayerProfile>(profileNameView);

	return m_previewProfile.get();
}

void PlayerProfileManager::SetSharedSaveGameFolder(const char* sharedSaveGameFolder)
{
	CryLogAlways("%s(\"%s\"): Not implemented!", __FUNCTION__, sharedSaveGameFolder);
}

const char* PlayerProfileManager::GetSharedSaveGameFolder()
{
	return "%USER%/SaveGames";
}

////////////////////////////////////////////////////////////////////////////////

void PlayerProfileManager::LoadProfileNames()
{
	m_profileNames.clear();

	for (auto& entry : CryFind("%USER%/Profiles/*"))
	{
		m_profileNames.emplace_back(entry.name);
	}
}

void PlayerProfileManager::DumpActionMapsCommand(IConsoleCmdArgs* args)
{
	IActionMapManager* pActionMapManager = GameFramework::GetInstance()->GetIActionMapManager();
	IActionMapIteratorPtr pActionMapIterator = pActionMapManager->CreateActionMapIterator();
	IActionMap* pActionMap = nullptr;
	while ((pActionMap = pActionMapIterator->Next()) != nullptr)
	{
		CryLogAlways("ActionMap: '%s' 0x%p", pActionMap->GetName(), pActionMap);

		IActionMapBindInfoIteratorPtr pBindInfoIterator = pActionMap->CreateBindInfoIterator();
		const SActionMapBindInfo* pBindInfo = nullptr;
		int bindCount = 0;
		while ((pBindInfo = pBindInfoIterator->Next()) != nullptr)
		{
			CryLogAlways("  - Action %d: '%s'", bindCount++, pBindInfo->action);

			for (int i = 0; i < pBindInfo->nKeys; i++)
			{
				CryLogAlways("    - Key %d: '%s'", i, pBindInfo->keys[i]);
			}
		}
	}
}
