/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior group which handles delays and
some randomness

-------------------------------------------------------------------------
History:
- 23:02:2006: Created by Mathieu Pinard

*************************************************************************/
#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CryAction/IVehicleSystem.h"
#include "Vehicle.h"
#include "VehicleDamageBehaviorGroup.h"

//------------------------------------------------------------------------
bool CVehicleDamageBehaviorGroup::Init(IVehicle* pVehicle, const CVehicleParams& table)
{
	m_pVehicle = (CVehicle*) pVehicle;

	CVehicleParams groupParams = table.findChild("Group");
	if (!groupParams)
		return false;

	if (!groupParams.haveAttr("name"))
		return false;

	m_damageGroupName = groupParams.getAttr("name");
	return !m_damageGroupName.empty();
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorGroup::Reset()
{
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorGroup::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams)
{
	if (CVehicleDamagesGroup* pDamagesGroup = m_pVehicle->GetDamagesGroup(m_damageGroupName.c_str()))
	{
		pDamagesGroup->OnDamageEvent(event, behaviorParams);
	}
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorGroup::Serialize(TSerialize ser, unsigned aspects)
{
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorGroup::Update(const float deltaTime)
{
}

void CVehicleDamageBehaviorGroup::GetMemoryStatistics(ICrySizer * s)
{
	s->Add(*this);
	s->Add(m_damageGroupName);
}

DEFINE_VEHICLEOBJECT(CVehicleDamageBehaviorGroup);
