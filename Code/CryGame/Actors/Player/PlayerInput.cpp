// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/CryInput/IInput.h"
#include "PlayerInput.h"
#include "Player.h"
#include "CryGame/Game.h"
#include "CryGame/GameCVars.h"
#include "CryGame/GameActions.h"
#include "CryGame/Items/Weapons/Weapon.h"
#include "CryGame/Items/Weapons/WeaponSystem.h"
#include "CryCommon/CryAction/IVehicleSystem.h"
#include "CryGame/Vehicles/VehicleClient.h"
#include "CryGame/Items/Weapons/OffHand.h"
#include "CryGame/Items/Weapons/Fists.h"
#include "CryGame/HUD/HUD.h"
#include "CryGame/GameRules.h"

#include "CryCommon/CryAction/IWorldQuery.h"
#include "CryCommon/CryAction/IInteractor.h"

#include <array>

TActionHandler<CPlayerInput>	CPlayerInput::s_actionHandler;

CPlayerInput::CPlayerInput(CPlayer* pPlayer) :
	m_pPlayer(pPlayer),
	m_pStats(&pPlayer->m_stats),
	m_actions(ACTION_GYROSCOPE),
	m_deltaRotation(0, 0, 0),
	m_deltaMovement(0, 0, 0),
	m_xi_deltaMovement(0, 0, 0),
	m_xi_deltaRotation(0, 0, 0),
	m_filteredDeltaMovement(0, 0, 0),
	m_buttonPressure(0.0f),
	m_speedLean(0.0f),
	m_bDisabledXIRot(false),
	m_fCrouchPressedTime(-1.0f),
	m_moveButtonState(0),
	m_bUseXIInput(false),
	m_checkZoom(false),
	m_lastPos(0, 0, 0),
	m_lookDir(0, 0, 0),
	m_iSuitModeActionPressed(0),
	m_fSuitModeActionTime(0.0f),
	m_lastSerializeFrameID(0)
{
	m_pPlayer->RegisterPlayerEventListener(this);
	m_pPlayer->GetGameObject()->CaptureActions(this);
	m_binocularsTime = 0.0f;
	// set up the handlers
	if (s_actionHandler.GetNumHandlers() == 0)
	{
#define ADD_HANDLER(action, func) s_actionHandler.AddHandler(actions.action, &CPlayerInput::func)
		const CGameActions& actions = g_pGame->Actions();

		ADD_HANDLER(moveforward, OnActionMoveForward);
		ADD_HANDLER(moveback, OnActionMoveBack);
		ADD_HANDLER(moveleft, OnActionMoveLeft);
		ADD_HANDLER(moveright, OnActionMoveRight);
		ADD_HANDLER(rotateyaw, OnActionRotateYaw);
		ADD_HANDLER(rotatepitch, OnActionRotatePitch);
		ADD_HANDLER(jump, OnActionJump);
		ADD_HANDLER(suitmode, OnActionSuitMode);
		ADD_HANDLER(suitskin, OnActionSuitSkin);
		ADD_HANDLER(crouch, OnActionCrouch);
		ADD_HANDLER(sprint, OnActionSprint);
		ADD_HANDLER(togglestance, OnActionToggleStance);
		ADD_HANDLER(prone, OnActionProne);
		//ADD_HANDLER(zerogbrake, OnActionZeroGBrake);
		ADD_HANDLER(gyroscope, OnActionGyroscope);
		ADD_HANDLER(gboots, OnActionGBoots);
		ADD_HANDLER(leanleft, OnActionLeanLeft);
		ADD_HANDLER(leanright, OnActionLeanRight);
		//ADD_HANDLER(holsteritem, OnActionHolsterItem);
		ADD_HANDLER(use, OnActionUse);

		ADD_HANDLER(speedmode, OnActionSpeedMode);
		ADD_HANDLER(strengthmode, OnActionStrengthMode);
		ADD_HANDLER(defensemode, OnActionDefenseMode);
		ADD_HANDLER(suitcloak, OnActionSuitCloak);

		ADD_HANDLER(thirdperson, OnActionThirdPerson);
		ADD_HANDLER(flymode, OnActionFlyMode);
		ADD_HANDLER(godmode, OnActionGodMode);

		ADD_HANDLER(v_rotateyaw, OnActionVRotateYaw); // needed so player can shake unfreeze while in a vehicle
		ADD_HANDLER(v_rotatepitch, OnActionVRotatePitch);

		ADD_HANDLER(xi_v_rotateyaw, OnActionXIRotateYaw);
		ADD_HANDLER(xi_rotateyaw, OnActionXIRotateYaw);
		ADD_HANDLER(xi_rotatepitch, OnActionXIRotatePitch);

		ADD_HANDLER(xi_v_rotatepitch, OnActionXIRotatePitch);
		ADD_HANDLER(xi_movex, OnActionXIMoveX);
		ADD_HANDLER(xi_movey, OnActionXIMoveY);
		ADD_HANDLER(xi_disconnect, OnActionXIDisconnect);
		ADD_HANDLER(xi_use, OnActionUse);

		ADD_HANDLER(invert_mouse, OnActionInvertMouse);

#undef ADD_HANDLER
	}

	//CryMP
	AddCustomBinds();
}

CPlayerInput::~CPlayerInput()
{
	m_pPlayer->UnregisterPlayerEventListener(this);
	m_pPlayer->GetGameObject()->ReleaseActions(this);
}

void CPlayerInput::Reset()
{
	m_actions = ACTION_GYROSCOPE;//gyroscope is on by default
	m_lastActions = m_actions;

	m_deltaMovement.zero();
	m_xi_deltaMovement.zero();
	m_filteredDeltaMovement.zero();
	m_deltaRotation.Set(0, 0, 0);
	m_xi_deltaRotation.Set(0, 0, 0);
	m_bDisabledXIRot = false;
	m_moveButtonState = 0;
	m_lastSerializeFrameID = 0;
	m_binocularsTime = 0.0f;
}

void CPlayerInput::AddCustomBinds()
{
	//CryMP: Enable [r] in landvehicles for "ReverseView"
	const int MAX_KEYS = 3;
	const CGameActions& actions = g_pGame->Actions();
	IActionMapManager* pMapManager = gEnv->pGame->GetIGameFramework()->GetIActionMapManager();
	if (IActionMap* playerMap = pMapManager->GetActionMap("player"))
	{
		SActionMapBindInfo s = {};
		std::array<const char*, MAX_KEYS> keysBuffer = {};
		s.keys = keysBuffer.data();

		if (playerMap->GetBindInfo(actions.reload, s, MAX_KEYS))
		{
			if (IActionMap* landVehicleMap = pMapManager->GetActionMap("landvehicle"))
			{
				/*<action name = "reload" onPress = "1">
					<key name = "r" / >
					<key name = "xi_x" / >
					< / action>
				*/
				if (s.keys[0] && s.keys[1])
				{
					landVehicleMap->CreateAction(actions.reload.c_str(), eAAM_OnPress | eAAM_OnRelease, s.keys[0], s.keys[1]);
				}
				else if (s.keys[0])
				{
					landVehicleMap->CreateAction(actions.reload.c_str(), eAAM_OnPress | eAAM_OnRelease, s.keys[0]);
				}
			}
		}
	}
}

void CPlayerInput::DisableXI(bool disabled)
{
	m_bDisabledXIRot = disabled;
}

void CPlayerInput::ApplyMovement(Vec3 delta)
{
	//m_deltaMovement += delta;
	m_deltaMovement.x = clamp_tpl(m_deltaMovement.x + delta.x, -1.0f, 1.0f);
	m_deltaMovement.y = clamp_tpl(m_deltaMovement.y + delta.y, -1.0f, 1.0f);
	m_deltaMovement.z = 0;

	//static float color[] = {1,1,1,1};
	//gEnv->pRenderer->Draw2dLabel(100,50,1.5,color,false,"m_deltaMovement:%f,%f (requested:%f,%f", m_deltaMovement.x, m_deltaMovement.y,delta.x,delta.y);
}

