/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a part for vehicles which uses animated characters

-------------------------------------------------------------------------
History:
- 24:08:2005: Created by Mathieu Pinard

*************************************************************************/
#include "StdAfx.h"

#include "ICryAnimation.h"
#include "IVehicleSystem.h"

#include "CryAction.h"
#include "Vehicle.h"
#include "VehiclePartBase.h"
#include "VehiclePartAnimated.h"
#include "VehiclePartAnimatedJoint.h"

int CVehiclePartAnimated_ProcessRotations(ICharacterInstance* pCharInstance, CVehiclePartAnimated* pPartAnimated);

//------------------------------------------------------------------------
CVehiclePartAnimated::CVehiclePartAnimated()
: m_pCharInstanceDestroyed(NULL),
  m_pCharInstance(NULL)
{
	m_typeId = eVPT_Animated;

	m_hullMatId[0] = m_hullMatId[1] = -1;
	m_lastAnimLayerAssigned = 0;
	m_iRotChangedFrameId = 0;    
	m_serializeForceRotationUpdate = false;
	m_initialiseOnChangeState = true;
}

//------------------------------------------------------------------------
CVehiclePartAnimated::~CVehiclePartAnimated()
{ 
  if (m_pCharInstanceDestroyed)    
  {
    m_pCharInstanceDestroyed->Release();		  
    m_pCharInstanceDestroyed = 0;
  }
}

namespace
{
  void CheckLowSpecFile(string& filename)
  {
    // try to load _low asset on lowspec    
    
    size_t dot = filename.rfind('.');    
    if (dot != ~0)
    {
      string stripped = filename.substr(0, dot);
      string suffix = filename.substr(dot);      
      
      string lowspecFile = stripped + "_low" + suffix;

      if (FILE *pFile = gEnv->pCryPak->FOpen(lowspecFile.c_str(), "rbx"))
      {        
        filename = lowspecFile;
        gEnv->pCryPak->FClose(pFile);

        CryLog("using lowspec file %s", lowspecFile.c_str());        
      }
    }    
  }
}

//------------------------------------------------------------------------
bool CVehiclePartAnimated::Init(IVehicle* pVehicle, const CVehicleParams& table, IVehiclePart* parent, CVehicle::SPartInitInfo& initInfo)
{
	CVehicleParams animatedTable = table.findChild("Animated");
	if (!animatedTable)
		return false;

	if (!CVehiclePartBase::Init(pVehicle, table, parent, initInfo))
		return false;

	ICharacterManager* pCharManager = gEnv->pCharacterManager;

	m_filename = animatedTable.getAttr("filename");
	if (!m_filename.empty())
	{
		m_filenameDestroyed = animatedTable.getAttr("filenameDestroyed");
    if (m_filenameDestroyed.empty())		
			m_filenameDestroyed = m_filename;

    if (VehicleCVars().v_vehicle_quality==1)
    {
      CheckLowSpecFile(m_filename);
      CheckLowSpecFile(m_filenameDestroyed);
    }
	}
	else
	{
		return false;
	}

	if (animatedTable.haveAttr("destroyedSuffix"))
		m_destroyedSuffix = animatedTable.getAttr("destroyedSuffix");
	else
		m_destroyedSuffix.clear();

  InitGeometry();
	
  m_state = eVGS_Default;

  m_pVehicle->SetObjectUpdate(this, IVehicle::eVOU_AlwaysUpdate);

	return true;
}

//------------------------------------------------------------------------
void CVehiclePartAnimated::Release()
{
  if (m_pCharInstanceDestroyed)
  {
    m_pCharInstanceDestroyed->Release();
    m_pCharInstanceDestroyed = 0;
  }

	if (m_slot != -1)
		m_pVehicle->GetEntity()->FreeSlot(m_slot);

  m_pCharInstance = 0;

	CVehiclePartBase::Release();
}

//------------------------------------------------------------------------
void CVehiclePartAnimated::Reset()
{
	CVehiclePartBase::Reset();
  	
  SetDrivingProxy(false);

	if (m_slot > -1 && m_pCharInstance)
	{	
		ISkeletonAnim* pSkeletonAnim = m_pCharInstance->GetISkeletonAnim();
		ISkeletonPose* pSkeletonPose = m_pCharInstance->GetISkeletonPose();
		CRY_ASSERT(pSkeletonAnim);

		pSkeletonAnim->StopAnimationsAllLayers();
		pSkeletonPose->SetDefaultPose();
    pSkeletonPose->SetForceSkeletonUpdate(0);

    for (int i=0; i<pSkeletonPose->GetJointCount(); ++i)
      pSkeletonPose->SetMaterialOnJoint(i, NULL);
	}

  m_iRotChangedFrameId = 0;
}

