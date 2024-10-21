/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a seat action to control vehicle weapons

-------------------------------------------------------------------------
History:
- 06:12:2005: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLESEATACTIONWEAPONS_H__
#define __VEHICLESEATACTIONWEAPONS_H__

#include <list>

#include "CryCommon/CryAction/IWeapon.h"

struct IWeapon;

struct SWeaponAction
{  
  std::vector<string> animations;

	void GetMemoryStatistics(ICrySizer * s)
	{
		s->AddContainer(animations);
		for (size_t i=0; i<animations.size(); i++)
			s->Add(animations[i]);
	}
};
typedef std::map<string, SWeaponAction> TWeaponActionMap;

typedef std::vector <IVehicleHelper*> THelperVector;

struct SVehicleWeapon
{		
  TWeaponActionMap actions;	  
  THelperVector helpers;  
  EntityId weaponEntityId;
  IVehiclePart* pPart;
  bool inheritVelocity;

  SVehicleWeapon()
  {
    weaponEntityId = 0;
    pPart = 0;
    inheritVelocity = true;
  }

	void GetMemoryStatistics(ICrySizer * s)
	{
		s->AddContainer(actions);
		s->AddContainer(helpers);
		for (TWeaponActionMap::iterator iter = actions.begin(); iter != actions.end(); ++iter)
		{
			s->Add(iter->first);
			iter->second.GetMemoryStatistics(s);
		}
	}
};


class CVehicleSeatActionWeapons
	: public IVehicleSeatAction,
	public IWeaponFiringLocator,
    public IWeaponEventListener
{
	IMPLEMENT_VEHICLEOBJECT
public:

    CVehicleSeatActionWeapons();
	~CVehicleSeatActionWeapons();

	// IVehicleSeatAction
	virtual bool Init(IVehicle* pVehicle, TVehicleSeatId seatId, const CVehicleParams& table) override;
	virtual void Reset() override;
	virtual void Release() override { delete this; }

	virtual void StartUsing(EntityId passengerId) override;
	virtual void StopUsing() override;
	virtual void OnAction(const TVehicleActionId actionId, int activationMode, float value) override;
	
	virtual void Serialize(TSerialize ser, unsigned aspects) override;
    virtual void PostSerialize() override;
	virtual void Update(const float deltaTime) override;

	virtual void GetMemoryStatistics(ICrySizer * s) override;
	// ~IVehicleSeatAction

	// IWeaponFiringLocator  
    virtual bool GetProbableHit(EntityId weaponId, const IFireMode* pFireMode, Vec3& hit) override;
	virtual bool GetFiringPos(EntityId weaponId, const IFireMode* pFireMode, Vec3& pos) override;  
	virtual bool GetFiringDir(EntityId weaponId, const IFireMode* pFireMode, Vec3& dir, const Vec3& probableHit, const Vec3& firingPos) override;  
	virtual bool GetActualWeaponDir(EntityId weaponId, const IFireMode* pFireMode, Vec3& dir, const Vec3& probableHit, const Vec3& firingPos) override;  
	virtual bool GetFiringVelocity(EntityId weaponId, const IFireMode* pFireMode, Vec3& vel, const Vec3& firingDir) override;  
	virtual void WeaponReleased() override {}
	// ~IWeaponFiringLocator

    // IWeaponEventListener
    virtual void OnShoot(IWeapon *pWeapon, EntityId shooterId, EntityId ammoId, IEntityClass* pAmmoType,
        const Vec3 &pos, const Vec3 &dir, const Vec3 &vel) override;
    virtual void OnStartFire(IWeapon *pWeapon, EntityId shooterId) override {}
    virtual void OnStopFire(IWeapon *pWeapon, EntityId shooterId) override {}
	//virtual void OnFireModeChanged(IWeapon *pWeapon, int currentFireMode) override {}
    virtual void OnStartReload(IWeapon *pWeapon, EntityId shooterId, IEntityClass* pAmmoType) override {}
    virtual void OnEndReload(IWeapon *pWeapon, EntityId shooterId, IEntityClass* pAmmoType) override {}
	//virtual void OnSetAmmoCount(IWeapon *pWeapon, EntityId shooterId) override {}
    virtual void OnOutOfAmmo(IWeapon *pWeapon, IEntityClass* pAmmoType) override;
    virtual void OnReadyToFire(IWeapon *pWeapon) override {}
    virtual void OnPickedUp(IWeapon *pWeapon, EntityId actorId, bool destroyed) override {}
    virtual void OnDropped(IWeapon *pWeapon, EntityId actorId) override {}
	virtual void OnMelee(IWeapon* pWeapon, EntityId shooterId) override {}
	virtual void OnStartTargetting(IWeapon *pWeapon) override {}
	virtual void OnStopTargetting(IWeapon *pWeapon) override {}
	virtual void OnSelected(IWeapon *pWeapon, bool select) override {}
    // ~IWeaponEventListener

	Vec3 GetAverageFiringPos();
    
    virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override;

	void StartFire();
	void StopFire();
	bool IsFiring() const { return m_isShooting; }
	void SetFireTarget(const Vec3& target) { m_fireTarget = target; }

    bool IsSecondary() const { return m_isSecondary; }
  
	unsigned int GetWeaponCount();
	unsigned int GetCurrentWeapon() { return m_weaponIndex; }  
	
    EntityId GetWeaponEntityId(unsigned int index);
	void SetWeaponEntityId(unsigned int index, EntityId weaponId);
    
    IItem* GetIItem(unsigned int index);
    IWeapon* GetIWeapon(unsigned int index);

    IItem* GetIItem(const SVehicleWeapon& vehicleWeapon);	
    IWeapon* GetIWeapon(const SVehicleWeapon& vehicleWeapon);

    SVehicleWeapon* GetVehicleWeapon(EntityId weaponId);
  
protected:

    void SetupWeapon(SVehicleWeapon& weapon);

	SVehicleWeapon& GetVehicleWeapon();
	SVehicleWeapon& GetWeaponInfo(int weaponIndex);
		
	IVehicleHelper* GetHelper(SVehicleWeapon& vehicleWeapon);  
    IActor* GetUserActor();

protected:

	int GetSkipEntities(IPhysicalEntity** pSkipEnts, int nMaxSkip);

    void UpdateWeaponTM(SVehicleWeapon& weapon);
    IEntity* GetEntity(const SVehicleWeapon& weapon);

	IVehicle* m_pVehicle;
	string m_partName;
	TVehicleSeatId m_seatId;
	
	EntityId m_passengerId;

	typedef std::vector<SVehicleWeapon> TVehicleWeaponVector;
	TVehicleWeaponVector m_weapons;
	TVehicleWeaponVector m_weaponsCopy;
	
	int m_weaponIndex;
	bool m_isShooting;
	int m_lastActionActivationMode;

	bool m_isUsingShootingByUpdate;
	bool m_isSecondary;

	bool m_isShootingToCrosshair;
	Vec3 m_fireTarget;
};


#endif
