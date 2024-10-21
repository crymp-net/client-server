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
- 05:09:2005: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEPARTANIMATEDJOINT_H__
#define __VEHICLEPARTANIMATEDJOINT_H__

#include <map>

#include "VehicleSystem/VehiclePartBase.h"

class CVehicle;


class CVehiclePartAnimatedJoint
	: public CVehiclePartBase
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehiclePartAnimatedJoint();	
	virtual ~CVehiclePartAnimatedJoint();  

	// IVehiclePart
	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table, IVehiclePart* parent, CVehicle::SPartInitInfo& initInfo);
  virtual void InitGeometry(const CVehicleParams& table);
	virtual void PostInit();
	virtual void Reset();
  virtual bool ChangeState(EVehiclePartState state, int flags);
  virtual void SetMaterial(IMaterial* pMaterial);
  virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params);	
  
	virtual const Matrix34& GetLocalTM(bool relativeToParentPart);
	virtual const Matrix34& GetWorldTM();
	virtual const AABB& GetLocalBounds();
	virtual const Matrix34& GetLocalInitialTM() { return m_initialTM; }
	
  virtual void Physicalize();
	virtual void Update(const float frameTime);  

  virtual void InvalidateTM(bool invalidate);    
  virtual void SetMoveable();

	virtual void GetMemoryStatistics(ICrySizer * s) { s->Add(*this); GetBaseMemoryStatistics(s); }
	// ~IVehiclePart
	
	virtual void SetLocalTM(const Matrix34& localTM);
  virtual void ResetLocalTM(bool recursive);

  virtual int GetJointId() { return m_jointId; }

	virtual IStatObj* GetStatObj();
	virtual bool SetStatObj(IStatObj* pStatObj);

	virtual IMaterial* GetMaterial();
  virtual void OnEvent(const SVehiclePartEvent& event);

	virtual void ApplyRotations();  

  virtual const Matrix34& GetLocalBaseTM() { return m_baseTM; }
  virtual void SetLocalBaseTM(const Matrix34& tm);

	virtual void SerMatrix(TSerialize ser, Matrix34& mat);
	virtual void Serialize(TSerialize ser, EEntityAspects aspects);

protected:
  
	_smart_ptr<ICharacterInstance> m_pCharInstance;
  CVehiclePartAnimated* m_pAnimatedPart;
	int m_jointId;
    
  Matrix34 m_baseTM;
  Matrix34 m_initialTM;	
	Matrix34 m_worldTM;  
  Matrix34 m_localTM;
	AABB m_localBounds;

  // if using external geometry
  _smart_ptr<IStatObj> m_pGeometry;
  _smart_ptr<IStatObj> m_pDestroyedGeometry;
  _smart_ptr<IMaterial> m_pMaterial;

  float m_dialsRotMax;
  float m_initialRotOfs;

  bool m_localTMInvalid;
  bool m_isMoveable;

	friend class CVehiclePartAnimated;
};

#endif
