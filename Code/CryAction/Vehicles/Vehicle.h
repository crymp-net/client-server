/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a wheels based vehicle

-------------------------------------------------------------------------
History:
- 05:10:2004: Created by Mathieu Pinard

*************************************************************************/
#ifndef __CVEHICLE_H__
#define __CVEHICLE_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <map>
#include <utility>
#include <vector>


#include "VehicleSystem.h"
#include "VehicleUtils.h"
#include "CryCommon/CryAction/IActionMapManager.h"
#include "CryCommon/CryScriptSystem/IScriptSystem.h"
#include "CryCommon/CryAction/Network/NetActionSync.h"
#include "VehicleDamages.h"
#include "VehicleCVars.h"
#include "CryCommon/CryCore/VectorMap.h"


class CEntityObject;
class CVehicleSeat;
class CVehicleSeatGroup;
class CVehicleComponent;
struct pe_params_car;
class CVehiclePartBase;
class CVehiclePartWheel;
class CVehicleHUDElement;
class CVehicleHelper;
struct ISound;
struct IInventory;

typedef std::pair <string, CVehicleSeat*> TVehicleSeatPair;
typedef std::vector <TVehicleSeatPair> TVehicleSeatVector;
typedef std::vector <CVehicleSeatGroup*> TVehicleSeatGroupVector;
typedef std::pair <string, IVehiclePart*> TVehiclePartPair;
typedef std::vector <TVehiclePartPair> TVehiclePartVector;
typedef std::vector<CVehicleComponent*> TVehicleComponentVector;
typedef std::list<CVehicleHUDElement*> TVehicleHUDElementList;
typedef std::map<IVehicleEventListener*, TVehicleListenerName> TVehicleEventListenerList;
typedef std::pair <string, IVehicleAnimation*> TVehicleStringAnimationPair;
typedef std::vector <TVehicleStringAnimationPair> TVehicleAnimationsVector;

struct SActivationInfo
{
	enum EActivationType
	{
		eAT_OnUsed = 0,
		eAT_OnGroundCollision,
		eAT_Last,
	};

	enum EActivationParam1
	{
		eAP1_Part = 0,
		eAP1_Component,
		eAP1_Last,
	};

	SActivationInfo()    
		: m_type(eAT_Last)
		, m_param1(eAP1_Last)
		, m_distance(0.f)
	{
	}

	EActivationType m_type;
	EActivationParam1 m_param1;
	string m_param2;
	float m_distance;
};

typedef std::vector <SActivationInfo> TActivationInfoVector;

struct SVehicleActionInfo
{
	TActivationInfoVector activations;
	IVehicleAction* pAction;
	int useWhenFlipped;
};

typedef std::vector <SVehicleActionInfo> TVehicleActionVector;

struct STransitionInfo
{
	STransitionInfo()     
	{
	}

	DynArray<TVehicleSeatId> waitFor;
};
typedef DynArray<STransitionInfo> TTransitionInfo;


