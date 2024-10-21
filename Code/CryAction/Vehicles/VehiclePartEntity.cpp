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
#include "CryCommon/CrySystem/ISystem.h"

#include "CryCommon/CryAction/IViewSystem.h"
#include "CryCommon/CryAction/IItemSystem.h"
#include "CryCommon/CryAction/IVehicleSystem.h"
#include "CryCommon/CryPhysics/IPhysics.h"
#include "CryCommon/CryAnimation/ICryAnimation.h"
#include "CryCommon/CryAction/IActorSystem.h"
#include "CryCommon/CrySoundSystem/ISound.h"
#include "CryCommon/CryNetwork/ISerialize.h"
#include "CryCommon/CryAISystem/IAgent.h"

//#include "CryAction.h"
#include "Vehicle.h"
#include "VehiclePartEntity.h"

//------------------------------------------------------------------------
bool CVehiclePartEntity::Init(IGameObject * pGameObject)
{
	return true;
}

//------------------------------------------------------------------------
void CVehiclePartEntity::Serialize(TSerialize ser, unsigned aspects)
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
