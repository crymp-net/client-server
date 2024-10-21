/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: implements a DamageIndicator behavior (lights, sounds,..)

-------------------------------------------------------------------------
History:
- 29:04:2006: Created by michaelr

*************************************************************************/

#ifndef __VEHICLEDAMAGEBEHAVIORINDICATOR_H__
#define __VEHICLEDAMAGEBEHAVIORINDICATOR_H__

#include "CryCommon/CryAction/IVehicleSystem.h"

class CVehicle;

class CVehicleDamageBehaviorIndicator
	: public IVehicleDamageBehavior
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehicleDamageBehaviorIndicator();
	virtual ~CVehicleDamageBehaviorIndicator() {}

	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table) override;
	virtual void Reset() override;
	virtual void Release() override { delete this; }
	virtual void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams) override;
	virtual void Serialize(TSerialize ser, unsigned aspects) override;
	virtual void Update(const float deltaTime) override;
	virtual void GetMemoryStatistics(ICrySizer* s) override;
	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override {};

protected:
	void GetMaterial();
	void SetActive(bool active);
	void EnableLight(bool enable);

	CVehicle* m_pVehicle;

	bool m_isActive;
	float m_ratioMin;
	float m_ratioMax;
	float m_currentDamageRatio;

	string m_material;
	IMaterial* m_pSubMtl;

	string m_sound;
	float m_soundRatioMin;
	IVehicleHelper* m_pHelper;

	tSoundID m_soundId;
	int m_soundsPlayed;
	float m_lastDamageRatio;

	float m_lightUpdate;
	float m_lightTimer;
	bool m_lightOn;

	static float m_frequencyMin;
	static float m_frequencyMax;
};


#endif
