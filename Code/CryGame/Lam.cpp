/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$

-------------------------------------------------------------------------
History:
- 16:10:2006   16:56 : Created by Benito Gangoso Rodriguez

*************************************************************************/
#include "StdAfx.h"
#include "Lam.h"
#include "Actor.h"
#include "Player.h"
#include "CryCommon/Cry3DEngine/ParticleParams.h"
#include "Weapon.h"
#include "GameCVars.h"

#include "CryCommon/CryAction/IMaterialEffects.h"

#define  CAMERA_DISTANCE_SQR	100
#define  LASER_UPDATE_TIME		0.15f
#define  LASER_UPDATE_TIME_FP 0.15f

uint CLam::s_lightCount = 0;		//Init light count


CLam::CLam()
{
	m_laserActivated = false;
	m_lightActivated = false;
	m_laserWasOn = false;
	m_lightID[0] = m_lightID[1] = 0;
	m_lightWasOn = false;
	m_lightWasOn = false;
	m_lightSoundId = INVALID_SOUNDID;
	m_pLaserEntityId = 0;
	m_dotEffectSlot = -1;
	m_laserEffectSlot = -1;
	m_laserHelperFP.clear();
	m_lastLaserHitPt.Set(0, 0, 0);
	m_lastLaserHitSolid = m_lastLaserHitViewPlane = false;
	m_allowUpdate = false;
	m_updateTime = 0.0f;
	m_smoothLaserLength = -1.0f;
	m_lastZPos = 0.0f;
	m_lightActiveSerialize = m_laserActiveSerialize = false;
	m_pParent = 0;
	m_lastUpdate = 0.0f;
	m_FPMode = false;
	m_DotHidden = false;
}

CLam::~CLam()
{
	m_pParent = 0;
	OnReset();
}

//-------------------------------------------------------------------------
bool CLam::Init(IGameObject* pGameObject)
{
	if (!CItem::Init(pGameObject))
		return false;

	m_pParent = static_cast<CItem*>(m_pItemSystem->GetItem(GetParentId()));

	return true;
}

//------------------------------------------------------------------------
bool CLam::ReadItemParams(const IItemParamsNode* root)
{
	if (!CItem::ReadItemParams(root))
		return false;

	const IItemParamsNode* pLamParams = root->GetChild("lam");
	m_lamparams.Reset(pLamParams);

	return true;
}

//-------------------------------------------------------------------------
void CLam::Reset()
{
	CItem::Reset();

	ActivateLight(false);
	ActivateLaser(false);

	m_lightWasOn = false;
	m_laserWasOn = false;
	m_lastLaserHitPt.Set(0, 0, 0);
	m_lastLaserHitSolid = false;
	m_smoothLaserLength = -1.0f;
	m_allowUpdate = false;
	m_updateTime = 0.0f;

	DestroyLaserEntity();
	m_laserHelperFP.clear();
}

//-------------------------------------------------------------------------
void CLam::ActivateLaser(bool activate, bool aiRequest /* = false */)
{
	if (m_laserActivated == activate)
		return;

	CItem* pParent = NULL;
	EntityId ownerId = 0;
	bool ok = false;

	if (m_pParent)
	{
		pParent = m_pParent;
		IWeapon* pWeapon = m_pParent->GetIWeapon();
		if (pWeapon)
			ownerId = m_pParent->GetOwnerId();

		ok = true;
	}
	else
	{
		pParent = this;
		ownerId = GetOwnerId();
	}

	IActor* pOwnerActor = gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(ownerId);
	if (!pOwnerActor)
		return;

	if (activate && !aiRequest && !pOwnerActor->IsPlayer())
		return;

	if (activate)
		RequireUpdate(eIUS_General);

	m_laserActivated = activate;

	if (!m_laserActivated)
	{
		AttachLAMLaser(false, eIGS_FirstPerson);
		AttachLAMLaser(false, eIGS_ThirdPerson);
	}
	else
	{
		bool tp = pOwnerActor->IsThirdPerson();
		if (m_laserHelperFP.empty())
		{
			SAccessoryParams* params = pParent->GetAccessoryParams(GetEntity()->GetClass()->GetName());
			if (!params)
				return;
			m_laserHelperFP.clear();
			m_laserHelperFP = params->attach_helper.c_str();
			m_laserHelperFP.replace("_LAM", "");
		}
		AttachLAMLaser(true, tp ? eIGS_ThirdPerson : eIGS_FirstPerson);
	}
}

