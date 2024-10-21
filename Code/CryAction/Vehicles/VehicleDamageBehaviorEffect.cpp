/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements an effect damage behavior

-------------------------------------------------------------------------
History:
- 13:10:2005: Created by Mathieu Pinard

*************************************************************************/
#include "StdAfx.h"

#include "ParticleParams.h"
#include "IVehicleSystem.h"
#include "VehicleDamageBehaviorEffect.h"
#include "VehicleCVars.h"

//------------------------------------------------------------------------
bool CVehicleDamageBehaviorEffect::Init(IVehicle* pVehicle, const CVehicleParams& table)
{
	m_pVehicle = pVehicle;
	m_slot = -1;

  if (!table.getAttr("damageRatioMin", m_damageRatioMin))
    m_damageRatioMin = 1.f;
    
	CVehicleParams effectParams = table.findChild("Effect");
	if (!effectParams)
		return false;

  m_effectName = effectParams.getAttr("effect");

	if (!effectParams.getAttr("disableAfterExplosion", m_disableAfterExplosion))
		m_disableAfterExplosion = false;

	return true;
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorEffect::Reset()
{
	if (m_slot != -1)
	{
    SEntitySlotInfo info;
    if (m_pVehicle->GetEntity()->GetSlotInfo(m_slot, info) && info.pParticleEmitter)
    {
      info.pParticleEmitter->Activate(false);
    }

		m_pVehicle->GetEntity()->FreeSlot(m_slot);
		m_slot = -1;
	}
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorEffect::LoadEffect(IVehicleComponent* pComponent)
{
  if (m_damageEffect.effectName.empty())
  {
    TDamageEffectMap* damageEffects = m_pVehicle->GetParticleParams()->GetDamageEffectMap();

    TDamageEffectMap::iterator ite = damageEffects->find(m_effectName);
    if (ite == damageEffects->end())
		{
			if (VehicleCVars().v_debugdraw == eVDB_Damage)
				CryLog("Failed to find damage effect %s", m_effectName.c_str());
      return;
		}

		if (VehicleCVars().v_debugdraw == eVDB_Damage)
			CryLog("Found effect %s", m_effectName.c_str());
    m_damageEffect = ite->second;
  }

	if (VehicleCVars().v_debugdraw == eVDB_Damage)
		CryLog("Starting vehicle damage effect: %s", m_damageEffect.effectName.c_str());
  
  if (IParticleEffect *pEffect = gEnv->pParticleManager->FindEffect(m_damageEffect.effectName.c_str(), "VehicleDamageBehaviorEffect"))
  {
    IEntity* pEntity = m_pVehicle->GetEntity();
    CRY_ASSERT(pEntity);

    m_slot = pEntity->LoadParticleEmitter(m_slot, pEffect, NULL, false, true);

    if (m_damageEffect.pHelper)
      pEntity->SetSlotLocalTM(m_slot, m_damageEffect.pHelper->GetVehicleTM());
    else if (pComponent)
    { 
      Matrix34 tm(IDENTITY);
      tm.SetTranslation( pComponent->GetBounds().GetCenter() );
      pEntity->SetSlotLocalTM(m_slot, tm);
    }
  }
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorEffect::UpdateEffect(float randomness, float damageRatio)
{
  if (IParticleEmitter* pParticleEmitter = m_pVehicle->GetEntity()->GetParticleEmitter(m_slot))
  {
    SpawnParams spawnParams;

    //spawnParams.fSizeScale = (1.0f - randomness) * Random();
    //spawnParams.fSizeScale = (spawnParams.fSizeScale * 0.35f) + 0.65f;
    //spawnParams.fSizeScale *= min(1.0f, damageRatio);
    //CryLog("- fSizeScale = %f", spawnParams.fSizeScale);

    spawnParams.fPulsePeriod = m_damageEffect.pulsePeriod;
    spawnParams.fPulsePeriod *= (1.0f - randomness) * Random();

    pParticleEmitter->SetSpawnParams(spawnParams);
  }
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorEffect::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams)
{
	if (event == eVDBE_MaxRatioExceeded || (event == eVDBE_VehicleDestroyed && m_disableAfterExplosion) || (event == eVDBE_Repair && behaviorParams.componentDamageRatio < m_damageRatioMin))
	{
		Reset();
		return;
	}

  bool bUpdate = (event == eVDBE_Hit || event == eVDBE_ComponentDestroyed || event == eVDBE_Repair || (event == eVDBE_VehicleDestroyed && !m_disableAfterExplosion));
  	
	if (bUpdate)
  {
	  if (m_slot == -1 && !(m_disableAfterExplosion && m_pVehicle->GetStatus().health <= 0.f) && event != eVDBE_Repair)
	  {
			if (VehicleCVars().v_debugdraw == eVDB_Damage)
				CryLog("Vehicle damage %.2f", behaviorParams.componentDamageRatio);
      LoadEffect(behaviorParams.pVehicleComponent);
	  }

    // update the particle scale
    if (m_slot > -1)
    { 
      UpdateEffect(behaviorParams.randomness, behaviorParams.componentDamageRatio);
    }
  }
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorEffect::Serialize(TSerialize ser, EEntityAspects aspects)
{
	bool isEffectActive = m_slot > -1;
  ser.Value("isActive", isEffectActive);

  int slot = m_slot;
  ser.Value("slot", slot);
	
  if (ser.IsReading())
  {
    if (!isEffectActive && m_slot > -1)
    {
      Reset();
    }    
    m_slot = slot;
  }	
}

void CVehicleDamageBehaviorEffect::GetMemoryStatistics(ICrySizer * s)
{
	s->Add(*this);
	s->Add(m_damageEffect);
	m_damageEffect.GetMemoryStatistics(s);
}

DEFINE_VEHICLEOBJECT(CVehicleDamageBehaviorEffect);
