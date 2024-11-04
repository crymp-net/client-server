// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
// Reference weapon for lighting tests
// John Newfield

#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CryAnimation/ICryAnimation.h"
#include "ReferenceWeapon.h"

CReferenceWeapon::CReferenceWeapon(void)
{
}

CReferenceWeapon::~CReferenceWeapon(void)
{
}

void CReferenceWeapon::OnAction(EntityId actorId, const ActionId& actionId, int activationMode, float value)
{
	if (actionId == "attack1")  
	{     
		if (activationMode == eAAM_OnPress)
		{
			PlayAction(g_pItemStrings->fire, 0, true);
		}
		else if(activationMode == eAAM_OnRelease)
		{
			ICharacterInstance* pCharacter = GetEntity()->GetCharacter(eIGS_FirstPerson);
			if (pCharacter)
			{
				pCharacter->GetISkeletonAnim()->StopAnimationInLayer(0, 0.0f);
			}
			PlayAction(g_pItemStrings->idle, 0, true);
		}
	}
	else if (actionId == "zoom")
	{
		//
	}
	else
		CWeapon::OnAction(actorId, actionId, activationMode, value);
}
