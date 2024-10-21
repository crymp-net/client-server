/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a base class for vehicle parts

-------------------------------------------------------------------------
History:
- 23:08:2005: Created by Mathieu Pinard

*************************************************************************/
#include "CryCommon/CrySystem/ISystem.h"

#include "CryCommon/CryAnimation/ICryAnimation.h"
//#include "CryAction.h"
#include "Vehicle.h"
#include "VehiclePartBase.h"
#include "VehicleComponent.h"
#include "VehicleSeat.h"
#include "VehicleSeatSerializer.h"

const char * CVehiclePartBase::m_geometryDestroyedSuffix = "_damaged";


CVehiclePartBase::CVehiclePartBase()
: m_slot( -1 )
, m_typeId( eVPT_Base )
, m_physId( -1 )
, m_pSeat( 0 )
, m_state( eVGS_Last )
, m_users(0)
, m_pParentPart(NULL)
, m_pVehicle(NULL)
{
  m_position.zero();  
  m_hideCount = 0;
  m_useOption = -1;
  m_mass = 0.0f;
  m_density = -1.0f;
  m_isPhysicalized = true;
  m_disableCollision = false;  
  m_damageRatio = 0.0f;  
  m_pClonedMaterial = NULL;
  m_hideMode = eVPH_NoFade;  
  m_detachBaseForce.zero();
  m_detachProbability = 0.f;
  m_hideTimeCount = 0.f;
  m_hideTimeMax = 0.f;  
  m_pComponentName = 0;  
}

//------------------------------------------------------------------------
CVehiclePartBase::~CVehiclePartBase()
{   
}

//------------------------------------------------------------------------
bool CVehiclePartBase::Init(IVehicle* pVehicle, const CVehicleParams& table, IVehiclePart* pParent, CVehicle::SPartInitInfo& initInfo)
{	
	m_pVehicle = (CVehicle*) pVehicle;
	
	m_name = table.getAttr("name");
	if (m_name.empty())
		return false;

	m_helperPosName = table.getAttr("helper");
  
  table.getAttr("position", m_position);
	bool hidden = false;
	table.getAttr("isHidden", hidden);  
	m_hideCount = (hidden == true) ? 1 : 0;
  table.getAttr("useOption", m_useOption);
	table.getAttr("mass", m_mass);
  table.getAttr("density", m_density);

	reinterpret_cast<CVehicle*>(pVehicle)->m_mass += m_mass;

	m_pParentPart = pParent;

  if (pParent)
    pParent->AddChildPart(this);

	if (table.getAttr("disablePhysics", m_isPhysicalized))
		m_isPhysicalized = !m_isPhysicalized;

	table.getAttr("disableCollision", m_disableCollision);
	
  string component = table.getAttr("component");
  if (!component.empty())
    initInfo.partComponentMap.push_back(CVehicle::SPartComponentPair(this, component.c_str()));

  // read optional multiple components (part->component relationship is now 1:n)
  if (CVehicleParams components = table.findChild("Components"))
  {
		int i = 0;
		int c = components.getChildCount();
		
    for (; i < c; i++)
		{
			CVehicleParams componentRef = components.getChild(i);
      initInfo.partComponentMap.push_back(CVehicle::SPartComponentPair(this, componentRef.getAttr("value")));
		}
  }
	
  string filename = table.getAttr("filename");
  if (!filename.empty())
  { 
    if (filename.find("."))
      m_slot = GetEntity()->LoadGeometry(m_slot, filename.c_str());
    else
      m_slot = GetEntity()->LoadCharacter(m_slot, filename.c_str());
  }

	m_isRotationBlocked = false;

	return true;
}

//------------------------------------------------------------------------
void CVehiclePartBase::PostInit()
{
}

//------------------------------------------------------------------------
void CVehiclePartBase::InitComponent()
{
  if (m_pComponentName)
  {
    CVehicleComponent* pVehicleComponent = static_cast<CVehicleComponent*>(m_pVehicle->GetComponent(m_pComponentName));
    
    if (pVehicleComponent)
      pVehicleComponent->AddPart(this);

    m_pComponentName = 0;
  }
}

