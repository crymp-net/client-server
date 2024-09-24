#include "CryCommon/Cry3DEngine/I3DEngine.h"
#include "CryCommon/CryAction/IGameplayRecorder.h"
#include "CryCommon/CryAISystem/IAISystem.h"
#include "CryCommon/CryAnimation/ICryAnimation.h"
#include "CryCommon/CryEntitySystem/IEntitySystem.h"
#include "CryCommon/CryScriptSystem/IScriptSystem.h"
#include "CryCommon/CrySoundSystem/ISound.h"
#include "CryCommon/CrySystem/gEnv.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/CrySystem/ISystem.h"
#include "CrySystem/CryLog.h"

#include "Library/DigitCount.h"

#include "EquipmentManager.h"
#include "ItemParamsNode.h"
#include "ItemSystem.h"

////////////////////////////////////////////////////////////////////////////////

static char ToLower(char ch)
{
	return ch | ((ch >= 'A' && ch <= 'Z') << 5);
}

static char FixSlash(char ch)
{
	return (ch == '\\') ? '/' : ch;
}

static bool IsEqualNoCase(const char* strA, const char* strB)
{
	char chA;

	do
	{
		chA = *strA++;

		if (ToLower(chA) != ToLower(*strB++))
		{
			return false;
		}
	}
	while (chA);

	return true;
}

static std::string NormalizePath(const char* path)
{
	if (!path)
	{
		return {};
	}

	std::string result(path);

	for (char& ch : result)
	{
		ch = ToLower(ch);
		ch = FixSlash(ch);
	}

	return result;
}

static std::string_view GetPathExtension(std::string_view path)
{
	const auto dot = path.find_last_of('.');
	if (dot == std::string_view::npos)
	{
		return {};
	}

	path.remove_prefix(dot + 1);

	return path;
}

////////////////////////////////////////////////////////////////////////////////

ItemSystem::ItemSystem(IGameFramework* pGameFramework, ISystem* pSystem) : m_pGameFramework(pGameFramework)
{
	this->RegisterCVars();

	m_garbage.reserve(64);
	m_listeners.reserve(8);
	m_spawnName.reserve(64);

	m_pEquipmentManager = std::make_unique<EquipmentManager>(this);
}

ItemSystem::~ItemSystem()
{
	this->UnregisterCVars();
}

void ItemSystem::Update(float frameTime)
{
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_GAME);

	IActor* pActor = m_pGameFramework->GetClientActor();
	if (pActor)
	{
		IInventory* pInventory = pActor->GetInventory();
		if (pInventory)
		{
			IItem* pCurrentItem = this->GetItem(pInventory->GetCurrentItem());
			if (pCurrentItem)
			{
				pCurrentItem->UpdateFPView(frameTime);
			}

			IEntityClass* pOffHandClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("OffHand");
			IItem* pOffHand = this->GetItem(pInventory->GetItemByClass(pOffHandClass));
			if (pOffHand)
			{
				pOffHand->UpdateFPView(frameTime);
			}
		}
	}
}

void ItemSystem::PrecacheLevel()
{
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_GAME);

	IEntitySystem* pEntitySystem = gEnv->pEntitySystem;

	for (auto& [itemId, pItem] : m_items)
	{
		IEntity* pItemEntity = pEntitySystem->GetEntity(itemId);
		if (pItemEntity)
		{
			const char* className = pItemEntity->GetClass()->GetName();

			this->CacheItemGeometry(className);
			this->CacheItemSound(className);
		}
	}
}

void ItemSystem::RegisterItemFactory(const char* name, IGameFramework::IItemCreator* pCreator)
{
	m_factories[name] = ItemFactory(pCreator);
}

////////////////////////////////////////////////////////////////////////////////
// ILevelSystemListener
////////////////////////////////////////////////////////////////////////////////

void ItemSystem::OnLevelNotFound(const char *levelName)
{
}

void ItemSystem::OnLoadingStart(ILevelInfo *pLevel)
{
	this->Reset();
	this->ClearGeometryCache();
	this->ClearSoundCache();

	m_multiplayer = gEnv->bMultiplayer;
}

void ItemSystem::OnLoadingComplete(ILevel *pLevel)
{
	this->PrecacheLevel();
}

void ItemSystem::OnLoadingError(ILevelInfo *pLevel, const char *error)
{
}

