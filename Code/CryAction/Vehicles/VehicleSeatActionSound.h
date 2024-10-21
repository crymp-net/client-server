/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a seat action for sounds (ie: honk on trucks)

-------------------------------------------------------------------------
History:
- 16:11:2005: Created by Mathieu Pinard

*************************************************************************/

#ifndef __VEHICLESEATACTIONSOUND_H__
#define __VEHICLESEATACTIONSOUND_H__

class CVehicleSeatActionSound
	: public IVehicleSeatAction
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehicleSeatActionSound();

	virtual bool Init(IVehicle* pVehicle, TVehicleSeatId seatId, const CVehicleParams& table) override;
	virtual void Reset() override {}

	virtual void StartUsing(EntityId passengerId) override {}
	virtual void StopUsing() override;
	virtual void OnAction(const TVehicleActionId actionId, int activationMode, float value) override;

	virtual void Serialize(TSerialize ser, unsigned aspects) override;
	virtual void PostSerialize() override {}
	virtual void Update(const float deltaTime) override {}

	virtual void Release() override { delete this; } // Added missing Release method
	virtual void PlaySound(bool play);

	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override {}

	virtual void GetMemoryStatistics(ICrySizer* s) override;

protected:

	IVehicle* m_pVehicle;
	IVehicleHelper* m_pHelper;
	TVehicleSeatId m_seatId;

	bool m_enabled;

	tSoundID m_soundId;
	string m_soundName;
};


#endif
