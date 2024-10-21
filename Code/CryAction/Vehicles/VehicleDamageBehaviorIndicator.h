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

#include "IVehicleSystem.h"

class CVehicle;

class CVehicleDamageBehaviorIndicator
	: public IVehicleDamageBehavior
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehicleDamageBehaviorIndicator();
	virtual ~CVehicleDamageBehaviorIndicator() {}

	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table);
	virtual void Reset();
	virtual void Release() { delete this; }

	virtual void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams);
	
	virtual void Serialize(TSerialize ser, EEntityAspects aspects);
	virtual void Update(const float deltaTime);

  virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params){}
	virtual void GetMemoryStatistics(ICrySizer * s);

protected:
  void GetMaterial();
  void SetActive(bool active);
	void EnableLight(bool enable);

	CVehicle* m_pVehicle;
  
  bool m_isActive;
  float m_ratioMin;  
  float m_ratioMax;
  float m_currentDamageRatio;
	  
  // Light
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
