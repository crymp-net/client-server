/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a seat action to handle IK on the passenger body

-------------------------------------------------------------------------
History:
- 10:01:2006: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLESEATACTIONPASSENGERIK_H__
#define __VEHICLESEATACTIONPASSENGERIK_H__

#include <vector>

class CVehicleSeatActionPassengerIK
	: public IVehicleSeatAction
{
	IMPLEMENT_VEHICLEOBJECT
public:

	virtual bool Init(IVehicle* pVehicle, TVehicleSeatId seatId, const CVehicleParams& table);
	virtual void Reset();
	virtual void Release() { delete this; }

	virtual void StartUsing(EntityId passengerId);
	virtual void StopUsing();
	virtual void OnAction(const TVehicleActionId actionId, int activationMode, float value) {}

	virtual void Serialize(TSerialize ser, EEntityAspects aspects) {}
  virtual void PostSerialize(){}
	virtual void Update(const float deltaTime);

  virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params){}

	virtual void GetMemoryStatistics(ICrySizer * s);

protected:

	IVehicle* m_pVehicle;
	EntityId m_passengerId;
	
	struct SIKLimb
	{
		string limbName;
		IVehicleHelper* pHelper;
	};

	typedef std::vector <SIKLimb> TIKLimbVector;
	TIKLimbVector m_ikLimbs;

	bool m_waitShortlyBeforeStarting;
};

#endif
