#include <cstdint>

#include "EquipmentManager.h"

EquipmentManager::EquipmentManager(ItemSystem* pItemSystem)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x3051e8a0;
#else
	std::uintptr_t ctor = 0x3051ae40;
#endif

	(this->*reinterpret_cast<void(EquipmentManager::*&)(ItemSystem*)>(ctor))(pItemSystem);
}

void EquipmentManager::OnBeginGiveEquipmentPack()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x3051d690;
#else
	std::uintptr_t func = 0x3051a470;
#endif

	(this->*reinterpret_cast<void(EquipmentManager::*&)()>(func))();
}

void EquipmentManager::OnEndGiveEquipmentPack()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x3051d6d0;
#else
	std::uintptr_t func = 0x3051a4a0;
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
