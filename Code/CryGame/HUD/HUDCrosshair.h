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

#ifndef HUD_CROSSHAIR_H
#define HUD_CROSSHAIR_H

# pragma once

#define WEAPON_FADECROSSHAIR_FRIENDLYCROSS		(0.200f)

#include "HUDObject.h"
#include "CryCommon/CrySystem/IFlashPlayer.h"
#include "CryCommon/CryAction/IVehicleSystem.h"
#include "GameFlashAnimation.h"

class CHUD;
class CPlayer;

class CHUDCrosshair : public CHUDObject
{

public:
	CHUDCrosshair(CHUD* pHUD);
	~CHUDCrosshair();

	// CHUDObject
	virtual void Update(float fDeltaTime);
	void UpdateDamageIndicator(CPlayer* pPlayer, float fDeltaTime);
	// ~CHUDObject

	//use-icon
	void SetUsability(int usable, const char* actionLabel = nullptr, const char* paramA = nullptr, 
		const char* paramB = nullptr, bool skipParamATranslation = false);
	void HandleUsability(int objId, const char* message);
	bool GetUsability() const;
	//show enemy hit in crosshair
	void CrosshairHit();
	//choose and set crosshair design 
	void SelectCrosshair(IItem *pItem = NULL);
	void SetCrosshair(int iCrosshair);
	//get crosshair flash movie
	CGameFlashAnimation *GetFlashAnim() {return &m_animCrossHair;}
	bool IsFriendlyEntity(IEntity *pEntity);

	bool IsFriendlyCrossVisible() const
	{
		return m_animFriendCross.GetVisible();
	}

	void Break(bool state);
	
	//Fading
	void SetVisibility(bool visible);
	bool GetVisibility() const;
	void SetOpacity(float opacity);
	float GetOpacity() const;
	void Fade(float from, float to, float time);
	void UpdateOpacity(float frameTime);

	void OnLookatEntityChangeTeam(EntityId entityId);

	ILINE int GetCrosshairType() const { return m_iCrosshair; }

	void Reset();

	void ShowDamageIndicator(float seconds);

	bool IsDamageIndicatorProcessing()
	{
		return m_fDamageIndicatorTimer > 0.0f;
	}

private:

	//update function
	void UpdateCrosshair();

	void SetFlashOpacity(float opacity);

	//the main HUD
	CHUD* m_pHUD = nullptr;
	//the crosshair flash asset
	CGameFlashAnimation	m_animCrossHair;
	//the friend-cross flash asset
	CGameFlashAnimation	m_animFriendCross;
	//the use icons flash asset
	CGameFlashAnimation m_animInterActiveIcons;
	//usability flag (can use lookat object)
	bool m_bUsable = false;
	// targetted friendly unit
	int m_iFriendlyTarget = 0;
	// crosshair type cache value
	int m_iCrosshair = -1;
	bool m_setCrosshairInFlash = false;
	// maps usability strings to icons
	std::map<string, int> m_useIcons;
	//hide the use icon in special cases
	bool m_bHideUseIconTemp = false;
	//broken hud state
	bool m_bBroken = false;
	// crosshair opacity
	float m_opacity = false;
	float m_spread = false;
	float m_smoothSpread = false;
	
	//CryMP
	float m_fDamageIndicatorTimer = 0.0f;

	struct Fading
	{
		bool fading = false;
		bool visible = true;
		float fadefrom = 1.0f;
		float fadeto = 1.0f;
		float fadetime = 0.0f;
		float fadetimer = 0.0f;
		float opacity = 0.0f;
	};

	Fading m_fading;

};

#endif
