#include "CryCommon/CrySystem/gEnv.h"
#include "CrySystem/CryLog.h"

#include "ScriptBind_VehicleSystem.h"
#include "Vehicles/VehicleSystem.h"

ScriptBind_VehicleSystem::ScriptBind_VehicleSystem(VehicleSystem* pVehicleSystem) : m_pVehicleSystem(pVehicleSystem)
{
	this->Init(gEnv->pScriptSystem, gEnv->pSystem);
	this->SetGlobalName("Vehicle");

#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &ScriptBind_VehicleSystem::

	SCRIPT_REG_TEMPLFUNC(LogSlots, "vehicleName");
	SCRIPT_REG_TEMPLFUNC(GetVehicleImplementations, "");
	SCRIPT_REG_TEMPLFUNC(GetOptionalScript, "vehicleName");
	SCRIPT_REG_TEMPLFUNC(SetTpvDistance, "distance");
	SCRIPT_REG_TEMPLFUNC(SetTpvHeight, "height");
	SCRIPT_REG_TEMPLFUNC(ReloadSystem, "");
}

int ScriptBind_VehicleSystem::LogSlots(IFunctionHandler* pH, const char* vehicleName)
{
	// unused
	CryLogErrorAlways("[ScriptBind_VehicleSystem::LogSlots] Not implemented!");

	return pH->EndFunction();
}

int ScriptBind_VehicleSystem::GetVehicleImplementations(IFunctionHandler* pH)
{
	SmartScriptTable vehicles(m_pSS);

	for (const auto& [ name, pCreator ] : m_pVehicleSystem->m_vehicleFactories)
	{
		vehicles->PushBack(name.c_str());
	}

	return pH->EndFunction(vehicles);
}

int ScriptBind_VehicleSystem::GetOptionalScript(IFunctionHandler* pH, const char* vehicleName)
{
	if (!m_pVehicleSystem->m_vehicleFactories.contains(vehicleName))
	{
		return pH->EndFunction();
	}

	std::string path;
	path.reserve(100);
	path += "Scripts/Entities/Vehicles/Implementations/";
	path += vehicleName;
	path += ".lua";

	return pH->EndFunction(path.c_str());
}

int ScriptBind_VehicleSystem::SetTpvDistance(IFunctionHandler* pH, float distance)
{
	// unused
	CryLogErrorAlways("[ScriptBind_VehicleSystem::SetTpvDistance] Not implemented!");

	return pH->EndFunction();
}

int ScriptBind_VehicleSystem::SetTpvHeight(IFunctionHandler* pH, float height)
{
	// unused
	CryLogErrorAlways("[ScriptBind_VehicleSystem::SetTpvHeight] Not implemented!");

	return pH->EndFunction();
}

int ScriptBind_VehicleSystem::ReloadSystem(IFunctionHandler* pH)
{
	m_pVehicleSystem->Reload();

	return pH->EndFunction();
}
