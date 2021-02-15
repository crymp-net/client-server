// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#include "StdAfx.h"
#include "GameActions.h"
#include "Game.h"

#define DECL_ACTION(name) name = #name;
CGameActions::CGameActions()
: m_pFilterNoMove(0)
, m_pFilterNoMouse(0)
, m_pFilterNoGrenades(0)
, m_pFilterInVehicleSuitMenu(0)
, m_pFilterSuitMenu(0)
, m_pFilterFreezeTime(0)
, m_pFilterNoVehicleExit(0)
, m_pFilterMPRadio(0)
, m_pFilterCutscene(0)
, m_pFilterCutsceneNoPlayer(0)
, m_pFilterNoMapOpen(0)
, m_pFilterNoObjectivesOpen(0)
, m_pFilterVehicleNoSeatChangeAndExit(0)
, m_pFilterNoConnectivity(0)
{
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
}
#undef DECL_ACTION

void CGameActions::Init()
{
	CreateFilterNoMove();
	CreateFilterNoMouse();
	CreateFilterNoGrenades();
	CreateFilterInVehicleSuitMenu();
	CreateFilterSuitMenu();
	CreateFilterFreezeTime();
	CreateFilterNoVehicleExit();
	CreateFilterMPRadio();
	CreateFilterCutscene();
	CreateFilterCutsceneNoPlayer();
	CreateFilterNoMapOpen();
	CreateFilterNoObjectivesOpen();
	CreateFilterVehicleNoSeatChangeAndExit();
	CreateFilterNoConnectivity();
}

void CGameActions::CreateFilterNoMove()
{
	IActionMapManager* pActionMapMan = g_pGame->GetIGameFramework()->GetIActionMapManager();

	m_pFilterNoMove = pActionMapMan->CreateActionFilter("no_move", eAFT_ActionFail);
	m_pFilterNoMove->Filter(leanleft);
	m_pFilterNoMove->Filter(leanright);
	m_pFilterNoMove->Filter(crouch);
	m_pFilterNoMove->Filter(prone);
	m_pFilterNoMove->Filter(jump);
	m_pFilterNoMove->Filter(moveleft);
	m_pFilterNoMove->Filter(moveright);
	m_pFilterNoMove->Filter(moveforward);
	m_pFilterNoMove->Filter(moveback);
	m_pFilterNoMove->Filter(sprint);
	m_pFilterNoMove->Filter(xi_movey);
	m_pFilterNoMove->Filter(xi_movex);
}

void CGameActions::CreateFilterNoGrenades()
{
	IActionMapManager* pActionMapMan = g_pGame->GetIGameFramework()->GetIActionMapManager();

	m_pFilterNoGrenades = pActionMapMan->CreateActionFilter("no_grenades", eAFT_ActionFail);
	m_pFilterNoGrenades->Filter(grenade);
	m_pFilterNoGrenades->Filter(xi_grenade);
}

void CGameActions::CreateFilterNoMouse()
{
	IActionMapManager* pActionMapMan = g_pGame->GetIGameFramework()->GetIActionMapManager();

	m_pFilterNoMouse = pActionMapMan->CreateActionFilter("no_mouse", eAFT_ActionFail);
	m_pFilterNoMouse->Filter(attack1);
	m_pFilterNoMouse->Filter(v_attack1);
	m_pFilterNoMouse->Filter(v_attack2);
	m_pFilterNoMouse->Filter(rotateyaw);
	m_pFilterNoMouse->Filter(v_rotateyaw);
	m_pFilterNoMouse->Filter(rotatepitch);
	m_pFilterNoMouse->Filter(v_rotatepitch);
	m_pFilterNoMouse->Filter(nextitem);
	m_pFilterNoMouse->Filter(previtem);
	m_pFilterNoMouse->Filter(small);
	m_pFilterNoMouse->Filter(medium);
	m_pFilterNoMouse->Filter(heavy);
	m_pFilterNoMouse->Filter(handgrenade);
	m_pFilterNoMouse->Filter(explosive);
	m_pFilterNoMouse->Filter(suitmode);
	m_pFilterNoMouse->Filter(utility);
	m_pFilterNoMouse->Filter(zoom);
	m_pFilterNoMouse->Filter(reload);
	m_pFilterNoMouse->Filter(use);
	m_pFilterNoMouse->Filter(xi_use);
	m_pFilterNoMouse->Filter(xi_grenade);
	m_pFilterNoMouse->Filter(xi_handgrenade);
	m_pFilterNoMouse->Filter(xi_zoom);
	m_pFilterNoMouse->Filter(jump);
}

