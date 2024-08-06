#include "CryAction/GameFramework.h"
#include "CryAction/ScriptBind_Vehicle.h"
#include "CryAction/ScriptBind_VehicleSeat.h"
#include "CryCommon/CryAction/IGameRulesSystem.h"
#include "CryCommon/CryAction/IItemSystem.h"
#include "CryCommon/CrySystem/gEnv.h"
#include "CrySystem/CryLog.h"

#include "Vehicle.h"

Vehicle::Vehicle()
{
#ifdef BUILD_64BIT
	static_assert(sizeof(Vehicle) == 0x550);
	static_assert(offsetof(Vehicle, m_pVehicleSystem) == 0xb0);
	static_assert(offsetof(Vehicle, m_pInventory) == 0xb8);
	static_assert(offsetof(Vehicle, m_physicalizeParams) == 0xc0);
	static_assert(offsetof(Vehicle, m_pMovement) == 0x120);
	static_assert(offsetof(Vehicle, m_engineSlotBySpeed) == 0x15e);
	static_assert(offsetof(Vehicle, m_seats) == 0x168);
	static_assert(offsetof(Vehicle, m_seatGroups) == 0x180);
	static_assert(offsetof(Vehicle, m_components) == 0x1a0);
	static_assert(offsetof(Vehicle, m_parts) == 0x1b8);
	static_assert(offsetof(Vehicle, m_buoyancy) == 0x230);
	static_assert(offsetof(Vehicle, m_physics) == 0x278);
	static_assert(offsetof(Vehicle, m_physicsParams) == 0x2c4);
	static_assert(offsetof(Vehicle, m_isDestroyed) == 0x2d4);
	static_assert(offsetof(Vehicle, m_mass) == 0x2d8);
	static_assert(offsetof(Vehicle, m_animations) == 0x420);
	static_assert(offsetof(Vehicle, m_pSoundProxy) == 0x450);
	static_assert(offsetof(Vehicle, m_retainGravity) == 0x470);
	static_assert(offsetof(Vehicle, m_gravity) == 0x474);
	static_assert(offsetof(Vehicle, m_modName) == 0x498);
	static_assert(offsetof(Vehicle, m_paintName) == 0x4a0);
	static_assert(offsetof(Vehicle, m_totalMaxDamage) == 0x4ac);
	static_assert(offsetof(Vehicle, m_majorComponentMaxDamage) == 0x4b0);
	static_assert(offsetof(Vehicle, m_actionMapName) == 0x4b8);
#else
	static_assert(sizeof(Vehicle) == 0x37c);
	static_assert(offsetof(Vehicle, m_pVehicleSystem) == 0x68);
	static_assert(offsetof(Vehicle, m_pInventory) == 0x6c);
	static_assert(offsetof(Vehicle, m_physicalizeParams) == 0x70);
	static_assert(offsetof(Vehicle, m_pMovement) == 0xb0);
	static_assert(offsetof(Vehicle, m_engineSlotBySpeed) == 0xea);
	static_assert(offsetof(Vehicle, m_seats) == 0xf4);
	static_assert(offsetof(Vehicle, m_seatGroups) == 0x100);
	static_assert(offsetof(Vehicle, m_components) == 0x110);
	static_assert(offsetof(Vehicle, m_parts) == 0x11c);
	static_assert(offsetof(Vehicle, m_buoyancy) == 0x158);
	static_assert(offsetof(Vehicle, m_physics) == 0x1a0);
	static_assert(offsetof(Vehicle, m_physicsParams) == 0x1ec);
	static_assert(offsetof(Vehicle, m_isDestroyed) == 0x1fc);
	static_assert(offsetof(Vehicle, m_mass) == 0x200);
	static_assert(offsetof(Vehicle, m_animations) == 0x2cc);
	static_assert(offsetof(Vehicle, m_pSoundProxy) == 0x2e4);
	static_assert(offsetof(Vehicle, m_retainGravity) == 0x2f4);
	static_assert(offsetof(Vehicle, m_gravity) == 0x2f8);
	static_assert(offsetof(Vehicle, m_modName) == 0x318);
	static_assert(offsetof(Vehicle, m_paintName) == 0x31c);
	static_assert(offsetof(Vehicle, m_totalMaxDamage) == 0x324);
	static_assert(offsetof(Vehicle, m_majorComponentMaxDamage) == 0x328);
	static_assert(offsetof(Vehicle, m_actionMapName) == 0x330);
#endif

#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x3057f340;
#else
	std::uintptr_t ctor = 0x305591b0;
#endif

	(this->*reinterpret_cast<void(IVehicle::*&)()>(ctor))();
}

