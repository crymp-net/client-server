/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2010.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior which disables
							seat actions for a specified seat

-------------------------------------------------------------------------
History:
- 23:07:2010: Created by SNH

*************************************************************************/

#ifndef __VEHICLEDAMAGEBEHAVIORDISABLESEATACTION_H__
#define __VEHICLEDAMAGEBEHAVIORDISABLESEATACTION_H__

class CVehicle;

class CVehicleDamageBehaviorDisableSeatAction
	: public IVehicleDamageBehavior
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehicleDamageBehaviorDisableSeatAction();
	virtual ~CVehicleDamageBehaviorDisableSeatAction() {}

	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table) override;
	virtual void Reset() override;
	virtual void Release() override { delete this; }

	virtual void Serialize(TSerialize ser, unsigned aspects) override {};
	virtual void Update(const float deltaTime) override {};

	virtual void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams) override;

	virtual void GetMemoryStatistics(ICrySizer* s) override;

	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override {}

	virtual void GetMemoryUsage(ICrySizer* s) const override { s->Add(*this); }

protected:

	CVehicle* m_pVehicle;
	string m_seatName;
	string m_seatActionName;
};


#endif