void CGameActions::CreateFilterInVehicleSuitMenu()
{
	//need when suit menu is on in a vehicle. for XBOX controller and for keyboard
	IActionMapManager* pActionMapMan = g_pGame->GetIGameFramework()->GetIActionMapManager();

	m_pFilterInVehicleSuitMenu = pActionMapMan->CreateActionFilter("in_vehicle_suit_menu", eAFT_ActionFail);
	m_pFilterInVehicleSuitMenu->Filter(use);
	m_pFilterInVehicleSuitMenu->Filter(v_changeseat);
	m_pFilterInVehicleSuitMenu->Filter(v_changeview);
	m_pFilterInVehicleSuitMenu->Filter(v_lights);
	m_pFilterInVehicleSuitMenu->Filter(v_changeseat1);
	m_pFilterInVehicleSuitMenu->Filter(v_changeseat2);
	m_pFilterInVehicleSuitMenu->Filter(v_changeseat3);
	m_pFilterInVehicleSuitMenu->Filter(v_changeseat4);
	m_pFilterInVehicleSuitMenu->Filter(v_changeseat5);
}

void CGameActions::CreateFilterSuitMenu()
{
	IActionMapManager* pActionMapMan = g_pGame->GetIGameFramework()->GetIActionMapManager();

	m_pFilterSuitMenu = pActionMapMan->CreateActionFilter("suit_menu", eAFT_ActionFail);
	m_pFilterSuitMenu->Filter(attack1);
	m_pFilterSuitMenu->Filter(v_attack1);
	m_pFilterSuitMenu->Filter(v_attack2);
	m_pFilterSuitMenu->Filter(rotateyaw);
	m_pFilterSuitMenu->Filter(v_rotateyaw);
	m_pFilterSuitMenu->Filter(rotatepitch);
	m_pFilterSuitMenu->Filter(v_rotatepitch);
	m_pFilterSuitMenu->Filter(nextitem);
	m_pFilterSuitMenu->Filter(previtem);
	m_pFilterSuitMenu->Filter(small);
	m_pFilterSuitMenu->Filter(medium);
	m_pFilterSuitMenu->Filter(heavy);
	m_pFilterSuitMenu->Filter(handgrenade);
	m_pFilterSuitMenu->Filter(explosive);
	m_pFilterSuitMenu->Filter(suitmode);
	m_pFilterSuitMenu->Filter(utility);
	m_pFilterSuitMenu->Filter(reload);
	m_pFilterSuitMenu->Filter(use);
	m_pFilterSuitMenu->Filter(xi_use);
	m_pFilterSuitMenu->Filter(xi_grenade);
	m_pFilterSuitMenu->Filter(xi_handgrenade);
	m_pFilterSuitMenu->Filter(xi_zoom);
}

