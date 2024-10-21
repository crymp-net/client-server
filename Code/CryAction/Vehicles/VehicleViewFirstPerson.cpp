/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements the first person pit view for vehicles

-------------------------------------------------------------------------
History:
- 29:01:2006: Created by Mathieu Pinard

*************************************************************************/
#include "StdAfx.h"
#include "CryAction.h"
#include "IActorSystem.h"
#include "ICryAnimation.h"
#include "IViewSystem.h"
#include "IVehicleSystem.h"
#include "VehicleViewFirstPerson.h"
#include "VehicleSeat.h"


const char* CVehicleViewFirstPerson::m_name = "FirstPerson";

//------------------------------------------------------------------------
CVehicleViewFirstPerson::CVehicleViewFirstPerson()
	: m_passengerId(0)
{
	m_hideVehicle = false;
	m_pHelper = NULL;
	m_offset.zero();
	m_relToHorizon = 0.f;
	m_frameSlot = -1;
	m_passengerId = 0;
}


//------------------------------------------------------------------------
bool CVehicleViewFirstPerson::Init(CVehicleSeat* pSeat, const CVehicleParams& table)
{
	if (!CVehicleViewBase::Init(pSeat, table))
		return false;

	if (CVehicleParams paramsTable = table.findChild(m_name))
	{
		paramsTable.getAttr("offset", m_offset);
		paramsTable.getAttr("hidePlayer", m_hidePlayer);
		paramsTable.getAttr("hideVehicle", m_hideVehicle);
		paramsTable.getAttr("relativeToHorizon", m_relToHorizon);

		string helperName = paramsTable.getAttr("helper");
		if (!helperName.empty())
		{
			if (helperName != "auto")
			{
				m_pHelper = m_pVehicle->GetHelper(helperName);
			}
			else
			{
				// create a helper with default viewpos above sithelper    
				const string& seatName = pSeat->GetName();
				helperName = seatName + string("_ghostview_pos");

				if (IVehicleHelper* pSitHelper = pSeat->GetSitHelper())
				{
					const Matrix34& tm = pSitHelper->GetVehicleTM();
					Vec3 pos = tm.GetTranslation() + Vec3(0, 0, 0.625); // player eye height

					m_pVehicle->AddHelper(helperName.c_str(), pos, tm.GetColumn1(), pSitHelper->GetParentPart());
					m_pHelper = m_pVehicle->GetHelper(helperName.c_str());
				}
			}

			if (!m_pHelper)
				GameWarning("[%s, seat %s]: view helper %s not found, using character head", m_pVehicle->GetEntity()->GetName(), m_pSeat->GetName().c_str(), helperName.c_str());
		}

		string frame = paramsTable.getAttr("frameObject");
		if (!frame.empty())
		{
			// todo: aspect ratio?
			if (strstr(frame, ".cgf"))
				m_frameSlot = m_pVehicle->GetEntity()->LoadGeometry(-1, frame);
			else
				m_frameSlot = m_pVehicle->GetEntity()->LoadCharacter(-1, frame);

			if (m_frameSlot != -1)
			{
				m_pVehicle->GetEntity()->SetSlotFlags(m_frameSlot, m_pVehicle->GetEntity()->GetSlotFlags(m_frameSlot) & ~(ENTITY_SLOT_RENDER | ENTITY_SLOT_RENDER_NEAREST));

				if (m_pHelper)
					m_pVehicle->GetEntity()->SetSlotLocalTM(m_frameSlot, m_pHelper->GetVehicleTM());
			}
		}
	}

	if (m_hideVehicle)
		m_hidePlayer = true;

	m_speedRot = 4.0f;
	m_speedPos = 60.0f;

	if (pSeat->GetAimPart())
		m_speedPos *= 2.0f;

	if (IVehicleMovement* pMovement = m_pVehicle->GetMovement())
	{
		if (pMovement->GetMovementType() == IVehicleMovement::eVMT_Air)
		{
			m_speedRot *= 2.0f;
			m_speedPos *= 2.0f;
		}
	}

	Reset();
	return true;
}


