/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$

 -------------------------------------------------------------------------
  History:
  - 11:8:2004   10:50 : Created by Márcio Martins

*************************************************************************/
#include <cstring>

#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "GameCVars.h"
#include "GameRules.h"
#include "Items/ItemSharedParams.h"

#include "CryCommon/CryAction/IGameObject.h"
#include "CryCommon/CryAction/IActorSystem.h"
#include "CryCommon/CryAction/IItemSystem.h"
#include "Items/Weapons/WeaponSystem.h"
#include "ServerSynchedStorage.h"
#include "Items/ItemString.h"
#include "HUD/HUD.h"
#include "Menus/QuickGame.h"
#include "Environment/BattleDust.h"
#include "NetInputChainDebug.h"

#include "Menus/FlashMenuObject.h"
#include "Menus/MPHub.h"

static void OnChangeThirdPerson(ICVar* pCVar)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());
	if (pPlayer)
	{
		const int iThirdPerson = pCVar->GetIVal();
		if (!iThirdPerson)
		{
			pPlayer->EnableThirdPerson(false);
		}
	}
}

static void OnChangeFlyMode(ICVar* pCVar)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());
	if (pPlayer)
	{
		const int iFlyMode = pCVar->GetIVal();
		pPlayer->SetFlyMode(iFlyMode);
	}
}

static void BroadcastChangeSafeMode(ICVar*)
{
	SGameObjectEvent event(eCGE_ResetMovementController, eGOEF_ToExtensions);
	IEntitySystem* pES = gEnv->pEntitySystem;
	IEntityItPtr pIt = pES->GetEntityIterator();
	while (!pIt->IsEnd())
	{
		if (IEntity* pEnt = pIt->Next())
			if (IActor* pActor = g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor(pEnt->GetId()))
				pActor->HandleEvent(event);
	}
}

static void CmdBulletTimeMode(IConsoleCmdArgs* cmdArgs)
{
	g_pGameCVars->goc_enable = 0;
	g_pGameCVars->goc_tpcrosshair = 0;

	g_pGameCVars->bt_ironsight = 1;
	g_pGameCVars->bt_speed = 0;
	g_pGameCVars->bt_energy_decay = 2.5;
	g_pGameCVars->bt_end_reload = 1;
	g_pGameCVars->bt_end_select = 1;
	g_pGameCVars->bt_end_melee = 0;
}

static void CmdGOCMode(IConsoleCmdArgs* cmdArgs)
{
	g_pGameCVars->goc_enable = 1;
	g_pGameCVars->goc_tpcrosshair = 1;

	g_pGameCVars->bt_ironsight = 1;
	g_pGameCVars->bt_speed = 0;
	g_pGameCVars->bt_energy_decay = 0;
	g_pGameCVars->bt_end_reload = 1;
	g_pGameCVars->bt_end_select = 1;
	g_pGameCVars->bt_end_melee = 0;

	//
	CPlayer* pPlayer = static_cast<CPlayer*>(gEnv->pGame->GetIGameFramework()->GetClientActor());
	if (pPlayer && !pPlayer->IsThirdPerson())
	{
		pPlayer->ToggleThirdPerson();
	}
}

SCVars::SCVars()
{
	std::memset(this, 0, sizeof(SCVars));
}

SCVars::~SCVars()
{
	this->ReleaseCVars();
}