Vehicle::~Vehicle()
{
}

////////////////////////////////////////////////////////////////////////////////
// IGameObjectExtension
////////////////////////////////////////////////////////////////////////////////

bool Vehicle::Init(IGameObject* pGameObject)
{
	this->SetGameObject(pGameObject);

	if (!this->GetGameObject()->CaptureProfileManager(this))
	{
		CryLogErrorAlways("[Vehicle] Profile manager capture failed for %s", this->GetEntity()->GetName());
		return false;
	}

	m_pSoundProxy = static_cast<IEntitySoundProxy*>(this->GetEntity()->CreateProxy(ENTITY_PROXY_SOUND));

	m_pVehicleSystem = GameFramework::GetInstance()->GetIVehicleSystem();
	m_engineSlotBySpeed = true;

	m_pVehicleSystem->AddVehicle(this->GetEntityId(), this);
	GameFramework::GetInstance()->GetScriptBind_Vehicle()->AttachTo(this);

	m_pInventory = static_cast<IInventory*>(this->GetGameObject()->AcquireExtension("Inventory"));

	this->InitRespawn();

	if (m_modName.empty())
	{
		this->InitModName();
	}

	if (m_paintName.empty())
	{
		this->InitPaintName();
	}

	this->GetEntity()->AddFlags(ENTITY_FLAG_CASTSHADOW | ENTITY_FLAG_CUSTOM_VIEWDIST_RATIO | ENTITY_FLAG_ON_RADAR);

	IEntityProxy* pRenderProxy = this->GetEntity()->GetProxy(ENTITY_PROXY_RENDER);
	if (pRenderProxy)
	{
		static_cast<IEntityRenderProxy*>(pRenderProxy)->GetRenderNode()->SetViewDistRatio(50);
	}

	return true;
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

void Vehicle::NeedsUpdate(int flags, bool threadSafe)
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
// IGameObjectProfileManager
////////////////////////////////////////////////////////////////////////////////

bool Vehicle::SetAspectProfile(EEntityAspects aspect, std::uint8_t profile)
{
	if (aspect != eEA_Physics)
	{
		return false;
	}

	if (profile == 0)
	{
		m_physicalizeParams.type = PE_NONE;
		m_physicalizeParams.nSlot = -1;

		this->GetEntity()->Physicalize(m_physicalizeParams);

		return true;
	}
	else if (profile == 1)
	{
		m_physicalizeParams.type = PE_RIGID;
		m_physicalizeParams.nSlot = -1;
		m_physicalizeParams.mass = m_mass;
		m_physicalizeParams.pCar = nullptr;

		if (!m_pMovement || m_pMovement->GetPhysicalizationType() != PE_WHEELEDVEHICLE)
		{
			this->GetEntity()->Physicalize(m_physicalizeParams);
		}

		if (m_pMovement)
		{
			m_pMovement->Physicalize();
		}

		IPhysicalEntity* pPhysicalEntity = this->GetEntity()->GetPhysics();
		if (!pPhysicalEntity)
		{
			return false;
		}

		pe_params_buoyancy buoyancy(m_buoyancy);
		pPhysicalEntity->SetParams(&buoyancy);

		pe_simulation_params physics(m_physics);
		pPhysicalEntity->SetParams(&physics);

		pe_params_flags physicsParams(m_physicsParams);
		pPhysicalEntity->SetParams(&physicsParams);

		for (auto& [partName, pPart] : m_parts)
		{
			pPart->Physicalize();
		}

		this->GetGameObject()->EnablePhysicsEvent(true, eEPE_OnPostStepImmediate);

		{
			pe_params_flags newPhysicsParams;
			newPhysicsParams.flagsOR = pef_fixed_damping | pef_custom_poststep | pef_log_collisions;
			pPhysicalEntity->SetParams(&newPhysicsParams);
		}

		if (pPhysicalEntity->GetParams(&physics))
		{
			m_gravity = physics.gravity;
		}

		if (m_retainGravity)
		{
			pe_params_flags newPhysicsParams;
			newPhysicsParams.flagsOR = pef_ignore_areas;
			pPhysicalEntity->SetParams(&newPhysicsParams);
		}

		if (m_pMovement)
		{
			m_pMovement->PostPhysicalize();
		}

		if (!GameFramework::GetInstance()->IsEditing())
		{
			this->NeedsUpdate(eVUF_AwakePhysics, false);
		}

		return true;
	}

	return false;
}

std::uint8_t Vehicle::GetDefaultProfile(EEntityAspects aspect)
{
	if (aspect == eEA_Physics)
	{
		return 1;
	}

	return 0;
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

void Vehicle::PatchVTable()
{
	static constinit std::uintptr_t vtable[101] = {
#ifdef BUILD_64BIT
		0x30584BC0, 0x3056BFC0, 0x30522A50, 0x30563A00, 0x30581180, 0x3057B5C0, 0x3057E6C0, 0x30567BB0,
		0x30572910, 0x3056BD70, 0x305712A0, 0x30574A80, 0x30574A10, 0x3056D850, 0x30522A50, 0x3057F950,
		0x30566FA0, 0x30522A50, 0x30522A50, 0x3057CF20, 0x3057F940, 0x30563AB0, 0x30564400, 0x305716E0,
		0x305753B0, 0x305648F0, 0x30572D40, 0x30563A70, 0x30563A90, 0x3057F900, 0x3057F910, 0x30568730,
		0x305718E0, 0x30564600, 0x305793C0, 0x3057FA10, 0x30564680, 0x30565360, 0x30565390, 0x305654B0,
		0x30564860, 0x305654D0, 0x305646E0, 0x3057FA30, 0x30565470, 0x305644A0, 0x305644D0, 0x30564FE0,
		0x30564FA0, 0x3057F9F0, 0x30564F20, 0x30569870, 0x30565130, 0x30565210, 0x30565320, 0x305661E0,
		0x30566280, 0x30565010, 0x3056FC20, 0x30564A10, 0x30564AE0, 0x3057F980, 0x3057F990, 0x305653F0,
		0x3057F9A0, 0x3056A480, 0x30563AC0, 0x3056D1A0, 0x3057C3A0, 0x30576630, 0x3057F9B0, 0x3050CA60,
		0x30577BA0, 0x3057A040, 0x30563AF0, 0x30565D00, 0x3057F9C0, 0x30564360, 0x30578CC0, 0x30573030,
		0x30564E80, 0x3057F9D0, 0x3056DBF0, 0x30566660, 0x30566780, 0x3056D670, 0x30563A40, 0x3057F920,
		0x3057F930, 0x30588060, 0x3057E0A0, 0x3057C080, 0x3056F4F0, 0x30564BA0, 0x30563B40, 0x30566360,
		0x305657C0, 0x30565B90, 0x30565BE0, 0x30571B00, 0x30573130,
#else
		0x3055B4E0, 0x30550A90, 0x305F9730, 0x3054B470, 0x30562D00, 0x305581E0, 0x30558D30, 0x3054E050,
		0x305540D0, 0x30550A10, 0x30553C60, 0x305549F0, 0x30554990, 0x30551DC0, 0x305F9730, 0x305629D0,
		0x3055E300, 0x3054B500, 0x30703E60, 0x305588F0, 0x305629C0, 0x3054B530, 0x3054BBB0, 0x30554500,
		0x305551A0, 0x3054BF40, 0x30554140, 0x3054B4C0, 0x3054B4E0, 0x30562980, 0x30562990, 0x3054EA80,
		0x30553F20, 0x3054BDB0, 0x305568D0, 0x30562A80, 0x3054BE00, 0x3054C630, 0x3054C660, 0x3054C740,
		0x3054BEE0, 0x3054C750, 0x3054BE70, 0x30562A90, 0x3054C710, 0x3054BC10, 0x3054BC30, 0x3054C380,
		0x3054C350, 0x30562A70, 0x3054C2F0, 0x3054FAA0, 0x3054C4B0, 0x3054C510, 0x3054C600, 0x3054D080,
		0x3054D0F0, 0x3054C3B0, 0x30553410, 0x3054C060, 0x3054C100, 0x305629F0, 0x30562A00, 0x3054C6B0,
		0x30562A10, 0x30550360, 0x3054B540, 0x305518F0, 0x30558740, 0x30556320, 0x30562A20, 0x30562A30,
		0x30556460, 0x30557520, 0x3054B590, 0x3054CD80, 0x30562A40, 0x3054BB30, 0x305564D0, 0x30554380,
		0x3054C2C0, 0x30562A50, 0x30552090, 0x3054D370, 0x3054D480, 0x30551D00, 0x3054B4A0, 0x305629A0,
		0x305629B0, 0x3055CDC0, 0x30558B80, 0x30558510, 0x305530D0, 0x3054C1A0, 0x3054B5E0, 0x3054D1B0,
		0x3054CA40, 0x3054CCC0, 0x3054CD00, 0x30554760, 0x305543C0,
#endif
	};

	const std::uintptr_t* current_vtable = *reinterpret_cast<std::uintptr_t**>(this);

	// use original IGameObjectExtension and IVehicle functions from CryAction DLL except the following
	vtable[0] = current_vtable[0];  // Vehicle::Init

	*reinterpret_cast<std::uintptr_t**>(this) = vtable;
}

void Vehicle::InitActions(const SmartScriptTable& table)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x305841f0;
#else
	std::uintptr_t func = 0x3055b000;
#endif

	(this->*reinterpret_cast<void(IVehicle::*&)(const SmartScriptTable&)>(func))(table);
}

void Vehicle::InitDamages(const SmartScriptTable& table)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x305a8dd0;
#else
	std::uintptr_t func = 0x3057b1e0;
#endif

#ifdef BUILD_64BIT
	const unsigned int thisOffset = 0x28;
#else
	const unsigned int thisOffset = 0x14;
#endif

	Vehicle* pThis = reinterpret_cast<Vehicle*>(reinterpret_cast<unsigned char*>(this) + thisOffset);

	(pThis->*reinterpret_cast<void(IVehicle::*&)(Vehicle*, const SmartScriptTable&)>(func))(this, table);
}

