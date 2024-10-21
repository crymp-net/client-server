/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior which notify the movement class

-------------------------------------------------------------------------
History:
- 17:10:2005: Created by Mathieu Pinard

*************************************************************************/

#ifndef __VEHICLEDAMAGEBEHAVIORMOVEMENTNOTIFICATION_H__
#define __VEHICLEDAMAGEBEHAVIORMOVEMENTNOTIFICATION_H__

class CVehicle;

class CVehicleDamageBehaviorMovementNotification
	: public IVehicleDamageBehavior
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehicleDamageBehaviorMovementNotification();
	virtual ~CVehicleDamageBehaviorMovementNotification() {}

	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table);
	virtual void Reset();
	virtual void Release() { delete this; }

	virtual void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams);
	
	virtual void Serialize(TSerialize ser, EEntityAspects aspects) {}
	virtual void Update(const float deltaTime) {}

  virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params){}
	virtual void GetMemoryStatistics(ICrySizer * s) { s->Add(*this); }

protected:

	IVehicle* m_pVehicle;

	bool m_isSteeringInvolved;
  bool m_isFatal;
	bool m_isDamageAlwaysFull;

	int m_param;
};

#endif
