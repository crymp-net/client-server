/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a seat action for the steering wheel

-------------------------------------------------------------------------
History:
- 28:11:2005: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLESEATACTIONSTEERINGWHEEL_H__
#define __VEHICLESEATACTIONSTEERINGWHEEL_H__

#include "IAnimationGraph.h"

class CVehiclePartAnimatedJoint;
class CVehiclePartSubPartWheel;

class CVehicleSeatActionSteeringWheel
	: public IVehicleSeatAction,
	public IAnimationGraphStateListener
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehicleSeatActionSteeringWheel();
	~CVehicleSeatActionSteeringWheel() { Reset(); }

	// IVehicleSeatAction
	virtual bool Init(IVehicle* pVehicle, TVehicleSeatId seatId, const CVehicleParams& table);
	virtual void Reset();
	virtual void Release() { delete this; }

	virtual void StartUsing(EntityId passengerId);
	virtual void StopUsing();
	virtual void OnAction(const TVehicleActionId actionId, int activationMode, float value);
	
	virtual void Serialize(TSerialize ser, EEntityAspects aspects);
  virtual void PostSerialize();
	virtual void Update(const float deltaTime);

	virtual void GetMemoryStatistics(ICrySizer * s) {s->Add(*this);}
	// ~IVehicleSeatAction

	// IAnimationGraphStateListener
	virtual void SetOutput(const char * output, const char * value);
	virtual void QueryComplete(TAnimationGraphQueryID queryID, bool succeeded) {}
	virtual void DestroyedState(IAnimationGraphState*) {}
	// ~IAnimationGraphStateListener

  virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params){}

protected:

	ISkeletonAnim* GetActorSkeleton(EntityId actorId);
	void SetAnimManualUpdate(ISkeletonAnim* pSkeletonAnim, float value);

	IVehicle* m_pVehicle;

	IVehicleAnimation* m_pVehicleAnim;

	EntityId m_userId;
	bool m_isBeingUsed;
	bool m_isUsedSerialization;

	enum ESteeringClass
	{
		eSC_Generic = 1,
		eSC_Car,
	};

	ESteeringClass m_steeringClass;

	// car based steering
	CVehiclePartSubPartWheel* m_pFirstWheel;
	float m_initialRotation;
	float m_wheelRotationMax;
	float m_wheelRotationMult;

	// generic (non car) steering
	Vec3 m_steeringMaxAngles;
	Vec3 m_steeringActions;
	Vec3 m_steeringValues;
	float m_steeringRelaxMult;
	float m_steeringForce;
};

#endif
