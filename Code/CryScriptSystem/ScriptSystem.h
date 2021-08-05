#pragma once

#include <set>
#include <string>

#include "CryCommon/CryScriptSystem/IScriptSystem.h"
#include "Library/Util.h"

#include "ScriptTimerManager.h"
#include "ScriptBindings/ScriptBindings.h"

struct lua_State;
struct IConsoleCmdArgs;

class ScriptSystem : public IScriptSystem
{
	lua_State *m_L = nullptr;
	int m_funcParamCount = 0;
	int m_errorHandlerRef = 0;

	std::set<std::string, Util::TransparentStringCompareNoCase> m_loadedScripts;

	ScriptTimerManager m_timers;
	ScriptBindings m_bindings;

public:
	ScriptSystem();
	~ScriptSystem();

	void Init();

	void *Allocate(size_t size);
	void Deallocate(void *block);

	void PushAny(const ScriptAnyValue & any);
	void PushVec3(const Vec3 & vec);

	bool PopAny(ScriptAnyValue & any);
	bool PopAnys(ScriptAnyValue *anys, int count);

	bool ToAny(ScriptAnyValue & any, int index);
	bool ToVec3(Vec3 & vec, int index);

	ScriptTimerManager & GetScriptTimerManager()
	{
		return m_timers;
	}

	///////////////////
	// IScriptSystem //
	///////////////////

	void Update() override;
	void SetGCFrequency(const float rate) override;

	bool ExecuteFile(const char *fileName, bool raiseError = true, bool forceReload = false) override;
	bool ExecuteBuffer(const char *buffer, size_t bufferSize, const char *bufferDescription = "") override;

	void UnloadScript(const char *fileName) override;
	void UnloadScripts() override;

	bool ReloadScript(const char *fileName, bool raiseError = true) override;
	bool ReloadScripts() override;

	void DumpLoadedScripts() override;

	IScriptTable *CreateTable(bool empty = false) override;

	int BeginCall(HSCRIPTFUNCTION func) override;
	int BeginCall(const char *funcName) override;
	int BeginCall(const char *tableName, const char *funcName) override;
	int BeginCall(IScriptTable *pTable, const char *funcName) override;

	bool EndCall() override;
	bool EndCallAny(ScriptAnyValue & any) override;
	bool EndCallAnyN(int resultCount, ScriptAnyValue *anys) override;

	HSCRIPTFUNCTION GetFunctionPtr(const char *funcName) override;
	HSCRIPTFUNCTION GetFunctionPtr(const char *tableName, const char *funcName) override;

	void ReleaseFunc(HSCRIPTFUNCTION func) override;

	void PushFuncParamAny(const ScriptAnyValue & any) override;

	void SetGlobalAny(const char *key, const ScriptAnyValue & any) override;
	bool GetGlobalAny(const char *key, ScriptAnyValue & any) override;

	void SetGlobalToNull(const char *key) override;

	IScriptTable *CreateUserData(void *data, size_t dataSize) override;

	void ForceGarbageCollection() override;

	int GetCGCount() override;
	void SetGCThreshhold(int kilobytes) override;

	void GetMemoryStatistics(ICrySizer *pSizer) override;

	void RaiseError(const char *format, ...) override;

	void PostInit() override;

	void SerializeTimers(ISerialize *pSer) override;
	void ResetTimers() override;

	int GetStackSize() override;
	uint32_t GetScriptAllocSize() override;

private:
	void LuaInit();
	void LuaClose();
	void LuaGarbageCollectStep();
	void LuaGarbageCollectFull();
	bool LuaCall(int paramCount, int resultCount);

	std::string SanitizeScriptFileName(const char *fileName);

	static int ErrorHandler(lua_State *L);
	static int PanicHandler(lua_State *L);

	static void *LuaAllocator(void *userData, void *originalBlock, size_t originalSize, size_t newSize);

	static void OnDumpStateCmd(IConsoleCmdArgs *pArgs);
	static void OnDumpScriptsCmd(IConsoleCmdArgs *pArgs);
	static void OnGarbageCollectCmd(IConsoleCmdArgs *pArgs);
};
