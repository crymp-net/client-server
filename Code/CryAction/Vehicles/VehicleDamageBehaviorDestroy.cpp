/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior which destroy a vehicle

-------------------------------------------------------------------------
History:
- 13:10:2005: Created by Mathieu Pinard

*************************************************************************/
#include "StdAfx.h"

#include "IVehicleSystem.h"

#include "Vehicle.h"
#include "VehicleDamageBehaviorDestroy.h"
#include "VehicleComponent.h"
#include "VehicleSeat.h"

//------------------------------------------------------------------------
bool CVehicleDamageBehaviorDestroy::Init(IVehicle* pVehicle, const CVehicleParams& table)
{
	m_pVehicle = (CVehicle*) pVehicle;	

	if (table.haveAttr("effect"))
		m_effectName = table.getAttr("effect");
	
	return true;
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorDestroy::Reset()
{

}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorDestroy::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams)
{
  if (event == eVDBE_Repair)
    return;

	if (behaviorParams.componentDamageRatio >= 1.0f)
		SetDestroyed(true, behaviorParams.shooterId);
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorDestroy::SetDestroyed(bool isDestroyed, EntityId shooterId)
{
	// destroy the vehicle
	IEntity* pVehicleEntity = m_pVehicle->GetEntity();

	bool destroyed = m_pVehicle->IsDestroyed();
	m_pVehicle->SetDestroyedStatus(isDestroyed);

	if (!destroyed && isDestroyed)
	{
		// do some obscure things over script side
		HSCRIPTFUNCTION scriptFunction(NULL);
		if (pVehicleEntity->GetScriptTable()->GetValue("OnVehicleDestroyed", scriptFunction) && scriptFunction)
		{
			IScriptSystem*	pIScriptSystem = gEnv->pScriptSystem;
			Script::Call(pIScriptSystem, scriptFunction, pVehicleEntity->GetScriptTable());
			pIScriptSystem->ReleaseFunc(scriptFunction);
		}
		// notify the other components

		TVehicleComponentVector& components = m_pVehicle->GetComponents();

		for (TVehicleComponentVector::iterator ite = components.begin(); ite != components.end(); ++ite)
		{
			CVehicleComponent* pComponent = *ite;
			pComponent->OnVehicleDestroyed();
		}

		// tell the parts to thrash themselves, if possible

		TVehiclePartVector parts = m_pVehicle->GetParts();
		IVehiclePart::SVehiclePartEvent partEvent;
		partEvent.type = IVehiclePart::eVPE_Damaged;
		partEvent.fparam = 1.0f;

		for (TVehiclePartVector::iterator ite = parts.begin(); ite != parts.end(); ++ite)
		{
			IVehiclePart* pPart = ite->second;
			pPart->OnEvent(partEvent);
		}

		if (IVehicleMovement* pVehicleMovement = m_pVehicle->GetMovement())
		{
      SVehicleMovementEventParams movementParams;
      movementParams.fValue = 1.0f;
			movementParams.bValue = true;
			pVehicleMovement->OnEvent(IVehicleMovement::eVME_Damage, movementParams);

			movementParams.fValue = 1.0f;
			movementParams.bValue = false;
			pVehicleMovement->OnEvent(IVehicleMovement::eVME_VehicleDestroyed, movementParams);
		}

		SVehicleEventParams eventParams;
    eventParams.entityId = shooterId;
		m_pVehicle->BroadcastVehicleEvent(eVE_Destroyed, eventParams);

		const char pEventName[] = "Destroy";
		const bool val = true;

		SEntityEvent entityEvent;
		entityEvent.event = ENTITY_EVENT_SCRIPT_EVENT;
		entityEvent.nParam[0] = (INT_PTR) pEventName;
		entityEvent.nParam[1] = IEntityClass::EVT_BOOL;
		entityEvent.nParam[2] = (INT_PTR)&val;
		m_pVehicle->GetEntity()->SendEvent(entityEvent);
    
		if (gEnv->pAISystem)
	    gEnv->pAISystem->GetSmartObjectManager()->SetSmartObjectState(m_pVehicle->GetEntity(), "Dead");

		m_pVehicle->OnDestroyed();
	}
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorDestroy::Serialize(TSerialize ser, EEntityAspects aspects)
{

}

DEFINE_VEHICLEOBJECT(CVehicleDamageBehaviorDestroy);