//------------------------------------------------------------------------
void CVehicleViewFirstPerson::Reset()
{
	CVehicleViewBase::Reset();

	if (m_hideVehicle)
		HideEntitySlots(m_pVehicle->GetEntity(), false);
}

//------------------------------------------------------------------------
void CVehicleViewFirstPerson::OnStartUsing(EntityId passengerId)
{
	CVehicleViewBase::OnStartUsing(passengerId);

	if (m_hideVehicle)
	{
		m_slotFlags.clear();
		HideEntitySlots(m_pVehicle->GetEntity(), true);
	}

	if (m_frameSlot != -1)
	{
		m_pVehicle->GetEntity()->SetSlotFlags(m_frameSlot, m_pVehicle->GetEntity()->GetSlotFlags(m_frameSlot) | ENTITY_SLOT_RENDER | ENTITY_SLOT_RENDER_NEAREST);
	}

	m_passengerId = passengerId;

	m_viewPosition = GetWorldPosGoal();
	m_viewRotation = m_pVehicle->GetEntity()->GetRotation();

	if (m_passengerId && m_pSeat && !m_pSeat->IsDriver() && m_pSeat->IsPassengerShielded())
	{
		// disable rendering of ocean for passenger of amphibious vehicles (we see water inside the cabin)
		if (IVehicleMovement* pMovement = m_pVehicle->GetMovement())
		{
			if (pMovement->GetMovementType() == IVehicleMovement::eVMT_Amphibious)
			{
				I3DEngine* p3DEngine = gEnv->p3DEngine;
				p3DEngine->SetOceanRenderFlags(OCR_NO_DRAW);
			}
		}
	}

	if (m_hidePlayer)
	{
		if (IEntity* pEntity = gEnv->pEntitySystem->GetEntity(m_passengerId))
			HideEntitySlots(pEntity, true);
	}

	m_pVehicle->RegisterVehicleEventListener(this, "1stPersonView");
}

//------------------------------------------------------------------------
void CVehicleViewFirstPerson::OnStopUsing()
{
	CVehicleViewBase::OnStopUsing();

	m_pVehicle->UnregisterVehicleEventListener(this);

	if (m_hideVehicle)
		HideEntitySlots(m_pVehicle->GetEntity(), false);

	if (m_hidePlayer)
	{
		if (IEntity* pEntity = gEnv->pEntitySystem->GetEntity(m_passengerId))
			HideEntitySlots(pEntity, false);
	}

	if (m_frameSlot != -1)
	{
		m_pVehicle->GetEntity()->SetSlotFlags(m_frameSlot, m_pVehicle->GetEntity()->GetSlotFlags(m_frameSlot) & ~(ENTITY_SLOT_RENDER | ENTITY_SLOT_RENDER_NEAREST));
	}

	// enable rendering of ocean again, if it was a passenger in amphibious mode
	I3DEngine* p3DEngine = gEnv->p3DEngine;
	p3DEngine->SetOceanRenderFlags(OCR_OCEANVOLUME_VISIBLE);

	if (EntityId weaponId = m_pVehicle->GetCurrentWeaponId(m_passengerId))
	{
		if (IItem* pItem = CCryAction::GetCryAction()->GetIItemSystem()->GetItem(weaponId))
		{
			if (IWeapon* pWeapon = pItem->GetIWeapon())
			{
				if (pWeapon->IsZoomed() || pWeapon->IsZoomingInOrOut())
					pWeapon->StopZoom(m_passengerId);
			}

		}
	}

	m_passengerId = 0;
}

