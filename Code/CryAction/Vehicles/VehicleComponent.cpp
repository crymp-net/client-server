/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a standard class for a vehicle component

-------------------------------------------------------------------------
History:
- 12:10:2005: Created by Mathieu Pinard

*************************************************************************/
#include "StdAfx.h"

#include "GameObjects/GameObject.h"
#include "IActorSystem.h"
#include "IVehicleSystem.h"
#include "IViewSystem.h"
#include "CryAction.h"
#include "IGameObject.h"
#include "IAgent.h"

#include "Vehicle.h"
#include "VehicleComponent.h"
#include "VehicleDamageBehaviorDetachPart.h"
#include "VehiclePartAnimatedJoint.h"

#include "IRenderer.h"
#include "IRenderAuxGeom.h"
#include "IGameRulesSystem.h"

namespace
{
  ILINE bool DebugDamage() { return VehicleCVars().v_debugdraw == eVDB_Damage; }
}

//------------------------------------------------------------------------
CVehicleComponent::CVehicleComponent()
: m_pVehicle(NULL),
  m_damage(0.f),
  m_damageMax(0.f),
  m_hullAffection(0.f),
  m_pHudMessage(NULL),
  m_isOnlyDamageableByPlayer(false),
  m_useDamageLevels(true),
  m_bHull(false),
  m_lastHitRadius(0.f),
  m_lastHitType(0),
  m_lastHitShooterId(0),
	m_majorComponent(true),
	m_proportionOfVehicleHealth(0.f)
{
  m_bounds.Reset();  
  m_lastHitLocalPos.zero();
}

//------------------------------------------------------------------------
CVehicleComponent::~CVehicleComponent()
{
	for (TVehicleDamageBehaviorVector::iterator ite = m_damageBehaviors.begin(); ite != m_damageBehaviors.end(); ++ite)
		ite->second->Release();
}

//------------------------------------------------------------------------
bool CVehicleComponent::Init(IVehicle* pVehicle, const CVehicleParams& paramsTable)
{
	m_pVehicle = (CVehicle*) pVehicle;

	m_name = paramsTable.getAttr("name");
	if (m_name.empty())
		return false;

  m_bHull = m_name == "hull" || m_name == "Hull";

	// reading the bounding box
	m_bounds.Reset();  
  m_lastHitType = 0;
	
	Vec3 bound;
	if (paramsTable.getAttr("minBound", bound))
		m_bounds.Add(bound);

	if (paramsTable.getAttr("maxBound", bound))
		m_bounds.Add(bound);

	Vec3 position;
	Vec3 size;
	if (paramsTable.getAttr("position", position) && paramsTable.getAttr("size", size))
	{
		bool hasOldBoundInfo = !m_bounds.IsReset();
		if ( hasOldBoundInfo )
		{
			GameWarning("Component %s for vehicle %s has maxBound and/or minBound together with position and size properties. Using position and size.", m_name.c_str(), m_pVehicle->GetEntity()->GetName());
		}
		Vec3 halfSize = size * 0.5f;
		m_bounds.min = position - halfSize;
		m_bounds.max = position + halfSize;
	}

	if (m_bounds.IsReset())
  {
		m_bounds.min.zero();
    m_bounds.max.zero();
  }
  else if (!(m_bounds.min.x<=m_bounds.max.x && m_bounds.min.y<=m_bounds.max.y && m_bounds.min.z<=m_bounds.max.z))
  {
    GameWarning("Invalid bounding box read for %s, component %s", m_pVehicle->GetEntity()->GetName(), m_name.c_str());
    m_bounds.min.zero();
    m_bounds.max.zero();
  }

	if (!paramsTable.getAttr("useBoundsFromParts", m_useBoundsFromParts))
	{
		m_useBoundsFromParts = false;
	}

  if (!paramsTable.getAttr("useDamageLevels", m_useDamageLevels)) 
    m_useDamageLevels = true;

	m_damage = 0.0f;
  
	if (!paramsTable.getAttr("damageMax", m_damageMax))
		m_damageMax = 0.0f;

	paramsTable.getAttr("major", m_majorComponent);

  if (!paramsTable.getAttr("hullAffection", m_hullAffection))
    m_hullAffection = 1.0f;

	if (CVehicleParams damageBehaviorsTable = paramsTable.findChild("DamageBehaviors"))
	{
		int i = 0;
		int c = damageBehaviorsTable.getChildCount();
		m_damageBehaviors.reserve(c);

		for (; i < c; i++)
		{
			if (CVehicleParams damageBehaviorTable = damageBehaviorsTable.getChild(i))
			{
				SDamageBehaviorParams behaviorParams;

				if (!damageBehaviorTable.getAttr("damageRatioMin", behaviorParams.damageRatioMin))
					behaviorParams.damageRatioMin = 1.0f;

				if (!damageBehaviorTable.getAttr("damageRatioMax", behaviorParams.damageRatioMax))
					behaviorParams.damageRatioMax = 0.0f;

				if (!damageBehaviorTable.getAttr("ignoreVehicleDestruction", behaviorParams.ignoreOnVehicleDestroyed))
					behaviorParams.ignoreOnVehicleDestroyed = false;
				else
					behaviorParams.ignoreOnVehicleDestroyed = behaviorParams.ignoreOnVehicleDestroyed;

				if (damageBehaviorTable.haveAttr("class"))
				{
					IVehicleSystem* pVehicleSystem = CCryAction::GetCryAction()->GetIVehicleSystem();

					if (IVehicleDamageBehavior* pDamageBehavior
						= pVehicleSystem->CreateVehicleDamageBehavior(damageBehaviorTable.getAttr("class")))
					{
						if (pDamageBehavior->Init(pVehicle, damageBehaviorTable))
							m_damageBehaviors.push_back(TVehicleDamageBehaviorPair(behaviorParams, pDamageBehavior));
						else
							pDamageBehavior->Release();
					}
				}
			}
		}
	}

	m_pHudMessage = NULL;

	CVehicleDamages::ParseDamageMultipliers(m_damageMultipliers, paramsTable);

	if (!paramsTable.getAttr("isOnlyDamageableByPlayer", m_isOnlyDamageableByPlayer))
		m_isOnlyDamageableByPlayer = false;

	m_lastHitLocalPos.zero();
	m_lastHitRadius = 0.0f;
	m_lastHitShooterId = 0;
  
	return true;
}

