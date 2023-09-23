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

static void RegisterInventoryFactory(IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	auto* pInventoryCreator = reinterpret_cast<IGameFramework::IGameObjectExtensionCreator*>(0x309651a8);
	auto* vtable = reinterpret_cast<void*>(0x3088c968);
#else
	auto* pInventoryCreator = reinterpret_cast<IGameFramework::IGameObjectExtensionCreator*>(0x307db8dc);
	auto* vtable = reinterpret_cast<void*>(0x30798bd4);
#endif

	// construct
	*reinterpret_cast<void**>(pInventoryCreator) = vtable;

	pGameFramework->RegisterFactory("Inventory", pInventoryCreator, false);
}

static void RegisterSomeExtensions(IGameFramework* pGameFramework)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x307bb6d0;
#else
	std::uintptr_t func = 0x306dbcb0;
#endif

	reinterpret_cast<void(*)(IGameFramework*)>(func)(pGameFramework);
}

GameFramework::GameFramework()
{
	static constinit std::uintptr_t vtable[97] = {
#ifdef BUILD_64BIT
		0x30819390, 0x30819380, 0x30819370, 0x30819360, 0x308191f0, 0x308191e0, 0x3081d250, 0x308286d0,
		0x3081cbb0, 0x308252a0, 0x30824700, 0x30818bb0, 0x3081b040, 0x30818bc0, 0x30818d60, 0x3081a260,
		0x30785590, 0x3081afa0, 0x308193c0, 0x30819450, 0x308193d0, 0x308193e0, 0x308193f0, 0x308193b0,
		0x30819420, 0x30819430, 0x30819410, 0x30819440, 0x30819090, 0x30819460, 0x30819470, 0x30819480,
		0x30819490, 0x308194a0, 0x3081b020, 0x30819900, 0x3081b110, 0x3081a2c0, 0x3081b230, 0x3081b030,
		0x3081a280, 0x3081a2a0, 0x30818d90, 0x30818de0, 0x308190a0, 0x30819150, 0x3081a3b0, 0x3081a3d0,
		0x3081a400, 0x3081a430, 0x30819290, 0x30819320, 0x308192d0, 0x30819200, 0x3081d000, 0x3081d150,
		0x30819250, 0x30825600, 0x30818e00, 0x3081a2e0, 0x3081afb0, 0x3081a630, 0x3081a650, 0x308199e0,
		0x3081afc0, 0x3081afd0, 0x30819a00, 0x30819a60, 0x308195f0, 0x308194f0, 0x3081a4f0, 0x30819620,
		0x30819630, 0x3081afe0, 0x3081aff0, 0x30827e60, 0x308249a0, 0x3081a670, 0x308198c0, 0x3081b000,
		0x3081a690, 0x30821a50, 0x308198f0, 0x30821fc0, 0x308285e0, 0x30823e60, 0x3081b010, 0x3081a6b0,
		0x3081d860, 0x30819810, 0x30819840, 0x3081b580, 0x308197f0, 0x30819800, 0x3082a300, 0x3081d800,
		0x3081a370,
#else
		0x3071cd60, 0x3071cd50, 0x3071cd30, 0x3071cd20, 0x3071cbf0, 0x3071cbd0, 0x3071f760, 0x30743ab0,
		0x3071eff0, 0x30742570, 0x307415f0, 0x3071c600, 0x3071f400, 0x3071c610, 0x3071c780, 0x3071e030,
		0x306b2d50, 0x307450c0, 0x3071cd90, 0x3071ce20, 0x3071cda0, 0x3071cdb0, 0x3071cdc0, 0x3071cd80,
		0x3071cdf0, 0x3071ce00, 0x3071cde0, 0x3071ce10, 0x3071ca30, 0x3071ce30, 0x3071ce40, 0x3071ce50,
		0x3071ce60, 0x3071ce70, 0x30745140, 0x3071d250, 0x3071f4c0, 0x3071e070, 0x3071f580, 0x30745150,
		0x3071e050, 0x3071e060, 0x3071c7b0, 0x3071c7f0, 0x3071ca80, 0x3071cb20, 0x3071e1b0, 0x3071e1c0,
		0x3071e1e0, 0x3071e220, 0x3071cc80, 0x3071ccf0, 0x3071ccb0, 0x3071cc10, 0x3071f660, 0x3071f6b0,
		0x3071cc50, 0x30742850, 0x3071c810, 0x3071e090, 0x307450d0, 0x3071e3c0, 0x3071e3e0, 0x3071d300,
		0x307450e0, 0x307450f0, 0x3071d320, 0x3071d370, 0x3071cf90, 0x3071cec0, 0x3071e2c0, 0x3071cfc0,
		0x3071cfd0, 0x30745100, 0x30745110, 0x30742c70, 0x30741840, 0x3071e430, 0x3071d200, 0x30745120,
		0x3071e440, 0x3072f840, 0x3071d230, 0x3072fc30, 0x30743a50, 0x3073f150, 0x30745130, 0x3071e460,
		0x3071fa50, 0x3071d160, 0x3071d180, 0x3071fa90, 0x3071d130, 0x3071d140, 0x30749310, 0x3071fa00,
		0x3071e190,
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
	*reinterpret_cast<GameFramework**>(0x30964b40) = this;
#else
	*reinterpret_cast<GameFramework**>(0x307db0ac) = this;
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
	*reinterpret_cast<SSystemGlobalEnvironment**>(0x30964ae0) = m_pSystem->GetGlobalEnvironment();
#else
	*reinterpret_cast<SSystemGlobalEnvironment**>(0x307db06c) = m_pSystem->GetGlobalEnvironment();
#endif

	// register CryAction's global system event listener
#ifdef BUILD_64BIT
	m_pSystem->GetISystemEventDispatcher()->RegisterListener(reinterpret_cast<ISystemEventListener*>(0x30920bb0));
#else
	m_pSystem->GetISystemEventDispatcher()->RegisterListener(reinterpret_cast<ISystemEventListener*>(0x307a15e4));
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
	std::uintptr_t func = 0x30824ea0;
#else
	std::uintptr_t func = 0x30741b00;
#endif

	(this->*reinterpret_cast<void(GameFramework::*&)(const SActionEvent&)>(func))(event);
}

void GameFramework::RegisterConsoleVariables()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x308212a0;
#else
	std::uintptr_t func = 0x30721ab0;
#endif

	(this->*reinterpret_cast<void(GameFramework::*&)()>(func))();
}

void GameFramework::RegisterConsoleCommands()
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x308277a0;
#else
	std::uintptr_t func = 0x30743600;
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
	std::uintptr_t func = 0x30824a50;
#else
	std::uintptr_t func = 0x307418e0;
#endif

	(this->*reinterpret_cast<void(GameFramework::*&)()>(func))();
}
