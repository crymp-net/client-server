// TODO: refactor this mess

#pragma once

#include <string.h>
#include <string>
#include <set>

extern "C"
{
#include "Library/External/Lua/src/lua.h"
#include "Library/External/Lua/src/lauxlib.h"
}

#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CryScriptSystem/IScriptSystem.h"
#include "CryCommon/CryCore/StlUtils.h"

#include "ScriptBindings/ScriptBindings.h"
#include "ScriptTimerManager.h"

struct SLuaStackEntry
{
	int    line;
	string source;
	string description;
};

class CScriptSystem : public IScriptSystem, public ISystemEventListener
{
public:
	//! constructor
	CScriptSystem();
	//! destructor
	~CScriptSystem();
	//!
	bool          Init(ISystem* pSystem, bool bStdLibs, int nStackSize);

	void          Update();
	void          SetGCFrequency(const float fRate);

	void          SetEnvironment(HSCRIPTFUNCTION scriptFunction, IScriptTable* pEnv);
	IScriptTable* GetEnvironment(HSCRIPTFUNCTION scriptFunction);

	//!
	void RegisterErrorHandler(void);

	//!
	bool _ExecuteFile(const char* sFileName, bool bRaiseError);
	//!

	// interface IScriptSystem -----------------------------------------------------------

	virtual bool          ExecuteFile(const char *fileName, bool raiseError, bool forceReload);
	virtual bool          ExecuteBuffer(const char* sBuffer, size_t nSize, const char* sBufferDescription);
	virtual void          UnloadScript(const char* sFileName);
	virtual void          UnloadScripts();
	virtual bool          ReloadScript(const char* sFileName, bool bRaiseError);
	virtual bool          ReloadScripts();
	virtual void          DumpLoadedScripts();

	virtual IScriptTable* CreateTable(bool bEmpty = false);

	//////////////////////////////////////////////////////////////////////////
	// Begin Call.
	//////////////////////////////////////////////////////////////////////////
	virtual int BeginCall(HSCRIPTFUNCTION hFunc);
	virtual int BeginCall(const char* sFuncName);
	virtual int BeginCall(const char* sTableName, const char* sFuncName);
	virtual int BeginCall(IScriptTable* pTable, const char* sFuncName);

	//////////////////////////////////////////////////////////////////////////
	// End Call.
	//////////////////////////////////////////////////////////////////////////
	virtual bool EndCall();
	virtual bool EndCallAny(ScriptAnyValue& any);
	virtual bool EndCallAnyN(int n, ScriptAnyValue* anys);

	//////////////////////////////////////////////////////////////////////////
	// Get function pointer.
	//////////////////////////////////////////////////////////////////////////
	virtual HSCRIPTFUNCTION GetFunctionPtr(const char* sFuncName);
	virtual HSCRIPTFUNCTION GetFunctionPtr(const char* sTableName, const char* sFuncName);
	virtual void            ReleaseFunc(HSCRIPTFUNCTION f);
	virtual HSCRIPTFUNCTION AddFuncRef(HSCRIPTFUNCTION f);
	virtual bool            CompareFuncRef(HSCRIPTFUNCTION f1, HSCRIPTFUNCTION f2);

	virtual ScriptAnyValue  CloneAny(const ScriptAnyValue& any);
	virtual void            ReleaseAny(const ScriptAnyValue& any);

	//////////////////////////////////////////////////////////////////////////
	// Push function param.
	//////////////////////////////////////////////////////////////////////////
	virtual void PushFuncParamAny(const ScriptAnyValue& any);

	//////////////////////////////////////////////////////////////////////////
	// Set global value.
	//////////////////////////////////////////////////////////////////////////
	virtual void          SetGlobalAny(const char* sKey, const ScriptAnyValue& any);
	virtual bool          GetGlobalAny(const char* sKey, ScriptAnyValue& any);

	virtual IScriptTable* CreateUserData(void* ptr, size_t size);
	virtual void          ForceGarbageCollection();
	virtual int           GetCGCount();
	virtual void          SetGCThreshhold(int nKb);
	virtual void          Release();
	virtual void          ShowDebugger(const char* pszSourceFile, int iLine, const char* pszReason);
	virtual HBREAKPOINT   AddBreakPoint(const char* sFile, int nLineNumber);
	virtual IScriptTable* GetLocalVariables(int nLevel);
	virtual IScriptTable* GetCallsStack();// { return 0; };
	virtual void          DumpCallStack();

