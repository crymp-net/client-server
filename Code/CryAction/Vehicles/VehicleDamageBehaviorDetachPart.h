/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior which detach a part with its 
children

-------------------------------------------------------------------------
History:
- 26:10:2005: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEDAMAGEBEHAVIORMOVEMENTDETACHPART_H__
#define __VEHICLEDAMAGEBEHAVIORMOVEMENTDETACHPART_H__

class CVehicle;
struct IParticleEffect;

class CVehicleDamageBehaviorDetachPart
	: public IVehicleDamageBehavior
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehicleDamageBehaviorDetachPart();
	virtual ~CVehicleDamageBehaviorDetachPart();

	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table);
	virtual bool Init(IVehicle* pVehicle, const string& partName, const string& effectName);
	virtual void Reset();
	virtual void Release() { delete this; }

	virtual void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams);
	
	virtual void Serialize(TSerialize ser, EEntityAspects aspects);
	virtual void Update(const float deltaTime) {}

  virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params){}

	virtual void GetMemoryStatistics(ICrySizer * s);

protected:

	IEntity* SpawnDetachedEntity();
	bool MovePartToTheNewEntity(IEntity* pTargetEntity, CVehiclePartBase* pPart);
	void AttachParticleEffect(IEntity* pDetachedEntity, IParticleEffect* pEffect);

	IVehicle* m_pVehicle;
	string m_partName;
	
	EntityId m_detachedEntityId;
	IStatObj* m_pDetachedStatObj;
  IParticleEffect* m_pEffect;
	bool m_pickableDebris;
};

#endif
