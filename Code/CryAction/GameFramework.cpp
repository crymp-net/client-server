#include <algorithm>
#include <cstdint>

#include "CryCommon/CryScriptSystem/IScriptSystem.h"
#include "CryCommon/CrySoundSystem/IMusicSystem.h"
#include "CryCommon/CrySystem/gEnv.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/ITextModeConsole.h"
#include "CryCommon/CrySystem/ITimer.h"
#include "CrySystem/CryLog.h"

#include "ActionMapManager.h"
#include "ActorSystem.h"
#include "AnimationGraphSystem.h"
#include "CallbackTimer.h"
#include "DebrisMgr.h"
#include "DevMode.h"
#include "DialogSystem.h"
#include "EffectSystem.h"
#include "FlowSystem.h"
#include "GameFramework.h"
#include "GameFrameworkCVars.h"
#include "GameObject.h"
#include "GameObjectSystem.h"
#include "GameplayAnalyst.h"
#include "GameplayRecorder.h"
#include "GameRulesSystem.h"
#include "GameSerialize.h"
#include "GameStatsConfig.h"
#include "GameTokenSystem.h"
#include "ItemSystem.h"
#include "LevelSystem.h"
#include "MaterialEffects.h"
#include "MusicLogic.h"
#include "NetworkCVars.h"
#include "PersistantDebug.h"
#include "PlayerProfileManager.h"
#include "ScriptBind_ActionMapManager.h"
#include "ScriptBind_Actor.h"
#include "ScriptBind_AI.h"
#include "ScriptBind_CryAction.h"
#include "ScriptBind_DialogSystem.h"
#include "ScriptBind_Inventory.h"
#include "ScriptBind_ItemSystem.h"
#include "ScriptBind_MaterialEffects.h"
#include "ScriptBind_Network.h"
#include "ScriptBind_Vehicle.h"
#include "ScriptBind_VehicleSeat.h"
#include "ScriptBind_VehicleSystem.h"
#include "ScriptRMI.h"
#include "SubtitleManager.h"
#include "TimeDemoRecorder.h"
#include "TimeOfDayScheduler.h"
#include "UIDraw.h"
#include "Vehicles/VehicleSystem.h"
#include "ViewSystem.h"

extern std::uintptr_t CRYACTION_BASE;

static void RegisterInventoryFactory(IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	auto* pInventoryCreator = reinterpret_cast<IGameFramework::IGameObjectExtensionCreator*>(CRYACTION_BASE + 0x4651a8);
	auto* vtable = reinterpret_cast<void*>(CRYACTION_BASE + 0x38c968);
#else
	auto* pInventoryCreator = reinterpret_cast<IGameFramework::IGameObjectExtensionCreator*>(CRYACTION_BASE + 0x2db8dc);
	auto* vtable = reinterpret_cast<void*>(CRYACTION_BASE + 0x298bd4);
#endif

	// construct
	*reinterpret_cast<void**>(pInventoryCreator) = vtable;

	pGameFramework->RegisterFactory("Inventory", pInventoryCreator, false);
}

static void RegisterSomeExtensions(IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x2bb6d0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x1dbcb0;
#endif

	reinterpret_cast<void(*)(IGameFramework*)>(func)(pGameFramework);
}