void CPlayerInput::OnAction(const ActionId& actionId, int activationMode, float value)
{
	FUNCTION_PROFILER(GetISystem(), PROFILE_GAME);

	m_pPlayer->GetGameObject()->ChangedNetworkState(INPUT_ASPECT);

	m_lastActions = m_actions;

	//this tell if OnAction have to be forwarded to scripts, now its true by default, only high framerate actions are ignored
	bool filterOut = true;
	m_checkZoom = false;
	const CGameActions& actions = g_pGame->Actions();
	IVehicle* pVehicle = m_pPlayer->GetLinkedVehicle();

	bool canMove = CanMove();

	// disable movement while standing up
	if (!canMove)
		m_deltaMovement.zero();

	// try to dispatch action to OnActionHandlers
	bool handled;

	{
		FRAME_PROFILER("New Action Processing", GetISystem(), PROFILE_GAME);
		handled = s_actionHandler.Dispatch(this, m_pPlayer->GetEntityId(), actionId, activationMode, value, filterOut);
	}

	{
		FRAME_PROFILER("Regular Action Processing", GetISystem(), PROFILE_GAME);

		if (!handled)
		{
			filterOut = true;
			if (!m_pPlayer->m_stats.spectatorMode)
			{
				if (actions.ulammo == actionId && m_pPlayer->m_pGameFramework->CanCheat() && gEnv->pSystem->IsDevMode())
				{
					g_pGameCVars->i_unlimitedammo = 1;
				}
				else if (actions.debug_ag_step == actionId)
				{
					gEnv->pConsole->ExecuteString("ag_step");
				}
				else if (actions.voice_chat_talk == actionId)
				{
					if (gEnv->bMultiplayer)
					{
						if (activationMode == eAAM_OnPress)
							g_pGame->GetIGameFramework()->EnableVoiceRecording(true);
						else if (activationMode == eAAM_OnRelease)
							g_pGame->GetIGameFramework()->EnableVoiceRecording(false);
					}
				}
				else if (actions.xi_voice_chat_talk == actionId)
				{
					IInventory* pInventory = m_pPlayer->GetInventory();
					bool binoculars = false;
					bool scope = false;
					if (pInventory)
					{
						EntityId itemId = pInventory->GetCurrentItem();
						CWeapon* pWeapon = 0;
						if (itemId)
						{
							pWeapon = m_pPlayer->GetWeapon(itemId);
							if (pWeapon)
							{
								binoculars = (pWeapon->GetEntity()->GetClass() == CItem::sBinocularsClass);
								scope = (pWeapon->IsZoomed() && pWeapon->GetMaxZoomSteps() > 1);
							}
						}
					}

					if (activationMode == eAAM_OnPress)
					{
						m_binocularsTime = 0.5f;
					}
					else if (activationMode == eAAM_OnRelease)
					{
						if (m_binocularsTime > 0.0f)
						{
							m_binocularsTime = 0.0f;
							COffHand* pOffHand = static_cast<COffHand*>(m_pPlayer->GetWeaponByClass(CItem::sOffHandClass));
							if (binoculars)
								m_pPlayer->SelectLastItem(false, true);
							else if (!m_pPlayer->m_stats.mountedWeaponID && (!pOffHand || (pOffHand->GetOffHandState() == eOHS_INIT_STATE) && !(m_pPlayer->GetNanoSuit() && !m_pPlayer->GetNanoSuit()->IsActive())) && !SAFE_HUD_FUNC_RET(IsInitializing()))
							{
								m_pPlayer->SelectItemByName("Binoculars", true);
							}
							DisableXI(false);
						}
						else
						{
							if (gEnv->bMultiplayer)
							{
								g_pGame->GetIGameFramework()->EnableVoiceRecording(false);
							}
						}
					}
					DisableXI(false);
				}
			}
		}

		if (!m_pPlayer->m_stats.spectatorMode)
		{
			IInventory* pInventory = m_pPlayer->GetInventory();
			if (!pInventory)
				return;

			bool binoculars = false;
			bool scope = false;
			EntityId itemId = pInventory->GetCurrentItem();
			CWeapon* pWeapon = 0;
			if (itemId)
			{
				pWeapon = m_pPlayer->GetWeapon(itemId);
				if (pWeapon)
				{
					binoculars = (pWeapon->GetEntity()->GetClass() == CItem::sBinocularsClass);
					scope = (pWeapon->IsZoomed() && pWeapon->GetMaxZoomSteps() > 1);
				}
			}

			if (pVehicle)
			{
				// 06/06/2007: Bernd took the (good) decision to remove the ability to use binoculars from inside a vehicle
				// Let's keep the (buggy in third person) code for now and remove it once it's validated by everyone else!

/*				if (binoculars)
				{
					// This is needed to forward action like v_zoom_in/v_zoom_out to Binoculars
					// FIXME?: Does it produce some conflicts with others things?
					m_pPlayer->CActor::OnAction(actionId, activationMode, value);
				}*/

				//CryMP: Reverse view
				if (actions.reload == actionId)
				{
					if (activationMode == eAAM_OnPress)
					{
						m_vehicleReverseView = true;
					}
					else if (activationMode == eAAM_OnRelease)
					{
						m_vehicleReverseView = false;
					}
				}

				if (m_pPlayer->m_pVehicleClient && !m_pPlayer->IsFrozen())
					m_pPlayer->m_pVehicleClient->OnAction(pVehicle, m_pPlayer->GetEntityId(), actionId, activationMode, value);

				/*				if (actions.binoculars == actionId)
								{
									COffHand* pOffHand = static_cast<COffHand*>(m_pPlayer->GetWeaponByClass(CItem::sOffHandClass));
									if (binoculars)
									{
										m_pPlayer->SelectLastItem(false);
										g_pGame->GetIGameFramework()->GetIItemSystem()->GetItem(itemId)->Select(false);
										if(m_pPlayer->GetCurrentItem(false))
										{
											m_pPlayer->GetInventory()->HolsterItem(false);
											m_pPlayer->GetInventory()->HolsterItem(true);
										}
									}
									else if(!m_pPlayer->m_stats.mountedWeaponID && (!pOffHand || (pOffHand->GetOffHandState()==eOHS_INIT_STATE)))
									{
										if(IVehicleSeat *pSeat = pVehicle->GetSeatForPassenger(m_pPlayer->GetEntityId()))
										{
											if(pSeat->IsGunner() && !pSeat->IsDriver())
											{
												m_pPlayer->SelectItemByName("Binoculars", true);
												// If we open Binoculars while we are shooting, we need to simulate a release of the shooting key
												pVehicle->OnAction(eVAI_Attack1,eAAM_OnRelease,1.0f,m_pPlayer->GetEntityId());
											}
										}
									}
								}

								if (actions.xi_binoculars == actionId)
								{
									if(activationMode == eAAM_OnPress)
									{
										if(IVehicleSeat *pSeat = pVehicle->GetSeatForPassenger(m_pPlayer->GetEntityId()))
										{
											if(pSeat->IsGunner() && !pSeat->IsDriver())
											{
												m_pPlayer->SelectItemByName("Binoculars", true);
												// If we open Binoculars while we are shooting, we need to simulate a release of the shooting key
												pVehicle->OnAction(eVAI_Attack1,eAAM_OnRelease,1.0f,m_pPlayer->GetEntityId());
											}
										}
									}
									else if(activationMode == eAAM_OnRelease)
									{
										if(binoculars)
											m_pPlayer->SelectLastItem(false);
									}
								}*/

								//FIXME:not really good
				m_actions = 0;
				//			m_deltaRotation.Set(0,0,0);
				m_deltaMovement.Set(0, 0, 0);
			}
			else if (m_pPlayer->GetHealth() > 0 && !m_pPlayer->m_stats.isFrozen.Value() && !m_pPlayer->m_stats.inFreefall.Value() && !m_pPlayer->m_stats.isOnLadder
				&& !m_pPlayer->m_stats.isStandingUp && m_pPlayer->GetGameObject()->GetAspectProfile(eEA_Physics) != eAP_Sleep)
			{
				m_pPlayer->CActor::OnAction(actionId, activationMode, value);

				if (!binoculars && (!scope || actionId == actions.use))
				{
					COffHand* pOffHand = static_cast<COffHand*>(m_pPlayer->GetWeaponByClass(CItem::sOffHandClass));
					if (pOffHand)
					{
						pOffHand->OnAction(m_pPlayer->GetEntityId(), actionId, activationMode, value);
					}

					if ((!pWeapon || !pWeapon->IsMounted()))
					{
						if ((actions.drop == actionId) && itemId)
						{
							float impulseScale = 1.0f;
							if (activationMode == eAAM_OnPress)
								m_buttonPressure = 2.5f;
							if (activationMode == eAAM_OnRelease)
							{
								m_buttonPressure = CLAMP(m_buttonPressure, 0.0f, 2.5f);
								impulseScale = 1.0f + (1.0f - m_buttonPressure / 2.5f) * 15.0f;
								if (m_pPlayer->DropItem(itemId, impulseScale, true) && pOffHand && pOffHand->IsSelected())
								{
									if (EntityId fistsId = pInventory->GetItemByClass(CItem::sFistsClass))
									{
										m_pPlayer->SelectItem(fistsId, false);
									}
									pOffHand->PreExecuteAction(eOHA_REINIT_WEAPON, eAAM_OnPress);
									CItem* pItem = static_cast<CItem*>(m_pPlayer->GetCurrentItem());
									if (pItem)
									{
										pItem->SetActionSuffix("akimbo_");
										pItem->PlayAction(g_pItemStrings->idle);
									}
								}
							}
						}
						else if (actions.nextitem == actionId)
							m_pPlayer->SelectNextItem(1, true, 0);
						else if (actions.previtem == actionId)
							m_pPlayer->SelectNextItem(-1, true, 0);
						else if (actions.handgrenade == actionId)
							m_pPlayer->SelectNextItem(1, true, actionId.c_str());
						else if (actions.explosive == actionId)
							m_pPlayer->SelectNextItem(1, true, actionId.c_str());
						else if (actions.utility == actionId)
							m_pPlayer->SelectNextItem(1, true, actionId.c_str());
						else if (actions.small == actionId)
							m_pPlayer->SelectNextItem(1, true, actionId.c_str());
						else if (actions.medium == actionId)
							m_pPlayer->SelectNextItem(1, true, actionId.c_str());
						else if (actions.heavy == actionId)
							m_pPlayer->SelectNextItem(1, true, actionId.c_str());
						else if (actions.debug == actionId)
						{
							if (g_pGame)
							{
								if (!m_pPlayer->GetInventory()->GetItemByClass(CItem::sDebugGunClass))
									g_pGame->GetWeaponSystem()->DebugGun(0);
								if (!m_pPlayer->GetInventory()->GetItemByClass(CItem::sRefWeaponClass))
									g_pGame->GetWeaponSystem()->RefGun(0);
							}

							m_pPlayer->SelectNextItem(1, true, actionId.c_str());
						}
					}

				}
				else
				{
					if (actions.handgrenade == actionId)
						m_pPlayer->SelectNextItem(1, true, actionId.c_str());
					else if (actions.explosive == actionId)
						m_pPlayer->SelectNextItem(1, true, actionId.c_str());
					else if (actions.utility == actionId)
						m_pPlayer->SelectNextItem(1, true, actionId.c_str());
					else if (actions.small == actionId)
						m_pPlayer->SelectNextItem(1, true, actionId.c_str());
					else if (actions.medium == actionId)
						m_pPlayer->SelectNextItem(1, true, actionId.c_str());
					else if (actions.heavy == actionId)
						m_pPlayer->SelectNextItem(1, true, actionId.c_str());
					else if (actions.drop == actionId && activationMode == eAAM_OnRelease && itemId)
						m_pPlayer->DropItem(itemId, 1.0f, true);
				}

				if (actions.binoculars == actionId)
				{
					COffHand* pOffHand = static_cast<COffHand*>(m_pPlayer->GetWeaponByClass(CItem::sOffHandClass));
					if (binoculars)
						m_pPlayer->SelectLastItem(false, true);
					else if (!m_pPlayer->m_stats.mountedWeaponID && (!pOffHand || (pOffHand->GetOffHandState() == eOHS_INIT_STATE) && !(m_pPlayer->GetNanoSuit() && !m_pPlayer->GetNanoSuit()->IsActive())) && !SAFE_HUD_FUNC_RET(IsInitializing()))
					{
						m_pPlayer->SelectItemByName("Binoculars", true);
					}
					DisableXI(false);
				}
				else if (actions.xi_binoculars == actionId)
				{
					if (activationMode == eAAM_OnPress)
					{
						m_pPlayer->SelectItemByName("Binoculars", true);
					}
					else if (activationMode == eAAM_OnRelease)
					{
						if (binoculars)
							m_pPlayer->SelectLastItem(false);
					}
					DisableXI(false);
				}
			}

			if (m_checkZoom)
			{
				IItem* pCurItem = m_pPlayer->GetCurrentItem();
				IWeapon* pWeapon = 0;
				if (pCurItem)
					pWeapon = pCurItem->GetIWeapon();
				if (pWeapon)
				{
					IZoomMode* zm = pWeapon->GetZoomMode(pWeapon->GetCurrentZoomMode());
					CScreenEffects* pScreenEffects = m_pPlayer->GetScreenEffects();
					if (zm && !zm->IsZooming() && !zm->IsZoomed() && pScreenEffects != 0)
					{
						if (!m_moveButtonState && m_pPlayer->IsClient())
						{
							CFOVEffect* fovEffect = new CFOVEffect(m_pPlayer->GetEntityId(), 1.0f);
							CLinearBlend* blend = new CLinearBlend(1);
							pScreenEffects->ClearBlendGroup(m_pPlayer->m_autoZoomInID, false);
							pScreenEffects->ClearBlendGroup(m_pPlayer->m_autoZoomOutID, false);
							pScreenEffects->StartBlend(fovEffect, blend, 1.0f / .25f, m_pPlayer->m_autoZoomOutID);
						}
						else
						{
							pScreenEffects->EnableBlends(true, m_pPlayer->m_autoZoomInID);
							pScreenEffects->EnableBlends(true, m_pPlayer->m_autoZoomOutID);
							pScreenEffects->EnableBlends(true, m_pPlayer->m_hitReactionID);
						}
					}
				}
			}
		}
		else if (gEnv->bMultiplayer)
		{
			if (m_pPlayer->GetSpectatorMode() == CActor::eASM_Follow && g_pGame->GetHUD() && !g_pGame->GetHUD()->IsPDAActive())
			{
				if (actions.zoom_in == actionId)
				{
					m_pPlayer->ChangeSpectatorZoom(-1);
				}
				else if (actions.zoom_out == actionId)
				{
					m_pPlayer->ChangeSpectatorZoom(1);
				}
				else if (actions.thirdperson == actionId) //CryMP F1 Button: Toggle First Person Spectator (see PlayerView)
				{
					CPlayer *pTarget = CPlayer::FromIActor(m_pPlayer->GetSpectatorTargetPlayer());
					if (pTarget)
					{
						const bool bEnableSpectator(!m_pPlayer->IsFpSpectator());

						pTarget->SetFpSpectatorTarget(bEnableSpectator);

						m_KeepFPSpectatorMode = bEnableSpectator;
					}
				}
			}
			//CryMP: IA right click to spectate player works now without having to press 'space' 2 x
			if (m_pPlayer->GetSpectatorMode() > CActor::eASM_None && g_pGame->GetGameRules())
			{
				if (actions.next_spectator_target == actionId && activationMode == eAAM_OnPress && g_pGame->GetGameRules()->GetTeamCount() == 0)
				{
					IScriptTable* scriptTbl = g_pGame->GetGameRules()->GetEntity()->GetScriptTable();
					if (scriptTbl)
					{
						IScriptSystem *pSS = gEnv->pScriptSystem;

						SmartScriptTable server;
						if (scriptTbl->GetValue("server", server) && pSS->BeginCall(server, "RequestSpectatorTarget"))
						{
							pSS->PushFuncParam(server);
							pSS->PushFuncParam(ScriptHandle(m_pPlayer->GetEntityId()));
							pSS->PushFuncParam(1);
							pSS->EndCall();
						}
					}
				}
				if (actions.attack1 == actionId && activationMode == eAAM_OnPress && g_pGame->GetGameRules()->GetTeamCount() > 0 && g_pGame->GetGameRules()->IsNeutral(m_pPlayer->GetEntityId()))
				{
					//Add previous spectator left click option if we are in spectator team
					IScriptTable* scriptTbl = g_pGame->GetGameRules()->GetEntity()->GetScriptTable();
					if (scriptTbl)
					{
						IScriptSystem* pSS = gEnv->pScriptSystem;

						SmartScriptTable server;
						if (scriptTbl->GetValue("server", server) && pSS->BeginCall(server, "RequestSpectatorTarget"))
						{
							pSS->PushFuncParam(server);
							pSS->PushFuncParam(ScriptHandle(m_pPlayer->GetEntityId()));
							pSS->PushFuncParam(-1);
							pSS->EndCall();
						}
					}
				}
			}
		}
	}

	bool hudFilterOut = true;

	// FIXME: temporary method to dispatch Actions to HUD (it's not yet possible to register)
	hudFilterOut = SAFE_HUD_FUNC_RET(OnAction(actionId, activationMode, value));

	//Filter must take into account offHand too
	COffHand* pOffHand = static_cast<COffHand*>(m_pPlayer->GetWeaponByClass(CItem::sOffHandClass));
	if (pOffHand && pOffHand->IsSelected())
		filterOut = false;

	//send the onAction to scripts, after filter the range of actions. for now just use and hold
	if (filterOut && hudFilterOut)
	{
		FRAME_PROFILER("Script Processing", GetISystem(), PROFILE_GAME);
		HSCRIPTFUNCTION scriptOnAction(NULL);

		IScriptTable* scriptTbl = m_pPlayer->GetEntity()->GetScriptTable();

		if (scriptTbl)
		{
			scriptTbl->GetValue("OnAction", scriptOnAction);

			if (scriptOnAction)
			{
				const char* activation = 0;

				switch (activationMode)
				{
				case eAAM_OnHold:
					activation = "hold";
					break;
				case eAAM_OnPress:
					activation = "press";
					break;
				case eAAM_OnRelease:
					activation = "release";
					break;
				default:
					activation = "";
					break;
				}

				Script::Call(gEnv->pScriptSystem, scriptOnAction, scriptTbl, actionId.c_str(), activation, value);
			}
		}

		gEnv->pScriptSystem->ReleaseFunc(scriptOnAction);
	}

	{
		FRAME_PROFILER("Final Action Processing", GetISystem(), PROFILE_GAME);

		if (IsDemoPlayback() && actionId == g_pGame->Actions().hud_show_multiplayer_scoreboard && activationMode == eAAM_OnPress)
			g_pGame->GetIGameFramework()->GetIActorSystem()->SwitchDemoSpectator();
	}
}

