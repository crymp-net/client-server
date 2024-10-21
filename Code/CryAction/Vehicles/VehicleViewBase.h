/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a base class for the vehicle views

-------------------------------------------------------------------------
History:
- 06:07:2006: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEVIEWBASE_H__
#define __VEHICLEVIEWBASE_H__
class CVehicleViewBase : public IVehicleView
{
public:
	CVehicleViewBase();

	// IVehicleView
	virtual bool Init(CVehicleSeat* pSeat, const CVehicleParams& table) override;
	virtual void Reset() override;
	virtual void ResetPosition() override {};

	virtual const char* GetName() override { return nullptr; }
	virtual bool IsThirdPerson() override = 0;
	virtual bool IsPassengerHidden() override { return m_hidePlayer; }

	virtual void OnStartUsing(EntityId passengerId) override;
	virtual void OnStopUsing() override;

	virtual void OnAction(const TVehicleActionId actionId, int activationMode, float value) override;
	virtual void UpdateView(SViewParams& viewParams, EntityId playerId) override {};

	virtual void Update(const float frameTime) override;
	virtual void Serialize(TSerialize serialize, unsigned int v) override;

	virtual void SetDebugView(bool debug) override { m_isDebugView = debug; }
	virtual bool IsDebugView() override { return m_isDebugView; }

	virtual bool ShootToCrosshair() override { return true; }
	virtual bool IsAvailableRemotely() const override { return m_isAvailableRemotely; }
	// ~IVehicleView

	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override {};

	// Added methods
	virtual void Release() override { delete this; } 

	virtual void GetMemoryStatistics(ICrySizer* s) override { s->Add(*this); }
	// ~IVehicleView

	bool Init(CVehicleSeat* pSeat);

protected:
	IVehicle* m_pVehicle;
	CVehicleSeat* m_pSeat;

	// view settings (changed only inside Init)
	bool m_isRotating;

	Vec3 m_rotationMin;
	Vec3 m_rotationMax;
	Vec3 m_rotationInit;
	float m_relaxDelayMax;
	float m_relaxTimeMax;
	float m_velLenMin;
	float m_velLenMax;

	bool m_isSendingActionOnRotation;
	float m_rotationBoundsActionMult;

	// status variables (changed during run-time)
	Ang3 m_rotation;
	Vec3 m_rotatingAction;
	Ang3 m_viewAngleOffset;

	bool m_isRelaxEnabled;
	float m_relaxDelay;
	float m_relaxTime;

	int m_yawLeftActionOnBorderAAM;
	int m_yawRightActionOnBorderAAM;
	int m_pitchUpActionOnBorderAAM;
	int m_pitchDownActionOnBorderAAM;

	float m_pitchVal;
	float m_yawVal;

	static ICVar* m_pSensitivity;

	struct SViewGeneratedAction
	{
		TVehicleActionId actionId;
		int activationMode;
	};

	bool m_hidePlayer;
	bool m_isDebugView;
	bool m_isAvailableRemotely;
	bool m_playerViewThirdOnExit;

	typedef std::vector<string> TVehiclePartNameVector;
	TVehiclePartNameVector m_hideParts;
};

#endif
