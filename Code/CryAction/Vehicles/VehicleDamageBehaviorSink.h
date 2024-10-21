/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a damage behavior which sink the vehicle

-------------------------------------------------------------------------
History:
- 03:11:2005: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEDAMAGEBEHAVIORSINK_H__
#define __VEHICLEDAMAGEBEHAVIORSINK_H__

class CVehicle;
class CVehicleDamageBehaviorSink : public IVehicleDamageBehavior
{
    IMPLEMENT_VEHICLEOBJECT
public:
    CVehicleDamageBehaviorSink() {}
    virtual ~CVehicleDamageBehaviorSink() {}

    // --- IVehicleDamageBehavior interface functions ---
    virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table) override;
    virtual void Reset() override;
    virtual void Release() override { delete this; }

    virtual void OnDamageEvent(EVehicleDamageBehaviorEvent event, const SVehicleDamageBehaviorEventParams& behaviorParams) override;

    virtual void Serialize(TSerialize ser, unsigned aspects) override;
    virtual void Update(const float deltaTime) override {}

    virtual void GetMemoryStatistics(ICrySizer* s) override { s->Add(*this); }
    // --- End of IVehicleDamageBehavior interface functions ---

    // IVehicleObject
    void OnVehicleEvent(EVehicleEvent event, const SVehicleEventParams& params) override;

    void ChangeSinkingBehavior(bool isSinking);

protected:
    IVehicle* m_pVehicle;           
    float m_formerWaterDensity;    
    int m_sinkingTimer;            
    bool m_isSinking;               
};

#endif