// game related cvars must start with an g_
// game server related cvars must start with sv_
// game client related cvars must start with cl_
// no other types of cvars are allowed to be defined here!
void SCVars::InitCVars(IConsole* pConsole)
{
	const int OPTIONAL_SYNC = 0;

	#define GAMECVAR_REGISTERLIST
		#include "GameCVarsLib.h"
	#undef GAMECVAR_REGISTERLIST

	ca_GameControlledStrafingPtr = pConsole->GetCVar("ca_GameControlledStrafing");
	pl_debug_filter = pConsole->RegisterString("pl_debug_filter", "", VF_CHEAT);

	aln_debug_filter = pConsole->RegisterString("aln_debug_filter", "", VF_CHEAT);

	pAltitudeLimitCVar = pConsole->Register("v_altitudeLimit", &v_altitudeLimit, v_altitudeLimitDefault(), VF_CHEAT, "Used to restrict the helicopter and VTOL movement from going higher than a set altitude. If set to zero, the altitude limit is disabled.");
	pAltitudeLimitLowerOffsetCVar = pConsole->Register("v_altitudeLimitLowerOffset", &v_altitudeLimitLowerOffset, 0.1f, VF_CHEAT, "Used in conjunction with v_altitudeLimit to set the zone when gaining altitude start to be more difficult.");
	
	// weapon system
	i_debuggun_1 = pConsole->RegisterString("i_debuggun_1", "ai_statsTarget", VF_DUMPTODISK, "Command to execute on primary DebugGun fire");
	i_debuggun_2 = pConsole->RegisterString("i_debuggun_2", "ag_debug", VF_DUMPTODISK, "Command to execute on secondary DebugGun fire");

	// quick game

	g_quickGame_map = pConsole->RegisterString("g_quickGame_map", "", VF_DUMPTODISK, "QuickGame option");
	g_quickGame_mode = pConsole->RegisterString("g_quickGame_mode", "PowerStruggle", VF_DUMPTODISK, "QuickGame option");
	
	// battledust
	g_battleDust_effect = pConsole->RegisterString("g_battleDust_effect", "misc.battledust.light", 0, "Sets the effect to use for battledust");

	pVehicleQuality = pConsole->GetCVar("v_vehicle_quality");		assert(pVehicleQuality);

	NetInputChainInitCVars();
}

//------------------------------------------------------------------------
void SCVars::ReleaseCVars()
{
	IConsole* pConsole = gEnv->pConsole;

	pConsole->UnregisterVariable("pl_debug_filter", true);
	pConsole->UnregisterVariable("v_altitudeLimit", true);
	pConsole->UnregisterVariable("v_altitudeLimitLowerOffset", true);

	// weapon system
	pConsole->UnregisterVariable("i_debuggun_1", true);
	pConsole->UnregisterVariable("i_debuggun_2", true);

	pConsole->UnregisterVariable("g_quickGame_map", true);
	pConsole->UnregisterVariable("g_quickGame_mode", true);

	pConsole->UnregisterVariable("g_battleDust_effect", true);

	// alien debugging
	pConsole->UnregisterVariable("aln_debug_movement", true);
	pConsole->UnregisterVariable("aln_debug_filter", true);

	#define UNREGISTER_GAMECVAR
		#include "GameCvarsLib.h"
	#undef UNREGISTER_GAMECVAR
}

//------------------------------------------------------------------------
void CGame::CmdDumpSS(IConsoleCmdArgs* pArgs)
{
	if (g_pGame->GetSynchedStorage())
		g_pGame->GetSynchedStorage()->Dump();
}

//------------------------------------------------------------------------
void CGame::RegisterConsoleVars()
{
	assert(m_pConsole);

	if (m_pCVars)
	{
		m_pCVars->InitCVars(m_pConsole);
	}
}

//------------------------------------------------------------------------
void CmdDumpItemNameTable(IConsoleCmdArgs* pArgs)
{
	ItemString::DumpNameTableToLog();
}

//------------------------------------------------------------------------
void CGame::RegisterConsoleCommands()
{
	assert(m_pConsole);

	#define COMMAND_REGISTERLIST
		#include "GameCvarsLib.h"
	#undef COMMAND_REGISTERLIST
}

//------------------------------------------------------------------------
void CGame::CmdLastInv(IConsoleCmdArgs* pArgs)
{
	if (!gEnv->bClient)
		return;

	if (CActor* pClientActor = static_cast<CActor*>(g_pGame->GetIGameFramework()->GetClientActor()))
		pClientActor->SelectLastItem(true);
}

//------------------------------------------------------------------------
void CGame::CmdName(IConsoleCmdArgs* pArgs)
{
	if (!gEnv->bClient)
		return;

	IActor* pClientActor = g_pGame->GetIGameFramework()->GetClientActor();
	if (!pClientActor)
		return;

	CGameRules* pGameRules = g_pGame->GetGameRules();
	if (pGameRules)
		pGameRules->RenamePlayer(pGameRules->GetActorByEntityId(pClientActor->GetEntityId()), pArgs->GetArg(1));
}