void Vehicle::InitParticles(const SmartScriptTable& table)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x30581430;
#else
	std::uintptr_t func = 0x3055a040;
#endif

	(this->*reinterpret_cast<void(IVehicle::*&)(const SmartScriptTable&)>(func))(table);
}

void Vehicle::InitHelpers()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x3056f4f0;
#else
	std::uintptr_t func = 0x305530d0;
#endif

	(this->*reinterpret_cast<void(IVehicle::*&)()>(func))();
}

void Vehicle::InitMaxDamage()
{
	m_totalMaxDamage = 0;
	m_majorComponentMaxDamage = 0;

	for (VehicleComponent* pComponent : m_components)
	{
		m_totalMaxDamage += pComponent->GetMaxDamage();

		if (pComponent->IsMajorComponent())
		{
			m_majorComponentMaxDamage += pComponent->GetMaxDamage();
		}
	}

	if (m_majorComponentMaxDamage > 0)
	{
		for (VehicleComponent* pComponent : m_components)
		{
			if (pComponent->IsMajorComponent())
			{
				pComponent->SetVehicleHealthProportion(
					pComponent->GetMaxDamage() / m_majorComponentMaxDamage
				);
			}
		}
	}
}

void Vehicle::InitSeatTransitions()
{
	const int seatCount = static_cast<int>(m_seats.size());

	m_seatTransitions.resize(seatCount);

	for (int i = 0; i < seatCount; i++)
	{
		IVehicleHelper* pExitHelper = m_seats[i].second->GetExitHelper();

		for (int j = 0; j < seatCount; j++)
		{
			if (i != j && m_seats[j].second->GetExitHelper() == pExitHelper && pExitHelper)
			{
				m_seatTransitions[i].push_back(j);
			}
		}
	}
}