//-------------------------------------------------------------------------
void CLam::ActivateLight(bool activate, bool aiRequest /* = false */)
{
	CItem* pParent = NULL;
	EntityId ownerId = 0;

	if (m_pParent)
	{
		//if (!activate && m_pParent->GetStats().dropped)
		//	return;

		pParent = m_pParent;
		IWeapon* pWeapon = m_pParent->GetIWeapon();
		if (pWeapon)
			ownerId = m_pParent->GetOwnerId();
	}
	else
	{
		pParent = this;
		ownerId = GetOwnerId();
	}

	IActor* pOwnerActor = gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(ownerId);
	if (activate && !pOwnerActor)
		return;

	//For AI must be deactivated by default (if they don't request)
	if (activate && !m_lightWasOn && !aiRequest && !pOwnerActor->IsPlayer())
		return;

	if (activate)
		RequireUpdate(eIUS_General);

	m_lightActivated = activate;

	//Activate or deactivate effect
	if (!m_lightActivated)
	{
		AttachLAMLight(false, pParent, eIGS_FirstPerson);
		AttachLAMLight(false, pParent, eIGS_ThirdPerson);	
	}
	else
	{
		uint8 id = pOwnerActor->IsThirdPerson() ? 1 : 0;
		if (m_lightID[id] == 0)
		{
			AttachLAMLight(true, pParent, id ? eIGS_ThirdPerson : eIGS_FirstPerson);
		}
	}
}

//-------------------------------------------------------------------------
void CLam::OnAttach(bool attach)
{
	CItem::OnAttach(attach);

	m_pParent = static_cast<CItem*>(m_pItemSystem->GetItem(GetParentId()));

	if (m_pParent)
	{
		auto* pOwner = m_pParent->GetOwnerActor();
		if (pOwner && pOwner->IsPlayer())
		{
			if (attach)
				m_FPMode = !pOwner->IsThirdPerson();

			if (m_lamparams.isLaser)
				ActivateLaser(attach);

			if (m_lamparams.isFlashLight)
				ActivateLight(attach);
		}
	}
}

//=======================================
void CLam::OnEnterFirstPerson()
{
	const bool lightActive = IsLightActivated();
	const bool laserActive = IsLaserActivated();

	//CryMP: Check 1st/3rd person transition
	if (lightActive)
	{
		ActivateLight(false);
		ActivateLight(true);
	}
	else if (laserActive)
	{
		ActivateTPLaser(false);
		ActivateLaserDot(false, false);
		ActivateLaserDot(true, true);
	}
	m_FPMode = true;
}

//=======================================
void CLam::OnEnterThirdPerson()
{
	const bool lightActive = IsLightActivated();
	const bool laserActive = IsLaserActivated();

	//CryMP: Check 1st/3rd person transition
	if (lightActive)
	{
		ActivateLight(false);
		ActivateLight(true);
	}
	else if (laserActive)
	{
		ActivateTPLaser(true);
		ActivateLaserDot(false, false);
		ActivateLaserDot(true, false);
	}
	m_FPMode = false;
}

//=======================================
void CLam::Update(SEntityUpdateContext& ctx, int slot)
{
	FUNCTION_PROFILER(GetISystem(), PROFILE_GAME);

	if (slot == eIUS_General)
	{
		if (m_pParent && m_pParent->IsSelected())
		{
			RequireUpdate(eIUS_General);

			const bool pFirstPerson = m_FPMode;
			if (!pFirstPerson && m_pParent->IsSelected())
			{
				UpdateTPLaser(ctx.fFrameTime, m_pParent);
			}
			else if (pFirstPerson)
				UpdateFPLaser(ctx.fFrameTime, m_pParent);
		}
	}
}

//-------------------------------------------------------------------------
void CLam::OnParentSelect(bool select)
{
	CItem::OnParentSelect(select);
	
	if (m_lamparams.isLaser)
		ActivateLaser(select);

	if (m_lamparams.isFlashLight)
		ActivateLight(select);

	//Track previous state if deselected
	if (!select)
		SaveLastState();
}

//-------------------------------------------------------------------------
void CLam::OnSelect(bool select)
{
	CItem::OnSelect(select);
}

