#pragma once

#include "CryCommon/CryAction/IActorSystem.h"

struct ISystem;
struct IEntitySystem;

class ActorSystem : public IActorSystem
{
#ifdef BUILD_64BIT
	unsigned char m_data[0xa0 - 0x8] = {};
#else
	unsigned char m_data[0x54 - 0x4] = {};
#endif

public:
	explicit ActorSystem(ISystem* pSystem, IEntitySystem* pEntitySystem);
	virtual ~ActorSystem();

	////////////////////////////////////////////////////////////////////////////////
	// IActorSystem
	////////////////////////////////////////////////////////////////////////////////

	void Reset() override;
	IActor* GetActor(EntityId entityId) override;
	IActor* GetActorByChannelId(std::uint16_t channelId) override;
	IActor* CreateActor(
		std::uint16_t channelId,
		const char* name,
		const char* actorClass,
		const Vec3& pos,
		const Quat& rot,
		const Vec3& scale,
		EntityId id) override;

	int GetActorCount() const override;
	IActorIteratorPtr CreateActorIterator() override;

	void SetDemoPlaybackMappedOriginalServerPlayer(EntityId id) override;
	EntityId GetDemoPlaybackMappedOriginalServerPlayer() const override;
	void SwitchDemoSpectator(EntityId id) override;
	IActor* GetCurrentDemoSpectator() override;
	IActor* GetOriginalDemoSpectator() override;

	void AddActor(EntityId entityId, IActor* pActor) override;
	void RemoveActor(EntityId entityId) override;

	////////////////////////////////////////////////////////////////////////////////
};
