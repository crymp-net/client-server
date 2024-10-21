/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a standard seat for vehicles

-------------------------------------------------------------------------
History:
- 14:04:2005: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLESEAT_H__
#define __VEHICLESEAT_H__

#include <list>
#include <utility>
#include <vector>

class CGameChannel;
class CVehicle;
class CVehicleSeatSerializer;
class CVehicleSeatGroup;
class CScriptBind_Vehicle;
class CScriptBind_VehicleSeat;
class CCryAction;
class CVehicleSeatActionWeapons;
class CVehicleSeatActionRotateTurret;
struct IAnimationGraph;
struct IAnimationGraphState;
class CVehicleViewThirdPerson;

#include "IVehicleSystem.h"
#include "IMovementController.h"
#include "AIProxy.h"
#include "IAnimationGraph.h"

struct SSeatActionData
{
	IVehicleSeatAction* pSeatAction;
	bool isAvailableRemotely;

  SSeatActionData()
  {
    pSeatAction = NULL;
    isAvailableRemotely = false;
  }
};


typedef std::vector <SSeatActionData> TVehicleSeatActionVector;
typedef std::vector <IVehicleView*> TVehicleViewVector;
typedef std::vector <string> TStringVector;

class CVehicleSeat
	: 
	public IVehicleSeat,
	public IMovementController,
	public IFireController,
	public IAnimationGraphStateListener
{
public:

	CVehicleSeat();
	~CVehicleSeat();

	VIRTUAL bool Init(IVehicle* pVehicle, TVehicleSeatId seatId, const CVehicleParams& seatTable);
	VIRTUAL void PostInit(IVehicle* pVehicle);
	VIRTUAL void Reset();
	virtual void Release() { delete this; }

	void GetMemoryStatistics(ICrySizer * s);
  
  const char* GetSeatName() const { return m_name.c_str(); }
	IMovementController * GetMovementController() { return this; }

	//	void SetAIDirection(const Vec3& vDir);

	void SerializeActions(TSerialize ser, EEntityAspects aspects);

	// IVehicleSeat
	VIRTUAL bool Enter(EntityId actorId, bool isTransitionEnabled = true);
	VIRTUAL bool Exit(bool isTransitionEnabled, bool force=false, Vec3 exitPos=ZERO);

  VIRTUAL void Lock(bool lock) { m_isLocked = lock; }

	VIRTUAL bool IsDriver() { return m_isDriver; }
	VIRTUAL bool IsGunner();
	VIRTUAL bool IsLocked() { return m_isLocked; }
  virtual bool IsRotatable();

  VIRTUAL bool IsPassengerHidden();
	VIRTUAL bool IsPassengerExposed() const { return m_isPassengerExposed; }
  VIRTUAL SSeatSoundParams& GetSoundParams() { return m_soundParams; }
  
  VIRTUAL bool IsAnimGraphStateExisting(EntityId actorId);

	VIRTUAL TVehicleViewId GetCurrentView() { return m_currentView; }
	VIRTUAL IVehicleView* GetView(TVehicleViewId viewId);
	VIRTUAL bool SetView(TVehicleViewId viewId);
	VIRTUAL TVehicleViewId GetNextView(TVehicleViewId viewId);
	// ~IVehicleSeat

	bool EnterRemotely(EntityId actorId);
	bool ExitRemotely();
	
	bool SitDown();
	bool StandUp();
	
	void GivesActorSeatFeatures(bool enabled = true, bool isRemote = false);
	bool SetAnimGraphInputs(EntityId actorId, bool writeOrClear, bool requestQuery = false, bool teleport = false);

	EntityId GetPassenger(bool remoteUser = false);
  IActor* GetPassengerActor(bool remoteUser = false);
	bool IsFree();

	void OnAction(const TVehicleActionId actionId, int activationMode, float value);
	void Update(float deltaTime);
	void UpdateView(SViewParams &viewParams);
  void UpdateRemote(IActor* pActor, float deltaTime);
  
  void UpdateSounds(float deltaTime);
  void StopSounds();

	void Serialize(TSerialize ser, EEntityAspects);
  void PostSerialize();
	CVehicleSeatSerializer *GetSerializer();
	void SetSerializer(CVehicleSeatSerializer *pSerializer);

	IVehicle* GetVehicle(){ return (IVehicle*)m_pVehicle; }
	IVehiclePart* GetAimPart() { return m_pAimPart; }
  
	float ProcessPassengerDamage(float actorHealth, float damage, const char* pDamageClass, bool explosion);

	VIRTUAL void OnPassengerDeath();
	VIRTUAL void UnlinkPassenger(bool ragdoll);
	VIRTUAL void ForceAnimGraphInputs();
  void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params);
  VIRTUAL void OnCameraShake(float& angleAmount, float& shiftAmount, const Vec3& pos, const char* source) const;

	// network
	EntityId NetGetPassengerId() const;
	void NetSetPassengerId(EntityId passengerId);

	// IMovementController
	virtual bool RequestMovement(CMovementRequest& request);
	virtual void GetMovementState(SMovementState& state);
	virtual bool GetStanceState(const SStanceStateQuery& query, SStanceState& state);
	// ~IMovementController

	// IFireController
	virtual bool RequestFire(bool bFire);
	// ~IFireController

	// IAnimationGraphStateListener
	virtual void SetOutput(const char * output, const char * value);
	virtual void QueryComplete(TAnimationGraphQueryID queryID, bool succeeded);
	virtual void DestroyedState(IAnimationGraphState*);
	// ~IAnimationGraphStateListener

	VIRTUAL void ForceFinishExiting() { QueryComplete( m_agVehicleQueryId, true ); }

	TVehicleSeatActionVector& GetSeatActions() { return m_seatActions; }

	bool IsPassengerClientActor() const;

	int GetSeatActionId(IVehicleSeatAction *pAction);
	IVehicleSeatAction *GetSeatActionById(int id);
	void ChangedNetworkState(NetworkAspectType aspects);

	const string& GetName() { return m_name; }
	TVehicleSeatId GetSeatId() { return m_seatId; }

	CGameChannel *GetGameChannel(EntityId actorId);

	VIRTUAL IVehicleHelper* GetSitHelper() { return m_pSitHelper; }
  virtual IVehicleHelper* GetEnterHelper() { return m_pEnterHelper; }
  virtual IVehicleHelper* GetExitHelper() { return m_pExitHelper ? m_pExitHelper : m_pEnterHelper; }

	VIRTUAL int GetCurrentTransition() const { return m_transitionType; }

	CVehicleSeatGroup* GetSeatGroup() { return m_pSeatGroup; }

	CVehicleSeatActionWeapons* GetSeatActionWeapons() { return m_pWeaponAction; }
	int GetSeatGroupIndex() { return m_seatGroupIndex; }

	// vehicle exit code.
  Matrix34 GetExitTM(IActor* pActor, bool opposite = false);	// get this seat's current exit TM
	bool TestExitPosition(IActor* pActor, Vec3& testPos, EntityId* pBlockingEntity);	// test a specific position to see if the actor can be placed there.

  void OnSeatFreed(TVehicleSeatId seatId, EntityId passengerId);
  CVehicleSeat* GetSeatUsedRemotely(bool onlyIfBeingUsed);

	bool IsPassengerShielded() const { return m_isPassengerShielded; };
  