//------------------------------------------------------------------------
void CGame::CmdTeam(IConsoleCmdArgs* pArgs)
{
	if (!gEnv->bClient)
		return;

	IActor* pClientActor = g_pGame->GetIGameFramework()->GetClientActor();
	if (!pClientActor)
		return;

	CGameRules* pGameRules = g_pGame->GetGameRules();
	if (pGameRules)
		pGameRules->ChangeTeam(pGameRules->GetActorByEntityId(pClientActor->GetEntityId()), pArgs->GetArg(1));
}

//------------------------------------------------------------------------
void CGame::CmdLoadLastSave(IConsoleCmdArgs* pArgs)
{
	if (!gEnv->bClient || gEnv->bMultiplayer)
		return;

	if (g_pGame->GetMenu() && g_pGame->GetMenu()->IsActive())
		return;

	string* lastSave = NULL;
	if (g_pGame->GetMenu())
		lastSave = g_pGame->GetMenu()->GetLastInGameSave();
	if (lastSave && lastSave->size())
	{
		if (!g_pGame->GetIGameFramework()->LoadGame(lastSave->c_str(), true))
			g_pGame->GetIGameFramework()->LoadGame(g_pGame->GetLastSaveGame().c_str(), false);
	}
	else
	{
		const string& file = g_pGame->GetLastSaveGame().c_str();
		if (!g_pGame->GetIGameFramework()->LoadGame(file.c_str(), true))
			g_pGame->GetIGameFramework()->LoadGame(file.c_str(), false);
	}
}

//------------------------------------------------------------------------
void CGame::CmdSpectator(IConsoleCmdArgs* pArgs)
{
	if (!gEnv->bClient)
		return;

	IActor* pClientActor = g_pGame->GetIGameFramework()->GetClientActor();
	if (!pClientActor)
		return;

	CGameRules* pGameRules = g_pGame->GetGameRules();
	if (pGameRules)
	{
		int mode = 2;
		if (pArgs->GetArgCount() == 2)
			mode = atoi(pArgs->GetArg(1));
		pGameRules->ChangeSpectatorMode(pGameRules->GetActorByEntityId(pClientActor->GetEntityId()), mode, 0, true);
	}
}

//------------------------------------------------------------------------
void CGame::CmdJoinGame(IConsoleCmdArgs* pArgs)
{
	if (!gEnv->bClient)
		return;

	IActor* pClientActor = g_pGame->GetIGameFramework()->GetClientActor();
	if (!pClientActor)
		return;

	if (g_pGame->GetGameRules()->GetTeamCount() > 0)
		return;

	CGameRules* pGameRules = g_pGame->GetGameRules();
	if (pGameRules)
		pGameRules->ChangeSpectatorMode(pGameRules->GetActorByEntityId(pClientActor->GetEntityId()), 0, 0, true);
}

//------------------------------------------------------------------------
void CGame::CmdKill(IConsoleCmdArgs* pArgs)
{
	if (!gEnv->bClient)
		return;

	IActor* pClientActor = g_pGame->GetIGameFramework()->GetClientActor();
	if (!pClientActor)
		return;

	CGameRules* pGameRules = g_pGame->GetGameRules();
	if (pGameRules)
	{
		HitInfo suicideInfo(pClientActor->GetEntityId(), pClientActor->GetEntityId(), pClientActor->GetEntityId(),
			-1, 0, 0, -1, 0, ZERO, ZERO, ZERO);
		suicideInfo.SetDamage(10000);

		pGameRules->ClientHit(suicideInfo);
	}
}

//------------------------------------------------------------------------
void CGame::CmdVehicleKill(IConsoleCmdArgs* pArgs)
{
	if (!gEnv->bClient)
		return;

	IActor* pClientActor = g_pGame->GetIGameFramework()->GetClientActor();
	if (!pClientActor)
		return;

	IVehicle* pVehicle = pClientActor->GetLinkedVehicle();
	if (!pVehicle)
		return;

	CGameRules* pGameRules = g_pGame->GetGameRules();
	if (pGameRules)
	{
		HitInfo suicideInfo(pVehicle->GetEntityId(), pVehicle->GetEntityId(), pVehicle->GetEntityId(),
			-1, 0, 0, -1, 0, pVehicle->GetEntity()->GetWorldPos(), ZERO, ZERO);
		suicideInfo.SetDamage(10000);

		pGameRules->ClientHit(suicideInfo);
	}
}

