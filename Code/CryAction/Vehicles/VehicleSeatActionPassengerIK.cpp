/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a seat action to handle IK on the passenger body

-------------------------------------------------------------------------
History:
- 10:01:2006: Created by Mathieu Pinard

*************************************************************************/
#include "CryCommon/CrySystem/ISystem.h"
//#include "CryAction.h"
#include "CryCommon/CryAction/IActorSystem.h"
#include "CryCommon/CryAction/IVehicleSystem.h"
#include "Vehicle.h"
#include "VehicleSeatActionPassengerIK.h"

#include "CryCommon/CryRenderer/IRenderAuxGeom.h"

//------------------------------------------------------------------------
bool CVehicleSeatActionPassengerIK::Init(IVehicle* pVehicle, TVehicleSeatId seatId, const CVehicleParams& table)
{
	m_pVehicle = pVehicle;
	m_passengerId = 0;

	CVehicleParams ikTable = table.findChild("PassengerIK");
	if (!ikTable)
		return false;

	CVehicleParams limbsTable = ikTable.findChild("Limbs");
	if (!limbsTable)
		return false;

	if (!ikTable.getAttr("waitShortlyBeforeStarting", m_waitShortlyBeforeStarting))
		m_waitShortlyBeforeStarting = false;

	int i = 0;
	int c = limbsTable.getChildCount();
	m_ikLimbs.reserve(c);

	for (; i < c; i++)
	{
		if (CVehicleParams limbTable = limbsTable.getChild(i))
		{
			SIKLimb limb;

			if (limbTable.haveAttr("limb"))
			{
				limb.limbName = limbTable.getAttr("limb");
				if (limbTable.haveAttr("helper"))
				{
					if (limb.pHelper = m_pVehicle->GetHelper(limbTable.getAttr("helper")))
						m_ikLimbs.push_back(limb);
				}
			}			
		}
	}

	if (m_ikLimbs.size() == 0)
		return false;

	return true;
}

//------------------------------------------------------------------------
void CVehicleSeatActionPassengerIK::Reset()
{

}

//------------------------------------------------------------------------
void CVehicleSeatActionPassengerIK::StartUsing(EntityId passengerId)
{
	m_passengerId = passengerId;

	m_pVehicle->SetObjectUpdate(this, IVehicle::eVOU_Visible);
}

//------------------------------------------------------------------------
void CVehicleSeatActionPassengerIK::StopUsing()
{
	m_passengerId = 0;

	m_pVehicle->SetObjectUpdate(this, IVehicle::eVOU_NoUpdate);
}

//------------------------------------------------------------------------
void CVehicleSeatActionPassengerIK::Update(float frameTime)
{
	if (!m_passengerId)
		return;

	IActor* pActor = gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(m_passengerId);
	if (!pActor)
	{
		assert(!"Invalid entity id for the actor id, Actor System didn't know it");
		return;
	}

	if (ICharacterInstance* pCharInstance = pActor->GetEntity()->GetCharacter(0))
	{
		ISkeletonAnim* pSkeletonAnim = pCharInstance->GetISkeletonAnim();
		assert(pSkeletonAnim);

		if (pSkeletonAnim->GetNumAnimsInFIFO(0) >= 1)
		{
			CAnimation& anim = pSkeletonAnim->GetAnimFromFIFO(0, 0);
			if (!m_waitShortlyBeforeStarting || (anim.m_nLoopCount > 0 || anim.m_fAnimTime > 0.9f))
			{
				for (TIKLimbVector::iterator ite = m_ikLimbs.begin(); ite != m_ikLimbs.end(); ++ite)
				{
					const SIKLimb& limb = *ite;

					Vec3 helperPos = limb.pHelper->GetWorldTM().GetTranslation();
					pActor->SetIKPos(limb.limbName.c_str(), helperPos, 1);
				}
			}
		}
	}

}

void CVehicleSeatActionPassengerIK::GetMemoryStatistics(ICrySizer * s)
{
	s->Add(*this);
	s->AddContainer(m_ikLimbs);
	for (size_t i=0; i<m_ikLimbs.size(); i++)
	{
		s->Add(m_ikLimbs[i].limbName);
	}
}

DEFINE_VEHICLEOBJECT(CVehicleSeatActionPassengerIK);
