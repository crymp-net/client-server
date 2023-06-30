/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2007.
-------------------------------------------------------------------------
$Id$
$DateTime$

-------------------------------------------------------------------------
History:
- 18:1:2007   15:17 : Created by Márcio Martins

*************************************************************************/
#include "CryGame/StdAfx.h"
#include "Scan.h"
#include "CryGame/Game.h"
#include "CryGame/Actors/Actor.h"
#include "CryGame/GameRules.h"
#include "CryCommon/CryEntitySystem/IEntityProxy.h"
#include "CryCommon/Cry3DEngine/IMaterial.h"
#include "CryGame/HUD/HUD.h"
#include "CryCommon/CrySystem/IFlashPlayer.h"

#include "CryCommon/CrySoundSystem/ISound.h"


//------------------------------------------------------------------------
CScan::CScan()
:	m_scanning(false),
	m_delayTimer(0.0f),
	m_durationTimer(0.0f),
	m_scanLoopId(INVALID_SOUNDID),
	m_tagEntitiesDelay(0.0f)
{
}

//------------------------------------------------------------------------
CScan::~CScan()
{
}

//------------------------------------------------------------------------
void CScan::Init(IWeapon *pWeapon, const struct IItemParamsNode *params)
{
	m_pWeapon = static_cast<CWeapon *>(pWeapon);

	if (params)
		ResetParams(params);
}

//------------------------------------------------------------------------
void CScan::Release()
{
	delete this;
}

//------------------------------------------------------------------------
void CScan::ResetParams(const struct IItemParamsNode *params)
{
	const IItemParamsNode *scan = params?params->GetChild("scan"):0;
	const IItemParamsNode *actions = params?params->GetChild("actions"):0;
	m_scanparams.Reset(scan);
	m_scanactions.Reset(actions);
}

//------------------------------------------------------------------------
void CScan::PatchParams(const struct IItemParamsNode *patch)
{
	const IItemParamsNode *scan = patch->GetChild("scan");
	const IItemParamsNode *actions = patch->GetChild("actions");
	m_scanparams.Reset(scan, false);
	m_scanactions.Reset(actions, false);
}

//------------------------------------------------------------------------
const char *CScan::GetType() const
{
	return "Scan";
}

//------------------------------------------------------------------------
void CScan::Activate(bool activate)
{
	m_scanning=false;
	m_delayTimer=m_durationTimer=m_tagEntitiesDelay=0.0f;

	if (m_scanLoopId != INVALID_SOUNDID)
	{
		m_pWeapon->StopSound(m_scanLoopId);
		m_scanLoopId = INVALID_SOUNDID;
	}
}

//------------------------------------------------------------------------
void CScan::Update(float frameTime, unsigned int frameId)
{
	auto* pOwner = m_pWeapon->GetOwnerActor();
	if (m_scanning && m_pWeapon->IsClient() && pOwner && (pOwner->IsClient() || pOwner->IsFpSpectatorTarget()))
	{
		if (m_delayTimer > 0.0f)
		{
			m_delayTimer -= frameTime;
			if (m_delayTimer > 0.0f)
				return;

			m_delayTimer = 0.0f;

			int slot = m_pWeapon->GetStats().fp ? CItem::eIGS_FirstPerson : CItem::eIGS_ThirdPerson;
			int id = m_pWeapon->GetStats().fp ? 0 : 1;

			m_scanLoopId = m_pWeapon->PlayAction(m_scanactions.scan, 0, true, CItem::eIPAF_Default | CItem::eIPAF_CleanBlending);

			ISound* pSound = m_pWeapon->GetSoundProxy()->GetSound(m_scanLoopId);
			if (pSound)
				pSound->SetLoopMode(true);
		}

		if (m_delayTimer == 0.0f && pOwner->IsClient())
		{
			if (m_tagEntitiesDelay > 0.0f)
			{
				m_tagEntitiesDelay -= frameTime;
				if (m_tagEntitiesDelay <= 0.0f)
				{
					m_tagEntitiesDelay = 0.0f;

					//Here is when entities are displayed on Radar
					if (gEnv->bServer)
						NetShoot(ZERO, 0);
					else
						m_pWeapon->RequestShoot(0, ZERO, ZERO, ZERO, ZERO, 1.0f, 0, 0, 0, false);
				}
			}

			if (m_durationTimer > 0.0f)
			{
				m_durationTimer -= frameTime;
				if (m_durationTimer <= 0.0f)
				{
					m_durationTimer = 0.0f;
					StopFire();
				}
			}
		}

		m_pWeapon->RequireUpdate(eIUS_FireMode);
	}
}

