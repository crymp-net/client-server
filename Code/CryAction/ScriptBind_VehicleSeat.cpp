#include "CryCommon/CryAction/IVehicleSystem.h"

#include "ScriptBind_VehicleSeat.h"

extern std::uintptr_t CRYACTION_BASE;

ScriptBind_VehicleSeat::ScriptBind_VehicleSeat(ISystem* pSystem, IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = CRYACTION_BASE + 0x417b0;
#else
	std::uintptr_t ctor = CRYACTION_BASE + 0x2fef0;
#endif

	(this->*reinterpret_cast<void(ScriptBind_VehicleSeat::*&)(ISystem*, IGameFramework*)>(ctor))(pSystem, pGameFramework);
}

void ScriptBind_VehicleSeat::AttachTo(IVehicle* pVehicle, int seatId)
{
	IScriptTable* pScriptTable = pVehicle->GetEntity()->GetScriptTable();
	SmartScriptTable seatsTable;
	SmartScriptTable seatTable;
	if (!pScriptTable
	 || !pScriptTable->GetValue("Seats", seatsTable)
	 || !seatId
	 || !seatsTable->GetAt(seatId, seatTable))
	{
		return;
	}

	SmartScriptTable methodsTable(m_pSS);
	methodsTable->SetValue("vehicleId", ScriptHandle(pVehicle->GetEntityId()));
	methodsTable->SetValue("seatId", seatId);
	methodsTable->Delegate(this->GetMethodsTable());

	seatTable->SetValue("seat", methodsTable);
}
