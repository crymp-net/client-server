#pragma once

#include "CryCommon/CryAction/IItemSystem.h"

class ItemSystem;

class EquipmentManager : public IEquipmentManager
{
	void* m_reserved[8 - 1] = {};

public:
	explicit EquipmentManager(ItemSystem* pItemSystem);

	void OnBeginGiveEquipmentPack();
	void OnEndGiveEquipmentPack();

	////////////////////////////////////////////////////////////////////////////////
	// IEquipmentManager
	////////////////////////////////////////////////////////////////////////////////

	void DeleteAllEquipmentPacks() override;

	void LoadEquipmentPacks(const XmlNodeRef& rootNode) override;
	void LoadEquipmentPacksFromPath(const char* path) override;
	bool LoadEquipmentPack(const XmlNodeRef& rootNode, bool overrideExisiting) override;

	bool GiveEquipmentPack(IActor* pActor, const char* packName, bool add, bool selectPrimary) override;

	IEquipmentPackIteratorPtr CreateEquipmentPackIterator() override;

	void RegisterListener(IListener* pListener) override;
	void UnregisterListener(IListener* pListener) override;

	////////////////////////////////////////////////////////////////////////////////
};
