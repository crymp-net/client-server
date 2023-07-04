#pragma once

#include "CryCommon/CryAction/IGameFramework.h"

struct ICVar;

class DevMode;
class ScriptRMI;
class TimeDemoRecorder;

class GameFramework : public IGameFramework
{
	void* m_reserved_0x4_0x8 = {};
	ISystem* m_pSystem = nullptr;
	INetwork* m_pNetwork = nullptr;
	I3DEngine* m_p3DEngine = nullptr;
	IScriptSystem* m_pScriptSystem = nullptr;
	IEntitySystem* m_pEntitySystem = nullptr;
	ITimer* m_pTimer = nullptr;
	ILog* m_pLog = nullptr;
	void* m_reserved_0x24_0x48[2] = {};
	char m_reserved_0x2c_0x58[1024] = {};
	char m_guid[128] = "{00000000-0000-0000-0000-000000000000}";
	ILevelSystem* m_pLevelSystem = nullptr;
	IActorSystem* m_pActorSystem = nullptr;
	IItemSystem* m_pItemSystem = nullptr;
	void* m_reserved_0x4b8_0x4f0[6] = {};
	IUIDraw* m_pUIDraw = nullptr;
	void* m_reserved_0x4d4_0x528 = {};
	ScriptRMI* m_pScriptRMI = nullptr;
	void* m_reserved_0x4dc_0x538[6] = {};
	IGameTokenSystem* m_pGameTokenSystem = nullptr;
	IEffectSystem* m_pEffectSystem = nullptr;
	void* m_reserved_0x4fc_0x578[5] = {};
	DevMode* m_pDevMode = nullptr;
	TimeDemoRecorder* m_pTimeDemoRecorder = nullptr;
	void* m_reserved_0x518_0x5b0[21] = {};
	ICVar* m_pEnableLoadingScreenCVar = nullptr;
	void* m_reserved_0x570_0x660 = {};
	ICVar* m_pLANBrowserCVar = nullptr;
	int m_reserved_0x578_0x670[3] = {};
	void* m_reserved_0x584_0x680[3] = {};
	int m_voiceRecording = 0;
	bool m_unknownFlag1 = true;
	bool m_unknownFlag2 = true;
	void* m_reserved_0x598_0x6a0[64] = {};  // some free space at the end, total size is unknown

	GameFramework();

public:
	virtual ~GameFramework();

	static IGameFramework& GetInstance();

	////////////////////////////////////////////////////////////////////////////////
	// IGameFramework
	////////////////////////////////////////////////////////////////////////////////

	void RegisterFactory(const char* name, IAnimationStateNodeFactory* (*)(), bool isAI) override;
	void RegisterFactory(const char* name, ISaveGame* (*)(), bool isAI) override;
	void RegisterFactory(const char* name, ILoadGame* (*)(), bool isAI) override;
	void RegisterFactory(const char* actor, IActorCreator*, bool isAI) override;
	void RegisterFactory(const char* item, IItemCreator*, bool isAI) override;
	void RegisterFactory(const char* vehicle, IVehicleCreator*, bool isAI) override;
	void RegisterFactory(const char* gameObjectExtension, IGameObjectExtensionCreator*, bool isAI) override;

	bool Init(SSystemInitParams& startupParams) override;
	bool CompleteInit() override;
	bool PreUpdate(bool haveFocus, unsigned int updateFlags) override;
	void PostUpdate(bool haveFocus, unsigned int updateFlags) override;
	void Shutdown() override;
	void Reset(bool clients) override;
	void PauseGame(bool pause, bool force) override;
	bool IsGamePaused() override;
	bool IsGameStarted() override;

