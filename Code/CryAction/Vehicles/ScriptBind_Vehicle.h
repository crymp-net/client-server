/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Script Binding for the Vehicle System

-------------------------------------------------------------------------
History:
- 05:10:2004   12:05 : Created by Mathieu Pinard

*************************************************************************/
#ifndef __SCRIPTBIND_VEHICLE_H__
#define __SCRIPTBIND_VEHICLE_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include "CryCommon/CryScriptSystem/IScriptSystem.h"

struct IVehicleSystem;
struct IGameFramework;
class CVehicle;

// <title VehicleSystem>
// Syntax: VehicleSystem
class CScriptBind_Vehicle :
	public CScriptableBase
{
public:
	CScriptBind_Vehicle( ISystem *pSystem, IGameFramework *pGameFW );
	virtual ~CScriptBind_Vehicle();

	void Release() { delete this; };

	void AttachTo(IVehicle *pVehicle);

	// <title GetVehicle>
	// Syntax: Vehicle.GetVehicle()
	// Description:
	//		Gets the vehicle identifier.
	CVehicle* GetVehicle(IFunctionHandler *pH);

	// <title Reset>
	// Syntax: Vehicle.Reset()
	// Description:
	//		Resets the vehicle.
	int Reset(IFunctionHandler *pH);

	// <title SetOwnerId>
	// Syntax: Vehicle.SetOwnerId( ScriptHandle ownerId )
	// Arguments:
	//		ownerId - Owner identifier.
	// Description:
	//		Sets the owner identifier for the vehicle.
	int SetOwnerId(IFunctionHandler *pH, ScriptHandle ownerId);
	// <title GetOwnerId>
	// Syntax: Vehicle.GetOwnerId()
	// Description:
	//		Gets the vehicle owner identifier.
	int GetOwnerId(IFunctionHandler *pH);

	// <title RefreshPhysicsGeometry>
	// Syntax: Vehicle.RefreshPhysicsGeometry( ScriptHandle partHandle, int slot, int physIndex )
	// Arguments:
	//		partHandle	- .
	//		physIndex	- .
	// Description:
	//		Refreshes the physics geometry.
	int RefreshPhysicsGeometry(IFunctionHandler *pH, ScriptHandle partHandle, int slot, int physIndex);

	// <title GetVehiclePhysicsStatus>
	// Syntax: Vehicle.GetVehiclePhysicsStatus( SmartScriptTable statusTable )
	// Arguments:
	//		statusTable - Status table of the vehicle.
	// Description:
	//		Gets the vehicle physics status.
	int GetVehiclePhysicsStatus(IFunctionHandler *pH, SmartScriptTable statusTable);	
	// <title SetPlayerToSit>
	// Syntax: Vehicle.SetPlayerToSit( ScriptHandle playerId, int flags )
	// Arguments:
	//		playerId - Player identifier.
	//		flags	 - Flag to identify where the player is.
	// Description:
	//		Sets the player sitting into the vehicle.
	int SetPlayerToSit(IFunctionHandler *pH, ScriptHandle playerId, int flags);	
	
	// <title IsPartInsideRadius>
	// Syntax: Vehicle.IsPartInsideRadius( int slot, Vec3 pos, float radius )
	// Arguments:
	//		slot	- .
	//		pos		- .
	//		radius	- .
	// Description:
	//		Checks if a part is inside the specified radius.
	int IsPartInsideRadius(IFunctionHandler *pH, int slot, Vec3 pos, float radius);
	// <title IsInsideRadius>
	// Syntax: Vehicle.IsInsideRadius( Vec3 pos, float radius )
	// Description:
	//		Checks if the vehicle is inside the specified radius.
	int IsInsideRadius(IFunctionHandler *pH, Vec3 pos, float radius);
	// <title IsEmpty>
	// Syntax: Vehicle.IsEmpty()
	// Description:
	//		Checks if the vehicle is empty.
	int IsEmpty(IFunctionHandler *pH);

	// <title GetRepairableDamage>
	// Syntax: Vehicle.GetRepairableDamage()
	int GetRepairableDamage(IFunctionHandler *pH);

	// <title StartAbandonTimer>
	// Syntax: Vehicle.StartAbandonTimer( bool force, float timer)
	// Arguments:
	//		force - True to force the abandon, false otherwise.
	//		timer - Timer value.
	// Description:
	//		Starts the abandon timer.
	int StartAbandonTimer(IFunctionHandler *pH);
	// <title KillAbandonTimer>
	// Syntax: Vehicle.KillAbandonTimer()
	// Description:
	//		Kills the abandon timer.
	int KillAbandonTimer(IFunctionHandler *pH);
	// <title Destroy>
	// Syntax: Vehicle.Destroy()
	// Description:
	//		Destroys the vehicle.
	int Destroy(IFunctionHandler *pH);

	// <title MultiplyWithEntityLocalTM>
	// Syntax: Vehicle.MultiplyWithEntityLocalTM( ScriptHandle entityHandle, Vec3 pos )
	// Arguments:
	//		entityHandle	- Entity identifier.
	//		pos				- Position vector.
	// Description:
	//		Multiplies with the entity local transformation matrix.
	int MultiplyWithEntityLocalTM(IFunctionHandler *pH, ScriptHandle entityHandle, Vec3 pos);
	// <title MultiplyWithWorldTM>
	// Syntax: Vehicle.MultiplyWithWorldTM( Vec3 pos )
	// Arguments:
	//		pos				- Position vector.
	// Description:
	//		Multiplies with the world transformation matrix.
	int MultiplyWithWorldTM(IFunctionHandler *pH, Vec3 pos);
	// <title UpdateVehicleAnimation>
	// Syntax: Vehicle.UpdateVehicleAnimation( ScriptHandle entity, int slot )
	// Arguments:
	//		entity	- Entity identifier.
	//		slot	- Slot number.
	// Description:
	//		Updates the vehicle animation.
	int UpdateVehicleAnimation(IFunctionHandler *pH, ScriptHandle entity, int slot);
	// <title ResetSlotGeometry>
	// Syntax: Vehicle.ResetSlotGeometry( int slot, const char* filename, const char* geometry )
	int ResetSlotGeometry(IFunctionHandler* pH, int slot, const char* filename, const char* geometry);
  
	// <title AddSeat>
	// Syntax: Vehicle.AddSeat( SmartScriptTable paramsTable )
	// Arguments:
	//		paramsTable - Seat parameters.
	// Description:
	//		Adds a seat to the vehicle.
	int AddSeat(IFunctionHandler* pH, SmartScriptTable paramsTable);

	// <title HasHelper>
	// Syntax: Vehicle.HasHelper(const char* name)
	// Arguments:
	//		name - Helper name.
	// Description:
	//		Checks if the vehicle has the specified helper.
	int HasHelper(IFunctionHandler* pH, const char* name);
	// <title GetHelperPos>
	// Syntax: Vehicle.GetHelperPos( const char* name, bool isInVehicleSpace )
	// Arguments:
	//		name				- Helper name.
	//		isInVehicleSpace	- . 
	// Description:
	//		Gets the helper position.
	int GetHelperPos(IFunctionHandler* pH, const char* name, bool isInVehicleSpace);
	// <title GetHelperDir>
	// Syntax: Vehicle.GetHelperDir( const char* name, bool isInVehicleSpace )
	// Arguments:
	//		name				- Helper name.
	//		isInVehicleSpace	- . 
	// Description:
	//		Gets the helper direction.
	int GetHelperDir(IFunctionHandler* pH, const char* name, bool isInVehicleSpace);
	// <title GetHelperWorldPos>
	// Syntax: Vehicle.GetHelperWorldPos( const char* name )
	// Arguments:
	//		name				- Helper name.
	// Description:
	//		Gets the helper position in the world coordinates.
	int GetHelperWorldPos(IFunctionHandler* pH, const char* name);

	// <title SetExtensionActivation>
	// Syntax: Vehicle.SetExtensionActivation( const char *extension, bool bActivated )
	// Arguments:
	//		extension - Extension name.
	//		bActivated - True to activate the extension, false to deactivate it.
	// Description:
	//		Activates/deactivates the extension.
	int SetExtensionActivation(IFunctionHandler* pH, const char *extension, bool bActivated);
	// <title SetExtensionParams>
	// Syntax: Vehicle.SetExtensionParams( const char *extension, SmartScriptTable params )
	// Arguments:
	//		extension - Extension name.
	//		params	  - Extension parameters
	// Description:
	//		Sets extension parameters.
	int SetExtensionParams(IFunctionHandler* pH, const char *extension, SmartScriptTable params);
	// <title GetExtensionParams>
	// Syntax: Vehicle.GetExtensionParams( const char *extension, SmartScriptTable params )
	// Arguments:
	//		extension - Extension name.
	//		params	  - Extension parameters
	// Description:
	//		Gets extension parameters.
	int GetExtensionParams(IFunctionHandler* pH, const char *extension, SmartScriptTable params);

	// <title EnableMovement>
	// Syntax: Vehicle.EnableMovement( bool enable )
	// Arguments:
	//		enable - True to enable movement, false to disable.
	// Description:
	//		Enables/disables the movement of the vehicle.
	int EnableMovement(IFunctionHandler *pH, bool enable);
  
	// <title DisableEngine>
	// Syntax: Vehicle.DisableEngine( bool disable )
	// Arguments:
	//		disable - True to disable the engine, false to enable.
	// Description:
	//		Disables/enables the engine of the vehicle.
	int DisableEngine(IFunctionHandler *pH, bool disable);

	// <title OnHit>
	// Syntax: Vehicle.OnHit( ScriptHandle targetId, ScriptHandle shooterId, float damage, Vec3 position, float radius, char* pHitClass, bool explosion )
	// Arguments:
	//		targetId	- Target identifier.
	//		shooterId	- Shooter identifier.
	//		damage		- Damage amount.
	//		radius		- Radius of the hit.
	//		pHitClass	- Hit class.
	//		explosion	- True if the hit cause an explosion, false otherwise.
	// Description:
	//		Event that occurs after the vehicle is hit.
	int OnHit(IFunctionHandler* pH, ScriptHandle targetId, ScriptHandle shooterId, float damage, Vec3 position, float radius, char* pHitClass, bool explosion);
	
	// <title ProcessPassengerDamage>
	// Syntax: Vehicle.ProcessPassengerDamage( ScriptHandle passengerId, float actorHealth, float damage, const char* pDamageClass, bool explosion )
	// Arguments:
	//		passengerId		- Passenger identifier.
	//		actorHealt		- Actor healt amount.
	//		damage			- Damage amount.
	//		pDamageClass	- Damage class.
	//		explosion		- True if there is an explosion, false otherwise.
	// Description:
	//		Processes passenger damages.
	int ProcessPassengerDamage(IFunctionHandler* pH, ScriptHandle passengerId, float actorHealth, float damage, const char* pDamageClass, bool explosion);
	// <title OnPassengerKilled>
	// Syntax: Vehicle.OnPassengerKilled( ScriptHandle passengerHandle )
	// Arguments:
	//		passengerHandle - Passenger identifier.
	// Description:
	//		Event that occurs if the passenger is killed.
	int OnPassengerKilled(IFunctionHandler* pH, ScriptHandle passengerHandle);
	// <title IsDestroyed>
	// Syntax: Vehicle.IsDestroyed()
	// Description:
	//		Checks if the vehicle is destroyed.
	int IsDestroyed(IFunctionHandler* pH);
	// <title IsFlipped>
	// Syntax: Vehicle.IsFlipped()
	// Description:
	//		Checks if the vehicle is flipped.
	int IsFlipped(IFunctionHandler* pH);
	// <title IsSubmerged>
	// Syntax: Vehicle.IsSubmerged()
	// Description:
	//		Checks if the vehicle is submerged.
	int IsSubmerged(IFunctionHandler* pH);

	// <title IsUsable>
	// Syntax: Vehicle.IsUsable( ScriptHandle userHandle )
	// Arguments:
	//		userHandle - User identifier.
	// Description:
	//		Checks if the vehicle is usable by the user.
	int IsUsable(IFunctionHandler* pH, ScriptHandle userHandle);
	// <title OnUsed>
	// Syntax: Vehicle.OnUsed( ScriptHandle userHandle, int index )
	// Arguments:
	//		userHandle	- User identifier.
	//		index		- Seat identifier.
	// Description:
	//		Events that occurs when the user uses the vehicle.
	int OnUsed(IFunctionHandler* pH, ScriptHandle userHandle, int index);

	// <title EnterVehicle>
	// Syntax: Vehicle.EnterVehicle(  ScriptHandle actorHandle, int seatId, bool isAnimationEnabled )
	// Arguments:
	//		actorHandle - Actor identifier.
	//		seatId		- Seat identifier.
	//		isAnimationEnabled - True to enable the animation, false otherwise.
	// Description:
	//		Makes the actor entering the vehicle.
	int EnterVehicle(IFunctionHandler* pH, ScriptHandle actorHandle, int seatId, bool isAnimationEnabled);
	// <title ChangeSeat>
	// Syntax: Vehicle.ChangeSeat(ScriptHandle actorHandle, int seatId, bool isAnimationEnabled )
	// Arguments:
	//		actorHandle - Actor identifier.
	//		seatId		- Seat identifier.
	//		isAnimationEnabled - True to enable the animation, false otherwise.
	// Description:
	//		Makes the actor changing the seat inside the vehicle.
	int ChangeSeat(IFunctionHandler* pH, ScriptHandle actorHandle, int seatId, bool isAnimationEnabled);
	// <title ExitVehicle>
	// Syntax: Vehicle.ExitVehicle( criptHandle actorHandle )
	// Arguments:
	//		actorHandle - Actor identifier.
	// Description:
	//		Makes the actor going out from the vehicle.
	int ExitVehicle(IFunctionHandler* pH, ScriptHandle actorHandle);

	// <title GetComponentDamageRatio>
	// Syntax: Vehicle.GetComponentDamageRatio( const char* pComponentName )
	// Description:
	//		Gets the damage ratio of the specified component.
	int GetComponentDamageRatio(IFunctionHandler* pH, const char* pComponentName);
	// <title GetCollisionDamageThreshold>
	// Syntax: Vehicle.GetCollisionDamageThreshold()
	// Arguments:
	//		pComponentName - Name of the component.
	// Description:
	//		Gets the collision damage threshold.
	int GetCollisionDamageThreshold(IFunctionHandler* pH);  
	// <title GetSelfCollisionMult>
	// Syntax: Vehicle.GetSelfCollisionMult( Vec3 velocity, Vec3 normal, int partId, ScriptHandle colliderId )
	// Arguments:
	//		velocity	- Velocity vector.
	//		normal		- Normal vector.
	//		partId		- Part identifier.
	//		colliderId	- Collider identifier.
	// Description:
	//		Gets self collision mult.
	int GetSelfCollisionMult(IFunctionHandler* pH, Vec3 velocity, Vec3 normal, int partId, ScriptHandle colliderId);
	// <title GetMovementDamageRatio>
	// Syntax: Vehicle.GetMovementDamageRatio()
	// Description:
	//		Gets the movement damage ratio.
	int GetMovementDamageRatio(IFunctionHandler* pH);
	// <title SetMovementMode>
	// Syntax: Vehicle.SetMovementMode( int mode )
	// Arguments:
	//		mode - Movement mode.
	// Description:
	//		Sets the movement mode.
	int SetMovementMode(IFunctionHandler* pH, int mode);
	// <title GetMovementType>
	// Syntax: Vehicle.GetMovementType()
	// Description:
	//		Gets the movement type.
	int GetMovementType(IFunctionHandler* pH);

	// <title SetMovementSoundVolume>
	// Syntax: Vehicle.SetMovementSoundVolume( float volume )
	// Arguments:
	//		volume - Sound volume for the movement.
	// Description:
	//		Sets the sound volume for the movement.
	int SetMovementSoundVolume(IFunctionHandler *pH, float volume);

	// <title SetAmmoCount>
	// Syntax: Vehicle.SetAmmoCount( const char *name, int amount )
	// Arguments:
	//		name	- Ammunition name.
	//		amount	- Amount of the ammunition.
	// Description:
	//		Sets the amount of the specified ammunition.
	int SetAmmoCount(IFunctionHandler *pH, const char *name, int amount);
  
	// <title GetFrozenAmount>
	// Syntax: Vehicle.GetFrozenAmount()
	// Description:
	//		Gets the frozen amount.
	int GetFrozenAmount(IFunctionHandler *pH);
	// <title SetFrozenAmount>
	// Syntax: Vehicle.SetFrozenAmount( float frost )
	// Arguments:
	//		frost - Frost amount.
	// Description:
	//		Sets the frozen amount.
	int SetFrozenAmount(IFunctionHandler *pH, float frost);

	// <title OpenAutomaticDoors>
	// Syntax: Vehicle.OpenAutomaticDoors()
	// Description:
	//		Broadcast to the vehicle the message to automatically open the doors.
	int OpenAutomaticDoors(IFunctionHandler *pH);
	// <title CloseAutomaticDoors>
	// Syntax: Vehicle.CloseAutomaticDoors()
	// Description:
	//		Broadcast to the vehicle the message to automatically close the doors.
	int CloseAutomaticDoors(IFunctionHandler *pH);
	// <title BlockAutomaticDoors>
	// Syntax: Vehicle.BlockAutomaticDoors()
	// Description:
	//		Broadcast to the vehicle the message to automatically block the doors.
	int BlockAutomaticDoors(IFunctionHandler *pH, bool isBlocked);
	// <title ExtractGears>
	// Syntax: Vehicle.ExtractGears()
	// Description:
	//		Broadcast to the vehicle the message to extract gears.
	int ExtractGears(IFunctionHandler *pH);
	// <title RetractGears>
	// Syntax: Vehicle.RetractGears()
	// Description:
	//		Broadcast to the vehicle the message to retract gears.
	int RetractGears(IFunctionHandler *pH);

private:
	void RegisterGlobals();
	void RegisterMethods();
  	
	IGameFramework *m_pGameFW;  
	IVehicleSystem *m_pVehicleSystem;
};

#endif //__SCRIPTBIND_VEHICLE_H__
