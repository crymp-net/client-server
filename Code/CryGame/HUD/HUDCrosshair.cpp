/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2007.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Crosshair HUD object (refactored from old HUD code)

-------------------------------------------------------------------------
History:
- 15:05:2007  11:00 : Created by Jan Müller

*************************************************************************/

#include "CryGame/StdAfx.h"
#include "HUDCrosshair.h"
#include "CryCommon/CryAction/IWorldQuery.h"
#include "CryGame/GameCVars.h"
#include "CryGame/GameRules.h"
#include "CryCommon/CryGame/GameUtils.h"
#include "HUD.h"
#include "HUDVehicleInterface.h"
#include "CryGame/Items/Item.h"
#include "CryGame/Items/Weapons/Weapon.h"
#include "CryGame/Items/Weapons/OffHand.h"

//-----------------------------------------------------------------------------------------------------

CHUDCrosshair::CHUDCrosshair(CHUD* pHUD)
{
	m_pHUD = pHUD;
	m_animCrossHair.Load("Libs/UI/HUD_Crosshair.gfx", eFD_Center, eFAF_ManualRender);
	m_animFriendCross.Load("Libs/UI/HUD_FriendlyCross.gfx", eFD_Center, eFAF_ManualRender);
	m_animInterActiveIcons.Load("Libs/UI/HUD_InterActiveIcons.gfx", eFD_Center, eFAF_ManualRender);

	m_useIcons["@use"] = 1;
	m_useIcons["@use_vehicle"] = 2; //"enter
	m_useIcons["@pick_item"] = 3; //pick
	m_useIcons["@pick_weapon"] = 3; //pick
	m_useIcons["@grab_enemy"] = 4;	//grab
	m_useIcons["@grab_object"] = 4;	//grab
	m_useIcons["@use_ladder"] = 5; //ladder

	Reset();
}

//-----------------------------------------------------------------------------------------------------

CHUDCrosshair::~CHUDCrosshair()
{
	m_animCrossHair.Unload();
	m_animFriendCross.Unload();
	m_animInterActiveIcons.Unload();
}

//-----------------------------------------------------------------------------------------------------

void CHUDCrosshair::Reset()
{
	m_spread = 0.0f;
	m_smoothSpread = 0.0f;
	m_animCrossHair.Invoke("setRecoil", 0.0f);
	m_iCrosshair = -1;
}

//-----------------------------------------------------------------------------------------------------