//-------------------------------------------------------------------------
void CLam::AttachLAMLaser(bool attach, eGeometrySlot slot)
{
	ActivateLaserDot(attach, slot == eIGS_FirstPerson);

	if (slot == eIGS_ThirdPerson)
	{
		ActivateTPLaser(attach);
	}
}

//-------------------------------------------------------------------------
void CLam::AttachLAMLight(bool attach, CItem* pLightAttach, eGeometrySlot slot)
{
	int id = (slot == eIGS_FirstPerson) ? 0 : 1;

	if (attach)
	{
		if (m_lamparams.light_range[id] == 0.f)
			return;

		Vec3 color = m_lamparams.light_color[id] * m_lamparams.light_diffuse_mul[id];
		float specular = 1.0f / m_lamparams.light_diffuse_mul[id];

		string helper;
		Vec3 dir(-1, 0, 0);
		Vec3 localOffset(0.0f, 0.0f, 0.0f);

		if (this != pLightAttach)
		{
			SAccessoryParams* params = pLightAttach->GetAccessoryParams(GetEntity()->GetClass()->GetName());
			if (!params)
				return;

			helper = params->attach_helper.c_str();
			if (slot == eIGS_FirstPerson)
				helper.append("_light");

			//Assets don't have same orientation for pistol/rifle.. 8/
			dir = (m_lamparams.isLamRifle && id == 0) ? Vec3(-0.1f, -1.0f, 0.0f) : Vec3(-1.0f, -0.1f, 0.0f);
			dir.Normalize();
		}

		bool fakeLight = false;
		bool castShadows = false;

		//Some MP/SP restrictions for lights
		IRenderNode* pCasterException = NULL;
		if (CActor* pOwner = pLightAttach->GetOwnerActor())
		{
			if (gEnv->bMultiplayer)
			{
				if (pOwner->IsClient() || pOwner->IsFpSpectatorTarget()) //CryMP Fp spec support
					castShadows = true;
				else
					fakeLight = true;
			}
			else
			{
				if (pOwner->IsPlayer())
					castShadows = true;
				//castShadows = false; //Not for now
			}

			if (castShadows)
			{
				if (IEntityRenderProxy* pRenderProxy = static_cast<IEntityRenderProxy*>(pOwner->GetEntity()->GetProxy(ENTITY_PROXY_RENDER)))
					pCasterException = pRenderProxy->GetRenderNode();
			}
		}

		m_lightID[id] = pLightAttach->AttachLightEx(slot, 0, true, fakeLight, castShadows, pCasterException, m_lamparams.light_range[id], color, specular, m_lamparams.light_texture[id], m_lamparams.light_fov[id], helper.c_str(), localOffset, dir, m_lamparams.light_material[id].c_str(), m_lamparams.light_hdr_dyn[id]);

		if (m_lightID[id])
			++s_lightCount;

		// sounds
		pLightAttach->PlayAction(g_pItemStrings->enable_light);

		if (m_lightSoundId == INVALID_SOUNDID)
			m_lightSoundId = pLightAttach->PlayAction(g_pItemStrings->use_light);

		//Detach the non-needed light 
		uint8 other = id ^ 1;
		if (m_lightID[other])
		{
			pLightAttach->AttachLightEx(other, m_lightID[other], false, true),
				m_lightID[other] = 0;
			--s_lightCount;
		}
	}
	else
	{
		if (m_lightID[id])
		{
			pLightAttach->AttachLightEx(slot, m_lightID[id], false, true);
			m_lightID[id] = 0;
			--s_lightCount;

			PlayAction(g_pItemStrings->disable_light);
			StopSound(m_lightSoundId);
			m_lightSoundId = INVALID_SOUNDID;
		}
	}
}


//--------------------------------------------------------------------------
void CLam::SaveLastState()
{
	if (m_lightActivated)
		m_lightWasOn = true;
	else
		m_lightWasOn = false;

	if (m_laserActivated)
		m_laserWasOn = true;
	else
		m_laserWasOn = false;
}