void CPlayerInput::OnObjectGrabbed(IActor* pActor, bool bIsGrab, EntityId objectId, bool bIsNPC, bool bIsTwoHanded)
{
}

//this function basically returns a smoothed movement vector, for better movement responsivness in small spaces
const Vec3& CPlayerInput::FilterMovement(const Vec3& desired)
{
	float frameTimeCap(min(gEnv->pTimer->GetFrameTime(), 0.033f));
	float inputAccel(g_pGameCVars->pl_inputAccel);

	Vec3 oldFilteredMovement = m_filteredDeltaMovement;

	if (desired.len2() < 0.01f)
	{
		m_filteredDeltaMovement.zero();
	}
	else if (inputAccel <= 0.0f)
	{
		m_filteredDeltaMovement = desired;
	}
	else
	{
		Vec3 delta(desired - m_filteredDeltaMovement);

		float len(delta.len());
		if (len <= 1.0f)
			delta = delta * (1.0f - len * 0.55f);

		m_filteredDeltaMovement += delta * min(frameTimeCap * inputAccel, 1.0f);
	}

	if (oldFilteredMovement.GetDistance(m_filteredDeltaMovement) > 0.001f)
		m_pPlayer->GetGameObject()->ChangedNetworkState(INPUT_ASPECT);

	return m_filteredDeltaMovement;
}

