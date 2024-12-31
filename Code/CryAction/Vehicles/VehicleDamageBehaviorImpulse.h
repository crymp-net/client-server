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

	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table) override;
	virtual void Reset() override;
	virtual void Release() override { delete this; }

	virtual void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams) override;

	virtual void Serialize(TSerialize ser, unsigned aspects) override {};
	virtual void Update(const float deltaTime) override {}

	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override {};
	virtual void GetMemoryStatistics(ICrySizer* s) override { s->Add(*this); }

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