//------------------------------------------------------------------------
void CVehiclePartAnimated::OnEvent(const SVehiclePartEvent& event)
{
	CVehiclePartBase::OnEvent(event);
	
  switch (event.type)
  {
  case eVPE_DriverEntered:
    {
      if (!m_pVehicle->IsFlipped())
        SetDrivingProxy(true);          
    }    
    break;
  case eVPE_DriverLeft:
    {
      SetDrivingProxy(false);
    }    
    break;
  case eVPE_FlippedOver:
    {
      if (event.bparam)    
        SetDrivingProxy(false);
      else if (m_pVehicle->GetDriver())
        SetDrivingProxy(true);
    }
    break;   
  }
}

//------------------------------------------------------------------------
void CVehiclePartAnimated::OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params)
{
  CVehiclePartBase::OnVehicleEvent(event, params);

  switch (event)
  {
  case eVE_PreVehicleDeletion:
    { 
      break;
    }
  }
}

//------------------------------------------------------------------------
IVehiclePart::EVehiclePartState CVehiclePartAnimated::GetStateForDamageRatio(float ratio)
{
  if (ratio >= 1.f)
    return eVGS_Destroyed;

  return eVGS_Default;
}

//------------------------------------------------------------------------
void CVehiclePartAnimated::SetMaterial(IMaterial* pMaterial)
{  
  GetEntity()->SetMaterial(pMaterial);
}

//------------------------------------------------------------------------
void CVehiclePartAnimated::InitGeometry()
{	
  if (!m_filename.empty())
	{ 
    m_slot = GetEntity()->LoadCharacter(m_slot, m_filename.c_str());
    m_pCharInstance = GetEntity()->GetCharacter(m_slot);

		if (m_pCharInstance)
		{
      ISkeletonAnim* pSkeletonAnim = m_pCharInstance->GetISkeletonAnim();
			ISkeletonPose* pSkeletonPose = m_pCharInstance->GetISkeletonPose();
			
      if (pSkeletonAnim)
        pSkeletonAnim->StopAnimationsAllLayers();

			if (m_hideCount == 0)
				GetEntity()->SetSlotFlags(m_slot, GetEntity()->GetSlotFlags(m_slot)|ENTITY_SLOT_RENDER);
			else
				GetEntity()->SetSlotFlags(m_slot, GetEntity()->GetSlotFlags(m_slot)&~(ENTITY_SLOT_RENDER|ENTITY_SLOT_RENDER_NEAREST));

      if (IsDebugParts())
      {        
        CryLog("joint transformations for %s", m_pCharInstance->GetFilePath());
        
        for (int i=0; i<pSkeletonPose->GetJointCount(); ++i)      					
          VehicleUtils::LogMatrix(pSkeletonPose->GetJointNameByID(i), Matrix34(pSkeletonPose->GetRelJointByID(i)) );      
      }     
		} 
	  
    if (!m_pCharInstanceDestroyed && !m_filenameDestroyed.empty())
      m_pCharInstanceDestroyed = gEnv->pCharacterManager->CreateInstance(m_filenameDestroyed.c_str());

    if (m_pCharInstanceDestroyed)
    {
      m_pCharInstanceDestroyed->AddRef();

      if (IsDebugParts())
      {         
        CryLog("joint transformations for %s", m_pCharInstanceDestroyed->GetFilePath());
        
        ISkeletonPose* pSkeletonDestroyed = m_pCharInstanceDestroyed->GetISkeletonPose();
        for (int i=0; i<pSkeletonDestroyed->GetJointCount(); ++i)      					
          VehicleUtils::LogMatrix(pSkeletonDestroyed ->GetJointNameByID(i), Matrix34(pSkeletonDestroyed->GetRelJointByID(i)) );      
      }     
    }
	}

	if (m_isPhysicalized && m_slot > -1)
		GetEntity()->UnphysicalizeSlot(m_slot);
	
	if (m_pCharInstance)
	{
		m_pCharInstance->GetISkeletonAnim()->StopAnimationsAllLayers();

		if (m_hideCount == 0)
			GetEntity()->SetSlotFlags(m_slot, GetEntity()->GetSlotFlags(m_slot)|ENTITY_SLOT_RENDER);
		else
			GetEntity()->SetSlotFlags(m_slot, GetEntity()->GetSlotFlags(m_slot)&~(ENTITY_SLOT_RENDER|ENTITY_SLOT_RENDER_NEAREST));
	}	
}

//------------------------------------------------------------------------
IStatObj* CVehiclePartAnimated::GetGeometryForState(CVehiclePartBase* pPart, EVehiclePartState state)
{
  string name;
  pPart->GetGeometryName(state, name);

  IStatObj* pStatObj = 0;
  
  if (state > eVGS_Default)
  {
    ISkeletonPose* pSkeletonPose = m_pCharInstanceDestroyed ? m_pCharInstanceDestroyed->GetISkeletonPose() : 0;
    if (pSkeletonPose)
    {
      int jointId = pSkeletonPose->GetJointIDByName(name.c_str());

      if (jointId != -1)
        pStatObj = pSkeletonPose->GetStatObjOnJoint(jointId);
    }
  }
  else
  {
    TStringStatObjMap::const_iterator it = m_intactStatObjs.find(name.c_str());
    if (it != m_intactStatObjs.end())
      pStatObj = it->second;
  }
  
  return pStatObj;
}


