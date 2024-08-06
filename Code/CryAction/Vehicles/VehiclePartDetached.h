#pragma once

#include "CryCommon/CryAction/IGameObject.h"

class VehiclePartDetached : public CGameObjectExtensionHelper<VehiclePartDetached, IGameObjectExtension>
{
	float m_reserved = 0;

public:
	VehiclePartDetached();

	////////////////////////////////////////////////////////////////////////////////
	// IGameObjectExtension
	////////////////////////////////////////////////////////////////////////////////

	bool Init(IGameObject* pGameObject) override;
	void PostInit(IGameObject* pGameObject) override;

	void InitClient(int channelId) override;
	void PostInitClient(int channelId) override;

	void Release() override;

	void FullSerialize(TSerialize ser) override;
	bool NetSerialize(TSerialize ser, EEntityAspects aspect, std::uint8_t profile, int flags) override;
	void PostSerialize() override;

	void SerializeSpawnInfo(TSerialize ser) override;
	ISerializableInfoPtr GetSpawnInfo() override;

	void Update(SEntityUpdateContext& context, int updateSlot) override;

	void HandleEvent(const SGameObjectEvent& event) override;

	void ProcessEvent(SEntityEvent& event) override;

	void GetMemoryStatistics(ICrySizer* s) override;

	void SetChannelId(std::uint16_t id) override;
	void SetAuthority(bool auth) override;

	const void* GetRMIBase() const override;

	void PostUpdate(float frameTime) override;

	void PostRemoteSpawn() override;

	////////////////////////////////////////////////////////////////////////////////

	static void Register(IGameFramework* pGameFramework);
};

#ifdef BUILD_64BIT
static_assert(sizeof(VehiclePartDetached) == 0x28);
#else
static_assert(sizeof(VehiclePartDetached) == 0x14);
#endif
