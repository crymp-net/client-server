#include "CryCommon/CryMath/Cry_Camera.h"

#include "System.h"

System::System()
{
#ifdef BUILD_64BIT
	//static_assert(sizeof(System) == 0x1a18);
#else
	//static_assert(sizeof(System) == 0x11e0);
#endif
}

System::~System()
{
}

////////////////////////////////////////////////////////////////////////////////
// ISystem
////////////////////////////////////////////////////////////////////////////////

void System::Release()
{
	// TODO
}

SSystemGlobalEnvironment* System::GetGlobalEnvironment()
{
	// TODO
	return {};
}

const char* System::GetRootFolder() const
{
	// TODO
	return {};
}

bool System::Update(int updateFlags, int pauseMode)
{
	// TODO
	return {};
}

void System::RenderBegin()
{
	// TODO
}

void System::Render()
{
	// TODO
}

void System::RenderEnd()
{
	// TODO
}

void System::RenderStatistics()
{
	// TODO
}

void* System::AllocMem(void* oldptr, std::size_t newsize)
{
	// TODO
	return {};
}

std::uint32_t System::GetUsedMemory()
{
	// TODO
	return {};
}

const char* System::GetUserName()
{
	// TODO
	return {};
}

int System::GetCPUFlags()
{
	// TODO
	return {};
}

double System::GetSecondsPerCycle()
{
	// TODO
	return {};
}

void System::DumpMemoryUsageStatistics(bool useKB)
{
	// TODO
}

void System::Quit()
{
	// TODO
}

void System::Relaunch(bool relaunch)
{
	// TODO
}

bool System::IsRelaunch() const
{
	// TODO
	return {};
}

void System::SerializingFile(int mode)
{
	// TODO
}

int System::IsSerializingFile() const
{
	// TODO
	return {};
}

bool System::IsQuitting()
{
	// TODO
	return {};
}

void System::Error(const char* format, ...)
{
	// TODO
}

void System::Warning(EValidatorModule module, EValidatorSeverity severity, int flags, const char* file, const char* format, ...)
{
	// TODO
}

bool System::CheckLogVerbosity(int verbosity)
{
	// TODO
	return {};
}

bool System::IsDedicated()
{
	// TODO
	return {};
}

bool System::IsEditor()
{
	// TODO
	return {};
}

bool System::IsEditorMode()
{
	// TODO
	return {};
}

IConsole* System::GetIConsole()
{
	// TODO
	return {};
}

IScriptSystem* System::GetIScriptSystem()
{
	// TODO
	return {};
}

I3DEngine* System::GetI3DEngine()
{
	// TODO
	return {};
}

ISoundSystem* System::GetISoundSystem()
{
	// TODO
	return {};
}

IMusicSystem* System::GetIMusicSystem()
{
	// TODO
	return {};
}

IPhysicalWorld* System::GetIPhysicalWorld()
{
	// TODO
	return {};
}

IMovieSystem* System::GetIMovieSystem()
{
	// TODO
	return {};
}

IAISystem* System::GetAISystem()
{
	// TODO
	return {};
}

IMemoryManager* System::GetIMemoryManager()
{
	// TODO
	return {};
}

IEntitySystem* System::GetIEntitySystem()
{
	// TODO
	return {};
}

ICryFont* System::GetICryFont()
{
	// TODO
	return {};
}

ICryPak* System::GetIPak()
{
	// TODO
	return {};
}

ILog* System::GetILog()
{
	// TODO
	return {};
}

ICmdLine* System::GetICmdLine()
{
	// TODO
	return {};
}

IStreamEngine* System::GetStreamEngine()
{
	// TODO
	return {};
}

ICharacterManager* System::GetIAnimationSystem()
{
	// TODO
	return {};
}

IValidator* System::GetIValidator()
{
	// TODO
	return {};
}

IFrameProfileSystem* System::GetIProfileSystem()
{
	// TODO
	return {};
}

INameTable* System::GetINameTable()
{
	// TODO
	return {};
}

IBudgetingSystem* System::GetIBudgetingSystem()
{
	// TODO
	return {};
}

IFlowSystem* System::GetIFlowSystem()
{
	// TODO
	return {};
}

IAnimationGraphSystem* System::GetIAnimationGraphSystem()
{
	// TODO
	return {};
}

IDialogSystem* System::GetIDialogSystem()
{
	// TODO
	return {};
}

IHardwareMouse* System::GetIHardwareMouse()
{
	// TODO
	return {};
}

ISystemEventDispatcher* System::GetISystemEventDispatcher()
{
	// TODO
	return {};
}

IGlobalTaskScheduler* System::GetIGlobalTaskScheduler()
{
	// TODO
	return {};
}

IThreadTaskManager* System::GetIThreadTaskManager()
{
	// TODO
	return {};
}

IGame* System::GetIGame()
{
	// TODO
	return {};
}

INetwork* System::GetINetwork()
{
	// TODO
	return {};
}

IRenderer* System::GetIRenderer()
{
	// TODO
	return {};
}

IInput* System::GetIInput()
{
	// TODO
	return {};
}

ITimer* System::GetITimer()
{
	// TODO
	return {};
}

IGPUPhysicsManager* System::GetIGPUPhysicsManager()
{
	// TODO
	return {};
}

void System::SetLoadingProgressListener(ILoadingProgressListener* pListener)
{
	// TODO
}

ISystem::ILoadingProgressListener* System::GetLoadingProgressListener() const
{
	// TODO
	return {};
}

