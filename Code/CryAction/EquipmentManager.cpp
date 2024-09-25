#include <cstdint>

#include "EquipmentManager.h"

extern std::uintptr_t CRYACTION_BASE;

EquipmentManager::EquipmentManager(ItemSystem* pItemSystem)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x1e8a0;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x1ae40;
#endif

	(this->*reinterpret_cast<void(EquipmentManager::*&)(ItemSystem*)>(ctor))(pItemSystem);
}

void EquipmentManager::OnBeginGiveEquipmentPack()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x1d690;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x1a470;
#endif

	(this->*reinterpret_cast<void(EquipmentManager::*&)()>(func))();
}

void EquipmentManager::OnEndGiveEquipmentPack()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x1d6d0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x1a4a0;
#endif

	(this->*reinterpret_cast<void(EquipmentManager::*&)()>(func))();
}

////////////////////////////////////////////////////////////////////////////////
// IEquipmentManager
////////////////////////////////////////////////////////////////////////////////

void EquipmentManager::DeleteAllEquipmentPacks()
{
}

void EquipmentManager::LoadEquipmentPacks(const XmlNodeRef& rootNode)
{
}

void EquipmentManager::LoadEquipmentPacksFromPath(const char* path)
{
}

bool EquipmentManager::LoadEquipmentPack(const XmlNodeRef& rootNode, bool overrideExisiting)
{
	return {};
}

bool EquipmentManager::GiveEquipmentPack(IActor* pActor, const char* packName, bool add, bool selectPrimary)
{
	return {};
}

IEquipmentManager::IEquipmentPackIteratorPtr EquipmentManager::CreateEquipmentPackIterator()
{
	return {};
}

void EquipmentManager::RegisterListener(IListener* pListener)
{
}

void EquipmentManager::UnregisterListener(IListener* pListener)
{
}

////////////////////////////////////////////////////////////////////////////////
