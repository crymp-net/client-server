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
#ifndef __VEHICLECOMPONENT_H__
#define __VEHICLECOMPONENT_H__

#include <utility>
#include <vector>

#include "VehicleDamages.h"

class CVehicleHUDElement;
class CVehicle;
struct IVehiclePart;

#define COMPONENT_DAMAGE_LEVEL_RATIO 0.25f

class CVehicleComponent
	: public IVehicleComponent
{
public:

	CVehicleComponent();
	~CVehicleComponent();

	bool Init(IVehicle* pVehicle, const CVehicleParams& paramsTable);
	void Reset();
	void Release() { delete this; }

	void GetMemoryStatistics(ICrySizer * s);

	// IVehicleComponent
	virtual unsigned int GetPartCount() const;
	virtual IVehiclePart* GetPart(unsigned int index) const;    
  virtual const AABB& GetBounds(); // bounds are in vehicle space
  virtual float GetDamageRatio() const;
	virtual void SetDamageRatio(float ratio);
	virtual float GetMaxDamage() const;
  virtual const char* GetComponentName() const { return m_name.c_str(); }
	// ~IVehicleComponent
	
  const string& GetName() { return m_name; }
  ILINE bool IsHull() { return m_bHull; }

	void OnHit(EntityId targetId, EntityId shooterId, float damage, Vec3 position, float radius, const char* damageType, bool explosion=false, const TVehicleComponentVector* pAffectedComponents=0);
	void OnVehicleDestroyed();
	void Update(float deltaTime);
	void BroadcastDamage(float damage, EntityId shooterId);

	void Serialize(TSerialize ser, EEntityAspects aspects);

	void AddPart(IVehiclePart* pPart);
  void DebugDraw();

	bool IsMajorComponent() const { return m_majorComponent; } 
	void SetProportionOfVehicleHealth(float proportion);
	float GetProportionOfVehicleHealth() const { return m_proportionOfVehicleHealth; } 

protected:

	float ProcessDamage(float damage, const Vec3& localPos, float radius, const char* damageType, bool explosion, bool impact, const TVehicleComponentVector* pAffectedComponents);

	CVehicle* m_pVehicle;

	string m_name;
	AABB m_bounds;
	bool m_useBoundsFromParts;
	
	float m_damage;
	float m_damageMax;
  float m_hullAffection;
  bool  m_bHull;

	struct SDamageBehaviorParams
	{
		float damageRatioMin;
		float damageRatioMax;
		bool ignoreOnVehicleDestroyed;
	};

	typedef std::pair <SDamageBehaviorParams, IVehicleDamageBehavior*> TVehicleDamageBehaviorPair;
	typedef std::vector <TVehicleDamageBehaviorPair> TVehicleDamageBehaviorVector;

  TVehicleDamageBehaviorVector m_damageBehaviors;

	typedef std::vector <IVehiclePart*> TVehiclePartVector;
	TVehiclePartVector m_parts;

	CVehicleHUDElement* m_pHudMessage;

	CVehicleDamages::TDamageMultipliers m_damageMultipliers;

	bool m_isOnlyDamageableByPlayer;
  bool m_useDamageLevels;

	bool m_majorComponent;
	float m_proportionOfVehicleHealth;

	Vec3 m_lastHitLocalPos;
	float m_lastHitRadius;
	EntityId m_lastHitShooterId;
  int m_lastHitType;
};

#endif