GameFramework::GameFramework()
{
	static std::uintptr_t vtable[97] = {
#ifdef BUILD_64BIT
		CRYACTION_BASE + 0x319390, CRYACTION_BASE + 0x319380, CRYACTION_BASE + 0x319370, CRYACTION_BASE + 0x319360, CRYACTION_BASE + 0x3191f0, CRYACTION_BASE + 0x3191e0, CRYACTION_BASE + 0x31d250, CRYACTION_BASE + 0x3286d0,
		CRYACTION_BASE + 0x31cbb0, CRYACTION_BASE + 0x3252a0, CRYACTION_BASE + 0x324700, CRYACTION_BASE + 0x318bb0, CRYACTION_BASE + 0x31b040, CRYACTION_BASE + 0x318bc0, CRYACTION_BASE + 0x318d60, CRYACTION_BASE + 0x31a260,
		CRYACTION_BASE + 0x285590, CRYACTION_BASE + 0x31afa0, CRYACTION_BASE + 0x3193c0, CRYACTION_BASE + 0x319450, CRYACTION_BASE + 0x3193d0, CRYACTION_BASE + 0x3193e0, CRYACTION_BASE + 0x3193f0, CRYACTION_BASE + 0x3193b0,
		CRYACTION_BASE + 0x319420, CRYACTION_BASE + 0x319430, CRYACTION_BASE + 0x319410, CRYACTION_BASE + 0x319440, CRYACTION_BASE + 0x319090, CRYACTION_BASE + 0x319460, CRYACTION_BASE + 0x319470, CRYACTION_BASE + 0x319480,
		CRYACTION_BASE + 0x319490, CRYACTION_BASE + 0x3194a0, CRYACTION_BASE + 0x31b020, CRYACTION_BASE + 0x319900, CRYACTION_BASE + 0x31b110, CRYACTION_BASE + 0x31a2c0, CRYACTION_BASE + 0x31b230, CRYACTION_BASE + 0x31b030,
		CRYACTION_BASE + 0x31a280, CRYACTION_BASE + 0x31a2a0, CRYACTION_BASE + 0x318d90, CRYACTION_BASE + 0x318de0, CRYACTION_BASE + 0x3190a0, CRYACTION_BASE + 0x319150, CRYACTION_BASE + 0x31a3b0, CRYACTION_BASE + 0x31a3d0,
		CRYACTION_BASE + 0x31a400, CRYACTION_BASE + 0x31a430, CRYACTION_BASE + 0x319290, CRYACTION_BASE + 0x319320, CRYACTION_BASE + 0x3192d0, CRYACTION_BASE + 0x319200, CRYACTION_BASE + 0x31d000, CRYACTION_BASE + 0x31d150,
		CRYACTION_BASE + 0x319250, CRYACTION_BASE + 0x325600, CRYACTION_BASE + 0x318e00, CRYACTION_BASE + 0x31a2e0, CRYACTION_BASE + 0x31afb0, CRYACTION_BASE + 0x31a630, CRYACTION_BASE + 0x31a650, CRYACTION_BASE + 0x3199e0,
		CRYACTION_BASE + 0x31afc0, CRYACTION_BASE + 0x31afd0, CRYACTION_BASE + 0x319a00, CRYACTION_BASE + 0x319a60, CRYACTION_BASE + 0x3195f0, CRYACTION_BASE + 0x3194f0, CRYACTION_BASE + 0x31a4f0, CRYACTION_BASE + 0x319620,
		CRYACTION_BASE + 0x319630, CRYACTION_BASE + 0x31afe0, CRYACTION_BASE + 0x31aff0, CRYACTION_BASE + 0x327e60, CRYACTION_BASE + 0x3249a0, CRYACTION_BASE + 0x31a670, CRYACTION_BASE + 0x3198c0, CRYACTION_BASE + 0x31b000,
		CRYACTION_BASE + 0x31a690, CRYACTION_BASE + 0x321a50, CRYACTION_BASE + 0x3198f0, CRYACTION_BASE + 0x321fc0, CRYACTION_BASE + 0x3285e0, CRYACTION_BASE + 0x323e60, CRYACTION_BASE + 0x31b010, CRYACTION_BASE + 0x31a6b0,
		CRYACTION_BASE + 0x31d860, CRYACTION_BASE + 0x319810, CRYACTION_BASE + 0x319840, CRYACTION_BASE + 0x31b580, CRYACTION_BASE + 0x3197f0, CRYACTION_BASE + 0x319800, CRYACTION_BASE + 0x32a300, CRYACTION_BASE + 0x31d800,
		CRYACTION_BASE + 0x31a370,
#else
		CRYACTION_BASE + 0x21cd60, CRYACTION_BASE + 0x21cd50, CRYACTION_BASE + 0x21cd30, CRYACTION_BASE + 0x21cd20, CRYACTION_BASE + 0x21cbf0, CRYACTION_BASE + 0x21cbd0, CRYACTION_BASE + 0x21f760, CRYACTION_BASE + 0x243ab0,
		CRYACTION_BASE + 0x21eff0, CRYACTION_BASE + 0x242570, CRYACTION_BASE + 0x2415f0, CRYACTION_BASE + 0x21c600, CRYACTION_BASE + 0x21f400, CRYACTION_BASE + 0x21c610, CRYACTION_BASE + 0x21c780, CRYACTION_BASE + 0x21e030,
		CRYACTION_BASE + 0x1b2d50, CRYACTION_BASE + 0x2450c0, CRYACTION_BASE + 0x21cd90, CRYACTION_BASE + 0x21ce20, CRYACTION_BASE + 0x21cda0, CRYACTION_BASE + 0x21cdb0, CRYACTION_BASE + 0x21cdc0, CRYACTION_BASE + 0x21cd80,
		CRYACTION_BASE + 0x21cdf0, CRYACTION_BASE + 0x21ce00, CRYACTION_BASE + 0x21cde0, CRYACTION_BASE + 0x21ce10, CRYACTION_BASE + 0x21ca30, CRYACTION_BASE + 0x21ce30, CRYACTION_BASE + 0x21ce40, CRYACTION_BASE + 0x21ce50,
		CRYACTION_BASE + 0x21ce60, CRYACTION_BASE + 0x21ce70, CRYACTION_BASE + 0x245140, CRYACTION_BASE + 0x21d250, CRYACTION_BASE + 0x21f4c0, CRYACTION_BASE + 0x21e070, CRYACTION_BASE + 0x21f580, CRYACTION_BASE + 0x245150,
		CRYACTION_BASE + 0x21e050, CRYACTION_BASE + 0x21e060, CRYACTION_BASE + 0x21c7b0, CRYACTION_BASE + 0x21c7f0, CRYACTION_BASE + 0x21ca80, CRYACTION_BASE + 0x21cb20, CRYACTION_BASE + 0x21e1b0, CRYACTION_BASE + 0x21e1c0,
		CRYACTION_BASE + 0x21e1e0, CRYACTION_BASE + 0x21e220, CRYACTION_BASE + 0x21cc80, CRYACTION_BASE + 0x21ccf0, CRYACTION_BASE + 0x21ccb0, CRYACTION_BASE + 0x21cc10, CRYACTION_BASE + 0x21f660, CRYACTION_BASE + 0x21f6b0,
		CRYACTION_BASE + 0x21cc50, CRYACTION_BASE + 0x242850, CRYACTION_BASE + 0x21c810, CRYACTION_BASE + 0x21e090, CRYACTION_BASE + 0x2450d0, CRYACTION_BASE + 0x21e3c0, CRYACTION_BASE + 0x21e3e0, CRYACTION_BASE + 0x21d300,
		CRYACTION_BASE + 0x2450e0, CRYACTION_BASE + 0x2450f0, CRYACTION_BASE + 0x21d320, CRYACTION_BASE + 0x21d370, CRYACTION_BASE + 0x21cf90, CRYACTION_BASE + 0x21cec0, CRYACTION_BASE + 0x21e2c0, CRYACTION_BASE + 0x21cfc0,
		CRYACTION_BASE + 0x21cfd0, CRYACTION_BASE + 0x245100, CRYACTION_BASE + 0x245110, CRYACTION_BASE + 0x242c70, CRYACTION_BASE + 0x241840, CRYACTION_BASE + 0x21e430, CRYACTION_BASE + 0x21d200, CRYACTION_BASE + 0x245120,
		CRYACTION_BASE + 0x21e440, CRYACTION_BASE + 0x22f840, CRYACTION_BASE + 0x21d230, CRYACTION_BASE + 0x22fc30, CRYACTION_BASE + 0x243a50, CRYACTION_BASE + 0x23f150, CRYACTION_BASE + 0x245130, CRYACTION_BASE + 0x21e460,
		CRYACTION_BASE + 0x21fa50, CRYACTION_BASE + 0x21d160, CRYACTION_BASE + 0x21d180, CRYACTION_BASE + 0x21fa90, CRYACTION_BASE + 0x21d130, CRYACTION_BASE + 0x21d140, CRYACTION_BASE + 0x249310, CRYACTION_BASE + 0x21fa00,
		CRYACTION_BASE + 0x21e190,
#endif
	};

	const std::uintptr_t* current_vtable = *reinterpret_cast<std::uintptr_t**>(this);

	// use original functions from CryAction DLL except the following
	vtable[1] = current_vtable[1];    // GameFramework::RegisterFactory(IVehicleCreator)
	vtable[2] = current_vtable[2];    // GameFramework::RegisterFactory(IItemCreator)
	vtable[7] = current_vtable[7];    // GameFramework::Init
	vtable[8] = current_vtable[8];    // GameFramework::CompleteInit
	vtable[9] = current_vtable[9];    // GameFramework::PreUpdate
	//vtable[10] = current_vtable[10];  // GameFramework::PostUpdate
	vtable[11] = current_vtable[11];  // GameFramework::Shutdown
	vtable[22] = current_vtable[22];  // GameFramework::GetIItemSystem
	vtable[26] = current_vtable[26];  // GameFramework::GetIVehicleSystem
	vtable[81] = current_vtable[81];  // GameFramework::GetMemoryStatistics
	vtable[92] = current_vtable[92];  // GameFramework::PrefetchLevelAssets
	vtable[94] = current_vtable[94];  // GameFramework::~GameFramework

	// replace our vtable
	*reinterpret_cast<std::uintptr_t**>(this) = vtable;

	// set global instance pointer inside CryAction DLL
#ifdef BUILD_64BIT
	*reinterpret_cast<GameFramework**>(CRYACTION_BASE + 0x464b40) = this;
#else
	*reinterpret_cast<GameFramework**>(CRYACTION_BASE + 0x2db0ac) = this;
#endif
}

