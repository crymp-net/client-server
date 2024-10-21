/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a third person view for vehicles

-------------------------------------------------------------------------
History:
- 02:05:2005: Created by Mathieu Pinard
- 19:02:2013: modfied by Marco Hopp

*************************************************************************/
#include "StdAfx.h"

#include "IViewSystem.h"
#include "IVehicleSystem.h"
#include "VehicleSeat.h"
#include "VehicleViewRoadThirdPerson.h"
#include "VehicleCVars.h"

#include <Cry_GeoIntersect.h>
#include <Cry_GeoDistance.h>

const char* CVehicleViewRoadThirdPerson::m_className = "RoadThirdPerson";
static float cameraRadius = 0.42f;

//------------------------------------------------------------------------
CVehicleViewRoadThirdPerson::CVehicleViewRoadThirdPerson()
: m_heightAboveWater(0.0f)
{	
	m_pAimPart = NULL;
	m_zoom = 1.0f;
	m_actionZoom = 0.0f;
	m_zoomTarget = 1.f; 
	m_lagSpeed = 1.f;  
	m_extraLag.zero();  
	m_worldViewPos.zero();
	m_worldCameraPos.zero();
	m_worldCameraAim.zero();
	m_localCameraPos.zero();
	m_localCameraAim.zero();
	m_velocityMult.Set(1.f, 1.f, 1.f);
	m_cameraOffset.zero();
}

//------------------------------------------------------------------------
CVehicleViewRoadThirdPerson::~CVehicleViewRoadThirdPerson()
{
}

//------------------------------------------------------------------------
bool CVehicleViewRoadThirdPerson::Init(CVehicleSeat* pSeat, const CVehicleParams& table)
{
	if (!CVehicleViewBase::Init(pSeat, table))
		return false;

	m_pSeat = pSeat;
	m_pVehicle = pSeat->GetVehicle();
	m_pAimPart = pSeat->GetAimPart();

	if (CVehicleParams paramsTable = table.findChild(m_className))
	{
		paramsTable.getAttr("heightAboveWater", m_heightAboveWater);
		
    float heightOffset = 1.5f; // default offset (suggested by designers)
    paramsTable.getAttr("heightOffset", heightOffset);    
		
		AABB bounds;
		
		if (m_pAimPart)
		{
			bounds = m_pAimPart->GetLocalBounds();
			bounds.SetTransformedAABB(m_pAimPart->GetLocalTM(false).GetInverted(), bounds);
		}
		else
			m_pVehicle->GetEntity()->GetLocalBounds(bounds);

		bool ignoreBounds;
		if (!paramsTable.getAttr("ignoreBounds", ignoreBounds))
			ignoreBounds = false;

		if (ignoreBounds)
		{
			m_localCameraPos.Set(0.0f, 0.0f, 0.0f + heightOffset);
			m_localCameraAim.Set(0.0f, 0.0f, 0.0f);
		}
		else
		{
			m_localCameraPos.Set(0.0f, bounds.min.y, bounds.max.z + heightOffset);
			m_localCameraAim.Set(0.0f, bounds.max.y, bounds.max.z * 0.5f);
		}

		if(!paramsTable.getAttr("camCollision", m_camCollision))
			m_camCollision = false;

		Vec3 offset;

		if (paramsTable.getAttr("cameraAimOffset", offset))
			m_localCameraAim += offset;

		if (paramsTable.getAttr("cameraPosOffset", offset))
			m_localCameraPos += offset;

		if (!paramsTable.getAttr("velocityMult", m_velocityMult))
			m_velocityMult.zero();

		paramsTable.getAttr("lagSpeed", m_lagSpeed);
		//paramsTable.getAttr("velocityMult", m_velocityMult);	
		paramsTable.getAttr("cameraSmoothingHeight", m_cameraSmoothingHeight);

		Vec3 distanceNew = m_localCameraAim;

		if(!ignoreBounds)
			distanceNew.y-=bounds.max.y;

		m_zoomClampValue = m_localCameraPos.GetDistance(distanceNew);
		m_cameraSmoothingOffset = 0.f;
	}

	Reset();
	return (true);
}