//------------------------------------------------------------------------
EntityId CVehiclePartBase::SpawnEntity()
{
	char pPartName[128];
	_snprintf(pPartName, 128, "%s_part_%s", m_pVehicle->GetEntity()->GetName(), m_name.c_str());
  pPartName[sizeof(pPartName)-1] = '\0';

	SEntitySpawnParams params;
	params.sName = pPartName;
	params.nFlags = ENTITY_FLAG_CLIENT_ONLY;
	params.pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass("VehiclePart");

	IEntity* pPartEntity = gEnv->pEntitySystem->SpawnEntity(params, true);
	if (pPartEntity)
	{
		if (m_pParentPart)
			m_pParentPart->GetEntity()->AttachChild(pPartEntity);
		else
			m_pVehicle->GetEntity()->AttachChild(pPartEntity);

		pPartEntity->SetFlags(pPartEntity->GetFlags() | ENTITY_FLAG_CASTSHADOW );

		return pPartEntity->GetId();
	}
	
	assert(0);
	return 0;
}

//------------------------------------------------------------------------
IEntity* CVehiclePartBase::GetEntity()
{
	if (m_pParentPart)
		return m_pParentPart->GetEntity();
	else
		return m_pVehicle->GetEntity();
}

//------------------------------------------------------------------------
void CVehiclePartBase::Release()
{
  delete this;
}

//------------------------------------------------------------------------
void CVehiclePartBase::Reset()
{ 
	m_damageRatio = 0.0f;	

  ResetLocalTM(true);

  ChangeState(eVGS_Default);   
}

//------------------------------------------------------------------------
void CVehiclePartBase::OnEvent(const SVehiclePartEvent& event)
{
  switch (event.type)
  {  
  case eVPE_Damaged:
	  { 
      float old = m_damageRatio;
		  m_damageRatio = event.fparam;
  		
      if (m_damageRatio >= 1.0f && old < 1.f)
      {
        EVehiclePartState state = GetStateForDamageRatio(m_damageRatio);
        ChangeState(state, eVPSF_Physicalize);
      }
      else if (m_damageRatio == 0.f)
      {
        ChangeState(eVGS_Default);
      }      
	  }  
    break;  
  case eVPE_Repair:
    {
      float old = m_damageRatio;
      m_damageRatio = max(0.f, event.fparam);
      
      if (eVPT_Base == m_typeId && m_damageRatio <= COMPONENT_DAMAGE_LEVEL_RATIO && m_hideCount==0)
        Hide(false);    

      if (m_damageRatio <= COMPONENT_DAMAGE_LEVEL_RATIO && old > COMPONENT_DAMAGE_LEVEL_RATIO)
        ChangeState(eVGS_Default, eVPSF_Physicalize);      
    }
    break;
  case eVPE_StartUsing:
    {
		  m_users++;

		  if (m_users == 1)
		  {
			  m_pVehicle->SetObjectUpdate(this, IVehicle::eVOU_AlwaysUpdate);
		  }
    }  
    break;  
  case eVPE_StopUsing:
    {
		  m_users = max(--m_users, 0);

		  if (m_users <= 0)
		  {
			  m_pVehicle->SetObjectUpdate(this, IVehicle::eVOU_NoUpdate);
		  }
    }
    break;
  case eVPE_GotDirty:
	  break;
  case eVPE_Hide:
	  {
		  if (event.bparam)
			  m_hideMode = eVPH_FadeOut;
		  else
			  m_hideMode = eVPH_FadeIn;

		  m_hideTimeCount = event.fparam;
		  m_hideTimeMax = event.fparam;

		  m_pVehicle->SetObjectUpdate(this, IVehicle::eVOU_AlwaysUpdate);
	  }
    break;
  case eVPE_BlockRotation:
 		m_isRotationBlocked = event.bparam;      	  
    break;
  }
}