GameFramework::~GameFramework()
{
}

GameFramework* GameFramework::GetInstance()
{
	static GameFramework* pInstance = new GameFramework();
	return pInstance;
}

////////////////////////////////////////////////////////////////////////////////
// IGameFramework
////////////////////////////////////////////////////////////////////////////////

void GameFramework::RegisterFactory(const char* name, IAnimationStateNodeFactory* (*)(), bool isAI)
{
}

void GameFramework::RegisterFactory(const char* name, ISaveGame* (*)(), bool isAI)
{
}

void GameFramework::RegisterFactory(const char* name, ILoadGame* (*)(), bool isAI)
{
}

void GameFramework::RegisterFactory(const char* name, IActorCreator*, bool isAI)
{
}

void GameFramework::RegisterFactory(const char* name, IItemCreator* pCreator, bool isAI)
{
	m_pItemSystem->RegisterItemFactory(name, pCreator);
}

void GameFramework::RegisterFactory(const char* name, IVehicleCreator* pCreator, bool isAI)
{
	m_pVehicleSystem->RegisterVehicleFactory(name, pCreator);
}

void GameFramework::RegisterFactory(const char* name, IGameObjectExtensionCreator*, bool isAI)
{
}

bool GameFramework::Init(SSystemInitParams& startupParams)
{
	CryLogAlways("$3[CryMP] Initializing Game Framework");

	m_pSystem = gEnv->pSystem;

	// set gEnv inside CryAction DLL
#ifdef BUILD_64BIT
	*reinterpret_cast<SSystemGlobalEnvironment**>(CRYACTION_BASE + 0x464ae0) = m_pSystem->GetGlobalEnvironment();
#else
	*reinterpret_cast<SSystemGlobalEnvironment**>(CRYACTION_BASE + 0x2db06c) = m_pSystem->GetGlobalEnvironment();
#endif

	// register CryAction's global system event listener
#ifdef BUILD_64BIT
	m_pSystem->GetISystemEventDispatcher()->RegisterListener(reinterpret_cast<ISystemEventListener*>(CRYACTION_BASE + 0x420bb0));
#else
	m_pSystem->GetISystemEventDispatcher()->RegisterListener(reinterpret_cast<ISystemEventListener*>(CRYACTION_BASE + 0x2a15e4));
#endif

	m_pNetwork = m_pSystem->GetINetwork();
	m_p3DEngine = m_pSystem->GetI3DEngine();
	m_pScriptSystem = m_pSystem->GetIScriptSystem();
	m_pEntitySystem = m_pSystem->GetIEntitySystem();
	m_pTimer = m_pSystem->GetITimer();
	m_pLog = m_pSystem->GetILog();

	// original CryAction obtains Crysis CD key from Windows registry at this point
	// CryMP client sets a random CD key during its initialization
	// no need to obtain CD key here

	this->RegisterConsoleVariables();
	this->RegisterConsoleCommands();

	if (m_pSystem->IsDevMode())
	{
		m_pDevMode = new DevMode();
	}

	m_pTimeDemoRecorder = new TimeDemoRecorder(m_pSystem);

	ScriptRMI::RegisterCVars();
	GameObject::RegisterCVars();

	m_pScriptRMI = new ScriptRMI();
	m_pGameTokenSystem = new GameTokenSystem();
	m_pEffectSystem = new EffectSystem();
	m_pEffectSystem->Init();
	m_pUIDraw = new UIDraw();
	m_pLevelSystem = new LevelSystem(m_pSystem, "levels");
	m_pActorSystem = new ActorSystem(m_pSystem, m_pEntitySystem);
	m_pItemSystem = new ItemSystem(this);
	m_pActionMapManager = new ActionMapManager(m_pSystem->GetIInput());
	m_pViewSystem = new ViewSystem(m_pSystem);
	m_pGameplayRecorder = new GameplayRecorder(this);
	m_pGameplayAnalyst = new GameplayAnalyst();
	m_pGameRulesSystem = new GameRulesSystem(m_pSystem, this);
	m_pVehicleSystem = new VehicleSystem(this);
	m_pNetworkCVars = new NetworkCVars();
	m_pGameFrameworkCVars = new GameFrameworkCVars();

	m_pGameObjectSystem = new GameObjectSystem();
	m_pGameObjectSystem->Init();
	m_pGameObjectSystem->RegisterEvent(eGFE_PauseGame, "PauseGame");
	m_pGameObjectSystem->RegisterEvent(eGFE_ResumeGame, "ResumeGame");
	m_pGameObjectSystem->RegisterEvent(eGFE_OnCollision, "OnCollision");
	m_pGameObjectSystem->RegisterEvent(eGFE_OnPostStep, "OnPostStep");
	m_pGameObjectSystem->RegisterEvent(eGFE_OnStateChange, "OnStateChange");
	m_pGameObjectSystem->RegisterEvent(eGFE_ResetAnimationGraphs, "ResetAnimationGraphs");
	m_pGameObjectSystem->RegisterEvent(eGFE_OnBreakable2d, "OnBreakable2d");
	m_pGameObjectSystem->RegisterEvent(eGFE_OnBecomeVisible, "OnBecomeVisible");
	m_pGameObjectSystem->RegisterEvent(eGFE_PreFreeze, "PreFreeze");
	m_pGameObjectSystem->RegisterEvent(eGFE_PreShatter, "PreShatter");
	m_pGameObjectSystem->RegisterEvent(eGFE_BecomeLocalPlayer, "BecomeLocalPlayer");
	m_pGameObjectSystem->RegisterEvent(eGFE_DisablePhysics, "DisablePhysics");
	m_pGameObjectSystem->RegisterEvent(eGFE_EnablePhysics, "EnablePhysics");

	m_pAnimationGraphSystem = new AnimationGraphSystem();
	m_pGameSerialize = new GameSerialize();
	m_pCallbackTimer = new CallbackTimer();
	m_pPersistantDebug = new PersistantDebug();
	m_pPlayerProfileManager = new PlayerProfileManager();
	m_pDialogSystem = new DialogSystem();
	m_pDialogSystem->Init();
	m_pDebrisMgr = new DebrisMgr();
	m_pTimeOfDayScheduler = new TimeOfDayScheduler();
	m_pSubtitleManager = new SubtitleManager();

	IMovieSystem* pMovieSystem = m_pSystem->GetIMovieSystem();
	if (pMovieSystem)
	{
		pMovieSystem->SetUser(m_pViewSystem);
	}

	m_pVehicleSystem->Init();

	RegisterInventoryFactory(this);

	m_pLevelSystem->AddListener(m_pItemSystem);

	this->RegisterScriptBindings();

	m_pSomeStrings = new SomeStrings();

	m_pVehicleSystem->RegisterVehicles(this);
	m_pGameObjectSystem->RegisterFactories(this);
	m_pGameSerialize->RegisterFactories(this);

	RegisterSomeExtensions(this);

	m_pPlayerProfileManager->Initialize();

	// skip instantiating CDownloadTask for downloading maps and stuff (m_reserved_0x51c_0x5b8)
	// it is more harmful than useful and all accesses seem to be guarded by null checks making it optional

	m_pListenersA = new Listeners();
	m_pListenersB = new Listeners();

	m_pNextFrameCommand = new CryStringT<char>();

	m_pGameStatsConfig = new GameStatsConfig();
	m_pGameStatsConfig->Init();

	return true;
}

