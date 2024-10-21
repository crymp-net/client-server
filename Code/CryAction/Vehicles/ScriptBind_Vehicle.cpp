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
#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySoundSystem/ISound.h"
#include "CryCommon/CryAction/IActionMapManager.h"
#include "CryCommon/CryAnimation/ICryAnimation.h"
#include "CryCommon/CryAction/IGameFramework.h"
#include "CryCommon/CryAction/IActorSystem.h"
#include <vector>
#include "CryCommon/CryRenderer/IRenderAuxGeom.h"
#include "CryCommon/CryMath/Cry_GeoOverlap.h"

#include "VehicleSystem.h"
#include "Vehicle.h"
#include "VehicleComponent.h"
#include "VehicleSeat.h"
#include "ScriptBind_Vehicle.h"
#include "VehicleDamages.h"
#include "CryCommon/CryAction/IGameObject.h"


//------------------------------------------------------------------------
// macro for retrieving vehicle and entity
#undef GET_ENTITY
#define GET_ENTITY IVehicle* pVehicle = GetVehicle(pH); \
  IEntity* pEntity = pVehicle ? pVehicle->GetEntity() : NULL; \
  if (!pEntity) return pH->EndFunction();

//------------------------------------------------------------------------
ScriptBind_Vehicle::ScriptBind_Vehicle(ISystem* pSystem, IGameFramework* pGameFW)
{
	m_pVehicleSystem = pGameFW->GetIVehicleSystem();

	Init(gEnv->pScriptSystem, gEnv->pSystem, 1);

	//CScriptableBase::Init(m_pSS, pSystem);
	//SetGlobalName("vehicle");

	RegisterMethods();

	RegisterGlobal("SOUND_EVENT_ON_PLAY", SOUND_EVENT_ON_START);
	RegisterGlobal("SOUND_EVENT_ON_STOP", SOUND_EVENT_ON_STOP);
}