//------------------------------------------------------------------------
void CVehiclePartBase::Hide(bool hide)
{
  if (m_slot == -1)
    return;

  IEntity* pEntity = GetEntity();
  if (!pEntity)
    return;

  if (hide)
	{
		++m_hideCount;

		if(m_hideCount > 0)
			pEntity->SetSlotFlags(m_slot, pEntity->GetSlotFlags(m_slot)&~ENTITY_SLOT_RENDER);
	}
  else
	{
		--m_hideCount;

		if(m_hideCount <= 0)
			pEntity->SetSlotFlags(m_slot, pEntity->GetSlotFlags(m_slot)|ENTITY_SLOT_RENDER);
	}
}

//------------------------------------------------------------------------
IVehiclePart::EVehiclePartState CVehiclePartBase::GetMaxState()
{
  EVehiclePartState state = m_state;
  
  for (TVehicleChildParts::iterator it=m_children.begin(), end=m_children.end(); it!=end; ++it)
  {
    EVehiclePartState maxState = (*it)->GetMaxState();
    
    if (maxState > state && maxState != eVGS_Last)
      state = maxState;    
  }

  return state;
}

//------------------------------------------------------------------------
IVehiclePart::EVehiclePartState CVehiclePartBase::GetStateForDamageRatio(float ratio)
{
  if (ratio >= 1.f)
    return m_pVehicle->IsDestroyed() ? eVGS_Destroyed : eVGS_Damaged1;

  return eVGS_Default;
}


//------------------------------------------------------------------------
bool CVehiclePartBase::ChangeState(EVehiclePartState state, int flags)
{ 
	if (state == m_state && !(flags&eVPSF_Force))
		return false;

  // once destroyed, we only allow to return to default state
  if (m_state == eVGS_Destroyed && state != eVGS_Default && !(flags&eVPSF_Force))
    return false;

  if (eVPT_Base == m_typeId && m_hideCount==0)
  {
    if (state == eVGS_Destroyed)
      Hide(true);
    else if (state == eVGS_Default)
      Hide(false);
  }

	return true;
}

//------------------------------------------------------------------------
IStatObj* CVehiclePartBase::GetSubGeometry(CVehiclePartBase* pPart, EVehiclePartState state, Matrix34& position, bool removeFromParent)
{
	if (CVehiclePartBase* pParentPartBase = (CVehiclePartBase*) m_pParentPart)
		return pParentPartBase->GetSubGeometry(pPart, state, position, removeFromParent);
	else
		return NULL;
}


//------------------------------------------------------------------------
void CVehiclePartBase::GetGeometryName(EVehiclePartState state, string& name)
{
  name = GetName(); 

  switch (state)
  {  
  case eVGS_Destroyed:
    name += m_geometryDestroyedSuffix; 
    break;
  case eVGS_Damaged1:
    name += "_damaged_1";
    break;
  }  
}


//------------------------------------------------------------------------
const Matrix34& CVehiclePartBase::GetLocalTM(bool relativeToParentPart)
{
  const Matrix34& tm = GetEntity()->GetSlotLocalTM(m_slot, relativeToParentPart);

	return VALIDATE_MAT(tm);  
}

//------------------------------------------------------------------------
void CVehiclePartBase::SetLocalTM(const Matrix34& localTM)
{
	GetEntity()->SetSlotLocalTM(m_slot, localTM);
}

//------------------------------------------------------------------------
void CVehiclePartBase::ResetLocalTM(bool recursive)
{
  if (recursive)
  {
    for (TVehicleChildParts::iterator it=m_children.begin(),end=m_children.end(); it!=end; ++it)  
      (*it)->ResetLocalTM(true);
  }    
}

//------------------------------------------------------------------------
const Matrix34& CVehiclePartBase::GetWorldTM()
{
	return VALIDATE_MAT(GetEntity()->GetSlotWorldTM(m_slot));
}