bool GameFramework::CompleteInit()
{
	m_pAnimationGraphSystem->RegisterFactories(this);

	if (IAnimationGraphPtr pMusicGraph = m_pAnimationGraphSystem->LoadGraph("MusicLogic.xml", false, true))
	{
		m_pMusicGraphState = pMusicGraph->CreateState();
	}

	if (!m_pMusicGraphState)
	{
		CryLogWarningAlways("[GameFramework::CompleteInit] Unable to load music logic graph");
	}

	m_pMusicLogic = new MusicLogic(m_pMusicGraphState);
	m_pMusicLogic->Init();

	this->EndGameContext();

	m_pFlowSystem = new FlowSystem();

	m_pSystem->SetIFlowSystem(m_pFlowSystem);
	m_pSystem->SetIAnimationGraphSystem(m_pAnimationGraphSystem);
	m_pSystem->SetIDialogSystem(m_pDialogSystem);

	m_pGameplayRecorder->RegisterListener(m_pGameplayAnalyst);

	m_pMaterialEffects = new MaterialEffects();
	m_pScriptBind_MaterialEffects = new ScriptBind_MaterialEffects(m_pSystem, m_pMaterialEffects);
	m_pMaterialEffects->Load("MaterialEffects.xml");

	m_pSystem->SetIMaterialEffects(m_pMaterialEffects);

	if (!m_pSystem->IsEditor())
	{
		IEquipmentManager* pEquipmentManager = m_pItemSystem->GetIEquipmentManager();

		pEquipmentManager->DeleteAllEquipmentPacks();
		pEquipmentManager->LoadEquipmentPacksFromPath("Libs/EquipmentPacks");
	}

	IGame* pGame = gEnv->pGame;
	if (pGame)
	{
		pGame->CompleteInit();
	}

	m_pMaterialEffects->LoadFlowGraphLibs();

	m_pScriptSystem->ExecuteFile("scripts/main.lua");
	m_pScriptSystem->BeginCall("OnInit");
	m_pScriptSystem->EndCall();

	return true;
}