void Vehicle::LoadParts(const SmartScriptTable& parts, IVehiclePart* parent, ComponentParts& componentParts)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x30563c80;
#else
	std::uintptr_t func = 0x3054b6c0;
#endif

	(this->*reinterpret_cast<void(IVehicle::*&)(const SmartScriptTable&, IVehiclePart*, ComponentParts&)>(func))(
		parts, parent, componentParts
	);
}

void Vehicle::RegisterComponentParts(const ComponentParts& componentParts)
{
	for (const auto& [pPart, componentName] : componentParts)
	{
		IVehicleComponent* pComponent = this->GetComponent(componentName.c_str());
		if (pComponent)
		{
			static_cast<VehicleComponent*>(pComponent)->AddPart(pPart);
		}
	}
}

bool Vehicle::SetMovement(const char* name, const SmartScriptTable& movement)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x30564ba0;
#else
	std::uintptr_t func = 0x3054c1a0;
#endif

	return (this->*reinterpret_cast<bool(IVehicle::*&)(const CryStringT<char>&, const SmartScriptTable&)>(func))(
		name, movement
	);
}

void Vehicle::SetPaintMaterial(const char* name)
{
	const bool create = false;

	IMaterial* pMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial(name, create);
	if (!pMaterial)
	{
		CryLogErrorAlways("[Vehicle::SetPaintMaterial] Cannot find material %s", name);
		return;
	}

	this->GetEntity()->SetMaterial(pMaterial);
}