//------------------------------------------------------------------------
void CGame::CmdRestart(IConsoleCmdArgs* pArgs)
{
	if (g_pGame && g_pGame->GetGameRules())
		g_pGame->GetGameRules()->Restart();
}

//------------------------------------------------------------------------
void CGame::CmdSay(IConsoleCmdArgs* pArgs)
{
	if (pArgs->GetArgCount() > 1 && gEnv->bServer)
	{
		const char* msg = pArgs->GetCommandLine() + strlen(pArgs->GetArg(0)) + 1;
		g_pGame->GetGameRules()->SendTextMessage(eTextMessageServer, msg, eRMI_ToAllClients);

		if (!gEnv->bClient)
			CryLogAlways("** Server: %s **", msg);
	}
}

//------------------------------------------------------------------------
void CGame::CmdLoadActionmap(IConsoleCmdArgs* pArgs)
{
	if (pArgs->GetArg(1))
		g_pGame->LoadActionMaps(pArgs->GetArg(1));
}

//------------------------------------------------------------------------
void CGame::CmdRestartGame(IConsoleCmdArgs* pArgs)
{
	GetISystem()->Relaunch(true);
	GetISystem()->Quit();
}

//------------------------------------------------------------------------
void CGame::CmdReloadItems(IConsoleCmdArgs* pArgs)
{
	g_pGame->GetItemSharedParamsList()->Reset();
	g_pGame->GetIGameFramework()->GetIItemSystem()->Reload();
	g_pGame->GetWeaponSystem()->Reload();
}

//------------------------------------------------------------------------
void CGame::CmdReloadGameRules(IConsoleCmdArgs* pArgs)
{
	if (gEnv->bMultiplayer)
		return;

	IGameRulesSystem* pGameRulesSystem = g_pGame->GetIGameFramework()->GetIGameRulesSystem();
	IGameRules* pGameRules = pGameRulesSystem->GetCurrentGameRules();

	const char* name = "SinglePlayer";
	IEntityClass* pEntityClass = 0;

	if (pGameRules)
	{
		pEntityClass = pGameRules->GetEntity()->GetClass();
		name = pEntityClass->GetName();
	}
	else
		pEntityClass = gEnv->pEntitySystem->GetClassRegistry()->FindClass(name);

	if (pEntityClass)
	{
		pEntityClass->LoadScript(true);

		if (pGameRulesSystem->CreateGameRules(name))
			CryLog("reloaded GameRules <%s>", name);
		else
			CryLogWarningAlways("reloading GameRules <%s> failed!", name);
	}
}

void CGame::CmdNextLevel(IConsoleCmdArgs* pArgs)
{
	ILevelRotation* pLevelRotation = g_pGame->GetIGameFramework()->GetILevelSystem()->GetLevelRotation();
	if (pLevelRotation->GetLength())
		pLevelRotation->ChangeLevel(pArgs);
}

void CGame::CmdStartKickVoting(IConsoleCmdArgs* pArgs)
{	
	if (!gEnv->bClient)
		return;

	if (pArgs->GetArgCount() < 2)
	{
		CryLogAlways("Usage: startKickVoting player_id");
		CryLogAlways("Use listplayers to get a list of player ids.");

		return;
	}

	IActor* pClientActor = g_pGame->GetIGameFramework()->GetClientActor();
	if (!pClientActor)
		return;

	if (CGameRules* pGameRules = g_pGame->GetGameRules())
	{
		if (CActor* pActor = pGameRules->GetActorByChannelId(atoi(pArgs->GetArg(1))))
			pGameRules->StartVoting(static_cast<CActor*>(pClientActor), eVS_kick, pActor->GetEntityId(), "");
	}
}

void CGame::CmdStartNextMapVoting(IConsoleCmdArgs* pArgs)
{
	if (!gEnv->bClient)
		return;

	IActor* pClientActor = g_pGame->GetIGameFramework()->GetClientActor();
	if (!pClientActor)
		return;

	if (CGameRules* pGameRules = g_pGame->GetGameRules())
		pGameRules->StartVoting(static_cast<CActor*>(pClientActor), eVS_nextMap, 0, "");
}