//-------------------------------------------------------------------------
void CLam::DestroyLaserEntity()
{
	/*if (m_pParent)
	{
		//CryLogAlways("Destroy $8%s | %s", GetEntity()->GetName(), m_pParent->GetStats().dropped ? "DROPPED" : "nope");
		if (m_pParent->GetStats().dropped)
			return;
	}*/
	if (m_laserEffectSlot > 0)
	{
		GetEntity()->FreeSlot(m_laserEffectSlot);
		m_laserEffectSlot = -1;
	}

	if (m_dotEffectSlot > 0)
	{
		GetEntity()->FreeSlot(m_dotEffectSlot);
		m_dotEffectSlot = -1;
	}
}

//------------------------------------------------------------------
void CLam::UpdateAILightAndLaser(const Vec3& pos, const Vec3& dir, float lightRange, float fov, float laserRange)
{
	if (!gEnv->pAISystem)
		return;

	IAIObject* pUserAI = 0;
	if (m_pParent)
	{
		CActor* pActor = m_pParent->GetOwnerActor();
		if (pActor && pActor->GetEntity())
			pUserAI = pActor->GetEntity()->GetAI();
	}

	if (lightRange > 0.0001f)
	{
		gEnv->pAISystem->DynSpotLightEvent(pos, dir, lightRange, DEG2RAD(fov) / 2, AILE_FLASH_LIGHT, pUserAI, 1.0f);
		if (pUserAI)
			gEnv->pAISystem->DynOmniLightEvent(pUserAI->GetPos() + dir * 0.75f, 1.5f, AILE_FLASH_LIGHT, pUserAI, 2.0f);
	}
}

//------------------------------------------------------------------
bool CLam::CanUpdate(CActor* pActor)
{
	if (!pActor)
		return false;

	if (pActor && !pActor->IsClient() && m_pParent)
	{
		if (!pActor->GetGameObject()->IsProbablyVisible() || pActor->GetGameObject()->IsProbablyDistant())
		{
			if (!m_DotHidden)
			{
				if (IParticleEmitter* pEmitter = GetEntity()->GetParticleEmitter(m_dotEffectSlot))
				{
					m_DotHidden = true;
					pEmitter->Activate(false);
				}
			}
			return false;
		}
		else if (m_DotHidden)
		{
			if (IParticleEmitter* pEmitter = GetEntity()->GetParticleEmitter(m_dotEffectSlot))
			{
				m_DotHidden = false;
				pEmitter->Activate(true);
			}
		}
	}
	return true;
}

//=========================================
void CLam::ActivateLaserDot(bool activate, bool fp)
{
	if (activate)
	{
		/*const char* custom = g_pGameCVars->cl_laserDotParticle->GetString();
		const char* name = strlen(custom) > 0 ? custom : m_lamparams.laser_dot[slot];*/
		IParticleEffect* pEffect = gEnv->p3DEngine->FindParticleEffect("muzzleflash.LAM.aimdot_FP");

		if (pEffect)
		{
			if (m_dotEffectSlot < 1)
			{
				m_dotEffectSlot = GetEntity()->LoadParticleEmitter(CItem::eIGS_Aux0, pEffect);
				if (IParticleEmitter* pEmitter = GetEntity()->GetParticleEmitter(m_dotEffectSlot))
				{
					pEmitter->SetRndFlags(pEmitter->GetRndFlags() | ERF_RENDER_ALWAYS);
					pEmitter->SetViewDistUnlimited();
					pEmitter->SetLodRatio(10000);
				}
			}
		}
	}
	else if (m_dotEffectSlot > -1)
	{
		GetEntity()->FreeSlot(m_dotEffectSlot);
		m_dotEffectSlot = -1;
	}
}

//----------------------------------------------------
void CLam::ActivateTPLaser(bool activate)
{
	if (activate)
	{
		//Force first update
		m_lastUpdate = 0.0f;
		m_smoothLaserLength = -1.0f;
		UpdateTPLaser(0.0f, m_pParent);

		//laser geometry
		if (m_laserEffectSlot > 0)
			DrawSlot(m_laserEffectSlot, true);
		else
			m_laserEffectSlot = GetEntity()->LoadGeometry(eIGS_ThirdPersonAux, m_lamparams.laser_geometry_tp);
	}
	else
	{
		//hide laser geometry
		if (m_laserEffectSlot > 0)
		{
			DrawSlot(m_laserEffectSlot, false);
			return;
		}
	}
}

