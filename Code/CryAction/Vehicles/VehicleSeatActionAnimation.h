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
	virtual bool Init(IVehicle* pVehicle, TVehicleSeatId seatId, const CVehicleParams& table);
	virtual void Reset();
	virtual void Release() { delete this; }

	virtual void StartUsing(EntityId passengerId);
	virtual void StopUsing();
	virtual void OnAction(const TVehicleActionId actionId, int activationMode, float value);
	
	virtual void Serialize(TSerialize ser, EEntityAspects aspects);
  virtual void PostSerialize(){}
	virtual void Update(const float deltaTime);

	virtual void GetMemoryStatistics(ICrySizer * s) {s->Add(*this);}
	// ~IVehicleSeatAction

  virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params){}

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