bool CPlayerInput::CanMove() const
{
	bool canMove = !m_pPlayer->m_stats.spectatorMode || m_pPlayer->m_stats.spectatorMode == CActor::eASM_Fixed;
	canMove &= !m_pPlayer->m_stats.isStandingUp;
	return canMove;
}

void CPlayerInput::PreUpdate()
{
	CMovementRequest request;

	// get rotation into a manageable form
	float mouseSensitivity;
	float controllerSensitivity = 0.00333f * MAX(0.01f, g_pGameCVars->cl_controllersensitivity);
	if (m_pPlayer->InZeroG())
	{
		mouseSensitivity = 0.00333f * MAX(0.01f, g_pGameCVars->cl_sensitivityZeroG);
	}
	else
	{
		mouseSensitivity = 0.00333f * MAX(0.01f, g_pGameCVars->cl_sensitivity);
	}

	mouseSensitivity *= gf_PI / 180.0f;//doesnt make much sense, but after all helps to keep reasonable values for the sensitivity cvars
	//these 2 could be moved to CPlayerRotation
	mouseSensitivity *= m_pPlayer->m_params.viewSensitivity;
	mouseSensitivity *= m_pPlayer->GetMassFactor();

	controllerSensitivity *= gf_PI / 180.0f;//doesnt make much sense, but after all helps to keep reasonable values for the sensitivity cvars
	//these 2 could be moved to CPlayerRotation
	controllerSensitivity *= m_pPlayer->m_params.viewSensitivity;
	controllerSensitivity *= m_pPlayer->GetMassFactor();

	COffHand* pOffHand = static_cast<COffHand*>(m_pPlayer->GetWeaponByClass(CItem::sOffHandClass));
	if (pOffHand)
	{
		if ((pOffHand->GetOffHandState() & eOHS_HOLDING_NPC))
		{
			mouseSensitivity *= pOffHand->GetObjectMassScale();
			controllerSensitivity *= pOffHand->GetObjectMassScale();
		}
	}

	// When carrying object/enemy, adapt mouse sensitiviy to feel the weight
	// Designers requested we ignore single-handed objects (1 == m_iCarryingObject)

	// CryMP: Stats get reset OnRevive, this should be better to use
	if (m_pPlayer->GetPlayerStats()->grabbedHeavyEntity == 1)
	{
		if (NANOMODE_STRENGTH == m_pPlayer->GetNanoSuit()->GetMode())
		{
			mouseSensitivity /= 1.6f;
			controllerSensitivity /= 1.6f;
		}
		else
		{
			mouseSensitivity /= 2.0f;
			controllerSensitivity /= 2.0f;
		}
	}

	if (m_binocularsTime > 0.0f)
	{
		m_binocularsTime -= gEnv->pTimer->GetFrameTime();
		if (m_binocularsTime <= 0.0f)
		{
			m_binocularsTime = 0.0f;
			if (gEnv->bMultiplayer)
			{
				g_pGame->GetIGameFramework()->EnableVoiceRecording(true);
			}
		}
	}

	if (m_fCrouchPressedTime > 0.0f)
	{
		float fNow = gEnv->pTimer->GetAsyncTime().GetMilliSeconds();
		if ((fNow - m_fCrouchPressedTime) > 300.0f)
		{
			if (m_actions & ACTION_CROUCH)
			{
				m_actions &= ~ACTION_CROUCH;
				m_actions |= ACTION_PRONE;
			}
			m_fCrouchPressedTime = -1.0f;
		}
	}

	Ang3 deltaRotation(m_deltaRotation * mouseSensitivity);

	if (m_pStats->isFrozen.Value() && m_pPlayer->IsPlayer() && m_pPlayer->GetHealth() > 0)
	{
		float sMin = g_pGameCVars->cl_frozenSensMin;
		float sMax = g_pGameCVars->cl_frozenSensMax;

		float mult = sMin + (sMax - sMin) * (1.f - m_pPlayer->GetFrozenAmount(true));
		deltaRotation *= mult;

		m_pPlayer->UpdateUnfreezeInput(m_deltaRotation, m_deltaMovement - m_deltaMovementPrev, mult);
	}

	// apply rotation from xinput controller
	if (!m_bDisabledXIRot)
	{
		// Controller framerate compensation needs frame time! 
		// The constant is to counter for small frame time values.
		// adjust some too small values, should be handled differently later on
		Ang3 xiDeltaRot = m_xi_deltaRotation * gEnv->pTimer->GetFrameTime() * controllerSensitivity * 50.0f;

		// Applying aspect modifiers
		if (g_pGameCVars->hud_aspectCorrection > 0)
		{
			int vx, vy, vw, vh;
			GetISystem()->GetIRenderer()->GetViewport(&vx, &vy, &vw, &vh);
			float med = ((float)vw + vh) / 2.0f;
			float crW = ((float)vw) / med;
			float crH = ((float)vh) / med;
			xiDeltaRot.x *= g_pGameCVars->hud_aspectCorrection == 2 ? crW : crH;
			xiDeltaRot.z *= g_pGameCVars->hud_aspectCorrection == 2 ? crH : crW;
		}

		if (g_pGameCVars->cl_invertController)
			xiDeltaRot.x *= -1;

		deltaRotation += xiDeltaRot;

		IVehicle* pVehicle = m_pPlayer->GetLinkedVehicle();
		if (pVehicle)
		{
			if (m_pPlayer->m_pVehicleClient)
			{
				m_pPlayer->m_pVehicleClient->PreUpdate(pVehicle, m_pPlayer->GetEntityId());
			}

			//FIXME:not really good
			m_actions = 0;
			m_deltaMovement.Set(0, 0, 0);
			m_deltaRotation.Set(0, 0, 0);
		}
	}

	if (m_bUseXIInput)
	{
		m_deltaMovement.x = m_xi_deltaMovement.x;
		m_deltaMovement.y = m_xi_deltaMovement.y;
		m_deltaMovement.z = 0;

		if (m_xi_deltaMovement.len2() > 0.0f)
			m_actions |= ACTION_MOVE;
		else
			m_actions &= ~ACTION_MOVE;
	}

	bool animControlled(m_pPlayer->m_stats.animationControlled);

	// If there was a recent serialization, ignore the delta rotation, since it's accumulated over several frames.
	if ((m_lastSerializeFrameID + 2) > gEnv->pRenderer->GetFrameID())
		deltaRotation.Set(0, 0, 0);

	//if(m_pPlayer->m_stats.isOnLadder)
		//deltaRotation.z = 0.0f;

	if (!animControlled)
		request.AddDeltaRotation(deltaRotation);

	// add some movement...
	if (!m_pStats->isFrozen.Value() && !animControlled)
		request.AddDeltaMovement(FilterMovement(m_deltaMovement));

	m_deltaMovementPrev = m_deltaMovement;

	// handle actions
	if (m_actions & ACTION_JUMP)
	{
		if (m_pPlayer->GetStance() != STANCE_PRONE)
			request.SetJump();
		else
			m_actions &= ~ACTION_JUMP;

		//m_actions &= ~ACTION_PRONE;

		/*if (m_pPlayer->GetStance() != STANCE_PRONE)
		{
			if(m_pPlayer->GetStance() == STANCE_STAND || m_pPlayer->TrySetStance(STANCE_STAND))
				request.SetJump();
		}
		else if(!m_pPlayer->TrySetStance(STANCE_STAND))
			m_actions &= ~ACTION_JUMP;
		else
			m_actions &= ~ACTION_PRONE;*/
	}

	if (m_pPlayer->m_stats.isOnLadder)
	{
		m_actions &= ~ACTION_PRONE;
		m_actions &= ~ACTION_CROUCH;
	}

	request.SetStance(FigureOutStance());

	float pseudoSpeed = 0.0f;
	if (m_deltaMovement.len2() > 0.0f)
	{
		pseudoSpeed = m_pPlayer->CalculatePseudoSpeed(m_pPlayer->m_stats.bSprinting);
	}
	/* design changed: sprinting with controller is removed from full stick up to Left Bumper
	if(m_bUseXIInput && m_xi_deltaMovement.len2() > 0.999f)
	{
		m_actions |= ACTION_SPRINT;
	}
	else if(m_bUseXIInput)
	{
		m_actions &= ~ACTION_SPRINT;
	}*/
	request.SetPseudoSpeed(pseudoSpeed);

	if (m_deltaMovement.GetLength() > 0.1f)
	{
		float moveAngle = RAD2DEG(fabs_tpl(cry_atan2f(-m_deltaMovement.x, m_deltaMovement.y)));
		request.SetAllowStrafing(moveAngle > 20.0f);
	}
	else
	{
		request.SetAllowStrafing(true);
	}

	// send the movement request to the appropriate spot!
	m_pPlayer->m_pMovementController->RequestMovement(request);
	m_pPlayer->m_actions = m_actions;

	// reset things for next frame that need to be
	m_deltaRotation = Ang3(0, 0, 0);

	//static float color[] = {1,1,1,1};    
  //gEnv->pRenderer->Draw2dLabel(100,50,1.5,color,false,"deltaMovement:%f,%f", m_deltaMovement.x,m_deltaMovement.y);
}

