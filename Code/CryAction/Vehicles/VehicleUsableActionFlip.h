/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2010.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a usable action to recover a vehicle by flipping it over

-------------------------------------------------------------------------
History:
- Created by Stan Fichele

*************************************************************************/
#ifndef __VEHICLEUSABLEACTIONFLIP_H__
#define __VEHICLEUSABLEACTIONFLIP_H__

#include <vector>

class CVehicleUsableActionFlip
	: public IVehicleAction
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehicleUsableActionFlip() {}
	~CVehicleUsableActionFlip() {}

	// IVehicleAction
	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table);
	virtual void Reset();
	virtual void Release() { delete this; }

	virtual int OnEvent(int eventType, SVehicleEventParams& eventParams);
	void GetMemoryStatistics(ICrySizer * s);
	// ~IVehicleAction

	// IVehicleObject
	virtual void Serialize(TSerialize ser, EEntityAspects aspects) {}
	virtual void Update(const float deltaTime);
  virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params){}
	// ~IVehicleObject

protected:

	CVehicle* m_pVehicle;
	float m_timer;
	Vec3 m_localAngVel;
};

#endif
