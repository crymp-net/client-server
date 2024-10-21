/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a wheel vehicle part

-------------------------------------------------------------------------
History:
- 26:08:2005: Created by Mathieu Pinard

*************************************************************************/
#include "StdAfx.h"

#include "ICryAnimation.h"
#include "IVehicleSystem.h"

#include "CryAction.h"
#include "Vehicle.h"
#include "VehiclePartBase.h"
#include "VehiclePartSubPartWheel.h"
#define THREAD_SAFE 1	

//------------------------------------------------------------------------
CVehiclePartSubPartWheel::CVehiclePartSubPartWheel()
{
  m_typeId = eVPT_Wheel;

  m_physGeomParams.bCanBrake = 1;      
  m_physGeomParams.minFriction = m_physGeomParams.maxFriction = 1.f;
  m_physGeomParams.kLatFriction = 1.f;
  m_physGeomParams.pivot.zero();
  m_rimRadius = 0.f;
  m_suspLength = 0.0f;
  m_slipFrictionMod = 0.f;
  m_slipSlope = 1.f;
  m_wheelIndex = 0;
  m_torqueScale = 1.f;
}

//------------------------------------------------------------------------
bool CVehiclePartSubPartWheel::Init(IVehicle* pVehicle, const CVehicleParams& table, IVehiclePart* parent, CVehicle::SPartInitInfo& initInfo)
{
	if (!CVehiclePartSubPart::Init(pVehicle, table, parent, initInfo))
		return false;
  
  CVehicleParams subTable = table.findChild("SubPartWheel");
  if (!subTable)
    return false;

  bool bTemp = false;

	subTable.getAttr("axle", m_physGeomParams.iAxle);	
	subTable.getAttr("density", m_physGeomParams.density);
	subTable.getAttr("damping", m_physGeomParams.kDamping);	
	subTable.getAttr("lenInit", m_physGeomParams.lenInitial);
	subTable.getAttr("lenMax", m_physGeomParams.lenMax);	
	subTable.getAttr("maxFriction", m_physGeomParams.maxFriction);
	subTable.getAttr("minFriction", m_physGeomParams.minFriction);      
  subTable.getAttr("latFriction", m_physGeomParams.kLatFriction);      
	subTable.getAttr("stiffness", m_physGeomParams.kStiffness);
	subTable.getAttr("surfaceId", m_physGeomParams.surface_idx);
	subTable.getAttr("suspLength", m_suspLength);
  subTable.getAttr("slipFrictionMod", m_slipFrictionMod);
  subTable.getAttr("slipSlope", m_slipSlope);
  subTable.getAttr("rimRadius", m_rimRadius);
  subTable.getAttr("torqueScale", m_torqueScale);

  if (subTable.getAttr("canBrake", bTemp))
    m_physGeomParams.bCanBrake = bTemp;
  if (subTable.getAttr("driving", bTemp))
    m_physGeomParams.bDriving = bTemp;
  if (subTable.getAttr("rayCast", bTemp))
    m_physGeomParams.bRayCast = bTemp;

  m_physGeomParams.mass = m_mass;
  
	m_wheelIndex = pVehicle->GetWheelCount();
	reinterpret_cast<CVehicle*>(pVehicle)->m_wheelCount++;

  InitGeometry();

	return true;
}

//------------------------------------------------------------------------
void CVehiclePartSubPartWheel::Reset()
{

	CVehiclePartSubPart::Reset();

}

//------------------------------------------------------------------------
void CVehiclePartSubPartWheel::OnEvent(const SVehiclePartEvent& event)
{
	CVehiclePartSubPart::OnEvent(event);
}

//------------------------------------------------------------------------
void CVehiclePartSubPartWheel::InitGeometry()
{
	if (m_slot == -1)
	{
		m_slot = 0;
    
    while (GetEntity()->IsSlotValid(m_slot))
			m_slot++;
	}

  ChangeState(eVGS_Default);
}

//------------------------------------------------------------------------
bool CVehiclePartSubPartWheel::ChangeState(EVehiclePartState state, int flags)
{
  EVehiclePartState old = m_state;

  if (state == eVGS_Default)
  {
	IPhysicalEntity* pPhysics = m_pVehicle->GetEntity()->GetPhysics();
	if ( pPhysics )
	{
		pe_params_wheel wheelParams;
		wheelParams.iWheel = m_wheelIndex;      
		wheelParams.suspLenMax = m_physGeomParams.lenMax;
		wheelParams.suspLenInitial = m_suspLength;
		pPhysics->SetParams(&wheelParams, THREAD_SAFE);
	}		
  }

  if (state == eVGS_Destroyed)
  {
	IPhysicalEntity* pPhysics = m_pVehicle->GetEntity()->GetPhysics();
	if ( pPhysics )
	{
		pe_params_wheel wheelParams;
		wheelParams.iWheel = m_wheelIndex;      
		wheelParams.suspLenMax = 0.0f;
		pPhysics->SetParams(&wheelParams, THREAD_SAFE);
	}		
  }

  if (!CVehiclePartSubPart::ChangeState(state, flags))
    return false;
  
  if (old == eVGS_Damaged1 && m_state == eVGS_Default)
  {
    // tire repaired
    if (IPhysicalEntity* pPhysics = m_pVehicle->GetEntity()->GetPhysics())
    {
      pe_action_impulse imp;
      imp.impulse = m_pVehicle->GetEntity()->GetWorldRotation() * Vec3(0.f, 0.f, m_pVehicle->GetMass()/m_pVehicle->GetWheelCount());
      imp.point = GetWorldTM().GetTranslation();
      pPhysics->Action(&imp);
    }    
  }

  return true;
}

