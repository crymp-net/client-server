/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2007.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior which improves collision damages 

-------------------------------------------------------------------------
History:
- 06:10:2007: Created by Mathieu Pinard

*************************************************************************/
#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CryAction/IVehicleSystem.h"
#include "VehicleDamageBehaviorCollisionEx.h"
#include "CryGame/Game.h"
#include "CryGame/GameRules.h"

//------------------------------------------------------------------------
CVehicleDamageBehaviorCollisionEx::~CVehicleDamageBehaviorCollisionEx()
{
	m_pVehicle->UnregisterVehicleEventListener(this);
}

//------------------------------------------------------------------------
bool CVehicleDamageBehaviorCollisionEx::Init(IVehicle* pVehicle, const CVehicleParams& table)
{
	m_pVehicle = pVehicle;

	//<CollisionEx component="CollisionDamages" damages="500">

	CVehicleParams collisionParams = table.findChild("CollisionEx");
	if (!collisionParams)
		return false;

	m_componentName = collisionParams.getAttr("component");

	if (!collisionParams.getAttr("damages", m_damages))
		return false;

	m_pVehicle->RegisterVehicleEventListener(this, "CollisionEx");
	return true;
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorCollisionEx::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
	// SNH: only apply damage on the server
	if (event == eVE_Collision && gEnv->bServer)
	{
		Vec3 localPos = m_pVehicle->GetEntity()->GetWorldTM().GetInverted() * params.vParam;

		IVehicleComponent* pComponent = m_pVehicle->GetComponent(m_componentName.c_str());

		if (pComponent->GetBounds().IsContainPoint(localPos))
		{
			float damages = max(1.0f, m_damages * params.fParam2);

			if (!params.entityId && damages > 0.0f)
			{
				m_pVehicle->OnHit(m_pVehicle->GetEntityId(), 0, damages, params.vParam, 5.0f, "collision", false);
			}
		}
	}
}

DEFINE_VEHICLEOBJECT(CVehicleDamageBehaviorCollisionEx);