void CGameActions::CreateFilterFreezeTime()
{
	IActionMapManager* pActionMapMan = g_pGame->GetIGameFramework()->GetIActionMapManager();

	m_pFilterFreezeTime = pActionMapMan->CreateActionFilter("freezetime", eAFT_ActionPass);
	m_pFilterFreezeTime->Filter(reload);
	m_pFilterFreezeTime->Filter(rotateyaw);
	m_pFilterFreezeTime->Filter(rotatepitch);
	m_pFilterFreezeTime->Filter(drop);
	m_pFilterFreezeTime->Filter(modify);
	m_pFilterFreezeTime->Filter(jump);
	m_pFilterFreezeTime->Filter(crouch);
	m_pFilterFreezeTime->Filter(prone);
	m_pFilterFreezeTime->Filter(togglestance);
	m_pFilterFreezeTime->Filter(leanleft);
	m_pFilterFreezeTime->Filter(leanright);

	m_pFilterFreezeTime->Filter(rotateyaw);
	m_pFilterFreezeTime->Filter(rotatepitch);

	m_pFilterFreezeTime->Filter(reload);
	m_pFilterFreezeTime->Filter(drop);
	m_pFilterFreezeTime->Filter(modify);
	m_pFilterFreezeTime->Filter(nextitem);
	m_pFilterFreezeTime->Filter(previtem);
	m_pFilterFreezeTime->Filter(small);
	m_pFilterFreezeTime->Filter(medium);
	m_pFilterFreezeTime->Filter(heavy);
	m_pFilterFreezeTime->Filter(explosive);
	m_pFilterFreezeTime->Filter(handgrenade);
	m_pFilterFreezeTime->Filter(holsteritem);

	m_pFilterFreezeTime->Filter(utility);
	m_pFilterFreezeTime->Filter(debug);
	m_pFilterFreezeTime->Filter(firemode);
	m_pFilterFreezeTime->Filter(objectives);

	m_pFilterFreezeTime->Filter(speedmode);
	m_pFilterFreezeTime->Filter(strengthmode);
	m_pFilterFreezeTime->Filter(defensemode);

	m_pFilterFreezeTime->Filter(invert_mouse);

	m_pFilterFreezeTime->Filter(gboots);
	m_pFilterFreezeTime->Filter(lights);

	m_pFilterFreezeTime->Filter(radio_group_0);
	m_pFilterFreezeTime->Filter(radio_group_1);
	m_pFilterFreezeTime->Filter(radio_group_2);
	m_pFilterFreezeTime->Filter(radio_group_3);
	m_pFilterFreezeTime->Filter(radio_group_4);

	m_pFilterFreezeTime->Filter(voice_chat_talk);
	m_pFilterFreezeTime->Filter(xi_voice_chat_talk);

	// XInput specific actions
	m_pFilterFreezeTime->Filter(xi_binoculars);
	m_pFilterFreezeTime->Filter(xi_rotateyaw);
	m_pFilterFreezeTime->Filter(xi_rotatepitch);
	m_pFilterFreezeTime->Filter(xi_v_rotateyaw);
	m_pFilterFreezeTime->Filter(xi_v_rotatepitch);

	// HUD
	m_pFilterFreezeTime->Filter(hud_nanosuit_nextitem);
	m_pFilterFreezeTime->Filter(hud_nanosuit_minus);
	m_pFilterFreezeTime->Filter(hud_nanosuit_plus);
	m_pFilterFreezeTime->Filter(hud_mousex);
	m_pFilterFreezeTime->Filter(hud_mousey);
	m_pFilterFreezeTime->Filter(hud_mouseclick);
	m_pFilterFreezeTime->Filter(hud_suit_menu);
	m_pFilterFreezeTime->Filter(hud_openchat);
	m_pFilterFreezeTime->Filter(hud_openteamchat);
	m_pFilterFreezeTime->Filter(hud_mousewheelup);
	m_pFilterFreezeTime->Filter(hud_mousewheeldown);
	m_pFilterFreezeTime->Filter(hud_mouserightbtndown);
	m_pFilterFreezeTime->Filter(hud_mouserightbtnup);
	m_pFilterFreezeTime->Filter(hud_show_multiplayer_scoreboard);
	m_pFilterFreezeTime->Filter(hud_hide_multiplayer_scoreboard);
	m_pFilterFreezeTime->Filter(hud_toggle_scoreboard_cursor);
	m_pFilterFreezeTime->Filter(hud_pda_switch);
	m_pFilterFreezeTime->Filter(hud_show_pda_map);
	m_pFilterFreezeTime->Filter(hud_buy_weapons);
	m_pFilterFreezeTime->Filter(hud_pda_scroll);
	m_pFilterFreezeTime->Filter(scores);
	m_pFilterFreezeTime->Filter(hud_menu);
	m_pFilterFreezeTime->Filter(hud_back);
	m_pFilterFreezeTime->Filter(xi_hud_back);
	m_pFilterFreezeTime->Filter(hud_night_vision);
	m_pFilterFreezeTime->Filter(hud_weapon_mod);
	m_pFilterFreezeTime->Filter(hud_suit_mod);
	m_pFilterFreezeTime->Filter(hud_select1);
	m_pFilterFreezeTime->Filter(hud_select2);
	m_pFilterFreezeTime->Filter(hud_select3);
	m_pFilterFreezeTime->Filter(hud_select4);
	m_pFilterFreezeTime->Filter(hud_select5);

	m_pFilterFreezeTime->Filter(buyammo);
}

void CGameActions::CreateFilterNoVehicleExit()
{
	IActionMapManager* pActionMapMan = g_pGame->GetIGameFramework()->GetIActionMapManager();

	m_pFilterNoVehicleExit = pActionMapMan->CreateActionFilter("no_vehicle_exit", eAFT_ActionFail);
	m_pFilterNoVehicleExit->Filter(use);
}

void CGameActions::CreateFilterMPRadio()
{
	IActionMapManager* pActionMapMan = g_pGame->GetIGameFramework()->GetIActionMapManager();

	m_pFilterMPRadio = pActionMapMan->CreateActionFilter("mp_radio", eAFT_ActionFail);
	m_pFilterMPRadio->Filter(small);
	m_pFilterMPRadio->Filter(medium);
	m_pFilterMPRadio->Filter(heavy);
	m_pFilterMPRadio->Filter(suitmode);
	m_pFilterMPRadio->Filter(explosive);
	m_pFilterMPRadio->Filter(handgrenade);
	m_pFilterMPRadio->Filter(v_changeseat1);
	m_pFilterMPRadio->Filter(v_changeseat2);
	m_pFilterMPRadio->Filter(v_changeseat3);
	m_pFilterMPRadio->Filter(v_changeseat4);
	m_pFilterMPRadio->Filter(v_changeseat5);
	m_pFilterMPRadio->Filter(v_changeseat);
}

