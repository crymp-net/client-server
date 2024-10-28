/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements the base of the vehicle damages

-------------------------------------------------------------------------
History:
- 23:02:2006: Created by Mathieu Pinard

*************************************************************************/
#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CryAction/IVehicleSystem.h"
#include "VehicleDamages.h"
#include "Vehicle.h"
#include "VehicleComponent.h"
#include "CryCommon/CryAction/IActorSystem.h"

//------------------------------------------------------------------------
void CVehicleDamages::InitDamages(CVehicle* pVehicle, const CVehicleParams& table)
{
	m_pVehicle = pVehicle;
  
	if (CVehicleParams damagesTable = table.findChild("Damages"))
	{
		if (CVehicleParams damagesGroupTable = damagesTable.findChild("DamagesGroups"))
		{
			int c = damagesGroupTable.getChildCount();
			int i = 0;

			m_damagesGroups.reserve(c);

			for (; i < c; i++)
			{
				if (CVehicleParams groupTable = damagesGroupTable.getChild(i))
				{
					CVehicleDamagesGroup* pDamageGroup = new CVehicleDamagesGroup;
					if (pDamageGroup->Init(pVehicle, groupTable))
						m_damagesGroups.push_back(pDamageGroup);
					else
						delete pDamageGroup;
				}
			}
		}

		damagesTable.getAttr("submergedRatioMax", m_damageParams.submergedRatioMax);
		damagesTable.getAttr("submergedDamageMult", m_damageParams.submergedDamageMult);

    damagesTable.getAttr("collDamageThreshold", m_damageParams.collisionDamageThreshold);    
    damagesTable.getAttr("groundCollisionMinMult", m_damageParams.groundCollisionMinMult);    
    damagesTable.getAttr("groundCollisionMaxMult", m_damageParams.groundCollisionMaxMult);    
    damagesTable.getAttr("groundCollisionMinSpeed", m_damageParams.groundCollisionMinSpeed);    
    damagesTable.getAttr("groundCollisionMaxSpeed", m_damageParams.groundCollisionMaxSpeed);  
		damagesTable.getAttr("vehicleCollisionDestructionSpeed", m_damageParams.vehicleCollisionDestructionSpeed);

		ParseDamageMultipliers(m_damageMultipliers, damagesTable);
	}
}

//------------------------------------------------------------------------
void CVehicleDamages::ParseDamageMultipliers(TDamageMultipliers& multipliers, const CVehicleParams& table)
{
	CVehicleParams damageMultipliersTable = table.findChild("DamageMultipliers");
	if (!damageMultipliersTable)
		return;

	int i = 0;
	int c = damageMultipliersTable.getChildCount();

	for (; i < c; i++)
	{
		if (CVehicleParams multiplierTable = damageMultipliersTable.getChild(i))
		{
			string damageType = multiplierTable.getAttr("damageType");
			if (!damageType.empty())
			{
				SDamageMultiplier mult;

				if (multiplierTable.getAttr("multiplier", mult.mult) && mult.mult >= 0.0f)
				{
					multiplierTable.getAttr("splash", mult.splash);
					multipliers.insert(TDamageMultipliers::value_type(damageType, mult));
				}
			}				
		}
	}
}

//------------------------------------------------------------------------
void CVehicleDamages::ReleaseDamages()
{
	for (TVehicleDamagesGroupVector::iterator ite = m_damagesGroups.begin(); ite != m_damagesGroups.end(); ++ite)
	{
		CVehicleDamagesGroup* pDamageGroup = *ite;
		pDamageGroup->Release();
	}
}

//------------------------------------------------------------------------
void CVehicleDamages::ResetDamages()
{
	for (TVehicleDamagesGroupVector::iterator ite = m_damagesGroups.begin(); ite != m_damagesGroups.end(); ++ite)
	{
		CVehicleDamagesGroup* pDamageGroup = *ite;
		pDamageGroup->Reset();
	}
}

//------------------------------------------------------------------------
void CVehicleDamages::UpdateDamages(float frameTime)
{
    FUNCTION_PROFILER( GetISystem(), PROFILE_GAME);

	for (TVehicleDamagesGroupVector::iterator ite = m_damagesGroups.begin(), end = m_damagesGroups.end(); ite != end; ++ite)
	{
		CVehicleDamagesGroup* pDamageGroup = *ite;
		pDamageGroup->Update(frameTime);
	}
}

//------------------------------------------------------------------------
bool CVehicleDamages::ProcessHit(float& damage, const char* hitClass, bool splash)
{
	TDamageMultipliers::const_iterator ite = m_damageMultipliers.find(CONST_TEMP_STRING(hitClass));
	if (ite != m_damageMultipliers.end())
	{
    const SDamageMultiplier& mult = ite->second;
    damage *= mult.mult * (splash ? mult.splash : 1.f);    
    
    if (VehicleCVars().v_debugdraw == eVDB_Damage)
      CryLog("mults for %s: %.2f, splash %.2f", hitClass, mult.mult, mult.splash);
    
    return true;
	}  

  return false;
}

//------------------------------------------------------------------------
CVehicleDamagesGroup* CVehicleDamages::GetDamagesGroup(const char* groupName)
{
	for (TVehicleDamagesGroupVector::iterator ite = m_damagesGroups.begin(); ite != m_damagesGroups.end(); ++ite)
	{
		CVehicleDamagesGroup* pDamageGroup = *ite;
		if (!strcmp(pDamageGroup->GetName().c_str(), groupName))
		{
			return pDamageGroup;
		}
	}

	return NULL;
}

//------------------------------------------------------------------------
void CVehicleDamages::GetDamagesMemoryStatistics(ICrySizer* pSizer)
{
	pSizer->Add(*this);
	pSizer->AddContainer(m_damagesGroups);
	pSizer->Add(m_damageParams);
	pSizer->AddContainer(m_damageMultipliers);
}