//------------------------------------------------------------------------
void ScriptBind_Vehicle::RegisterMethods()
{
#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &ScriptBind_Vehicle::

	SCRIPT_REG_TEMPLFUNC(SetOwnerId, "ownerId");
	SCRIPT_REG_TEMPLFUNC(GetOwnerId, "");

	SCRIPT_REG_TEMPLFUNC(GetVehiclePhysicsStatus, "statusTable");
	SCRIPT_REG_TEMPLFUNC(SetPlayerToSit, "playerId, flags");

	SCRIPT_REG_TEMPLFUNC(IsPartInsideRadius, "slot, pos, radius");
	SCRIPT_REG_TEMPLFUNC(IsInsideRadius, "pos, radius");
	SCRIPT_REG_TEMPLFUNC(IsEmpty, "");

	SCRIPT_REG_TEMPLFUNC(GetRepairableDamage, "");

	SCRIPT_REG_TEMPLFUNC(StartAbandonTimer, "[force, [timer]");
	SCRIPT_REG_TEMPLFUNC(KillAbandonTimer, "");
	SCRIPT_REG_TEMPLFUNC(Destroy, "");

	SCRIPT_REG_TEMPLFUNC(MultiplyWithEntityLocalTM, "entityID, entity, pos");
	SCRIPT_REG_TEMPLFUNC(MultiplyWithWorldTM, "pos");

	SCRIPT_REG_TEMPLFUNC(UpdateVehicleAnimation, "entity, slot");
	SCRIPT_REG_TEMPLFUNC(RefreshPhysicsGeometry, "part, slot, physIndex");
	SCRIPT_REG_TEMPLFUNC(AddSeat, "params");
	SCRIPT_REG_TEMPLFUNC(SetExtensionActivation, "name, bActivated");
	SCRIPT_REG_TEMPLFUNC(SetExtensionParams, "extension,params");
	SCRIPT_REG_TEMPLFUNC(GetExtensionParams, "extension,params");

	SCRIPT_REG_TEMPLFUNC(HasHelper, "name");
	SCRIPT_REG_TEMPLFUNC(GetHelperPos, "name, isVehicleSpace");
	SCRIPT_REG_TEMPLFUNC(GetHelperDir, "name, isVehicleSpace");

	SCRIPT_REG_TEMPLFUNC(GetHelperWorldPos, "name");

	SCRIPT_REG_TEMPLFUNC(EnableMovement, "enable");
	SCRIPT_REG_TEMPLFUNC(DisableEngine, "disable");

	SCRIPT_REG_TEMPLFUNC(OnHit, "shooterId, damage, position, radius, hitClass, explosion, frost");
	SCRIPT_REG_TEMPLFUNC(ProcessPassengerDamage, "passengerHandle, actorHealth, damage, pDamageClass, explosion");
	SCRIPT_REG_TEMPLFUNC(OnPassengerKilled, "passengerHandle");
	SCRIPT_REG_TEMPLFUNC(IsDestroyed, "");
	SCRIPT_REG_TEMPLFUNC(IsFlipped, "");
	SCRIPT_REG_TEMPLFUNC(IsSubmerged, "");

	SCRIPT_REG_TEMPLFUNC(IsUsable, "userId");
	SCRIPT_REG_TEMPLFUNC(OnUsed, "userId, index");

	SCRIPT_REG_TEMPLFUNC(EnterVehicle, "actorId, seatIndex, isAnimationEnabled");
	SCRIPT_REG_TEMPLFUNC(ChangeSeat, "actorId, seatIndex, isAnimationEnabled");
	SCRIPT_REG_TEMPLFUNC(ExitVehicle, "actorId");

	SCRIPT_REG_TEMPLFUNC(GetComponentDamageRatio, "componentName");
	SCRIPT_REG_TEMPLFUNC(GetCollisionDamageThreshold, "");
	SCRIPT_REG_TEMPLFUNC(GetSelfCollisionMult, "velocity, normal, partId, colliderId");
	SCRIPT_REG_TEMPLFUNC(GetMovementDamageRatio, "");
	SCRIPT_REG_TEMPLFUNC(SetMovementSoundVolume, "volume");

	SCRIPT_REG_TEMPLFUNC(SetAmmoCount, "name, amount");

	SCRIPT_REG_TEMPLFUNC(SetMovementMode, "mode");
	SCRIPT_REG_TEMPLFUNC(GetMovementType, "");

	SCRIPT_REG_TEMPLFUNC(GetFrozenAmount, "");
	SCRIPT_REG_TEMPLFUNC(SetFrozenAmount, "frost");

	SCRIPT_REG_TEMPLFUNC(OpenAutomaticDoors, "");
	SCRIPT_REG_TEMPLFUNC(CloseAutomaticDoors, "");
	SCRIPT_REG_TEMPLFUNC(BlockAutomaticDoors, "isBlocked");

	SCRIPT_REG_TEMPLFUNC(ExtractGears, "");
	SCRIPT_REG_TEMPLFUNC(RetractGears, "");
}

//------------------------------------------------------------------------
void ScriptBind_Vehicle::AttachTo(IVehicle* pVehicle)
{
	IScriptTable* pScriptTable = pVehicle->GetEntity()->GetScriptTable();

	if (!pScriptTable)
		return;

	SmartScriptTable thisTable(gEnv->pScriptSystem);
	thisTable->SetValue("vehicleId", ScriptHandle(pVehicle->GetEntityId()));
	thisTable->Delegate(GetMethodsTable());
	pScriptTable->SetValue("vehicle", thisTable);

	SmartScriptTable seatTable(gEnv->pScriptSystem);
	pScriptTable->SetValue("Seats", seatTable);
}

//------------------------------------------------------------------------
CVehicle* ScriptBind_Vehicle::GetVehicle(IFunctionHandler* pH)
{
	ScriptHandle handle;
	SmartScriptTable table;

	if (pH->GetSelf(table))
	{
		if (table->GetValue("vehicleId", handle))
		{
			return (CVehicle*)m_pVehicleSystem->GetVehicle((EntityId)handle.n);
		}
	}

	return 0;
}