void CHUDCrosshair::Update(float fDeltaTime)
{
	if (m_bBroken)
		return;

	CPlayer* pPlayer = m_pHUD->m_pClientActor;
	IItemSystem* pItemSystem = g_pGame->GetIGameFramework()->GetIItemSystem();

	if (pPlayer->IsFpSpectator())
		pPlayer = CPlayer::FromIActor(pPlayer->GetSpectatorTargetPlayer());

	if (!pPlayer)
		return;

	UpdateDamageIndicator(pPlayer, fDeltaTime);

	IInventory* pInventory = pPlayer->GetInventory();
	if (!pInventory)
		return;

	IItem* pItem = pItemSystem->GetItem(pInventory->GetCurrentItem());
	IWeapon* pWeapon = NULL;
	IWeapon* pSlaveWeapon = NULL;
	const float fAlternateIronSight = 0.03f;

	if (pItem)
	{
		pWeapon = pItem->GetIWeapon();

		if (pItem->IsDualWieldMaster())
		{
			if (IItem* pSlave = pItem->GetDualWieldSlave())
				pSlaveWeapon = pSlave->GetIWeapon();
		}
	}
	else if (IVehicle* pVehicle = pPlayer->GetLinkedVehicle())
	{
		pItem = pItemSystem->GetItem(pVehicle->GetCurrentWeaponId(pPlayer->GetEntityId()));
		if (pItem)
			pWeapon = pItem->GetIWeapon();
	}

	if (pWeapon)
	{
		float fMinSpread = 0.0f;
		float fMaxSpread = 0.0f;

		m_spread = 0.0f;

		if (IFireMode* pFireMode = pWeapon->GetFireMode(pWeapon->GetCurrentFireMode()))
		{
			fMinSpread = pFireMode->GetMinSpread();
			fMaxSpread = pFireMode->GetMaxSpread();

			m_spread = pFireMode->GetSpread();
		}

		if (pSlaveWeapon)
		{
			if (IFireMode* pSlaveFireMode = pSlaveWeapon->GetFireMode(pSlaveWeapon->GetCurrentFireMode()))
			{
				fMinSpread += pSlaveFireMode->GetMinSpread();
				fMaxSpread += pSlaveFireMode->GetMaxSpread();

				m_spread += pSlaveFireMode->GetSpread();
			}
		}

		if (pPlayer->GetNanoSuit() && pPlayer->GetNanoSuit()->GetMode() == NANOMODE_STRENGTH)
			m_spread *= 0.5;

		if (g_pGameCVars->hud_iAlternateCrosshairSpread)
		{
			if (m_spread < fMinSpread)
				m_spread = min(m_spread, g_pGameCVars->hud_fAlternateCrosshairSpreadCrouch) / g_pGameCVars->hud_fAlternateCrosshairSpreadCrouch;
			else
				m_spread = min(m_spread, g_pGameCVars->hud_fAlternateCrosshairSpreadNeutral) / g_pGameCVars->hud_fAlternateCrosshairSpreadNeutral;
		}
		else
		{
			m_spread = min((m_spread - fMinSpread), 15.0f) / 15.0f;

			IZoomMode* pZoomMode = pWeapon->GetZoomMode(pWeapon->GetCurrentZoomMode());
			if (pZoomMode && !pZoomMode->IsToggle() && (pZoomMode->IsZoomed() || pZoomMode->IsZooming()))
			{
				m_spread -= fAlternateIronSight;
			}
			else
			{
				m_spread = min(m_spread, 1.0f);
				m_spread = max(m_spread, 0.0f);
			}
		}
	}

	if ((!m_pHUD->InSpectatorMode() || pPlayer->IsFpSpectatorTarget())) //CryMP Fp Spec render crosshair
	{
		//also disables the damage indicator
		if (/*g_pGameCVars->hud_crosshair>0 && m_iCrosshair > 0 &&*/ (g_pGameCVars->g_difficultyLevel < 4 || gEnv->bMultiplayer))
		{
			if (m_animCrossHair.GetVisible())
			{
				m_animCrossHair.GetFlashPlayer()->Advance(fDeltaTime);
				m_animCrossHair.GetFlashPlayer()->Render();

				/*f32 fColor[4] = { 1,1,0,1 };
				f32 g_YLine = 130.0f;
				gEnv->pRenderer->Draw2dLabel(1, g_YLine, 1.3f, fColor, false, "Rendering crosshair: %s", m_animCrossHair.GetVisible() ? "visible" : "invisible");
				*/
			}
		}

		if (m_animInterActiveIcons.GetVisible()) //if the crosshair is invisible, the use icon should be too
		{
			if (!m_bHideUseIconTemp)	//hides the icon, when something is already grabbed/being used
			{
				m_animInterActiveIcons.GetFlashPlayer()->Advance(fDeltaTime);
				m_animInterActiveIcons.GetFlashPlayer()->Render();
			}
		}
	}

	if (m_animFriendCross.GetVisible())
	{
		m_animFriendCross.GetFlashPlayer()->Advance(fDeltaTime);
		m_animFriendCross.GetFlashPlayer()->Render();
	}

	if (!g_pGameCVars->hud_iAlternateCrosshairSpread)
	{
		m_spread = max(m_spread, -fAlternateIronSight);
		m_spread = min(m_spread, 1.0f);
	}

	if (m_smoothSpread != m_spread)
	{
		Interpolate(m_smoothSpread, m_spread, 20.0f, fDeltaTime);
		m_animCrossHair.Invoke("setRecoil", m_smoothSpread);
	}

	UpdateCrosshair();
}

void CHUDCrosshair::UpdateDamageIndicator(CPlayer *pPlayer, float fDeltaTime)
{
	if (!pPlayer)
		return;

	if (m_fDamageIndicatorTimer > 0.0f)
	{
		m_fDamageIndicatorTimer -= fDeltaTime;

		float angle = ((pPlayer->GetAngles().z * 180.0f / gf_PI) + 180.0f);
		if (angle < 0.0f)
			angle = 360.0f - angle;

		GetFlashAnim()->CheckedSetVariable("DamageDirection._rotation", SFlashVarValue(angle));

		if (m_fDamageIndicatorTimer < 0.1f)
		{
			m_fDamageIndicatorTimer = 0.0f;
		}
	}
}

//-----------------------------------------------------------------------------------------------------