//------------------------------------------------------------------------
void CScan::ShowFlashAnimation(bool enable)
{
	// add the flash animation part here
	IEntity* pEntity = m_pWeapon->GetEntity();
	if (pEntity)
	{
		IEntityRenderProxy* pRenderProxy((IEntityRenderProxy*)pEntity->GetProxy(ENTITY_PROXY_RENDER));
		if (pRenderProxy)
		{
			IMaterial* pMtl(pRenderProxy->GetRenderMaterial(0));
			if (pMtl)
			{
				pMtl = pMtl->GetSafeSubMtl(2);
				if (pMtl)
				{
					const SShaderItem& shaderItem(pMtl->GetShaderItem());
					if (shaderItem.m_pShaderResources && shaderItem.m_pShaderResources->GetTexture(0))
					{
						SEfResTexture* pTex(shaderItem.m_pShaderResources->GetTexture(0));
						if (pTex->m_Sampler.m_pDynTexSource)
						{
							IFlashPlayer* pFlashPlayer(0);
							IDynTextureSource::EDynTextureSource type(IDynTextureSource::DTS_I_FLASHPLAYER);

							pTex->m_Sampler.m_pDynTexSource->GetDynTextureSource((void*&)pFlashPlayer, type);
							if (pFlashPlayer && type == IDynTextureSource::DTS_I_FLASHPLAYER)
							{
								if (enable)
								{
									pFlashPlayer->Invoke0("startScan");
								}
								else
								{
									pFlashPlayer->Invoke0("cancelScan");
								}
							}
						}
					}
				}
			}
		}
	}
}

//------------------------------------------------------------------------
void CScan::StartFire()
{
	if (!m_pWeapon->IsBusy())
	{
		if(m_pWeapon->GetOwnerActor())
		{
			ShowFlashAnimation(true);

			m_scanning=true;
			m_delayTimer=m_scanparams.delay;
			m_durationTimer=m_scanparams.duration;
			m_tagEntitiesDelay=m_scanparams.tagDelay;
			m_pWeapon->SetBusy(true);
		}

		m_pWeapon->PlayAction(m_scanactions.spin_up, 0, false, CItem::eIPAF_Default|CItem::eIPAF_CleanBlending);
		m_pWeapon->RequestStartFire();
		m_pWeapon->RequireUpdate(eIUS_FireMode);
	}
}

//------------------------------------------------------------------------
void CScan::StopFire()
{
	if (!m_scanning)
		return;

	ShowFlashAnimation(false);

	m_pWeapon->PlayAction(m_scanactions.spin_down, 0, false, CItem::eIPAF_Default|CItem::eIPAF_CleanBlending);

	m_scanning=false;
	m_pWeapon->SetBusy(false);
	m_pWeapon->RequestStopFire();

	if (m_scanLoopId != INVALID_SOUNDID)
	{
		m_pWeapon->StopSound(m_scanLoopId);
		m_scanLoopId = INVALID_SOUNDID;
	}
}

