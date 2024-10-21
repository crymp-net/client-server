/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements an effect damage behavior

-------------------------------------------------------------------------
History:
- 13:10:2005: Created by Mathieu Pinard

*************************************************************************/

#ifndef __VEHICLEDAMAGEBEHAVIOREFFECT_H__
#define __VEHICLEDAMAGEBEHAVIOREFFECT_H__

class CVehicleDamageBehaviorEffect
	: public IVehicleDamageBehavior
{
	IMPLEMENT_VEHICLEOBJECT
public:

  CVehicleDamageBehaviorEffect() {}
	virtual ~CVehicleDamageBehaviorEffect() {}

	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table);
	virtual void Reset();
	virtual void Release() { delete this; }

	virtual void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams);
	
	virtual void Serialize(TSerialize ser, EEntityAspects aspects);
	virtual void Update(const float deltaTime)  {}

  virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params){}

	virtual void GetMemoryStatistics(ICrySizer * s);

protected:

  void LoadEffect(IVehicleComponent* pComponent);
  void UpdateEffect(float randomness, float damageRatio);

	IVehicle* m_pVehicle;
  float m_damageRatioMin;
  string m_effectName;
	SDamageEffect m_damageEffect;
  	
	int m_slot;
	float m_scale;
  
	bool m_disableAfterExplosion;
};

#endif