//------------------------------------------------------------------------
IStatObj* CVehiclePartAnimated::GetDestroyedGeometry(const char* pJointName, unsigned int index)
{
	if (pJointName[0] && m_pCharInstanceDestroyed)
	{
		ISkeletonPose* pSkeletonDestroyed = m_pCharInstanceDestroyed->GetISkeletonPose();
		CRY_ASSERT(pSkeletonDestroyed);
 
    char buffer[256];

		const char* pSuffix = !m_destroyedSuffix.empty() ? m_destroyedSuffix.c_str() : 
			m_geometryDestroyedSuffix;

    if (index == 0)
      _snprintf(buffer, sizeof(buffer), "%s%s", pJointName, pSuffix);
		else
			_snprintf(buffer, sizeof(buffer), "%s_debris_%d", pJointName, index);

    buffer[sizeof(buffer)-1] = '\0';
    
		int16 jointIdForDestroyed = pSkeletonDestroyed->GetJointIDByName(buffer);
		if (jointIdForDestroyed > -1)
		{
			return pSkeletonDestroyed->GetStatObjOnJoint(jointIdForDestroyed);
		}
	}

	return NULL;
}

//------------------------------------------------------------------------
Matrix34 CVehiclePartAnimated::GetDestroyedGeometryTM(const char* pJointName, unsigned int index)
{
	if (pJointName[0] && m_pCharInstanceDestroyed)
	{
		ISkeletonPose* pSkeletonDestroyed = m_pCharInstanceDestroyed->GetISkeletonPose();
		CRY_ASSERT(pSkeletonDestroyed);

		char buffer[256];

		const char* pSuffix = !m_destroyedSuffix.empty() ? m_destroyedSuffix.c_str() : 
			m_geometryDestroyedSuffix;

		if (index == 0)
			_snprintf(buffer, sizeof(buffer), "%s%s", pJointName, pSuffix);
		else
			_snprintf(buffer, sizeof(buffer), "%s_debris_%d", pJointName, index);

		buffer[sizeof(buffer)-1] = '\0';

		int16 jointIdForDestroyed = pSkeletonDestroyed->GetJointIDByName(buffer);
		if (jointIdForDestroyed > -1)
			return Matrix34(pSkeletonDestroyed->GetAbsJointByID(jointIdForDestroyed));
	}

	Matrix34 identTM;
	identTM.SetIdentity();
	return identTM;
}



