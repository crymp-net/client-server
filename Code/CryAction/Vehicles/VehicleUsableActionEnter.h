/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a usable action to enter a vehicle seat

-------------------------------------------------------------------------
History:
- 19:01:2006: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEUSABLEACTIONENTER_H__
#define __VEHICLEUSABLEACTIONENTER_H__

#include <vector>

class CVehicleSeat;

class CVehicleUsableActionEnter
	: public IVehicleAction
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehicleUsableActionEnter() {}
	~CVehicleUsableActionEnter() {}

	// IVehicleAction
	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table);
	virtual void Reset() {}
	virtual void Release() { delete this; }

	virtual int OnEvent(int eventType, SVehicleEventParams& eventParams);
	void GetMemoryStatistics(ICrySizer * s);
	// ~IVehicleAction

	// IVehicleObject
	virtual void Serialize(TSerialize ser, EEntityAspects aspects) {}
	virtual void Update(const float deltaTime) {}
  virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params){}
	// ~IVehicleObject

protected:

	bool IsSeatAvailable(TVehicleSeatId seatId, EntityId userId);

	CVehicle* m_pVehicle;

	typedef std::vector <TVehicleSeatId> TVehicleSeatIdVector;
	TVehicleSeatIdVector m_seatIds;
};

#endif
