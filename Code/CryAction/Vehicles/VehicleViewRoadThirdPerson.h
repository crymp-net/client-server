/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a third person view for vehicles tweaked for 
racing gameplay

-------------------------------------------------------------------------
History:
- 18:08:2006: Created by Mathieu Pinard
- 19:02:2013: modfied by Marco Hopp

*************************************************************************/
#ifndef __VEHICLEVIEWACTIONTHIRDPERSON_H__
#define __VEHICLEVIEWACTIONTHIRDPERSON_H__

#include "VehicleViewBase.h"

class CVehicleViewRoadThirdPerson
	: public CVehicleViewBase
{
	IMPLEMENT_VEHICLEOBJECT;
public:

	CVehicleViewRoadThirdPerson();
	~CVehicleViewRoadThirdPerson();

	// IVehicleView
	virtual bool Init(CVehicleSeat* pSeat, const CVehicleParams& table);
	virtual void Reset();
	virtual void ResetPosition()
	{
		m_worldCameraPos = m_pVehicle->GetEntity()->GetWorldPos();
		m_worldViewPos = m_worldCameraPos;
	}

	virtual bool IsThirdPerson() { return true; }
	virtual bool IsPassengerHidden() { return false; }

	virtual void OnStartUsing(EntityId passengerId);

	virtual void OnAction(const TVehicleActionId actionId, int activationMode, float value);
	virtual void UpdateView(SViewParams &viewParams, EntityId playerId);

	virtual void Update(const float frameTime);
	virtual void Serialize(TSerialize serialize, EEntityAspects);

	virtual void GetMemoryUsage(ICrySizer * s) const  {s->Add(*this);}

	virtual void OffsetPosition(const Vec3 &delta);
	// ~IVehicleView

protected:

	Matrix34 GetCameraTM(const Matrix34& worldTM, const Vec3& aimPos);
	Vec3 GetAimPos(const Matrix34& worldTM);

	Vec3 m_worldViewPos;
	Vec3 m_worldCameraPos;
	Vec3 m_worldCameraAim;
	
	IVehiclePart* m_pAimPart;

	Vec3 m_localCameraPos;
	Vec3 m_localCameraAim;
	Vec3 m_velocityMult;
	float m_lagSpeed;
	
	Vec3 m_extraLag;
			
	float m_zoom;
	float m_zoomTarget;
	float m_actionZoom;
	float m_zoomClampValue;

	float m_cameraSmoothingOffset;
	float m_cameraSmoothingHeight;
	bool  m_camCollision;

	float m_heightAboveWater;

	Vec3 m_cameraOffset;

	static const char* m_className;
};

#endif
