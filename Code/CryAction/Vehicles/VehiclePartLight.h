/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements a light part

-------------------------------------------------------------------------
History:
- 24:10:2005: Created by Mathieu Pinard

*************************************************************************/
#ifndef __VEHICLEPARTLIGHT_H__
#define __VEHICLEPARTLIGHT_H__

#include "VehiclePartBase.h"

class CVehiclePartLight
	: public CVehiclePartBase
{
	IMPLEMENT_VEHICLEOBJECT
public:

	CVehiclePartLight();
	~CVehiclePartLight();

	// IVehiclePart
	virtual bool Init(IVehicle* pVehicle, const CVehicleParams& table, IVehiclePart* parent, CVehicle::SPartInitInfo& initInfo) override;
	virtual void PostInit() override;
	virtual void Reset() override;
	virtual void Release() override { CVehiclePartBase::Release(); }

	virtual void OnEvent(const SVehiclePartEvent& event) override;

	virtual void Physicalize() override {}

	virtual void Update(const float frameTime) override;

	virtual void Serialize(TSerialize serialize, unsigned aspects) override;
	virtual void RegisterSerializer(IGameObjectExtension* gameObjectExt) override {}
	virtual void GetMemoryStatistics(ICrySizer* s)  override { s->Add(*this); GetBaseMemoryStatistics(s); }
	// ~IVehiclePart

	virtual void ToggleLight(bool enable);
	virtual bool IsEnabled() { return m_enabled; }
	const string& GetLightType() const { return m_lightType; }

protected:

	void UpdateLight(const float frameTime);

	string m_lightType;
	CDLight m_light;
	_smart_ptr<IMaterial> m_pMaterial;

	std::vector<IVehicleComponent*> m_components;

	IVehicleHelper* m_pHelper;

	float m_diffuseMult[2];
	Vec3  m_diffuseCol;

	bool m_enabled;
};

#endif
