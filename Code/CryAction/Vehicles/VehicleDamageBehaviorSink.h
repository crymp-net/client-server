/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior which sink the vehicle

-------------------------------------------------------------------------
History:
- 03:11:2005: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEDAMAGEBEHAVIORSINK_H__
#define __VEHICLEDAMAGEBEHAVIORSINK_H__

class CVehicle;

class CVehicleDamageBehaviorSink
	: public IVehicleDamageBehavior
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehicleDamageBehaviorSink() {}
	virtual ~CVehicleDamageBehaviorSink() {}

	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table);
	virtual void Reset();
	virtual void Release() { delete this; }

	virtual void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams);
	
	virtual void Serialize(TSerialize ser, EEntityAspects aspects);
	virtual void Update(const float deltaTime) {}

  virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params);

	void ChangeSinkingBehavior(bool isSinking);

	virtual void GetMemoryStatistics(ICrySizer * s) { s->Add(*this); }

protected:

	IVehicle* m_pVehicle;
	
  float m_formerWaterDensity;  
  int m_sinkingTimer;
  bool m_isSinking;
};

#endif