class CVehicle : 
	public CGameObjectExtensionHelper<CVehicle, IVehicle>,
	public IGameObjectProfileManager,
	public CVehicleDamages
{
public:
	CVehicle();
	~CVehicle();
	
	static const EEntityAspects ASPECT_SEAT_PASSENGER	= eEA_GameServerStatic;
	static const EEntityAspects ASPECT_SEAT_ACTION	= eEA_GameClientDynamic;
	static const EEntityAspects ASPECT_COMPONENT_DAMAGE = eEA_GameServerStatic;
	static const EEntityAspects ASPECT_PART_MATRIX	= eEA_GameClientDynamic;
	static const EEntityAspects ASPECT_FROZEN = eEA_GameServerStatic;

	enum EVehicleTimers
	{
		eVT_Abandoned = 0, 
		eVT_AbandonedSound,
		eVT_Flipped,
		eVT_Last
	};

	struct SPartComponentPair
	{
		SPartComponentPair(IVehiclePart* pP, const char* pC) : pPart(pP), component(pC) {}

		IVehiclePart* pPart;
		string component;
	};

	struct SPartInitInfo
	{
		std::vector<SPartComponentPair> partComponentMap;
	};

	virtual void SetOwnerId(EntityId ownerId) { m_ownerId = ownerId; };
	virtual EntityId GetOwnerId() const { return m_ownerId; };
	virtual bool CanEnter(EntityId actorId);

	enum EVehiclePhysicsProfile
	{
		eVPhys_NotPhysicalized=0,
		eVPhys_Physicalized,
		eVPhys_DemoRecording
	};

	// IVehicle
	virtual bool Init( IGameObject * pGameObject );
	virtual void InitClient( int channelId ) {};
	virtual void PostInit(IGameObject * pGameObject);
	virtual void PostInitClient(int channelId);
	virtual void Reset(bool enterGame);
	virtual void Release() { delete this; }
	virtual void GetMemoryStatistics(ICrySizer * s);

	virtual void SetAmmoCapacity(IEntityClass* pAmmoType, int capacity);
	virtual void SetAmmoCount(IEntityClass* pAmmoType, int amount);
	virtual int  GetAmmoCount(IEntityClass* pAmmoType) const override;
	virtual int GetAmmoCapacity(IEntityClass* pAmmoType) const override;


	// set/get the last weapon created on this vehicle
	// see ClSetupWeapons, SendWeaponSetup and  CVehicleSeatActionWeapons::Init for more info
	virtual void SetLastCreatedWeaponId(EntityId lastWeaponId) { m_lastWeaponId=lastWeaponId; };
	virtual EntityId GetLastCreatedWeaponId() const { return m_lastWeaponId; };

	virtual void SendWeaponSetup(int where, int channelId=-1);

	virtual SEntityPhysicalizeParams& GetPhysicsParams() { return m_physicsParams; }

	virtual void ProcessEvent(SEntityEvent& entityEvent);	
	virtual void HandleEvent( const SGameObjectEvent& );
	virtual void PostUpdate(float frameTime);
	virtual void PostRemoteSpawn() {};

	virtual const SVehicleStatus& GetStatus();

	virtual void Update(SEntityUpdateContext& ctx, int nSlot);
	virtual void UpdateView(SViewParams &viewParams, EntityId playerId = 0);

	virtual void SetChannelId(uint16 id) {};
	virtual void SetAuthority(bool auth) { if (m_pMovement) m_pMovement->SetAuthority(auth); };

	virtual void FullSerialize( TSerialize ser );
	virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int flags );
	virtual void PostSerialize();
	virtual void SerializeSpawnInfo( TSerialize ser );
	virtual ISerializableInfoPtr GetSpawnInfo();
	virtual void OnAction(const TVehicleActionId actionId, int activationMode, float value, EntityId callerId);
	virtual void OnHit(EntityId targetId, EntityId shooterId, float damage, Vec3 position, float radius, const char* pHitClass, bool explosion);

	virtual int IsUsable(EntityId userId);
	virtual bool OnUsed(EntityId userId, int index);

	virtual IVehiclePart* AddPart(const CVehicleParams& partParams, IVehiclePart* parent, SPartInitInfo& initInfo);
	virtual bool AddSeat(const SmartScriptTable& seatParams);
	virtual bool AddHelper(const char* pName, Vec3 position, Vec3 direction, IVehiclePart* pPart);
	virtual void InitHelpers(const CVehicleParams& table);
	virtual bool HasHelper(const char* name);

	virtual bool SetMovement(const string& movementName, const CVehicleParams& table);
	virtual IVehicleMovement* GetMovement() { return m_pMovement; }

	virtual int GetWheelCount() { return m_wheelCount; }
	virtual IVehiclePart* GetWheelPart(int idx); 

	virtual float GetMass() { return m_mass; }
	virtual float GetAltitude();

	virtual unsigned int GetSeatCount() { return m_seats.size(); }

	virtual TVehicleSeatId GetLastSeatId();

	virtual IVehicleSeat* GetSeatForPassenger(EntityId passengerId);
	virtual IVehicleSeat* GetSeatById(const TVehicleSeatId seatId);
	virtual IActor* GetDriver();	
	virtual TVehicleSeatId GetSeatId(const char* pSeatName);

	virtual IVehiclePart* GetPart(const char* name);
	virtual IVehiclePart* GetWeaponParentPart(EntityId weaponId);
	IVehicleSeat* GetWeaponParentSeat(EntityId weaponId);

	virtual int GetComponentCount() const;
	virtual IVehicleComponent* GetComponent(int index);
	virtual IVehicleComponent* GetComponent(const char* name);
	virtual float GetDamageRatio(bool onlyMajorComponents = false) const;

	virtual SParticleParams* GetParticleParams() { return &m_particleParams; }
	virtual const SVehicleDamageParams& GetDamageParams() const { return m_damageParams; }

	virtual IVehicleAnimation* GetAnimation(const char* name);

	virtual IMovementController * GetMovementController();
	virtual IMovementController * GetPassengerMovementController( EntityId passenger );

	virtual IFireController* GetFireController();
	virtual EntityId GetCurrentWeaponId(EntityId passengerId, bool secondary=false);  
	virtual int GetWeaponCount() const;
	virtual EntityId GetWeaponId(int index) const;

	virtual float GetFrozenAmount() const;
	virtual void SetFrozenAmount(float frost);

	// check if player/client is driving
	virtual bool IsPlayerDriving(bool clientOnly = true);

	// check if any passenger is friendly to pPlayer
	virtual bool HasFriendlyPassenger(IEntity *pPlayer);

	// check if ClientActor is on board
	virtual bool IsPlayerPassenger();

	virtual void StartAbandonedTimer(bool force=false, float timer=-1.0f);
	virtual void KillAbandonedTimer();
	virtual void Destroy();
	virtual void EnableAbandonedWarnSound(bool enable);

	virtual void BroadcastVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params);
	virtual void RegisterVehicleEventListener(IVehicleEventListener* pEvenListener, const char* name);
	virtual void UnregisterVehicleEventListener(IVehicleEventListener* pEvenListener);

	void SetObjectUpdate(IVehicleObject* pObject, EVehicleObjectUpdate updatePolicy);

	virtual IVehicleHelper* GetHelper(const char* name);
	virtual bool IsDestroyed();

	virtual void TriggerEngineSlotBySpeed(bool trigger) { m_engineSlotBySpeed = trigger; }
	virtual int SetTimer(int timerId, int ms, IVehicleObject* pObject);
	virtual int KillTimer(int timerId);
	virtual void KillTimers();

	virtual void SetVehicleGlassSurfaceType(ISurfaceType* pSurface) { sSurfaceMatGlassVehicle=pSurface; }  

	virtual bool GetExitPositionForActor(IActor* pActor, Vec3& pos, bool extended = false);
	virtual void ExitVehicleAtPosition(EntityId passengerId, const Vec3 &pos);
	virtual void EvictAllPassengers();
	// ~IVehicle

	struct RequestUseParams
	{
		EntityId	actorId;
		int				index;
		RequestUseParams() {};
		RequestUseParams(EntityId _actorId, int _index)
			: actorId(_actorId),
			index(_index) {};
		void SerializeWith(TSerialize ser)
		{
			ser.Value("index", index, 'vUse');
			ser.Value("actorId", actorId, 'eid');
		};
	};

	struct RequestChangeSeatParams
	{
		EntityId	actorId;
		int seatChoice;
		RequestChangeSeatParams() {};
		RequestChangeSeatParams(EntityId _actorId, int _seatChoice)
			: actorId(_actorId), seatChoice(_seatChoice) {};
		void SerializeWith(TSerialize ser)
		{
			ser.Value("actorId", actorId, 'eid');
			ser.Value("seatChoice", seatChoice, 'vSit');
		};
	};

	/*
	struct RequestLeaveParams
	{
		EntityId	actorId;
		Vec3			exitPos;
		RequestLeaveParams() {};
		RequestLeaveParams(EntityId _actorId, Vec3 _exitPos)
			: actorId(_actorId), exitPos(_exitPos) {};
		void SerializeWith(TSerialize ser)
		{
			ser.Value("actorId", actorId, 'eid');
			ser.Value("exitPos", exitPos, 'wrld'); 
		};
	};*/
	
	struct RequestLeaveParams //CryMP
	{
		EntityId	actorId;
		RequestLeaveParams() {};
		RequestLeaveParams(EntityId _actorId)
			: actorId(_actorId) {};
		void SerializeWith(TSerialize ser)
		{
			ser.Value("actorId", actorId, 'eid');
		};
	};

	struct SetupWeaponsParams
	{
		struct SeatWeaponParams
		{
			struct SeatActionWeaponParams
			{
				std::vector<EntityId> weapons;

				void SerializeWith(TSerialize ser)
				{
					if (ser.IsReading())
					{
						int nweapons=0;
						ser.Value("NumberWeapons", nweapons, 'vNWp');
						for (int i=0;i<nweapons;i++)
						{
							EntityId id;
							ser.Value("weaponId", id, 'eid');
							weapons.push_back(id);
						}
					}
					else
					{
						int nweapons=(int)weapons.size();
						ser.Value("NumberWeapons", nweapons, 'vNWp');
						for (int i=0;i<nweapons;i++)
						{
							EntityId id = weapons[i];
							ser.Value("weaponId", id, 'eid');
						}
					}
				}
			};

			std::vector<SeatActionWeaponParams> seatactions;

			void SerializeWith(TSerialize ser)
			{
				if (ser.IsReading())
				{
					int nactions=0;
					ser.Value("NumberActions", nactions, 'vNWp');
					seatactions.reserve(nactions);
					for (int i=0;i<nactions;i++)
					{
						seatactions.push_back(SeatActionWeaponParams());
						seatactions.back().SerializeWith(ser);
					}
				}
				else
				{
					int nactions=(int)seatactions.size();
					ser.Value("NumberActions", nactions, 'vNWp');
					for (int i=0;i<nactions;i++)
						seatactions[i].SerializeWith(ser);
				}
			};
		};

		std::vector<SeatWeaponParams> seats;

		SetupWeaponsParams() {};
		void SerializeWith(TSerialize ser)
		{
			if (ser.IsReading())
			{
				int nseats=0;
				ser.Value("NumberSeats", nseats, 'vNWp');
				seats.reserve(nseats);
				for (int i=0;i<nseats;i++)
				{
					seats.push_back(SeatWeaponParams());
					seats.back().SerializeWith(ser);
				}
			}
			else
			{
				int nseats=(int)seats.size();
				ser.Value("NumberSeats", nseats, 'vNWp');
				for (int i=0;i<nseats;i++)
					seats[i].SerializeWith(ser);
			}
		};
	};

	struct AmmoParams
	{
		AmmoParams() {};
		AmmoParams(const char *name, int amount): ammo(name), count(amount) {};
		void SerializeWith(TSerialize ser)
		{
			ser.Value("ammo", ammo);
			ser.Value("amount", count, 'ammo');
		}
		string ammo;
		int	count;
	};

	struct AmmoCountsParams
	{
		struct Ammo
		{
			CryStringT<char> name;
			int count = 0;
		};

		std::vector<Ammo> ammos;

		void SerializeWith(TSerialize ser)
		{
			if (ser.IsReading())
			{
				int ammoCount = 0;
				ser.Value("NumberAmmos", ammoCount, 'vNWp');

				this->ammos.resize(ammoCount);
				for (int i = 0; i < ammoCount; i++)
				{
					ser.Value("AmmoName", this->ammos[i].name, 'stab');
					ser.Value("AmmoCount", this->ammos[i].count, 'ammo');
				}
			}
			else
			{
				int ammoCount = static_cast<int>(this->ammos.size());
				ser.Value("NumberAmmos", ammoCount, 'vNWp');

				for (int i = 0; i < ammoCount; i++)
				{
					ser.Value("AmmoName", this->ammos[i].name, 'stab');
					ser.Value("AmmoCount", this->ammos[i].count, 'ammo');
				}
			}
		}
	};

	struct AbandonWarningParams
	{
		bool enable;

		AbandonWarningParams(): enable(false) {};
		AbandonWarningParams(bool e): enable(e) {};

		void SerializeWith(TSerialize ser)
		{
			ser.Value("enable", enable, 'bool');
		}
	};

	DECLARE_SERVER_RMI_PREATTACH(SvRequestUse, RequestUseParams, eNRT_ReliableOrdered);
	DECLARE_SERVER_RMI_PREATTACH(SvRequestChangeSeat, RequestChangeSeatParams, eNRT_ReliableOrdered);
	DECLARE_SERVER_RMI_PREATTACH(SvRequestLeave, RequestLeaveParams, eNRT_ReliableOrdered);
	//DECLARE_CLIENT_RMI_PREATTACH(ClRequestLeave, RequestLeaveParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_NOATTACH(ClSetAmmo, AmmoParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_NOATTACH(ClSetupWeapons, SetupWeaponsParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_NOATTACH(ClAmmoCounts, AmmoCountsParams, eNRT_ReliableOrdered);
	DECLARE_CLIENT_RMI_NOATTACH(ClAbandonWarning, AbandonWarningParams, eNRT_ReliableOrdered);

	// IGameObjectProfileManager
	virtual bool SetAspectProfile( EEntityAspects aspect, uint8 profile );
	virtual uint8 GetDefaultProfile( EEntityAspects aspect ) { return aspect==eEA_Physics? eVPhys_Physicalized : 0; }
	// ~IGameObjectProfileManager

