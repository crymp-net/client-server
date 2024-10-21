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
	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table) override;
	virtual void Reset() override;
	virtual void Release() override { delete this; }

	virtual int OnEvent(int eventType, SVehicleEventParams& eventParams) override;
	virtual void Serialize(TSerialize ser, unsigned aspects) override {}
	virtual void Update(const float deltaTime) override;
	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override {}
	// ~IVehicleAction

	// Additional methods
	void GetMemoryStatistics(ICrySizer* s);

protected:

	CVehicle* m_pVehicle;
	float m_timer;
	Vec3 m_localAngVel;
};


#endif
