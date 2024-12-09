/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a usable action to enter a vehicle seat

-------------------------------------------------------------------------
History:
- 19:01:2006: Created by Mathieu Pinard

*************************************************************************/
#include "CryCommon/CrySystem/ISystem.h"
//#include "CryAction.h"

#include "CryCommon/CryAction/IActorSystem.h"
#include "CryCommon/CryAction/IGameObject.h"
#include "CryCommon/CryAction/IItem.h"
#include "CryCommon/CryAction/IItemSystem.h"
#include "CryCommon/CryAction/IVehicleSystem.h"
#include "Vehicle.h"
#include "VehicleSeat.h"
#include "VehicleSeatGroup.h"
#include "VehicleUsableActionEnter.h"

//------------------------------------------------------------------------
bool CVehicleUsableActionEnter::Init(IVehicle* pVehicle, const CVehicleParams& table)
{
	CVehicleParams enterTable = table.findChild("Enter");
	if (!enterTable)
		return false;

	m_pVehicle = static_cast<CVehicle*>(pVehicle);

	if (CVehicleParams seatsTables = enterTable.findChild("Seats"))
	{
		int c = seatsTables.getChildCount();
		int i = 0;

		for (; i < c; i++)
		{
			CVehicleParams seatRef = seatsTables.getChild(i);
			
			if (const char* pSeatName = seatRef.getAttr("value"))
			{
				if (TVehicleSeatId seatId = m_pVehicle->GetSeatId(pSeatName))
					m_seatIds.push_back(seatId);
			}
		}
	}
	
	return (m_seatIds.size() > 0);
}

//------------------------------------------------------------------------
int CVehicleUsableActionEnter::OnEvent(int eventType, SVehicleEventParams& eventParams)
{
	if (eventType == eVAE_IsUsable)
	{
		EntityId& userId = eventParams.entityId;

		for (TVehicleSeatIdVector::iterator ite = m_seatIds.begin(); ite != m_seatIds.end(); ++ite)
		{
			if (IVehicleSeat* pSeat = m_pVehicle->GetSeatById(*ite))
			{
				if (IsSeatAvailable(pSeat->GetSeatId(), userId))
				{
					eventParams.iParam = pSeat->GetSeatId();
					return 1;
				}
			}
		}

		return 0;
	}
	else if (eventType == eVAE_OnUsed)
	{
		EntityId& userId = eventParams.entityId;

		IVehicleSeat* pSeat = m_pVehicle->GetSeatById(eventParams.iParam);

		if (pSeat && IsSeatAvailable(pSeat->GetSeatId(), userId))
			return pSeat->Enter(userId);

		return -1;
	}

	return 0;
}

//------------------------------------------------------------------------
bool CVehicleUsableActionEnter::IsSeatAvailable(TVehicleSeatId seatId, EntityId userId)
{
	IEntity* pUserEntity = gEnv->pEntitySystem->GetEntity(userId);
	if (!pUserEntity)
		return false;

	CVehicleSeat* pSeat = (CVehicleSeat*)m_pVehicle->GetSeatById(seatId);
	if (!pSeat)
		return false;

	if (!pSeat->IsFree())
		return false;

	return true;
}

void CVehicleUsableActionEnter::GetMemoryStatistics(ICrySizer * s)
{
	s->Add(*this);
	s->AddContainer(m_seatIds);
}

DEFINE_VEHICLEOBJECT(CVehicleUsableActionEnter);
