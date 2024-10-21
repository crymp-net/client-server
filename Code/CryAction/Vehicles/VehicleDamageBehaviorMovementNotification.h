/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior which notify the movement class

-------------------------------------------------------------------------
History:
- 17:10:2005: Created by Mathieu Pinard

*************************************************************************/

#ifndef __VEHICLEDAMAGEBEHAVIORMOVEMENTNOTIFICATION_H__
#define __VEHICLEDAMAGEBEHAVIORMOVEMENTNOTIFICATION_H__

class CVehicle;

class CVehicleDamageBehaviorMovementNotification : public IVehicleDamageBehavior
{
    IMPLEMENT_VEHICLEOBJECT
public:
    CVehicleDamageBehaviorMovementNotification();
    virtual ~CVehicleDamageBehaviorMovementNotification() {};

    // --- IVehicleDamageBehavior interface functions ---
    virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table) override;
    virtual void Reset() override;
    virtual void Release() { delete this; }

    virtual void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams) override;

    virtual void Serialize(TSerialize ser, unsigned aspects) override {}
    virtual void Update(const float deltaTime) override {}

    virtual void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override {}
    virtual void GetMemoryStatistics(ICrySizer* s) override { s->Add(*this); }
    // --- End of IVehicleDamageBehavior interface functions ---

protected:
    IVehicle* m_pVehicle;

    bool m_isSteeringInvolved;
    bool m_isFatal;
    bool m_isDamageAlwaysFull;

    int m_param;
};

#endif