void CHUDCrosshair::SetUsability(int usable, const char* actionLabel, const char* paramA, const char* paramB)
{
	m_bUsable = (usable > 0) ? true : false;
	m_animCrossHair.Invoke("setUsable", usable);
	if (actionLabel)
	{
		if (paramA)
		{
			string paramLocA;
			string paramLocB;
			if (paramA[0] != '@')
			{
				paramLocA = "@";
				paramLocA.append(paramA);
			}
			else
				paramLocA = paramA;

			if (paramB)
			{
				if (paramB[0] != '@')
				{
					paramLocB = "@";
					paramLocB.append(paramB);
				}
				else
					paramLocB = paramB;
			}

			m_animInterActiveIcons.Invoke("setText", m_pHUD->LocalizeWithParams(actionLabel, true, paramLocA.c_str(), paramLocB.c_str()));
		}
		else
			m_animInterActiveIcons.Invoke("setText", actionLabel);

		//set icon
		int icon = stl::find_in_map(m_useIcons, actionLabel, 0);
		if (!icon && usable)
			icon = 1;
		if (icon)
		{
			m_animInterActiveIcons.SetVisible(true);
			m_animInterActiveIcons.Invoke("setUseIcon", icon);
		}
		else if (m_animInterActiveIcons.GetVisible())
			m_animInterActiveIcons.SetVisible(false);
	}
	else if (m_animInterActiveIcons.GetVisible())
		m_animInterActiveIcons.SetVisible(false);
}

//-----------------------------------------------------------------------------------------------------

bool CHUDCrosshair::GetUsability() const
{
	return m_bUsable;
}

//-----------------------------------------------------------------------------------------------------

void CHUDCrosshair::CrosshairHit()
{
	m_animCrossHair.Invoke("setHit");
}

//-----------------------------------------------------------------------------------------------------

void CHUDCrosshair::SetCrosshair(int iCrosshair)
{
	if (g_pGameCVars->g_difficultyLevel > 3 && !gEnv->bMultiplayer)
		iCrosshair = 0;

	iCrosshair = MAX(0, iCrosshair);
	iCrosshair = MIN(13, iCrosshair);

	if (m_iCrosshair != iCrosshair)
	{
		m_iCrosshair = iCrosshair;
		m_animCrossHair.Invoke("setCrossHair", iCrosshair);
		m_animCrossHair.Invoke("setUsable", m_bUsable);
	}
}

//-----------------------------------------------------------------------------------------------------

bool CHUDCrosshair::IsFriendlyEntity(IEntity* pEntity)
{
	IActor* pClientActor = g_pGame->GetIGameFramework()->GetClientActor();
	CGameRules* pGameRules = g_pGame->GetGameRules();

	if (!pEntity || !pClientActor || !pGameRules)
		return false;

	// Less than 2 teams means we are in a FFA based game.
	if (pGameRules->GetTeamCount() < 2)
		return false;

	bool bFriendly = false;

	int iClientTeam = pGameRules->GetTeam(pClientActor->GetEntityId());

	// First, check if entity is a player
	IActor* pActor = g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor(pEntity->GetId());
	if (pActor && pActor->IsPlayer())
	{
		if (iClientTeam && (pGameRules->GetTeam(pActor->GetEntityId()) == iClientTeam))
		{
			bFriendly = true;
		}
	}
	else
	{
		// Then, check if entity is a vehicle
		IVehicle* pVehicle = gEnv->pGame->GetIGameFramework()->GetIVehicleSystem()->GetVehicle(pEntity->GetId());
		if (pVehicle && pGameRules->GetTeam(pVehicle->GetEntityId()) == iClientTeam && pVehicle->GetStatus().passengerCount)
		{
			IActor* pDriver = pVehicle->GetDriver();
			/*if(pDriver && pGameRules->GetTeam(pDriver->GetEntityId()) == iClientTeam)
				bFriendly = true;
			else
				bFriendly = false;*/

			bFriendly = true;

			//fix for bad raycast
			if (pDriver && pDriver == pClientActor)
				bFriendly = false;
		}
	}

	return bFriendly;
}

//-----------------------------------------------------------------------------------------------------

