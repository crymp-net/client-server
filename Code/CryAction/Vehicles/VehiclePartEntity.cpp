/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2005.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Implements an entity class which can serialize vehicle parts

-------------------------------------------------------------------------
History:
- 16:09:2005: Created by Mathieu Pinard

*************************************************************************/
#include "StdAfx.h"

#include <IViewSystem.h>
#include <IItemSystem.h>
#include <IVehicleSystem.h>
#include <IPhysics.h>
#include <ICryAnimation.h>
#include <IActorSystem.h>
#include <ISound.h>
#include <ISerialize.h>
#include <IAgent.h>

#include "CryAction.h"
#include "Vehicle.h"
#include "VehiclePartEntity.h"

//------------------------------------------------------------------------
bool CVehiclePartEntity::Init(IGameObject * pGameObject)
{
	return true;
}

//------------------------------------------------------------------------
void CVehiclePartEntity::Serialize(TSerialize ser, EEntityAspects aspects)
{
}

//------------------------------------------------------------------------
void CVehiclePartEntity::Update(SEntityUpdateContext& ctx, int)
{
}

//------------------------------------------------------------------------
void CVehiclePartEntity::HandleEvent(const SGameObjectEvent& event)
{
}
