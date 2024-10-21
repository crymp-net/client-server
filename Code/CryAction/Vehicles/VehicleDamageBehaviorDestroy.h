/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior which destroy a vehicle

-------------------------------------------------------------------------
History:
- 13:10:2005: Created by Mathieu Pinard

*************************************************************************/

#ifndef __VEHICLEDAMAGEBEHAVIORDESTROY_H__
#define __VEHICLEDAMAGEBEHAVIORDESTROY_H__

class CVehicle;
class CVehicleDamageBehaviorDestroy
	: public IVehicleDamageBehavior
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehicleDamageBehaviorDestroy() {}
	virtual ~CVehicleDamageBehaviorDestroy() {}

	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table) override;
	virtual void Reset() override;
	virtual void Release() override { delete this; }

	virtual void Serialize(TSerialize ser, unsigned aspects) override;
	virtual void Update(const float deltaTime) override {}

	virtual void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams) override;

	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override {}

	virtual void GetMemoryStatistics(ICrySizer* s) override { s->Add(*this); }

	virtual const string& GetEffectName() const { return m_effectName; }

protected:

	void SetDestroyed(bool isDestroyed, EntityId shooterId);

	CVehicle* m_pVehicle;
	string m_effectName;
};


#endif
