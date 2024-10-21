/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements an entity class for detached parts

-------------------------------------------------------------------------
History:
- 26:10:2005: Created by Mathieu Pinard

*************************************************************************/
#include "CryCommon/CrySystem/ISystem.h"

#include "CryCommon/CryAction/IViewSystem.h"
#include "CryCommon/CryAction/IItemSystem.h"
#include "CryCommon/CryAction/IVehicleSystem.h"
#include "CryCommon/CryPhysics/IPhysics.h"
#include "CryCommon/CryAnimation/ICryAnimation.h"
#include "CryCommon/CryAction/IActorSystem.h"
#include "CryCommon/CrySoundSystem/ISound.h"
#include "CryCommon/CryNetwork/ISerialize.h"
#include "CryCommon/CryAISystem/IAgent.h"

//#include "CryAction.h"
#include "Vehicle.h"
#include "VehiclePartDetachedEntity.h"
#include "VehiclePartBase.h"

//------------------------------------------------------------------------
CVehiclePartDetachedEntity::~CVehiclePartDetachedEntity()
{
	GetGameObject()->EnablePhysicsEvent(false, eEPE_OnCollisionLogged);
}

//------------------------------------------------------------------------
bool CVehiclePartDetachedEntity::Init(IGameObject * pGameObject)
{
	SetGameObject(pGameObject);

	if (IPhysicalEntity* pPhysics = GetEntity()->GetPhysics())
	{
		pe_params_flags physFlags;

		if (pPhysics->GetParams(&physFlags))
		{
			physFlags.flags |= pef_log_collisions;
			pPhysics->SetParams(&physFlags);

			GetGameObject()->EnablePhysicsEvent(true, eEPE_OnCollisionLogged);
		}
	}

	m_timeUntilStartIsOver = 10.0f;

	return true;
}

//------------------------------------------------------------------------
void CVehiclePartDetachedEntity::Update(SEntityUpdateContext& ctx, int slot)
{
	const float frameTime = ctx.fFrameTime;

	switch (slot)
	{
	case 0:
		{
			if (m_timeUntilStartIsOver > 0.0f)
				m_timeUntilStartIsOver -= frameTime;
		}
		break;
	}
}

//------------------------------------------------------------------------
void CVehiclePartDetachedEntity::ProcessEvent(SEntityEvent& event)
{
	if (event.event == ENTITY_EVENT_RESET)
	{
		gEnv->pEntitySystem->RemoveEntity(GetEntity()->GetId());
	}
}

//------------------------------------------------------------------------
void CVehiclePartDetachedEntity::HandleEvent(const SGameObjectEvent& event)
{
	if (event.event == eGFE_OnCollision)
	{
		if (m_timeUntilStartIsOver <= 0.0f)
		{
			const EventPhysCollision *pCollision = static_cast<const EventPhysCollision *>(event.ptr);
			IEntitySystem * pES = gEnv->pEntitySystem;
			IEntity * pE1 = pES->GetEntityFromPhysics(pCollision->pEntity[0]);
			IEntity * pE2 = pES->GetEntityFromPhysics(pCollision->pEntity[1]);
			IEntity * pCollider = pE1 == GetEntity()? pE2 : pE1;

			if (pCollider)
			{
				//OnCollision( pCollider->GetId(), pCollision->pt, pCollision->n );
				IEntity* pEntity = GetEntity();
				assert(pEntity);

				int slotCount = pEntity->GetSlotCount();
				if (slotCount > 1)
				{
					for (int i = 1; i < slotCount; i++)
					{
						if (pEntity->IsSlotValid(i))
							pEntity->FreeSlot(i);
					}
				}
			}
		}
	}
}

void CVehiclePartDetachedEntity::FullSerialize(TSerialize ser)
{
	bool hasGeom = false;
	if(ser.IsWriting())
	{
		if(GetEntity()->GetStatObj(0))
			hasGeom = true;
	}
	ser.Value("hasGeometry", hasGeom);
	if(hasGeom)
	{
		if(ser.IsWriting())
		{
			gEnv->p3DEngine->SaveStatObj(GetEntity()->GetStatObj(0), ser);
			if(GetEntity()->GetPhysics())
				GetEntity()->GetPhysics()->GetStateSnapshot(ser);
		}
		else if(ser.IsReading())
		{
			IStatObj *pStatObj = gEnv->p3DEngine->LoadStatObj(ser);
			if(pStatObj)
			{
				GetEntity()->SetStatObj(pStatObj, 0, true, 200.0f);
				SEntityPhysicalizeParams physicsParams;
				if(!pStatObj->GetPhysicalProperties(physicsParams.mass, physicsParams.density))
					physicsParams.mass = 200.0f;
				physicsParams.type = PE_RIGID;
				physicsParams.nFlagsOR &= pef_log_collisions;
				physicsParams.nSlot = 0;
				GetEntity()->Physicalize(physicsParams);
				if(GetEntity()->GetPhysics())
					GetEntity()->GetPhysics()->SetStateFromSnapshot(ser);
			}
		}
	}
}