//------------------------------------------------------------------------
void CVehicleViewRoadThirdPerson::Reset()
{
	CVehicleViewBase::Reset();

	m_actionZoom = 0.0f;
	m_zoomTarget = 1.0f;
	m_zoom = 1.0f;
	m_extraLag.zero();
}

//------------------------------------------------------------------------
void CVehicleViewRoadThirdPerson::OnStartUsing(EntityId passengerId)
{
	CVehicleViewBase::OnStartUsing(passengerId);

	Vec3 worldPos = m_pVehicle->GetEntity()->GetWorldPos();

	m_worldCameraPos = gEnv->pSystem->GetViewCamera().GetPosition();
	m_worldViewPos = m_worldCameraPos;
	m_worldCameraAim = worldPos;
	m_cameraOffset = m_worldCameraPos - worldPos;

	m_zoomTarget = 1.0f;
	m_zoom = 1.0f;
}

//------------------------------------------------------------------------
void CVehicleViewRoadThirdPerson::OnAction(const TVehicleActionId actionId, int activationMode, float value)
{
	CVehicleViewBase::OnAction(actionId, activationMode, value);

	if (actionId == eVAI_RotatePitch)
	{
		m_zoomTarget -= value;
	}
}

//------------------------------------------------------------------------
void CVehicleViewRoadThirdPerson::Update(float frameTime)
{
	CVehicleViewBase::Update(frameTime);

	// apply any zoom changes
	m_zoomTarget += m_actionZoom;
	m_zoomTarget = clamp(m_zoomTarget, -m_zoomClampValue, m_zoomClampValue);
	Interpolate(m_zoom, m_zoomTarget, 2.5f, frameTime);
	m_actionZoom = 0.0f;
	
	const SVehicleStatus& status = m_pVehicle->GetStatus();	
	Vec3 localVel = m_pVehicle->GetEntity()->GetWorldRotation().GetInverted() * status.vel;

	Vec3 newMult;
	Vec3(m_worldViewPos - m_worldCameraAim).GetLength2D()>10.f?newMult=m_velocityMult:newMult.zero();

	Interpolate(m_extraLag.x, localVel.x * newMult.x, m_lagSpeed, frameTime);
	Interpolate(m_extraLag.y, localVel.y * newMult.y, m_lagSpeed, frameTime);
	Interpolate(m_extraLag.z, localVel.z * newMult.z, m_lagSpeed, frameTime);

	Matrix34 worldTM;

	if (m_pAimPart)
		worldTM = m_pAimPart->GetWorldTM();
	else
		worldTM = m_pVehicle->GetEntity()->GetWorldTM();

	Matrix34 lagTM;
	lagTM.SetIdentity();
	lagTM.SetTranslation(m_extraLag);

	// update both aim pos and camera pos
	Ang3 worldAngles(worldTM);
	float rot = worldAngles.z + m_rotation.z;

	m_worldCameraPos = (worldTM * lagTM) * m_localCameraPos;
	Vec3 camWorldPos = m_worldCameraPos;

	//float distance = Vec3(m_localCameraPos - m_localCameraAim).GetLength();
	//distance *= 0.5f;
	
	m_worldCameraAim = worldTM * m_localCameraAim;

	camWorldPos.x = /*(distance * m_zoom) */ cosf(rot + gf_PI*1.5f) + camWorldPos.x;
	camWorldPos.y = /*(distance * m_zoom) */ sinf(rot - gf_PI/2.0f) + camWorldPos.y;

	if (!iszero(m_heightAboveWater))
	{
		float waterLevel = gEnv->p3DEngine->GetWaterLevel(&camWorldPos);
		camWorldPos.z = max(waterLevel + m_heightAboveWater, camWorldPos.z);
	}
	
	m_worldCameraPos = camWorldPos;
	Vec3 newPos = m_worldCameraPos;

  
	AABB bounds;
	m_pVehicle->GetEntity()->GetWorldBounds(bounds);

	Vec3 center = worldTM.GetTranslation();
	center.z += 1.0f;

	if(m_camCollision)
	{
		IPhysicalEntity* pSkipEntities[10];
		int nSkip = 0;
		if(m_pVehicle)
		{
			nSkip = m_pVehicle->GetSkipEntities(pSkipEntities, 10);
		}

		primitives::sphere sphere;
		sphere.center = center;
		sphere.r = cameraRadius;
		Vec3 dir = newPos-center;

		geom_contact *pContact = 0;          
		float hitDist = gEnv->pPhysicalWorld->PrimitiveWorldIntersection(sphere.type, &sphere, dir, ent_static|ent_terrain|ent_rigid|ent_sleeping_rigid,
			&pContact, 0, geom_colltype_player, 0, 0, 0, pSkipEntities, nSkip);
		if(hitDist > 0.0f)
		{
			newPos = center + hitDist * dir.GetNormalizedSafe();
		}

		// check new pos is outside the vehicle bounds...
		bounds.Expand(Vec3(cameraRadius, cameraRadius, 0.0f));

		if(bounds.IsContainPoint(newPos))
		{
			// nope, still inside.
			// Take the height of the aim pos and sweep a sphere downwards to the requested pos. Places the camera
			//	on top of the vehicle.
			ray_hit hit;
			sphere.center = newPos;
			sphere.center.z = center.z + 3.0f;

			Vec3 newdir = Vec3(0,0,-5);
			if(gEnv->pPhysicalWorld->CollideEntityWithPrimitive(m_pVehicle->GetEntity()->GetPhysics(), sphere.type, &sphere, newdir, &hit))
			{
				newPos = hit.pt;
				newPos.z += cameraRadius;
			}
		}
	}

#if ENABLE_VEHICLE_DEBUG
	if (VehicleCVars().v_debugdraw == 4)
	{ 
		gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(m_worldCameraAim,1.f,ColorB(0,0,255,255));
		gEnv->pRenderer->GetIRenderAuxGeom()->DrawLine(m_pVehicle->GetEntity()->GetPos(),ColorB(255,255,0,0),m_worldCameraAim,ColorB(255,255,0,0),2.f);
	}
#endif
	
	// interpolate the offset, not the camera position (ensures camera moves with vehicle - reduces perceived jitter)
	static float interpSpeed = 5.f;
	Interpolate(m_cameraOffset, newPos - center, interpSpeed, frameTime);
	m_worldViewPos = center + m_cameraOffset;

	if (m_canZoom)
	{
		Vec3 zoomVec = (m_worldCameraAim - m_worldViewPos).GetNormalizedSafe();
		m_worldViewPos += (zoomVec * m_zoom);

		if (m_cameraSmoothingHeight)
		{
			float currentLocalHeight = m_worldViewPos.z - m_worldCameraAim.z;
			m_cameraSmoothingOffset = (currentLocalHeight/100) * ((100/m_zoomClampValue) * m_zoom);
		}
	}
}

