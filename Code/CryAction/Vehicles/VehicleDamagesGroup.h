/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2006.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements the base of the vehicle damages group

-------------------------------------------------------------------------
History:
- 23:02:2006: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEDAMAGESGROUP_H__
#define __VEHICLEDAMAGESGROUP_H__

#include <list>
#include <vector>

class CVehicle;

class CVehicleDamagesGroup
	: public IVehicleDamagesGroup
{
public:

	virtual ~CVehicleDamagesGroup();

	bool Init(CVehicle* pVehicle, const CVehicleParams& table);
	void Release() { delete this; }
	void Reset();
	void Serialize(TSerialize ser, EEntityAspects aspects);

	VIRTUAL bool ParseDamagesGroup(const CVehicleParams& table);

	const string& GetName() { return m_name; }
	bool IsPotentiallyFatal();

	void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams);
	void Update(float frameTime);

	void GetMemoryStatistics(ICrySizer* pSizer);

protected:

	typedef std::vector <IVehicleDamageBehavior*> TVehicleDamageBehaviorVector;

	typedef uint16 TDamagesSubGroupId; 
	struct SDamagesSubGroup
	{
		TDamagesSubGroupId id;
		float m_randomness;
		TVehicleDamageBehaviorVector m_damageBehaviors;
		float m_delay;
		bool m_isAlreadyInProcess;

    SDamagesSubGroup()
    {
      id = ~0;
      m_randomness = 0.f;
      m_delay = 0.f;
      m_isAlreadyInProcess = false;
    }
	};

	typedef std::vector <SDamagesSubGroup> TDamagesSubGroupVector;
	TDamagesSubGroupVector m_damageSubGroups;

protected:

	IVehicleDamageBehavior* ParseDamageBehavior(const CVehicleParams& table);

	CVehicle* m_pVehicle;
	string m_name;

	struct SDelayedDamagesSubGroupInfo
	{
		float delay;
		TDamagesSubGroupId subGroupId;
		SVehicleDamageBehaviorEventParams behaviorParams;
	};

	typedef std::list<SDelayedDamagesSubGroupInfo> TDelayedDamagesSubGroupList;
	TDelayedDamagesSubGroupList m_delayedSubGroups;

	friend class CVehicleDamagesTemplateRegistry;
};

#endif