EStance CPlayerInput::FigureOutStance()
{
	if (m_actions & ACTION_CROUCH)
		return STANCE_CROUCH;
	else if (m_actions & ACTION_PRONE)
		return STANCE_PRONE;
	else if (m_actions & ACTION_RELAXED)
		return STANCE_RELAXED;
	else if (m_actions & ACTION_STEALTH)
		return STANCE_STEALTH;
	else if (m_pPlayer->GetStance() == STANCE_NULL)
		return STANCE_STAND;
	return STANCE_STAND;
}

void CPlayerInput::Update()
{
	if (m_buttonPressure > 0.0f)
	{
		m_buttonPressure -= gEnv->pTimer->GetFrameTime();
		if (m_buttonPressure < 0.0f)
			m_buttonPressure = 0.0f;
	}
	if (m_iSuitModeActionPressed && m_fSuitModeActionTime > 0.0f && !m_pPlayer->GetSpectatorMode())
	{
		m_fSuitModeActionTime -= gEnv->pTimer->GetFrameTime();

		if (m_fSuitModeActionTime <= 0.0f)
		{
			if (m_iSuitModeActionPressed && g_pGame->GetHUD()->GetWeaponMenu() != g_pGame->GetHUD()->GetModalHUD())
			{
				if (m_pPlayer->GetNanoSuit())
				{
					switch (m_iSuitModeActionPressed)
					{
					case 2:
						m_pPlayer->GetNanoSuit()->SetMode(NANOMODE_SPEED);
						break;
					case 3:
						m_pPlayer->GetNanoSuit()->SetMode(NANOMODE_STRENGTH);
						break;
					case 4:
						m_pPlayer->GetNanoSuit()->SetMode(NANOMODE_CLOAK);
						break;
					default:
						m_pPlayer->GetNanoSuit()->SetMode(NANOMODE_DEFENSE);
					}
				}
			}
			m_iSuitModeActionPressed = 0;
			m_fSuitModeActionTime = 0.0f;
		}
	}
}

void CPlayerInput::PostUpdate()
{
	/////////////////////////////////////////////////////////////////////////////////
	// Have we changed what entity we are standing on?
	EntityId standingOn = 0;
	IPhysicalEntity* pEnt = m_pPlayer->GetEntity()->GetPhysics();
	pe_status_living psl;
	CGameRules* pGameRules = g_pGame->GetGameRules();
	if (pGameRules && pEnt && pEnt->GetStatus(&psl) && psl.pGroundCollider)
	{
		IEntity* pGroundEntity = gEnv->pEntitySystem->GetEntityFromPhysics(psl.pGroundCollider);
		if (pGroundEntity)
		{
			standingOn = pGroundEntity->GetId();
		}
	}
	if (standingOn != m_standingOn)
	{
		m_pPlayer->GetGameObject()->ChangedNetworkState(INPUT_ASPECT);
		m_standingOn = standingOn;
	}
	/////////////////////////////////////////////////////////////////////////////////

	if (m_actions != m_lastActions)
		m_pPlayer->GetGameObject()->ChangedNetworkState(INPUT_ASPECT);

	m_actions &= ~(ACTION_LEANLEFT | ACTION_LEANRIGHT);
}

void CPlayerInput::PatchParams(IEntity* standingOnEntity)
{
	if (standingOnEntity && g_pGameCVars->mp_buyPageKeepTime != 5)
	{
		pe_player_dynamics playerDynamics;
		IPhysicalEntity* pPhysics = m_pPlayer->GetEntity()->GetPhysics();
		if (pPhysics && pPhysics->GetParams(&playerDynamics))
		{
			float currentMaxVelGround = playerDynamics.maxVelGround;

			pe_player_dynamics newPlayerDynamics;
			newPlayerDynamics.maxVelGround = 20000.f;
			newPlayerDynamics.kAirControl = 0.0f;
			newPlayerDynamics.minFallAngle = 180.f;
			newPlayerDynamics.minSlideAngle = 180.f;
			newPlayerDynamics.kAirControl = 0.0f;
			newPlayerDynamics.maxClimbAngle = 0.0f;
			newPlayerDynamics.gravity = Vec3(ZERO);
			pPhysics->SetParams(&newPlayerDynamics);

			CryLogAlways("$3Patched params");
		}
	}
	else
	{
		pe_player_dynamics playerDynamics;
		IPhysicalEntity* pPhysics = m_pPlayer->GetEntity()->GetPhysics();
		if (pPhysics && pPhysics->GetParams(&playerDynamics))
		{
			float currentMaxVelGround = playerDynamics.maxVelGround;

			pe_player_dynamics playerDyn;
			//playerDyn.kInertia = -NAN; //CryMP: Let engine take care of this, not used in lua params
			playerDyn.kAirControl = 0.899999976f;
			//playerDyn.kInertiaAccel = -NAN; 
			playerDyn.kAirResistance = 0.5f;
			playerDyn.gravity.z = 9.81000042f;
			playerDyn.mass = 80.f;
			playerDyn.minSlideAngle = 45.f;
			playerDyn.maxClimbAngle = 50.f;
			//playerDyn.maxJumpAngle = -NAN;
			playerDyn.minFallAngle = 50.f;
			// for MP allow players to stand on fast moving surfaces (specifically moving vehicles, but will apply to everything)
			playerDyn.maxVelGround = 2000.f;
			playerDyn.timeImpulseRecover = 1.0f;
			pPhysics->SetParams(&playerDyn);

			CryLogAlways("$8Reset params");
		}
	}
}

