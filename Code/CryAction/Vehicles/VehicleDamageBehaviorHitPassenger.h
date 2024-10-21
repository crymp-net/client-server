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

	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table) override;
	virtual void Reset() override {}
	virtual void Release() override { delete this; }

	virtual void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams) override;

	virtual void Serialize(TSerialize ser, unsigned aspects) override {}
	virtual void Update(const float deltaTime) override {}

	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override {}

	virtual void GetMemoryStatistics(ICrySizer* s) override;

protected:

	IVehicle* m_pVehicle;

	int m_damage;
	bool m_isDamagePercent;
};


#endif
