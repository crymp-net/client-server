/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2010.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Vehicle part class that spawns a particle effect and attaches it to the vehicle.

-------------------------------------------------------------------------
History:
- 01:09:2010: Created by Paul Slinger

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

#include "Vehicle.h"
#include "VehiclePartParticleEffect.h"
#include "VehicleUtils.h"

//------------------------------------------------------------------------
CVehiclePartParticleEffect::CVehiclePartParticleEffect() : m_id(0), m_pParticleEffect(NULL), m_pHelper(NULL)
{
}

//------------------------------------------------------------------------
CVehiclePartParticleEffect::~CVehiclePartParticleEffect()
{
	assert(m_pVehicle);

	m_pVehicle->UnregisterVehicleEventListener(this);

	ActivateParticleEffect(false);

	if(m_pParticleEffect)
	{
		m_pParticleEffect->Release();
	}
}

//------------------------------------------------------------------------
bool CVehiclePartParticleEffect::Init(IVehicle *pVehicle, const CVehicleParams &table, IVehiclePart *parent, CVehicle::SPartInitInfo &initInfo, int partType)
{
	if(!CVehiclePartBase::Init(pVehicle, table, parent, initInfo))
	{
		return false;
	}

	if(CVehicleParams params = table.findChild("ParticleEffect"))
	{
		params.getAttr("id", m_id);

		m_particleEffectName	= params.getAttr("particleEffect");
		m_helperName					= params.getAttr("helper");
	}

	m_pVehicle->RegisterVehicleEventListener(this, "VehiclePartParticleEffect");

	return true;
}

//------------------------------------------------------------------------
void CVehiclePartParticleEffect::PostInit()
{
	if(!m_particleEffectName.empty())
	{
		m_pParticleEffect = gEnv->p3DEngine->FindParticleEffect(m_particleEffectName.c_str());

		if(m_pParticleEffect)
		{
			m_pParticleEffect->AddRef();
		}
	}

	if(!m_helperName.empty())
	{
		m_pHelper = m_pVehicle->GetHelper(m_helperName.c_str());
	}
}

//------------------------------------------------------------------------
void CVehiclePartParticleEffect::Reset()
{
	ActivateParticleEffect(false);
}

//------------------------------------------------------------------------
void CVehiclePartParticleEffect::Update(const float frameTime)
{
	CVehiclePartBase::Update(frameTime);
}

//------------------------------------------------------------------------
void CVehiclePartParticleEffect::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams &params)
{
	switch(event)
	{
		case eVE_Destroyed:
		{
			ActivateParticleEffect(false);

			break;
		}
		/* //CryMP: fixme
		case eVE_StartParticleEffect:
		case eVE_StopParticleEffect:
		{
			if((params.iParam == 0) || (params.iParam == m_id))
			{
				ActivateParticleEffect(event == eVE_StartParticleEffect);
			}

			break;
		}*/
	}
}

//------------------------------------------------------------------------
void CVehiclePartParticleEffect::GetMemoryUsage(ICrySizer *pSizer) const
{
	assert(pSizer);

	pSizer->Add(*this);

	//CVehiclePartBase::GetMemoryUsageInternal(pSizer); 
}

//------------------------------------------------------------------------
void CVehiclePartParticleEffect::ActivateParticleEffect(bool activate)
{
	IEntity	*pEntity = m_pVehicle->GetEntity();

	if(activate)
	{
		m_slot = pEntity->LoadParticleEmitter(-1, m_pParticleEffect, NULL, false, true);

		if(m_slot > 0)
		{
			SpawnParams	spawnParams;

			spawnParams.fPulsePeriod = 1.0f;

			pEntity->GetParticleEmitter(m_slot)->SetSpawnParams(spawnParams);

			if(m_pHelper)
			{
				Matrix34 tm = m_pHelper->GetVehicleTM();

				pEntity->SetSlotLocalTM(m_slot, tm);
			}
		}
	}
	else if(m_slot > 0)
	{
		pEntity->FreeSlot(m_slot);

		m_slot = -1;
	}
}

//------------------------------------------------------------------------
DEFINE_VEHICLEOBJECT(CVehiclePartParticleEffect)