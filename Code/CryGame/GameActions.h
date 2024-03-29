// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef __GAMEACTIONS_H__
#define __GAMEACTIONS_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include "CryCommon/CryAction/IActionMapManager.h"

#define DECL_ACTION(name) ActionId name;
class CGameActions
{
public:
	CGameActions();

	DECL_ACTION(moveleft)
	DECL_ACTION(moveright)
	DECL_ACTION(moveforward)
	DECL_ACTION(moveback)
	DECL_ACTION(jump)
	DECL_ACTION(crouch)
	DECL_ACTION(prone)
	DECL_ACTION(togglestance)
	DECL_ACTION(sprint)
	DECL_ACTION(special)
	DECL_ACTION(leanleft)
	DECL_ACTION(leanright)

	DECL_ACTION(rotateyaw)
	DECL_ACTION(rotatepitch)

	DECL_ACTION(nextspawnpoint)
	DECL_ACTION(flymode)
	DECL_ACTION(godmode)
	DECL_ACTION(toggle_airstrike)

	DECL_ACTION(cycle_spectator_mode)
	DECL_ACTION(prev_spectator_target)
	DECL_ACTION(next_spectator_target)

	DECL_ACTION(ulammo)
	DECL_ACTION(giveitems)

	DECL_ACTION(attack1)
	DECL_ACTION(attack2)
	DECL_ACTION(reload)
	DECL_ACTION(drop)
	DECL_ACTION(modify)
	DECL_ACTION(additem)
	DECL_ACTION(nextitem)
	DECL_ACTION(previtem)
	DECL_ACTION(small)
	DECL_ACTION(medium)
	DECL_ACTION(heavy)
	DECL_ACTION(explosive)
	DECL_ACTION(handgrenade)
	DECL_ACTION(xi_handgrenade)
	DECL_ACTION(holsteritem)

	DECL_ACTION(utility)
	DECL_ACTION(debug)
	DECL_ACTION(zoom)
	DECL_ACTION(firemode)
	DECL_ACTION(binoculars)
	DECL_ACTION(objectives)
	DECL_ACTION(grenade)
	DECL_ACTION(xi_grenade)

	DECL_ACTION(speedmode)
	DECL_ACTION(strengthmode)
	DECL_ACTION(defensemode)

	DECL_ACTION(zoom_in)
	DECL_ACTION(zoom_out)

	DECL_ACTION(invert_mouse)

	DECL_ACTION(thirdperson)
	DECL_ACTION(use)
	DECL_ACTION(xi_use)
	DECL_ACTION(xi_hud_mouseclick)

	DECL_ACTION(horn)
	DECL_ACTION(gyroscope)
	DECL_ACTION(gboots)
	DECL_ACTION(lights)

	DECL_ACTION(radio_group_0)
	DECL_ACTION(radio_group_1)
	DECL_ACTION(radio_group_2)
	DECL_ACTION(radio_group_3)
	DECL_ACTION(radio_group_4)

	DECL_ACTION(voice_chat_talk)
	DECL_ACTION(xi_voice_chat_talk)

	DECL_ACTION(save)
	DECL_ACTION(load)

	DECL_ACTION(suitcloak)
	DECL_ACTION(suitsave)
	DECL_ACTION(suitload)
	DECL_ACTION(suitmode)
	DECL_ACTION(suitskin)

	DECL_ACTION(ai_goto)
	DECL_ACTION(ai_follow)

	// XInput specific actions
	DECL_ACTION(xi_zoom)
	DECL_ACTION(xi_binoculars)
	DECL_ACTION(xi_movex)
	DECL_ACTION(xi_movey)
	DECL_ACTION(xi_disconnect)
	DECL_ACTION(xi_rotateyaw)
	DECL_ACTION(xi_rotatepitch)
	DECL_ACTION(xi_v_rotateyaw)
	DECL_ACTION(xi_v_rotatepitch)


	// HUD
	DECL_ACTION(hud_nanosuit_nextitem)
	DECL_ACTION(hud_nanosuit_minus)
	DECL_ACTION(hud_nanosuit_plus)
	DECL_ACTION(hud_mousex)
	DECL_ACTION(hud_mousey)
	DECL_ACTION(hud_mouseclick)
	DECL_ACTION(hud_suit_menu)
	DECL_ACTION(hud_openchat)
	DECL_ACTION(hud_openteamchat)
	DECL_ACTION(hud_mousewheelup)
	DECL_ACTION(hud_mousewheeldown)
	DECL_ACTION(hud_mouserightbtndown)
	DECL_ACTION(hud_mouserightbtnup)
	DECL_ACTION(hud_show_multiplayer_scoreboard)
	DECL_ACTION(hud_hide_multiplayer_scoreboard)
	DECL_ACTION(hud_toggle_scoreboard_cursor)
	DECL_ACTION(hud_pda_switch)
	DECL_ACTION(hud_show_pda_map)
	DECL_ACTION(xi_hud_show_pda_map)
	DECL_ACTION(hud_buy_weapons)
	DECL_ACTION(hud_pda_scroll)
	DECL_ACTION(scores)
	DECL_ACTION(hud_menu)
	DECL_ACTION(hud_back)
	DECL_ACTION(xi_hud_back)
	DECL_ACTION(hud_night_vision)
	DECL_ACTION(hud_weapon_mod)
	DECL_ACTION(hud_suit_mod)
	DECL_ACTION(hud_select1)
	DECL_ACTION(hud_select2)
	DECL_ACTION(hud_select3)
	DECL_ACTION(hud_select4)
	DECL_ACTION(hud_select5)
	DECL_ACTION(hud_mptutorial_disable)

