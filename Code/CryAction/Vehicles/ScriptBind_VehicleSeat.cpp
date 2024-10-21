/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Script Binding for the Vehicle Seat

-------------------------------------------------------------------------
History:
- 28:04:2004   17:02 : Created by Mathieu Pinard

*************************************************************************/
#include "StdAfx.h"
#include <ISound.h>
#include "CryString.h"
#include <IActionMapManager.h>
#include <ICryAnimation.h>
#include "IGameFramework.h"
#include "IActorSystem.h"
#include <vector>
#include <Cry_Math.h>
#include <IShader.h>
#include <IRenderAuxGeom.h>
#include <Cry_GeoOverlap.h>

#include "VehicleSystem.h"
#include "VehicleSeat.h"
#include "ScriptBind_VehicleSeat.h"
#include "Vehicle.h"
#include "VehicleSeatActionWeapons.h"


//------------------------------------------------------------------------
// macro for retrieving vehicle and entity
#undef GET_ENTITY
#define GET_ENTITY IVehicle* pVehicle = GetVehicle(pH); CRY_ASSERT(pVehicle); \
	IEntity* pEntity = pVehicle->GetEntity(); \
	if (!pEntity) return pH->EndFunction();

//------------------------------------------------------------------------
CScriptBind_VehicleSeat::CScriptBind_VehicleSeat(ISystem *pSystem, IGameFramework *pGameFW)
{	
	m_pVehicleSystem = pGameFW->GetIVehicleSystem();
	
	Init(gEnv->pScriptSystem, gEnv->pSystem, 1);

	RegisterMethods();
	RegisterGlobals();
}

//------------------------------------------------------------------------
CScriptBind_VehicleSeat::~CScriptBind_VehicleSeat()
{  
}

//------------------------------------------------------------------------
void CScriptBind_VehicleSeat::RegisterGlobals()
{
}

//------------------------------------------------------------------------
void CScriptBind_VehicleSeat::RegisterMethods()
{
#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &CScriptBind_VehicleSeat::

	SCRIPT_REG_TEMPLFUNC(Reset, "");
	SCRIPT_REG_TEMPLFUNC(SetPassenger, "passengerId, thirdPerson");
	SCRIPT_REG_TEMPLFUNC(RemovePassenger, "");
  SCRIPT_REG_TEMPLFUNC(IsFree, "");
	SCRIPT_REG_TEMPLFUNC(IsDriver, "");
	SCRIPT_REG_TEMPLFUNC(IsGunner, "");
	SCRIPT_REG_TEMPLFUNC(IsLocked, "");
	SCRIPT_REG_TEMPLFUNC(GetWeaponCount, "");
	SCRIPT_REG_TEMPLFUNC(GetWeaponId, "index");
	SCRIPT_REG_TEMPLFUNC(SetAIWeapon, "weaponId");
	SCRIPT_REG_TEMPLFUNC(GetPassengerId, "");
}

//------------------------------------------------------------------------
void CScriptBind_VehicleSeat::AttachTo(IVehicle *pVehicle, TVehicleSeatId seatId)
{
  IScriptTable *pScriptTable = pVehicle->GetEntity()->GetScriptTable();

  if (!pScriptTable || seatId == InvalidVehicleSeatId)
    return;

  SmartScriptTable seatsTable;
	if (!pScriptTable->GetValue("Seats", seatsTable))
	{
		CRY_ASSERT(!"cannot read the seats table");
		return;
	}

	SmartScriptTable seatTable(gEnv->pScriptSystem);

	SmartScriptTable thisTable(gEnv->pScriptSystem);
	thisTable->SetValue("vehicleId", ScriptHandle(pVehicle->GetEntityId())); 
	thisTable->SetValue("seatId", (int)seatId); 

	thisTable->Delegate(GetMethodsTable());
	seatTable->SetValue("seat", thisTable);
	
	if (IVehicleSeat* pSeat = pVehicle->GetSeatById(seatId))
		seatTable->SetValue("isDriver", pSeat->IsDriver());
	
	seatsTable->SetAt(seatId, seatTable);
}

//------------------------------------------------------------------------
CVehicleSeat* CScriptBind_VehicleSeat::GetVehicleSeat(IFunctionHandler *pH)
{
  ScriptHandle handle;
  int seatId = 0;
    
  SmartScriptTable table;
  if (pH->GetSelf(table))
  {
    if (table->GetValue("vehicleId", handle) && table->GetValue("seatId", seatId))
    { 
      CVehicle* pVehicle = (CVehicle*)m_pVehicleSystem->GetVehicle((EntityId)handle.n);

      if (pVehicle)
      {
        return (CVehicleSeat*)pVehicle->GetSeatById((TVehicleSeatId)seatId);
      }
    }
  }

	return 0;
}

//------------------------------------------------------------------------
int CScriptBind_VehicleSeat::Reset(IFunctionHandler *pH)
{
	CVehicleSeat* pVehicleSeat = GetVehicleSeat(pH);
	
	if (pVehicleSeat)
		pVehicleSeat->Reset();
	
	return pH->EndFunction();
}

