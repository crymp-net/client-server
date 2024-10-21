/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a tread for tanks

-------------------------------------------------------------------------
History:
- 14:09:2005: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEPARTTREAD_H__
#define __VEHICLEPARTTREAD_H__

#include <utility>
#include <vector>

#include "VehicleSystem/VehiclePartBase.h"

class CVehicle;

class CVehiclePartTread
	: public CVehiclePartBase
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehiclePartTread();
	virtual ~CVehiclePartTread() {}

	// IVehiclePart
	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table, IVehiclePart* parent, CVehicle::SPartInitInfo& initInfo);
  virtual void InitGeometry();
  virtual void Reset();
	virtual void Release();

	virtual void OnEvent(const SVehiclePartEvent& event);

	virtual bool ChangeState(EVehiclePartState state, int flags=0);
		
	virtual void Physicalize();

	virtual const Matrix34& GetLocalTM(bool relativeToParentPart);
	virtual const Matrix34& GetWorldTM();
	virtual const AABB& GetLocalBounds();

	virtual void Update(const float frameTime);

	virtual void Serialize(TSerialize serialize, EEntityAspects aspects);
	virtual void RegisterSerializer(IGameObjectExtension* gameObjectExt) {}
  virtual int GetType(){ return m_typeId; }
	virtual void GetMemoryStatistics(ICrySizer * s);
	// ~IVehiclePart

  virtual void SetUVSpeed(float wheelSpeed);
  
  void SkeletonPostProcess(ICharacterInstance* pCharInstance);
      
protected:  

	void SetDamageRatio(float value);

  _smart_ptr<ICharacterInstance> m_pCharInstance;  		
	int m_lastWheelIndex;
	float m_uvRateMax;
		
	float m_lastWheelW;
  float m_lastUpdate;
	
  _smart_ptr<IMaterial> m_pMaterial;
	_smart_ptr<IRenderShaderResources> m_pShaderResources;

	struct SWheelInfo
	{
		int slot;
		int jointId;    
		CVehiclePartSubPartWheel* pWheel;
	};

	typedef std::vector <SWheelInfo> TWheelInfoVector;
	TWheelInfoVector m_wheels;
};

#endif