	virtual void          DebugContinue() {}
	virtual void          DebugStepNext() {}
	virtual void          DebugStepInto() {}
	virtual void          DebugDisable()  {}

	virtual BreakState    GetBreakState() { return bsNoBreak; }
	virtual void          GetMemoryStatistics(ICrySizer* pSizer);
	virtual void          GetScriptHash(const char* sPath, const char* szKey, unsigned int& dwHash);
	virtual void          PostInit();
	virtual void          RaiseError(const char* format, ...) PRINTF_PARAMS(2, 3);
	virtual void          LoadScriptedSurfaceTypes(const char* sFolder, bool bReload);
	virtual void          SerializeTimers(ISerialize* pSer);
	virtual void          ResetTimers();

	virtual int           GetStackSize();
	virtual uint32        GetScriptAllocSize();

	virtual void*         Allocate(size_t sz);
	virtual size_t        Deallocate(void* ptr);

	void                  PushAny(const ScriptAnyValue& var);
	bool                  PopAny(ScriptAnyValue& var);
	// Convert top stack item to Any.
	bool                  ToAny(ScriptAnyValue& var, int index);
	void                  PushVec3(const Vec3& vec);
	bool                  ToVec3(Vec3& vec, int index);
	// Push table reference
	void                  PushTable(IScriptTable* pTable);
	// Attach reference at the top of the stack to the specified table pointer.
	void                  AttachTable(IScriptTable* pTable);
	bool                  GetRecursiveAny(IScriptTable* pTable, const char* sKey, ScriptAnyValue& any);

	lua_State*            GetLuaState() const { return L; }
	void                  TraceScriptError(const char* file, int line, const char* errorStr);
	void                  LogStackTrace();

	ScriptTimerManager *GetScriptTimerManager() { return m_pScriptTimerMgr; };

	void                  GetCallStack(std::vector<SLuaStackEntry>& callstack);
	bool                  IsCallStackEmpty(void);
	void                  DumpStateToFile(const char* filename);

	//////////////////////////////////////////////////////////////////////////
	// Facility to pre-catch any lua buffer
	//////////////////////////////////////////////////////////////////////////
	virtual HSCRIPTFUNCTION CompileBuffer(const char* sBuffer, size_t nSize, const char* sBufferDesc);
	virtual int             PreCacheBuffer(const char* sBuffer, size_t nSize, const char* sBufferDesc);
	virtual int             BeginPreCachedBuffer(int iIndex);
	virtual void            ClearPreCachedBuffer();

	//////////////////////////////////////////////////////////////////////////
	// ISystemEventListener
	//////////////////////////////////////////////////////////////////////////
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam);
	//////////////////////////////////////////////////////////////////////////

	static ScriptVarType LuaTypeToScriptVarType(int type);

	static const char *ScriptAnyTypeToString(ScriptAnyType type);
	static const char *ScriptVarTypeToString(ScriptVarType type);

private: // ---------------------------------------------------------------------
	//!
	bool       EndCallN(int nReturns);

	static int ErrorHandler(lua_State* L);

	// Create default metatables.
	void CreateMetatables();

	// Now private, doesn't need to be updated by main thread
	void        EnableDebugger(ELuaDebugMode eDebugMode);
	void        EnableCodeCoverage(bool enable);

	// Loaded file tracking helpers
	void AddFileToList(const char* sName);

	// ----------------------------------------------------------------------------

	struct ScriptFileNameComparator
	{
		bool operator()(const std::string & scriptA, const std::string & scriptB) const
		{
			return _stricmp(scriptA.c_str(), scriptB.c_str()) < 0;
		}
	};

	lua_State*            L;
	int                   m_nTempArg;
	int                   m_nTempTop;

	IScriptTable*         m_pUserDataMetatable;
	IScriptTable*         m_pPreCacheBufferTable;
	std::vector<string>   m_vecPreCached;

	HSCRIPTFUNCTION       m_pErrorHandlerFunc;

	std::set<std::string, ScriptFileNameComparator> m_loadedScripts;

	ScriptBindings        m_stdScriptBinds;
	ISystem*              m_pSystem;

	float                 m_fGCFreq;      //!< relative time in seconds
	float                 m_lastGCTime;   //!< absolute time in seconds
	int                   m_nLastGCCount; //!<

	ScriptTimerManager*      m_pScriptTimerMgr;
};