//------------------------------------------------------------------------
int CScriptBind_VehicleSeat::SetPassenger(IFunctionHandler* pH, ScriptHandle passengerHandle, bool isThirdPerson)
{
	CVehicleSeat* pVehicleSeat = GetVehicleSeat(pH);

	pVehicleSeat->Enter((EntityId)passengerHandle.n);
	return pH->EndFunction();
}

//------------------------------------------------------------------------
int CScriptBind_VehicleSeat::RemovePassenger(IFunctionHandler* pH)
{
	CVehicleSeat* pVehicleSeat = GetVehicleSeat(pH);

	pVehicleSeat->Exit(true);
	return pH->EndFunction();
}

//------------------------------------------------------------------------
int CScriptBind_VehicleSeat::GetWeaponId(IFunctionHandler* pH, int weaponIndex)
{
	// returns Weapon Id 02/11/05 Tetsuji
	CVehicleSeat* pVehicleSeat = GetVehicleSeat(pH);
	
	int weaponCounter = 1;

	if (pVehicleSeat)
	{
		TVehicleSeatActionVector& seatActions = pVehicleSeat->GetSeatActions();

		for (TVehicleSeatActionVector::iterator ite = seatActions.begin(); ite != seatActions.end(); ++ite)
		{
			IVehicleSeatAction* pSeatAction = ite->pSeatAction;
			if (CVehicleSeatActionWeapons* pSeatWeapons = CAST_VEHICLEOBJECT(CVehicleSeatActionWeapons, pSeatAction))
			{
				if ((weaponCounter == weaponIndex) && (pSeatWeapons->GetWeaponCount() > 0))
				{
					ScriptHandle weaponHandle;
					weaponHandle.n = pSeatWeapons->GetWeaponEntityId(0);
					return pH->EndFunction(weaponHandle);
				}

				weaponCounter++;
			}
		}
	}

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int CScriptBind_VehicleSeat::GetWeaponCount(IFunctionHandler* pH)
{
	CVehicleSeat* pVehicleSeat = GetVehicleSeat(pH);
	int weaponCount = 0;
	
	if (pVehicleSeat)
	{
		TVehicleSeatActionVector& seatActions = pVehicleSeat->GetSeatActions();

		for (TVehicleSeatActionVector::iterator ite = seatActions.begin(); ite != seatActions.end(); ++ite)
		{
			IVehicleSeatAction* pSeatAction = ite->pSeatAction;
			if (CAST_VEHICLEOBJECT(CVehicleSeatActionWeapons, pSeatAction))
				weaponCount++;
		}
	}

	return pH->EndFunction(weaponCount);
}

//------------------------------------------------------------------------
int CScriptBind_VehicleSeat::SetAIWeapon(IFunctionHandler* pH, ScriptHandle weaponHandle)
{
	if (CVehicleSeat* pVehicleSeat = GetVehicleSeat(pH))
	{
		pVehicleSeat->m_aiWeaponId = (EntityId)weaponHandle.n;
		return pH->EndFunction(true);
	}

	return pH->EndFunction(false);
}

//------------------------------------------------------------------------
int CScriptBind_VehicleSeat::IsFree(IFunctionHandler* pH)
{
  if (CVehicleSeat* pVehicleSeat = GetVehicleSeat(pH))
  {
    return pH->EndFunction(pVehicleSeat->IsFree());
  }

  return pH->EndFunction(false);
}

//------------------------------------------------------------------------
int CScriptBind_VehicleSeat::IsDriver(IFunctionHandler* pH)
{
	if (CVehicleSeat* pVehicleSeat = GetVehicleSeat(pH))
	{
		return pH->EndFunction(pVehicleSeat->IsDriver());
	}

	return pH->EndFunction(false);
}

//------------------------------------------------------------------------
int CScriptBind_VehicleSeat::IsGunner(IFunctionHandler* pH)
{
	if (CVehicleSeat* pVehicleSeat = GetVehicleSeat(pH))
	{
		return pH->EndFunction(pVehicleSeat->IsGunner());
	}

	return pH->EndFunction(false);
}

//------------------------------------------------------------------------
int CScriptBind_VehicleSeat::IsLocked(IFunctionHandler* pH)
{
	if (CVehicleSeat* pVehicleSeat = GetVehicleSeat(pH))
	{
		return pH->EndFunction(pVehicleSeat->IsLocked());
	}

	return pH->EndFunction(false);
}

//------------------------------------------------------------------------
int CScriptBind_VehicleSeat::GetPassengerId(IFunctionHandler* pH)
{
	if (CVehicleSeat* pVehicleSeat = GetVehicleSeat(pH))
	{
		return pH->EndFunction( ScriptHandle(pVehicleSeat->GetPassenger()) );
	}

	return pH->EndFunction();
}
