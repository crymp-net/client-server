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
#ifndef __VEHICLEPARTBASE_H__
#define __VEHICLEPARTBASE_H__

#include <list>
#include "CryCommon/CryAction/IVehicleSystem.h"
#include "CryCommon/CryAction/Network/NetActionSync.h"
#include "VehicleCVars.h"
#include "Vehicle.h"


class CVehiclePartSubPart;
class CVehiclePartSubPartEntity;
class CVehicle;
class CVehicleComponent;
class CVehicleSeatActionRotateTurret;

#define VEHICLE_MASS_COLLTYPE_HEAVY 1500.f
#define MAX_OPTIONAL_PARTS 5
#define GEOMETRY_DESTROYED_SUFFIX "_damaged"


class CVehiclePartBase
	: public IVehiclePart
{
  IMPLEMENT_VEHICLEOBJECT;
public:
  
	CVehiclePartBase();
	virtual ~CVehiclePartBase();

	// Initialization and Cleanup
	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table, IVehiclePart* parent, CVehicle::SPartInitInfo& initInfo);

	//IVehiclePart
	virtual void PostInit() override;
	virtual void Reset() override;
	virtual void Release() override;

	// Part Information
	virtual const string& GetName() override { return m_name; }
	virtual IVehiclePart* GetParent(bool root = false) override;
	virtual IEntity* GetEntity() override;

	// Child Parts Management
	virtual void InvalidateTM(bool invalidate) override {}
	virtual void AddChildPart(IVehiclePart* pPart) override { m_children.push_back((CVehiclePartBase*)pPart); }

	// Event Handling
	virtual void OnEvent(const SVehiclePartEvent& event) override;

	// State Management
	virtual bool ChangeState(EVehiclePartState state, int flags = 0) override;
	virtual EVehiclePartState GetState() const override { return m_state; }

	// Material Handling
	virtual void SetMaterial(IMaterial* pMaterial) override;

	// Transformation Management
	virtual void Physicalize() override {} 
	virtual void SetMoveable() override {} 

	virtual const Matrix34& GetLocalTM(bool relativeToParentPart) override;
	virtual const Matrix34& GetWorldTM() override;
	virtual void SetLocalTM(const Matrix34& localTM) override;
	virtual const AABB& GetLocalBounds() override;

	// Base Transform Handling
	virtual const Matrix34& GetLocalBaseTM() override { return GetLocalTM(true); }
	virtual void SetLocalBaseTM(const Matrix34& tm) override { SetLocalTM(tm); }

	// Initial Transform
	virtual const Matrix34& GetLocalInitialTM() override { return GetLocalTM(true); }

	// Update Loop
	virtual void Update(const float deltaTime) override;

	// Serialization
	virtual void Serialize(TSerialize ser, unsigned aspects) override;
	virtual void PostSerialize() override {}

	// Serializer Registration
	virtual void RegisterSerializer(IGameObjectExtension* gameObjectExt) override {}

	// Type Management
	virtual int GetType() override { return m_typeId; }
	virtual IVehicleWheel* GetIWheel() override { return nullptr; }

	// Physical Properties
	virtual const Vec3& GetDetachBaseForce() override { return m_detachBaseForce; }
	virtual float GetMass() override { return m_mass; }
	virtual int GetPhysId() override { return m_physId; }
	virtual int GetSlot() override { return m_slot; }

	// New: Memory Statistics (missing in implementation)
	virtual void GetMemoryStatistics(ICrySizer* s) override
	{
		s->Add(*this);
		GetBaseMemoryStatistics(s);
	}

	// New: Handle Part Rotation Limits (missing in implementation)
	virtual bool GetRotationLimits(int axis, float& min, float& max) override { return false; }

	// New: Part Hit Event (missing in implementation)
	virtual void OnHit(Vec3 localPos, float radius, float strength = 1.0f) override{} //CryMP: fixme: implement
	// ~IVehiclePart Implementation End

	virtual void ResetLocalTM(bool recursive);
  virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params){}
  
	virtual IStatObj* GetSubGeometry(CVehiclePartBase* pPart, EVehiclePartState state, Matrix34& position, bool removeFromParent);
	virtual IStatObj* GetStatObj() { return NULL; }
	virtual bool SetStatObj(IStatObj* pStatObj) {  return false; }
  virtual void GetGeometryName(EVehiclePartState state, string& name);
	
  virtual IMaterial* GetMaterial();
	
	bool IsPhysicalized() { return m_isPhysicalized; }
	void SetSeat(CVehicleSeat *pSeat) { m_pSeat = pSeat; };
	CVehicleSeat *GetSeat() { return m_pSeat; };
  
  const Matrix34& LocalToVehicleTM(const Matrix34& localTM);
  EVehiclePartState GetMaxState();
  float GetDetachProbability() { return m_detachProbability; }
  void InitComponent();

	void CloneMaterial();

	bool IsRotationBlocked() const { return m_isRotationBlocked; }

  static const char * m_geometryDestroyedSuffix;
public:

	enum EVehiclePartBaseEvent
	{
		eVPE_ParentPartUpdated = eVPE_OtherEvents,
	};

	typedef std::list <CVehiclePartBase*> TVehiclePartBaseList;

	virtual void Hide(bool hide);

protected:
    
	void ParsePhysicsParams(SEntityPhysicalizeParams& physicalizeParams, const CVehicleParams& table);
	  
  bool ClampToRotationLimits(Ang3& angles);
  
  void CheckColltypeHeavy(int partid);
  void SetFlags(int partid, int flags, bool add);
  void SetFlagsCollider(int partid, int flagsCollider);
	
	EntityId SpawnEntity();
  int GetCGASlot(int jointId);
    
  float GetDamageSpeedMul();
  virtual EVehiclePartState GetStateForDamageRatio(float ratio);
  
	void GetBaseMemoryStatistics(ICrySizer * s);
  
protected:
  int m_typeId;

	CVehicle* m_pVehicle;
	
	int m_slot;
	AABB m_bounds;
  	
	string m_name;	
  EVehiclePartState m_state;
  
  const char* m_pComponentName;    
  IVehiclePart* m_pParentPart;
  
  typedef std::vector<CVehiclePartBase*> TVehicleChildParts;
  TVehicleChildParts m_children;
	
	string m_helperPosName;
	Vec3 m_position;
  
	int m_hideCount;	// if 0, not hidden. Allows multiple hide/unhide requests simultaneously.
  int m_useOption;  	
		
	bool m_isRotationBlocked;
	bool m_isPhysicalized;
	bool m_disableCollision;	
  int m_physId;
	float m_mass;
  float m_density;
	
	CVehicleSeat *m_pSeat;  

	float m_damageRatio;		
  
	// used to fade in/out opacity
	enum EVehiclePartHide
	{
		eVPH_NoFade = 0,
		eVPH_FadeIn,
		eVPH_FadeOut,
	};

	EVehiclePartHide m_hideMode;
	float m_hideTimeMax;
	float m_hideTimeCount;

	Vec3 m_detachBaseForce;
  float m_detachProbability;

	int m_users;
    
	friend class CVehiclePartSubPart;
	friend class CVehiclePartSubPartEntity;
	friend class CVehiclePartAnimatedJoint;
	friend class CVehicleSeatActionRotateTurret;
  
	IMaterial* m_pClonedMaterial;
};

namespace
{
  ILINE bool IsDebugParts() 
  { 
    return VehicleCVars().v_debugdraw == eVDB_Parts; 
  } 
}

#endif
