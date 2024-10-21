/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior which spawn debris found in the
damaged model of the Animated parts

-------------------------------------------------------------------------
History:
- 30:11:2006: Created by Mathieu Pinard

*************************************************************************/
#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CryAction/IVehicleSystem.h"
#include "Vehicle.h"
#include "VehicleDamageBehaviorSpawnDebris.h"
#include "VehiclePartAnimated.h"

//------------------------------------------------------------------------
CVehicleDamageBehaviorSpawnDebris::CVehicleDamageBehaviorSpawnDebris()
: m_pVehicle(NULL), m_pickableDebris(false)
{
}

//------------------------------------------------------------------------
CVehicleDamageBehaviorSpawnDebris::~CVehicleDamageBehaviorSpawnDebris()
{
	Reset();
}

//------------------------------------------------------------------------
bool CVehicleDamageBehaviorSpawnDebris::Init(IVehicle* pVehicle, const CVehicleParams& table)
{
	m_pVehicle = pVehicle;

	if (CVehicleParams debrisParams = table.findChild("SpawnDebris"))
	{
		debrisParams.getAttr("pickable", m_pickableDebris);
	}

	int partCount = m_pVehicle->GetPartCount();
	for (int k = 0; k < partCount; k++)
	{
		IVehiclePart* pPart = m_pVehicle->GetPart(k);
		assert(pPart);

		if (CVehiclePartAnimated* pAnimPart = CAST_VEHICLEOBJECT(CVehiclePartAnimated, pPart))
		{
			ICharacterInstance* pCharInstance = pAnimPart->GetEntity()->GetCharacter(pAnimPart->GetSlot());
			if (pCharInstance)
      {
        ISkeletonPose* pSkeletonPose = pCharInstance->GetISkeletonPose();
        assert(pSkeletonPose);

        int jointCount = pSkeletonPose->GetJointCount();
        for (int jointId = 0; jointId < jointCount; jointId++)
        {
          int i = 1;
          IStatObj* pStatObj = NULL;
					const char* pJointName = pSkeletonPose->GetJointNameByID(jointId);

          while (i < 25)
          {
            pStatObj = pAnimPart->GetDestroyedGeometry(pJointName, i);
            if (pStatObj)
            {
              m_debris.resize(m_debris.size() + 1);
              SDebrisInfo& debrisInfo = m_debris.back();

              debrisInfo.pAnimatedPart = pAnimPart;
              debrisInfo.jointId = jointId;
              debrisInfo.slot = -1;
              debrisInfo.index = i;
              debrisInfo.entityId = 0;
              debrisInfo.time = 0.0f;

              if (VehicleCVars().v_debugdraw == eVDB_Parts)
              {
                CryLog("VehicleDamageBehaviorSpawnDebris[%s]: adding debris part %s", m_pVehicle->GetEntity()->GetName(), pStatObj->GetGeoName());
              }
            }
            else
              break;

            i++;
          } 
        }
      }			
		}
	}

	return m_debris.size() > 0;
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorSpawnDebris::Reset()
{
	m_pVehicle->SetObjectUpdate(this, IVehicle::eVOU_NoUpdate);

	TDebrisInfoList::iterator debrisIte = m_debris.begin();
	TDebrisInfoList::iterator debrisEnd = m_debris.end();

	for (; debrisIte != debrisEnd; ++debrisIte)
	{
		SDebrisInfo& debrisInfo = *debrisIte;
		debrisInfo.time = 0.0f;

		if (debrisInfo.entityId)
		{
			if(GetISystem()->IsSerializingFile() != 1)
				gEnv->pEntitySystem->RemoveEntity(debrisInfo.entityId);
			debrisInfo.entityId = 0;
		}
	}
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorSpawnDebris::OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& params)
{
	if (event != eVDBE_Hit && params.hitValue < 1.0f)
		return;

	bool isUpdateNeeded = false;

	TDebrisInfoList::iterator debrisIte = m_debris.begin();
	TDebrisInfoList::iterator debrisEnd = m_debris.end();

	for (; debrisIte != debrisEnd; ++debrisIte)
	{
		SDebrisInfo& debrisInfo = *debrisIte;

		if (!debrisInfo.entityId)
		{ 
			debrisInfo.time = (float(cry_rand()) / float(RAND_MAX) * (4.0f - min(3.0f, params.hitValue)));
			debrisInfo.force = params.componentDamageRatio;
			isUpdateNeeded = true;

			IEntity* pEntity = debrisInfo.pAnimatedPart->GetEntity();
			assert(pEntity);

			ICharacterInstance* pCharInstance = debrisInfo.pAnimatedPart->GetEntity()->GetCharacter(
				debrisInfo.pAnimatedPart->GetSlot());

			if (!pCharInstance)
				return;

			ISkeletonPose* pSkeletonPose = pCharInstance->GetISkeletonPose();
			assert(pSkeletonPose);

			const char* pJointName = pSkeletonPose->GetJointNameByID(debrisInfo.jointId);

			IStatObj* pDebrisObj = debrisInfo.pAnimatedPart->GetDestroyedGeometry(pJointName, debrisInfo.index);
			if (pDebrisObj)
			{
				Matrix34 vehicleTM = debrisInfo.pAnimatedPart->GetDestroyedGeometryTM(pJointName, debrisInfo.index);
				Matrix34 intactTM = Matrix34((pSkeletonPose->GetAbsJointByID(debrisInfo.jointId)));

				if (IEntity* pDebrisEntity = SpawnDebris(pDebrisObj, vehicleTM * Matrix33(intactTM), debrisInfo.force))
				{
					debrisInfo.entityId = pDebrisEntity->GetId();
					bool isDetaching = (float(cry_rand()) / float(RAND_MAX)) > 0.5f;

					// that's what we get for now...
					isDetaching = true;

					if (isDetaching)
					{
						float force = (float(cry_rand()) / float(RAND_MAX)) * 2.0f;
						GiveImpulse(pDebrisEntity, force);
						AttachParticleEffect(pEntity, "smoke_and_fire.black_smoke.truck_parts");
					}
					else
					{
						pEntity->AttachChild(pDebrisEntity);
					}
				}
				else
					debrisInfo.entityId = 0;				
			}
		}
	}

	if (isUpdateNeeded)
		m_pVehicle->SetObjectUpdate(this, IVehicle::eVOU_AlwaysUpdate);
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorSpawnDebris::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
	if (event == eVE_Collision)
	{
		TDebrisInfoList::iterator debrisIte = m_debris.begin();
		TDebrisInfoList::iterator debrisEnd = m_debris.end();

		for (; debrisIte != debrisEnd; ++debrisIte)
		{
			SDebrisInfo& debrisInfo = *debrisIte;
			if (debrisInfo.time > 0.0f)
			{
				if (IEntity* pEntity = gEnv->pEntitySystem->GetEntity(debrisInfo.entityId))
				{
					pEntity->DetachThis();
					AttachParticleEffect(pEntity, "smoke_and_fire.black_smoke.truck_parts");
					GiveImpulse(pEntity, 1.0f);
				}
				
				debrisInfo.time = 0.0f;
			}
		}
	}
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorSpawnDebris::Update(const float deltaTime)
{
	const Matrix34& worldTM = m_pVehicle->GetEntity()->GetWorldTM();

	TDebrisInfoList::iterator debrisIte = m_debris.begin();
	TDebrisInfoList::iterator debrisEnd = m_debris.end();

	for (; debrisIte != debrisEnd; ++debrisIte)
	{
		SDebrisInfo& debrisInfo = *debrisIte;

		if (debrisInfo.time > 0.0f)
		{
			debrisInfo.time -= deltaTime;

			if (debrisInfo.time <= 0.0f)
			{
				if (IEntity* pEntity = gEnv->pEntitySystem->GetEntity(debrisInfo.entityId))
				{
					pEntity->DetachThis();
					AttachParticleEffect(pEntity, "smoke_and_fire.black_smoke.truck_parts");
					GiveImpulse(pEntity, 2.0f);
				}
				debrisInfo.time = 0.0f;
			}
		}
	}
}

//------------------------------------------------------------------------
IEntity* CVehicleDamageBehaviorSpawnDebris::SpawnDebris(IStatObj* pStatObj, Matrix34 vehicleTM, float force)
{
	IEntity* pVehicleEntity = m_pVehicle->GetEntity();
	assert(pVehicleEntity);

	// spawn the detached entity

  char buffer[128];
  _snprintf(buffer, sizeof(buffer), "%s_DetachedPart_%s", m_pVehicle->GetEntity()->GetName(), pStatObj->GetGeoName());
  buffer[sizeof(buffer)-1] = 0;
	
	SEntitySpawnParams spawnParams;
	spawnParams.sName = buffer;
	spawnParams.nFlags = ENTITY_FLAG_CLIENT_ONLY;
	if(!m_pickableDebris)
		spawnParams.nFlags |= ENTITY_FLAG_NO_PROXIMITY;
	spawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("VehiclePartDetached");
	if (!spawnParams.pClass)
		return NULL;

	IEntity* pSpawnedDebris = gEnv->pEntitySystem->SpawnEntity(spawnParams, true);

	if (!pSpawnedDebris)
		return NULL;

	// place the geometry on the new entity
	int slot = pSpawnedDebris->SetStatObj(pStatObj, -1, true, 200.0f);
  
	pSpawnedDebris->SetWorldTM(m_pVehicle->GetEntity()->GetWorldTM() * vehicleTM);

  if (VehicleCVars().v_debugdraw == eVDB_Parts)
  {
    CryLog("VehicleDamageBehaviorSpawnDebris[%s]: spawned debris part %s (offfset %i %i %i)", m_pVehicle->GetEntity()->GetName(), pStatObj->GetGeoName(), (int)vehicleTM.GetTranslation().x, (int)vehicleTM.GetTranslation().y, (int)vehicleTM.GetTranslation().z);
  }

	SEntityPhysicalizeParams physicsParams;

	if (!pStatObj->GetPhysicalProperties(physicsParams.mass, physicsParams.density))
		physicsParams.mass = 200.0f;

	physicsParams.type = PE_RIGID;
	physicsParams.nFlagsOR &= pef_log_collisions;
	physicsParams.nSlot = 0;
	pSpawnedDebris->Physicalize(physicsParams);

	if (IPhysicalEntity* pPhysEntity = pSpawnedDebris->GetPhysics())
	{
		pe_params_buoyancy buoyancy;
		buoyancy.waterDensity = 0.15f;
		buoyancy.waterResistance = 30.0f;
		pPhysEntity->SetParams(&buoyancy);

		m_spawnedDebris.push_back(pSpawnedDebris->GetId());
		return pSpawnedDebris;
	}
	else
	{
		return NULL;
	}	
}

//------------------------------------------------------------------------
void CVehicleDamageBehaviorSpawnDebris::AttachParticleEffect(IEntity* pDetachedEntity, const string& effectName)
{
	if (IParticleEffect *pEffect = gEnv->p3DEngine->FindParticleEffect(effectName.c_str(), "VehicleDamageBehaviorEffect"))
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

//------------------------------------------------------------------------
void CVehicleDamageBehaviorSpawnDebris::GiveImpulse(IEntity* pDetachedEntity, float force)
{
	IPhysicalEntity* pDebrisPhysEntity = pDetachedEntity->GetPhysics();
	IPhysicalEntity* pVehiclePhysEntity = m_pVehicle->GetEntity()->GetPhysics();
	if (!pDebrisPhysEntity || !pVehiclePhysEntity)
		return;

	pe_action_impulse imp;
	Vec3 randomVel;

	pe_status_dynamics debrisDyn;
	pe_status_dynamics vehicleDyn;
	pDebrisPhysEntity ->GetStatus(&debrisDyn);
	pVehiclePhysEntity->GetStatus(&vehicleDyn);

	randomVel.x = (float(cry_rand()) / float(RAND_MAX) * 1.0f);
	randomVel.y = (float(cry_rand()) / float(RAND_MAX) * 1.0f);
	randomVel.z = (float(cry_rand()) / float(RAND_MAX) * 1.0f);

	imp.impulse = Vec3(vehicleDyn.v.x * randomVel.x, vehicleDyn.v.y * randomVel.y, vehicleDyn.v.z * randomVel.z);
	imp.impulse *= debrisDyn.mass * force;

	randomVel.x = (float(cry_rand()) / float(RAND_MAX) * 1.0f);
	randomVel.y = (float(cry_rand()) / float(RAND_MAX) * 1.0f);
	randomVel.z = (float(cry_rand()) / float(RAND_MAX) * 1.0f);

	imp.angImpulse = Vec3(vehicleDyn.w.x * randomVel.x, vehicleDyn.w.y * randomVel.y, vehicleDyn.w.z * randomVel.z);
	imp.angImpulse *= debrisDyn.mass * force;

	pDebrisPhysEntity ->Action(&imp);
}

void CVehicleDamageBehaviorSpawnDebris::GetMemoryStatistics(ICrySizer * s)
{
	s->Add(*this);
	s->AddContainer(m_spawnedDebris);
	s->AddContainer(m_debris);
}

DEFINE_VEHICLEOBJECT(CVehicleDamageBehaviorSpawnDebris);