//------------------------------------------------------------------------
void CVehicleViewFirstPerson::Update(const float frameTime)
{
	CVehicleViewBase::Update(frameTime);

	if (m_frameSlot != -1 && m_pHelper)
	{
		Matrix34 tm = m_pHelper->GetVehicleTM();
		tm.SetTranslation(tm.GetTranslation() + tm.TransformVector(Vec3(0, 0.18f, 0.01f)));
		m_pVehicle->GetEntity()->SetSlotLocalTM(m_frameSlot, tm);
	}

	m_viewPosition = GetWorldPosGoal();

	Quat vehicleWorldRot = GetWorldRotGoal();
	m_viewRotation.SetSlerp(m_viewRotation, vehicleWorldRot, min(1.0f, frameTime * m_speedRot));
}

//------------------------------------------------------------------------
void CVehicleViewFirstPerson::UpdateView(SViewParams& viewParams, EntityId playerId)
{
	if (!m_passengerId)
		return;

	viewParams.nearplane = 0.1f;

	if (EntityId weaponId = m_pVehicle->GetCurrentWeaponId(m_passengerId))
	{
		if (IItem* pItem = CCryAction::GetCryAction()->GetIItemSystem()->GetItem(weaponId))
		{
			if (pItem->FilterView(viewParams))
				return;
		}
	}

	viewParams.position = m_viewPosition;
	viewParams.rotation = m_viewRotation * GetVehicleRotGoal() * Quat::CreateRotationXYZ(m_rotation);

	// set view direction on actor
	IActor* pActor = CCryAction::GetCryAction()->GetIActorSystem()->GetActor(playerId);
	if (pActor && pActor->IsClient())
	{
		pActor->SetViewInVehicle(viewParams.rotation);
	}

	// recoil
	viewParams.rotation *= Quat::CreateRotationXYZ(m_viewAngleOffset);
}

//------------------------------------------------------------------------
Vec3 CVehicleViewFirstPerson::GetWorldPosGoal()
{
	Vec3 vehiclePos;
	Quat vehicleRot;

	if (m_pHelper)
	{
		const Matrix34& helperTM = m_pHelper->GetVehicleTM();
		vehiclePos = helperTM.GetTranslation();
	}
	else
	{
		IActor* pActor = CCryAction::GetCryAction()->GetIActorSystem()->GetActor(m_passengerId);
		CRY_ASSERT(pActor);

		IEntity* pEntity = pActor->GetEntity();

		if (pActor != NULL)
			vehiclePos = pActor->GetLocalEyePos() + m_offset;
		else
			vehiclePos = m_offset;

		const Matrix34& slotTM = pEntity->GetSlotLocalTM(0, false);
		vehiclePos = pEntity->GetLocalTM() * slotTM * vehiclePos;
	}

	return m_pVehicle->GetEntity()->GetWorldTM() * vehiclePos;
}

//------------------------------------------------------------------------
Quat CVehicleViewFirstPerson::GetWorldRotGoal()
{
	// now get fitting vehicle world pos/rot
	Quat vehicleWorldRot = m_pVehicle->GetEntity()->GetWorldRotation();

	if (m_relToHorizon > 0.f)
	{
		Quat vehicleRot = m_pVehicle->GetEntity()->GetRotation();
		Vec3 vx = vehicleRot * Vec3(1, 0, 0);
		Vec3 vy = vehicleRot * Vec3(0, 1, 0);
		Vec3 vz = vehicleRot * Vec3(0, 0, 1);

		// vx is "correct"
		vy = (1.0f - m_relToHorizon) * Vec3(0, 0, 1).Cross(vx) + m_relToHorizon * vy;
		vy.NormalizeSafe(Vec3Constants<float>::fVec3_OneY);
		vz = vx.Cross(vy);
		vz.NormalizeSafe(Vec3Constants<float>::fVec3_OneZ);

		vehicleWorldRot = Quat(Matrix33::CreateFromVectors(vx, vy, vz));
	}

	return vehicleWorldRot;
}

