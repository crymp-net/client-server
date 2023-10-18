#include <cstdint>

#include "GameObjectSystem.h"

GameObjectSystem::GameObjectSystem()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30827630;
#else
	std::uintptr_t ctor = 0x30749040;
#endif

	(this->*reinterpret_cast<void(GameObjectSystem::*&)()>(ctor))();
}

bool GameObjectSystem::Init()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x306b3360;
#else
	std::uintptr_t func = 0x3062e080;
#endif

	return (this->*reinterpret_cast<bool(GameObjectSystem::*&)()>(func))();
}

void GameObjectSystem::RegisterFactories(IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x306b3e60;
#else
	std::uintptr_t func = 0x3062f040;
#endif

	(this->*reinterpret_cast<void(GameObjectSystem::*&)(IGameFramework*)>(func))(pGameFramework);
}

////////////////////////////////////////////////////////////////////////////////
// IGameObjectSystem
////////////////////////////////////////////////////////////////////////////////

IGameObjectSystem::ExtensionID GameObjectSystem::GetID(const char* name)
{
	return {};
}

const char* GameObjectSystem::GetName(ExtensionID id)
{
	return {};
}

IGameObjectExtension* GameObjectSystem::Instantiate(ExtensionID id, IGameObject* pObject)
{
	return {};
}

void GameObjectSystem::BroadcastEvent(const SGameObjectEvent& evt)
{
}

void GameObjectSystem::RegisterEvent(uint32 id, const char* name)
{
}

uint32 GameObjectSystem::GetEventID(const char* name)
{
	return {};
}

const char* GameObjectSystem::GetEventName(uint32 id)
{
	return {};
}

IGameObject* GameObjectSystem::CreateGameObjectForEntity(EntityId entityId)
{
	return {};
}

void GameObjectSystem::RegisterExtension(const char* name, IGameObjectExtensionCreatorBase* pCreator, IEntityClassRegistry::SEntityClassDesc* pEntityCls)
{
}

void GameObjectSystem::DefineProtocol(bool server, IProtocolBuilder* pBuilder)
{
}

void GameObjectSystem::PostUpdate(float frameTime)
{
}

void GameObjectSystem::SetPostUpdate(IGameObject* pGameObject, bool enable)
{
}

void GameObjectSystem::SetSpawnSerializer(TSerialize* serializer)
{
}

////////////////////////////////////////////////////////////////////////////////
