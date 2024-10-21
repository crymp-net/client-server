#pragma once

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

class CVehicleSystem;

class ScriptBind_VehicleSystem :
	public CScriptableBase
{
public:
	ScriptBind_VehicleSystem(CVehicleSystem* vehicleSystem);
	virtual ~ScriptBind_VehicleSystem();

	void Release() { delete this; };

	int LogSlots(IFunctionHandler* pH, char* vehicleName);
	int GetVehicleImplementations(IFunctionHandler* pH);
	int GetOptionalScript(IFunctionHandler* pH, char* vehicleName);
	int SetTpvDistance(IFunctionHandler* pH, float distance);
	int SetTpvHeight(IFunctionHandler* pH, float height);
	int ReloadSystem(IFunctionHandler* pH);

private:
	void LogChildsSlots(IEntity* pEntity);
	void LogSlotsForEntity(IEntity* pVehicleEntity);

	void RegisterGlobals();
	void RegisterMethods();

	CVehicleSystem* m_pVehicleSystem;
};
