/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2007.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior group which gives hit to the 
passenger inside the vehicle

-------------------------------------------------------------------------
History:
- 30:07:2007: Created by Mathieu Pinard

*************************************************************************/
#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CryAction/IActorSystem.h"
#include "CryCommon/CryAction/IVehicleSystem.h"
#include "Vehicle.h"
#include "VehicleDamageBehaviorHitPassenger.h"
#include "CryCommon/CryAction/IGameRulesSystem.h"

//------------------------------------------------------------------------
CVehicleDamageBehaviorHitPassenger::CVehicleDamageBehaviorHitPassenger()
{
}

//------------------------------------------------------------------------
bool CVehicleDamageBehaviorHitPassenger::Init(IVehicle* pVehicle, const CVehicleParams& table)
{
	m_pVehicle = pVehicle;

	CVehicleParams hitPassTable = table.findChild("HitPassenger");
	if (!hitPassTable)
		return false;

	if (!hitPassTable.getAttr("damage", m_damage))
		return false;

	if (!hitPassTable.getAttr("isDamagePercent", m_isDamagePercent))
		m_isDamagePercent = false;

	return true;
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorHitPassenger::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams)
{
	if (event != eVDBE_Hit)
		return;

	IActorSystem* pActorSystem = gEnv->pGame->GetIGameFramework()->GetIActorSystem();
	assert(pActorSystem);

	for (TVehicleSeatId seatId = InvalidVehicleSeatId + 1; 
		seatId != m_pVehicle->GetLastSeatId(); seatId++)
	{
		if (IVehicleSeat* pSeat = m_pVehicle->GetSeatById(seatId))
		{
			EntityId passengerId = pSeat->GetPassenger();

			if (IActor* pActor = pActorSystem->GetActor(passengerId))
			{
				int damage = m_damage;

				if (m_isDamagePercent)
				{
					damage = (int)(pActor->GetMaxHealth() * float(m_damage)/ 100.f);
				}

				if (gEnv->bServer)
				{
					if (IGameRules *pGameRules = gEnv->pGame->GetIGameFramework()->GetIGameRulesSystem()->GetCurrentGameRules())
					{
						HitInfo hit;

						hit.targetId = pActor->GetEntityId();      
						hit.shooterId = behaviorParams.shooterId;
						hit.weaponId = 0;
						hit.damage = (float)damage;
						hit.type = 0;

						pGameRules->ServerHit(hit); 
					}  
				}
			}
		}
	}
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorHitPassenger::GetMemoryStatistics(ICrySizer * s)
{
		s->Add(*this);
}

DEFINE_VEHICLEOBJECT(CVehicleDamageBehaviorHitPassenger);