//------------------------------------------------------------------
void CLam::UpdateTPLaser(float frameTime, CItem* parent)
{
	CActor* pActor = parent->GetOwnerActor();
	if (!CanUpdate(pActor))
		return;
	
	//f32 fColor[4] = { 1,1,0,1 };
	//gEnv->pRenderer->Draw2dLabel(1, 130.0f, 1.3f, fColor, false, "UpdateTPLaser %s ", GetEntity()->GetName());

	FUNCTION_PROFILER(GetISystem(), PROFILE_GAME);

	m_lastUpdate -= frameTime;

	bool allowUpdate = true;
	if (m_lastUpdate <= 0.0f)
		m_lastUpdate = pActor->IsClient() ? 0.05f : LASER_UPDATE_TIME;
	else
		allowUpdate = false;

	float  dsg1Scale = 1.0f;

	//If character not visible, laser is not correctly updated
	if (parent)
	{
		ICharacterInstance* pCharacter = pActor ? pActor->GetEntity()->GetCharacter(0) : 0;
		if (pCharacter && !pCharacter->IsCharacterVisible())
			return;

		if (parent->GetEntity()->GetClass() == CItem::sDSG1Class)
			dsg1Scale = 3.0f;
	}

	Vec3   pos = GetEntity()->GetWorldPos();
	Vec3   dir = GetEntity()->GetWorldRotation().GetColumn1();

	Vec3 hitPos(0, 0, 0);
	float laserLength = 0.0f;
	const float range = m_lamparams.laser_range[eIGS_ThirdPerson] * dsg1Scale;

	if (allowUpdate)
	{
		IPhysicalEntity* pSkipEntity = NULL;
		if (parent->GetOwner())
			pSkipEntity = parent->GetOwner()->GetPhysics();

		// Use the same flags as the AI system uses for visibility.
		const int objects = ent_terrain | ent_static | ent_rigid | ent_sleeping_rigid | ent_independent; //ent_living;
		const int flags = (geom_colltype_ray << rwi_colltype_bit) | rwi_colltype_any | (10 & rwi_pierceability_mask) | (geom_colltype14 << rwi_colltype_bit);

		ray_hit hit;
		if (gEnv->pPhysicalWorld->RayWorldIntersection(pos, dir * range, objects, flags,
			&hit, 1, &pSkipEntity, pSkipEntity ? 1 : 0))
		{
			laserLength = hit.dist;
			m_lastLaserHitPt = hit.pt;
			m_lastLaserHitSolid = true;
		}
		else
		{
			m_lastLaserHitSolid = false;
			m_lastLaserHitPt = pos + dir * range;
			laserLength = range + 0.1f;
		}

		// Hit near plane
		const CCamera& camera = gEnv->pRenderer->GetCamera();
		if (dir.Dot(camera.GetViewdir()) < 0.0f)
		{
			Plane nearPlane;
			nearPlane.SetPlane(camera.GetViewdir(), camera.GetPosition());
			nearPlane.d -= camera.GetNearPlane() + 0.15f;
			Ray ray(pos, dir);
			Vec3 out;
			m_lastLaserHitViewPlane = false;
			if (Intersect::Ray_Plane(ray, nearPlane, out))
			{
				float dist = Distance::Point_Point(pos, out);
				if (dist < laserLength)
				{
					laserLength = dist;
					m_lastLaserHitPt = out;
					m_lastLaserHitSolid = true;
					m_lastLaserHitViewPlane = true;
				}
			}
		}

		hitPos = m_lastLaserHitPt;
	}
	else
	{
		laserLength = Distance::Point_Point(m_lastLaserHitPt, pos);
		hitPos = pos + dir * laserLength;
	}

	if (m_smoothLaserLength < 0.0f)
		m_smoothLaserLength = laserLength;
	else
	{
		if (laserLength < m_smoothLaserLength)
			m_smoothLaserLength = laserLength;
		else
			m_smoothLaserLength += (laserLength - m_smoothLaserLength) * min(1.0f, 10.0f * frameTime);
	}

	const float assetLength = 2.0f;
	m_smoothLaserLength = CLAMP(m_smoothLaserLength, 0.01f, range);
	float scale = m_smoothLaserLength / assetLength;

	float lightAIRange = 0.0f;
	if (m_lightActivated)
	{
		float range = clamp(m_smoothLaserLength, 0.5f, m_lamparams.light_range[eIGS_ThirdPerson]);
		lightAIRange = range * 1.5f;

		if (m_lightID[eIGS_ThirdPerson] && m_smoothLaserLength > 0.0f)
		{
			if (m_pParent)
				m_pParent->SetLightRadius(range, m_lightID[eIGS_ThirdPerson]);
		}
	}

	if (m_laserEffectSlot > 0)
	{
		// Scale the laser based on the distance.
		Matrix34 scl;
		scl.SetIdentity();
		scl.SetScale(Vec3(1, scale, 1));

		GetEntity()->SetSlotLocalTM(m_laserEffectSlot, scl);
	}

	if (m_dotEffectSlot >= 0)
	{
		if (m_lastLaserHitSolid)
		{
			Matrix34 dotMatrix = Matrix34::CreateTranslationMat(Vec3(0, m_smoothLaserLength, 0));

			if (m_lastLaserHitViewPlane)
				dotMatrix.Scale(Vec3(0.2f, 0.2f, 0.2f));
			GetEntity()->SetSlotLocalTM(m_dotEffectSlot, dotMatrix);
		}
		else
		{
			Matrix34 scaleMatrix;
			scaleMatrix.SetIdentity();
			scaleMatrix.SetScale(Vec3(0.001f, 0.001f, 0.001f));
			GetEntity()->SetSlotLocalTM(m_dotEffectSlot, scaleMatrix);
		}
	}
}

