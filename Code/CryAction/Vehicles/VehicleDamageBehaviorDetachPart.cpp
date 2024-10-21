/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior which detach a part with its 
children

-------------------------------------------------------------------------
History:
- 26:10:2005: Created by Mathieu Pinard

*************************************************************************/
#include "StdAfx.h"
#include "ICryAnimation.h"
#include "IVehicleSystem.h"
#include "Vehicle.h"
#include "VehiclePartBase.h"
#include "VehiclePartAnimatedJoint.h"
#include "VehicleDamageBehaviorDetachPart.h"

#define DEFAULT_EFFECT "smoke_and_fire.black_smoke.truck_parts"
//------------------------------------------------------------------------
CVehicleDamageBehaviorDetachPart::CVehicleDamageBehaviorDetachPart()
{
  m_pEffect = NULL;
	m_pickableDebris = false;
}

//------------------------------------------------------------------------
CVehicleDamageBehaviorDetachPart::~CVehicleDamageBehaviorDetachPart()
{
	if (m_detachedEntityId != 0)
	{
		gEnv->pEntitySystem->RemoveEntity(m_detachedEntityId);
		m_detachedEntityId = 0;
	}
}

//------------------------------------------------------------------------
bool CVehicleDamageBehaviorDetachPart::Init(IVehicle* pVehicle, const CVehicleParams& table)
{
	m_pVehicle = pVehicle;

	m_detachedEntityId = 0;
	m_pDetachedStatObj = NULL;

	//<DetachPartParams geometry="door2" direction="1.0,0.0,0.0" />

	if (CVehicleParams detachPartParams = table.findChild("DetachPart"))
	{
		m_partName = detachPartParams.getAttr("part");

		// Get optional custom particle effect
		if (detachPartParams.haveAttr("effect"))
			m_pEffect = gEnv->pParticleManager->FindEffect(detachPartParams.getAttr("effect"), "CVehicleDamageBehaviorDetachPart()");

		// Use default effect if necessary
		if (m_pEffect == NULL)
			m_pEffect = gEnv->pParticleManager->FindEffect(DEFAULT_EFFECT, "CVehicleDamageBehaviorDetachPart()");

		detachPartParams.getAttr("pickable", m_pickableDebris);

		return true;
	}
 
	return false;
}