//------------------------------------------------------------------------
const Matrix34& CVehiclePartBase::LocalToVehicleTM(const Matrix34& localTM)
{
  //FUNCTION_PROFILER( gEnv->pSystem, PROFILE_ACTION );

  static Matrix34 tm;  
  tm = VALIDATE_MAT(localTM);

  IVehiclePart* pParent = GetParent();
  while (pParent)
  {
    tm = pParent->GetLocalTM(true) * tm;
    pParent = pParent->GetParent();
  }      

  return VALIDATE_MAT(tm);
}

//------------------------------------------------------------------------
const AABB& CVehiclePartBase::GetLocalBounds()
{
	if (m_slot == -1)
	{
		m_bounds.Reset();
	}
	else
	{
		
		if (IStatObj* pStatObj = GetEntity()->GetStatObj(m_slot))
		{
			m_bounds = pStatObj->GetAABB();
			m_bounds.SetTransformedAABB(GetEntity()->GetSlotLocalTM(m_slot, false), m_bounds);
		}
		else if (ICharacterInstance* pCharInstance = GetEntity()->GetCharacter(m_slot))
		{
		  m_bounds = pCharInstance->GetAABB();
      m_bounds.SetTransformedAABB(GetEntity()->GetSlotLocalTM(m_slot, false), m_bounds);
		}
    else
    {
      GetEntity()->GetLocalBounds(m_bounds);
    }		
	}  
	
  return VALIDATE_AABB(m_bounds);
}

//------------------------------------------------------------------------
void CVehiclePartBase::Update(float frameTime)
{   
  //FUNCTION_PROFILER( GetISystem(), PROFILE_ACTION );
  
	if (m_hideMode != eVPH_NoFade)
	{
		if (m_hideMode == eVPH_FadeIn)
		{
			m_hideTimeCount += frameTime;

			if (m_hideTimeCount >= m_hideTimeMax)
				m_pVehicle->SetObjectUpdate(this, IVehicle::eVOU_NoUpdate);
		}
		else if (m_hideMode == eVPH_FadeOut)
		{
			m_hideTimeCount -= frameTime;

			if (m_hideTimeCount <= 0.0f)
				m_pVehicle->SetObjectUpdate(this, IVehicle::eVOU_NoUpdate);
		}
		else
			assert(0);

		if (IMaterial* pMaterialMain = GetMaterial())
		{
			float opacity = min(1.0f, max(0.0f, (m_hideTimeCount / m_hideTimeMax)));

			if (IRenderShaderResources* pShaderRes = pMaterialMain->GetShaderItem().m_pShaderResources)
			{
				pShaderRes->GetOpacity() = opacity;
			}

			for (int i = 0; i < pMaterialMain->GetSubMtlCount(); i++)
			{
				if (IMaterial* pMaterial = pMaterialMain->GetSubMtl(i))
				{
					if (IRenderShaderResources* pShaderRes = pMaterial->GetShaderItem().m_pShaderResources)
						pShaderRes->GetOpacity() = opacity;
				}
			}
		}
	}
}

//------------------------------------------------------------------------
IMaterial* CVehiclePartBase::GetMaterial()
{
	if (m_pClonedMaterial)
		return m_pClonedMaterial;
	else
		return GetEntity()->GetMaterial();
}

//------------------------------------------------------------------------
void CVehiclePartBase::SetMaterial(IMaterial* pMaterial)
{
	// moved to CVehiclePartAnimated
}

//------------------------------------------------------------------------
void CVehiclePartBase::ParsePhysicsParams(SEntityPhysicalizeParams& physicalizeParams, const CVehicleParams& table)
{
	table.getAttr("mass", physicalizeParams.mass);
	table.getAttr("density", physicalizeParams.density);
}

//------------------------------------------------------------------------
void CVehiclePartBase::CheckColltypeHeavy(int partid)
{
  if (partid < 0 || !GetEntity()->GetPhysics())
    return;

  if (!m_disableCollision)
  {
    pe_params_part paramsPart;
    paramsPart.partid = partid;
    
    if (m_pVehicle->GetMass() >= VEHICLE_MASS_COLLTYPE_HEAVY)
    {
      paramsPart.flagsAND = ~geom_colltype_debris; // light debris from breakable objs
      paramsPart.flagsColliderAND = geom_colltype3;
      paramsPart.flagsColliderOR = geom_colltype3; // for heavy vehicles
    }

    paramsPart.flagsColliderOR |= geom_colltype6; // vehicle-only colltype
    
    GetEntity()->GetPhysics()->SetParams(&paramsPart);
  }  
}

