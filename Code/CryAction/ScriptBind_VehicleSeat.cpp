#include "CryCommon/CryAction/IVehicleSystem.h"

#include "ScriptBind_VehicleSeat.h"

ScriptBind_VehicleSeat::ScriptBind_VehicleSeat(ISystem* pSystem, IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t ctor = 0x305417b0;
#else
	std::uintptr_t ctor = 0x3052fef0;
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
