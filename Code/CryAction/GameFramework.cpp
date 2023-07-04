#include <cstdint>

#include "Library/StringTools.h"
#include "Library/WinAPI.h"

#include "CryCommon/CrySystem/gEnv.h"
#include "CryCommon/CrySystem/IConsole.h"

#include "ActorSystem.h"
#include "DevMode.h"
#include "EffectSystem.h"
#include "GameFramework.h"
#include "GameTokenSystem.h"
#include "ItemSystem.h"
#include "LevelSystem.h"
#include "ScriptRMI.h"
#include "TimeDemoRecorder.h"
#include "UIDraw.h"

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

	vtable[7] = current_vtable[7];    // GameFramework::Init
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

IGameFramework& GameFramework::GetInstance()
{
	static GameFramework instance;
	return instance;
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

void GameFramework::RegisterFactory(const char* actor, IActorCreator*, bool isAI)
{
}

void GameFramework::RegisterFactory(const char* item, IItemCreator*, bool isAI)
{
}

void GameFramework::RegisterFactory(const char* vehicle, IVehicleCreator*, bool isAI)
{
}

void GameFramework::RegisterFactory(const char* gameObjectExtension, IGameObjectExtensionCreator*, bool isAI)
{
}

bool GameFramework::Init(SSystemInitParams& startupParams)
{
	using CrySystemEntry = ISystem* (*)(SSystemInitParams& startupParams);

	// our launcher takes care of loading CrySystem DLL
	void* pCrySystemDLL = WinAPI::DLL::Get("CrySystem.dll");
	if (!pCrySystemDLL)
	{
		throw StringTools::ErrorFormat("The CrySystem DLL is not loaded!");
	}

	auto entry = static_cast<CrySystemEntry>(WinAPI::DLL::GetSymbol(pCrySystemDLL, "CreateSystemInterface"));
	if (!entry)
	{
		throw StringTools::ErrorFormat("The CrySystem DLL is not valid!");
	}

	m_pSystem = entry(startupParams);
	if (!m_pSystem)
	{
		throw StringTools::ErrorFormat("CrySystem initialization failed!");
	}

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
	// our client sets a random CD key during its initialization
	// so let's skip obtaining CD key here

#ifdef BUILD_64BIT
	std::uintptr_t registerConsoleVariables = 0x308212a0;
#else
	std::uintptr_t registerConsoleVariables = 0x30721ab0;
#endif
	// call the original registration of console variables
	(this->*reinterpret_cast<decltype(&GameFramework::RegisterConsoleVariables)&>(registerConsoleVariables))();

#ifdef BUILD_64BIT
	std::uintptr_t registerConsoleCommands = 0x308277a0;
#else
	std::uintptr_t registerConsoleCommands = 0x30743600;
#endif
	// call the original registration of console commands
	(this->*reinterpret_cast<decltype(&GameFramework::RegisterConsoleCommands)&>(registerConsoleCommands))();

	if (m_pSystem->IsDevMode())
	{
		m_pDevMode = new DevMode();
	}

	m_pTimeDemoRecorder = new TimeDemoRecorder(m_pSystem);

	IConsole* pConsole = gEnv->pConsole;

#ifdef BUILD_64BIT
	ICVar** pLogRMICVar = reinterpret_cast<ICVar**>(0x30961068);
	ICVar** pDisconnectOnRMIErrorCVar = reinterpret_cast<ICVar**>(0x30961070);
#else
	ICVar** pLogRMICVar = reinterpret_cast<ICVar**>(0x307d79a8);
	ICVar** pDisconnectOnRMIErrorCVar = reinterpret_cast<ICVar**>(0x307d79ac);
#endif

	*pLogRMICVar = pConsole->RegisterInt("net_log_remote_methods", 0, VF_DUMPTODISK,
		"Log remote method invocations."
	);
	*pDisconnectOnRMIErrorCVar = pConsole->RegisterInt("net_disconnect_on_rmi_error", 0, VF_DUMPTODISK,
		"Disconnect remote connections on script exceptions during RMI calls."
	);

#ifdef BUILD_64BIT
	ICVar** pForceFastUpdateCVar = reinterpret_cast<ICVar**>(0x30941ae0);
	ICVar** pVisibilityTimeoutCVar = reinterpret_cast<ICVar**>(0x30941ae8);
	ICVar** pVisibilityTimeoutTimeCVar = reinterpret_cast<ICVar**>(0x30941af0);
	int* pShowUpdateState = reinterpret_cast<int*>(0x30941adc);
#else
	ICVar** pForceFastUpdateCVar = reinterpret_cast<ICVar**>(0x307b9030);
	ICVar** pVisibilityTimeoutCVar = reinterpret_cast<ICVar**>(0x307b9034);
	ICVar** pVisibilityTimeoutTimeCVar = reinterpret_cast<ICVar**>(0x307b9038);
	int* pShowUpdateState = reinterpret_cast<int*>(0x307b902c);
#endif

	*pForceFastUpdateCVar = pConsole->RegisterInt("g_goForceFastUpdate", 0, VF_CHEAT,
		"GameObjects IsProbablyVisible->TRUE && IsProbablyDistant()->FALSE"
	);
	*pVisibilityTimeoutCVar = pConsole->RegisterInt("g_VisibilityTimeout", 0, VF_CHEAT,
		"Add visibility timeout to IsProbablyVisible() calculations."
	);
	*pVisibilityTimeoutTimeCVar = pConsole->RegisterFloat("g_VisibilityTimeoutTime", 30.0f, VF_CHEAT,
		"Visibility timeout time used by IsProbablyVisible() calculations."
	);
	pConsole->Register("g_showUpdateState", pShowUpdateState, 0, VF_CHEAT,
		"Show the game object update state of any activated entities; 3-4 -- AI objects only."
	);

	m_pScriptRMI = new ScriptRMI();
	m_pGameTokenSystem = new GameTokenSystem();
	m_pEffectSystem = new EffectSystem();
	m_pEffectSystem->Init();
	m_pUIDraw = new UIDraw();
	m_pLevelSystem = new LevelSystem(m_pSystem, "levels");
	m_pActorSystem = new ActorSystem(m_pSystem, m_pEntitySystem);
	m_pItemSystem = new ItemSystem(this, m_pSystem);

	// TODO
	throw StringTools::ErrorFormat("End of GameFramework::Init reached!!");

	return true;
}

bool GameFramework::CompleteInit()
{
	return false;
}

bool GameFramework::PreUpdate(bool haveFocus, unsigned int updateFlags)
{
	return false;
}

void GameFramework::PostUpdate(bool haveFocus, unsigned int updateFlags)
{
}

void GameFramework::Shutdown()
{
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
	return nullptr;
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
	return nullptr;
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

IDebrisMgr* GameFramework::GetDebrisMgr ()
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

void GameFramework::RegisterConsoleVariables()
{
}

void GameFramework::RegisterConsoleCommands()
{
}
