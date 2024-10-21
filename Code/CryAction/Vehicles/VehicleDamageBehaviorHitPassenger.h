/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2007.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior group which gives hit to the 
passenger inside the vehicle

-------------------------------------------------------------------------
History:
- 30:07:2007: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEDAMAGEHITPASSENGER_H__
#define __VEHICLEDAMAGEHITPASSENGER_H__

class CVehicle;

class CVehicleDamageBehaviorHitPassenger
	: public IVehicleDamageBehavior
{
	IMPLEMENT_VEHICLEOBJECT

public:

	CVehicleDamageBehaviorHitPassenger();
	virtual ~CVehicleDamageBehaviorHitPassenger() {}

	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table);
	virtual void Reset() {}
	virtual void Release() { delete this; }

	virtual void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams);

	virtual void Serialize(TSerialize ser, EEntityAspects aspects) {}
	virtual void Update(const float deltaTime) {}

	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params){}

	virtual void GetMemoryStatistics(ICrySizer * s);

protected:

	IVehicle* m_pVehicle;

	int m_damage;
	bool m_isDamagePercent;
};

#endif