void CGame::CmdVote(IConsoleCmdArgs* pArgs)
{	
	if (!gEnv->bClient)
		return;

	IActor* pClientActor = g_pGame->GetIGameFramework()->GetClientActor();
	if (!pClientActor)
		return;

	if (CGameRules* pGameRules = g_pGame->GetGameRules())
		pGameRules->Vote(pGameRules->GetActorByEntityId(pClientActor->GetEntityId()), true);
}

void CGame::CmdListPlayers(IConsoleCmdArgs* pArgs)
{
	IActor* pClientActor = g_pGame->GetIGameFramework()->GetClientActor();
	if (!pClientActor)
		return;

	if (CGameRules* pGameRules = g_pGame->GetGameRules())
	{
		CGameRules::TPlayers players;
		pGameRules->GetPlayers(players);

		if (!players.empty())
		{
			const Vec3 clientPos = pClientActor->GetEntity()->GetWorldPos();
			CryLogAlways("  [chan] [name]               [dist]  [physDist]  [profile] [viewDist]   [team]  [lastSeen]  [visible]");

			for (CGameRules::TPlayers::iterator it = players.begin(); it != players.end(); ++it)
			{
				if (CActor* pActor = pGameRules->GetActorByEntityId(*it))
				{
					float PhysDistance = -1.0f;
					uint8 profile = pActor->GetGameObject()->GetAspectProfile(eEA_Physics);
					const char* profileName;
					if (profile == eAP_Alive)
						profileName = "alive";
					else if (profile == eAP_NotPhysicalized)
						profileName = "not physicalized";
					else if (profile == eAP_Ragdoll)
						profileName = "ragdoll";
					else if (profile == eAP_Sleep)
						profileName = "sleep";
					else if (profile == eAP_Spectator)
						profileName = "spectator";
					else if (profile == eAP_Linked)
						profileName = "vehicle";
					else if (profile == eAP_Frozen)
						profileName = "frozen";
					else
						profileName = "none";

					const char* visible = pActor->GetGameObject()->IsProbablyVisible() ? "$5yes" : "$9no";

					IEntityRenderProxy* pProxy = static_cast<IEntityRenderProxy*>(pActor->GetEntity()->GetProxy(ENTITY_PROXY_RENDER));
					IRenderNode *pRenderNode = pProxy ? pProxy->GetRenderNode() : nullptr;

					auto ViewDist = pRenderNode ? pRenderNode->GetViewDistRatio() : -1;

					auto lastSeen = pProxy ? pProxy->GetLastSeenTime() : -1.0f;
					auto timeSinceLastSeen = gEnv->pTimer->GetCurrTime() - lastSeen;

					if (pRenderNode)
					{
						ViewDist = pRenderNode->GetViewDistRatio();
					}

					if (IPhysicalEntity* pPhysics = pActor->GetEntity()->GetPhysics())
					{
						pe_status_pos pos;
						if (pPhysics->GetStatus(&pos))
						{
							PhysDistance = (clientPos - pos.pos).len();
						}
					}

					const float WorldDistance = (clientPos - pActor->GetEntity()->GetWorldPos()).len();
					CryLogAlways("  %5d  %-19s $6%6i$9m     $8%6i$9m  $3%-10s    $7%4i       $9%i    $8%7.fs        %s", 
						pActor->GetChannelId(), 
						pActor->GetCleanNick().c_str(), 
						(int)WorldDistance, 
						(int)PhysDistance, 
						profileName, 
						ViewDist, 
						pGameRules->GetTeam(pActor->GetEntityId()), 
						timeSinceLastSeen,
						visible);
				}
			}
		}
	}
}

void CGame::CmdQuickGame(IConsoleCmdArgs* pArgs)
{
	g_pGame->GetMenu()->GetMPHub()->OnQuickGame();
}

void CGame::CmdQuickGameStop(IConsoleCmdArgs* pArgs)
{

}

void CGame::CmdBattleDustReload(IConsoleCmdArgs* pArgs)
{
	if (CBattleDust* pBD = g_pGame->GetGameRules()->GetBattleDust())
	{
		pBD->ReloadXml();
	}
}
