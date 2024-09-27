#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

class VehicleSystem;

class ScriptBind_VehicleSystem : public CScriptableBase
{
	VehicleSystem* m_pVehicleSystem = nullptr;

public:
	explicit ScriptBind_VehicleSystem(VehicleSystem* pVehicleSystem);

	int LogSlots(IFunctionHandler* pH, const char* vehicleName);
	int GetVehicleImplementations(IFunctionHandler* pH);
	int GetOptionalScript(IFunctionHandler* pH, const char* vehicleName);
	int SetTpvDistance(IFunctionHandler* pH, float distance);
	int SetTpvHeight(IFunctionHandler* pH, float height);
	int ReloadSystem(IFunctionHandler* pH);
};
