#include <cstdint>

#include "CryCommon/CrySystem/gEnv.h"
#include "CryCommon/CrySystem/IConsole.h"

#include "ItemSystem.h"

ItemSystem::ItemSystem(IGameFramework* pGameFramework, ISystem* pSystem)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x3052e450;
#else
	std::uintptr_t ctor = 0x30522e00;
#endif

	(this->*reinterpret_cast<void(ILevelSystemListener::*&)(IGameFramework*, ISystem*)>(ctor))(pGameFramework, pSystem);

/*
	IConsole* pConsole = gEnv->pConsole;

	pConsole->AddCommand("i_giveitem", &ItemSystem::GiveItemCmd, VF_CHEAT, "Gives specified item to the player!");
	pConsole->AddCommand("i_giveallitems", &ItemSystem::GiveAllItemsCmd, VF_CHEAT, "Gives all available items to the player!");
	pConsole->AddCommand("i_givedebugitems", &ItemSystem::GiveDebugItemsCmd, VF_CHEAT, "Gives special debug items to the player!");
	pConsole->RegisterInt("i_noweaponlimit", 0, VF_CHEAT, "Player can carry all weapons he wants!");
	pConsole->RegisterInt("i_precache", 1, VF_DUMPTODISK, "Enables precaching of items during level loading.");
	pConsole->RegisterInt("i_lying_item_limit", 64, 0, "Max number of items lying around in a level. Only works in multiplayer.");
*/
}

ItemSystem::~ItemSystem()
{
/*
	IConsole* pConsole = gEnv->pConsole;

	pConsole->RemoveCommand("i_giveitem");
	pConsole->RemoveCommand("i_giveallitems");
	pConsole->RemoveCommand("i_givedebugitems");
	pConsole->UnregisterVariable("i_noweaponlimit", true);
	pConsole->UnregisterVariable("i_precache", true);
	pConsole->UnregisterVariable("i_lying_item_limit", true);
*/
}

void ItemSystem::Update()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x30528d90;
#else
	std::uintptr_t func = 0x30520880;
#endif

	(this->*reinterpret_cast<void(ILevelSystemListener::*&)()>(func))();
}

////////////////////////////////////////////////////////////////////////////////
// ILevelSystemListener
////////////////////////////////////////////////////////////////////////////////

void ItemSystem::OnLevelNotFound(const char *levelName)
{
}

void ItemSystem::OnLoadingStart(ILevelInfo *pLevel)
{
}

void ItemSystem::OnLoadingComplete(ILevel *pLevel)
{
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
}

void ItemSystem::Reload()
{
}

void ItemSystem::Scan(const char* folderName)
{
}

IItemParamsNode* ItemSystem::CreateParams()
{
	return nullptr;
}

const IItemParamsNode* ItemSystem::GetItemParams(const char* itemName) const
{
	return nullptr;
}

int ItemSystem::GetItemParamsCount() const
{
	return 0;
}

const char* ItemSystem::GetItemParamName(int index) const
{
	return nullptr;
}

std::uint8_t ItemSystem::GetItemPriority(const char* item) const
{
	return 0;
}

const char* ItemSystem::GetItemCategory(const char* item) const
{
	return nullptr;
}

std::uint8_t ItemSystem::GetItemUniqueId(const char* item) const
{
	return 0;
}

bool ItemSystem::IsItemClass(const char* name) const
{
	return false;
}

void ItemSystem::RegisterForCollection(EntityId itemId)
{
}

void ItemSystem::UnregisterForCollection(EntityId itemId)
{
}

void ItemSystem::AddItem(EntityId itemId, IItem* pItem)
{
}

void ItemSystem::RemoveItem(EntityId itemId)
{
}

IItem* ItemSystem::GetItem(EntityId itemId) const
{
	return nullptr;
}

void ItemSystem::SetConfiguration(const char* name)
{
}

const char* ItemSystem::GetConfiguration() const
{
	return nullptr;
}

ICharacterInstance* ItemSystem::GetCachedCharacter(const char* fileName)
{
	return nullptr;
}

IStatObj* ItemSystem::GetCachedObject(const char* fileName)
{
	return nullptr;
}

void ItemSystem::CacheObject(const char* fileName)
{
}

void ItemSystem::CacheGeometry(const IItemParamsNode* geometry)
{
}

void ItemSystem::CacheItemGeometry(const char* className)
{
}

void ItemSystem::ClearGeometryCache()
{
}

void ItemSystem::CacheItemSound(const char* className)
{
}

void ItemSystem::ClearSoundCache()
{
}

void ItemSystem::Serialize(TSerialize ser)
{
}

EntityId ItemSystem::GiveItem(IActor* pActor, const char* item, bool sound, bool select, bool keepHistory)
{
	return 0;
}

void ItemSystem::SetActorItem(IActor* pActor, EntityId itemId, bool keepHistory)
{
}

void ItemSystem::SetActorItem(IActor* pActor, const char* name, bool keepHistory)
{
}

void ItemSystem::DropActorItem(IActor* pActor, EntityId itemId)
{
}

void ItemSystem::SetActorAccessory(IActor* pActor, EntityId itemId, bool keepHistory)
{
}

void ItemSystem::DropActorAccessory(IActor* pActor, EntityId itemId)
{
}

void ItemSystem::RegisterListener(IItemSystemListener* pListener)
{
}

void ItemSystem::UnregisterListener(IItemSystemListener* pListener)
{
}

IEquipmentManager* ItemSystem::GetIEquipmentManager()
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