//------------------------------------------------------------------------
void CVehicleComponent::Reset()
{
	m_damage = 0.0f;

	for (TVehicleDamageBehaviorVector::iterator ite = m_damageBehaviors.begin(); ite != m_damageBehaviors.end(); ++ite)
	{
		IVehicleDamageBehavior* pVehicleDamageBehavior = ite->second;
		pVehicleDamageBehavior->Reset();
	}

	if (m_pHudMessage)
	{
		m_pVehicle->UnregisterHUDElement(m_pHudMessage);
		m_pHudMessage->Release();
		m_pHudMessage = NULL;
	}

	m_lastHitLocalPos.zero();
	m_lastHitRadius = 0.0f;
	m_lastHitShooterId = 0;
  m_lastHitType = 0;

	m_proportionOfVehicleHealth = 0.0f;
}

//------------------------------------------------------------------------
unsigned int CVehicleComponent::GetPartCount() const
{
	return m_parts.size();	
}

//------------------------------------------------------------------------
IVehiclePart* CVehicleComponent::GetPart(unsigned int index) const
{
	if (index <= m_parts.size())
	{
		return m_parts[index];
	}

	return NULL;
}

//------------------------------------------------------------------------
const AABB& CVehicleComponent::GetBounds()
{
	if (m_useBoundsFromParts)
	{
		m_bounds.Reset();

		for (TVehiclePartVector::iterator ite = m_parts.begin(); ite != m_parts.end(); ++ite)
		{
			IVehiclePart* pPart = *ite;
			m_bounds.Add(pPart->GetLocalBounds());
		}

    if (!m_bounds.IsReset())
    {
      // add a tolerance - bbox checks may fail, depending on part geometry
      Vec3 delta(0.05f, 0.05f, 0.05f);
      m_bounds.max += delta;
      m_bounds.min -= delta;
    }    
	}
	else if (m_bounds.IsEmpty())
	{
		m_pVehicle->GetEntity()->GetLocalBounds(m_bounds);
	}

	return m_bounds;
}