void CGameActions::CreateFilterCutscene()
{
	IActionMapManager* pActionMapMan = g_pGame->GetIGameFramework()->GetIActionMapManager();

	m_pFilterCutscene = pActionMapMan->CreateActionFilter("cutscene", eAFT_ActionFail);	
	m_pFilterCutscene->Filter(binoculars);
	m_pFilterCutscene->Filter(hud_night_vision);
	m_pFilterCutscene->Filter(hud_show_multiplayer_scoreboard);
	m_pFilterCutscene->Filter(hud_hide_multiplayer_scoreboard);
	m_pFilterCutscene->Filter(hud_suit_menu);
	m_pFilterCutscene->Filter(hud_suit_mod);
	m_pFilterCutscene->Filter(hud_weapon_mod);
	m_pFilterCutscene->Filter(hud_show_pda_map);
	m_pFilterCutscene->Filter(leanleft);
	m_pFilterCutscene->Filter(leanright);
}

void CGameActions::CreateFilterCutsceneNoPlayer()
{
	IActionMapManager* pActionMapMan = g_pGame->GetIGameFramework()->GetIActionMapManager();

	m_pFilterCutsceneNoPlayer = pActionMapMan->CreateActionFilter("cutscene_no_player", eAFT_ActionPass);
	m_pFilterCutsceneNoPlayer->Filter(loadLastSave);
	m_pFilterCutsceneNoPlayer->Filter(load);
}

void CGameActions::CreateFilterNoMapOpen()
{
	IActionMapManager* pActionMapMan = g_pGame->GetIGameFramework()->GetIActionMapManager();

	m_pFilterNoMapOpen = pActionMapMan->CreateActionFilter("no_map_open", eAFT_ActionFail);
	m_pFilterNoMapOpen->Filter(hud_show_pda_map);
}

void CGameActions::CreateFilterNoObjectivesOpen()
{
	IActionMapManager* pActionMapMan = g_pGame->GetIGameFramework()->GetIActionMapManager();

	m_pFilterNoObjectivesOpen = pActionMapMan->CreateActionFilter("no_objectives_open", eAFT_ActionFail);
	m_pFilterNoObjectivesOpen->Filter(hud_show_multiplayer_scoreboard);
	m_pFilterNoObjectivesOpen->Filter(hud_hide_multiplayer_scoreboard);
	m_pFilterNoObjectivesOpen->Filter(scores);
}

void CGameActions::CreateFilterVehicleNoSeatChangeAndExit()
{
	IActionMapManager* pActionMapMan = g_pGame->GetIGameFramework()->GetIActionMapManager();

	m_pFilterVehicleNoSeatChangeAndExit = pActionMapMan->CreateActionFilter("vehicle_no_seat_change_and_exit", eAFT_ActionFail);
	m_pFilterVehicleNoSeatChangeAndExit->Filter(use);
	m_pFilterVehicleNoSeatChangeAndExit->Filter(v_changeseat);
	m_pFilterVehicleNoSeatChangeAndExit->Filter(v_changeseat1);
	m_pFilterVehicleNoSeatChangeAndExit->Filter(v_changeseat2);
	m_pFilterVehicleNoSeatChangeAndExit->Filter(v_changeseat3);
	m_pFilterVehicleNoSeatChangeAndExit->Filter(v_changeseat4);
	m_pFilterVehicleNoSeatChangeAndExit->Filter(v_changeseat5);
}

void CGameActions::CreateFilterNoConnectivity()
{
	IActionMapManager* pActionMapMan = g_pGame->GetIGameFramework()->GetIActionMapManager();

	m_pFilterNoConnectivity = pActionMapMan->CreateActionFilter("no_connectivity", eAFT_ActionPass);
	m_pFilterNoConnectivity->Filter(hud_show_multiplayer_scoreboard);
	m_pFilterNoConnectivity->Filter(hud_hide_multiplayer_scoreboard);
	m_pFilterNoConnectivity->Filter(scores);

	//CryMP feature:
	//Allow chat, modify weapon, move mouse and zoom actions during network lag
	m_pFilterNoConnectivity->Filter(hud_openchat);
	m_pFilterNoConnectivity->Filter(hud_openteamchat);
	m_pFilterNoConnectivity->Filter(modify);
	m_pFilterNoConnectivity->Filter(rotatepitch);
	m_pFilterNoConnectivity->Filter(rotateyaw);
	m_pFilterNoConnectivity->Filter(zoom);
	m_pFilterNoConnectivity->Filter(zoom_out);
}