//------------------------------------------------------------------------
bool CVehiclePartAnimated::ChangeState(EVehiclePartState state, int flags)
{
	if ((state == eVGS_Default) && m_initialiseOnChangeState)
	{
		// Initialise!
		// Having to do this because of the way the glass code
		// swaps a cstatobj. The way the vehicle code stores its
		// statobj in m_intactStatObjs is going to need reviewing
		if (m_pCharInstance)
		{
			ISkeletonPose* pSkeletonPose = m_pCharInstance->GetISkeletonPose();
			if (pSkeletonPose)
			{
				for (uint32 i = 0; i < pSkeletonPose->GetJointCount(); i++)
				{
					if (IStatObj* pStatObjIntact = pSkeletonPose->GetStatObjOnJoint(i))
					{
						const char* jointName = pSkeletonPose->GetJointNameByID(i); 

						if (m_intactStatObjs.find(CONST_TEMP_STRING(jointName)) == m_intactStatObjs.end())
						{
							m_intactStatObjs.insert(TStringStatObjMap::value_type(jointName, pStatObjIntact));
						}
					}
				}
			}
		}
		m_initialiseOnChangeState = false;
	}

  bool change = CVehiclePartBase::ChangeState(state, flags);
  
  if (state == eVGS_Default && !change)
  {
    // need to restore state if one of the children is in higher state    
    EVehiclePartState maxState = GetMaxState();

    if (maxState > m_state)
      change = true;
  }

  if (!change)
    return false;

	if (state == eVGS_Destroyed)
	{		
		if (m_pCharInstance && m_pCharInstanceDestroyed)
		{
			ISkeletonPose* pSkeletonPose = m_pCharInstance->GetISkeletonPose();
			ISkeletonPose* pSkeletonDestroyed = m_pCharInstanceDestroyed->GetISkeletonPose();

			if (pSkeletonPose && pSkeletonDestroyed)
			{
				for (uint32 i = 0; i < pSkeletonPose->GetJointCount(); i++)
				{
					if (IStatObj* pStatObjIntact = pSkeletonPose->GetStatObjOnJoint(i))
					{
						const char* jointName = pSkeletonPose->GetJointNameByID(i);            
            IStatObj* pStatObj = GetDestroyedGeometry(jointName);

            // sets new StatObj to joint, if null, removes it. 
            // object whose name includes "proxy" are not removed.
            if (pStatObj || !strstr(jointName, "proxy")) 
            {
              GetEntity()->SetStatObj(pStatObj, GetCGASlot(i), true);

				//  if (m_intactStatObjs.find(CONST_TEMP_STRING(jointName)) == m_intactStatObjs.end())
				//	  m_intactStatObjs.insert(TStringStatObjMap::value_type(jointName, pStatObjIntact));

              if (IsDebugParts())
                CryLog("swapping StatObj on joint %i (%s) -> %s", i, jointName, pStatObj ? pStatObj->GetGeoName() : "<NULL>");						
            }            
					}
				}
        
        FlagSkeleton(pSkeletonPose);
        
        for (TStringVehiclePartMap::iterator ite = m_jointParts.begin(); ite != m_jointParts.end(); ++ite)
        {
          IVehiclePart* pPart = ite->second;
          pPart->ChangeState(state, flags|eVPSF_Physicalize);           
        }

				CryCharAnimationParams animParams;
				animParams.m_nFlags |= CA_LOOP_ANIMATION;
				//pSkeleton->SetRedirectToLayer0(1);
				//pSkeleton->StartAnimation("Default",0,  0,0, animParams);  // [MR: commented out on Ivos request]

				const char* destroyedMaterial = m_pVehicle->GetDestroyedMaterial();
				if(destroyedMaterial && destroyedMaterial[0])
				{
					IMaterial* pMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial(destroyedMaterial, false);
					if (pMaterial)
						GetEntity()->SetMaterial(pMaterial);    
					else
						CryLog("Vehicle <%s>: Material %s (for destroyed mesh) not found", GetEntity()->GetName(), destroyedMaterial);
				}
			}
		}
	}
	else if (state == eVGS_Default)
	{	
		if (m_pCharInstance && m_pCharInstanceDestroyed)
		{
			ISkeletonPose* pSkeletonPose = m_pCharInstance->GetISkeletonPose();
			{
				for (TStringStatObjMap::iterator ite = m_intactStatObjs.begin(); ite != m_intactStatObjs.end(); ++ite)
				{
					const string& jointName = ite->first;
					IStatObj* pStatObj = ite->second;

					int16 jointId = pSkeletonPose->GetJointIDByName(jointName.c_str());
					if (jointId > -1)
					{	
            // if compound StatObj (from deformation), use first SubObj for restoring
						if (pStatObj != NULL)
						{
							if (!pStatObj->GetRenderMesh() && pStatObj->GetSubObjectCount() > 0)
							{
								pStatObj = pStatObj->GetSubObject(0)->pStatObj;
							}

							GetEntity()->SetStatObj(pStatObj, GetCGASlot(jointId), true);

							if (IsDebugParts())
								CryLog("restoring StatObj on joint %i (%s) -> %s", jointId, jointName.c_str(), pStatObj ? pStatObj->GetGeoName() : "<NULL>");
						}

						TStringVehiclePartMap::iterator it = m_jointParts.find(jointName);
						if (it != m_jointParts.end())
						{
							it->second->ChangeState(state, flags&~eVPSF_Physicalize|eVPSF_Force);
						}
					}          
				}  
        flags |= eVPSF_Physicalize;   
			}
		}
	}  

  m_state = state;

  // physicalize after all parts have been restored
  if (flags & eVPSF_Physicalize && GetEntity()->GetPhysics())
  {     
    Physicalize();
    for (TStringVehiclePartMap::iterator it=m_jointParts.begin(); it!=m_jointParts.end(); ++it)
    { 
      it->second->Physicalize();
    }
  }

  return true;
}

//------------------------------------------------------------------------
bool CVehiclePartAnimated::ChangeChildState(CVehiclePartAnimatedJoint* pPart, EVehiclePartState state, int flags)
{
  // only handle range between intact and destroyed
  if (state > pPart->GetState() && (state < eVGS_Damaged1 || state >= eVGS_Destroyed)) 
    return false;

  if (state < pPart->GetState() && pPart->GetState() >= eVGS_Destroyed)
    return false;

  int jointId = pPart->GetJointId();
  
  if (pPart->GetState() == eVGS_Default)
  { 
    ISkeletonPose* pSkeletonPose = m_pCharInstance ? m_pCharInstance->GetISkeletonPose() : NULL;
    
    if (IStatObj* pStatObjIntact = pSkeletonPose ? pSkeletonPose->GetStatObjOnJoint(jointId) : NULL)
    {
      const char* jointName = pSkeletonPose->GetJointNameByID(jointId);
      
      if (m_intactStatObjs.find(CONST_TEMP_STRING(jointName)) == m_intactStatObjs.end())
        m_intactStatObjs.insert(TStringStatObjMap::value_type(jointName, pStatObjIntact));      
    }
  }

  if (m_jointParts.find(pPart->GetName()) == m_jointParts.end())
    m_jointParts.insert(TStringVehiclePartMap::value_type(pPart->GetName(), pPart));
    
  IStatObj* pStatObj = GetGeometryForState(pPart, state);

  if (pStatObj)  
    GetEntity()->SetStatObj(pStatObj, GetCGASlot(jointId), flags & eVPSF_Physicalize);   

  return true;
}

