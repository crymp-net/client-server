#include "Vehicle.h"

Vehicle::Vehicle()
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x3057f340;
#else
	std::uintptr_t ctor = 0x305591b0;
#endif

	(this->*reinterpret_cast<void(Vehicle::*&)()>(ctor))();
}

////////////////////////////////////////////////////////////////////////////////
// IGameObjectExtension
////////////////////////////////////////////////////////////////////////////////

bool Vehicle::Init(IGameObject* pGameObject)
{
	return {};
}

void Vehicle::PostInit(IGameObject* pGameObject)
{
}

void Vehicle::InitClient(int channelId)
{
}

void Vehicle::PostInitClient(int channelId)
{
}

void Vehicle::Release()
{
}

void Vehicle::FullSerialize(TSerialize ser)
{
}

bool Vehicle::NetSerialize(TSerialize ser, EEntityAspects aspect, std::uint8_t profile, int flags)
{
	return {};
}

void Vehicle::PostSerialize()
{
}

void Vehicle::SerializeSpawnInfo(TSerialize ser)
{
}

ISerializableInfoPtr Vehicle::GetSpawnInfo()
{
	return {};
}

void Vehicle::Update(SEntityUpdateContext& context, int updateSlot)
{
}

void Vehicle::HandleEvent(const SGameObjectEvent& event)
{
}

void Vehicle::ProcessEvent(SEntityEvent& event)
{
}

void Vehicle::GetMemoryStatistics(ICrySizer* s)
{
}

void Vehicle::SetChannelId(std::uint16_t id)
{
}

void Vehicle::SetAuthority(bool auth)
{
}

const void* Vehicle::GetRMIBase() const
{
	return {};
}

void Vehicle::PostUpdate(float frameTime)
{
}

void Vehicle::PostRemoteSpawn()
{
}

////////////////////////////////////////////////////////////////////////////////
// IVehicle
////////////////////////////////////////////////////////////////////////////////

void Vehicle::Reset(bool enterGame)
{
}

SEntityPhysicalizeParams& Vehicle::GetPhysicsParams()
{
	static SEntityPhysicalizeParams params{};
	return params;
}

const SVehicleStatus& Vehicle::GetStatus()
{
	static const SVehicleStatus status{};
	return status;
}

void Vehicle::UpdateView(SViewParams& viewParams, EntityId playerId)
{
}

void Vehicle::OnAction(const TVehicleActionId actionId, int activationMode, float value, EntityId callerId)
{
}

void Vehicle::OnHit(EntityId targetId, EntityId shooterId, float damage, Vec3 position, float radius, const char* pHitClass, bool explosion)
{
}

float Vehicle::GetDamageRatio(bool onlyMajorComponents) const
{
	return {};
}

void Vehicle::SetAmmoCount(IEntityClass* pAmmoType, int amount)
{
}

int Vehicle::GetAmmoCount(IEntityClass* pAmmoType) const
{
	return {};
}

int Vehicle::GetAmmoCapacity(IEntityClass* pAmmoType) const
{
	return {};
}

void Vehicle::SetOwnerId(EntityId ownerId)
{
}

EntityId Vehicle::GetOwnerId() const
{
	return {};
}

int Vehicle::IsUsable(EntityId userId)
{
	return {};
}

bool Vehicle::OnUsed(EntityId userId, int index)
{
	return {};
}

bool Vehicle::AddSeat(const SmartScriptTable& seatParams)
{
	return {};
}

bool Vehicle::AddHelper(const char* name, Vec3 position, Vec3 direction, IVehiclePart* pPart)
{
	return {};
}

int Vehicle::GetPartCount()
{
	return {};
}

IVehiclePart* Vehicle::GetPart(unsigned int index)
{
	return {};
}

void Vehicle::GetParts(IVehiclePart** parts, int nMax)
{
}

int Vehicle::GetComponentCount() const
{
	return {};
}

IVehicleComponent* Vehicle::GetComponent(int index)
{
	return {};
}

IVehicleComponent* Vehicle::GetComponent(const char* name)
{
	return {};
}

IVehicleAnimation* Vehicle::GetAnimation(const char* name)
{
	return {};
}

int Vehicle::GetActionCount()
{
	return {};
}

IVehicleAction* Vehicle::GetAction(int index)
{
	return {};
}

