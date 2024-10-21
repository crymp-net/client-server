/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a part for vehicles which uses CGA files

-------------------------------------------------------------------------
History:
- 24:08:2005: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEPARTANIMATED_H__
#define __VEHICLEPARTANIMATED_H__

#include <list>
#include <map>

#include "VehiclePartBase.h"

class CScriptBind_VehiclePart;
class CVehicle;
class CVehiclePartAnimatedJoint;

class CVehiclePartAnimated
	: public CVehiclePartBase
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehiclePartAnimated();
	~CVehiclePartAnimated();

	// IVehiclePart
	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table, IVehiclePart* parent, CVehicle::SPartInitInfo& initInfo);  
	virtual void Release();
	virtual void Reset();
  
  virtual void SetMaterial(IMaterial* pMaterial);
  
	virtual void OnEvent(const SVehiclePartEvent& event);
  virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params);

	virtual bool ChangeState(EVehiclePartState state, int flags=0);
	virtual void Physicalize();
  
	virtual void Update(const float frameTime);

	virtual void Serialize(TSerialize serialize, EEntityAspects);
  virtual void PostSerialize();

	virtual void GetMemoryStatistics(ICrySizer * s);
	// ~IVehiclePart

	virtual IStatObj* GetSubGeometry(CVehiclePartBase* pPart, EVehiclePartState state, Matrix34& position, bool removeFromParent);
	IStatObj* GetGeometryForState(CVehiclePartBase* pPart, EVehiclePartState state);  
  IStatObj* GetDestroyedGeometry(const char* pJointName, unsigned int index = 0);
	Matrix34 GetDestroyedGeometryTM(const char* pJointName, unsigned int index);

  virtual void SetDrivingProxy(bool bDrive);

	void RegisterRotatingAnimatedJoint(CVehiclePartAnimatedJoint* pAnimatedJoint);
	void ProcessAnimatedJointRotations();
  void RotationChanged(CVehiclePartAnimatedJoint* pJoint);

	int AssignAnimationLayer() { return ++m_lastAnimLayerAssigned; }

  void Dump();
	int GetNextFreeLayer();
  bool ChangeChildState(CVehiclePartAnimatedJoint* pPart, EVehiclePartState state, int flags);
      
protected:

  virtual void InitGeometry();
  void FlagSkeleton(ISkeletonPose* pSkeletonPose);
  virtual EVehiclePartState GetStateForDamageRatio(float ratio);
  
	string m_filename;
	string m_filenameDestroyed;
	string m_destroyedSuffix;
		
  _smart_ptr<ICharacterInstance> m_pCharInstance;
	ICharacterInstance* m_pCharInstanceDestroyed;
	
	typedef std::map <string, /*_smart_ptr<*/IStatObj* > TStringStatObjMap;
	TStringStatObjMap m_intactStatObjs;

	typedef std::map<string, IVehiclePart*> TStringVehiclePartMap;
	TStringVehiclePartMap m_jointParts;

  int m_hullMatId[2];

	typedef std::list<CVehiclePartAnimatedJoint*> TAnimatedJointList;
	TAnimatedJointList m_rotatingAnimatedJoints;

	int m_lastAnimLayerAssigned;
  int m_iRotChangedFrameId;
	bool m_serializeForceRotationUpdate;
	bool m_initialiseOnChangeState;
};

#endif