public:

	struct SObjectUpdateInfo
	{
		IVehicleObject* pObject;
		int updatePolicy;
	};
	typedef std::list<SObjectUpdateInfo> TVehicleObjectUpdateInfoList;

public:

	bool AddComponent(const CVehicleParams& componentParams);

	TVehicleComponentVector& GetComponents() { return m_components; }
	TVehiclePartVector& GetParts() { return m_parts; }

	virtual int GetPartCount(){ return m_parts.size(); }
	virtual IVehiclePart* GetPart(unsigned int index);
	virtual void GetParts(IVehiclePart** parts, int nMax);
	CVehiclePartAnimated* GetVehiclePartAnimated();

	virtual int GetActionCount() { return m_actions.size(); }
	virtual IVehicleAction* GetAction(int index);

	TVehicleSeatId GetSeatId(CVehicleSeat *pSeat);
	TVehicleSeatId GetDriverSeatId();

	TVehicleSeatId GetNextSeatId(const TVehicleSeatId seatIdToStart = InvalidVehicleSeatId);
	void ChangeSeat(EntityId actorId, int seatChoice = 0, TVehicleSeatId newSeatId = InvalidVehicleSeatId);

	void OnCollision(EventPhysCollision* pCollision);
	float GetSelfCollisionMult(const Vec3& velocity, const Vec3& normal, int partId, EntityId colliderId);

	void RegisterHUDElement(CVehicleHUDElement* pHudElement);
	void UnregisterHUDElement(CVehicleHUDElement* pHudElement);

	void UpdateHUD(float deltaTime);
	int GetDebugIndex(){ return m_debugIndex++; }

	// when using this, take care of static buffer the result points to
	const char* GetSoundName(const char* eventName);

	bool InitRespawn();
	void OnDestroyed();
	void OnSubmerged(float ratio);
	bool SpawnAndDeleteEntities(bool clientBasedOnly = false);

	bool TriggerEngineSlotBySpeed() { return m_engineSlotBySpeed; }
	bool IsEmpty();
	virtual bool IsFlipped(float maxSpeed=0.f);
	bool IsFrozen();
	bool IsSubmerged();

	TVehicleSoundEventId AddSoundEvent(SVehicleSoundInfo& info);
	SVehicleSoundInfo* GetSoundInfo(TVehicleSoundEventId eventId);
	ISound* GetSound(TVehicleSoundEventId eventId, bool start=true);
	void SetSoundParam(TVehicleSoundEventId eventId, const char* param, float value, bool start=true);
	void StopSound(TVehicleSoundEventId eventId);
	inline bool EventIdValid(TVehicleSoundEventId eventId);

	int GetNextPhysicsSlot(bool high) const;
	void DumpParts() const;

	const string& GetActionMap() { return m_actionMap; }

	void RequestPhysicalization(IVehiclePart* pPart, bool request); 
	virtual void NeedsUpdate(int flags=0, bool bThreadSafe=false);

	void CheckFlippedStatus(const float deltaTime);  

	STransitionInfo& GetTransitionInfoForSeat(TVehicleSeatId seatId) 
	{ 
		assert(seatId>0 && seatId<=m_transitionInfo.size()); 
		return m_transitionInfo[seatId-1];
	}

	int GetSkipEntities(IPhysicalEntity** pSkipEnts, int nMaxSkip);

	virtual const char* GetModification()const;
	virtual const char* GetDestroyedMaterial() const { return m_destroyedMaterial.c_str(); }

	bool ExitSphereTest(IPhysicalEntity** pSkipEnts, int nskip, Vec3 startPos, Vec3 testPos, EntityId* pBlockingEntity);
	bool DoExtendedExitTest(Vec3 seatPos, Vec3 firstTestPos, EntityId blockingEntity, Vec3& outPos);

	void DebugFlipOver();