bool GameFramework::PreUpdate(bool haveFocus, unsigned int updateFlags)
{
	if (!m_pNextFrameCommand->empty())
	{
		gEnv->pConsole->ExecuteString(m_pNextFrameCommand->c_str());
		m_pNextFrameCommand->clear();
	}

	this->CheckEndLevelSchedule();

	ITextModeConsole* pTextModeConsole = m_pSystem->GetITextModeConsole();
	if (pTextModeConsole)
	{
		pTextModeConsole->BeginDraw();
	}

	bool isGameStarted = this->IsGameStarted();
	bool isGamePaused = !isGameStarted || this->IsGamePaused();

	if (!this->IsGamePaused())
	{
		m_pTimeDemoRecorder->PreUpdate();
	}

	if (!isGamePaused)
	{
		m_pCallbackTimer->UpdateTimer();
	}

	if (!(updateFlags & ESYSUPDATE_EDITOR))
	{
		gEnv->pFrameProfileSystem->StartFrame();
	}

	m_pSystem->RenderBegin();

	const float frameTime = gEnv->pTimer->GetFrameTime();
	bool status = true;

	if (!(updateFlags & ESYSUPDATE_EDITOR))
	{
		int pauseMode = (!isGameStarted || m_isPaused) ? 1 : 0;

		if (gEnv->bMultiplayer && !gEnv->bServer)
		{
			pauseMode = 0;
		}

		status = m_pSystem->Update(0, pauseMode);

		this->DispatchActionEvent(SActionEvent(eAE_earlyPreUpdate));

		isGameStarted = this->IsGameStarted();
		const bool wasGamePaused = isGamePaused;
		isGamePaused = !isGameStarted || this->IsGamePaused();

		if (!isGamePaused)
		{
			if (!wasGamePaused)
			{
				m_pViewSystem->Update(std::min(frameTime, 0.1f));
				m_pGameplayRecorder->Update(frameTime);
			}

			m_pFlowSystem->Update();
		}
	}

	m_pActionMapManager->Update();

	if (!isGamePaused)
	{
		m_pItemSystem->Update(frameTime);
		m_pMaterialEffects->Update(frameTime);
		m_pDialogSystem->Update(frameTime);
		m_pMusicLogic->Update();

		if (m_pMusicGraphState)
		{
			m_pMusicGraphState->Update();
		}

		m_pDebrisMgr->Update();
	}

	// skip updating CDownloadTask
	// skip updating RCon server
	// skip updating HTTP server

	return status;
}