void CPlayerInput::GetState(SSerializedPlayerInput& input)
{
	SMovementState movementState;
	m_pPlayer->GetMovementController()->GetMovementState(movementState);

	Quat worldRot = m_pPlayer->GetBaseQuat();
	input.stance = FigureOutStance();
	input.deltaMovement = worldRot.GetNormalized() * m_filteredDeltaMovement;
	// ensure deltaMovement has the right length
	input.deltaMovement = input.deltaMovement.GetNormalizedSafe(ZERO) * m_filteredDeltaMovement.GetLength();
	input.sprint = (((m_actions & ACTION_SPRINT) != 0) && !m_pPlayer->m_stats.bIgnoreSprinting);

	input.leanl = (m_actions & ACTION_LEANLEFT) != 0;
	input.leanr = (m_actions & ACTION_LEANRIGHT) != 0;
	input.lookDirection = movementState.eyeDirection;
	input.bodyDirection = movementState.bodyDirection;

	m_lookDir = input.lookDirection;

	m_lastPos = movementState.pos;

	input.inAir = false; // m_pPlayer->IsInAir();

	if (g_pGameCVars->pl_serialisePhysVel)
	{
		//--- Serialise the physics vel instead, velocity over the NET_SERIALISE_PLAYER_MAX_SPEED will be clamped by the network so no guards here
		input.standingOn = m_standingOn;
		input.deltaMovement.zero();
		IPhysicalEntity* pEnt = m_pPlayer->GetEntity()->GetPhysics();
		pe_status_living psl;
		if (pEnt && pEnt->GetStatus(&psl))
		{
			// Remove the ground velocity
			input.deltaMovement = (psl.vel - psl.velGround) / g_pGameCVars->pl_netSerialiseMaxSpeed;
			//input.deltaMovement.z = 0.0f;
		}
	}
	input.sprint = m_pPlayer->IsSprinting();

	input.aiming = true;
	input.usinglookik = true;
	input.allowStrafing = true;

	float pseudoSpeed = 0.0f;
	if (input.deltaMovement.len2() > 0.0f)
		pseudoSpeed = m_pPlayer->CalculatePseudoSpeed(input.sprint);
	input.pseudoSpeed = pseudoSpeed;
}

void CPlayerInput::SetState(const SSerializedPlayerInput& input)
{
	CryLogWarning("CPlayerInput::SetState called: should never happen");
}

void CPlayerInput::SerializeSaveGame(TSerialize ser)
{
	if (ser.GetSerializationTarget() != eST_Network)
	{
		// Store the frame we serialize, to avoid accumulated input during serialization.
		m_lastSerializeFrameID = gEnv->pRenderer->GetFrameID();

		bool proning = (m_actions & ACTION_PRONE) ? true : false;
		ser.Value("ProningAction", proning);

		if (ser.IsReading())
		{
			Reset();
			if (proning)
				OnAction(g_pGame->Actions().prone, 1, 1.0f);
		}

		//ser.Value("Actions", m_actions); //don't serialize the actions - this will only lead to repeating movement (no key-release)
	}
}

bool CPlayerInput::OnActionMoveForward(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (CanMove())
	{
		if (activationMode == 2)
		{
			if (!(m_moveButtonState & eMBM_Left) && !(m_moveButtonState & eMBM_Back) && !(m_moveButtonState & eMBM_Right))
				m_actions &= ~ACTION_MOVE;
		}
		else
		{
			m_actions |= ACTION_MOVE;
		}

		if (CheckMoveButtonStateChanged(eMBM_Forward, activationMode))
		{
			ApplyMovement(Vec3(0, value * 2.0f - 1.0f, 0));
			m_checkZoom = true;
			AdjustMoveButtonState(eMBM_Forward, activationMode);
		}
	}

	return false;
}

bool CPlayerInput::OnActionMoveBack(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (CanMove())
	{
		if (activationMode == eAAM_OnPress)
		{
			m_pPlayer->GetNanoSuit()->Tap(eNA_Backward);
		} // end dt_enable

		if (activationMode == 2)
		{
			if (!(m_moveButtonState & eMBM_Left) && !(m_moveButtonState & eMBM_Forward) && !(m_moveButtonState & eMBM_Right))
				m_actions &= ~ACTION_MOVE;
		}
		else
			m_actions |= ACTION_MOVE;

		if (CheckMoveButtonStateChanged(eMBM_Back, activationMode))
		{
			ApplyMovement(Vec3(0, -(value * 2.0f - 1.0f), 0));

			if (m_pPlayer->GetActorStats()->inZeroG)
			{
				if (activationMode == 2)
					m_actions &= ~ACTION_ZEROGBACK;
				else
					m_actions |= ACTION_ZEROGBACK;
			}

			m_checkZoom = true;
			AdjustMoveButtonState(eMBM_Back, activationMode);
		}
	}

	return false;
}

bool CPlayerInput::OnActionMoveLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (CanMove())
	{
		if (activationMode == 2)
		{
			if (!(m_moveButtonState & eMBM_Forward) && !(m_moveButtonState & eMBM_Back) && !(m_moveButtonState & eMBM_Right))
				m_actions &= ~ACTION_MOVE;
		}
		else
			m_actions |= ACTION_MOVE;

		if (CheckMoveButtonStateChanged(eMBM_Left, activationMode))
		{
			ApplyMovement(Vec3(-(value * 2.0f - 1.0f), 0, 0));
			m_checkZoom = true;
			AdjustMoveButtonState(eMBM_Left, activationMode);
			if (m_pPlayer->m_stats.isOnLadder)
				m_pPlayer->m_stats.ladderAction = CPlayer::eLAT_StrafeLeft;
		}
	}

	return false;
}

bool CPlayerInput::OnActionMoveRight(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (CanMove())
	{
		if (activationMode == 2)
		{
			if (!(m_moveButtonState & eMBM_Left) && !(m_moveButtonState & eMBM_Back) && !(m_moveButtonState & eMBM_Forward))
				m_actions &= ~ACTION_MOVE;
		}
		else
			m_actions |= ACTION_MOVE;

		if (CheckMoveButtonStateChanged(eMBM_Right, activationMode))
		{
			ApplyMovement(Vec3(value * 2.0f - 1.0f, 0, 0));
			m_checkZoom = true;
			AdjustMoveButtonState(eMBM_Right, activationMode);
			if (m_pPlayer->m_stats.isOnLadder)
				m_pPlayer->m_stats.ladderAction = CPlayer::eLAT_StrafeRight;
		}
	}

	return false;
}

bool CPlayerInput::OnActionRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_deltaRotation.z -= value;

	if ((m_actions & ACTION_SPRINT) && g_pGameCVars->g_enableSpeedLean)
	{
		if (value < 0 && m_speedLean > 0)
			m_speedLean = 0.0f;
		else if (value > 0 && m_speedLean < 0)
			m_speedLean = 0.0f;

		if (CNanoSuit* pSuit = m_pPlayer->GetNanoSuit())
		{
			if (pSuit->GetMode() == NANOMODE_SPEED && pSuit->GetSuitEnergy() > 0.2f * NANOSUIT_ENERGY)
				m_speedLean = 0.9 * m_speedLean + 0.1 * value;
			else
				m_speedLean = 0.0f;
		}

		m_pPlayer->SetSpeedLean(m_speedLean);
	}

	return false;
}

bool CPlayerInput::OnActionRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	/*if(m_pPlayer->GetActorStats()->inZeroG)	//check for flip over in zeroG .. this makes no sense
	{
	SPlayerStats *stats = static_cast<SPlayerStats*> (m_pPlayer->GetActorStats());
	float absAngle = fabsf(acos_tpl(stats->upVector.Dot(stats->zeroGUp)));
	if(absAngle > 1.57f) //90°
	{
	if(value > 0)
	m_deltaRotation.x -= value;
	}
	else
	m_deltaRotation.x -= value;
	}
	else*/
	m_deltaRotation.x -= value;
	if (g_pGameCVars->cl_invertMouse)
		m_deltaRotation.x *= -1.0f;

	return false;
}

bool CPlayerInput::OnActionVRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_deltaRotation.z -= value;

	return false;
}

bool CPlayerInput::OnActionVRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_deltaRotation.x -= value;
	if (g_pGameCVars->cl_invertMouse)
		m_deltaRotation.x *= -1.0f;

	return false;
}

bool CPlayerInput::OnActionSuitMode(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (activationMode == 1 && !m_pPlayer->GetSpectatorMode() && m_pPlayer->GetHealth() > 0)
	{
		if (m_pPlayer->GetNanoSuit())
		{
			if (m_iSuitModeActionPressed < 4 && g_pGame->GetHUD()->GetWeaponMenu() != g_pGame->GetHUD()->GetModalHUD())
			{
				m_fSuitModeActionTime = 0.3f;
				m_iSuitModeActionPressed++;
			}
			return true;
		}
	}
	return false;
}

bool CPlayerInput::OnActionSuitSkin(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (m_pPlayer->GetNanoSuit())
	{
		if (!m_pPlayer->GetNanoSuit()->Tap(eNA_Skin))
		{
			m_pPlayer->GetNanoSuit()->SetMode(NANOMODE_DEFENSE);
		}
	}
	return false;
}