//------------------------------------------------------------------------
void CVehicleComponent::OnHit(EntityId targetId, EntityId shooterId, float damage, Vec3 position, float radius, const char* damageType, bool explosion, const TVehicleComponentVector* pAffectedComponents)
{
  if (m_damageMax <= 0.0f)
		return;

  if (DebugDamage())
    CryLog("=== Hit Component <%s>, base damage %.1f ===", GetComponentName(), damage);

  Vec3 localPos = m_pVehicle->GetEntity()->GetWorldTM().GetInverted() * position;
  
  // direct impact?
  bool impact = !explosion || targetId == m_pVehicle->GetEntityId();
  
	float processedDamage = ProcessDamage(damage, localPos, radius, damageType, explosion, impact, pAffectedComponents);

  if (DebugDamage())
    CryLog("final damage: %.1f", processedDamage);	

  if (abs(processedDamage) < 0.001f)
    return;
  
  // repairing
  if (processedDamage <= 0.0f && (m_pVehicle->IsDestroyed() || m_damage == 0.f))
    return;

	// Repairing: special check here for vehicle damage caused by being submerged in water - otherwise the player
	//	can continually repair the vehicle while it is taking damage in water (gaining lots of PP in MP).
	const SVehicleStatus& status = m_pVehicle->GetStatus();
	const SVehicleDamageParams& damageParams = m_pVehicle->GetDamageParams();
	if (processedDamage <= 0.0f && status.submergedRatio > damageParams.submergedRatioMax)
	{ 
		return;
	}

	if (m_isOnlyDamageableByPlayer && processedDamage > 0.f)
	{
		if (IActor* pActor = CCryAction::GetCryAction()->GetIActorSystem()->GetActor(shooterId))
		{
			if (!pActor->IsPlayer())
				return;
		}
	}

	m_lastHitLocalPos = localPos;
	m_lastHitRadius = max(0.1f, min(3.0f, radius));
	m_lastHitShooterId = shooterId;
  
  IGameRules* pGameRules = CCryAction::GetCryAction()->GetIGameRulesSystem()->GetCurrentGameRules();
  m_lastHitType = (pGameRules) ? pGameRules->GetHitTypeId(damageType) : 0;
	
	float currentDamageRatio = m_damage / m_damageMax;
	int currentDamageLevel = int(min(currentDamageRatio, 1.0f) / 0.25f);
	
  m_damage = max(0.0f, m_damage + processedDamage);

	float newDamageRatio = m_damage / m_damageMax;
	int newDamageLevel = int(min(newDamageRatio, 1.0f) / 0.25f);

	if (gEnv->bServer)
		CHANGED_NETWORK_STATE(m_pVehicle, CVehicle::ASPECT_COMPONENT_DAMAGE);

	if (m_useDamageLevels && currentDamageLevel == newDamageLevel)
		return;

	if (VehicleCVars().v_debugdraw && !m_pHudMessage && newDamageLevel > 0 && newDamageLevel >= currentDamageLevel)
	{
		m_pHudMessage = new CVehicleHUDElement;
		
		m_pHudMessage->SetBlinkingSpeed(0.0f);
		m_pHudMessage->SetMessage(m_name + "  damaged");
		
		m_pVehicle->RegisterHUDElement(m_pHudMessage);
	}

	if (m_pHudMessage)
	{
    if (newDamageLevel == 0)
    {
      m_pVehicle->UnregisterHUDElement(m_pHudMessage);
      m_pHudMessage->Release();
      m_pHudMessage = 0;
    }
		else if (newDamageLevel == 1 || newDamageLevel == 2)
		{
			float color[3] = {1.0f, 1.0f, 0.0f};
			m_pHudMessage->SetColor(color);
		}
		else if (newDamageLevel == 3)
		{
			float color[3] = {1.0f, 0.7f, 0.0f};
			m_pHudMessage->SetColor(color);
		}
		else if (newDamageLevel == 4)
		{
			float color[3] = {1.0f, 0.0f, 0.0f};
			m_pHudMessage->SetColor(color);
		}
	}

	if (VehicleCVars().v_debugdraw)
		CryLog("%s - damaged <%s> with %.1f damages (total: %.1f)", m_pVehicle->GetEntity()->GetName(), m_name.c_str(), processedDamage, m_damage);

	BroadcastDamage(processedDamage, shooterId);
}