	ISystem* GetISystem() override;
	ILanQueryListener* GetILanQueryListener() override;
	IUIDraw* GetIUIDraw() override;
	IGameObjectSystem* GetIGameObjectSystem() override;
	ILevelSystem* GetILevelSystem() override;
	IActorSystem* GetIActorSystem() override;
	IItemSystem* GetIItemSystem() override;
	IActionMapManager* GetIActionMapManager() override;
	IViewSystem* GetIViewSystem() override;
	IGameplayRecorder* GetIGameplayRecorder() override;
	IVehicleSystem* GetIVehicleSystem() override;
	IGameRulesSystem* GetIGameRulesSystem() override;
	IFlowSystem* GetIFlowSystem() override;
	IGameTokenSystem* GetIGameTokenSystem() override;
	IEffectSystem* GetIEffectSystem() override;
	IMaterialEffects* GetIMaterialEffects() override;
	IDialogSystem* GetIDialogSystem() override;
	IPlayerProfileManager* GetIPlayerProfileManager() override;
	IDebrisMgr* GetDebrisMgr () override;
	ISubtitleManager* GetISubtitleManager() override;

	bool StartGameContext(const SGameStartParams* pGameStartParams) override;
	bool ChangeGameContext(const SGameContextParams* pGameContextParams) override;
	void EndGameContext() override;
	bool StartedGameContext() const override;

	bool BlockingSpawnPlayer() override;

	void ResetBrokenGameObjects() override;

	void SetEditorLevel(const char* levelName, const char* levelFolder) override;
	void GetEditorLevel(char** levelName, char** levelFolder) override;

	void BeginLanQuery() override;
	void EndCurrentQuery() override;

	IActor* GetClientActor() const override;
	EntityId GetClientActorId() const override;
	INetChannel* GetClientChannel() const override;
	CTimeValue GetServerTime() override;
	uint16 GetGameChannelId(INetChannel* pNetChannel) override;
	bool IsChannelOnHold(uint16 channelId) override;
	INetChannel* GetNetChannel(uint16 channelId) override;
	IGameObject* GetGameObject(EntityId id) override;

	bool GetNetworkSafeClassId(uint16& id, const char* className) override;
	bool GetNetworkSafeClassName(char* className, size_t maxn, uint16 id) override;

	IGameObjectExtension* QueryGameObjectExtension(EntityId id, const char* name) override;

	bool SaveGame(const char* path, bool quick, bool forceImmediate, ESaveGameReason reason, bool ignoreDelay, const char* checkPoint) override;
	bool LoadGame(const char* path, bool quick, bool ignoreDelay) override;

	void OnEditorSetGameMode(int mode) override;

	bool IsEditing() override;
	bool IsInLevelLoad() override;
	bool IsLoadingSaveGame() override;
	bool IsInTimeDemo() override;

	void AllowSave(bool allow) override;
	void AllowLoad(bool allow) override;
	bool CanSave() override;
	bool CanLoad() override;

	bool CanCheat() override;

	const char* GetLevelName() override;

	const char* GetAbsLevelPath() override;

	IPersistantDebug* GetIPersistantDebug() override;
	IGameStatsConfig* GetIGameStatsConfig() override;

	IAnimationGraphState* GetMusicGraphState() override;
	IMusicLogic* GetMusicLogic() override;

	void RegisterListener(IGameFrameworkListener* pGameFrameworkListener, const char* name, EFRAMEWORKLISTENERPRIORITY priority) override;
	void UnregisterListener(IGameFrameworkListener* pGameFrameworkListener) override;

	INetNub* GetServerNetNub() override;

	void SetGameGUID(const char* gameGUID) override;
	const char* GetGameGUID() override;
	INetContext* GetNetContext() override;

	void GetMemoryStatistics(ICrySizer*) override;

	void EnableVoiceRecording(const bool enable) override;

	void MutePlayerById(EntityId mutePlayer) override;

	IDebugHistoryManager* CreateDebugHistoryManager() override;

	void DumpMemInfo(const char* format, ...) override;

	bool IsVoiceRecordingEnabled() override;

	bool IsImmersiveMPEnabled() override;

	void ExecuteCommandNextFrame(const char* command) override;

	void ShowPageInBrowser(const char* url) override;

	bool StartProcess(const char* cmdLine) override;

	bool SaveServerConfig(const char* path) override;

	void PrefetchLevelAssets(const bool enforceAll) override;

	bool GetModInfo(SModInfo* modInfo, const char* modPath) override;

	////////////////////////////////////////////////////////////////////////////////

	virtual void UnknownFunction1();
	virtual void UnknownFunction2();

private:
	void RegisterConsoleVariables();
	void RegisterConsoleCommands();
};