void System::SetIGame(IGame* pGame)
{
	// TODO
}

void System::SetIFlowSystem(IFlowSystem* pFlowSystem)
{
	// TODO
}

void System::SetIAnimationGraphSystem(IAnimationGraphSystem* pAnimationGraphSystem)
{
	// TODO
}

void System::SetIDialogSystem(IDialogSystem* pDialogSystem)
{
	// TODO
}

void System::SetIMaterialEffects(IMaterialEffects* pMaterialEffects)
{
	// TODO
}

void System::ChangeUserPath(const char* userPath)
{
	// TODO
}

void System::DebugStats(bool checkpoint, bool leaks)
{
	// TODO
}

void System::DumpWinHeaps()
{
	// TODO
}

int System::DumpMMStats(bool log)
{
	// TODO
	return {};
}

void System::SetForceNonDevMode(const bool value)
{
	// TODO
}

bool System::GetForceNonDevMode() const
{
	// TODO
	return {};
}

bool System::WasInDevMode() const
{
	// TODO
	return {};
}

bool System::IsDevMode() const
{
	// TODO
	return {};
}

bool System::IsMODValid(const char* modName) const
{
	// TODO
	return {};
}

XmlNodeRef System::CreateXmlNode(const char* nodeName)
{
	// TODO
	return {};
}

XmlNodeRef System::LoadXmlFile(const char* filename)
{
	// TODO
	return {};
}

XmlNodeRef System::LoadXmlFromString(const char* xmlString)
{
	// TODO
	return {};
}

IXmlUtils* System::GetXmlUtils()
{
	// TODO
	return {};
}

void System::SetViewCamera(CCamera& camera)
{
	// TODO
}

CCamera& System::GetViewCamera()
{
	// TODO
	static CCamera camera;
	return camera;
}

void System::IgnoreUpdates(bool ignore)
{
	// TODO
}

void System::SetIProcess(IProcess* process)
{
	// TODO
}

IProcess* System::GetIProcess()
{
	// TODO
	return {};
}

bool System::IsTestMode() const
{
	// TODO
	return {};
}

void System::SetFrameProfiler(bool on, bool display, char* prefix)
{
	// TODO
}

void System::VTuneResume()
{
	// TODO
}

void System::VTunePause()
{
	// TODO
}

const SFileVersion& System::GetFileVersion()
{
	// TODO
	static SFileVersion version;
	return version;
}

const SFileVersion& System::GetProductVersion()
{
	// TODO
	static SFileVersion version;
	return version;
}

bool System::WriteCompressedFile(const char* filename, void* data, unsigned int bitlen)
{
	// TODO
	return {};
}

unsigned int System::ReadCompressedFile(const char* filename, void* data, unsigned int maxbitlen)
{
	// TODO
	return {};
}

unsigned int System::GetCompressedFileSize(const char* filename)
{
	// TODO
	return {};
}

bool System::CompressDataBlock(const void* input, std::size_t inputSize, void* output, std::size_t& outputSize, int level)
{
	// TODO
	return {};
}

bool System::DecompressDataBlock(const void* input, std::size_t inputSize, void* output, std::size_t& outputSize)
{
	// TODO
	return {};
}

IDataProbe* System::GetIDataProbe()
{
	// TODO
	return {};
}

void System::SaveConfiguration()
{
	// TODO
}

void System::LoadConfiguration(const char* filename, ILoadConfigurationEntrySink* pSink)
{
	// TODO
}

ESystemConfigSpec System::GetConfigSpec(bool client)
{
	// TODO
	return {};
}

ESystemConfigSpec System::GetMaxConfigSpec() const
{
	// TODO
	return {};
}

void System::SetConfigSpec(ESystemConfigSpec spec, bool client)
{
	// TODO
}

void System::AutoDetectSpec()
{
	// TODO
}

int System::SetThreadState(ESubsystem subsystem, bool active)
{
	// TODO
	return {};
}

ICrySizer* System::CreateSizer()
{
	// TODO
	return {};
}

bool System::IsPaused() const
{
	// TODO
	return {};
}

ILocalizationManager* System::GetLocalizationManager()
{
	// TODO
	return {};
}

IFlashPlayer* System::CreateFlashPlayerInstance() const
{
	// TODO
	return {};
}

void System::SetFlashLoadMovieHandler(IFlashLoadMovieHandler* pHandler) const
{
	// TODO
}

IAVI_Reader* System::CreateAVIReader()
{
	// TODO
	return {};
}

void System::ReleaseAVIReader(IAVI_Reader* pAVIReader)
{
	// TODO
}

void System::debug_GetCallStack(const char** pFunctions, int& count)
{
	// TODO
}

void System::debug_LogCallStack(int maxFuncs, int flags)
{
	// TODO
}

void System::ApplicationTest(const char* param)
{
	// TODO
}

ITestSystem* System::GetITestSystem()
{
	// TODO
	return {};
}

void System::ExecuteCommandLine()
{
	// TODO
}

void System::DumpMemoryCoverage()
{
	// TODO
}

ITextModeConsole* System::GetITextModeConsole()
{
	// TODO
	return {};
}

Crc32Gen* System::GetCrc32Gen()
{
	// TODO
	return {};
}

CPNoise3* System::GetNoiseGen()
{
	// TODO
	return {};
}

std::uint64_t System::GetUpdateCounter()
{
	// TODO
	return {};
}

////////////////////////////////////////////////////////////////////////////////

bool System::Initialize(const SSystemInitParams& params)
{
	// TODO
	return {};
}