//------------------------------------------------------------------------
void CVehicleComponent::OnVehicleDestroyed()
{
	for (TVehicleDamageBehaviorVector::iterator ite = m_damageBehaviors.begin(); ite != m_damageBehaviors.end(); ++ite)
	{
		const SDamageBehaviorParams& behaviorParams = ite->first;
		if (!behaviorParams.ignoreOnVehicleDestroyed)
		{
			IVehicleDamageBehavior* pVehicleDamageBehavior = ite->second;

			SVehicleDamageBehaviorEventParams eventParams;
			eventParams.componentDamageRatio = 1.0f;
			eventParams.localPos.zero();
			eventParams.hitValue = 1.0f;
			eventParams.radius = 0.0f;
			eventParams.shooterId=0;
			eventParams.pVehicleComponent = this;

			pVehicleDamageBehavior->OnDamageEvent(eVDBE_VehicleDestroyed, eventParams);
		}
	}

	// we also notify all the parts related to the component that the thing blew up
	IVehiclePart::SVehiclePartEvent partEvent;
	partEvent.type = IVehiclePart::eVPE_Damaged;

	for (TVehiclePartVector::iterator ite = m_parts.begin(); ite != m_parts.end(); ++ite)
	{
		IVehiclePart* pPart = *ite;
		pPart->OnEvent(partEvent);
	}
}

//------------------------------------------------------------------------
void CVehicleComponent::Update(float deltaTime)
{
  //FUNCTION_PROFILER( GetISystem(), PROFILE_ACTION );
}

//------------------------------------------------------------------------
void CVehicleComponent::DebugDraw()
{  
  if (VehicleCVars().v_draw_components == 1)
  {
    IRenderer* pRenderer = gEnv->pRenderer;
    IRenderAuxGeom* pRenderAux = pRenderer->GetIRenderAuxGeom();

    const Matrix34& worldTM = m_pVehicle->GetEntity()->GetWorldTM();
    const AABB& localBounds = GetBounds();

    static float drawColor[4] = {1,1,1,1};
    
    char pMessage[256];    
    _snprintf(pMessage, sizeof(pMessage), "%s - %5.2f (%3.2f)", m_name.c_str(), m_damage, m_damage/max(1.f, m_damageMax));
    pMessage[sizeof(pMessage)-1] = '\0';
            
    pRenderer->DrawLabelEx(worldTM * localBounds.GetCenter(), 1.0f, drawColor, true, true, pMessage);

    pRenderAux->DrawAABB(localBounds, worldTM, false, RGBA8(255, 0, 0, 0), eBBD_Faceted);
  }

  //if (m_lastHitRadius > 0.0f)
  //{
  //	IRenderer* pRenderer = gEnv->pRenderer;
  //	IRenderAuxGeom* pRenderAux = pRenderer->GetIRenderAuxGeom();

  //	pRenderAux->DrawSphere(m_pVehicle->GetEntity()->GetWorldTM() * m_lastHitLocalPos, m_lastHitRadius, RGBA8(255, 0, 0, 0));
  //}
}