//------------------------------------------------------------------------
bool CVehicleDamageBehaviorDetachPart::Init(IVehicle* pVehicle, const string& partName, const string& effectName)
{
	m_pVehicle = pVehicle;

	m_detachedEntityId = 0;
	m_pDetachedStatObj = NULL;

	m_partName = partName;

	// Get optional custom particle effect
	if (!effectName.empty())
		m_pEffect = gEnv->pParticleManager->FindEffect(effectName.c_str(), "CVehicleDamageBehaviorDetachPart()");

	// Use default effect if necessary
	if (m_pEffect == NULL)
		m_pEffect = gEnv->pParticleManager->FindEffect(DEFAULT_EFFECT, "CVehicleDamageBehaviorDetachPart()");

	return true;
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorDetachPart::Reset()
{
	if (m_detachedEntityId)
	{
		if (m_pDetachedStatObj)
		{
			if (CVehiclePartBase* pPartBase = static_cast<CVehiclePartBase*>(m_pVehicle->GetPart(m_partName.c_str())))
				pPartBase->SetStatObj(m_pDetachedStatObj);

			m_pDetachedStatObj = NULL;
		}

		if(GetISystem()->IsSerializingFile() != 1)
		{		
			IEntitySystem* pEntitySystem = gEnv->pEntitySystem;
			pEntitySystem->RemoveEntity(m_detachedEntityId, true);
		}
		
		m_detachedEntityId = 0;
	}	
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorDetachPart::Serialize(TSerialize ser, EEntityAspects aspects)
{
	if (ser.GetSerializationTarget() != eST_Network)
	{
    EntityId detachedId = m_detachedEntityId;
		ser.Value("m_detachedEntityId", detachedId);
		
		if (ser.IsReading() && m_detachedEntityId != detachedId)
		{
      if (detachedId)
      {
        m_detachedEntityId = detachedId;
        
        if (IEntity* pDetachedEntity = gEnv->pEntitySystem->GetEntity(m_detachedEntityId))
        {
          if (CVehiclePartBase* pPart = (CVehiclePartBase*)m_pVehicle->GetPart(m_partName.c_str()))
            MovePartToTheNewEntity(pDetachedEntity, pPart);
        }        
      }		
      else
      {        
        Reset();                
      }
		}
	}
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorDetachPart::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams)
{
	if (event == eVDBE_Repair)
		return;

	if (!m_detachedEntityId && behaviorParams.componentDamageRatio >= 1.0f)
	{
    CVehiclePartBase* pPart = (CVehiclePartBase*)m_pVehicle->GetPart(m_partName.c_str());
    if (!pPart || !pPart->GetStatObj())
      return;

		if (max(1.f-behaviorParams.randomness, pPart->GetDetachProbability()) < Random()) 
			return;

		IEntity* pDetachedEntity = SpawnDetachedEntity();
		if (!pDetachedEntity)
			return;

		m_detachedEntityId = pDetachedEntity->GetId();

    const Matrix34& partWorldTM = pPart->GetWorldTM();
    pDetachedEntity->SetWorldTM(partWorldTM);
		
    MovePartToTheNewEntity(pDetachedEntity, pPart);
		
		SEntityPhysicalizeParams physicsParams;
		physicsParams.mass = pPart->GetMass();
		physicsParams.type = PE_RIGID;    		    
		physicsParams.nSlot = 0;
		pDetachedEntity->Physicalize(physicsParams);
  
    IPhysicalEntity* pPhysics = pDetachedEntity->GetPhysics();
    if (pPhysics)
    {
      pe_params_part params;      
      params.flagsOR = geom_collides|geom_floats;
      params.flagsColliderAND = ~geom_colltype3;
      params.flagsColliderOR = geom_colltype0;
      pPhysics->SetParams(&params);

      pe_action_add_constraint ac;
      ac.flags = constraint_inactive|constraint_ignore_buddy;
      ac.pBuddy = m_pVehicle->GetEntity()->GetPhysics();
      ac.pt[0].Set(0,0,0);
      pPhysics->Action(&ac);
    
		  // set the impulse
		  const Vec3& velocity = m_pVehicle->GetStatus().vel;  				  		  
		  Vec3 baseForce = m_pVehicle->GetEntity()->GetWorldTM().TransformVector(pPart->GetDetachBaseForce());
		  baseForce *= Random() * (10 - 6) + 6;
  		
      pe_action_impulse actionImpulse;
		  actionImpulse.impulse = physicsParams.mass * (velocity + baseForce);
      actionImpulse.angImpulse = physicsParams.mass * Vec3(Random(-1,1), Random(-1,1), Random(-1,1));
      actionImpulse.iApplyTime = 1;
		  
      pPhysics->Action(&actionImpulse);		
    }

		// copy vehicle's material to new entity (fixes detaching parts from vehicles with different paints),
		//	or specify the destroyed material if it exists
		const char* destroyedMaterial = m_pVehicle->GetDestroyedMaterial();
		IMaterial* pMaterial = NULL;
		if(destroyedMaterial && destroyedMaterial[0])
		{
			pMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial(destroyedMaterial, false);
		}

		if (pMaterial)
			pDetachedEntity->SetMaterial(pMaterial);   
		else
			pDetachedEntity->SetMaterial(m_pVehicle->GetEntity()->GetMaterial());
		
		AttachParticleEffect(pDetachedEntity, m_pEffect);
	}
}

//------------------------------------------------------------------------
IEntity* CVehicleDamageBehaviorDetachPart::SpawnDetachedEntity()
{
	IEntity* pVehicleEntity = m_pVehicle->GetEntity();
	CRY_ASSERT(pVehicleEntity);

	// spawn the detached entity
	char pPartName[128];
	_snprintf(pPartName, 128, "%s_DetachedPart_%s", pVehicleEntity->GetName(), m_partName.c_str());
  pPartName[sizeof(pPartName)-1] = '\0';

	SEntitySpawnParams spawnParams;
	spawnParams.sName = pPartName;
	spawnParams.nFlags = ENTITY_FLAG_CLIENT_ONLY;
	if(!m_pickableDebris)
		spawnParams.nFlags |= ENTITY_FLAG_NO_PROXIMITY;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("VehiclePartDetached");
	if (!spawnParams.pClass)
	{
		CRY_ASSERT(0);
		return NULL;
	}

	return gEnv->pEntitySystem->SpawnEntity(spawnParams, true);
}

//------------------------------------------------------------------------
bool CVehicleDamageBehaviorDetachPart::MovePartToTheNewEntity(IEntity* pTargetEntity, CVehiclePartBase* pPartBase)
{
	if (!pPartBase)
		return false;

	IEntity* pVehicleEntity = m_pVehicle->GetEntity();
	CRY_ASSERT(pVehicleEntity);

	IEntity* pDetachedEntity = gEnv->pEntitySystem->GetEntity(m_detachedEntityId);
	if (!pDetachedEntity)
		return false;

	m_pDetachedStatObj = pPartBase->GetStatObj();
	if (!m_pDetachedStatObj)
		return false;

	// place the geometry on the new entity
	int slot = pDetachedEntity->SetStatObj(m_pDetachedStatObj, -1, true, pPartBase->GetMass());

	const Matrix34& partTM = pPartBase->GetWorldTM();
	Matrix34 localTM = pTargetEntity->GetWorldTM().GetInverted() * partTM;
	pDetachedEntity->SetSlotLocalTM(slot, localTM);

	pPartBase->SetStatObj(NULL);

	CVehicle* pVehicle = (CVehicle*) m_pVehicle;
	TVehiclePartVector& parts = pVehicle->GetParts();

	for (TVehiclePartVector::iterator ite = parts.begin(); ite != parts.end(); ++ite)
	{
		IVehiclePart* pPart = ite->second;
		if (pPart->GetParent() == pPartBase)
			MovePartToTheNewEntity(pTargetEntity, (CVehiclePartBase*)pPart);
	}

	return true;
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorDetachPart::AttachParticleEffect(IEntity* pDetachedEntity, IParticleEffect* pEffect)
{
  if (pEffect)
	{
		int slot = pDetachedEntity->LoadParticleEmitter(-1, pEffect, NULL, false, true);

		if (IParticleEmitter* pParticleEmitter = pDetachedEntity->GetParticleEmitter(slot))
		{
			SpawnParams spawnParams;
			spawnParams.fSizeScale = (Random() * 0.5f) + 0.5f;

			pParticleEmitter->SetSpawnParams(spawnParams);
		}
	}
}

void CVehicleDamageBehaviorDetachPart::GetMemoryStatistics(ICrySizer * s)
{
	s->Add(*this);
	s->Add(m_partName);
}

DEFINE_VEHICLEOBJECT(CVehicleDamageBehaviorDetachPart);
