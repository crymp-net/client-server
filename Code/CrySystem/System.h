#pragma once

#include "CryCommon/CrySystem/ISystem.h"

class System final : public ISystem
{
	// m_reserved_<32-bit-offset>_<64-bit-offset>
	IGame* m_pGame = nullptr;

public:
	System();
	~System();

	////////////////////////////////////////////////////////////////////////////////
	// ISystem
	////////////////////////////////////////////////////////////////////////////////

	void Release() override;

	SSystemGlobalEnvironment* GetGlobalEnvironment() override;

	const char* GetRootFolder() const override;

	bool Update(int updateFlags = 0, int pauseMode = 0) override;

	void RenderBegin() override;
	void Render() override;
	void RenderEnd() override;

	void RenderStatistics() override;

	void* AllocMem(void* oldptr, std::size_t newsize) override;

	std::uint32_t GetUsedMemory() override;

	const char* GetUserName() override;

	int GetCPUFlags() override;

	double GetSecondsPerCycle() override;

	void DumpMemoryUsageStatistics(bool useKB = false) override;

	void Quit() override;

	void Relaunch(bool relaunch) override;
	bool IsRelaunch() const override;

	void SerializingFile(int mode) override;
	int IsSerializingFile() const override;

	bool IsQuitting() override;

	void Error(const char* format, ...) override;

	void Warning(EValidatorModule module, EValidatorSeverity severity, int flags, const char* file, const char* format, ...) override;
	bool CheckLogVerbosity(int verbosity) override;

	bool IsDedicated() override;
	bool IsEditor() override;
	bool IsEditorMode() override;

	IConsole* GetIConsole() override;
	IScriptSystem* GetIScriptSystem() override;
	I3DEngine* GetI3DEngine() override;
	ISoundSystem* GetISoundSystem() override;
	IMusicSystem* GetIMusicSystem() override;
	IPhysicalWorld* GetIPhysicalWorld() override;
	IMovieSystem* GetIMovieSystem() override;
	IAISystem* GetAISystem() override;
	IMemoryManager* GetIMemoryManager() override;
	IEntitySystem* GetIEntitySystem() override;
	ICryFont* GetICryFont() override;
	ICryPak* GetIPak() override;
	ILog* GetILog() override;
	ICmdLine* GetICmdLine() override;
	IStreamEngine* GetStreamEngine() override;
	ICharacterManager* GetIAnimationSystem() override;
	IValidator* GetIValidator() override;
	IFrameProfileSystem* GetIProfileSystem() override;
	INameTable* GetINameTable() override;
	IBudgetingSystem* GetIBudgetingSystem() override;
	IFlowSystem* GetIFlowSystem() override;
	IAnimationGraphSystem* GetIAnimationGraphSystem() override;
	IDialogSystem* GetIDialogSystem() override;
	IHardwareMouse* GetIHardwareMouse() override;
	ISystemEventDispatcher* GetISystemEventDispatcher() override;
	IGlobalTaskScheduler* GetIGlobalTaskScheduler() override;
	IThreadTaskManager* GetIThreadTaskManager() override;
	IGame* GetIGame() override;
	INetwork* GetINetwork() override;
	IRenderer* GetIRenderer() override;
	IInput* GetIInput() override;
	ITimer* GetITimer() override;

	IGPUPhysicsManager* GetIGPUPhysicsManager() override;

	void SetLoadingProgressListener(ILoadingProgressListener* pListener) override;
	ILoadingProgressListener* GetLoadingProgressListener() const override;

	void SetIGame(IGame* pGame) override;
	void SetIFlowSystem(IFlowSystem* pFlowSystem) override;
	void SetIAnimationGraphSystem(IAnimationGraphSystem* pAnimationGraphSystem) override;
	void SetIDialogSystem(IDialogSystem* pDialogSystem) override;
	void SetIMaterialEffects(IMaterialEffects* pMaterialEffects) override;

	void ChangeUserPath(const char* userPath) override;

	void DebugStats(bool checkpoint, bool leaks) override;
	void DumpWinHeaps() override;
	int DumpMMStats(bool log) override;

	void SetForceNonDevMode(const bool value) override;
	bool GetForceNonDevMode() const override;
	bool WasInDevMode() const override;
	bool IsDevMode() const override;

	bool IsMODValid(const char* modName) const override;

	XmlNodeRef CreateXmlNode(const char* nodeName = "") override;
	XmlNodeRef LoadXmlFile(const char* filename) override;
	XmlNodeRef LoadXmlFromString(const char* xmlString) override;
	IXmlUtils* GetXmlUtils() override;

	void SetViewCamera(CCamera& camera) override;
	CCamera& GetViewCamera() override;

	void IgnoreUpdates(bool ignore) override;

	void SetIProcess(IProcess* process) override;
	IProcess* GetIProcess() override;

	bool IsTestMode() const override;

	void SetFrameProfiler(bool on, bool display, char* prefix) override;

	void VTuneResume() override;
	void VTunePause() override;

	const SFileVersion& GetFileVersion() override;
	const SFileVersion& GetProductVersion() override;

	bool WriteCompressedFile(const char* filename, void* data, unsigned int bitlen) override;
	unsigned int ReadCompressedFile(const char* filename, void* data, unsigned int maxbitlen) override;
	unsigned int GetCompressedFileSize(const char* filename) override;
	bool CompressDataBlock(const void* input, std::size_t inputSize, void* output, std::size_t& outputSize, int level = 3) override;
	bool DecompressDataBlock(const void* input, std::size_t inputSize, void* output, std::size_t& outputSize) override;

	IDataProbe* GetIDataProbe() override;

	void SaveConfiguration() override;
	void LoadConfiguration(const char* filename, ILoadConfigurationEntrySink* pSink = nullptr) override;

	ESystemConfigSpec GetConfigSpec(bool client = true) override;
	ESystemConfigSpec GetMaxConfigSpec() const override;
	void SetConfigSpec(ESystemConfigSpec spec, bool client) override;
	void AutoDetectSpec() override;

	int SetThreadState(ESubsystem subsystem, bool active) override;

	ICrySizer* CreateSizer() override;

	bool IsPaused() const override;

	ILocalizationManager* GetLocalizationManager() override;

	IFlashPlayer* CreateFlashPlayerInstance() const override;
	void SetFlashLoadMovieHandler(IFlashLoadMovieHandler* pHandler) const override;

	IAVI_Reader* CreateAVIReader() override;
	void ReleaseAVIReader(IAVI_Reader* pAVIReader) override;

	void debug_GetCallStack(const char** pFunctions, int& count) override;
	void debug_LogCallStack(int maxFuncs = 32, int flags = 0) override;

	void ApplicationTest(const char* param) override;
	ITestSystem* GetITestSystem() override;

	void ExecuteCommandLine() override;

	void DumpMemoryCoverage() override;

	ITextModeConsole* GetITextModeConsole() override;

	Crc32Gen* GetCrc32Gen() override;
	CPNoise3* GetNoiseGen() override;

	std::uint64_t GetUpdateCounter() override;

	////////////////////////////////////////////////////////////////////////////////

	virtual bool Initialize(const SSystemInitParams& params);
};
