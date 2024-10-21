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

	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table) override;
	virtual void Reset() override;
	virtual void Release() override { delete this; }

	virtual void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams) override;

	virtual void Serialize(TSerialize ser, unsigned aspects) override;
	virtual void Update(const float deltaTime) override {}

	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override {}

	virtual void GetMemoryStatistics(ICrySizer* s) override;

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
