#pragma once

#include "CryCommon/CryAction/IVehicleSystem.h"

#include "Library/StlportVector.h"

#include "VehicleAnimation.h"
#include "VehicleComponent.h"
#include "VehicleSeat.h"
#include "VehicleSeatGroup.h"

struct IInventory;

class Vehicle : public IVehicle, public IGameObjectProfileManager
{
protected:
	void* m_reserved1[13] = {};
	unsigned int m_reserved2[8] = {};
	IVehicleSystem* m_pVehicleSystem = nullptr;                                   // 0xb0, 0x68
	IInventory* m_pInventory = nullptr;                                           // 0xb8, 0x6c
	SEntityPhysicalizeParams m_physicalizeParams;                                 // 0xc0, 0x70
	IVehicleMovement* m_pMovement = nullptr;                                      // 0x120, 0xb0
	unsigned int m_reserved3[13] = {};
	bool m_reserved4 = false;
	bool m_reserved5 = false;
	bool m_engineSlotBySpeed = false;                                             // 0x15e, 0xea
	unsigned int m_reserved6[2] = {};
	StlportVector<std::pair<CryStringT<char>, VehicleSeat*>> m_seats;             // 0x168, 0xf4
	StlportVector<VehicleSeatGroup*> m_seatGroups;                                // 0x180, 0x100
	DynArray<DynArray<TVehicleSeatId>> m_seatTransitions;                         // 0x188, 0x104
	StlportVector<VehicleComponent*> m_components;                                // 0x1a0, 0x110
	StlportVector<std::pair<CryStringT<char>, IVehiclePart*>> m_parts;            // 0x1b8, 0x11c
	void* m_reserved7[12] = {};
	pe_params_buoyancy m_buoyancy;                                                // 0x230, 0x158
	pe_simulation_params m_physics;                                               // 0x278, 0x1a0
	pe_params_flags m_physicsParams;                                              // 0x2c4, 0x1ec
	bool m_isDestroyed = false;                                                   // 0x2d4, 0x1fc
	float m_mass = 0;                                                             // 0x2d8, 0x200
	void* m_reserved8[29] = {};
	unsigned int m_reserved9[21] = {};
	StlportVector<std::pair<CryStringT<char>, IVehicleAnimation*>> m_animations;  // 0x420, 0x2cc
	void* m_reserved10[2] = {};
	unsigned int m_reserved11 = 0;
	IEntitySoundProxy* m_pSoundProxy = nullptr;                                   // 0x450, 0x2e4
	void* m_reserved12[3] = {};
	bool m_retainGravity = false;                                                 // 0x470, 0x2f4
	Vec3 m_gravity = {};                                                          // 0x474, 0x2f8
	unsigned int m_reserved13[5] = {};
	CryStringT<char> m_modName;                                                   // 0x498, 0x318
	CryStringT<char> m_paintName;                                                 // 0x4a0, 0x31c
	unsigned int m_reserved14 = 0;
	float m_totalMaxDamage = 0;                                                   // 0x4ac, 0x324
	float m_majorComponentMaxDamage = 0;                                          // 0x4b0, 0x328
	unsigned int m_reserved15 = 0;
	CryStringT<char> m_actionMapName;                                             // 0x4b8, 0x330
	void* m_reserved16[18] = {};

public:
	Vehicle();
	~Vehicle();

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

	void NeedsUpdate(int flags, bool threadSafe) override;

	int SetTimer(int timerId, int ms, IVehicleObject* pObject) override;
	int KillTimer(int timerId) override;

	IActor* GetDriver() override;

	void SetVehicleGlassSurfaceType(ISurfaceType* pSurface) override;

	bool GetExitPositionForActor(IActor* pActor, Vec3& pos, bool extended) override;

	int GetSkipEntities(IPhysicalEntity** pSkipEnts, int nMaxSkip) override;

	const char* GetModification() const override;

	////////////////////////////////////////////////////////////////////////////////
	// IGameObjectProfileManager
	////////////////////////////////////////////////////////////////////////////////

	bool SetAspectProfile(EEntityAspects aspect, std::uint8_t profile) override;
	std::uint8_t GetDefaultProfile(EEntityAspects aspect) override;

	////////////////////////////////////////////////////////////////////////////////

	static void GetGameObjectExtensionRMIData(void** ppRMI, size_t* count);

protected:
	void PatchVTable();

	using ComponentParts = StlportVector<std::pair<IVehiclePart*, CryStringT<char>>>;

	void InitActions(const SmartScriptTable& table);
	void InitDamages(const SmartScriptTable& table);
	void InitParticles(const SmartScriptTable& table);
	void InitHelpers();
	void InitMaxDamage();
	void InitSeatTransitions();
	void LoadParts(const SmartScriptTable& parts, IVehiclePart* parent, ComponentParts& componentParts);
	void RegisterComponentParts(const ComponentParts& componentParts);
	bool SetMovement(const char* name, const SmartScriptTable& movement);
	void SetPaintMaterial(const char* name);
	void SetAmmoCapacity(const char* name, int capacity);
	void AttachScriptBindToSeats();
	bool BindVehicleToNetwork();

private:
	void InitRespawn();
	void InitModName();
	void InitPaintName();
};
