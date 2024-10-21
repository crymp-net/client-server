/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a seat action to head/spot lights

-------------------------------------------------------------------------
History:
- 01:03:2006: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLESEATACTIONLIGHTS_H__
#define __VEHICLESEATACTIONLIGHTS_H__

#include <vector>

class CVehiclePartLight;

class CVehicleSeatActionLights
	: public IVehicleSeatAction
{
	IMPLEMENT_VEHICLEOBJECT
public:

	virtual bool Init(IVehicle* pVehicle, TVehicleSeatId seatId, const CVehicleParams& table) override;
	virtual void Reset() override;

	virtual void StartUsing(EntityId passengerId) override {}
	virtual void StopUsing() override;
	virtual void OnAction(const TVehicleActionId actionId, int activationMode, float value) override;

	virtual void Serialize(TSerialize ser, unsigned aspects) override;
	virtual void PostSerialize() override {}
	virtual void Update(const float deltaTime) override {}

	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override;

	virtual void GetMemoryStatistics(ICrySizer* s) override;

	virtual void Release() override { delete this; }  // Added missing Release method

protected:
	void ToggleLights(bool enable);

	enum ELightActivation
	{
		eLA_Toggle = 0,
		eLA_Brake,
		eLA_Reversing,
	};

	struct SLightPart
	{
		CVehiclePartLight* pPart;

		SLightPart()
		{
			pPart = 0;
		}
	};

	ELightActivation m_activation;
	bool m_enabled;
	IVehicle* m_pVehicle;
	TVehicleSeatId m_seatId;
	_smart_ptr<ISound> m_pSound;

	typedef std::vector<SLightPart> TVehiclePartLightVector;
	TVehiclePartLightVector m_lightParts;
};


#endif
