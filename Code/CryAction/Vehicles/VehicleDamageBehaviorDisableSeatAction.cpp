/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2010.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior which disables
seat actions for a specified seat

-------------------------------------------------------------------------
History:
- 23:07:2010: Created by SNH

*************************************************************************/

#include "StdAfx.h"

#include "VehicleDamageBehaviorDisableSeatAction.h"

#include "IVehicleSystem.h"

#include "Vehicle.h"
#include "VehicleSeat.h"

CVehicleDamageBehaviorDisableSeatAction::CVehicleDamageBehaviorDisableSeatAction()
: m_pVehicle(NULL)
{
}

//------------------------------------------------------------------------
bool CVehicleDamageBehaviorDisableSeatAction::Init(IVehicle* pVehicle, const CVehicleParams& table)
{
	m_pVehicle = (CVehicle*) pVehicle;	

	CVehicleParams actionTable = table.findChild("DisableSeatAction");
 	m_seatName = actionTable.getAttr("seat");
	
	m_seatActionName = actionTable.getAttr("actionName");

	return true;
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorDisableSeatAction::Reset()
{

}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorDisableSeatAction::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams)
{
	if(event != eVDBE_Hit)
		return;

	TVehicleSeatId seatId = m_pVehicle->GetSeatId(m_seatName.c_str());

	if(seatId == InvalidVehicleSeatId)
	{
		CryLog("DisableSeatAction damage behavior referencing invalid vehicle seat (%s)", m_seatName.c_str());
		return;
	}

 	if(CVehicleSeat* pSeat = static_cast<CVehicleSeat*>(m_pVehicle->GetSeatById(seatId)))
 	{
		bool all = (m_seatActionName == "all");
 		TVehicleSeatActionVector& actions = pSeat->GetSeatActions();
 		for(TVehicleSeatActionVector::iterator ite = actions.begin(), end = actions.end(); ite != end; ++ite)
 		{
 			if(all || (m_seatActionName == ite->pSeatAction->GetName()))
			{
				ite->pSeatAction->StopUsing();

				ite->isEnabled = false;
			}
 		}
 	}
}


DEFINE_VEHICLEOBJECT(CVehicleDamageBehaviorDisableSeatAction);