//------------------------------------------------------------------------
void CVehiclePartBase::SetFlags(int partid, int flags, bool add)
{
  if (partid < 0 || !GetEntity()->GetPhysics())
    return;

  pe_params_part paramsPart;
  paramsPart.partid = partid;
  
  if (add)
  {
    paramsPart.flagsOR = flags;     
  }
  else
  {
    paramsPart.flagsAND = ~flags;      
  }

  GetEntity()->GetPhysics()->SetParams(&paramsPart);  
}

//------------------------------------------------------------------------
void CVehiclePartBase::SetFlagsCollider(int partid, int flagsCollider)
{
  if (partid < 0 || !GetEntity()->GetPhysics())
    return;

  pe_params_part paramsPart;
  paramsPart.partid = partid;

  paramsPart.flagsColliderAND = 0;
  paramsPart.flagsColliderOR = flagsCollider;    

  GetEntity()->GetPhysics()->SetParams(&paramsPart);  
}

//------------------------------------------------------------------------
IVehiclePart* CVehiclePartBase::GetParent(bool root/*=false*/)
{
  IVehiclePart* pParent = m_pParentPart;

  if (root && pParent)
  {
    while (IVehiclePart* pRoot = pParent->GetParent(false))
      pParent = pRoot;
  }

  return pParent;
}

//------------------------------------------------------------------------
void CVehiclePartBase::Serialize(TSerialize ser, unsigned aspects)
{	
	bool bSaveGame = ser.GetSerializationTarget() != eST_Network;

	if (bSaveGame)		  
	{	
		ser.BeginGroup("VehiclePart");

		ser.Value("m_damageRatio", m_damageRatio);    
		ser.EnumValue("m_hideMode", m_hideMode, eVPH_NoFade, eVPH_FadeOut);
		ser.Value("m_hideTimeCount", m_hideTimeCount);   
	} 

  if (bSaveGame || aspects&CVehicle::ASPECT_COMPONENT_DAMAGE)
  {
    EVehiclePartState state = m_state;
    ser.EnumValue("m_state", state, eVGS_Default, eVGS_Last);	

    if (ser.IsReading())
    { 
      ChangeState(state, eVPSF_Physicalize);
    }
  }

  if (!bSaveGame)
	{
		if (aspects & CVehicle::ASPECT_PART_MATRIX)
		{			
			Quat q;
			Matrix34 currentTM = GetLocalBaseTM();
			if (ser.IsWriting())
				q=Quat(currentTM);

			string tag;
			ser.Value("rotation", q, 'ori1');

			if (ser.IsReading())
			{			
				Matrix34 newTM(q);
				newTM.SetTranslation(currentTM.GetTranslation());
				SetLocalBaseTM(newTM);
			}
		}
	}

	if (bSaveGame)
		ser.EndGroup();
}

//------------------------------------------------------------------------
void CVehiclePartBase::CloneMaterial()
{
	if (m_pClonedMaterial)
		return;

	if (IMaterial* pMaterialOriginal = GetMaterial())
	{
		m_pClonedMaterial = pMaterialOriginal->GetMaterialManager()->CloneMultiMaterial(pMaterialOriginal);
		if (m_pClonedMaterial)
			SetMaterial(m_pClonedMaterial);
	}
}


//------------------------------------------------------------------------
int CVehiclePartBase::GetCGASlot(int jointId)
{ 
  assert(m_slot >= 0);

  if (m_slot < 0) 
    return -1; 

  return PARTID_CGA * (m_slot+1) + jointId;
}

void CVehiclePartBase::GetBaseMemoryStatistics(ICrySizer * s)
{
	s->Add(m_name);	
	s->Add(m_helperPosName);	
}


DEFINE_VEHICLEOBJECT(CVehiclePartBase);