//------------------------------------------------------------------------
void CVehicleComponent::BroadcastDamage(float damage, EntityId shooterId)
{
	float currentDamageRatio = (m_damage-damage)/m_damageMax;
	float newDamageRatio = m_damage / m_damageMax;
	int currentDamageLevel = int(min(currentDamageRatio, 1.0f) / 0.25f);
	int newDamageLevel = int(min(newDamageRatio, 1.0f) / 0.25f);

	if (m_useDamageLevels && currentDamageLevel == newDamageLevel)
		return;

	for (TVehicleDamageBehaviorVector::iterator ite = m_damageBehaviors.begin(); ite != m_damageBehaviors.end(); ++ite)
	{
		const SDamageBehaviorParams& behaviorParams = ite->first;
		IVehicleDamageBehavior* pVehicleDamageBehavior = ite->second;

		if (newDamageRatio >= behaviorParams.damageRatioMin || damage < 0.f)
		{
			bool hasPassedSecondCheck = true;
			if (damage > 0.f && behaviorParams.damageRatioMax > 0.0f && newDamageRatio >= behaviorParams.damageRatioMax)
      {
        if (currentDamageRatio < behaviorParams.damageRatioMax)
        {
          SVehicleDamageBehaviorEventParams behaviorEventParams;
          pVehicleDamageBehavior->OnDamageEvent(eVDBE_MaxRatioExceeded, behaviorEventParams);
        }
        hasPassedSecondCheck = false;
      }

			if (hasPassedSecondCheck)
			{ 
        EVehicleDamageBehaviorEvent behaviorEvent = damage > 0.f ? eVDBE_Hit : eVDBE_Repair;

				SVehicleDamageBehaviorEventParams behaviorEventParams;
				behaviorEventParams.componentDamageRatio = newDamageRatio;
				behaviorEventParams.localPos = m_lastHitLocalPos;
				behaviorEventParams.hitValue = damage;
        behaviorEventParams.hitType = m_lastHitType;
				behaviorEventParams.radius = m_lastHitRadius;
				behaviorEventParams.randomness = 0.0f;
				behaviorEventParams.shooterId = shooterId;
				behaviorEventParams.pVehicleComponent = this;

				pVehicleDamageBehavior->OnDamageEvent(behaviorEvent, behaviorEventParams);
			}
		}
	}

  // always adhere to damage levels here
  if (currentDamageLevel == newDamageLevel)
    return;

	// component damaged, so we notify all the parts related to the component
	IVehiclePart::SVehiclePartEvent partEvent;
  partEvent.type = damage > 0.f ? IVehiclePart::eVPE_Damaged : IVehiclePart::eVPE_Repair;
	partEvent.fparam = newDamageRatio;  
  partEvent.pData = this;

	for (TVehiclePartVector::iterator ite = m_parts.begin(); ite != m_parts.end(); ++ite)
	{
		IVehiclePart* pPart = *ite;
		pPart->OnEvent(partEvent);
	}
}

//------------------------------------------------------------------------
void CVehicleComponent::Serialize(TSerialize ser, EEntityAspects aspects)
{
	if (ser.GetSerializationTarget() != eST_Network)
	{
		ser.Value("damage", m_damage);

		if (ser.IsReading())
		{
			float damageRatio = 1.0f;
			if(m_damageMax > 0.0f)
				damageRatio = m_damage / m_damageMax;
			int damageLevel = int(min(damageRatio, 1.0f) / 0.25f);

			if (!m_pHudMessage && damageLevel > 0)
			{
				m_pHudMessage = new CVehicleHUDElement;

				m_pHudMessage->SetBlinkingSpeed(0.0f);
				m_pHudMessage->SetMessage(m_name + "  damaged");

				m_pVehicle->RegisterHUDElement(m_pHudMessage);
			} 
			else if (m_pHudMessage && damageLevel == 0)
			{
				m_pVehicle->UnregisterHUDElement(m_pHudMessage);
				m_pHudMessage->Release();
        m_pHudMessage = 0;
			}

			if (m_pHudMessage)
			{
				if (damageLevel == 1)
				{
					float color[3] = {1.0f, 1.0f, 0.0f};
					m_pHudMessage->SetColor(color);
				}
				else if (damageLevel == 3)
				{
					float color[3] = {1.0f, 0.7f, 0.0f};
					m_pHudMessage->SetColor(color);
				}
				else if (damageLevel == 4)
				{
					float color[3] = {1.0f, 0.0f, 0.0f};
					m_pHudMessage->SetColor(color);
				}
			}
		}

		int i = 0;
		for (TVehicleDamageBehaviorVector::iterator ite = m_damageBehaviors.begin(); ite != m_damageBehaviors.end(); ++ite)
		{
			char pGroupName[64];
			IVehicleDamageBehavior* pVehicleDamageBehavior = ite->second;

			sprintf(pGroupName, "damageBehavior%d", ++i);

			ser.BeginGroup(pGroupName);
			pVehicleDamageBehavior->Serialize(ser, aspects);
			ser.EndGroup();
		}
	}
	// network
	else
	{
		if (aspects & CVehicle::ASPECT_COMPONENT_DAMAGE)
		{
			float olddamage=m_damage;
			ser.Value("damage", m_damage, 'dmg');
      ser.Value("hitType", m_lastHitType, 'hTyp');
			ser.Value("lastHitPos", m_lastHitLocalPos, 'vHPs');
			ser.Value("lastHitRadius", m_lastHitRadius, 'vHRd');

			if (ser.IsReading() && abs(m_damage-olddamage) > 0.001f)
      {        
				BroadcastDamage(m_damage-olddamage, 0);
      }
		}
	}
}

