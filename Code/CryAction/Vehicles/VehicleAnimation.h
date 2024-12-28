/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: 

-------------------------------------------------------------------------
History:
- 22:03:2006: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEANIMATION_H__
#define __VEHICLEANIMATION_H__

#include <vector>
#include "VehicleParams.h"

class CVehiclePartAnimated;

class CVehicleAnimation
	: 
	public IVehicleAnimation
{
public:

	CVehicleAnimation();
	virtual ~CVehicleAnimation() {}

	//IVehicleAnimation
	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table) override;
	virtual void Reset() override;
	virtual void Release() override { delete this; }

	virtual bool StartAnimation() override;
	virtual void StopAnimation() override;

	virtual bool ChangeState(TVehicleAnimStateId stateId) override;
	virtual TVehicleAnimStateId GetState() override;

	virtual string GetStateName(TVehicleAnimStateId stateId) override;
	virtual TVehicleAnimStateId GetStateId(const string& name) override;
	
	virtual void SetSpeed(float speed) override;

	virtual void ToggleManualUpdate(bool isEnabled) override;
	virtual void SetTime(float time, bool force=false) override;
	
	virtual float GetAnimTime(bool raw=false) override;
	virtual bool IsUsingManualUpdates() override;

	virtual bool PlaySound(TVehicleAnimStateId stateId) override; //CryMP: Added
	//IVehicleAnimation
	
protected:

	struct SAnimationStateMaterial
	{
		string material;
		string setting;
		bool invertValue;
	};

	typedef std::vector <SAnimationStateMaterial> TAnimationStateMaterialVector;

	struct SAnimationState
	{
		string name;
		string animation;
		string sound;

		tSoundID soundId = 0;
		IVehicleHelper* pSoundHelper = nullptr;

		float speedDefault = 0.0f;
		float speedMin = 0.0f;
		float speedMax = 0.0f;
		bool isLooped = false;
		bool isLoopedEx = false;

		float timeRestartAnim = 0.0f; //CryMP

		TAnimationStateMaterialVector materials;
	};

	typedef std::vector <SAnimationState> TAnimationStateVector;

protected:

	bool ParseState(const CVehicleParams& table, IVehicle* pVehicle);
	IMaterial* FindMaterial(const SAnimationStateMaterial& animStateMaterial, IMaterial* pMaterial);

	CVehiclePartAnimated* m_pPartAnimated;
	int m_layerId;

	TAnimationStateVector m_animationStates;
	TVehicleAnimStateId m_currentStateId;

	bool m_currentAnimIsWaiting;
};


#endif
