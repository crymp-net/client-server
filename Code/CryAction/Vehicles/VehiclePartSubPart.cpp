/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a part for vehicles which is the an attachment 
of a parent Animated part

-------------------------------------------------------------------------
History:
- 25:08:2005: Created by Mathieu Pinard

*************************************************************************/
#include "StdAfx.h"

#include "ICryAnimation.h"
#include "IVehicleSystem.h"

#include "CryAction.h"
#include "Vehicle.h"
#include "VehiclePartBase.h"
#include "VehiclePartAnimated.h"
#include "VehiclePartSubPart.h"


//------------------------------------------------------------------------
bool CVehiclePartSubPart::Init(IVehicle* pVehicle, const CVehicleParams& table, IVehiclePart* parent, CVehicle::SPartInitInfo& initInfo)
{
	if (!CVehiclePartBase::Init(pVehicle, table, parent, initInfo))
		return false;

  m_savedTM.SetIdentity();
  InitGeometry();
  
	return true;
}


//------------------------------------------------------------------------
void CVehiclePartSubPart::InitGeometry()
{    
}

//------------------------------------------------------------------------
void CVehiclePartSubPart::Release()
{
	if (m_slot != -1)
		m_pVehicle->GetEntity()->FreeSlot(m_slot);

	CVehiclePartBase::Release();
}

//------------------------------------------------------------------------
void CVehiclePartSubPart::Reset()
{
	CVehiclePartBase::Reset();
  
  if (m_slot > -1)
  {
    GetEntity()->SetSlotLocalTM(m_slot, m_savedTM);    
  }  
}

//------------------------------------------------------------------------
void CVehiclePartSubPart::OnEvent(const SVehiclePartEvent& event)
{
	CVehiclePartBase::OnEvent(event);
}

//------------------------------------------------------------------------
bool CVehiclePartSubPart::ChangeState(EVehiclePartState state, int flags)
{
  if (!CVehiclePartBase::ChangeState(state, flags))
    return false;

  CVehiclePartBase* pPartBase = (CVehiclePartBase*) m_pParentPart;
  if (!pPartBase)
    return false;

  bool swapped = false;

  if (IsDebugParts())
    CryLog("SubPart %s: ChangeState ( %i -> %i %s)", m_name.c_str(), m_state, state, flags&eVPSF_Force ? ", forced":"");
	
  Matrix34 savedTM(m_savedTM);
  bool removeFromParent = (state == eVGS_Default);
	
  if (IStatObj* pStatObj = pPartBase->GetSubGeometry(this, state, savedTM, removeFromParent))
	{
    IStatObj* pOldObj = GetEntity()->GetStatObj(m_slot);

    if (IsDebugParts() && pOldObj)
      CryLog("SubPart %s: replacing StatObj %s (slot %i)", m_name.c_str(), pOldObj->GetGeoName(), m_slot);
		
		m_slot = GetEntity()->LoadGeometry(m_slot, pStatObj->GetFilePath(), pStatObj->GetGeoName());

    if (IsDebugParts())
      CryLog("SubPart %s: loaded %s to slot %i", m_name.c_str(), pStatObj->GetGeoName(), m_slot);

		if (m_slot == -1)
			return false;

    Matrix34 localTM(IDENTITY);		
		if (!m_position.IsZero())
		{
			localTM.SetTranslation(m_position);
		}
		else if (m_helperPosName.empty())
		{
			localTM = savedTM;				
			localTM.SetTranslation(savedTM.GetTranslation());
		}
		else
		{
			if (IVehicleHelper* pHelper = m_pVehicle->GetHelper(m_helperPosName.c_str()))
				localTM = pHelper->GetVehicleTM();			
		}

		GetEntity()->SetSlotLocalTM(m_slot, VALIDATE_MAT(localTM));
    m_savedTM = localTM;

    swapped = true;
	}
  else
  {
    if (IsDebugParts())
      CryLog("[CVehiclePartSubPart] <%s> not found on <%s>", m_name.c_str(), pPartBase->GetName().c_str());
  }

  if (m_slot > -1)
  {
    if (m_hideCount==0 && swapped)
      GetEntity()->SetSlotFlags(m_slot, GetEntity()->GetSlotFlags(m_slot)|ENTITY_SLOT_RENDER);
    else
      GetEntity()->SetSlotFlags(m_slot, GetEntity()->GetSlotFlags(m_slot)&~(ENTITY_SLOT_RENDER|ENTITY_SLOT_RENDER_NEAREST));
  }

  m_state = state;
  
  if (swapped && flags&eVPSF_Physicalize)
    Physicalize();
  
  return true;
}

//------------------------------------------------------------------------
void CVehiclePartSubPart::Physicalize()
{
	if (m_slot == -1)
		return;

  GetEntity()->UnphysicalizeSlot(m_slot);

  if (!m_isPhysicalized) 
    return;

  SEntityPhysicalizeParams params;
  params.mass = m_mass;
  params.density = m_density;
  params.nSlot = m_slot;
	
	if (m_disableCollision)
		params.nFlagsAND = ~(geom_collides|geom_floats);  

  m_physId = GetEntity()->PhysicalizeSlot(m_slot, params);

	if (GetEntity()->GetPhysics() && m_physId > -1)
	{
		const Matrix34& localTM = VALIDATE_MAT(GetEntity()->GetSlotLocalTM(m_slot, false));
		pe_params_part paramsPart;
		paramsPart.partid = m_physId;
		paramsPart.pos = localTM.GetTranslation();        
		paramsPart.q = Quat(localTM);
		GetEntity()->GetPhysics()->SetParams(&paramsPart);

    CheckColltypeHeavy(m_physId);

		GetEntity()->SetSlotFlags(m_slot, GetEntity()->GetSlotFlags(m_slot)|ENTITY_SLOT_IGNORE_PHYSICS);
	}
}

//------------------------------------------------------------------------
void CVehiclePartSubPart::Update(const float frameTime)
{
	CVehiclePartBase::Update(frameTime);

	/*
	if (m_physId > -1 && m_rotationChanged)
	{
		pe_params_part partParams;
		partParams.partid = m_physId;
		partParams.q = Quat(Matrix33(GetLocalTM()));
		m_pVehicle->GetEntity()->GetPhysics()->SetParams(&partParams, 1);
	}*/
}

DEFINE_VEHICLEOBJECT(CVehiclePartSubPart);