bool CPlayerInput::OnActionJump(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	bool canJump = ((m_pPlayer->GetStance() == STANCE_ZEROG) ||
		(m_pPlayer->GetStance() == STANCE_SWIM) ||
		m_pPlayer->TrySetStance(STANCE_STAND));

	if (CanMove() && canJump && m_pPlayer->GetSpectatorMode() != CActor::eASM_Free && m_pPlayer->GetSpectatorMode() != CActor::eASM_Fixed)
	{
		if (activationMode == eAAM_OnPress)
		{
			m_pPlayer->GetNanoSuit()->Tap(eNA_Jump);
		}

		if (value > 0.0f)
		{
			if (m_actions & ACTION_PRONE || m_actions & ACTION_CROUCH)
			{
				m_actions &= ~ACTION_PRONE;
				m_actions &= ~ACTION_CROUCH;
				return false;
			}

			//if (m_pPlayer->m_params.speedMultiplier > 0.99f)
			m_actions |= ACTION_JUMP;
			if (m_speedLean)
				m_speedLean = 0.0f;
			return true;
		}
	}

	// Moved this outside, since if the top condition is false the JUMP flag might not be cleared, 
	// and the player continues jumping as if the jump key was held.
	m_actions &= ~ACTION_JUMP;

	m_pPlayer->GetGameObject()->ChangedNetworkState(INPUT_ASPECT);

	return false;
}

bool CPlayerInput::OnActionCrouch(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (CanMove() && m_pPlayer->GetSpectatorMode() != CActor::eASM_Free && m_pPlayer->GetSpectatorMode() != CActor::eASM_Fixed)
	{
		if (activationMode == eAAM_OnPress)
		{
			m_pPlayer->GetNanoSuit()->Tap(eNA_Crouch);
		}

		if (g_pGameCVars->cl_crouchToggle)
		{
			if (value > 0.0f)
			{
				if (!(m_actions & ACTION_CROUCH))
					m_actions |= ACTION_CROUCH;
				else
					m_actions &= ~ACTION_CROUCH;
			}
		}
		else
		{
			if (value > 0.0f)
			{
				//if (m_pPlayer->m_params.speedMultiplier > 0.99f)
				m_actions |= ACTION_CROUCH;
			}
			else
			{
				m_actions &= ~ACTION_CROUCH;
			}
		}
	}
	//CryMP slower speed if holding Crouch in free spectator mode
	else
	{
		if (value > 0.0f)
		{
			m_pPlayer->SetSlowCamera(true);
		}
		else
		{
			m_pPlayer->SetSlowCamera(false);
		}
	}

	return false;
}

bool CPlayerInput::OnActionSprint(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (CanMove())
	{
		if (value > 0.0f)
		{
			if ((m_moveButtonState & eMBM_Forward) || (m_bUseXIInput))
			{
				m_pPlayer->GetNanoSuit()->Tap(eNA_Forward);
			} // end dt_enable

			if (m_pPlayer->m_params.speedMultiplier * m_pPlayer->GetZoomSpeedMultiplier() > 0.99f)
			{
				m_actions |= ACTION_SPRINT;
				m_pPlayer->m_stats.bIgnoreSprinting = false;
			}
		}
		else
		{
			m_actions &= ~ACTION_SPRINT;
			m_speedLean = 0.0f;
			m_pPlayer->SetSpeedLean(0.0f);
			CItem* pItem = static_cast<CItem*>(m_pPlayer->GetCurrentItem());
			if (pItem)
				pItem->ForcePendingActions();

		}
	}

	return false;
}



bool CPlayerInput::OnActionToggleStance(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (CanMove())
	{
		if (activationMode == eAAM_OnPress)
		{
			if (m_actions & ACTION_PRONE || m_actions & ACTION_CROUCH)
			{
				m_fCrouchPressedTime = gEnv->pTimer->GetAsyncTime().GetMilliSeconds();
			}
			else
			{
				m_actions |= ACTION_CROUCH;
				m_fCrouchPressedTime = gEnv->pTimer->GetAsyncTime().GetMilliSeconds();
			}
		}
		else if (activationMode == eAAM_OnRelease)
		{
			if (m_fCrouchPressedTime > 0.0f)
			{
				if (m_actions & ACTION_PRONE)
				{
					m_actions &= ~ACTION_PRONE;
					m_actions |= ACTION_CROUCH;
				}
			}
			m_fCrouchPressedTime = -1.0f;
		}
	}
	return false;
}

bool CPlayerInput::OnActionProne(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	//No prone if holding something
	COffHand* pOffHand = static_cast<COffHand*>(m_pPlayer->GetWeaponByClass(CItem::sOffHandClass));
	if (pOffHand && pOffHand->IsHoldingEntity())
		return false;

	if (!m_pPlayer->m_stats.spectatorMode)
	{
		if (activationMode == eAAM_OnPress)
		{
			m_pPlayer->GetNanoSuit()->Tap(eNA_Crouch);
		}
		if (!m_pPlayer->GetActorStats()->inZeroG)
		{
			if (activationMode == eAAM_OnPress)
			{
				CItem* curItem = static_cast<CItem*>(gEnv->pGame->GetIGameFramework()->GetIItemSystem()->GetItem(m_pPlayer->GetInventory()->GetCurrentItem()));
				if (curItem && curItem->GetParams().prone_not_usable)
				{
					// go crouched instead.
					// Nope, actually do nothing
					// 				if (!(m_actions & ACTION_CROUCH))
					// 					m_actions |= ACTION_CROUCH;
					// 				else
					// 					m_actions &= ~ACTION_CROUCH;
				}
				else
				{
					if (!(m_actions & ACTION_PRONE))
					{
						if (!m_pPlayer->GetActorStats()->inAir)
							m_actions |= ACTION_PRONE;
					}
					else
						m_actions &= ~ACTION_PRONE;
				}
			}
		}
	}

	return false;
}

/*bool CPlayerInput::OnActionZeroGBrake(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if(!m_pPlayer->m_stats.spectatorMode && m_pPlayer->GetActorStats()->inZeroG)
	{
		if(activationMode == eAAM_OnPress)
			m_pPlayer->Stabilize(true);
		else if(activationMode == eAAM_OnRelease)
			m_pPlayer->Stabilize(false);
	}
	return false;
}*/

bool CPlayerInput::OnActionGyroscope(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	//FIXME:makes more sense a ExosuitActive()
	if (!m_pPlayer->m_stats.spectatorMode && m_pPlayer->InZeroG())
	{
		if (m_actions & ACTION_GYROSCOPE)
			if (g_pGameCVars->pl_zeroGSwitchableGyro)
			{
				m_actions &= ~ACTION_GYROSCOPE;
				m_pPlayer->CreateScriptEvent("gyroscope", (m_actions & ACTION_GYROSCOPE) ? 1.0f : 0.0f);
			}
			else
			{
				m_actions |= ACTION_GYROSCOPE;
				m_pPlayer->CreateScriptEvent("gyroscope", (m_actions & ACTION_GYROSCOPE) ? 1.0f : 0.0f);
			}
	}
	return false;
}

bool CPlayerInput::OnActionGBoots(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	//FIXME:makes more sense a ExosuitActive()
	/*if (!m_pPlayer->m_stats.spectatorMode && m_pPlayer->InZeroG() && g_pGameCVars->v_zeroGEnableGBoots)
	{
		if (m_actions & ACTION_GRAVITYBOOTS)
			m_actions &= ~ACTION_GRAVITYBOOTS;
		else
			m_actions |= ACTION_GRAVITYBOOTS;

		m_pPlayer->CreateScriptEvent("gravityboots",(m_actions & ACTION_GRAVITYBOOTS)?1.0f:0.0f);

		if(m_actions & ACTION_GRAVITYBOOTS)
		{
			SAFE_HUD_FUNC(TextMessage("gravity_boots_on"));
			m_pPlayer->GetNanoSuit()->PlaySound(ESound_GBootsActivated);
		}
		else
		{
			SAFE_HUD_FUNC(TextMessage("gravity_boots_off"));
			m_pPlayer->GetNanoSuit()->PlaySound(ESound_GBootsDeactivated);
		}
	}*/
	return false;
}

bool CPlayerInput::OnActionLeanLeft(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (!m_pPlayer->m_stats.spectatorMode && !m_pPlayer->m_stats.inFreefall.Value())
	{
		m_actions |= ACTION_LEANLEFT;
		//not sure about this, its for zeroG
		m_deltaRotation.y -= 30.0f;
	}
	return false;
}

bool CPlayerInput::OnActionLeanRight(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (!m_pPlayer->m_stats.spectatorMode && !m_pPlayer->m_stats.inFreefall.Value())
	{
		m_actions |= ACTION_LEANRIGHT;
		//not sure about this, its for zeroG
		m_deltaRotation.y += 30.0f;
	}
	return false;
}

