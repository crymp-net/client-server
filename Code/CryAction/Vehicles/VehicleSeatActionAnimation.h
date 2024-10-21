/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2007.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a seat action for triggering animations

-------------------------------------------------------------------------
History:
- 20:07:2007: Created by MichaelR

*************************************************************************/
#ifndef __VEHICLESEATACTIONANIMATION_H__
#define __VEHICLESEATACTIONANIMATION_H__


class CVehiclePartAnimatedJoint;

class CVehicleSeatActionAnimation
	: public IVehicleSeatAction
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehicleSeatActionAnimation();
	~CVehicleSeatActionAnimation() { Reset(); }

	// IVehicleSeatAction
	virtual bool Init(IVehicle* pVehicle, TVehicleSeatId seatId, const CVehicleParams& table) override;
	virtual void Reset() override;

	virtual void StartUsing(EntityId passengerId) override;
	virtual void StopUsing() override;
	virtual void OnAction(const TVehicleActionId actionId, int activationMode, float value) override;

	virtual void Serialize(TSerialize ser, unsigned aspects) override;
	virtual void PostSerialize() override {}
	virtual void Update(const float deltaTime) override;

	virtual void GetMemoryStatistics(ICrySizer* s) override { s->Add(*this); }
	virtual void Release() override { delete this; }
	// ~IVehicleSeatAction

	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override {}

protected:

	IVehicle* m_pVehicle;
	IVehicleAnimation* m_pVehicleAnim;
	EntityId m_userId;
	TVehicleActionId m_control[2];

	IEntitySoundProxy* m_pSoundProxy;
	_smart_ptr<ISound> m_pSound;
	_smart_ptr<ISound> m_pStopSound;

	float m_action;
	float m_prevAction;
	float m_speed;
	bool m_manualUpdate;
};

#endif
