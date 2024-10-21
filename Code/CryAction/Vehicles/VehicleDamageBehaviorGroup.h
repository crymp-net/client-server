/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior group which handles delays and
some randomness

-------------------------------------------------------------------------
History:
- 23:02:2006: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEDAMAGEBEHAVIORGROUP_H__
#define __VEHICLEDAMAGEBEHAVIORGROUP_H__

class CVehicle;

class CVehicleDamageBehaviorGroup
	: public IVehicleDamageBehavior
{
	IMPLEMENT_VEHICLEOBJECT

public:

	CVehicleDamageBehaviorGroup() {}
	virtual ~CVehicleDamageBehaviorGroup() {}

	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table) override;
	virtual void Reset() override;
	virtual void Release() override { delete this; }

	virtual void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams) override;

	virtual void Serialize(TSerialize ser, unsigned aspects) override;
	virtual void Update(const float deltaTime) override;

	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override {}

	virtual void GetMemoryStatistics(ICrySizer* s) override;

protected:

	CVehicle* m_pVehicle;
	string m_damageGroupName;
};


#endif