//------------------------------------------------------------------------
void CVehiclePartSubPartWheel::GetGeometryName(EVehiclePartState state, string& name)
{
  if (state == eVGS_Damaged1)
  {
    name = GetName() + "_rim";
    return;
  }
  else
    CVehiclePartSubPart::GetGeometryName(state, name);
}

//------------------------------------------------------------------------
void CVehiclePartSubPartWheel::Physicalize()
{
  IPhysicalEntity* pPhysics = GetEntity()->GetPhysics();
  if (!pPhysics)
    return;

  IStatObj* pStatObj = GetEntity()->GetStatObj(m_slot);
  phys_geometry* pPhysGeom = pStatObj ? pStatObj->GetPhysGeom() : 0;
  bool updated = false;
  
  if (m_slot != -1 && m_physId != -1)
  {
    pe_status_pos status;
    status.partid = m_physId;
    if (pPhysics->GetStatus(&status))
    { 
      if (pStatObj && pPhysGeom)
      {
        Matrix34 tm(m_savedTM);
        pe_params_part params;        
        params.partid = m_physId;
        params.pPhysGeom = pPhysGeom;
        params.pMtx3x4 = &tm;        
        pPhysics->SetParams(&params);
      }
      else
      {
        pPhysics->RemoveGeometry(m_physId);
        m_physId = -1;
      }

      updated = true;
    }   
  }
  
  if (!updated && pStatObj && pPhysGeom)
  {
    Matrix34 slotTM = VALIDATE_MAT(m_savedTM);
    
    m_physGeomParams.pMtx3x4 = &slotTM;
    m_physGeomParams.pivot = slotTM.GetTranslation();
    m_physGeomParams.lenInitial = m_suspLength;    
    m_physGeomParams.flags &= ~geom_floats;
        
    if (m_pVehicle->GetMass() >= VEHICLE_MASS_COLLTYPE_HEAVY)      
    {
      m_physGeomParams.flags &= ~geom_colltype_debris;
      m_physGeomParams.flagsCollider = geom_colltype_vehicle;
    }

    m_physGeomParams.flagsCollider |= geom_colltype6; // vehicle-only colltype

    // use copy, physics eventually writes to the passed struct
    pe_cargeomparams params(m_physGeomParams);
		
    if (m_pVehicle->GetWheelCount()>8	&& params.lenMax>0)
    {
			if (params.bRayCast)
				params.flags = geom_colltype_player|geom_colltype_ray; // no object collisions vs inner tank wheels
			else if (VehicleCVars().v_vehicle_quality==1)
				params.bRayCast = 1;
    }
    
    m_physId = pPhysics->AddGeometry(pPhysGeom, &params, m_slot);

    if (m_physId < 0)        
      GameWarning("AddGeometry failed on <%s> (part %s)", m_pVehicle->GetEntity()->GetName(), m_name.c_str());    
  }

  if (m_physId != -1)
  {
    pe_params_wheel wp;
    wp.iWheel = m_wheelIndex;
    wp.Tscale = m_torqueScale;
    pPhysics->SetParams(&wp);
  }

  if (IsDebugParts())
    CryLog("[CVehicleSubPartWheel]: <%s> got physId %i", m_name.c_str(), m_physId);  
}


//------------------------------------------------------------------------
void CVehiclePartSubPartWheel::Serialize(TSerialize ser, EEntityAspects aspects)
{
	CVehiclePartBase::Serialize(ser, aspects);
  
  if (ser.GetSerializationTarget() != eST_Network)
	{
    IPhysicalEntity* pPhysics = GetEntity()->GetPhysics();
    if (!pPhysics)
      return;

		if (ser.IsWriting())
		{
			pe_params_wheel wheelParams;
			wheelParams.iWheel = m_wheelIndex;
			if (pPhysics->GetParams(&wheelParams))
			{
				float maxFrictionRatio = wheelParams.maxFriction / m_physGeomParams.maxFriction;
				float minFrictionRatio = wheelParams.minFriction / m_physGeomParams.minFriction;

				ser.Value( "maxFrictionRatio", maxFrictionRatio, 'vWFr');
				ser.Value( "minFrictionRatio", minFrictionRatio, 'vWFr');
			}
		}
		else if (ser.IsReading())
		{
			float maxFrictionRatio = 0.0f, minFrictionRatio = 0.0f;

			ser.Value( "maxFrictionRatio", maxFrictionRatio, 'vWFr');
			ser.Value( "minFrictionRatio", minFrictionRatio, 'vWFr');

			pe_params_wheel wheelParams;
			wheelParams.iWheel = m_wheelIndex;
			wheelParams.maxFriction = m_physGeomParams.maxFriction * maxFrictionRatio;
			wheelParams.minFriction = m_physGeomParams.minFriction * minFrictionRatio;
			pPhysics->SetParams(&wheelParams);
		}
	}
}

//------------------------------------------------------------------------
void CVehiclePartSubPartWheel::PostSerialize()
{
	CVehiclePartBase::PostSerialize();
}

//------------------------------------------------------------------------
float CVehiclePartSubPartWheel::GetSlipFrictionMod(float slip) const
{ 
  if (GetState() != eVGS_Default) // 0 for damaged wheels
    return 0.f;

  float mod = IsFront() ? VehicleCVars().v_slipFrictionModFront : VehicleCVars().v_slipFrictionModRear;
  if (mod <= 0.f)
    mod = m_slipFrictionMod;

  float slope = IsFront() ? VehicleCVars().v_slipSlopeFront : VehicleCVars().v_slipSlopeRear;
  if (slope <= 0.f)
    slope = m_slipSlope;

  return (slip * mod) / (slip + mod*slope);
}


DEFINE_VEHICLEOBJECT(CVehiclePartSubPartWheel);
