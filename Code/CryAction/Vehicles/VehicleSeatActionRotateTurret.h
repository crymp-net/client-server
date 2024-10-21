/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a seat action to rotate a turret

-------------------------------------------------------------------------
History:
- 14:12:2005: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLESEATACTIONROTATETURRET_H__
#define __VEHICLESEATACTIONROTATETURRET_H__

#include "InterpolationHelpers.h"

class CVehiclePartBase;

// holds information for each of the rotation types
struct SVehiclePartRotationParameters
{
	SVehiclePartRotationParameters()
	{
		m_pPart = NULL;
		m_action = 0.0f;
		m_currentValue = 0.0f;
		m_previousValue = 0.0f;
		m_speed = 0.0f;
		m_acceleration = 0.0f;
		m_minLimit = 0.0f;
		m_maxLimit = 0.0f;
		m_relSpeed = 0.0f;
		m_rotWorldSpace = 0.0f;
		m_prevWorldQuat = IDENTITY;
		m_prevBaseTM.SetIdentity();
		m_orientation = InterpolatedQuat(Quat::CreateIdentity(), 0.75f, 3.0f);
		m_turnSoundId = InvalidSoundEventId;
		m_damageSoundId = InvalidSoundEventId;
		m_rotationChanged = false;
		m_hasReceivedAction = false;
	}

	CVehiclePartBase* m_pPart;	// TODO: IVehiclePart*?

	float m_action;							// what the user input is requesting
	float m_currentValue;				// current rotation
	float m_previousValue;			// previous rotation
	float m_speed;							// speed of rotation (from vehicle xml)
	float m_acceleration;				// acceleration of rotation (from vehicle xml)

	float m_minLimit;						// smallest permissable value (from xml)
	float m_maxLimit;						// largest permissable value (from xml)

	float m_relSpeed;						// used to interpolate the rotation speed (for sound)
	float m_rotWorldSpace;			// rotation in world space

	Quat m_prevWorldQuat;				//	previous world rotation
	InterpolatedQuat m_orientation;	//	Interpolated orientation for this part
	Matrix34 m_prevBaseTM;			// previous tm for this part

	TVehicleSoundEventId m_turnSoundId;
	TVehicleSoundEventId m_damageSoundId;

	bool m_rotationChanged;			// has rotation changed this frame
	bool m_hasReceivedAction;		// got an action this frame
};

class CVehicleSeatActionRotateTurret
	: public IVehicleSeatAction
{
	IMPLEMENT_VEHICLEOBJECT
public:

	// IVehicleSeatAction
	virtual bool Init(IVehicle* pVehicle, TVehicleSeatId seatId, const CVehicleParams& table) override;
	virtual void Reset() override;

	virtual void StartUsing(EntityId passengerId) override;
	virtual void StopUsing() override;
	virtual void OnAction(const TVehicleActionId actionId, int activationMode, float value) override;

	virtual void Serialize(TSerialize ser, unsigned aspects) override;
	virtual void PostSerialize() override {}
	virtual void Update(const float deltaTime) override;

	virtual void GetMemoryStatistics(ICrySizer* s) override { s->Add(*this); }

	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override {}
	virtual void Release() override { delete this; } 
	// ~IVehicleSeatAction

	void SetAimGoal(Vec3 aimPos, int priority = 0);
	const Vec3& GetAimGoal();

	virtual bool GetRotationLimits(int axis, float& min, float& max);

protected:

	enum EVehicleTurretRotationType
	{
		eVTRT_Pitch = 0,
		eVTRT_Yaw,
		eVTRT_NumRotationTypes,
	};

	void UpdatePartRotation(EVehicleTurretRotationType eType, float frameTime);
	float GetDamageSpeedMul(CVehiclePartBase* pPart);

	bool InitRotation(IVehicle* pVehicle, const CVehicleParams& rotationTable, EVehicleTurretRotationType eType);
	bool InitRotationSounds(const CVehicleParams& rotationParams, EVehicleTurretRotationType eType);
	void UpdateRotationSound(EVehicleTurretRotationType eType, float deltaTime);

	CVehicle* m_pVehicle;
	IEntity* m_pUserEntity;
	TVehicleSeatId m_seatId;

	Vec3 m_aimGoal;
	int m_aimGoalPriority;

	SVehiclePartRotationParameters m_rotations[eVTRT_NumRotationTypes];

	IVehicleHelper* m_rotTestHelpers[2];
	string m_rotTestHelperNames[2];
	float m_rotTestRadius;

	friend class CVehiclePartBase;
};


#endif
