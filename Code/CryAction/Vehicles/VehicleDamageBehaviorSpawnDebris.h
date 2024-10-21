/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior which spawn debris found in the
damaged model of the Animated parts

-------------------------------------------------------------------------
History:
- 30:11:2006: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEDAMAGEBEHAVIORSPAWNDEBRIS_H__
#define __VEHICLEDAMAGEBEHAVIORSPAWNDEBRIS_H__

#include <list>

class CVehiclePartAnimated;

class CVehicleDamageBehaviorSpawnDebris
	: public IVehicleDamageBehavior
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehicleDamageBehaviorSpawnDebris();
	~CVehicleDamageBehaviorSpawnDebris();

	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table);
	virtual void Reset();
	virtual void Release() { delete this; }
	virtual void Serialize(TSerialize ser, EEntityAspects aspects) {}
	virtual void Update(const float deltaTime);
	virtual void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& params);
	virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params);

	virtual void GetMemoryStatistics(ICrySizer * s);

protected:

	IEntity* SpawnDebris(IStatObj* pStatObj, Matrix34 vehicleTM, float force);
	void AttachParticleEffect(IEntity* pDetachedEntity, const string& effectName);
	void GiveImpulse(IEntity* pDetachedEntity, float force);

	IVehicle* m_pVehicle;

	typedef std::list <EntityId> TEntityIdList;
	TEntityIdList m_spawnedDebris;

	struct SDebrisInfo
	{
		CVehiclePartAnimated* pAnimatedPart;
		int jointId; // id of the joint in the intact model
		int slot;
		int index;
		EntityId entityId;
		float time;
		float force;
	};

	typedef std::list <SDebrisInfo> TDebrisInfoList;
	TDebrisInfoList m_debris;
	bool m_pickableDebris;
};

#endif