//------------------------------------------------------------------
void CLam::UpdateFPLaser(float frameTime, CItem* parent)
{
	if (gEnv->bMultiplayer && m_lamparams.isFlashLight)
		return;
	
	//f32 fColor[4] = { 1,1,0,1 };
	//gEnv->pRenderer->Draw2dLabel(60.f, 60.0f, 1.3f, fColor, false, "Update FP Laser  %s ", GetEntity()->GetName());

	Vec3 lamPos, dir;

	AdjustLaserFPDirection(parent, dir, lamPos);

	const float range = m_lamparams.laser_range[eIGS_FirstPerson];

	dir.Normalize();

	const float nearClipPlaneLimit = 10.0f;

	Vec3 hitPos(0, 0, 0);
	float laserLength = 0.0f;
	float dotScale = 1.0f;
	
	IPhysicalEntity* pSkipEntity = NULL;
	if (parent->GetOwner())
		pSkipEntity = parent->GetOwner()->GetPhysics();

	const int objects = ent_all;
	const int flags = (geom_colltype_ray << rwi_colltype_bit) | rwi_colltype_any | (10 & rwi_pierceability_mask) | (geom_colltype14 << rwi_colltype_bit);

	ray_hit hit;
	if (gEnv->pPhysicalWorld->RayWorldIntersection(lamPos, dir * range, objects,
		flags, &hit, 1, &pSkipEntity, pSkipEntity ? 1 : 0))
	{
		//Clamp distance below near clip plane limits, if not dot will be overdrawn during rasterization
		if (hit.dist > nearClipPlaneLimit)
		{
			laserLength = nearClipPlaneLimit;
			hitPos = lamPos + (nearClipPlaneLimit * dir);
		}
		else
		{
			laserLength = hit.dist;
			hitPos = hit.pt;
		}
		if (GetOwnerActor() && GetOwnerActor()->GetActorParams())
			dotScale *= GetOwnerActor()->GetActorParams()->viewFoVScale;
	}
	else
	{
		hitPos = lamPos - (3.0f * dir);
		laserLength = 3.0f;
	}

	if (m_dotEffectSlot >= 0)
	{
		//Force it always to be active
		//if(IParticleEmitter* pEmitter = GetEntity()->GetParticleEmitter(m_dotEffectSlot))
			//pEmitter->Hide(false);

		Matrix34 worldMatrix = GetEntity()->GetWorldTM();
		if (laserLength <= 0.7f)
			hitPos = lamPos + (0.7f * dir);

		CPlayer* pPlayer = static_cast<CPlayer*>(GetOwnerActor());
		SPlayerStats* pStats = pPlayer ? static_cast<SPlayerStats*>(pPlayer->GetActorStats()) : NULL;
		if (pStats && pStats->bLookingAtFriendlyAI)
		{
			hitPos = lamPos + (2.0f * dir);
			laserLength = 2.0f;
		}

		if (laserLength <= 2.0f)
			dotScale *= min(1.0f, (0.35f + ((laserLength - 0.7f) * 0.5f)));

		Matrix34 localMatrix = worldMatrix.GetInverted() * Matrix34::CreateTranslationMat(hitPos - (0.2f * dir));
		localMatrix.Scale(Vec3(dotScale, dotScale, dotScale));
		GetEntity()->SetSlotLocalTM(m_dotEffectSlot, localMatrix);
	}

	if (gEnv->bMultiplayer)
		return;

	if (m_laserActivated || m_lightActivated)
	{
		float laserAIRange = m_laserActivated ? laserLength : 0.0f;
		float lightAIRange = m_lightActivated ? min(laserLength, m_lamparams.light_range[eIGS_FirstPerson] * 1.5f) : 0.0f;
		UpdateAILightAndLaser(lamPos, dir, lightAIRange, m_lamparams.light_fov[eIGS_FirstPerson], laserAIRange);
	}
}

