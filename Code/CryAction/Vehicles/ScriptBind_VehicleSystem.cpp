#include "CryCommon/CrySystem/gEnv.h"
#include "CryCommon/CrySystem/ICryPak.h"
#include "CrySystem/CryLog.h"

#include "CryCommon/CryEntitySystem/IEntitySystem.h"
#include "ScriptBind_VehicleSystem.h"
#include "CryAction/Vehicles/VehicleSystem.h"
#include "VehicleViewThirdPerson.h"

//------------------------------------------------------------------------
ScriptBind_VehicleSystem::ScriptBind_VehicleSystem(CVehicleSystem* vehicleSystem)
{
	m_pVehicleSystem = vehicleSystem;

	Init(gEnv->pScriptSystem, gEnv->pSystem);
	SetGlobalName("Vehicle");

	RegisterGlobals();
	RegisterMethods();

	gEnv->pScriptSystem->ExecuteFile("Scripts/Entities/Vehicles/VehicleSystem.lua");
}

//------------------------------------------------------------------------
ScriptBind_VehicleSystem::~ScriptBind_VehicleSystem()
{
}

//------------------------------------------------------------------------
void ScriptBind_VehicleSystem::RegisterGlobals()
{
}

//------------------------------------------------------------------------
void ScriptBind_VehicleSystem::RegisterMethods()
{
#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &ScriptBind_VehicleSystem::

	SCRIPT_REG_TEMPLFUNC(LogSlots, "vehicleName");
	SCRIPT_REG_FUNC(GetVehicleImplementations);
	SCRIPT_REG_TEMPLFUNC(GetOptionalScript, "vehicleName");
	SCRIPT_REG_TEMPLFUNC(SetTpvDistance, "distance");
	SCRIPT_REG_TEMPLFUNC(SetTpvHeight, "height");
	SCRIPT_REG_TEMPLFUNC(ReloadSystem, "");
}


//------------------------------------------------------------------------
void ScriptBind_VehicleSystem::LogSlotsForEntity(IEntity* pVehicleEntity)
{
	for (int s = 0; s < pVehicleEntity->GetSlotCount(); s++)
	{
		if (pVehicleEntity->IsSlotValid(s))
		{
			if (pVehicleEntity->GetStatObj(s))
			{
				IStatObj* pStatObj = pVehicleEntity->GetStatObj(s);
				Vec3 worldPos = pVehicleEntity->GetWorldPos();
				CryLogWarning("- slot %d: statobj   - filename=<%s>, geometry=<%s>, worldpos={%f,%f,%f}", s, pStatObj->GetFilePath(), pStatObj->GetGeoName(), worldPos.x, worldPos.y, worldPos.z);
			}
			else if (pVehicleEntity->GetCharacter(s))
			{
				ICharacterInstance* pCharInst = pVehicleEntity->GetCharacter(s);
				CryLogWarning("- slot %d: character - filename=<%s>", s, pCharInst->GetFilePath());
			}
			else
			{
				CryLogWarning("- slot %d: unknown content", s);
			}
		}
	}
}

//------------------------------------------------------------------------
void ScriptBind_VehicleSystem::LogChildsSlots(IEntity* pEntity)
{
	for (int i = 0; i < pEntity->GetChildCount(); i++)
	{
		IEntity* pChild = pEntity->GetChild(i);
		if (pChild)
		{
			CryLogWarning("* child %d: name=<%s>", i, pChild->GetName());
			LogSlotsForEntity(pChild);

			if (pChild->GetChildCount() > 0)
			{
				LogChildsSlots(pChild);
			}
		}
	}
}

//------------------------------------------------------------------------
int ScriptBind_VehicleSystem::LogSlots(IFunctionHandler* pH, char* vehicleName)
{
	IEntity* pVehicleEntity = gEnv->pEntitySystem->FindEntityByName(vehicleName);
	if (!pVehicleEntity)
	{
		CryLogWarning("Error: Vehicle <%s> not found", vehicleName);
		return pH->EndFunction();
	}

	IVehicle* pVehicle = m_pVehicleSystem->GetVehicle(pVehicleEntity->GetId());
	if (!pVehicle)
	{
		CryLogWarning("Error: Entity <%s> is not a vehicle", vehicleName);
		return pH->EndFunction();
	}

	CryLogWarning("Overview of the slot usage for vehicle <%s>", vehicleName);

	LogSlotsForEntity(pVehicleEntity);
	CryLogWarning(" ");
	LogChildsSlots(pVehicleEntity);

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_VehicleSystem::GetVehicleImplementations(IFunctionHandler* pH)
{
	SmartScriptTable tableImpls(m_pSS->CreateTable());
	SVehicleImpls impls;

	m_pVehicleSystem->GetVehicleImplementations(impls);

	//CryLog("Scriptbind got %i vehicles", impls.Count());
	for (int i = 0; i < impls.Count(); ++i)
	{
		tableImpls->SetAt(i + 1, impls.GetAt(i).c_str());
	}
	return pH->EndFunction(tableImpls);
}


//------------------------------------------------------------------------
int ScriptBind_VehicleSystem::GetOptionalScript(IFunctionHandler* pH, char* vehicleName)
{
	char scriptName[1024] = { 0 };

	if (m_pVehicleSystem->GetOptionalScript(vehicleName, scriptName, sizeof(scriptName)))
		return pH->EndFunction(scriptName);

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_VehicleSystem::SetTpvDistance(IFunctionHandler* pH, float distance)
{
	CVehicleViewThirdPerson::SetDefaultDistance(distance);

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_VehicleSystem::SetTpvHeight(IFunctionHandler* pH, float height)
{
	CVehicleViewThirdPerson::SetDefaultHeight(height);

	return pH->EndFunction();
}



//------------------------------------------------------------------------
int ScriptBind_VehicleSystem::ReloadSystem(IFunctionHandler* pH)
{
	m_pVehicleSystem->ReloadSystem();

	return pH->EndFunction();
}