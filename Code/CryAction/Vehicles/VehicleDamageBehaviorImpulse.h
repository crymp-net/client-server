/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior which apply an impulse to a 
vehicle

-------------------------------------------------------------------------
History:
- 14:10:2005: Created by Mathieu Pinard

*************************************************************************/

#ifndef __VEHICLEDAMAGEBEHAVIORIMPULSE_H__
#define __VEHICLEDAMAGEBEHAVIORIMPULSE_H__

class CVehicle;

class CVehicleDamageBehaviorImpulse
	: public IVehicleDamageBehavior
{
	IMPLEMENT_VEHICLEOBJECT
public:

  CVehicleDamageBehaviorImpulse();
	virtual ~CVehicleDamageBehaviorImpulse() {}

	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table);
	virtual void Reset();
	virtual void Release() { delete this; }

	virtual void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams);
	
	virtual void Serialize(TSerialize ser, EEntityAspects aspects) {}
	virtual void Update(const float deltaTime) {}

  virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params){}
	virtual void GetMemoryStatistics(ICrySizer * s) { s->Add(*this); }

protected:

	CVehicle* m_pVehicle;

	float m_forceMin;
	float m_forceMax;
	Vec3 m_impulseDir;
	Vec3 m_angImpulse;
  
	IVehicleHelper* m_pImpulseLocation;
  bool m_worldSpace;
};

#endif