//------------------------------------------------------------------
void CLam::AdjustLaserFPDirection(CItem* parent, Vec3& dir, Vec3& pos)
{
	pos = parent->GetSlotHelperPos(eIGS_FirstPerson, m_laserHelperFP.c_str(), true);
	Quat   lamRot = Quat(parent->GetSlotHelperRotation(eIGS_FirstPerson, m_laserHelperFP.c_str(), true));
	dir = -lamRot.GetColumn0();

	if (!m_lamparams.isLamRifle)
		dir = lamRot.GetColumn1();

	CActor* pActor = parent->GetOwnerActor();
	IMovementController* pMC = pActor ? pActor->GetMovementController() : NULL;
	if (pMC)
	{
		SMovementState info;
		pMC->GetMovementState(info);

		CWeapon* pWep = static_cast<CWeapon*>(parent->GetIWeapon());
		if (pWep && (pWep->IsReloading() || (!pActor->CanFire() && !pWep->IsZoomed())))
			return;

		if (dir.Dot(info.fireDirection) < 0.985f)
			return;

		CCamera& camera = gEnv->pSystem->GetViewCamera();
		pos = camera.GetPosition();
		dir = camera.GetMatrix().GetColumn1();
		dir.Normalize();
	}
}

//------------------------------------------------------------------
void CLam::PickUp(EntityId pickerId, bool sound, bool select, bool keepHistory)
{
	if (gEnv->bServer && m_lamparams.giveExtraAccessory)
	{
		CActor* pActor = GetActor(pickerId);
		if (pActor && pActor->IsPlayer())
		{
			IInventory* pInventory = GetActorInventory(pActor);
			if (pInventory)
			{
				if (!m_lamparams.isLamRifle && !pInventory->GetItemByClass(CItem::sLAMFlashLight) && gEnv->bMultiplayer)
					m_pItemSystem->GiveItem(pActor, m_lamparams.extraAccessoryName.c_str(), false, false, false);
				else if (m_lamparams.isLamRifle && !pInventory->GetItemByClass(CItem::sLAMRifleFlashLight) && gEnv->bMultiplayer)
					m_pItemSystem->GiveItem(pActor, m_lamparams.extraAccessoryName.c_str(), false, false, false);
			}
		}
	}

	CItem::PickUp(pickerId, sound, select, keepHistory);
}

//------------------------------------------------------------------
void CLam::GetMemoryStatistics(ICrySizer* s)
{
	s->Add(*this);
	s->Add(m_laserHelperFP);
	m_lamparams.GetMemoryStatistics(s);
}

//------------------------------------------------------------------
void CLam::FullSerialize(TSerialize ser)
{
	CItem::FullSerialize(ser);

	if (ser.IsReading())
	{
		ActivateLight(false);
		ActivateLaser(false);
		m_lastLaserHitPt.Set(0, 0, 0);
		m_lastLaserHitSolid = false;
		m_smoothLaserLength = -1.0f;
		DestroyLaserEntity();
		m_laserHelperFP.clear();
		m_allowUpdate = false;
		m_updateTime = 0.0f;
	}

	m_laserActiveSerialize = m_laserActivated;
	ser.Value("laserActivated", m_laserActiveSerialize);
	m_lightActiveSerialize = m_lightActivated;
	ser.Value("lightActivated", m_lightActiveSerialize);
}

//------------------------------------------------------------------
void CLam::PostSerialize()
{
}