void Vehicle::SetAmmoCapacity(const char* name, int capacity)
{
	IEntityClass* pAmmoClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(name);
	if (!pAmmoClass)
	{
		CryLogErrorAlways("[Vehicle::SetAmmoCapacity] Cannot find ammo class %s", name);
		return;
	}

	m_pInventory->SetAmmoCapacity(pAmmoClass, capacity);
}

void Vehicle::AttachScriptBindToSeats()
{
	ScriptBind_VehicleSeat* pScriptBind_VehicleSeat = GameFramework::GetInstance()->GetScriptBind_VehicleSeat();

	int seatId = 1;
	for (const auto& [name, pSeat] : m_seats)
	{
		pScriptBind_VehicleSeat->AttachTo(this, seatId);
		seatId++;
	}
}

bool Vehicle::BindVehicleToNetwork()
{
	if (this->GetEntity()->GetFlags() & (ENTITY_FLAG_CLIENT_ONLY | ENTITY_FLAG_SERVER_ONLY))
	{
		return true;
	}

	if (!this->GetGameObject()->BindToNetwork())
	{
		CryLogErrorAlways("[Vehicle::BindToNetwork] Failed for %s", this->GetEntity()->GetName());
		return false;
	}

	return true;
}

void Vehicle::InitRespawn()
{
	if (!gEnv->bServer)
	{
		return;
	}

	IScriptTable* pScriptTable = this->GetEntity()->GetScriptTable();
	SmartScriptTable propertiesTable;
	SmartScriptTable respawnTable;
	int respawn = 0;
	if (!pScriptTable
	 || !pScriptTable->GetValue("Properties", propertiesTable)
	 || !propertiesTable->GetValue("Respawn", respawnTable)
	 || !respawnTable->GetValue("bRespawn", respawn)
	 || !respawn)
	{
		return;
	}

	IGameRules* pGameRules = GameFramework::GetInstance()->GetIGameRulesSystem()->GetCurrentGameRules();
	if (!pGameRules)
	{
		return;
	}

	pGameRules->CreateEntityRespawnData(this->GetEntityId());
}

void Vehicle::InitModName()
{
	IScriptTable* pScriptTable = this->GetEntity()->GetScriptTable();
	SmartScriptTable propertiesTable;
	const char* mod = nullptr;
	if (!pScriptTable
	 || !pScriptTable->GetValue("Properties", propertiesTable)
	 || !propertiesTable->GetValue("Modification", mod))
	{
		return;
	}

	m_modName = mod;
}

void Vehicle::InitPaintName()
{
	IScriptTable* pScriptTable = this->GetEntity()->GetScriptTable();
	SmartScriptTable propertiesTable;
	const char* paint = nullptr;
	if (!pScriptTable
	 || !pScriptTable->GetValue("Properties", propertiesTable)
	 || !propertiesTable->GetValue("Paint", paint))
	{
		return;
	}

	m_paintName = paint;
}