IMovementController* Vehicle::GetMovementController()
{
	return {};
}

IFireController* Vehicle::GetFireController()
{
	return {};
}

TVehicleSeatId Vehicle::GetLastSeatId()
{
	return {};
}

IVehicleSeat* Vehicle::GetSeatById(const TVehicleSeatId seatId)
{
	return {};
}

unsigned int Vehicle::GetSeatCount()
{
	return {};
}

IVehicleSeat* Vehicle::GetSeatForPassenger(EntityId passengerId)
{
	return {};
}

TVehicleSeatId Vehicle::GetSeatId(const char* pSeatName)
{
	return {};
}

bool Vehicle::IsPlayerDriving(bool clientOnly)
{
	return {};
}

bool Vehicle::HasFriendlyPassenger(IEntity* pPlayer)
{
	return {};
}

bool Vehicle::IsPlayerPassenger()
{
	return {};
}

int Vehicle::GetWeaponCount() const
{
	return {};
}

EntityId Vehicle::GetWeaponId(int index) const
{
	return {};
}

EntityId Vehicle::GetCurrentWeaponId(EntityId passengerId, bool secondary)
{
	return {};
}

bool Vehicle::HasHelper(const char* pName)
{
	return {};
}

IVehiclePart* Vehicle::GetPart(const char* name)
{
	return {};
}

IVehiclePart* Vehicle::GetWeaponParentPart(EntityId weaponId)
{
	return {};
}

IVehicleSeat* Vehicle::GetWeaponParentSeat(EntityId weaponId)
{
	return {};
}

IVehicleMovement* Vehicle::GetMovement()
{
	return {};
}

int Vehicle::GetWheelCount()
{
	return {};
}

IVehiclePart* Vehicle::GetWheelPart(int idx)
{
	return {};
}

float Vehicle::GetMass()
{
	return {};
}

float Vehicle::GetAltitude()
{
	return {};
}

IMovementController* Vehicle::GetPassengerMovementController(EntityId passenger)
{
	return {};
}

void Vehicle::BroadcastVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
}

void Vehicle::RegisterVehicleEventListener(IVehicleEventListener* pEvenListener, const char* name)
{
}

void Vehicle::UnregisterVehicleEventListener(IVehicleEventListener* pEvenListener)
{
}

SParticleParams* Vehicle::GetParticleParams()
{
	return {};
}

const SVehicleDamageParams& Vehicle::GetDamageParams() const
{
	static const SVehicleDamageParams params{};
	return params;
}

void Vehicle::SetObjectUpdate(IVehicleObject* pObject, EVehicleObjectUpdate updatePolicy)
{
}

IVehicleHelper* Vehicle::GetHelper(const char* pName)
{
	return {};
}

bool Vehicle::IsDestroyed()
{
	return {};
}

bool Vehicle::IsFlipped(float maxSpeed)
{
	return {};
}

void Vehicle::TriggerEngineSlotBySpeed(bool trigger)
{
}

void Vehicle::NeedsUpdate(int flags, bool bThreadSafe)
{
}

int Vehicle::SetTimer(int timerId, int ms, IVehicleObject* pObject)
{
	return {};
}

int Vehicle::KillTimer(int timerId)
{
	return {};
}

IActor* Vehicle::GetDriver()
{
	return {};
}

void Vehicle::SetVehicleGlassSurfaceType(ISurfaceType* pSurface)
{
}

bool Vehicle::GetExitPositionForActor(IActor* pActor, Vec3& pos, bool extended)
{
	return {};
}

int Vehicle::GetSkipEntities(IPhysicalEntity** pSkipEnts, int nMaxSkip)
{
	return {};
}

const char* Vehicle::GetModification() const
{
	return {};
}

////////////////////////////////////////////////////////////////////////////////

void Vehicle::GetGameObjectExtensionRMIData(void** ppRMI, size_t* count)
{
	// SvRequestUse
	// SvRequestChangeSeat
	// SvRequestLeave
	// ClSetupWeapons
	// ClAmmoCounts
	// ClSetAmmo
	// ClAbandonWarning

#ifdef BUILD_64BIT
	void* pVehicleMessages = reinterpret_cast<void*>(0x30922fa0);
#else
	void* pVehicleMessages = reinterpret_cast<void*>(0x307a47b8);
#endif

	*ppRMI = pVehicleMessages;
	*count = 7;
}
