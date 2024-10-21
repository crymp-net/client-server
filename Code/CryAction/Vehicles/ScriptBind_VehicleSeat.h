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
#ifndef __SCRIPTBIND_VEHICLESEAT_H__
#define __SCRIPTBIND_VEHICLESEAT_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <IScriptSystem.h>
#include <ScriptHelpers.h>

struct IVehicleSystem;
struct IGameFramework;
class CVehicleSeat;

// <title VehicleSeat>
// Syntax: VehicleSeat
class CScriptBind_VehicleSeat :
	public CScriptableBase
{
public:

	CScriptBind_VehicleSeat( ISystem *pSystem, IGameFramework *pGameFW );
	virtual ~CScriptBind_VehicleSeat();

	void AttachTo(IVehicle *pVehicle, TVehicleSeatId seatId);
	void Release() { delete this; };

	// <title GetVehicleSeat>
	// Syntax: VehicleSeat.GetVehicleSeat()
	// Description:
	//		Gets the vehicle seat identifier.
	CVehicleSeat* GetVehicleSeat(IFunctionHandler *pH);

	// <title Reset>
	// Syntax: VehicleSeat.Reset()
	// Description:
	//		Resets the vehicle seat.
	int Reset(IFunctionHandler *pH);

	// <title IsFree>
	// Syntax: VehicleSeat.IsFree()
	// Description:
	//		Checks if the seat is free.
	int IsFree(IFunctionHandler *pH);
	// <title SetPassenger>
	// Syntax: VehicleSeat.SetPassenger( ScriptHandle passengerHandle, bool isThirdPerson )
	// Arguments:
	//		passengerHandle - Passenger identifier.
	//		isThirdPerson	- True if we are using the third person view, false otherwise.
	// Description:
	//		Sets the specified passenger to the seat.
	int SetPassenger(IFunctionHandler* pH, ScriptHandle passengerHandle, bool isThirdPerson);
	// <title RemovePassenger>
	// Syntax: VehicleSeat.RemovePassenger()
	// Description:
	//		Removes the passenger from the seat.
	int RemovePassenger(IFunctionHandler* pH);
	// <title IsDriver>
	// Syntax: VehicleSeat.IsDriver()
	// Description:
	//		Checks if the seat is the driver seat.
	int IsDriver(IFunctionHandler* pH);
	// <title IsGunner>
	// Syntax: VehicleSeat.IsGunner()
	// Description:
	//		Checks if the seat is the gunner seat.
	int IsGunner(IFunctionHandler* pH);
	// <title IsLocked>
	// Syntax: VehicleSeat.IsLocked()
	// Description:
	//		Checks if the seat is locked.
	int IsLocked(IFunctionHandler* pH);

	// <title GetWeaponId>
	// Syntax: VehicleSeat.GetWeaponId(int weaponIndex)
	// Arguments:
	//		weaponIndex - Weapon identifier.
	// Description:
	//		Gets the weapon identifier.
	int GetWeaponId(IFunctionHandler* pH, int weaponIndex);
	// <title GetWeaponCount>
	// Syntax: VehicleSeat.GetWeaponCount()
	// Description:
	//		Gets the number of weapons available on this seat.	
	int GetWeaponCount(IFunctionHandler* pH);

	// <title SetAIWeapon>
	// Syntax: VehicleSeat.SetAIWeapon(ScriptHandle weaponHandle)
	// Arguments:
	//		weaponHandle - Weapon identifier.
	// Description:
	//		Sets the weapon artificial intelligence.
	int SetAIWeapon(IFunctionHandler* pH, ScriptHandle weaponHandle);

	// <title GetPassengerId>
	// Syntax: VehicleSeat.GetPassengerId()
	// Description:
	//		Gets the passenger identifier.
	int GetPassengerId(IFunctionHandler* pH);

private:

	void RegisterGlobals();
	void RegisterMethods();
	
	IVehicleSystem *m_pVehicleSystem;
};

#endif //__SCRIPTBIND_VEHICLESEAT_H__
