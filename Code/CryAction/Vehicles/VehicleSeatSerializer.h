#pragma once

#include "CryCommon/CryAction/IGameObject.h"

class VehicleSeatSerializer : public CGameObjectExtensionHelper<VehicleSeatSerializer, IGameObjectExtension>
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x30 - sizeof(IGameObjectExtension)] = {};
#else
	unsigned char m_data[0x18 - sizeof(IGameObjectExtension)] = {};
#endif

public:
	VehicleSeatSerializer();
	virtual ~VehicleSeatSerializer();

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
	static_assert(sizeof(VehicleSeatSerializer) == 0x30);
#else
	static_assert(sizeof(VehicleSeatSerializer) == 0x18);
#endif