public:

	bool m_isFiring;//TEMPORARY:firing status on this seat

protected:

	bool InitSeatActions(const CVehicleParams& seatTable);
	
  TVehicleSeatId GetNextSeatId(const TVehicleSeatId currentSeatId);

	IAnimationGraphState* GetAnimGraphState(EntityId actorId);  
	void AddStateListener(IAnimationGraphState* pState);
	void RemoveStateListener(IAnimationGraphState* pState);
  void SetAnimGraphInput(EntityId actorId, EAIAGInput input, const char* value);
	void ResetAnimGraphInput(EntityId actorId, EAIAGInput input);

	IVehicleClient* GetVehicleClient();
  void EnableVehicleActionMaps(bool enable);

  void UpdateHiddenState(bool hide);
	
  void UpdatePassengerLocalTM(IActor* pActor);
  bool QueueTransition();
	
	static CCryAction* m_pGameFramework;

	CVehicle* m_pVehicle;
	CVehicleSeatSerializer *m_pSerializer;
	EntityId								m_serializerId;

	string m_name;
	TVehicleSeatId m_seatId;

	EntityId m_passengerId;

	bool m_isDriver;
	bool m_isPassengerShielded;
  bool m_isPassengerHidden;
	bool m_isPassengerExposed;

	TVehicleViewVector m_views;
	TVehicleViewId m_currentView;
	TStringVector m_viewNames;
	
	IVehiclePart* m_pAimPart;

	friend class CVehicle;
	friend class CScriptBind_VehicleSeat;
	
	EntityId m_aiWeaponId;

	IVehicleHelper* m_pEnterHelper;
	IVehicleHelper* m_pExitHelper;
	IVehicleHelper* m_pSitHelper;

  Vec3 m_exitOffsetPlayer;
	bool m_movePassengerOnExit;
	Vec3 m_adjustedExitPos;

	TVehicleSeatActionVector m_seatActions;
		
	CVehicleSeatActionWeapons* m_pWeaponAction;
  CVehicleSeatActionRotateTurret* m_pTurretAction;
	CVehicleSeatGroup* m_pSeatGroup;

	TAnimationGraphQueryID m_agVehicleQueryId;
	TAnimationGraphQueryID m_agSeatQueryId;
	TAnimationGraphQueryID m_agDeathQueryId;
	TAnimationGraphQueryID m_agStartEnterQueryId;
	TAnimationGraphQueryID m_agStartEndQueryId;

	int m_transitionType;
	int m_prevTransitionType;
  int m_queuedTransition;

	std::list<IAnimationGraphState*> m_stateList;

  SSeatSoundParams m_soundParams;
  float m_explosionShakeMult;

	friend class CVehicleSeatGroup;
	friend class CScriptBind_Vehicle;
  
	bool m_isRagdollingOnDeath;

	string m_seatNameToUseForEntering;
	string m_useActionsRemotelyOfSeat; // only for init
  CVehicleSeat* m_pRemoteSeat;

	bool m_disableStopAllAnimationsOnEnter;
	bool m_skipViewBlending;

	float m_blurMult;
	float m_blurPrevVal;

	string m_agVehicleName;
	int m_agSeatNumber;
  
  // ugly variables for PostSerialize, needed to support recent changes in entity serialization
  EntityId m_postEnterId;
  EntityId m_postViewId;
  // ~ugly

	bool m_isPlayerViewBlending;
  bool m_isLocked;
  
	int m_seatGroupIndex;

	Vec3 m_exitWorldPos;	// set when player requests exit (to avoid repeated raycasts)
};

#endif