void ItemSystem::OnLoadingProgress(ILevelInfo *pLevel, int progressAmount)
{
}

////////////////////////////////////////////////////////////////////////////////
// IItemSystem
////////////////////////////////////////////////////////////////////////////////

void ItemSystem::Reset()
{
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_GAME);

	if (gEnv->pSystem->IsSerializingFile() != 1)
	{
		return;
	}

	IEntitySystem* pEntitySystem = gEnv->pEntitySystem;

	for (auto it = m_items.begin(); it != m_items.end();)
	{
		if (pEntitySystem->GetEntity(it->first))
		{
			++it;
		}
		else
		{
			it = m_items.erase(it);
		}
	}
}

void ItemSystem::Reload()
{
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_GAME);

	m_reloading = true;

	m_params.clear();
	this->RegisterXMLData();

	IEntitySystem* pEntitySystem = gEnv->pEntitySystem;
	SEntityEvent resetEvent(ENTITY_EVENT_RESET);

	for (auto& [itemId, pItem] : m_items)
	{
		IEntity* pItemEntity = pEntitySystem->GetEntity(itemId);
		if (pItemEntity)
		{
			pItemEntity->SendEvent(resetEvent);
		}
	}

	m_reloading = false;
}

void ItemSystem::Scan(const char* folderName)
{
	this->RegisterXMLData();
}

IItemParamsNode* ItemSystem::CreateParams()
{
	return new ItemParamsNode();
}

const IItemParamsNode* ItemSystem::GetItemParams(const char* itemName) const
{
	const auto it = m_params.find(itemName);
	if (it == m_params.end())
	{
		return nullptr;
	}

	return this->GetParamsRoot(it->second);
}

int ItemSystem::GetItemParamsCount() const
{
	return static_cast<int>(m_params.size());
}

const char* ItemSystem::GetItemParamName(int index) const
{
	if (index < 0 || index >= static_cast<int>(m_params.size()))
	{
		return nullptr;
	}

	const auto it = std::next(m_params.begin(), index);

	return it->first.c_str();
}

std::uint8_t ItemSystem::GetItemPriority(const char* item) const
{
	const auto it = m_params.find(item);
	if (it == m_params.end())
	{
		return 0;
	}

	return static_cast<std::uint8_t>(it->second.priority);
}

const char* ItemSystem::GetItemCategory(const char* item) const
{
	const auto it = m_params.find(item);
	if (it == m_params.end())
	{
		return nullptr;
	}

	return it->second.category;
}

std::uint8_t ItemSystem::GetItemUniqueId(const char* item) const
{
	const auto it = m_params.find(item);
	if (it == m_params.end())
	{
		return 0;
	}

	return static_cast<std::uint8_t>(it->second.uniqueId);
}

bool ItemSystem::IsItemClass(const char* name) const
{
	return m_params.contains(name);
}

void ItemSystem::RegisterForCollection(EntityId itemId)
{
	if (!gEnv->bServer || !gEnv->bMultiplayer)
	{
		return;
	}

	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_GAME);

	m_garbage.push_back(itemId);

	const int lyingItemLimit = m_pLyingItemLimitCVar->GetIVal();
	if (lyingItemLimit < 0 || static_cast<int>(m_garbage.size()) <= lyingItemLimit)
	{
		return;
	}

	const EntityId itemIdToRemove = m_garbage[0];
	m_garbage.erase(m_garbage.begin());

	IEntitySystem* pEntitySystem = gEnv->pEntitySystem;

	IEntity* pItemEntityToRemove = pEntitySystem->GetEntity(itemIdToRemove);
	const char* itemNameToRemove = (pItemEntityToRemove) ? pItemEntityToRemove->GetName() : "unknown item";

	CryLogAlways("[ItemSystem] Removing %s (%u) due to i_lying_item_limit", itemNameToRemove, itemIdToRemove);

	pEntitySystem->RemoveEntity(itemIdToRemove);
}

void ItemSystem::UnregisterForCollection(EntityId itemId)
{
	if (!gEnv->bServer || !gEnv->bMultiplayer)
	{
		return;
	}

	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_GAME);

	std::erase(m_garbage, itemId);
}

void ItemSystem::AddItem(EntityId itemId, IItem* pItem)
{
	m_items[itemId] = pItem;
}

