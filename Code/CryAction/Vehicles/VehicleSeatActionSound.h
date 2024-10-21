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

	virtual bool Init(IVehicle* pVehicle, TVehicleSeatId seatId, const CVehicleParams& table);
	virtual void Reset() {}
	virtual void Release() { delete this; }

	virtual void StartUsing(EntityId passengerId) {}
	virtual void StopUsing();
	virtual void OnAction(const TVehicleActionId actionId, int activationMode, float value);

	virtual void Serialize(TSerialize ser, EEntityAspects aspects);
  virtual void PostSerialize(){}
	virtual void Update(const float deltaTime) {};

	virtual void PlaySound(bool play);

  virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params){}

	virtual void GetMemoryStatistics(ICrySizer * s);

protected:

	IVehicle* m_pVehicle;
	IVehicleHelper* m_pHelper;
	TVehicleSeatId m_seatId;

	bool m_enabled;

	tSoundID m_soundId;
	string m_soundName;
};

#endif
