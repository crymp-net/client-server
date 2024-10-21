/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior which send an AI signal

-------------------------------------------------------------------------
History:
- 23:11:2006: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEDAMAGEBEHAVIORAISIGNAL_H__
#define __VEHICLEDAMAGEBEHAVIORAISIGNAL_H__

class CVehicle;

class CVehicleDamageBehaviorAISignal
	: public IVehicleDamageBehavior
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehicleDamageBehaviorAISignal() {}
	virtual ~CVehicleDamageBehaviorAISignal() {}

	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table) override;
	virtual void Reset() override {}
	virtual void Release() override { delete this; }

	virtual void Serialize(TSerialize ser, unsigned aspects) override {}
	virtual void Update(const float deltaTime) override {}

	virtual void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams) override;
	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override {}

	virtual void GetMemoryStatistics(ICrySizer* s) override;

protected:

	IVehicle* m_pVehicle;

	int m_signalId;
	string m_signalText;
};


#endif