//------------------------------------------------------------------------
Quat CVehicleViewFirstPerson::GetVehicleRotGoal()
{
	Quat vehicleRot;

	if (m_pHelper)
	{
		const Matrix34& helperTM = m_pHelper->GetVehicleTM();
		vehicleRot = GetQuatFromMat33(Matrix33(helperTM));
	}
	else
	{
		IActor* pActor = CCryAction::GetCryAction()->GetIActorSystem()->GetActor(m_passengerId);
		CRY_ASSERT(pActor);

		if (pActor)
		{
			IEntity* pEntity = pActor->GetEntity();
			vehicleRot = pEntity->GetRotation();
		}
	}

	return vehicleRot;
}

//------------------------------------------------------------------------
void CVehicleViewFirstPerson::HideEntitySlots(IEntity* pEnt, bool hide)
{
	IActorSystem* pActorSystem = CCryAction::GetCryAction()->GetIActorSystem();
	CRY_ASSERT(pActorSystem);

	if (hide)
	{
		for (int i = 0; i < pEnt->GetSlotCount(); ++i)
		{
			if (pEnt->IsSlotValid(i) && pEnt->GetSlotFlags(i) & ENTITY_SLOT_RENDER)
			{
				if (pEnt->GetId() == m_pVehicle->GetEntity()->GetId())
				{
					// set character to always update
					if (ICharacterInstance* pCharInstance = pEnt->GetCharacter(i))
					{
						pCharInstance->SetFlags(pCharInstance->GetFlags() | CS_FLAG_UPDATE_ALWAYS);

						if (ISkeletonPose* pSkeletonPose = pCharInstance->GetISkeletonPose())
							pSkeletonPose->SetForceSkeletonUpdate(10);
					}
				}

				pEnt->SetSlotFlags(i, pEnt->GetSlotFlags(i) & ~ENTITY_SLOT_RENDER);

				if (IActor* pActor = pActorSystem->GetActor(pEnt->GetId()))
				{
					pActor->HideAllAttachments(true);
				}

				// store slot; we must not reveal previously hidden slots later      
				m_slotFlags.insert(std::pair<EntityId, int>(pEnt->GetId(), i));
			}
		}

		// hide all children
		for (int i = 0; i < pEnt->GetChildCount(); ++i)
			HideEntitySlots(pEnt->GetChild(i), hide);
	}
	else
	{
		// unhide all stored slots
		for (TSlots::iterator it = m_slotFlags.begin(); it != m_slotFlags.end(); ++it)
		{
			IEntity* pEntity = gEnv->pEntitySystem->GetEntity(it->first);

			if (pEntity && pEntity->IsSlotValid(it->second))
			{
				pEntity->SetSlotFlags(it->second, pEntity->GetSlotFlags(it->second) | (ENTITY_SLOT_RENDER));

				if (IActor* pActor = pActorSystem->GetActor(pEnt->GetId()))
				{
					pActor->HideAllAttachments(false);
				}

				if (pEntity->GetId() == m_pVehicle->GetEntity()->GetId())
				{
					// reset character flags
					if (ICharacterInstance* pCharInstance = pEntity->GetCharacter(it->second))
					{
						pCharInstance->SetFlags(pCharInstance->GetFlags() & ~CS_FLAG_UPDATE_ALWAYS);

						if (ISkeletonPose* pSkeletonPose = pCharInstance->GetISkeletonPose())
							pSkeletonPose->SetForceSkeletonUpdate(0);
					}
				}
			}
		}

		m_slotFlags.clear();
	}
}

//------------------------------------------------------------------------
void CVehicleViewFirstPerson::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
	CVehicleViewBase::OnVehicleEvent(event, params);

	if (event == eVE_PassengerEnter && m_hideVehicle)
	{
		if (IEntity* pPassengerEntity = gEnv->pEntitySystem->GetEntity(params.entityId))
		{
			HideEntitySlots(pPassengerEntity, true);
		}
	}
}

//------------------------------------------------------------------------
void CVehicleViewFirstPerson::GetMemoryStatistics(ICrySizer* s)
{
	s->Add(*this);
	s->AddContainer(m_slotFlags);
}


DEFINE_VEHICLEOBJECT(CVehicleViewFirstPerson);