//------------------------------------------------------------------------
float CVehicleComponent::GetDamageRatio() const
{
  return (m_damageMax > 0.f) ? min(1.f, m_damage/m_damageMax) : 0.f;
}

//------------------------------------------------------------------------
void CVehicleComponent::SetDamageRatio(float ratio)
{
	if(m_damageMax > 0.f)
		m_damage = m_damageMax * min(1.0f, max(0.0f, ratio));
}

//------------------------------------------------------------------------
float CVehicleComponent::GetMaxDamage() const
{
	return m_damageMax;
}

//------------------------------------------------------------------------
void CVehicleComponent::AddPart(IVehiclePart* pPart) 
{
	m_parts.push_back(pPart);
}

//------------------------------------------------------------------------
float CVehicleComponent::ProcessDamage(float damage, const Vec3& localPos, float radius, const char* damageType, bool explosion, bool impact, const TVehicleComponentVector* pAffectedComponents)
{
  // determine hit ratio/splash damage
	
  bool splash = false;
  const AABB& localBounds = GetBounds();		
  bool inside = localBounds.IsContainPoint(localPos);
  
  if (explosion && radius > 0.0f)
	{
    if (!(impact && inside))
    {
      // no direct hit, apply splash damage
      splash = true;
		  
      // todo: more accurate ratio calculation
      float distanceSq = (localPos - localBounds.GetCenter()).GetLengthSquared();      		  
      float hitRatio = max(0.f, 1.f - distanceSq/sqr(radius));		          

      damage *= hitRatio;
      
      if (DebugDamage())
        CryLog("splash damage, ratio: %.2f", hitRatio);
    }
	}
 
  if (IsHull() && pAffectedComponents && impact)
  {
    // check if the hit hit one or more other components. if so, get the hull affection from these.
    float affection = 0.f;    
    int affected = 0;
    
    for (TVehicleComponentVector::const_iterator it=pAffectedComponents->begin(),end=pAffectedComponents->end(); it!=end; ++it)
    {
      CVehicleComponent* pComponent = *it;
      if (pComponent == this)
        continue;

      if (radius > 0.f)
      {
        const AABB& compBounds = pComponent->GetBounds();
        if (!compBounds.IsContainPoint(localPos))
          continue;
      }
      
      affection += (*it)->m_hullAffection;
      ++affected;

      if (DebugDamage())
        CryLog("component <%s> affecting hull, affection %.2f", (*it)->GetComponentName(), (*it)->m_hullAffection);
    }   
    
    affection = (affected==0) ? 1.f : affection/affected;
    damage *= affection;

    if (DebugDamage())
      CryLog("total affection: %.2f", affection);
  }
 
    
  CVehicleDamages::TDamageMultipliers::const_iterator ite = m_damageMultipliers.find(CONST_TEMP_STRING(damageType));
  if (ite != m_damageMultipliers.end())
  {
    const CVehicleDamages::SDamageMultiplier& mult = ite->second;
    damage *= mult.mult;
    damage *= splash ? mult.splash : 1.f;
    
    if (DebugDamage())
      CryLog("mults for %s: %.2f, splash %.2f", damageType, mult.mult, mult.splash);
  }
  else
    m_pVehicle->ProcessHit(damage, damageType, splash);

	// major components don't get repaired at the normal rate, the repair amount is determined
	//	by how important the component is (what proportion of the vehicle's health it is)
	if(strcmp(damageType, "repair") == 0 && damage < 0.0f && m_majorComponent && m_proportionOfVehicleHealth > 0.0f)
	{
		damage *= m_proportionOfVehicleHealth;
	}
	
  return damage;
}

void CVehicleComponent::SetProportionOfVehicleHealth(float proportion)
{
	m_proportionOfVehicleHealth = clamp(proportion, 0.0f, 1.0f);
}

void CVehicleComponent::GetMemoryStatistics(ICrySizer * s)
{
	s->Add(*this);
	s->Add(m_name);
	s->AddContainer(m_damageBehaviors);
	s->AddContainer(m_parts);

	for (size_t i=0; i<m_damageBehaviors.size(); i++)
		m_damageBehaviors[i].second->GetMemoryStatistics(s);
}