//------------------------------------------------------------------------
void CScan::NetStartFire()
{
	if (!m_pWeapon->IsClient())
		return;

	auto* pOwner = m_pWeapon->GetOwnerActor();
	if (pOwner)
	{
		m_scanning = true;

		if (pOwner->IsFpSpectatorTarget()) //CryMP: Fp spec support
		{
			ShowFlashAnimation(true);

			m_pWeapon->PlayAction(m_scanactions.spin_up, 0, false, CItem::eIPAF_Default | CItem::eIPAF_CleanBlending);

			m_delayTimer = m_scanparams.delay;
			m_pWeapon->RequireUpdate(eIUS_FireMode);
		}
		else
		{
			m_scanLoopId = m_pWeapon->PlayAction(m_scanactions.scan);
		}
		ISound* pSound = m_pWeapon->GetSoundProxy()->GetSound(m_scanLoopId);
		if (pSound)
			pSound->SetLoopMode(true);
	}
}

//------------------------------------------------------------------------
void CScan::NetStopFire() 
{	
	if (!m_pWeapon->IsClient())
		return;

	if (m_scanLoopId != INVALID_SOUNDID)
	{
		m_pWeapon->StopSound(m_scanLoopId);
		m_scanLoopId = INVALID_SOUNDID;
	}

	m_scanning = false;

	auto* pOwner = m_pWeapon->GetOwnerActor();
	if (pOwner)
	{
		if (pOwner && pOwner->IsFpSpectatorTarget()) //CryMP: Fp spec support
		{
			ShowFlashAnimation(false);

			m_pWeapon->PlayAction(m_scanactions.spin_down, 0, false, CItem::eIPAF_Default | CItem::eIPAF_CleanBlending);
		}
	}
}

//------------------------------------------------------------------------
void CScan::NetShoot(const Vec3 &hit, int ph)
{
	if (m_pWeapon->IsServer())
	{
		IEntity *pOwner=m_pWeapon->GetOwner();
		EntityId ownerId=pOwner->GetId();

		SEntityProximityQuery query;
		float radius=m_scanparams.range;
		Vec3 pos=pOwner->GetWorldPos();
		query.box = AABB(Vec3(pos.x-radius,pos.y-radius,pos.z-radius), Vec3(pos.x+radius,pos.y+radius,pos.z+radius));
		query.nEntityFlags = ENTITY_FLAG_ON_RADAR; // Filter by entity flag.
		gEnv->pEntitySystem->QueryProximity( query );

		for(int i=0; i<query.nCount; i++)
		{
			IEntity *pEntity = query.pEntities[i];
			if(pEntity && pEntity != pOwner && !pEntity->IsHidden())
			{
				CActor *pActor=m_pWeapon->GetActor(pEntity->GetId());
				if (pActor && (pActor->GetSpectatorMode()!=0 || pActor->GetHealth()<=0.0f))
					continue;

/*				if (pActor && pActor->GetActorClass()==CPlayer::GetActorClassType())
				{
					CPlayer *pPlayer=static_cast<CPlayer *>(pActor);
					CNanoSuit *pSuit=pPlayer->GetNanoSuit();
					if (pSuit && pSuit->GetMode()==NANOMODE_CLOAK && pSuit->GetCloak()->GetType()==CLOAKMODE_REFRACTION)
						continue;
				}
				else*/ if(IItem *pItem = g_pGame->GetIGameFramework()->GetIItemSystem()->GetItem(pEntity->GetId()))
					continue;

				g_pGame->GetGameRules()->AddTaggedEntity(ownerId, pEntity->GetId(), true);
			}
		}
	}
}

//------------------------------------------------------------------------
bool CScan::IsClientScanning() const
{
	CPlayer* pClientActor = static_cast<CPlayer*>(g_pGame->GetIGameFramework()->GetClientActor());
	if (pClientActor)
	{
		CWeapon* pWeapon = pClientActor->GetCurrentWeapon(false);
		if (pWeapon && pWeapon->GetEntity()->GetClass() == CItem::sRadarKitClass)
		{
			CScan* pScan = static_cast<CScan*>(pWeapon->GetActiveFireMode());
			if (pScan && pScan->IsScanning())
			{
				return true;
			}
		}
	}
	return false;
}

//------------------------------------------------------------------------
void CScan::GetMemoryStatistics(ICrySizer * s)
{
	s->Add(m_name);
	m_scanparams.GetMemoryStatistics(s);
	m_scanactions.GetMemoryStatistics(s);
}