//------------------------------------------------------------------------
IStatObj* CVehiclePartAnimated::GetSubGeometry(CVehiclePartBase* pPart, EVehiclePartState state, Matrix34& localTM, bool removeFromParent)
{
	ICharacterInstance* pCharInstance = 0;
  string jointName;
  pPart->GetGeometryName(state, jointName);
  int jointId = -1;
	
  if (state == eVGS_Destroyed)
  { 
    pCharInstance = m_pCharInstanceDestroyed;    

    if (pCharInstance)
      jointId = pCharInstance->GetISkeletonPose()->GetJointIDByName(jointName.c_str());
  }
	else
  {
    // lookup first on intact, then on destroyed model        
    pCharInstance = m_pCharInstance;
      
    if (pCharInstance)
      jointId = pCharInstance->GetISkeletonPose()->GetJointIDByName(jointName.c_str());

    if (jointId == -1)
    {
      pCharInstance = m_pCharInstanceDestroyed;

      if (pCharInstance)
        jointId = pCharInstance->GetISkeletonPose()->GetJointIDByName(jointName.c_str());    
    } 
  }
	
	if (jointId != -1)
	{
		if (IStatObj* pStatObj = pCharInstance->GetISkeletonPose()->GetStatObjOnJoint(jointId))
		{			
			localTM = Matrix34(pCharInstance->GetISkeletonPose()->GetAbsJointByID(jointId));

			if (removeFromParent && pCharInstance != m_pCharInstanceDestroyed)
			{
				if (m_intactStatObjs.find(pPart->GetName()) == m_intactStatObjs.end())
				{
					//pStatObj->AddRef();
					m_intactStatObjs.insert(TStringStatObjMap::value_type(pPart->GetName(), pStatObj));
				}
        
        GetEntity()->SetStatObj(NULL, GetCGASlot(jointId),true);
			}

			if (pPart)
				m_jointParts.insert(TStringVehiclePartMap::value_type(pPart->GetName(), pPart));
			
			return pStatObj;
		}
    else if (state == eVGS_Default && pPart->GetState() != eVGS_Default)
    {      
      TStringStatObjMap::const_iterator it = m_intactStatObjs.find(pPart->GetName());
      if (it != m_intactStatObjs.end())
        return it->second;
    }

	}
	
	return NULL;
}

//------------------------------------------------------------------------
void CVehiclePartAnimated::Physicalize()
{
	if (m_isPhysicalized && GetEntity()->GetPhysics())
	{
    if (m_slot != -1)
      GetEntity()->UnphysicalizeSlot(m_slot);

    SEntityPhysicalizeParams params;
    params.mass = m_mass;
    params.density = m_density;
		params.nSlot = m_slot;
		GetEntity()->PhysicalizeSlot(m_slot, params); // always returns -1 for chars

    if (m_pCharInstance)
    { 
      FlagSkeleton(m_pCharInstance->GetISkeletonPose());
    }

    m_pVehicle->RequestPhysicalization(this, false);
	}
 
	GetEntity()->SetSlotFlags(m_slot, GetEntity()->GetSlotFlags(m_slot)|ENTITY_SLOT_IGNORE_PHYSICS);
}

