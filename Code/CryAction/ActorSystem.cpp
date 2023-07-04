#include <cstdint>

#include "ActorSystem.h"

ActorSystem::ActorSystem(ISystem* pSystem, IEntitySystem* pEntitySystem)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30510ee0;
#else
	std::uintptr_t ctor = 0x30510660;
#endif

	(this->*reinterpret_cast<void(ActorSystem::*&)(ISystem*, IEntitySystem*)>(ctor))(pSystem, pEntitySystem);
}

ActorSystem::~ActorSystem()
{
}

////////////////////////////////////////////////////////////////////////////////
// IActorSystem
////////////////////////////////////////////////////////////////////////////////

void ActorSystem::Reset()
{
}

IActor* ActorSystem::GetActor(EntityId entityId)
{
	return nullptr;
}

IActor* ActorSystem::GetActorByChannelId(std::uint16_t channelId)
{
	return nullptr;
}

IActor* ActorSystem::CreateActor(
	std::uint16_t channelId,
	const char* name,
	const char* actorClass,
	const Vec3& pos,
	const Quat& rot,
	const Vec3& scale,
	EntityId id)
{
	return nullptr;
}

int ActorSystem::GetActorCount() const
{
	return 0;
}

IActorIteratorPtr ActorSystem::CreateActorIterator()
{
	return {};
}

void ActorSystem::SetDemoPlaybackMappedOriginalServerPlayer(EntityId id)
{
}

EntityId ActorSystem::GetDemoPlaybackMappedOriginalServerPlayer() const
{
	return 0;
}

void ActorSystem::SwitchDemoSpectator(EntityId id)
{
}

IActor* ActorSystem::GetCurrentDemoSpectator()
{
	return nullptr;
}

IActor* ActorSystem::GetOriginalDemoSpectator()
{
	return nullptr;
}

void ActorSystem::AddActor(EntityId entityId, IActor* pActor)
{
}

void ActorSystem::RemoveActor(EntityId entityId)
{
}

////////////////////////////////////////////////////////////////////////////////