void ItemSystem::RemoveItem(EntityId itemId)
{
	m_items.erase(itemId);

	this->UnregisterForCollection(itemId);
}

IItem* ItemSystem::GetItem(EntityId itemId) const
{
	FUNCTION_PROFILER(gEnv->pSystem, PROFILE_GAME);

	const auto it = m_items.find(itemId);
	if (it == m_items.end())
	{
		return nullptr;
	}

	return it->second;
}

void ItemSystem::SetConfiguration(const char* name)
{
	m_multiplayer = IsEqualNoCase(name, "mp");
}

const char* ItemSystem::GetConfiguration() const
{
	return m_multiplayer ? "mp" : "";
}

ICharacterInstance* ItemSystem::GetCachedCharacter(const char* fileName)
{
	const auto it = m_cachedCharacters.find(NormalizePath(fileName));
	if (it == m_cachedCharacters.end())
	{
		return nullptr;
	}

	return it->second.get();
}

IStatObj* ItemSystem::GetCachedObject(const char* fileName)
{
	const auto it = m_cachedObjects.find(NormalizePath(fileName));
	if (it == m_cachedObjects.end())
	{
		return nullptr;
	}

	return it->second.get();
}

void ItemSystem::CacheObject(const char* fileName)
{
	const std::string path = NormalizePath(fileName);

	if (path.empty() || m_cachedObjects.contains(path) || m_cachedCharacters.contains(path))
	{
		return;
	}

	const std::string_view extension = GetPathExtension(path);

	if (extension == "cdf" || extension == "chr" || extension == "cga")
	{
		ICharacterInstance* pCharacter = gEnv->pSystem->GetIAnimationSystem()->CreateInstance(fileName);
		if (pCharacter)
		{
			pCharacter->AddRef();
			m_cachedCharacters[path] = SmartCharacterInstance(pCharacter);
		}
	}
	else
	{
		IStatObj* pObject = gEnv->p3DEngine->LoadStatObj(fileName);
		if (pObject)
		{
			pObject->AddRef();
			m_cachedObjects[path] = SmartStatObj(pObject);
		}
	}
}

void ItemSystem::CacheGeometry(const IItemParamsNode* geometry)
{
	if (!geometry)
	{
		return;
	}

	const int childCount = geometry->GetChildCount();

	for (int i = 0; i < childCount; i++)
	{
		this->CacheObject(geometry->GetChild(i)->GetNameAttribute());
	}
}

void ItemSystem::CacheItemGeometry(const char* className)
{
	const auto it = m_params.find(className);
	if (it == m_params.end())
	{
		return;
	}

	if (it->second.geometryCached)
	{
		return;
	}

	it->second.geometryCached = true;

	const IItemParamsNode* root = this->GetParamsRoot(it->second);
	if (!root)
	{
		return;
	}

	this->CacheGeometry(root->GetChild("geometry"));
}

void ItemSystem::ClearGeometryCache()
{
	m_cachedObjects.clear();
	m_cachedCharacters.clear();

	for (auto& [name, params] : m_params)
	{
		params.geometryCached = false;
	}
}

void ItemSystem::CacheItemSound(const char* className)
{
	ISoundSystem* pSoundSystem = gEnv->pSoundSystem;
	if (!pSoundSystem)
	{
		return;
	}

	const auto it = m_params.find(className);
	if (it == m_params.end())
	{
		return;
	}

	if (it->second.soundCached)
	{
		return;
	}

	it->second.soundCached = true;

	const IItemParamsNode* root = this->GetParamsRoot(it->second);
	if (!root)
	{
		return;
	}

	const IItemParamsNode* actions = root->GetChild("actions");
	if (!actions)
	{
		return;
	}

	const int actionsCount = actions->GetChildCount();

	for (int i = 0; i < actionsCount; i++)
	{
		const IItemParamsNode* action = actions->GetChild(i);
		const int childCount = action->GetChildCount();

		for (int j = 0; j < childCount; j++)
		{
			const IItemParamsNode* child = action->GetChild(j);

			if (IsEqualNoCase(child->GetName(), "sound"))
			{
				pSoundSystem->Precache(child->GetNameAttribute(), 0, FLAG_SOUND_PRECACHE_LOAD_GROUP);
			}
		}
	}
}

void ItemSystem::ClearSoundCache()
{
	for (auto& [name, params] : m_params)
	{
		params.soundCached = false;
	}
}