//------------------------------------------------------------------------
void CVehiclePartAnimated::FlagSkeleton(ISkeletonPose* pSkeletonPose)
{
  if (!pSkeletonPose)
    return;

  IPhysicalEntity* pPhysics = GetEntity()->GetPhysics();
  if (!pPhysics)
    return;
  
  string name;
  int idWater = pSkeletonPose->GetJointIDByName("proxy_water");
  uint32 buoyancyParts = (idWater != -1) ? 1 : 0;
        
  uint32 jointCount = pSkeletonPose->GetJointCount();
  for (uint32 i=0; i<jointCount; ++i)
  { 
    int physId = pSkeletonPose->GetPhysIdOnJoint(i);

    if (physId >= 0)
    {
      CheckColltypeHeavy(physId);
      name = pSkeletonPose->GetJointNameByID(i); 
            
      // when water proxy available, remove float from all others
      // if no water proxy, we leave only "proxy" parts floating
      if (idWater != -1)
      {
        if (i == idWater)
        {
          SetFlags(physId, geom_collides, false);
          SetFlagsCollider(physId, 0);
        }
        else 
          SetFlags(physId, geom_floats, false);
      }
      else
      {
        if (name.find("proxy") != string::npos)        
          ++buoyancyParts;
        else
          SetFlags(physId, geom_floats, false);
      }          
       
      // all objects which have a corresponding *_proxy on the skeleton
      // are set to ray collision only    
      if (name.find("_proxy") == string::npos)
      {      
        name.append("_proxy");
        int proxyId = pSkeletonPose->GetJointIDByName(name.c_str());

        if (proxyId != -1)
        { 
          // remove ray collision from hull proxy(s)
          SetFlags(pSkeletonPose->GetPhysIdOnJoint(proxyId), geom_colltype_ray|geom_colltype13, false);

          // get StatObj from main part, to connect proxies foreignData with it
          IStatObj* pStatObj = pSkeletonPose->GetStatObjOnJoint(i);

          if (pStatObj)
          {
            pe_params_part params;
            params.partid = proxyId;
            if (pPhysics->GetParams(&params))
            {
              if (params.pPhysGeom && params.pPhysGeom->pGeom)
                params.pPhysGeom->pGeom->SetForeignData(pStatObj, 0);
            }                    
          }            

          for (int p=2;p<6;++p)
          {
            // check additional proxies, by naming convention _02, .. _05
            char buf[64];
            _snprintf(buf, sizeof(buf), "%s_%02i", name.c_str(), p);
            buf[sizeof(buf)-1] = '\0';
                        
            proxyId = pSkeletonPose->GetJointIDByName(buf);            
            if (proxyId == -1)
              break;

            int proxyPhysId = pSkeletonPose->GetPhysIdOnJoint(proxyId);
            if (proxyPhysId == -1)
              continue;
            
            SetFlags(proxyPhysId, geom_colltype_ray|geom_colltype13, false);

            // connect proxies to main StatObj (needed for bullet tests, decals)
            if (pStatObj)
            {
              pe_params_part params;
              params.partid = proxyPhysId;
              if (pPhysics->GetParams(&params))
              {
                if (params.pPhysGeom && params.pPhysGeom->pGeom)
                  params.pPhysGeom->pGeom->SetForeignData(pStatObj, 0);
              }                    
            }            
          }

          // set ray-collision only on the part
          SetFlags(physId, geom_collides|geom_floats, false);
          SetFlags(physId, geom_colltype_ray|geom_colltype13, true);
          SetFlagsCollider(physId, 0); 
        }
      }
    }
  }

  if (buoyancyParts == 0)
  { 
    // as fallback, use part with largest volume for buoyancy
    int partId = -1;
    float maxV = 0.f;

    pe_status_nparts nparts;
    int numParts = pPhysics->GetStatus(&nparts);
        
    for (int i=0; i<numParts; ++i)
    {
      pe_params_part params;
      params.ipart = i;
      if (pPhysics->GetParams(&params))
      {
        float v = (params.pPhysGeomProxy) ? params.pPhysGeomProxy->V : params.pPhysGeom->V;
        if (v > maxV)
        {
          partId = params.partid;
          maxV = v;
        }
      }
    }

    if (partId != -1)    
      SetFlags(partId, geom_floats, true);    
    else
      GameWarning("[CVehiclePartAnimated]: <%s> has no buoyancy parts!",GetEntity()->GetName());
  }
  
  int jointId, physId;
  if ((jointId = pSkeletonPose->GetJointIDByName("proxy_skirt")) != -1)
  {
    if ((physId = pSkeletonPose->GetPhysIdOnJoint(jointId)) != -1)
    {
      SetFlags(physId, geom_collides|geom_floats, false);
      SetFlags(physId, geom_colltype_ray|geom_colltype13|geom_colltype_player|geom_colltype_foliage, true);
      SetFlagsCollider(physId, 0); 
    }
  }

  // remove collision flags from all _proxy geoms by debug cvar
  // useful for seeing through, testing ray proxies etc  
  if (VehicleCVars().v_disable_hull > 0)
  {
    for (uint32 i=0; i<pSkeletonPose->GetJointCount(); ++i)
    {      
      if (strstr(pSkeletonPose->GetJointNameByID(i), "_proxy"))
      {
        SetFlags(pSkeletonPose->GetPhysIdOnJoint(i), geom_collides|geom_floats, false);
        SetFlagsCollider(pSkeletonPose->GetPhysIdOnJoint(i), 0);
      }
    }
  }  
}

//------------------------------------------------------------------------
void CVehiclePartAnimated::RotationChanged(CVehiclePartAnimatedJoint* pJoint)
{
	// craig: cannot drop these changes if the vehicle is on a server
	bool cull = false;
	if (gEnv->bClient)
		cull = m_pVehicle->GetGameObject()->IsProbablyDistant();

	if (m_iRotChangedFrameId != gEnv->pRenderer->GetFrameID() && !cull)
  {    
    // force skeleton update for this frame (this is intended, contact Ivo)
    if (m_pCharInstance)    
      m_pCharInstance->GetISkeletonPose()->SetForceSkeletonUpdate(1);

    m_iRotChangedFrameId = gEnv->pRenderer->GetFrameID();
    
    // require update for the next frame to reset skeleton update
    m_pVehicle->NeedsUpdate();
  }
}