void CHUDCrosshair::UpdateCrosshair()
{
	CPlayer* pPlayer = m_pHUD->m_pClientActor;

	//CryMP: Fp spec support
	if (pPlayer && pPlayer->IsFpSpectator())
		pPlayer = CPlayer::FromIActor(pPlayer->GetSpectatorTargetPlayer());

	if (!pPlayer)
		return;

	int iNewFriendly = 0;

	if (pPlayer->GetLinkedVehicle())
	{
		// JanM/MichaelR: 
		// Get status from the VehicleWeapon, which raycasts considering the necessary SkipEntities (in contrast to WorldQuery)
		// Julien: this is now done in MP as well
		iNewFriendly = m_pHUD->GetVehicleInterface()->GetFriendlyFire();
	}
	else
	{
		if (!gEnv->bMultiplayer)
		{
			CWeapon* pWeapon = m_pHUD->GetCurrentWeapon();
			if (pWeapon)
			{
				iNewFriendly = pWeapon->IsWeaponLowered() && pWeapon->IsPendingFireRequest();
				if (iNewFriendly && pWeapon->GetEntity()->GetClass() == CItem::sTACGunFleetClass)
					iNewFriendly = 0;
			}
			else {
				//Two handed pickups need the red X as well
				if (CWeapon* pOffHand = static_cast<CWeapon*>(pPlayer->GetItemByClass(CItem::sOffHandClass)))
					iNewFriendly = pOffHand->IsWeaponLowered();
			}
		}
		else
		{
			EntityId uiCenterId = pPlayer->GetGameObject()->GetWorldQuery()->GetLookAtEntityId();
			if (uiCenterId)
			{
				iNewFriendly = IsFriendlyEntity(gEnv->pEntitySystem->GetEntity(uiCenterId));
			}
		}
	}

	// SNH: if player is carrying a claymore or mine, ask the weapon whether it is possible to place it currently
	//	(takes into account player speed / stance / aim direction).
	// So 'friendly' is a bit of a misnomer here, but we want the "don't/can't fire" crosshair...
	if (iNewFriendly != 1)
	{
		CWeapon * pWeapon = pPlayer->GetWeapon(pPlayer->GetCurrentItemId());
		if (pWeapon)
		{
			const IEntityClass* pClass = pWeapon->GetEntity()->GetClass();

			if (pClass == CItem::sClaymoreClass || pClass == CItem::sAVMineClass)
			{
				if (IFireMode* pfm = pWeapon->GetFireMode(pWeapon->GetCurrentFireMode()))
				{
					if (!pfm->IsFiring())
						iNewFriendly = pWeapon->CanFire() ? 0 : 1;
				}
			}
		}
	}

	if (iNewFriendly != m_iFriendlyTarget)
	{
		m_iFriendlyTarget = iNewFriendly;
		//m_animCrossHair.Invoke("setFriendly", m_iFriendlyTarget);
		if (iNewFriendly)
			m_animFriendCross.SetVisible(true);
		else
			m_animFriendCross.SetVisible(false);
	}

	if (m_animInterActiveIcons.GetVisible())
	{
		m_bHideUseIconTemp = false;

		CWeapon* pWeapon = pPlayer->GetCurrentWeapon(false);
		if (pWeapon)
		{
			CItem::SStats stats = pWeapon->GetStats();
			if (stats.mounted && stats.used)
				m_bHideUseIconTemp = true;
		}
		
		if (!m_bHideUseIconTemp)
		{
			EntityId offHandId = pPlayer->GetInventory()->GetItemByClass(CItem::sOffHandClass);
			IItem* pOffHandItem = g_pGame->GetIGameFramework()->GetIItemSystem()->GetItem(offHandId);
			if (pOffHandItem)
			{
				COffHand* pOffHand = static_cast<COffHand*>(pOffHandItem);
				uint32 offHandState = pOffHand->GetOffHandState();
				if (offHandState == eOHS_HOLDING_OBJECT || offHandState == eOHS_THROWING_OBJECT ||
					offHandState == eOHS_HOLDING_NPC || offHandState == eOHS_THROWING_NPC)
				{
					m_bHideUseIconTemp = true;
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------------

void CHUDCrosshair::SetOpacity(float opacity)
{
	if (opacity != m_opacity)
	{
		m_animCrossHair.Invoke("setOpacity", opacity);
		m_opacity = opacity;
	}
}

//-----------------------------------------------------------------------------------------------------

void CHUDCrosshair::SelectCrosshair(IItem* pItem)
{
	//set special crosshairs design comes up with ...
	bool bSpecialCrosshairSet = false;
	CPlayer* pPlayer = m_pHUD->m_pClientActor;

	//CryMP: Fp spec support
	if (pPlayer->IsFpSpectator())
		pPlayer = CPlayer::FromIActor(pPlayer->GetSpectatorTargetPlayer());

	if (!pPlayer)
	{
		SetCrosshair(0);
		return;
	}

	if (g_pGameCVars->hud_crosshair != 0)
	{
		if (!pItem)
			pItem = pPlayer->GetCurrentItem();

		const IEntityClass *pClass = pItem ? pItem->GetEntity()->GetClass() : nullptr;

		if (!pItem ||
			pClass == CItem::sFistsClass ||
			pClass == CItem::sOffHandClass ||
			pClass == CItem::sBinocularsClass ||
			pClass == CItem::sAlienCloak)
		{
			SetCrosshair(0); //was 10 
			bSpecialCrosshairSet = true;
		}
		else if (IWeapon* pWeapon = pItem->GetIWeapon())		//Laser attached
		{
			if ((static_cast<CWeapon*>(pWeapon))->IsLamAttached())
			{
				SetCrosshair(0);
				bSpecialCrosshairSet = true;
			}
		}

		// No current item or current item are fists or AlienCloak or LAW
		if (!bSpecialCrosshairSet)
		{
			if (pClass == CItem::sRocketLauncherClass ||
				pClass == CItem::sTACGunFleetClass)
			{
				SetCrosshair(0);
				bSpecialCrosshairSet = true;
			}
			else if (pClass == CItem::sTACGunClass)
			{
				SetCrosshair(11);
				bSpecialCrosshairSet = true;
			}
			else if (m_pHUD->GetSelectedFiremode() == 6) //sleep bullet
			{
				SetCrosshair(12);
				bSpecialCrosshairSet = true;
			}
			else if (m_pHUD->GetSelectedFiremode() == 4) //grenade launcher
			{
				SetCrosshair(13);
				bSpecialCrosshairSet = true;
			}
		}
	}

	//now set normal crosshair
	if (!bSpecialCrosshairSet)
		SetCrosshair(g_pGameCVars->hud_crosshair);
}

void CHUDCrosshair::Break(bool state)
{
	m_bBroken = state;

	if (CGameFlashAnimation* pAnim = &m_animCrossHair)
	{
		pAnim->Invoke("clearDamageDirection");
		pAnim->GetFlashPlayer()->Advance(0.1f);
	}
}

bool CHUD::ShouldRenderCrosshair() const
{
	if (!m_pHUDCrosshair)
		return false;

	if (m_pHUDCrosshair->IsDamageIndicatorProcessing())
		return true; //always render

	if (m_bHideCrosshair)
		return false;

	if (m_pHUDCrosshair->GetCrosshairType() == 0)
		return false;

	if (m_animScoreBoard.GetVisible() || m_animObjectivesTab.GetVisible())
		return false;

	if (m_pModalHUD && m_pModalHUD->GetVisible())
		return false;

	CPlayer* pPlayer = m_pClientActor;

	if (IActor* pSpec = pPlayer->GetSpectatorTargetPlayer())
	{
		pPlayer = CPlayer::FromIActor(pSpec);
		if (!pPlayer)
			return false;

		if (IVehicle *pVehicle = pPlayer->GetLinkedVehicle())
		{
			if (IVehicleSeat* pSeat = pVehicle->GetSeatForPassenger(pPlayer->GetEntityId()))
			{
				bool b(pSeat->IsGunner());
				
				return b;
			}
		}
	}

	if (CWeapon* pWeapon = pPlayer->GetCurrentWeapon(false))
	{
		if (pWeapon->IsLamAttached() || pWeapon->IsModifyingNoTS())
			return false;
	}

	if (m_pHUDVehicleInterface->GetVehicle())
	{
		return m_pHUDVehicleInterface->ForceCrosshair();
	}
	return true;
}
void CHUD::UpdateCrosshairVisibility()
{
	if (!m_pHUDCrosshair)
		return;

	const bool bShow = ShouldRenderCrosshair();
	if (bShow != m_pHUDCrosshair->GetFlashAnim()->GetVisible())
	{
		m_pHUDCrosshair->GetFlashAnim()->SetVisible(bShow);

		//turn off damage circle
		m_pHUDCrosshair->GetFlashAnim()->Invoke("clearDamageDirection");
		m_pHUDCrosshair->GetFlashAnim()->GetFlashPlayer()->Advance(0.1f);
	}
}