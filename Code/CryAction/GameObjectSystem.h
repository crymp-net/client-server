#pragma once

#include "CryCommon/CryAction/IGameObjectSystem.h"

class GameObjectSystem : public IGameObjectSystem
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x170 - 0x8] = {};
#else
	unsigned char m_data[0xbc - 0x4] = {};
#endif

public:
	GameObjectSystem();

	bool Init();

	void RegisterFactories(IGameFramework* pGameFramework);

	////////////////////////////////////////////////////////////////////////////////
	// IGameObjectSystem
	////////////////////////////////////////////////////////////////////////////////

	ExtensionID GetID(const char* name) override;
	const char* GetName(ExtensionID id) override;
	IGameObjectExtension* Instantiate(ExtensionID id, IGameObject* pObject) override;
	void BroadcastEvent(const SGameObjectEvent& evt) override;

	void RegisterEvent(uint32 id, const char* name) override;
	uint32 GetEventID(const char* name) override;
	const char* GetEventName(uint32 id) override;

	IGameObject* CreateGameObjectForEntity(EntityId entityId) override;

	void RegisterExtension(const char* name, IGameObjectExtensionCreatorBase* pCreator, IEntityClassRegistry::SEntityClassDesc* pEntityCls) override;
	void DefineProtocol(bool server, IProtocolBuilder* pBuilder) override;

	void PostUpdate(float frameTime) override;
	void SetPostUpdate(IGameObject* pGameObject, bool enable) override;

	void SetSpawnSerializer(TSerialize* serializer) override;

	////////////////////////////////////////////////////////////////////////////////
};