//------------------------------------------------------------------------
void CVehiclePartAnimated::Update(const float frameTime)
{
	CVehiclePartBase::Update(frameTime);

  if (m_iRotChangedFrameId)
  {   
    if (m_iRotChangedFrameId < gEnv->pRenderer->GetFrameID())
    {
      if (m_pCharInstance)
        m_pCharInstance->GetISkeletonPose()->SetForceSkeletonUpdate(0);
      
      m_iRotChangedFrameId = 0;
    }
    else
      m_pVehicle->NeedsUpdate();
  }
	else if(m_serializeForceRotationUpdate)
	{
		m_serializeForceRotationUpdate = false;
		if (m_pCharInstance)    
			m_pCharInstance->GetISkeletonPose()->SetForceSkeletonUpdate(1);
		m_iRotChangedFrameId = gEnv->pRenderer->GetFrameID();
		m_pVehicle->NeedsUpdate();
	}
}

//------------------------------------------------------------------------
void CVehiclePartAnimated::SetDrivingProxy(bool bDrive)
{	
  IVehicleMovement* pMovement = m_pVehicle->GetMovement();
  if (!(pMovement && pMovement->UseDrivingProxy()))
    return;
	
	if (0 == m_hullMatId[bDrive]) // 0 means, nothin to do
		return;
  
  if (!m_pCharInstance)
    return;

	ISkeletonPose* pSkeletonPose = m_pCharInstance->GetISkeletonPose();    
  if (!pSkeletonPose)
    return;

  IPhysicalEntity* pPhysics = m_pVehicle->GetEntity()->GetPhysics();
  if (!pPhysics)
    return;

	int id = pSkeletonPose->GetJointIDByName("hull_proxy"); 

	if (id < 0)
	{
		m_hullMatId[0] = m_hullMatId[1] = 0;      
		return;
	}

  int partid = pSkeletonPose->GetPhysIdOnJoint(id);

  if (partid == -1)
    return;

	pe_params_part params;
	params.partid = partid;
	params.ipart = -1;
	if (!pPhysics->GetParams(&params) || !params.nMats)
		return; 

	phys_geometry* pGeom = params.pPhysGeom;
	if (pGeom && pGeom->surface_idx < pGeom->nMats)
	{ 
    ISurfaceTypeManager* pSurfaceMan = gEnv->p3DEngine->GetMaterialManager()->GetSurfaceTypeManager();

		// initialize once
		if (m_hullMatId[0] < 0)
		{ 
      int idDriving = 0;				
      int idOrig = pGeom->pMatMapping[pGeom->surface_idx];
      const char* matOrig = pSurfaceMan->GetSurfaceType(idOrig)->GetName();
      
      if (strstr(matOrig, "mat_metal"))
        idDriving = pSurfaceMan->GetSurfaceTypeByName("mat_metal_nofric")->GetId();
      else
      {
        string mat(matOrig);
        mat.append("_nofric");

        idDriving = pSurfaceMan->GetSurfaceTypeByName(mat.c_str(), NULL, false)->GetId();
      }
			
      //if (pDebug->GetIVal())
			  //CryLog("%s looking up driving surface replacement for %s (id %i) -> got id %i", m_pVehicle->GetEntity()->GetName(), matOrig, idOrig, idDriving);

			if (idDriving>0)
			{ 
				// store old and new id
				m_hullMatId[0] = idOrig; 
				m_hullMatId[1] = idDriving;

        /*if (pDebug->GetIVal())
        {
          const char* matDriving = pSurfaceMan->GetSurfaceType(idDriving)->GetName();
          CryLog("%s storing hull matId for swapping: %i (%s) -> %i (%s)", m_pVehicle->GetEntity()->GetName(), m_hullMatId[0], matOrig, m_hullMatId[1], matDriving);
        }*/
			}
			else 
				m_hullMatId[0] = m_hullMatId[1] = 0;
		}

		// only swap if materials available
		if (m_hullMatId[bDrive] > 0)
		{ 
			if (VehicleCVars().v_debugdraw == eVDB_Parts)
				CryLog("%s swapping hull proxy from %i (%s) to matId %i (%s)", m_pVehicle->GetEntity()->GetName(), m_hullMatId[bDrive^1], pSurfaceMan->GetSurfaceType(m_hullMatId[bDrive^1])->GetName(), m_hullMatId[bDrive], pSurfaceMan->GetSurfaceType(m_hullMatId[bDrive])->GetName());

			for (int n=0; n<pGeom->nMats; ++n)        
				pGeom->pMatMapping[n] = m_hullMatId[bDrive];
		}
	}	
}

