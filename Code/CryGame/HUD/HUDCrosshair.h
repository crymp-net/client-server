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
	void SetUsability(int usable, const char* actionLabel = NULL, const char* paramA = NULL, const char* paramB = NULL);
	bool GetUsability() const;
	//show enemy hit in crosshair
	void CrosshairHit();
	//choose and set crosshair design 
	void SelectCrosshair(IItem *pItem = NULL);
	// set opacity of crosshair (0 is invisible, 1 is fully visible)
	void SetOpacity(float opacity);
	void SetCrosshair(int iCrosshair);
	//get crosshair flash movie
	CGameFlashAnimation *GetFlashAnim() {return &m_animCrossHair;}
	bool IsFriendlyEntity(IEntity *pEntity);

	void Break(bool state);
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
};

#endif
