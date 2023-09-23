#include "CryCommon/CryAction/IVehicleSystem.h"

#include "ScriptBind_Vehicle.h"

ScriptBind_Vehicle::ScriptBind_Vehicle(ISystem* pSystem, IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x30540730;
#else
	std::uintptr_t ctor = 0x3052ebf0;
#endif

	(this->*reinterpret_cast<void(ScriptBind_Vehicle::*&)(ISystem*, IGameFramework*)>(ctor))(pSystem, pGameFramework);
}

void ScriptBind_Vehicle::AttachTo(IVehicle* pVehicle)
{
	IScriptTable* pScriptTable = pVehicle->GetEntity()->GetScriptTable();
	if (!pScriptTable)
	{
		return;
	}

	SmartScriptTable methodsTable(m_pSS);
	methodsTable->SetValue("vehicleId", ScriptHandle(pVehicle->GetEntityId()));
	methodsTable->Delegate(this->GetMethodsTable());

	pScriptTable->SetValue("vehicle", methodsTable);
}