void GameFramework::PostUpdate(bool haveFocus, unsigned int updateFlags)
{
	if (m_pLanBrowserCVar->GetIVal())
	{
		if (!m_isLanBrowserRunning)
		{
			m_isLanBrowserRunning = true;
			this->BeginLanQuery();
		}
	}
	else
	{
		if (m_isLanBrowserRunning)
		{
			m_isLanBrowserRunning = false;
			this->EndCurrentQuery();
		}
	}

	const float frameTime = gEnv->pTimer->GetFrameTime();

	if (!this->IsGamePaused())
	{
		m_pEffectSystem->Update(frameTime);

		if (m_lastSaveLoad > 0)
		{
			m_lastSaveLoad -= frameTime;

			if (m_lastSaveLoad < 0)
			{
				m_lastSaveLoad = 0;
			}
		}
	}

	m_pSystem->Render();

	// TODO: call OnPostUpdate on listeners
	// TODO
}

void GameFramework::Shutdown()
{
	CryLogErrorAlways("[GameFramework::Shutdown] Not implemented!");
}

void GameFramework::Reset(bool clients)
{
}

void GameFramework::PauseGame(bool pause, bool force)
{
}

bool GameFramework::IsGamePaused()
{
	return false;
}

bool GameFramework::IsGameStarted()
{
	return false;
}

