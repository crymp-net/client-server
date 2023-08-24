#pragma once

#include "CryCommon/CryAction/IVehicleSystem.h"

class Vehicle : public IVehicle
{
#ifdef BUILD_64BIT
	unsigned char m_data[0x550 - sizeof(IVehicle)] = {};
#else
	unsigned char m_data[0x37c - sizeof(IVehicle)] = {};
#endif

public:
	Vehicle();

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
	// IVehicle
	////////////////////////////////////////////////////////////////////////////////

	void Reset(bool enterGame) override;

	SEntityPhysicalizeParams& GetPhysicsParams() override;
	const SVehicleStatus& GetStatus() override;

	void UpdateView(SViewParams& viewParams, EntityId playerId) override;

	void OnAction(const TVehicleActionId actionId, int activationMode, float value, EntityId callerId) override;
	void OnHit(EntityId targetId, EntityId shooterId, float damage, Vec3 position, float radius, const char* pHitClass, bool explosion) override;

	float GetDamageRatio(bool onlyMajorComponents) const override;

	void SetAmmoCount(IEntityClass* pAmmoType, int amount) override;
	int GetAmmoCount(IEntityClass* pAmmoType) const override;

	int GetAmmoCapacity(IEntityClass* pAmmoType) const override;

	void SetOwnerId(EntityId ownerId) override;
	EntityId GetOwnerId() const override;

	int IsUsable(EntityId userId) override;
	bool OnUsed(EntityId userId, int index) override;

	bool AddSeat(const SmartScriptTable& seatParams) override;
	bool AddHelper(const char* name, Vec3 position, Vec3 direction, IVehiclePart* pPart) override;

	int GetPartCount() override;
	IVehiclePart* GetPart(unsigned int index) override;
	void GetParts(IVehiclePart** parts, int nMax) override;

	int GetComponentCount() const override;
	IVehicleComponent* GetComponent(int index) override;
	IVehicleComponent* GetComponent(const char* name) override;

	IVehicleAnimation* GetAnimation(const char* name) override;

	int GetActionCount() override;
	IVehicleAction* GetAction(int index) override;

	IMovementController* GetMovementController() override;
	IFireController* GetFireController() override;

	TVehicleSeatId GetLastSeatId() override;
	IVehicleSeat* GetSeatById(const TVehicleSeatId seatId) override;
	unsigned int GetSeatCount() override;
	IVehicleSeat* GetSeatForPassenger(EntityId passengerId) override;
	TVehicleSeatId GetSeatId(const char* pSeatName) override;

	bool IsPlayerDriving(bool clientOnly) override;

	bool HasFriendlyPassenger(IEntity* pPlayer) override;
	bool IsPlayerPassenger() override;

	int GetWeaponCount() const override;
	EntityId GetWeaponId(int index) const override;
	EntityId GetCurrentWeaponId(EntityId passengerId, bool secondary) override;

	bool HasHelper(const char* pName) override;

	IVehiclePart* GetPart(const char* name) override;

	IVehiclePart* GetWeaponParentPart(EntityId weaponId) override;
	IVehicleSeat* GetWeaponParentSeat(EntityId weaponId) override;

	IVehicleMovement* GetMovement() override;

	int GetWheelCount() override;
	IVehiclePart* GetWheelPart(int idx) override;

	float GetMass() override;
	float GetAltitude() override;

	IMovementController* GetPassengerMovementController(EntityId passenger) override;

	void BroadcastVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override;
	void RegisterVehicleEventListener(IVehicleEventListener* pEvenListener, const char* name) override;
	void UnregisterVehicleEventListener(IVehicleEventListener* pEvenListener) override;

	SParticleParams* GetParticleParams() override;
	const SVehicleDamageParams& GetDamageParams() const override;

	void SetObjectUpdate(IVehicleObject* pObject, EVehicleObjectUpdate updatePolicy) override;

	IVehicleHelper* GetHelper(const char* pName) override;

	bool IsDestroyed() override;
	bool IsFlipped(float maxSpeed) override;

	void TriggerEngineSlotBySpeed(bool trigger) override;

	void NeedsUpdate(int flags, bool bThreadSafe) override;

	int SetTimer(int timerId, int ms, IVehicleObject* pObject) override;
	int KillTimer(int timerId) override;

	IActor* GetDriver() override;

	void SetVehicleGlassSurfaceType(ISurfaceType* pSurface) override;

	bool GetExitPositionForActor(IActor* pActor, Vec3& pos, bool extended) override;

	int GetSkipEntities(IPhysicalEntity** pSkipEnts, int nMaxSkip) override;

	const char* GetModification() const override;

	////////////////////////////////////////////////////////////////////////////////

	static void GetGameObjectExtensionRMIData(void** ppRMI, size_t* count);
};

#ifdef BUILD_64BIT
	static_assert(sizeof(Vehicle) == 0x550);
#else
	static_assert(sizeof(Vehicle) == 0x37c);
#endif
