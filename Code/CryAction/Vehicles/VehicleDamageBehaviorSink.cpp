/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior which sink the vehicle

-------------------------------------------------------------------------
History:
- 03:11:2005: Created by Mathieu Pinard

*************************************************************************/

#include "StdAfx.h"
#include "IVehicleSystem.h"
#include "Vehicle.h"
#include "VehicleDamageBehaviorSink.h"

//------------------------------------------------------------------------
bool CVehicleDamageBehaviorSink::Init(IVehicle* pVehicle, const CVehicleParams& table)
{
	m_pVehicle = pVehicle;
	m_isSinking = false;
  m_sinkingTimer = -1;  

	// save the initial buoyancy here (prevents problems with multiple versions of this damage behaviour acting on the same boat)
	if (IPhysicalEntity* pPhysEntity = m_pVehicle->GetEntity()->GetPhysics())
	{
		pe_params_buoyancy buoyancyParams;
		if (pPhysEntity->GetParams(&buoyancyParams))
		{
			m_formerWaterDensity = buoyancyParams.kwaterDensity;			
		}
	}

	return true;
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorSink::Reset()
{
	if (m_isSinking)
		ChangeSinkingBehavior(false);
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorSink::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams)
{
	if (event == eVDBE_Hit || event == eVDBE_VehicleDestroyed || event == eVDBE_ComponentDestroyed)
	{
		if (behaviorParams.componentDamageRatio >= 1.0f)
			ChangeSinkingBehavior(true);
	}
  else if (event == eVDBE_Repair && behaviorParams.componentDamageRatio < 1.f)
  {
    ChangeSinkingBehavior(false);
  }
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorSink::ChangeSinkingBehavior(bool isSinking)
{
	IEntity* pEntity = m_pVehicle->GetEntity();
	CRY_ASSERT(pEntity);

	if (isSinking && !m_isSinking)
	{
		if (IPhysicalEntity* pPhysEntity = pEntity->GetPhysics())
		{
			pe_params_buoyancy buoyancyParams;

			if (pPhysEntity->GetParams(&buoyancyParams))
			{
				//m_formerWaterDensity = buoyancyParams.kwaterDensity;			
				buoyancyParams.kwaterDensity *= 0.5f;

				pPhysEntity->SetParams(&buoyancyParams);
			}
		}
		
    m_sinkingTimer = m_pVehicle->SetTimer(-1, 10000, this); // starts complete sinking
    m_isSinking = true;
	}
	else if (!isSinking && m_isSinking)
	{
		if (IPhysicalEntity* pPhysEntity = pEntity->GetPhysics())
		{
			pe_params_buoyancy buoyancyParams;

			if (pPhysEntity->GetParams(&buoyancyParams))
			{
				buoyancyParams.kwaterDensity = m_formerWaterDensity;			
				pPhysEntity->SetParams(&buoyancyParams);
			}
		}

    m_sinkingTimer = m_pVehicle->KillTimer(m_sinkingTimer);    
		m_isSinking = false;
	}
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorSink::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
  switch (event)
  {
  case eVE_Timer:
    {       
      if (IPhysicalEntity* pPhysEntity = m_pVehicle->GetEntity()->GetPhysics())
      {
        // decrease water density further      
        pe_params_buoyancy buoyancyParams;
        
        if (pPhysEntity->GetParams(&buoyancyParams))
				{
					buoyancyParams.kwaterDensity -= 0.05f * m_formerWaterDensity;

					pPhysEntity->SetParams(&buoyancyParams);

					if (buoyancyParams.kwaterDensity > 0.05f * m_formerWaterDensity) 
						m_sinkingTimer = m_pVehicle->SetTimer(-1, 1000, this);
				}
      }
    }
    break;
  }
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorSink::Serialize(TSerialize serialize, EEntityAspects)
{
  if (serialize.GetSerializationTarget() != eST_Network)
  {
    serialize.Value("sinkTimer", m_sinkingTimer);
    
    bool isNowSinking = m_isSinking;
    serialize.Value("isSinking", isNowSinking);

    if (serialize.IsReading())
    {
      ChangeSinkingBehavior(isNowSinking);
    }
  }  
}

DEFINE_VEHICLEOBJECT(CVehicleDamageBehaviorSink);