ISystem* GameFramework::GetISystem()
{
	return nullptr;
}

ILanQueryListener* GameFramework::GetILanQueryListener()
{
	return nullptr;
}

IUIDraw* GameFramework::GetIUIDraw()
{
	return nullptr;
}

IGameObjectSystem* GameFramework::GetIGameObjectSystem()
{
	return nullptr;
}

ILevelSystem* GameFramework::GetILevelSystem()
{
	return nullptr;
}

IActorSystem* GameFramework::GetIActorSystem()
{
	return nullptr;
}

IItemSystem* GameFramework::GetIItemSystem()
{
	return m_pItemSystem;
}

IActionMapManager* GameFramework::GetIActionMapManager()
{
	return nullptr;
}

IViewSystem* GameFramework::GetIViewSystem()
{
	return nullptr;
}

IGameplayRecorder* GameFramework::GetIGameplayRecorder()
{
	return nullptr;
}

IVehicleSystem* GameFramework::GetIVehicleSystem()
{
	return m_pVehicleSystem;
}

IGameRulesSystem* GameFramework::GetIGameRulesSystem()
{
	return nullptr;
}

IFlowSystem* GameFramework::GetIFlowSystem()
{
	return nullptr;
}

IGameTokenSystem* GameFramework::GetIGameTokenSystem()
{
	return nullptr;
}

IEffectSystem* GameFramework::GetIEffectSystem()
{
	return nullptr;
}

IMaterialEffects* GameFramework::GetIMaterialEffects()
{
	return nullptr;
}

IDialogSystem* GameFramework::GetIDialogSystem()
{
	return nullptr;
}

IPlayerProfileManager* GameFramework::GetIPlayerProfileManager()
{
	return nullptr;
}

IDebrisMgr* GameFramework::GetDebrisMgr()
{
	return nullptr;
}

ISubtitleManager* GameFramework::GetISubtitleManager()
{
	return nullptr;
}

bool GameFramework::StartGameContext(const SGameStartParams* pGameStartParams)
{
	return false;
}

bool GameFramework::ChangeGameContext(const SGameContextParams* pGameContextParams)
{
	return false;
}

void GameFramework::EndGameContext()
{
}

bool GameFramework::StartedGameContext() const
{
	return false;
}

bool GameFramework::BlockingSpawnPlayer()
{
	return false;
}

void GameFramework::ResetBrokenGameObjects()
{
}

void GameFramework::SetEditorLevel(const char* levelName, const char* levelFolder)
{
}

void GameFramework::GetEditorLevel(char** levelName, char** levelFolder)
{
}

void GameFramework::BeginLanQuery()
{
}

void GameFramework::EndCurrentQuery()
{
}

IActor* GameFramework::GetClientActor() const
{
	return nullptr;
}

EntityId GameFramework::GetClientActorId() const
{
	return 0;
}

INetChannel* GameFramework::GetClientChannel() const
{
	return nullptr;
}

CTimeValue GameFramework::GetServerTime()
{
	return {};
}

uint16 GameFramework::GetGameChannelId(INetChannel* pNetChannel)
{
	return 0;
}

bool GameFramework::IsChannelOnHold(uint16 channelId)
{
	return false;
}

INetChannel* GameFramework::GetNetChannel(uint16 channelId)
{
	return nullptr;
}

IGameObject* GameFramework::GetGameObject(EntityId id)
{
	return nullptr;
}

bool GameFramework::GetNetworkSafeClassId(uint16& id, const char* className)
{
	return false;
}

bool GameFramework::GetNetworkSafeClassName(char* className, size_t maxn, uint16 id)
{
	return false;
}

IGameObjectExtension* GameFramework::QueryGameObjectExtension(EntityId id, const char* name)
{
	return nullptr;
}

bool GameFramework::SaveGame(const char* path, bool quick, bool forceImmediate, ESaveGameReason reason, bool ignoreDelay, const char* checkPoint)
{
	return false;
}

bool GameFramework::LoadGame(const char* path, bool quick, bool ignoreDelay)
{
	return false;
}

void GameFramework::OnEditorSetGameMode(int mode)
{
}

bool GameFramework::IsEditing()
{
	return false;
}

bool GameFramework::IsInLevelLoad()
{
	return false;
}

bool GameFramework::IsLoadingSaveGame()
{
	return false;
}

bool GameFramework::IsInTimeDemo()
{
	return false;
}

void GameFramework::AllowSave(bool allow)
{
}

void GameFramework::AllowLoad(bool allow)
{
}

bool GameFramework::CanSave()
{
	return false;
}

