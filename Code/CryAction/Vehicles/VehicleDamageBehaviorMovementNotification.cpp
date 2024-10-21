/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior which notify the movement class

-------------------------------------------------------------------------
History:
- 17:10:2005: Created by Mathieu Pinard

*************************************************************************/

#include "StdAfx.h"
#include "IVehicleSystem.h"
#include "Vehicle.h"
#include "VehicleDamageBehaviorMovementNotification.h"

//------------------------------------------------------------------------
CVehicleDamageBehaviorMovementNotification::CVehicleDamageBehaviorMovementNotification()
: m_isDamageAlwaysFull(false),
	m_isFatal(true),
	m_isSteeringInvolved(false),
	m_pVehicle(NULL)
{

}

//------------------------------------------------------------------------
bool CVehicleDamageBehaviorMovementNotification::Init(IVehicle* pVehicle, const CVehicleParams& table)
{
	CRY_ASSERT(pVehicle);
	m_pVehicle = pVehicle;

	m_param = 0;

	if (CVehicleParams notificationParams = table.findChild("MovementNotification"))
	{
		notificationParams.getAttr("isSteering", m_isSteeringInvolved);		
    notificationParams.getAttr("isFatal", m_isFatal);
		notificationParams.getAttr("param", m_param);
		notificationParams.getAttr("isDamageAlwaysFull", m_isDamageAlwaysFull);
	}

	return true;
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorMovementNotification::Reset()
{
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorMovementNotification::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams)
{
	if (event == eVDBE_Hit || event == eVDBE_VehicleDestroyed || event == eVDBE_Repair)
	{
		IVehicleMovement::EVehicleMovementEvent eventType;

    if (event == eVDBE_Repair)
      eventType = IVehicleMovement::eVME_Repair;
		else if (m_isSteeringInvolved)
			eventType = IVehicleMovement::eVME_DamageSteering;
		else
			eventType = IVehicleMovement::eVME_Damage;

    SVehicleMovementEventParams params;
		if (m_isDamageAlwaysFull)
		{
			if(event != eVDBE_Repair)
				params.fValue = 1.0f;
			else
				params.fValue = 0.0f;
		}
		else
		{
			if(event != eVDBE_Repair)
				params.fValue = min(1.0f, behaviorParams.componentDamageRatio);
			else
			{
				// round down to nearest 0.25 (ensures movement object is sent zero damage on fully repairing vehicle).
				params.fValue = ((int)(behaviorParams.componentDamageRatio * 4)) / 4.0f;
			}
		}

    params.vParam = behaviorParams.localPos;
		params.bValue = m_isFatal;
		params.iValue = m_param;
    params.pComponent = behaviorParams.pVehicleComponent;
    
		m_pVehicle->GetMovement()->OnEvent(eventType, params);
	}
}

DEFINE_VEHICLEOBJECT(CVehicleDamageBehaviorMovementNotification);