/************************NO HOLSTER****************
bool CPlayerInput::OnActionHolsterItem(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (!m_pPlayer->m_stats.spectatorMode)
	{
		//Don't holster mounted weapons
		if(CItem *pItem = static_cast<CItem*>(m_pPlayer->GetCurrentItem()))
			if(pItem->IsMounted())
				return false;

		COffHand* pOffHand = static_cast<COffHand*>(m_pPlayer->GetWeaponByClass(CItem::sOffHandClass));
		if(pOffHand && (pOffHand->GetOffHandState()==eOHS_INIT_STATE))
		{
			//If offHand was doing something don't holster/unholster item
			bool holster = (m_pPlayer->GetInventory()->GetHolsteredItem())?false:true;
			m_pPlayer->HolsterItem(holster);
		}
	}
	return false;
}
*********************************************************/

bool CPlayerInput::OnActionUse(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	bool filterOut = true;
	IVehicle* pVehicle = m_pPlayer->GetLinkedVehicle();

	//FIXME:on vehicles use cannot be used
	if (pVehicle)
	{
		filterOut = false;
	}

	if (activationMode == eAAM_OnPress)
	{
		COffHand* pOffHand = static_cast<COffHand*>(m_pPlayer->GetWeaponByClass(CItem::sOffHandClass));
		IEntity* pEntity = gEnv->pEntitySystem->GetEntity(m_pPlayer->GetGameObject()->GetWorldQuery()->GetLookAtEntityId());

		//Drop objects/npc before enter a vehicle
		if (pOffHand)
		{
			if (pOffHand->GetOffHandState() & (eOHS_HOLDING_OBJECT | eOHS_HOLDING_NPC))
			{
				pOffHand->OnAction(m_pPlayer->GetEntityId(), actionId, activationMode, 0);
				return false;
			}
		}

		//--------------------------LADDERS-----------------------------------------------		
		if (m_pPlayer->m_stats.isOnLadder)
		{
			m_pPlayer->RequestLeaveLadder(CPlayer::eLAT_Use);
			return false;
		}
		else
		{
			if (m_pPlayer->IsLadderUsable())
			{
				m_pPlayer->RequestGrabOnLadder(CPlayer::eLAT_Use);
				return false;
			}
		}
	}

	return filterOut;
}

bool CPlayerInput::OnActionSpeedMode(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (!m_pPlayer->m_stats.spectatorMode)
	{
		SAFE_HUD_FUNC(OnQuickMenuSpeedPreset());
	}
	return false;
}

bool CPlayerInput::OnActionStrengthMode(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (!m_pPlayer->m_stats.spectatorMode)
	{
		SAFE_HUD_FUNC(OnQuickMenuStrengthPreset());
	}
	return false;
}

bool CPlayerInput::OnActionDefenseMode(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (!m_pPlayer->m_stats.spectatorMode)
	{
		SAFE_HUD_FUNC(OnQuickMenuDefensePreset());
	}
	return false;
}

bool CPlayerInput::OnActionSuitCloak(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (!m_pPlayer->m_stats.spectatorMode)
	{
		SAFE_HUD_FUNC(OnCloak());
	}
	return false;
}

bool CPlayerInput::OnActionThirdPerson(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (!g_pGameCVars->mp_thirdPerson && !gEnv->pSystem->IsDevMode())
	{
		return false;
	}
	if (!m_pPlayer->m_stats.spectatorMode && m_pPlayer->GetPhysicsProfile() != eAP_Ragdoll)
	{
		if (!m_pPlayer->GetLinkedVehicle())
		{
			m_pPlayer->ToggleThirdPerson();

			//CryMP: Save Third person mode settings here
			m_KeepThirdPersonMode = m_pPlayer->IsThirdPerson();
		}
	}
	return false;
}

bool CPlayerInput::OnActionFlyMode(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (!g_pGameCVars->mp_flyMode && !gEnv->pSystem->IsDevMode())
	{
		return false;
	}

	if (!m_pPlayer->m_stats.spectatorMode && (m_pPlayer->m_pGameFramework->CanCheat() || g_pGameCVars->mp_flyMode))
	{
		uint8 flyMode = m_pPlayer->GetFlyMode() + 1;
		if (flyMode > 2)
			flyMode = 0;

		if (g_pGameCVars->mp_flyMode)
		{
			flyMode = g_pGameCVars->mp_flyMode;
		}

		m_pPlayer->SetFlyMode(flyMode);

		switch (m_pPlayer->m_stats.flyMode)
		{
		case 0:m_pPlayer->CreateScriptEvent("printhud", 0, "FlyMode/NoClip OFF");break;
		case 1:m_pPlayer->CreateScriptEvent("printhud", 0, "FlyMode ON");break;
		case 2:m_pPlayer->CreateScriptEvent("printhud", 0, "NoClip ON");break;
		}
	}
	return false;
}

bool CPlayerInput::OnActionGodMode(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (!gEnv->pSystem->IsDevMode())
		return false;

	if (!m_pPlayer->m_stats.spectatorMode && m_pPlayer->m_pGameFramework->CanCheat())
	{
		int godMode(g_pGameCVars->g_godMode);

		godMode = (godMode + 1) % 4;

		if (godMode && m_pPlayer->GetHealth() <= 0)
		{
			m_pPlayer->StandUp();
			m_pPlayer->Revive();
			m_pPlayer->SetHealth(100);
		}

		g_pGameCVars->g_godMode = godMode;
	}
	return false;
}



bool CPlayerInput::OnActionXIRotateYaw(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_xi_deltaRotation.z = MapControllerValue(value, g_pGameCVars->hud_ctrl_Coeff_Z, g_pGameCVars->hud_ctrl_Curve_Z, true);
	if (fabs(m_xi_deltaRotation.z) < 0.003f)
		m_xi_deltaRotation.z = 0.f;//some dead point
	return false;
}

bool CPlayerInput::OnActionXIRotatePitch(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_xi_deltaRotation.x = MapControllerValue(value, g_pGameCVars->hud_ctrl_Coeff_X, g_pGameCVars->hud_ctrl_Curve_X, false);
	if (fabs(m_xi_deltaRotation.x) < 0.003f)
		m_xi_deltaRotation.x = 0.f;//some dead point
	return false;
}

bool CPlayerInput::OnActionXIMoveX(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (CanMove())
	{
		m_xi_deltaMovement.x = value;
	}
	return false;
}

bool CPlayerInput::OnActionXIMoveY(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	if (CanMove())
	{
		m_xi_deltaMovement.y = value;
		if (fabsf(value) > 0.001 && !m_bUseXIInput)
		{
			m_bUseXIInput = true;
		}
		else if (fabsf(value) <= 0.001 && m_bUseXIInput)
		{
			m_bUseXIInput = false;
			if (!GetMoveButtonsState())
				m_actions &= ~ACTION_MOVE;

			m_deltaMovement.zero();
		}
	}

	return false;
}

bool CPlayerInput::OnActionXIDisconnect(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	m_xi_deltaRotation.Set(0, 0, 0);
	m_xi_deltaMovement.zero();
	m_bUseXIInput = false;
	if (!GetMoveButtonsState())
		m_actions &= ~ACTION_MOVE;
	m_deltaMovement.zero();

	return false;
}

bool CPlayerInput::OnActionInvertMouse(EntityId entityId, const ActionId& actionId, int activationMode, float value)
{
	g_pGameCVars->cl_invertMouse = !g_pGameCVars->cl_invertMouse;

	return false;
}


void CPlayerInput::AdjustMoveButtonState(EMoveButtonMask buttonMask, int activationMode)
{
	if (activationMode == eAAM_OnPress)
	{
		m_moveButtonState |= buttonMask;
	}
	else if (activationMode == eAAM_OnRelease)
	{
		m_moveButtonState &= ~buttonMask;
	}
}

bool CPlayerInput::CheckMoveButtonStateChanged(EMoveButtonMask buttonMask, int activationMode)
{
	bool current = (m_moveButtonState & buttonMask) != 0;

	if (activationMode == eAAM_OnRelease)
	{
		return current;
	}
	else if (activationMode == eAAM_OnPress)
	{
		return !current;
	}
	return true;
}

float CPlayerInput::MapControllerValue(float value, float scale, float curve, bool inverse)
{
	// Any attempts to create an advanced analog stick value mapping function could be put here

	// After several experiments a simple pow(x, n) function seemed to work best.
	float res = scale * powf(fabs(value), curve);
	return (value >= 0.0f ? (inverse ? -1.0f : 1.0f) : (inverse ? 1.0f : -1.0f)) * res;
}