bool GameFramework::CanLoad()
{
	return false;
}

bool GameFramework::CanCheat()
{
	return false;
}

const char* GameFramework::GetLevelName()
{
	return nullptr;
}

const char* GameFramework::GetAbsLevelPath()
{
	return nullptr;
}

IPersistantDebug* GameFramework::GetIPersistantDebug()
{
	return nullptr;
}

IGameStatsConfig* GameFramework::GetIGameStatsConfig()
{
	return nullptr;
}

IAnimationGraphState* GameFramework::GetMusicGraphState()
{
	return nullptr;
}

IMusicLogic* GameFramework::GetMusicLogic()
{
	return nullptr;
}

void GameFramework::RegisterListener(IGameFrameworkListener* pGameFrameworkListener, const char* name, EFRAMEWORKLISTENERPRIORITY priority)
{
}

void GameFramework::UnregisterListener(IGameFrameworkListener* pGameFrameworkListener)
{
}

INetNub* GameFramework::GetServerNetNub()
{
	return nullptr;
}

void GameFramework::SetGameGUID(const char* gameGUID)
{
}

const char* GameFramework::GetGameGUID()
{
	return nullptr;
}

INetContext* GameFramework::GetNetContext()
{
	return nullptr;
}

void GameFramework::GetMemoryStatistics(ICrySizer*)
{
}

void GameFramework::EnableVoiceRecording(const bool enable)
{
}

void GameFramework::MutePlayerById(EntityId mutePlayer)
{
}

IDebugHistoryManager* GameFramework::CreateDebugHistoryManager()
{
	return nullptr;
}

void GameFramework::DumpMemInfo(const char* format, ...)
{
}

bool GameFramework::IsVoiceRecordingEnabled()
{
	return false;
}

bool GameFramework::IsImmersiveMPEnabled()
{
	return false;
}

void GameFramework::ExecuteCommandNextFrame(const char* command)
{
}

void GameFramework::ShowPageInBrowser(const char* url)
{
}

bool GameFramework::StartProcess(const char* cmdLine)
{
	return false;
}

bool GameFramework::SaveServerConfig(const char* path)
{
	return false;
}

void GameFramework::PrefetchLevelAssets(const bool enforceAll)
{
	m_pItemSystem->PrecacheLevel();
}

bool GameFramework::GetModInfo(SModInfo* modInfo, const char* modPath)
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////

void GameFramework::UnknownFunction1()
{
}

void GameFramework::UnknownFunction2()
{
}

void GameFramework::DispatchActionEvent(const SActionEvent& event)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x324ea0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x241b00;
#endif

	(this->*reinterpret_cast<void(GameFramework::*&)(const SActionEvent&)>(func))(event);
}

void GameFramework::RegisterConsoleVariables()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x3212a0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x221ab0;
#endif

	(this->*reinterpret_cast<void(GameFramework::*&)()>(func))();
}

void GameFramework::RegisterConsoleCommands()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x3277a0;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x243600;
#endif

	(this->*reinterpret_cast<void(GameFramework::*&)()>(func))();
}

void GameFramework::RegisterScriptBindings()
{
	m_pScriptBind_Network = new ScriptBind_Network(m_pSystem, this);
	m_pScriptBind_CryAction = new ScriptBind_CryAction(this);
	m_pScriptBind_ItemSystem = new ScriptBind_ItemSystem(m_pSystem, m_pItemSystem, this);
	m_pScriptBind_Actor = new ScriptBind_Actor(m_pSystem, this);
	m_pScriptBind_AI = new ScriptBind_AI(m_pSystem);
	m_pScriptBind_ActionMapManager = new ScriptBind_ActionMapManager(m_pSystem, m_pActionMapManager);
	m_pScriptBind_VehicleSystem = new ScriptBind_VehicleSystem(m_pVehicleSystem);
	m_pScriptBind_Vehicle = new ScriptBind_Vehicle(m_pSystem, this);
	m_pScriptBind_VehicleSeat = new ScriptBind_VehicleSeat(m_pSystem, this);
	m_pScriptBind_Inventory = new ScriptBind_Inventory(m_pSystem, this);
	m_pScriptBind_DialogSystem = new ScriptBind_DialogSystem(m_pSystem, m_pDialogSystem);
}

void GameFramework::CheckEndLevelSchedule()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = CRYACTION_BASE + 0x324a50;
#else
	std::uintptr_t func = CRYACTION_BASE + 0x2418e0;
#endif

	(this->*reinterpret_cast<void(GameFramework::*&)()>(func))();
}