void ItemSystem::Serialize(TSerialize ser)
{
	if (ser.GetSerializationTarget() == eST_Network)
	{
		return;
	}

	if (ser.IsReading())
	{
		m_playerLevelToLevelData = XmlNodeRef();
	}

	bool hasInventoryNode = static_cast<bool>(m_playerLevelToLevelData);
	ser.Value("hasInventoryNode", hasInventoryNode);

	if (!hasInventoryNode)
	{
		return;
	}

	CryStringT<char> inventoryData;

	if (ser.IsWriting())
	{
		using SmartXmlData = std::unique_ptr<IXmlStringData, Releaser<IXmlStringData>>;
		inventoryData = SmartXmlData(m_playerLevelToLevelData->getXMLData(50'000))->GetString();
	}

	ser.Value("LTLInventoryData", inventoryData);

	if (ser.IsReading())
	{
		m_playerLevelToLevelData = gEnv->pSystem->LoadXmlFromString(inventoryData.c_str());
	}
}

EntityId ItemSystem::GiveItem(IActor* pActor, const char* item, bool sound, bool select, bool keepHistory)
{
	if (!pActor || !item)
	{
		return 0;
	}

	if (!gEnv->bServer)
	{
		CryLogError("[ItemSystem] %s spawn failed: We are not a server!", item);
		return 0;
	}

	if (gEnv->pSystem->IsSerializingFile())
	{
		CryLogError("[ItemSystem] %s spawn failed: Serialization in progress!", item);
		return 0;
	}

	IEntityClass* pItemClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(item);
	if (!pItemClass)
	{
		CryLogError("[ItemSystem] %s spawn failed: Unknown item!", item);
		return 0;
	}

	this->SetSpawnName(item);

	SEntitySpawnParams params;
	params.sName = m_spawnName.c_str();
	params.pClass = pItemClass;
	params.nFlags = ENTITY_FLAG_NO_PROXIMITY;

	IEntity* pItemEntity = gEnv->pEntitySystem->SpawnEntity(params, true);
	if (!pItemEntity)
	{
		return 0;
	}

	const EntityId itemId = pItemEntity->GetId();

	IItem* pItem = this->GetItem(itemId);
	if (!pItem)
	{
		return 0;
	}

	pItem->PickUp(pActor->GetEntityId(), sound, select, keepHistory);

	IAIObject* pActorAI = pActor->GetEntity()->GetAI();
	if (pActorAI)
	{
		gEnv->pAISystem->SendSignal(0, 0, "OnUpdateItems", pActorAI, nullptr);
	}

	pItemEntity = gEnv->pEntitySystem->GetEntity(itemId);
	if (!pItemEntity || pItemEntity->IsGarbage())
	{
		return 0;
	}

	IScriptTable* pScriptTable = pItemEntity->GetScriptTable();
	if (pScriptTable)
	{
		pScriptTable->SetToNull("Properties");
	}

	return pItemEntity->GetId();
}

void ItemSystem::SetActorItem(IActor* pActor, EntityId itemId, bool keepHistory)
{
	if (!pActor)
	{
		return;
	}

	IInventory* pInventory = pActor->GetInventory();
	if (!pInventory)
	{
		return;
	}

	EntityId currentItemId = pInventory->GetCurrentItem();
	if (currentItemId == itemId)
	{
		return;
	}

	if (currentItemId)
	{
		IItem* pCurrentItem = this->GetItem(currentItemId);
		if (pCurrentItem)
		{
			pInventory->SetCurrentItem(0);
			pCurrentItem->Select(false);

			if (keepHistory)
			{
				pInventory->SetLastItem(currentItemId);
			}
		}
	}

	IItem* pItem = this->GetItem(itemId);

	for (IItemSystemListener* pListener : m_listeners)
	{
		pListener->OnSetActorItem(pActor, pItem);
	}

	if (!pItem)
	{
		return;
	}

	pInventory->SetCurrentItem(itemId);
	pItem->SetHand(IItem::eIH_Right);
	pItem->Select(true);

	const GameplayEvent event(eGE_ItemSelected, nullptr, 0.0f, reinterpret_cast<void*>(itemId));

	m_pGameFramework->GetIGameplayRecorder()->Event(pActor->GetEntity(), event);
}

void ItemSystem::SetActorItem(IActor* pActor, const char* name, bool keepHistory)
{
	if (!pActor || !name)
	{
		return;
	}

	IInventory* pInventory = pActor->GetInventory();
	if (!pInventory)
	{
		return;
	}

	IEntityClass* pItemClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(name);
	if (!pItemClass)
	{
		return;
	}

	this->SetActorItem(pActor, pInventory->GetItemByClass(pItemClass), keepHistory);
}

void ItemSystem::DropActorItem(IActor* pActor, EntityId itemId)
{
	if (!pActor)
	{
		return;
	}

	IInventory* pInventory = pActor->GetInventory();
	if (!pInventory)
	{
		return;
	}

	IItem* pItem = this->GetItem(itemId);
	if (!pItem)
	{
		return;
	}

	for (IItemSystemListener* pListener : m_listeners)
	{
		pListener->OnDropActorItem(pActor, pItem);
	}
}

void ItemSystem::SetActorAccessory(IActor* pActor, EntityId itemId, bool keepHistory)
{
	if (!pActor)
	{
		return;
	}

	IInventory* pInventory = pActor->GetInventory();
	if (!pInventory)
	{
		return;
	}

	IItem* pItem = this->GetItem(itemId);

	for (IItemSystemListener* pListener : m_listeners)
	{
		pListener->OnSetActorAccessory(pActor, pItem);
	}
}

void ItemSystem::DropActorAccessory(IActor* pActor, EntityId itemId)
{
	if (!pActor)
	{
		return;
	}

	IInventory* pInventory = pActor->GetInventory();
	if (!pInventory)
	{
		return;
	}

	IItem* pItem = this->GetItem(itemId);
	if (!pItem)
	{
		return;
	}

	for (IItemSystemListener* pListener : m_listeners)
	{
		pListener->OnDropActorAccessory(pActor, pItem);
	}
}

void ItemSystem::RegisterListener(IItemSystemListener* pListener)
{
	if (!pListener)
	{
		return;
	}

	m_listeners.push_back(pListener);
}

void ItemSystem::UnregisterListener(IItemSystemListener* pListener)
{
	if (!pListener)
	{
		return;
	}

	std::erase(m_listeners, pListener);
}

IEquipmentManager* ItemSystem::GetIEquipmentManager()
{
	return m_pEquipmentManager.get();
}

////////////////////////////////////////////////////////////////////////////////

void ItemSystem::SerializePlayerLTLInfo(bool reading)
{
	if (reading && !m_playerLevelToLevelData)
	{
		return;
	}

	if (gEnv->bMultiplayer)
	{
		return;
	}

	if (!reading)
	{
		m_playerLevelToLevelData = gEnv->pSystem->CreateXmlNode("Inventory");
	}

	using SmartXmlSerializer = std::unique_ptr<IXmlSerializer, Releaser<IXmlSerializer>>;
	SmartXmlSerializer pXmlSerializer(gEnv->pSystem->GetXmlUtils()->CreateXmlSerializer());
	TSerialize ser(reading ?
		pXmlSerializer->GetReader(m_playerLevelToLevelData) :
		pXmlSerializer->GetWriter(m_playerLevelToLevelData));

	if (reading)
	{
		m_pEquipmentManager->OnBeginGiveEquipmentPack();
	}

	IActor* pActor = m_pGameFramework->GetClientActor();
	if (pActor)
	{
		pActor->SerializeLevelToLevel(ser);
	}

	if (reading)
	{
		m_pEquipmentManager->OnEndGiveEquipmentPack();
	}
}

const IItemParamsNode* ItemSystem::GetParamsRoot(const ItemParams& params) const
{
	if (m_multiplayer && params.root_multiplayer)
	{
		return params.root_multiplayer.get();
	}
	else
	{
		return params.root.get();
	}
}

void ItemSystem::SetSpawnName(std::string_view name)
{
	// more efficient than "%s%.03d" in the original implementation

	unsigned int number = ++m_spawnCount;
	unsigned int digitCount = DigitCount(number);

	if (digitCount < 3)
	{
		// Parachute001, Parachute002, ...
		digitCount = 3;
	}

	m_spawnName.assign(name);
	m_spawnName.resize(name.length() + digitCount);

	const auto endIt = m_spawnName.rbegin() + digitCount;
	for (auto it = m_spawnName.rbegin(); it != endIt; ++it)
	{
		*it = '0' + static_cast<char>(number % 10);
		number /= 10;
	}
}

void ItemSystem::RegisterItemClass(const ItemClassData& item)
{
	const auto factoryIt = m_factories.find(item.factory);
	if (factoryIt == m_factories.end())
	{
		CryLogErrorAlways("[ItemSystem] Unknown factory '%s' of item class '%s'", item.factory, item.name);
		return;
	}

	const auto [it, added] = m_params.try_emplace(item.name);

	ItemParams& params = it->second;

	if (added)
	{
		IGameObjectExtensionCreatorBase* pCreator = factoryIt->second.pCreator;

		IEntityClassRegistry::SEntityClassDesc entityClass;
		entityClass.flags = 0;
		entityClass.sName = item.name;
		entityClass.sScriptFile = item.script;
		entityClass.pUserProxyCreateFunc = reinterpret_cast<IEntityClass::UserProxyCreateFunc>(pCreator);
		entityClass.pUserProxyData = nullptr;

		if (item.invisible)
		{
			entityClass.flags |= ECLF_INVISIBLE;
		}

		if (!entityClass.sScriptFile)
		{
			const char* defaultScript = "Scripts/Entities/Items/Item.lua";

			IScriptSystem* pScriptSystem = gEnv->pScriptSystem;

			pScriptSystem->ExecuteFile(defaultScript);
			pScriptSystem->BeginCall("CreateItemTable");
			pScriptSystem->PushFuncParam(item.name);
			pScriptSystem->EndCall();

			entityClass.sScriptFile = defaultScript;
		}

		if (!m_reloading)
		{
			m_pGameFramework->GetIGameObjectSystem()->RegisterExtension(item.name, pCreator, &entityClass);
		}
	}

	if (item.multiplayer)
	{
		params.root_multiplayer = SmartParamsNode(item.params);
	}
	else
	{
		params.root = SmartParamsNode(item.params);

		params.category = params.root->GetAttribute("category");
		params.root->GetAttribute("priority", params.priority);
		params.root->GetAttribute("uniqueId", params.uniqueId);
	}
}

void ItemSystem::RegisterCVars()
{
	IConsole* pConsole = gEnv->pConsole;

	m_pPrecacheCVar = pConsole->RegisterInt("i_precache", 1, VF_DUMPTODISK,
		"Enables precaching of items during level loading.");
	m_pNoWeaponLimitCVar = pConsole->RegisterInt("i_noweaponlimit", 0, VF_CHEAT,
		"Player can carry all weapons he wants!");
	m_pLyingItemLimitCVar = pConsole->RegisterInt("i_lying_item_limit", 64, 0,
		"Max number of items lying around in a level. Only works in multiplayer.");

	pConsole->AddCommand("i_giveitem", &ItemSystem::GiveItemCmd, VF_CHEAT,
		"Gives specified item to the player!");
	pConsole->AddCommand("i_giveallitems", &ItemSystem::GiveAllItemsCmd, VF_CHEAT,
		"Gives all available items to the player!");
	pConsole->AddCommand("i_givedebugitems", &ItemSystem::GiveDebugItemsCmd, VF_CHEAT,
		"Gives special debug items to the player!");
}

void ItemSystem::UnregisterCVars()
{
	IConsole* pConsole = gEnv->pConsole;

	m_pPrecacheCVar->Release();
	m_pNoWeaponLimitCVar->Release();
	m_pLyingItemLimitCVar->Release();

	pConsole->RemoveCommand("i_giveitem");
	pConsole->RemoveCommand("i_giveallitems");
	pConsole->RemoveCommand("i_givedebugitems");
}

void ItemSystem::GiveItemCmd(IConsoleCmdArgs* args)
{
	// TODO
	CryLogErrorAlways("[ItemSystem::GiveItemCmd] NOT IMPLEMENTED !!!");
}

void ItemSystem::GiveAllItemsCmd(IConsoleCmdArgs* args)
{
	// TODO
	CryLogErrorAlways("[ItemSystem::GiveAllItemsCmd] NOT IMPLEMENTED !!!");
}

void ItemSystem::GiveDebugItemsCmd(IConsoleCmdArgs* args)
{
	// TODO
	CryLogErrorAlways("[ItemSystem::GiveDebugItemsCmd] NOT IMPLEMENTED !!!");
}