protected:

	bool InitActions(const CVehicleParams& vehicleTable);
	void InitPaint(const CVehicleParams& xmlContent);
	bool IsActionUsable(const SVehicleActionInfo& actionInfo, const SMovementState* movementState);

	void LoadParts(const CVehicleParams& table, IVehiclePart* pParent, SPartInitInfo& initInfo);

	void OnPhysPostStep(const EventPhys *pEvent);
	void OnPhysStateChange(EventPhysStateChange* pEvent);
	void OnMaterialLayerChanged(const SEntityEvent& event);
	bool InitParticles(const CVehicleParams& table);  
	void InitModification(const CVehicleParams& data, const char *modification);
	void OnTimer(int timerId);
	void CheckDisableUpdate(int slot);
	void ProcessFlipped();

	void UpdateStatus(const float deltaTime);
	void UpdateHelpers();
	void SetDestroyedStatus(bool isDestroyed) { m_isDestroyed = isDestroyed; }
	void DoRequestedPhysicalization();
	void DebugDraw(const float frameTime);
	bool IsDebugDrawing();
	bool IsCrewHostile(EntityId userId);

	unsigned int GetPartChildrenCount(IVehiclePart* pParentPart);

	void KillPassengersInExposedSeats(bool includePlayers);

	std::vector<IParticleEffect*> m_pParticleEffects;
	IVehicleSystem* m_pVehicleSystem;
	IInventory* m_pInventory;

	SEntityPhysicalizeParams m_physicsParams;

	IVehicleMovement* m_pMovement;
	SVehicleStatus m_status;

	EntityId	m_ownerId;
	bool			m_frozen;

	bool m_customEngineSlot;
	bool m_engineSlotBySpeed;

	int m_lastFrameId;
	bool m_bNeedsUpdate;

	TVehicleSeatVector m_seats;
	TVehicleSeatGroupVector m_seatGroups;  
	TTransitionInfo m_transitionInfo;

	TVehicleComponentVector m_components;
	TVehiclePartVector m_parts;

	typedef std::map <string, CVehicleHelper*> TVehicleHelperMap;
	TVehicleHelperMap m_helpers;

	typedef std::map <int, IVehicleObject*> TVehicleTimerMap;
	TVehicleTimerMap m_timers;

	pe_params_buoyancy m_buoyancyParams;
	pe_simulation_params m_simParams;
	pe_params_flags m_paramsFlags;    

	bool m_isDestroyed;
	float m_mass;	
	int m_wheelCount;	

	CParticleParams m_particleParams;
	TVehicleHUDElementList m_hudElements;
	TVehicleEventListenerList m_eventListeners;
	TVehicleActionVector m_actions;
	TVehicleAnimationsVector m_animations;
	TVehicleObjectUpdateInfoList m_objectsToUpdate;
						
	EntityId m_lastWeaponId;

	IEntitySoundProxy* m_pSoundProxy;   

	typedef std::vector<SVehicleSoundInfo> TVehicleSoundEvents;
	TVehicleSoundEvents m_soundEvents;

	bool m_bRetainGravity;
	Vec3 m_gravity;

	int m_debugIndex;

	bool m_usesVehicleActionToEnter;

	Vec3		m_initialposition;
	string	m_modification;
	string  m_paint;
	string  m_destroyedMaterial;
	tSoundID m_abandonedSoundId;

	float m_damageMax;
	float m_majorComponentDamageMax;
	bool m_indestructible;

	string m_actionMap;

	typedef std::set<IVehiclePart*> TVehiclePartSet;
	TVehiclePartSet m_physicalizeParts;

	float m_collisionDisabledTime;

	VectorMap<EntityId, int> m_ser_passengers;

	_smart_ptr<IMaterial> m_pBrokenWindowMat;
	static ISurfaceType* sSurfaceMatGlassVehicle;

	friend class CScriptBind_Vehicle;
	friend class CVehiclePartBase;
	friend class CVehiclePartSubPartWheel;
	friend class CVehiclePartMassBox;
	friend class CVehicleDamageBehaviorDestroy;
};

class CVehicleHUDElement
{
public:

	CVehicleHUDElement() : m_refCount(0) { AddRef(); }

	void SetMessage(const string& message) { m_message = message; }
	void SetColor(const float* color) { m_color[0] = color[0]; m_color[1] = color[1]; m_color[2] = color[2]; m_color[3] = 1.0f; }
	void SetBlinkingSpeed(float blinkSpeed) { m_blinkSpeed = blinkSpeed; }

	void AddRef() { m_refCount++; }
	void Release()
	{
		m_refCount--;
		//if (m_refCount == 0)
		//	delete this;
	}
protected:

	int m_refCount;

	string m_message;
	float m_color[4];
	float m_blinkSpeed;

	friend class CVehicle;
};

#endif // __CVEHICLE_H__
