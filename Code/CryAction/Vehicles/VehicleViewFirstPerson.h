/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements the first person pit view for vehicles

-------------------------------------------------------------------------
History:
- 29:01:2006: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEVIEWFIRSTPERSON_H__
#define __VEHICLEVIEWFIRSTPERSON_H__

#include "VehicleViewBase.h"

class CVehicleViewFirstPerson
	: public CVehicleViewBase
{
	IMPLEMENT_VEHICLEOBJECT;
public:

  CVehicleViewFirstPerson();
  ~CVehicleViewFirstPerson() {}

	// IVehicleView
	virtual bool Init(CVehicleSeat* pSeat, const CVehicleParams& table);
  virtual void Reset();

	virtual const char* GetName() { return m_name; }
	virtual bool IsThirdPerson() { return false; }

  virtual void OnStartUsing(EntityId playerId);
  virtual void OnStopUsing();  
  
	virtual void Update(const float frameTime);
	virtual void UpdateView(SViewParams &viewParams, EntityId playerId = 0);

	void GetMemoryStatistics(ICrySizer * s);
	// ~IVehicleView

	// IVehicleEventListener
	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params);
	// ~IVehicleEventListener

protected:

	Vec3 GetWorldPosGoal();
	Quat GetWorldRotGoal();
	Quat GetVehicleRotGoal();

  void HideEntitySlots(IEntity* pEnt, bool hide);

	static const char* m_name;
	
  IVehicleHelper* m_pHelper;
	Vec3 m_offset;  
  float m_relToHorizon;
  bool m_hideVehicle; 
  int m_frameSlot;

  typedef std::multimap<EntityId, int> TSlots;
  TSlots m_slotFlags;
  
	Vec3 m_viewPosition;
	Quat m_viewRotation;

	float m_speedPos;
	float m_speedRot;

	EntityId m_passengerId;  
};

#endif