//------------------------------------------------------------------------
ScriptBind_Vehicle::~ScriptBind_Vehicle()
{
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::Reset(IFunctionHandler* pH)
{
	IVehicle* pVehicle = GetVehicle(pH);

	if (pVehicle)
		pVehicle->Reset(0);

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::SetOwnerId(IFunctionHandler* pH, ScriptHandle ownerId)
{
	IVehicle* pVehicle = GetVehicle(pH);

	if (pVehicle)
		pVehicle->SetOwnerId((EntityId)ownerId.n);

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::GetOwnerId(IFunctionHandler* pH)
{
	IVehicle* pVehicle = GetVehicle(pH);

	if (pVehicle && pVehicle->GetOwnerId())
		return pH->EndFunction(ScriptHandle(pVehicle->GetOwnerId()));

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::GetVehiclePhysicsStatus(IFunctionHandler* pH, SmartScriptTable statusTable)
{
	IVehicle* vehicle = GetVehicle(pH);
	if (!vehicle)
		return pH->EndFunction();

	IEntity* vehicleEntity = vehicle->GetEntity();
	IPhysicalEntity* physics = vehicleEntity->GetPhysics();

	if (!physics)
		return pH->EndFunction();

	// query dynamic info  
	pe_status_dynamics statusDyn;
	if (physics->GetStatus(&statusDyn) == 0)
		return pH->EndFunction();

	static Vec3 w;
	w = vehicleEntity->GetRotation() * statusDyn.w;

	// query contact info    
	pe_status_collisions collisions;
	coll_history_item item;
	collisions.pHistory = &item;
	collisions.len = 1;

	CScriptSetGetChain statusChain(statusTable);
	{
		statusChain.SetValue("velocity", statusDyn.v);
		statusChain.SetValue("velocityMod", statusDyn.v.len());
		statusChain.SetValue("angularVelZ", w.z);
		statusChain.SetValue("mass", statusDyn.mass);
		statusChain.SetValue("submergedFraction", statusDyn.submergedFraction);

		statusChain.SetValue("chassisContact", physics->GetStatus(&collisions));
	}

	// if we have a wheeledvehicle, add its status info
	pe_status_vehicle vehicleStatus;
	if (physics->GetStatus(&vehicleStatus))
	{
		CScriptSetGetChain chain(statusTable);
		{
			chain.SetValue("clutch", vehicleStatus.clutch);
			chain.SetValue("currentGear", vehicleStatus.iCurGear);
			chain.SetValue("footBrake", vehicleStatus.footbrake);
			chain.SetValue("handBrake", vehicleStatus.bHandBrake);
			chain.SetValue("pedal", vehicleStatus.pedal);
			chain.SetValue("steer", vehicleStatus.steer);
			chain.SetValue("wheelContact", vehicleStatus.bWheelContact);
			chain.SetValue("engineRPM", vehicleStatus.engineRPM);
		}
	}

	return pH->EndFunction();
}


//------------------------------------------------------------------------
int ScriptBind_Vehicle::SetPlayerToSit(IFunctionHandler* pH, ScriptHandle playerId, int flags)
{
	IVehicle* vehicle = GetVehicle(pH);
	if (!vehicle)
		return pH->EndFunction();

	IEntity* vehicleEntity = vehicle->GetEntity();

	IEntity* playerEntity = gEnv->pEntitySystem->GetEntity((EntityId)playerId.n);
	if (playerEntity == NULL)
		return pH->EndFunction();

	if (flags == 1)
	{
		//CryMP: Fixme
		//gEnv->pGame->GetIGameFramework()->GetGameObject((EntityId)playerId.n)->OnAction("UseVehicle", (int)vehicleEntity->GetId(), 0.0f);
	}
	else
	{
		//gEnv->pGame->GetIGameFramework()->GetGameObject((EntityId)playerId.n)->OnAction("UseVehicle", 0, 0.0f);
	}

	return pH->EndFunction();
}


//------------------------------------------------------------------------
int ScriptBind_Vehicle::IsPartInsideRadius(IFunctionHandler* pH, int slot, Vec3 pos, float radius)
{
	IVehicle* vehicle = GetVehicle(pH);

	if (!vehicle)
		return pH->EndFunction();

	AABB boundingBox;
	IEntity* vehicleEntity = vehicle->GetEntity();

	IStatObj* pObj = vehicleEntity->GetStatObj(slot);
	if (pObj)
	{
		boundingBox = pObj->GetAABB();
	}
	else
	{
		ICharacterInstance* pChar = vehicleEntity->GetCharacter(0);
		if (pChar)
		{
			boundingBox = pChar->GetAABB();
		}
		else
		{
			return pH->EndFunction(false);
		}
	}

	Sphere sphere(vehicleEntity->GetWorldPos() - pos, radius);
	return pH->EndFunction(Overlap::Sphere_AABB(sphere, boundingBox));
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::IsInsideRadius(IFunctionHandler* pH, Vec3 pos, float radius)
{
	IVehicle* vehicle = GetVehicle(pH);

	if (!vehicle)
		return pH->EndFunction();

	AABB boundingBox;
	IEntity* vehicleEntity = vehicle->GetEntity();
	vehicleEntity->GetWorldBounds(boundingBox);

	Sphere sphere(pos, radius);
	return pH->EndFunction(Overlap::Sphere_AABB(sphere, boundingBox));
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::IsEmpty(IFunctionHandler* pH)
{
	CVehicle* pVehicle = GetVehicle(pH);

	if (pVehicle && pVehicle->IsEmpty())
		return pH->EndFunction(true);

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::GetRepairableDamage(IFunctionHandler* pH)
{
	CVehicle* pVehicle = GetVehicle(pH);

	if (pVehicle)
	{
		// HUD displays the higher of hull damage and total damage %, so we should do the same here.
		float overallDamage = pVehicle->GetDamageRatio(true);
		float hullDamage = 0.0f;
		IVehicleComponent* pHull = pVehicle->GetComponent("hull");
		if (pHull)
		{
			hullDamage = pHull->GetDamageRatio();
		}

		return pH->EndFunction(max(hullDamage, overallDamage));
	}

	return pH->EndFunction(0.0f);
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::StartAbandonTimer(IFunctionHandler* pH)
{
	CVehicle* pVehicle = GetVehicle(pH);
	if (!pVehicle)
		return pH->EndFunction();

	bool force = false;
	float timer = -1.0f;

	if (pH->GetParamCount() > 0 && (pH->GetParamType(1) == svtNumber || pH->GetParamType(1) == svtBool))
		pH->GetParam(1, force);
	if (pH->GetParamCount() > 1 && pH->GetParamType(2) == svtNumber)
		pH->GetParam(2, timer);

	pVehicle->StartAbandonedTimer(force, timer);

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::KillAbandonTimer(IFunctionHandler* pH)
{
	CVehicle* pVehicle = GetVehicle(pH);
	if (!pVehicle)
		return pH->EndFunction();

	pVehicle->KillAbandonedTimer();

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::Destroy(IFunctionHandler* pH)
{
	CVehicle* pVehicle = GetVehicle(pH);
	if (!pVehicle)
		return pH->EndFunction();

	pVehicle->Destroy();

	return pH->EndFunction();
}


//------------------------------------------------------------------------
int ScriptBind_Vehicle::MultiplyWithEntityLocalTM(IFunctionHandler* pH, ScriptHandle entityHandle, Vec3 pos)
{
	GET_ENTITY;

	IEntity* e = gEnv->pEntitySystem->GetEntity((EntityId)entityHandle.n);
	if (!e)
		return pH->EndFunction();

	Matrix34 mat;
	IEntity* recurseEntity = e;

	mat.SetIdentity();

	while (recurseEntity != NULL)
	{
		if (recurseEntity->GetParent() != NULL)
		{
			mat = recurseEntity->GetLocalTM() * mat;
		}
		recurseEntity = recurseEntity->GetParent();
	}

	return pH->EndFunction(mat * pos);
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::MultiplyWithWorldTM(IFunctionHandler* pH, Vec3 pos)
{
	GET_ENTITY;

	return pH->EndFunction(pEntity->GetWorldTM() * pos);
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::UpdateVehicleAnimation(IFunctionHandler* pH, ScriptHandle entityHandle, int slot)
{
	GET_ENTITY;

	IEntity* part = gEnv->pEntitySystem->GetEntity((EntityId)entityHandle.n);
	if (!part)
		return pH->EndFunction();

	ICharacterInstance* charInstance = part->GetCharacter(slot);
	if (charInstance)
	{
		QuatT renderLocation = QuatT(part->GetWorldTM());
		charInstance->SkeletonPreProcess(renderLocation, renderLocation, gEnv->pSystem->GetViewCamera(), 0);
		//CryMP: Fixme
		//charInstance->SetPostProcessParameter(renderLocation, renderLocation, 0, (part->GetWorldTM().GetTranslation() - gEnv->pSystem->GetViewCamera().GetPosition()).GetLength(), 0);
	}
	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::RefreshPhysicsGeometry(IFunctionHandler* pH, ScriptHandle partHandle, int slot, int physIndex)
{
	GET_ENTITY;

	IEntity* part = gEnv->pEntitySystem->GetEntity((EntityId)partHandle.n);
	if (!part)
		return pH->EndFunction();

	IPhysicalEntity* physics = pEntity->GetPhysics();
	if (physics == NULL)
		return pH->EndFunction();

	if (physIndex > -1)
	{
		pe_params_part partParams;
		const Matrix34& worldTM = part->GetSlotWorldTM(slot);
		Matrix34 localTM = pEntity->GetWorldTM().GetInvertedFast() * worldTM;

		partParams.partid = physIndex;
		partParams.pMtx3x4 = const_cast<Matrix34*>(&localTM);
		pEntity->GetPhysics()->SetParams(&partParams);
	}

	return pH->EndFunction();
}


//------------------------------------------------------------------------
int ScriptBind_Vehicle::AddSeat(IFunctionHandler* pH, SmartScriptTable paramsTable)
{
	IVehicle* pVehicle = GetVehicle(pH);
	if (pVehicle)
	{
		pVehicle->AddSeat(paramsTable);
	}

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::SetExtensionActivation(IFunctionHandler* pH, const char* extension, bool bActivated)
{
	IVehicle* pVehicle = GetVehicle(pH);
	bool ok = false;
	if (pVehicle)
	{
		if (bActivated)
			ok = pVehicle->GetGameObject()->ActivateExtension(extension);
		else
		{
			pVehicle->GetGameObject()->DeactivateExtension(extension);
			ok = true;
		}
	}
	if (!ok)
		pH->GetIScriptSystem()->RaiseError("Failed to %s extension %s", bActivated ? "enable" : "disable", extension);
	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::SetExtensionParams(IFunctionHandler* pH, const char* extension, SmartScriptTable params)
{
	IVehicle* pVehicle = GetVehicle(pH);
	bool ok = false;
	if (pVehicle)
		ok = pVehicle->GetGameObject()->SetExtensionParams(extension, params);
	if (!ok)
		pH->GetIScriptSystem()->RaiseError("Failed to set params for extension %s", extension);
	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::GetExtensionParams(IFunctionHandler* pH, const char* extension, SmartScriptTable params)
{
	IVehicle* pVehicle = GetVehicle(pH);
	bool ok = false;
	if (pVehicle)
		ok = pVehicle->GetGameObject()->GetExtensionParams(extension, params);
	if (!ok)
		pH->GetIScriptSystem()->RaiseError("Failed to set params for extension %s", extension);
	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::HasHelper(IFunctionHandler* pH, const char* name)
{
	if (IVehicle* pVehicle = GetVehicle(pH))
	{
		IVehicleHelper* pHelper = pVehicle->GetHelper(name);
		return pH->EndFunction(pHelper != NULL);
	}

	return pH->EndFunction(false);
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::GetHelperPos(IFunctionHandler* pH, const char* name, bool isInVehicleSpace)
{
	if (IVehicle* pVehicle = GetVehicle(pH))
	{
		if (IVehicleHelper* pHelper = pVehicle->GetHelper(name))
		{
			if (isInVehicleSpace)
				return pH->EndFunction(pHelper->GetVehicleTM().GetTranslation());
			else
				return pH->EndFunction(pHelper->GetLocalTM().GetTranslation());
		}
	}

	return pH->EndFunction(Vec3(0.0f, 0.0f, 0.0f));
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::GetHelperDir(IFunctionHandler* pH, const char* name, bool isInVehicleSpace)
{
	if (IVehicle* pVehicle = GetVehicle(pH))
	{
		if (IVehicleHelper* pHelper = pVehicle->GetHelper(name))
		{
			Matrix34 tm;
			if (isInVehicleSpace)
				tm = pHelper->GetVehicleTM();
			else
				tm = pHelper->GetLocalTM();

			return pH->EndFunction(tm.TransformVector(FORWARD_DIRECTION));
		}
	}

	return pH->EndFunction(Vec3(0.0f, 1.0f, 0.0f));
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::GetHelperWorldPos(IFunctionHandler* pH, const char* name)
{
	if (IVehicle* pVehicle = GetVehicle(pH))
	{
		if (IVehicleHelper* pHelper = pVehicle->GetHelper(name))
			return pH->EndFunction(pHelper->GetWorldTM().GetTranslation());
	}

	return pH->EndFunction(Vec3(0.0f, 0.0f, 0.0f));
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::EnableMovement(IFunctionHandler* pH, bool enable)
{
	if (CVehicle* pVehicle = GetVehicle(pH))
	{
		if (pVehicle->GetMovement())
			pVehicle->GetMovement()->EnableMovementProcessing(enable);
	}

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::DisableEngine(IFunctionHandler* pH, bool disable)
{
	if (CVehicle* pVehicle = GetVehicle(pH))
	{
		if (pVehicle->GetMovement())
			pVehicle->GetMovement()->DisableEngine(disable);
	}

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::OnHit(IFunctionHandler* pH, ScriptHandle targetId, ScriptHandle shooterId, float damage, Vec3 position, float radius, char* pHitClass, bool explosion)
{
	if (CVehicle* pVehicle = GetVehicle(pH))
	{
		pVehicle->OnHit((EntityId)targetId.n, (EntityId)shooterId.n, damage, position, radius, pHitClass, explosion);
	}
	return pH->EndFunction();
}


//------------------------------------------------------------------------
int ScriptBind_Vehicle::ProcessPassengerDamage(IFunctionHandler* pH, ScriptHandle passengerHandle, float actorHealth, float damage, const char* pDamageClass, bool explosion)
{
	CVehicle* pVehicle = GetVehicle(pH);
	if (!pVehicle || !passengerHandle.n)
		return pH->EndFunction(0.0f);

	if (CVehicleSeat* pSeat = (CVehicleSeat*)pVehicle->GetSeatForPassenger((EntityId)passengerHandle.n))
	{
		float newDamage = pSeat->ProcessPassengerDamage(actorHealth, damage, pDamageClass, explosion);
		return pH->EndFunction(newDamage);
	}

	return pH->EndFunction(0.0f);
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::OnPassengerKilled(IFunctionHandler* pH, ScriptHandle passengerHandle)
{
	CVehicle* pVehicle = GetVehicle(pH);
	if (!pVehicle || !passengerHandle.n)
		return pH->EndFunction(0.0f);

	if (CVehicleSeat* pSeat = (CVehicleSeat*)pVehicle->GetSeatForPassenger((EntityId)passengerHandle.n))
	{
		pSeat->OnPassengerDeath();
	}

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::IsUsable(IFunctionHandler* pH, ScriptHandle userHandle)
{
	CVehicle* pVehicle = GetVehicle(pH);
	if (!pVehicle || !userHandle.n)
		return pH->EndFunction(0);

	int ret = pVehicle->IsUsable((EntityId)userHandle.n);
	return pH->EndFunction(ret);
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::OnUsed(IFunctionHandler* pH, ScriptHandle userHandle, int index)
{
	CVehicle* pVehicle = GetVehicle(pH);
	if (!pVehicle || !userHandle.n)
		return pH->EndFunction(0);

	int ret = pVehicle->OnUsed((EntityId)userHandle.n, index);
	return pH->EndFunction(ret);
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::EnterVehicle(IFunctionHandler* pH, ScriptHandle actorHandle, int seatId, bool isAnimationEnabled)
{
	CVehicle* pVehicle = GetVehicle(pH);
	if (!pVehicle || !actorHandle.n)
		return pH->EndFunction(0);

	IVehicleSeat* pSeat = pVehicle->GetSeatById(seatId);

	bool ret = false;

	if (pSeat)
		ret = pSeat->Enter((EntityId)actorHandle.n, isAnimationEnabled);

	return pH->EndFunction(ret);
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::ChangeSeat(IFunctionHandler* pH, ScriptHandle actorHandle, int seatId, bool isAnimationEnabled)
{
	CVehicle* pVehicle = GetVehicle(pH);
	if (!pVehicle || !actorHandle.n)
		return pH->EndFunction(0);

	bool ret = true;
	pVehicle->ChangeSeat((EntityId)actorHandle.n, seatId, 0);

	return pH->EndFunction(ret);
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::ExitVehicle(IFunctionHandler* pH, ScriptHandle actorHandle)
{
	CVehicle* pVehicle = GetVehicle(pH);
	if (!pVehicle || !actorHandle.n)
		return pH->EndFunction(0);

	if (IVehicleSeat* pSeat = pVehicle->GetSeatForPassenger((EntityId)actorHandle.n))
	{
		bool ret = pSeat->Exit(true);
		return pH->EndFunction(ret);
	}

	return pH->EndFunction(false);
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::GetComponentDamageRatio(IFunctionHandler* pH, const char* pComponentName)
{
	CVehicle* pVehicle = GetVehicle(pH);

	if (!pVehicle)
		return pH->EndFunction();

	if (IVehicleComponent* pComponent = pVehicle->GetComponent(pComponentName))
		return pH->EndFunction(pComponent->GetDamageRatio());

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::GetCollisionDamageThreshold(IFunctionHandler* pH)
{
	if (CVehicle* pVehicle = GetVehicle(pH))
	{
		return pH->EndFunction(pVehicle->GetDamageParams().collisionDamageThreshold);
	}

	return pH->EndFunction(0);
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::GetSelfCollisionMult(IFunctionHandler* pH, Vec3 velocity, Vec3 normal, int partId, ScriptHandle colliderId)
{
	if (CVehicle* pVehicle = GetVehicle(pH))
	{
		return pH->EndFunction(pVehicle->GetSelfCollisionMult(velocity, normal, partId, (EntityId)colliderId.n));
	}

	return pH->EndFunction(1.f);
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::GetMovementDamageRatio(IFunctionHandler* pH)
{
	if (CVehicle* pVehicle = GetVehicle(pH))
	{
		if (IVehicleMovement* pMovement = pVehicle->GetMovement())
		{
			return pH->EndFunction(pMovement->GetDamageRatio());
		}
	}

	return pH->EndFunction(0.0f);
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::IsDestroyed(IFunctionHandler* pH)
{
	if (CVehicle* pVehicle = GetVehicle(pH))
	{
		return pH->EndFunction(pVehicle->IsDestroyed());
	}

	return pH->EndFunction(false);
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::IsFlipped(IFunctionHandler* pH)
{
	if (CVehicle* pVehicle = GetVehicle(pH))
	{
		return pH->EndFunction(pVehicle->IsFlipped());
	}

	return pH->EndFunction(false);
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::IsSubmerged(IFunctionHandler* pH)
{
	if (CVehicle* pVehicle = GetVehicle(pH))
		return pH->EndFunction(pVehicle->IsSubmerged());

	return pH->EndFunction(false);
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::SetAmmoCount(IFunctionHandler* pH, const char* name, int amount)
{
	CVehicle* pVehicle = GetVehicle(pH);

	IEntityClass* pClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(name);
	assert(pClass);
	pVehicle->SetAmmoCount(pClass, amount);

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::SetMovementMode(IFunctionHandler* pH, int mode)
{
	if (CVehicle* pVehicle = GetVehicle(pH))
	{
		if (IVehicleMovement* pMovement = pVehicle->GetMovement())
		{
			SVehicleMovementEventParams params;
			params.iValue = mode;
			pMovement->OnEvent(IVehicleMovement::eVME_SetMode, params);
		}
	}

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::GetMovementType(IFunctionHandler* pH)
{
	if (CVehicle* pVehicle = GetVehicle(pH))
	{
		if (IVehicleMovement* pMovement = pVehicle->GetMovement())
		{
			const char* movement = "other";
			switch (pMovement->GetMovementType())
			{
			case IVehicleMovement::eVMT_Sea:
				movement = "sea";
				break;
			case IVehicleMovement::eVMT_Air:
				movement = "air";
				break;
			case IVehicleMovement::eVMT_Land:
				movement = "land";
				break;
			case IVehicleMovement::eVMT_Amphibious:
				movement = "amphybious";
				break;
			default:
				break;
			}

			return pH->EndFunction(movement);
		}
	}

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::GetFrozenAmount(IFunctionHandler* pH)
{
	if (CVehicle* pVehicle = GetVehicle(pH))
		return pH->EndFunction(pVehicle->GetFrozenAmount());

	return pH->EndFunction(0.0f);
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::SetFrozenAmount(IFunctionHandler* pH, float frost)
{
	if (CVehicle* pVehicle = GetVehicle(pH))
		pVehicle->SetFrozenAmount(frost);

	return pH->EndFunction();
}


//------------------------------------------------------------------------
int ScriptBind_Vehicle::OpenAutomaticDoors(IFunctionHandler* pH)
{
	if (CVehicle* pVehicle = GetVehicle(pH))
	{
		SVehicleEventParams eventParams;
		pVehicle->BroadcastVehicleEvent(eVE_OpenDoors, eventParams);
	}

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::CloseAutomaticDoors(IFunctionHandler* pH)
{
	if (CVehicle* pVehicle = GetVehicle(pH))
	{
		SVehicleEventParams eventParams;
		pVehicle->BroadcastVehicleEvent(eVE_CloseDoors, eventParams);
	}

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::ExtractGears(IFunctionHandler* pH)
{
	if (CVehicle* pVehicle = GetVehicle(pH))
	{
		SVehicleEventParams eventParams;
		pVehicle->BroadcastVehicleEvent(eVE_ExtractGears, eventParams);
	}

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::RetractGears(IFunctionHandler* pH)
{
	if (CVehicle* pVehicle = GetVehicle(pH))
	{
		SVehicleEventParams eventParams;
		pVehicle->BroadcastVehicleEvent(eVE_RetractGears, eventParams);
	}

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::BlockAutomaticDoors(IFunctionHandler* pH, bool isBlocked)
{
	if (CVehicle* pVehicle = GetVehicle(pH))
	{
		SVehicleEventParams eventParams;
		eventParams.bParam = isBlocked;
		pVehicle->BroadcastVehicleEvent(eVE_BlockDoors, eventParams);
	}

	return pH->EndFunction();
}

//------------------------------------------------------------------------
int ScriptBind_Vehicle::SetMovementSoundVolume(IFunctionHandler* pH, float volume)
{
	if (CVehicle* pVehicle = GetVehicle(pH))
	{
		if (IVehicleMovement* pMovement = pVehicle->GetMovement())
			pMovement->SetSoundMasterVolume(volume);
	}

	return pH->EndFunction();
}