//------------------------------------------------------------------------
void CVehiclePartAnimated::Serialize(TSerialize ser, EEntityAspects aspects)
{
	CVehiclePartBase::Serialize(ser, aspects);
}

//------------------------------------------------------------------------
void CVehiclePartAnimated::PostSerialize() 
{
  if (m_iRotChangedFrameId)
  {
    // if necessary, make sure skeleton gets updated in 1st frame  
		m_iRotChangedFrameId = 0;
		RotationChanged(0);
  }
	else if(gEnv->pSystem->IsSerializingFile() == 2 && m_pVehicle->IsDestroyed())
		m_serializeForceRotationUpdate = true;
}


//------------------------------------------------------------------------
void CVehiclePartAnimated::ProcessAnimatedJointRotations()
{
	for (TAnimatedJointList::iterator ite = m_rotatingAnimatedJoints.begin(); ite != m_rotatingAnimatedJoints.end(); ++ite)
	{
		CVehiclePartAnimatedJoint* pAnimatedJoint = *ite;    
    CRY_ASSERT(pAnimatedJoint);
    
    pAnimatedJoint->ApplyRotations();    
	}  
}

//------------------------------------------------------------------------
void CVehiclePartAnimated::RegisterRotatingAnimatedJoint(CVehiclePartAnimatedJoint* pAnimatedJoint)
{ 
	if (m_rotatingAnimatedJoints.size() == 0)
	{
		if (m_pCharInstance)
		{
			ISkeletonPose* pSkeletonPose = m_pCharInstance->GetISkeletonPose();
			CRY_ASSERT(pSkeletonPose);
      
			//CryCharAnimationParams animParams;
			//animParams.m_nFlags |= CA_LOOP_ANIMATION;
			//pSkeleton->StartAnimation("Default",0, 0,0,  animParams); // [MR: commented out on Ivos request]

			pSkeletonPose->SetPostProcessCallback(	(int (*)(ICharacterInstance*,void*)) CVehiclePartAnimated_ProcessRotations, (void*)this);
		}
	}

  stl::push_back_unique(m_rotatingAnimatedJoints, pAnimatedJoint); 
}

//------------------------------------------------------------------------
int CVehiclePartAnimated_ProcessRotations(ICharacterInstance* pCharInstance, CVehiclePartAnimated* pPartAnimated)
{
	pPartAnimated->ProcessAnimatedJointRotations();
	return 0;
}




void DumpSkeleton(ISkeletonPose* pSkeletonPose)
{
  if (!pSkeletonPose)
    return;

  for (int i=0; i<pSkeletonPose->GetJointCount(); ++i)
  {
    const char* name = pSkeletonPose->GetJointNameByID(i);
    int parentid = pSkeletonPose->GetParentIDByID(i);
    
    if (parentid != -1)
      CryLog("joint %i: %s (parent %i)", i, name, parentid);
    else
      CryLog("joint %i: %s", i, name);
  }
}

//------------------------------------------------------------------------
void CVehiclePartAnimated::Dump()
{  
  if (m_pCharInstance)  
  {
    CryLog("<%s> pCharInstance:", m_name.c_str());
    DumpSkeleton(m_pCharInstance->GetISkeletonPose());
  }

  if (m_pCharInstanceDestroyed)
  {
    CryLog("<%s> pCharInstanceDestroyed:", m_name.c_str());
    DumpSkeleton(m_pCharInstanceDestroyed->GetISkeletonPose());
  }
}

//------------------------------------------------------------------------
int CVehiclePartAnimated::GetNextFreeLayer()
{
	if (ICharacterInstance* pCharInstance = GetEntity()->GetCharacter(m_slot))  
	{
		ISkeletonAnim* pSkeletonAnim = pCharInstance->GetISkeletonAnim();
		CRY_ASSERT(pSkeletonAnim);

		for (int i=1; i<0x10; i++)
		{
			if (pSkeletonAnim->GetNumAnimsInFIFO(i) == 0)
				return i;
		}
	}

	return -1;
}

void CVehiclePartAnimated::GetMemoryStatistics(ICrySizer * s)
{
	s->Add(*this);
	GetBaseMemoryStatistics(s);
	s->Add(m_filename);
	s->Add(m_filenameDestroyed);
	s->AddContainer(m_intactStatObjs);
	for (TStringStatObjMap::iterator iter = m_intactStatObjs.begin(); iter != m_intactStatObjs.end(); ++iter)
		s->Add(iter->first);
	s->AddContainer(m_jointParts);
	for (TStringVehiclePartMap::iterator iter = m_jointParts.begin(); iter != m_jointParts.end(); ++iter)
		s->Add(iter->first);
	s->AddContainer(m_rotatingAnimatedJoints);
}

DEFINE_VEHICLEOBJECT(CVehiclePartAnimated);
