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

	virtual bool Init(IVehicle* pVehicle, TVehicleSeatId seatId, const CVehicleParams& table);
	virtual void Reset();
	virtual void Release() { delete this; }

	virtual void StartUsing(EntityId passengerId) {}
	virtual void StopUsing();
	virtual void OnAction(const TVehicleActionId actionId, int activationMode, float value);

	virtual void Serialize(TSerialize ser, EEntityAspects aspects);
  virtual void PostSerialize(){}
	virtual void Update(const float deltaTime) {}

  virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params);

	virtual void GetMemoryStatistics(ICrySizer * s);

protected:
  void ToggleLights(bool enable);

  enum ELightActivation
  {
    // add more activation modes here if needed    
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