//------------------------------------------------------------------------
void CVehicleViewRoadThirdPerson::UpdateView(SViewParams &viewParams, EntityId playerId)
{
	Vec3 newDir = m_worldCameraAim;

	if (m_cameraSmoothingOffset>0.f)
		newDir.z += m_cameraSmoothingOffset;
	
	//Matrix33 cameraTM = Matrix33::CreateRotationVDir((m_worldCameraAim - m_worldViewPos).GetNormalizedSafe());
	Matrix33 cameraTM = Matrix33::CreateRotationVDir((newDir - m_worldViewPos).GetNormalizedSafe());
	cameraTM.CreateRotationZ(90);
	viewParams.rotation = Quat(cameraTM);

	// set view direction on actor
	IActor* pActor = CCryAction::GetCryAction()->GetIActorSystem()->GetActor(playerId);
	if(pActor && pActor->IsClient())
	{
		pActor->SetViewInVehicle(viewParams.rotation);
	}

	viewParams.position = m_worldViewPos;	
	viewParams.nearplane = cameraRadius + 0.1f;
}

//------------------------------------------------------------------------
void CVehicleViewRoadThirdPerson::Serialize(TSerialize serialize, EEntityAspects aspects)
{
	CVehicleViewBase::Serialize(serialize, aspects);

	if (serialize.GetSerializationTarget() != eST_Network)
	{
		serialize.Value("zoom", m_zoom);		
	}
}

//------------------------------------------------------------------------
void CVehicleViewRoadThirdPerson::OffsetPosition(const Vec3 &delta)
{
#ifdef SEG_WORLD
	m_worldViewPos += delta;
	m_worldCameraPos += delta;
#endif
}

DEFINE_VEHICLEOBJECT(CVehicleViewRoadThirdPerson);