	// Vehicle
	DECL_ACTION(v_changeseat1)
	DECL_ACTION(v_changeseat2)
	DECL_ACTION(v_changeseat3)
	DECL_ACTION(v_changeseat4)
	DECL_ACTION(v_changeseat5)

	DECL_ACTION(v_changeview)
	DECL_ACTION(v_viewoption)
	DECL_ACTION(v_zoom_in)
	DECL_ACTION(v_zoom_out)

	DECL_ACTION(v_attack1)
	DECL_ACTION(v_attack2)
	DECL_ACTION(v_firemode)
	DECL_ACTION(v_lights)
	DECL_ACTION(v_horn)

	DECL_ACTION(v_rotateyaw)
	DECL_ACTION(v_rotatepitch)

	DECL_ACTION(v_moveforward)
	DECL_ACTION(v_moveback)
	DECL_ACTION(v_moveup)
	DECL_ACTION(v_movedown)
	DECL_ACTION(v_rotatedir)
	DECL_ACTION(v_turnleft)
	DECL_ACTION(v_turnright)
	DECL_ACTION(v_strafeleft)
	DECL_ACTION(v_straferight)
	DECL_ACTION(v_rollleft)
	DECL_ACTION(v_rollright)

	DECL_ACTION(v_pitchup)
	DECL_ACTION(v_pitchdown)

	DECL_ACTION(v_brake)
	DECL_ACTION(v_afterburner)
	DECL_ACTION(v_boost)
	DECL_ACTION(v_changeseat)

	DECL_ACTION(v_debug_1)
	DECL_ACTION(v_debug_2)
	DECL_ACTION(buyammo)
	DECL_ACTION(loadLastSave)

	DECL_ACTION(debug_ag_step)

	void Init();
	ILINE IActionFilter*	FilterNoMove() const {	return m_pFilterNoMove;	}
	ILINE IActionFilter*	FilterNoMouse() const {	return m_pFilterNoMouse;	}
	ILINE IActionFilter*	FilterNoGrenades() const {	return m_pFilterNoGrenades;	}
	ILINE IActionFilter*	FilterInVehicleSuitMenu() const {	return m_pFilterInVehicleSuitMenu;	}
	ILINE IActionFilter*	FilterSuitMenu() const {	return m_pFilterSuitMenu;	}
	ILINE IActionFilter*	FilterFreezeTime() const {	return m_pFilterFreezeTime;	}
	ILINE IActionFilter*	FilterNoVehicleExit() const {	return m_pFilterNoVehicleExit;	}
	ILINE IActionFilter*	FilterMPRadio() const {	return m_pFilterMPRadio;	}
	ILINE IActionFilter*	FilterCutscene() const {	return m_pFilterCutscene;	}
	ILINE IActionFilter*	FilterCutsceneNoPlayer() const {	return m_pFilterCutsceneNoPlayer;	}
	ILINE IActionFilter*	FilterNoMapOpen() const {	return m_pFilterNoMapOpen;	}
	ILINE IActionFilter*	FilterNoObjectivesOpen() const {	return m_pFilterNoObjectivesOpen;	}
	ILINE IActionFilter*	FilterVehicleNoSeatChangeAndExit() const {	return m_pFilterVehicleNoSeatChangeAndExit;	}
	ILINE IActionFilter*	FilterNoConnectivity() const {	return m_pFilterNoConnectivity;	}


private:
	void	CreateFilterNoMove();
	void	CreateFilterNoMouse();
	void	CreateFilterNoGrenades();
	void	CreateFilterInVehicleSuitMenu();
	void	CreateFilterSuitMenu();
	void	CreateFilterFreezeTime();
	void	CreateFilterNoVehicleExit();
	void	CreateFilterMPRadio();
	void	CreateFilterCutscene();
	void	CreateFilterCutsceneNoPlayer();
	void	CreateFilterNoMapOpen();
	void	CreateFilterNoObjectivesOpen();
	void	CreateFilterVehicleNoSeatChangeAndExit();
	void	CreateFilterNoConnectivity();

	IActionFilter*	m_pFilterNoMove;
	IActionFilter*	m_pFilterNoMouse;
	IActionFilter*	m_pFilterNoGrenades;
	IActionFilter*	m_pFilterInVehicleSuitMenu;
	IActionFilter*	m_pFilterSuitMenu;
	IActionFilter*	m_pFilterFreezeTime;
	IActionFilter*	m_pFilterNoVehicleExit;
	IActionFilter*	m_pFilterMPRadio;
	IActionFilter*	m_pFilterCutscene;
	IActionFilter*	m_pFilterCutsceneNoPlayer;
	IActionFilter*	m_pFilterNoMapOpen;
	IActionFilter*	m_pFilterNoObjectivesOpen;
	IActionFilter*	m_pFilterVehicleNoSeatChangeAndExit;
	IActionFilter*	m_pFilterNoConnectivity;
};
#undef DECL_ACTION

extern CGameActions* g_pGameActions;

#endif //__GAMEACTIONS_H__
