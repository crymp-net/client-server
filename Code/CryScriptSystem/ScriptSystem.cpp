// TODO: refactor this mess
// TODO: add optimized memory allocator

#include <stdio.h>
#include <algorithm>

extern "C"
{
#include "Library/External/Lua/src/lualib.h"
#include "Library/External/Lua/src/lobject.h"
}

#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/ICryPak.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/CrySystem/ICmdLine.h"
#include "CryCommon/CrySystem/ITimer.h"
#include "CryCommon/CryEntitySystem/IEntity.h"
#include "CryCommon/CryAISystem/IAISystem.h"
#include "CryCommon/CryNetwork/ISerialize.h"

#include "ScriptSystem.h"
#include "ScriptTable.h"

// Fine tune this value for optimal performance/memory
#define PER_FRAME_LUA_GC_STEP        2

namespace
{
	CScriptSystem *g_self;

	void LuaDumpState(IConsoleCmdArgs*)
	{
		g_self->DumpStateToFile("LuaState.txt");
	}

	void ReloadScriptCmd(IConsoleCmdArgs* pArgs)
	{
		int nArgs = pArgs->GetArgCount();
		// First arg is the console command itself
		if (nArgs < 2)
		{
			CryLogWarning("No file specified for reload");
		}
		for (int i = 1; i < nArgs; i++)
		{
			const char* sFilename = pArgs->GetArg(i);
			if (g_self->ExecuteFile(sFilename, true, true))
			{
				CryLog("Loaded %s", sFilename);
			}
			// No need to give an error on fail, ExecuteFile already does this
		}
	}

	void LuaGarbargeCollect(IConsoleCmdArgs*)
	{
		g_self->ForceGarbageCollection();
	}
}

//////////////////////////////////////////////////////////////////////
extern "C"
{
	int lua_bitlib_init(lua_State *L);
	//int lua_vectorlib_init(lua_State* L);

	//////////////////////////////////////////////////////////////////////////
	void DumpCallStack(lua_State* L)
	{
		lua_Debug ar;

		memset(&ar, 0, sizeof(lua_Debug));

		//////////////////////////////////////////////////////////////////////////
		// Print callstack.
		//////////////////////////////////////////////////////////////////////////
		int level = 0;
		while (lua_getstack(L, level++, &ar))
		{
			const char* slevel = "";
			if (level == 1)
				slevel = "  ";
			lua_getinfo(L, "lnS", &ar);
			if (ar.name)
				CryLog("$6%s    > %s, (%s: %d)", slevel, ar.name, ar.short_src, ar.currentline);
			else
				CryLog("$6%s    > (null) (%s: %d)", slevel, ar.short_src, ar.currentline);
		}
	}

	void* custom_lua_alloc(void* ud, void* ptr, size_t osize, size_t nsize)
	{
		//MEMSTAT_CONTEXT(EMemStatContextTypes::MSC_LUA, 0, "Lua");

		(void)ud;
		(void)osize;
		if (nsize == 0)
		{
			free(ptr);
			return NULL;
		}
		else
			return realloc(ptr, nsize);
	}
	static int cutsom_lua_panic(lua_State* L)
	{
		CryLogWarning("PANIC: unprotected error during Lua-API call\nLua error string: '%s'", lua_tostring(L, -1));
		DumpCallStack(L);

		// from lua docs:
		//
		// "Lua calls a panic function and then calls exit(EXIT_FAILURE), thus exiting the host application."
		//
		//	so we might as well fatal error here - at least we can inform the user, create a crash dump and fill out a jira bug etc...
		gEnv->pSystem->Error("PANIC: unprotected error during Lua-API call\nLua error string: '%s'", lua_tostring(L, -1));

		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////
// For debugger.
//////////////////////////////////////////////////////////////////////////
IScriptTable* CScriptSystem::GetLocalVariables(int nLevel)
{
	bool bRecursive = false;  // param

	lua_Debug ar;
	const char* name;
	IScriptTable* pObj = CreateTable(true);
	pObj->AddRef();

	// Attach a new table
	const int checkStack = lua_gettop(L);
	lua_newtable(L);
	lua_pushvalue(L, -1);
	AttachTable(pObj);

	// Get the stack frame
	while (lua_getstack(L, nLevel, &ar) != 0)
	{
		// Push a sub-table for this frame (recursive only)
		if (bRecursive)
		{
			assert(lua_istable(L, -1) && "The result table should be on the top of the stack");
			lua_pushinteger(L, nLevel);
			lua_newtable(L);
			lua_pushvalue(L, -1);
			lua_insert(L, -3);
			lua_rawset(L, -4);
			assert(lua_istable(L, -1) && lua_istable(L, -2) && lua_gettop(L) == checkStack + 2 && "There should now be two tables on the top of the stack");
		}

		// Assign variable names and values for the current frame to the table on top of the stack
		int i = 1;
		const int checkInner = lua_gettop(L);
		assert(checkInner == checkStack + 1 + bRecursive && "Too much stack space used");
		assert(lua_istable(L, -1) && "The target table must be on the top of the stack");
		while ((name = lua_getlocal(L, &ar, i++)) != NULL)
		{
			if (strcmp(name, "(*temporary)") == 0)
			{
				// Not interested in temporaries
				lua_pop(L, 1);
			}
			else
			{
				// Push the name, swap the top two items, and set in the table
				lua_pushstring(L, name);
				lua_insert(L, -2);
				assert(lua_gettop(L) == checkInner + 2 && "There should be a key-value pair on top of the stack");
				lua_rawset(L, -3);
			}
			assert(lua_gettop(L) == checkInner && "Unbalanced algorithm problem");
			assert(lua_istable(L, -1) && "The target table should be on the top of the stack");
		}

		// Pop the sub-table (recursive only)
		if (bRecursive)
		{
			assert(lua_istable(L, -1) && lua_istable(L, -2) && "There should now be two tables on the top of the stack");
			lua_pop(L, 1);
		}
		else break;

		nLevel++;
	}

	// Pop the result table from the stack
	lua_pop(L, 1);
	assert(lua_gettop(L) == checkStack && "Unbalanced algorithm problem");
	return pObj;
}

const int LEVELS1 = 12; /* size of the first part of the stack */
const int LEVELS2 = 10; /* size of the second part of the stack */

void CScriptSystem::GetCallStack(std::vector<SLuaStackEntry>& callstack)
{
	callstack.clear();

	int level = 0;
	int firstpart = 1;  /* still before eventual `...' */
	lua_Debug ar;
	while (lua_getstack(L, level++, &ar))
	{

		char buff[512];  /* enough to fit following `sprintf's */
		if (level == 2)
		{
			//luaL_addstring(&b, ("stack traceback:\n"));
		}
		else if (level > LEVELS1 && firstpart)
		{
			/* no more than `LEVELS2' more levels? */
			if (!lua_getstack(L, level + LEVELS2, &ar))
				level--;  /* keep going */
			else
			{
				//		luaL_addstring(&b, ("       ...\n"));  /* too many levels */
				while (lua_getstack(L, level + LEVELS2, &ar))  /* find last levels */
					level++;
			}
			firstpart = 0;
			continue;
		}

		sprintf(buff, ("%4d:  "), level - 1);

		lua_getinfo(L, ("Snl"), &ar);
		switch (*ar.namewhat)
		{
		case 'l':
			sprintf(buff, "function[local] `%.50s'", ar.name);
			break;
		case 'g':
			sprintf(buff, "function[global] `%.50s'", ar.name);
			break;
		case 'f':  /* field */
			sprintf(buff, "field `%.50s'", ar.name);
			break;
		case 'm':  /* field */
			sprintf(buff, "method `%.50s'", ar.name);
			break;
		case 't':  /* tag method */
			sprintf(buff, "`%.50s' tag method", ar.name);
			break;
		default:
			strcpy(buff, "");
			break;
		}

		SLuaStackEntry se;
		se.description = buff;
		se.line = ar.currentline;
		se.source = ar.source;

		callstack.push_back(se);
	}
}

IScriptTable* CScriptSystem::GetCallsStack()
{
	std::vector<SLuaStackEntry> stack;

	IScriptTable* pCallsStack = CreateTable();
	assert(pCallsStack);

	pCallsStack->AddRef();
	GetCallStack(stack);

	for (size_t i = 0; i < stack.size(); ++i)
	{
		SmartScriptTable pEntry(this);

		pEntry->SetValue("description", stack[i].description.c_str());
		pEntry->SetValue("line", stack[i].line);
		pEntry->SetValue("sourcefile", stack[i].source.c_str());

		pCallsStack->PushBack(pEntry);
	}
	return pCallsStack;
}

void CScriptSystem::DumpCallStack()
{
	CryLogAlways("LUA call stack : ============================================================");
	LogStackTrace();
	CryLogAlways("=============================================================================");
}

bool CScriptSystem::IsCallStackEmpty(void)
{
	lua_Debug ar;
	return (lua_getstack(L, 0, &ar) == 0);
}

int listvars(lua_State* L, int level)
{
	char sTemp[1000];
	lua_Debug ar;
	int i = 1;
	const char* name;
	if (lua_getstack(L, level, &ar) == 0)
		return 0; /* failure: no such level in the stack */
	while ((name = lua_getlocal(L, &ar, i)) != NULL)
	{
		sprintf(sTemp, "%s =", name);
		//OutputDebugString(sTemp);

		if (lua_isnumber(L, i))
		{
			int n = (int)lua_tonumber(L, i);
			itoa(n, sTemp, 10);
			//OutputDebugString(sTemp);
		}
		else if (lua_isstring(L, i))
		{
			//OutputDebugString(lua_tostring(L, i));
		}
		else if (lua_isnil(L, i))
		{
			//OutputDebugString("nil");
		}
		else
		{
			//OutputDebugString("<<unknown>>");
		}
		//OutputDebugString("\n");
		i++;
		lua_pop(L, 1);   /* remove variable value */
	}
	/*lua_getglobals(L);
	   i = 1;
	   while ((name = lua_getlocal(L, &ar, i)) != NULL)
	   {
	   sprintf(sTemp, "%s =", name);
	   OutputDebugString(sTemp);


	   if (lua_isnumber(L, i))
	   {
	   int n = (int)lua_tonumber(L, i);
	   itoa(n, sTemp, 10);
	   OutputDebugString(sTemp);
	   }
	   else if (lua_isstring(L, i))
	   {
	   OutputDebugString(lua_tostring(L, i));
	   }else
	   if (lua_isnil(L, i))
	   {
	   OutputDebugString("nil");
	   }
	   else
	   {
	   OutputDebugString("<<unknown>>");
	   }
	   OutputDebugString("\n");
	   i++;
	   lua_pop(L, 1);
	   }*/

	return 1;
}

/*
   static void callhook(lua_State *L, lua_Debug *ar)
   {
   CScriptSystem *pSS = (CScriptSystem*)gEnv->pScriptSystem;
   if(pSS->m_bsBreakState!=bsStepInto)return;
   lua_getinfo(L, "Sl", ar);
   ScriptDebugInfo sdi;
   pSS->m_sLastBreakSource = sdi.sSourceName = ar->source;
   pSS->m_nLastBreakLine =	sdi.nCurrentLine = ar->currentline;

   pSS->ShowDebugger( sdi.sSourceName,sdi.nCurrentLine, "Breakpoint Hit");
   }

   static void linehook(lua_State *L, lua_Debug *ar)
   {
   CScriptSystem *pSS=(CScriptSystem*)gEnv->pScriptSystem;

   if(pSS->m_bsBreakState!=bsNoBreak)
   {
    switch(pSS->m_bsBreakState)
    {
    case bsContinue:
      if(pSS->m_BreakPoint.nLine==ar->currentline)
      {
        lua_getinfo(L, "Sl", ar);
        if(ar->source)
        {
          if(stricmp(ar->source,pSS->m_BreakPoint.sSourceFile.c_str())==0)
            break;
        }
      }
      return;
    case bsStepNext:
    case bsStepInto:
      if(pSS->m_BreakPoint.nLine!=ar->currentline)
      {
        lua_getinfo(L, "Sl", ar);
        if((stricmp(pSS->m_sLastBreakSource.c_str(),ar->source)==0)){
          break;
        }
      }
      return;

      /*lua_getinfo(L, "S", ar);
      if(ar->source)
      {
      if(pSS->m_BreakPoint.nLine!=ar->currentline && (stricmp(pSS->m_sLastBreakSource.c_str(),ar->source)!=0))
      break;
      }
      return;*//*
    default:
      return;
    };
    ScriptDebugInfo sdi;
    pSS->m_sLastBreakSource = sdi.sSourceName = ar->source;
    pSS->m_nLastBreakLine =	sdi.nCurrentLine = ar->currentline;
    pSS->ShowDebugger( sdi.sSourceName,sdi.nCurrentLine, "Breakpoint Hit");
   }

   }
 */

//////////////////////////////////////////////////////////////////////////
const char* FormatPath(char* const sLowerName, const char* sPath)
{
	strcpy(sLowerName, sPath);
	int i = 0;
	while (sLowerName[i] != 0)
	{
		if (sLowerName[i] == '\\')
			sLowerName[i] = '/';
		i++;
	}
	return sLowerName;
}

/* For LuaJIT
   static void l_message (const char *pname, const char *msg) {
   if (pname) fprintf(stderr, "%s: ", pname);
   fprintf(stderr, "%s\n", msg);
   fflush(stderr);
   }

   static const char *progname = "CryScriptSystem";

   static int report (lua_State *L, int status) {
   if (status && !lua_isnil(L, -1)) {
    const char *msg = lua_tostring(L, -1);
    if (msg == NULL) msg = "(error object is not a string)";
    l_message(progname, msg);
    lua_pop(L, 1);
   }
   return status;
   }

   // ---- start of LuaJIT extensions
   static int loadjitmodule (lua_State *L, const char *notfound)
   {
   lua_getglobal(L, "require");
   lua_pushliteral(L, "jit.");
   lua_pushvalue(L, -3);
   lua_concat(L, 2);
   if (lua_pcall(L, 1, 1, 0)) {
    const char *msg = lua_tostring(L, -1);
    if (msg && !strncmp(msg, "module ", 7)) {
      l_message(progname, notfound);
      return 1;
    }
    else
      return report(L, 1);
   }
   lua_getfield(L, -1, "start");
   lua_remove(L, -2);  // drop module table
   return 0;
   }

   // start optimizer
   static int dojitopt (lua_State *L, const char *opt) {
   lua_pushliteral(L, "opt");
   if (loadjitmodule(L, "LuaJIT optimizer module not installed"))
    return 1;
   lua_remove(L, -2);  // drop module name
   if (*opt) lua_pushstring(L, opt);
   return report(L, lua_pcall(L, *opt ? 1 : 0, 0, 0));
   }

   // JIT engine control command: try jit library first or load add-on module
   static int dojitcmd (lua_State *L, const char *cmd) {
   const char *val = strchr(cmd, '=');
   lua_pushlstring(L, cmd, val ? val - cmd : strlen(cmd));
   lua_getglobal(L, "jit");  // get jit.* table
   lua_pushvalue(L, -2);
   lua_gettable(L, -2);  // lookup library function
   if (!lua_isfunction(L, -1)) {
    lua_pop(L, 2);  // drop non-function and jit.* table, keep module name
    if (loadjitmodule(L, "unknown luaJIT command"))
      return 1;
   }
   else {
    lua_remove(L, -2);  // drop jit.* table
   }
   lua_remove(L, -2);  // drop module name
   if (val) lua_pushstring(L, val+1);
   return report(L, lua_pcall(L, val ? 1 : 0, 0, 0));
   }
 */

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CScriptSystem::CScriptSystem()
	: L(nullptr)
	, m_nTempArg(0)
	, m_nTempTop(0)
	, m_pUserDataMetatable(nullptr)
	, m_pPreCacheBufferTable(nullptr)
	, m_pErrorHandlerFunc(nullptr)
	, m_pSystem(nullptr)
	, m_fGCFreq(10.0f)
	, m_lastGCTime(0.0f)
	, m_nLastGCCount(0)
	, m_pScriptTimerMgr(nullptr)
{
	g_self = this;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
CScriptSystem::~CScriptSystem()
{
	m_pSystem->GetISystemEventDispatcher()->RemoveListener(this);

	delete m_pScriptTimerMgr;

	if (L)
	{
		lua_close(L);

		L = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
bool CScriptSystem::Init(ISystem* pSystem, bool bStdLibs, int nStackSize)
{
	assert(gEnv->pConsole);

	m_pSystem = pSystem;
	m_pScriptTimerMgr = new ScriptTimerManager(this);

	m_pSystem->GetISystemEventDispatcher()->RegisterListener(this);

	//L = lua_open();
	L = lua_newstate(custom_lua_alloc, NULL);
	lua_atpanic(L, &cutsom_lua_panic);

	//lua_storedebuginfo(L, 0);

	if (bStdLibs)
	{
		luaL_openlibs(L);
	}

	lua_bitlib_init(L);
	//lua_vectorlib_init(L);

	// For LuaJIT
	//dojitopt(L,"2");
	//dojitcmd(L,"trace");
	//dojitcmd(L,"dumphints=ljit_dumphints.txt");
	//dojitcmd(L,"dump=ljit_dump.txt");

	CreateMetatables();

	m_stdScriptBinds.Init(pSystem, this);

	// Set global time variable into the script.
	SetGlobalValue("_time", 0);
	SetGlobalValue("_frametime", 0);
	SetGlobalValue("_aitick", 0);
	m_nLastGCCount = GetCGCount();

	// Make the error handler available to LUA
	RegisterErrorHandler();

	IConsole *pConsole = gEnv->pConsole;

	pConsole->AddCommand("lua_dump_state", LuaDumpState, 0, "Dumps the current state of the lua memory (defined symbols and values) into the file LuaState.txt");
	pConsole->AddCommand("lua_garbagecollect", LuaGarbargeCollect, 0, "Forces a garbage collection of the lua state");

	pConsole->RegisterInt("lua_debugger", 0, VF_CHEAT, "Enables the script debugger.\n1 to trigger on breakpoints and errors\n2 to only trigger on errors\nUsage: lua_debugger [0/1/2]\n");
	pConsole->RegisterInt("lua_StopOnError", 0, VF_CHEAT, "Stops on error");
	pConsole->RegisterInt("lua_stackonmalloc", 0, 0, "Enables/disables logging of the called lua functions and respective callstacks, whenever a new lua object is instantiated.");

	// Register the command for reloading script files
	pConsole->AddCommand("lua_reload_script", ReloadScriptCmd, VF_CHEAT, "Reloads given script files and their dependencies");

	//DumpStateToFile( "LuaState.txt" );

	//////////////////////////////////////////////////////////////////////////
	// Execute common lua file.
	//////////////////////////////////////////////////////////////////////////
	ExecuteFile("scripts/common.lua", true, false);

	//initvectortag(L);
	return L ? true : false;
}

void CScriptSystem::PostInit()
{
}

//////////////////////////////////////////////////////////////////////////
void CScriptSystem::EnableDebugger(ELuaDebugMode eDebugMode)
{
}

void CScriptSystem::EnableCodeCoverage(bool enable)
{
}

//////////////////////////////////////////////////////////////////////////
void CScriptSystem::TraceScriptError(const char* file, int line, const char* errorStr)
{
	//lua_Debug ar;

	LogStackTrace();
}

//////////////////////////////////////////////////////////////////////////
void CScriptSystem::LogStackTrace()
{
	::DumpCallStack(L);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int CScriptSystem::ErrorHandler(lua_State* L)
{
	//if (!lua_isstoredebuginfo(L))
	//	return 0; // ignore script errors if engine is running without game

	// Handle error
	lua_Debug ar;

	memset(&ar, 0, sizeof(lua_Debug));

	const char* sErr = lua_tostring(L, 1);

	if (sErr)
	{
		CryLogWarning("[Lua Error] %s", sErr);
	}

	//////////////////////////////////////////////////////////////////////////
	// Print error callstack.
	//////////////////////////////////////////////////////////////////////////
	int level = 1;
	while (lua_getstack(L, level++, &ar))
	{
		lua_getinfo(L, "lnS", &ar);
		if (ar.name)
			CryLog("$6    > %s, (%s: %d)", ar.name, ar.short_src, ar.currentline);
		else
			CryLog("$6    > (null) (%s: %d)", ar.short_src, ar.currentline);
	}

	if (sErr)
	{
		ICVar* lua_StopOnError = gEnv->pConsole->GetCVar("lua_StopOnError");
		if (lua_StopOnError && lua_StopOnError->GetIVal() != 0)
		{
			CryLogWarning("![Lua Error] %s", sErr);
		}
	}

	g_self->TraceScriptError(ar.source, ar.currentline, sErr);

	return 0;
}

void CScriptSystem::RegisterErrorHandler()
{
	// Legacy approach
	/*
	   if(bDebugger)
	   {
	   //lua_register(L, LUA_ERRORMESSAGE, CScriptSystem::ErrorHandler );
	   //lua_setglobal(L, LUA_ERRORMESSAGE);
	   }
	   else
	   {
	   //lua_register(L, LUA_ERRORMESSAGE, CScriptSystem::ErrorHandler );

	   //lua_newuserdatabox(L, this);
	   //lua_pushcclosure(L, CScriptSystem::ErrorHandler, 1);
	   //lua_setglobal(L, LUA_ALERT);
	   //lua_pushcclosure(L, errorfb, 0);
	   //lua_setglobal(L, LUA_ERRORMESSAGE);
	   }
	 */

	// Register global error handler.
	// This just makes it available - when we call LUA, we insert it so it will be called
	if (!m_pErrorHandlerFunc)
	{
		lua_pushcfunction(L, CScriptSystem::ErrorHandler);
		m_pErrorHandlerFunc = (HSCRIPTFUNCTION)(INT_PTR)lua_ref(L, 1);
	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void CScriptSystem::CreateMetatables()
{
	m_pUserDataMetatable = CreateTable();
	m_pUserDataMetatable->AddRef();

	m_pPreCacheBufferTable = CreateTable();
	m_pPreCacheBufferTable->AddRef();

	/*
	   m_pUserDataMetatable->AddFunction( )

	   // Make Garbage collection for user data metatable.
	   lua_newuserdatabox(L, this);
	   lua_pushcclosure(L, CScriptSystem::GCTagHandler, 1);
	   lua_settagmethod(L, m_nGCTag, "gc");
	 */
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
IScriptTable* CScriptSystem::CreateTable(bool bEmpty)
{
	return ScriptTable::Create(this, L, bEmpty);
}

bool CScriptSystem::_ExecuteFile(const char* sFileName, bool bRaiseError)
{
	CCryFile file;

	if (!file.Open(sFileName, "rb"))
	{
		if (bRaiseError)
			CryLogWarningAlways("[Lua Error] Failed to load script file %s", sFileName);

		return false;
	}

	uint32 fileSize = file.GetLength();
	if (!fileSize)
		return false;

	std::vector<char> buffer(fileSize);

	if (file.ReadRaw(&buffer.front(), fileSize) == 0)
		return false;

	// Translate pak alias filenames
	char translatedBuf[_MAX_PATH + 1];
	const char* translated = gEnv->pCryPak->AdjustFileName(sFileName, translatedBuf, ICryPak::FLAGS_NO_FULL_PATH);

	std::string fileName;
	fileName += '@';
	fileName += translated;
	std::replace(fileName.begin(), fileName.end(), '\\', '/');

	//CRY_DEFINE_ASSET_SCOPE("LUA", sFileName);

	return ExecuteBuffer(&buffer.front(), fileSize, fileName.c_str());
}

bool CScriptSystem::ExecuteFile(const char *fileName, bool raiseError, bool forceReload)
{
	if (!fileName)
		return false;

	std::string file = fileName;

	// convert backslashes to normal slashes
	std::replace(file.begin(), file.end(), '\\', '/');

	if (file.empty())
		return false;

	// TODO: use C++17 structured binding
	const auto result = m_loadedScripts.emplace(std::move(file));

	if (result.second || forceReload)
	{
		if (!_ExecuteFile(result.first->c_str(), raiseError))
		{
			m_loadedScripts.erase(result.first);
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void CScriptSystem::UnloadScript(const char* sFileName)
{
	if (strlen(sFileName) <= 0)
		return;

	char lowerName[_MAX_PATH];
	const char* sTemp = FormatPath(lowerName, sFileName);
	//auto itor = std::find(m_loadedScripts.begin(), m_loadedScripts.end(), sTemp.c_str());
	auto itor = m_loadedScripts.find(sTemp);
	if (itor != m_loadedScripts.end())
	{
		itor = m_loadedScripts.erase(itor);
	}
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void CScriptSystem::UnloadScripts()
{
	m_loadedScripts.clear();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
bool CScriptSystem::ReloadScripts()
{
	auto itor = m_loadedScripts.begin();
	while (itor != m_loadedScripts.end())
	{
		ReloadScript(itor->c_str(), true);
		++itor;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
bool CScriptSystem::ReloadScript(const char* sFileName, bool bRaiseError)
{
	return ExecuteFile(sFileName, bRaiseError, gEnv->pSystem->IsDevMode());
}

void CScriptSystem::DumpLoadedScripts()
{
	auto itor = m_loadedScripts.begin();
	while (itor != m_loadedScripts.end())
	{
		CryLogAlways("%s", itor->c_str());
		++itor;
	}
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void CScriptSystem::AddFileToList(const char* sName)
{
	m_loadedScripts.insert(sName);
}

/*
   void CScriptSystem::GetScriptHashFunction( IScriptTable &Current, unsigned int &dwHash)
   {
   unsigned int *pCode=0;
   int iSize=0;

   if(pCurrent.GetCurrentFuncData(pCode,iSize))						// function ?
   {
   if(pCode)                                            // lua function ?
   GetScriptHashFunction(pCode,iSize,dwHash);
   }
   }
 */

void CScriptSystem::GetScriptHash(const char* sPath, const char* szKey, unsigned int& dwHash)
{
	//	IScriptTable *pCurrent;

	//	GetGlobalValue(szKey,pCurrent);

	//	if(!pCurrent)		// is not a table
	//	{
	//	}
	/*
	   else if(lua_isfunction(L, -1))
	   {
	   GetScriptHashFunction(*pCurrent,dwHash);

	   return;
	   }
	   else
	   {
	   lua_pop(L, 1);
	   return;
	   }
	 */
	/*
	   pCurrent->BeginIteration();

	   while(pCurrent->MoveNext())
	   {
	   char *szKeyName;

	   if(!pCurrent->GetCurrentKey(szKeyName))
	   szKeyName="NO";

	   ScriptVarType type=pCurrent->GetCurrentType();

	   if(type==svtObject)		// svtNull,svtFunction,svtString,svtNumber,svtUserData,svtObject
	   {
	   void *pVis;

	   pCurrent->GetCurrentPtr(pVis);

	   gEnv->pLog->Log("  table '%s/%s'",sPath.c_str(),szKeyName);

	   if(setVisited.count(pVis)!=0)
	   {
	   gEnv->pLog->Log("    .. already processed ..");
	   continue;
	   }

	   setVisited.insert(pVis);

	   {
	   IScriptTable *pNewObject = m_pScriptSystem->CreateEmptyObject();

	   pCurrent->GetCurrent(pNewObject);

	   Debug_Full_recursive(pNewObject,sPath+string("/")+szKeyName,setVisited);

	   pNewObject->Release();
	   }
	   }
	   else if(type==svtFunction)		// svtNull,svtFunction,svtString,svtNumber,svtUserData,svtObject
	   {
	   GetScriptHashFunction(*pCurrent,dwHash);
	   }
	   }

	   pCurrent->EndIteration();
	 */
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
bool CScriptSystem::ExecuteBuffer(const char* sBuffer, size_t nSize, const char* sBufferDescription)
{
	IScriptTable *pEnv = nullptr;  // param

	int status;

	//MEMSTAT_CONTEXT(EMemStatContextTypes::MSC_Other, 0, "Lua LoadScript");
	//MEMSTAT_CONTEXT_FMT(EMemStatContextTypes::MSC_ScriptCall, 0, "%s", sBufferDescription);

	{
		status = luaL_loadbuffer(L, sBuffer, nSize, sBufferDescription);
	}

	if (status == 0)
	{
		// parse OK?
		int base = lua_gettop(L);  // function index.
		lua_getref(L, (int)(INT_PTR)m_pErrorHandlerFunc);
		lua_insert(L, base);  // put it under chunk and args

		if (pEnv)
		{
			PushTable(pEnv);
			lua_setfenv(L, -2);
		}

		status = lua_pcall(L, 0, LUA_MULTRET, base); // call main
		lua_remove(L, base);                         // remove error handler function.
	}
	if (status != 0)
	{
		const char* sErr = lua_tostring(L, -1);
		if (sBufferDescription && strlen(sBufferDescription) != 0)
			GetISystem()->Warning(VALIDATOR_MODULE_SCRIPTSYSTEM, VALIDATOR_WARNING, VALIDATOR_FLAG_FILE | VALIDATOR_FLAG_SCRIPT, sBufferDescription,
			                      "[Lua Error] Failed to execute file %s: %s", sBufferDescription, sErr);
		else
			CryLogWarning("[Lua Error] Error executing lua %s", sErr);
		assert(GetStackSize() > 0);
		lua_pop(L, 1);
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void CScriptSystem::Release()
{
	delete this;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int CScriptSystem::BeginCall(HSCRIPTFUNCTION hFunc)
{
	assert(hFunc != 0);
	if (!hFunc)
		return 0;

	lua_getref(L, (int)(INT_PTR)hFunc);
	if (!lua_isfunction(L, -1))
	{
		CryLogWarning("[CScriptSystem::BeginCall] Function Ptr:%d not found", hFunc);
		m_nTempArg = -1;
		lua_pop(L, 1);
		return 0;
	}
	m_nTempArg = 0;

	return 1;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int CScriptSystem::BeginCall(const char* sTableName, const char* sFuncName)
{
	lua_getglobal(L, sTableName);

	if (!lua_istable(L, -1))
	{
		CryLogWarning("[CScriptSystem::BeginCall] Tried to call %s:%s(), Table %s not found (check for syntax errors or if the file wasn't loaded)", sTableName, sFuncName, sTableName);
		m_nTempArg = -1;
		lua_pop(L, 1);
		return 0;
	}

	lua_pushstring(L, sFuncName);
	lua_gettable(L, -2);
	lua_remove(L, -2);  // Remove table global.
	m_nTempArg = 0;

	if (!lua_isfunction(L, -1))
	{
		CryLogWarning("[CScriptSystem::BeginCall] Function %s:%s not found(check for syntax errors or if the file wasn't loaded)", sTableName, sFuncName);
		m_nTempArg = -1;
		lua_pop(L, 1);
		return 0;
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////
int CScriptSystem::BeginCall(IScriptTable* pTable, const char* sFuncName)
{
	PushTable(pTable);

	lua_pushstring(L, sFuncName);
	lua_gettable(L, -2);
	lua_remove(L, -2);  // Remove table global.
	m_nTempArg = 0;

	if (!lua_isfunction(L, -1))
	{
		CryLogWarning("[CScriptSystem::BeginCall] Function %s not found in the table", sFuncName);
		m_nTempArg = -1;
		lua_pop(L, 1);
		return 0;
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
int CScriptSystem::BeginCall(const char* sFuncName)
{
	if (L)
	{
		lua_getglobal(L, sFuncName);
		m_nTempArg = 0;

		if (!lua_isfunction(L, -1))
		{
			CryLogWarning("[CScriptSystem::BeginCall] Function %s not found(check for syntax errors or if the file wasn't loaded)", sFuncName);
			m_nTempArg = -1;
			lua_pop(L, 1);
			return 0;
		}

		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
HSCRIPTFUNCTION CScriptSystem::GetFunctionPtr(const char* sFuncName)
{
	HSCRIPTFUNCTION func;
	lua_getglobal(L, sFuncName);
	if (lua_isnil(L, -1) || (!lua_isfunction(L, -1)))
	{
		lua_pop(L, 1);
		return NULL;
	}
	func = (HSCRIPTFUNCTION)(INT_PTR)lua_ref(L, 1);

	return func;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
HSCRIPTFUNCTION CScriptSystem::GetFunctionPtr(const char* sTableName, const char* sFuncName)
{
	HSCRIPTFUNCTION func;
	lua_getglobal(L, sTableName);
	if (!lua_istable(L, -1))
	{
		lua_pop(L, 1);
		return 0;
	}
	lua_pushstring(L, sFuncName);
	lua_gettable(L, -2);
	lua_remove(L, -2);  // Remove table global.
	if (lua_isnil(L, -1) || (!lua_isfunction(L, -1)))
	{
		lua_pop(L, 1);
		return 0;
	}
	func = (HSCRIPTFUNCTION)(INT_PTR)lua_ref(L, 1);
	return func;
}

//////////////////////////////////////////////////////////////////////////
void CScriptSystem::PushAny(const ScriptAnyValue& var)
{
	switch (var.type)
	{
	case ANY_ANY:
	case ANY_TNIL:
		lua_pushnil(L);
		break;
		;
	case ANY_TBOOLEAN:
		lua_pushboolean(L, var.b);
		break;
		;
	case ANY_THANDLE:
		lua_pushlightuserdata(L, const_cast<void*>(var.ptr));
		break;
	case ANY_TNUMBER:
		lua_pushnumber(L, var.number);
		break;
	case ANY_TSTRING:
		lua_pushstring(L, var.str);
		break;
	case ANY_TTABLE:
		if (var.table)
			PushTable(var.table);
		else
			lua_pushnil(L);
		break;
	case ANY_TFUNCTION:
		lua_getref(L, (int)(INT_PTR)var.function);
		assert(lua_type(L, -1) == LUA_TFUNCTION);
		break;
	case ANY_TUSERDATA:
		lua_getref(L, var.ud.nRef);
		break;
	case ANY_TVECTOR:
		PushVec3(Vec3(var.vec3.x, var.vec3.y, var.vec3.z));
		break;
	default:
		// Must handle everything.
		assert(0);
	}
}

//////////////////////////////////////////////////////////////////////////
bool CScriptSystem::ToAny(ScriptAnyValue& var, int index)
{
	if (!lua_gettop(L))
		return false;

	if (var.type == ANY_ANY)
	{
		switch (lua_type(L, index))
		{
		case LUA_TNIL:
			var.type = ANY_TNIL;
			break;
		case LUA_TBOOLEAN:
			var.b = lua_toboolean(L, index) != 0;
			var.type = ANY_TBOOLEAN;
			break;
		case LUA_TLIGHTUSERDATA:
			var.ptr = lua_topointer(L, index);
			var.type = ANY_THANDLE;
			break;
		case LUA_TNUMBER:
			var.number = (float)lua_tonumber(L, index);
			var.type = ANY_TNUMBER;
			break;
		case LUA_TSTRING:
			var.str = lua_tostring(L, index);
			var.type = ANY_TSTRING;
			break;
		case LUA_TTABLE:
		case LUA_TUSERDATA:
			if (!var.table)
			{
				var.table = ScriptTable::Create(this, L, true);
				var.table->AddRef();
			}
			lua_pushvalue(L, index);
			AttachTable(var.table);
			var.type = ANY_TTABLE;
			break;
		case LUA_TFUNCTION:
			{
				// Make reference to function.
				lua_pushvalue(L, index);
				var.function = (HSCRIPTFUNCTION)(INT_PTR)lua_ref(L, 1);
			}
			break;
		case LUA_TTHREAD:
		default:
			return false;
		}
		return true;
	}
	else
	{
		bool res = false;
		switch (lua_type(L, index))
		{
		case LUA_TNIL:
			if (var.type == ANY_TNIL)
				res = true;
			break;
		case LUA_TBOOLEAN:
			if (var.type == ANY_TBOOLEAN)
			{
				var.b = lua_toboolean(L, index) != 0;
				res = true;
			}
			break;
		case LUA_TLIGHTUSERDATA:
			if (var.type == ANY_THANDLE)
			{
				var.ptr = lua_topointer(L, index);
				res = true;
			}
			break;
		case LUA_TNUMBER:
			if (var.type == ANY_TNUMBER)
			{
				var.number = (float)lua_tonumber(L, index);
				res = true;
			}
			else if (var.type == ANY_TBOOLEAN)
			{
				var.b = lua_tonumber(L, index) != 0;
				res = true;
			}
			break;
		case LUA_TSTRING:
			if (var.type == ANY_TSTRING)
			{
				var.str = lua_tostring(L, index);
				res = true;
			}
			break;
		case LUA_TTABLE:
			if (var.type == ANY_TTABLE)
			{
				if (!var.table)
				{
					var.table = ScriptTable::Create(this, L, true);
					var.table->AddRef();
				}
				lua_pushvalue(L, index);
				AttachTable(var.table);
				res = true;
			}
			else if (var.type == ANY_TVECTOR)
			{
				Vec3 v(0, 0, 0);
				if (res = ToVec3(v, index))
				{
					var.vec3.x = v.x;
					var.vec3.y = v.y;
					var.vec3.z = v.z;
				}
			}
			break;
		case LUA_TUSERDATA:
			if (var.type == ANY_TTABLE)
			{
				if (!var.table)
				{
					var.table = ScriptTable::Create(this, L, true);
					var.table->AddRef();
				}
				lua_pushvalue(L, index);
				AttachTable(var.table);
				res = true;
			}
			break;
		case LUA_TFUNCTION:
			if (var.type == ANY_TFUNCTION)
			{
				// Make reference to function.
				lua_pushvalue(L, index);
				var.function = (HSCRIPTFUNCTION)(INT_PTR)lua_ref(L, 1);
				res = true;
			}
			break;
		case LUA_TTHREAD:
			break;
		}
		return res;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool CScriptSystem::PopAny(ScriptAnyValue& var)
{
	bool res = ToAny(var, -1);
	lua_pop(L, 1);
	return res;
}

/*
   #include <signal.h>
   static void cry_lstop (lua_State *l, lua_Debug *ar) {
   (void)ar;  // unused arg.
   lua_sethook(l, NULL, 0, 0);
   luaL_error(l, "interrupted!");
   }
   static void cry_laction (int i)
   {
   CScriptSystem *pThis = (CScriptSystem *)gEnv->pScriptSystem;
   lua_State *L = pThis->GetLuaState();
   signal(i, SIG_DFL); // if another SIGINT happens before lstop, terminate process (default action)
   lua_sethook(L, cry_lstop, LUA_MASKCALL | LUA_MASKRET | LUA_MASKCOUNT, 1);
   }
 */

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
bool CScriptSystem::EndCallN(int nReturns)
{
	if (m_nTempArg < 0 || !L)
		return false;

	int base = lua_gettop(L) - m_nTempArg;  // function index.
	lua_getref(L, (int)(INT_PTR)m_pErrorHandlerFunc);
	lua_insert(L, base);  // put it under chunk and args

	//signal(SIGINT, cry_laction);
	int status = lua_pcall(L, m_nTempArg, nReturns, base);
	//signal(SIGINT, SIG_DFL);
	lua_remove(L, base);  // remove error handler function.

	return status == 0;
}

//////////////////////////////////////////////////////////////////////////
bool CScriptSystem::EndCall()
{
	return EndCallN(0);
}

//////////////////////////////////////////////////////////////////////////
bool CScriptSystem::EndCallAny(ScriptAnyValue& any)
{
	if (!EndCallN(1))
		return false;
	return PopAny(any);
}

//////////////////////////////////////////////////////////////////////
bool CScriptSystem::EndCallAnyN(int n, ScriptAnyValue* anys)
{
	if (!EndCallN(n))
		return false;
	for (int i = 0; i < n; i++)
	{
		if (!PopAny(anys[i]))
			return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
void CScriptSystem::PushFuncParamAny(const ScriptAnyValue& any)
{
	if (m_nTempArg == -1)
		return;
	PushAny(any);
	m_nTempArg++;
}

//////////////////////////////////////////////////////////////////////////
void CScriptSystem::SetGlobalAny(const char* sKey, const ScriptAnyValue& any)
{
	PushAny(any);
	lua_setglobal(L, sKey);
}

//////////////////////////////////////////////////////////////////////////
bool CScriptSystem::GetRecursiveAny(IScriptTable* pTable, const char* sKey, ScriptAnyValue& any)
{
	char key1[256];
	char key2[256];

	const char* const sep = strchr(sKey, '.');
	if (sep)
	{
		strncpy(key1, sKey, (size_t)(sep - sKey));
		strcpy(key2, sep + 1);
	}
	else
	{
		strcpy(key1, sKey);
		key2[0] = 0;
	}

	ScriptAnyValue localAny;
	if (!pTable->GetValueAny(key1, localAny))
		return false;

	if (localAny.type == ANY_TFUNCTION && NULL == sep)
	{
		any = localAny;
		return true;
	}
	else if (localAny.type == ANY_TTABLE && NULL != sep)
	{
		return GetRecursiveAny(localAny.table, key2, any);
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool CScriptSystem::GetGlobalAny(const char* sKey, ScriptAnyValue& any)
{
	const char* sep = strchr(sKey, '.');
	if (sep)
	{
		ScriptAnyValue globalAny;
		char key1[256];
		strcpy(key1, sKey);
		key1[sep - sKey] = 0;
		GetGlobalAny(key1, globalAny);
		if (globalAny.type == ANY_TTABLE)
		{
			return GetRecursiveAny(globalAny.table, sep + 1, any);
		}
		return false;
	}

	lua_getglobal(L, sKey);
	if (!PopAny(any))
	{
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void CScriptSystem::ForceGarbageCollection()
{
	int beforeUsage = lua_gc(L, LUA_GCCOUNT, 0) * 1024 + lua_gc(L, LUA_GCCOUNTB, 0);

	// Do a full garbage collection cycle.
	lua_gc(L, LUA_GCCOLLECT, 0);

	int fracUsage = lua_gc(L, LUA_GCCOUNTB, 0);
	int totalUsage = lua_gc(L, LUA_GCCOUNT, 0) * 1024 + fracUsage;

	CryLog("Lua garbage collection %i -> %i", beforeUsage, totalUsage);

	/*char sTemp[200];
	   lua_StateStats lss;
	   lua_getstatestats(L,&lss);
	   sprintf(sTemp,"protos=%d closures=%d tables=%d udata=%d strings=%d\n",lss.nProto,lss.nClosure,lss.nHash,lss.nUdata,lss.nString);
	   OutputDebugString("BEFORE GC STATS :");
	   OutputDebugString(sTemp);*/

	//lua_setgcthreshold(L, 0);

	/*lua_getstatestats(L,&lss);
	   sprintf(sTemp,"protos=%d closures=%d tables=%d udata=%d strings=%d\n",lss.nProto,lss.nClosure,lss.nHash,lss.nUdata,lss.nString);
	   OutputDebugString("AFTER GC STATS :");
	   OutputDebugString(sTemp);*/
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int CScriptSystem::GetCGCount()
{
	return lua_getgccount(L);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void CScriptSystem::SetGCThreshhold(int nKb)
{
	//lua_setgcthreshold(L, nKb);
}

IScriptTable* CScriptSystem::CreateUserData(void* ptr, size_t size)   //AMD Port
{
	void* nptr = lua_newuserdata(L, size);
	memcpy(nptr, ptr, size);
	ScriptTable* pNewTbl = ScriptTable::Create(this, L, true);
	pNewTbl->Attach();

	return pNewTbl;
}

//////////////////////////////////////////////////////////////////////
HBREAKPOINT CScriptSystem::AddBreakPoint(const char* sFile, int nLineNumber)
{
	return 0;
}

HSCRIPTFUNCTION CScriptSystem::AddFuncRef(HSCRIPTFUNCTION f)
{
	if (f)
	{
		int ret;
		lua_getref(L, (int)(INT_PTR)f);
		assert(lua_type(L, -1) == LUA_TFUNCTION);
		ret = lua_ref(L, 1);
		if (ret != LUA_REFNIL)
		{
			return (HSCRIPTFUNCTION)(size_t)ret;
		}
		assert(0);
	}
	return 0;
}

bool CScriptSystem::CompareFuncRef(HSCRIPTFUNCTION f1, HSCRIPTFUNCTION f2)
{
	if (f1 == f2)
		return true;

	lua_getref(L, (int)(INT_PTR)f1);
	assert(lua_type(L, -1) == LUA_TFUNCTION);
	const void* f1p = lua_topointer(L, -1);
	lua_pop(L, 1);
	lua_getref(L, (int)(INT_PTR)f2);
	assert(lua_type(L, -1) == LUA_TFUNCTION);
	const void* f2p = lua_topointer(L, -1);
	lua_pop(L, 1);
	if (f1p == f2p)
		return true;
	return false;
}

void CScriptSystem::ReleaseFunc(HSCRIPTFUNCTION f)
{
	if (f)
	{
#ifdef _DEBUG
		lua_getref(L, (int)(INT_PTR)f);
		assert(lua_type(L, -1) == LUA_TFUNCTION);
		lua_pop(L, 1);
#endif
		lua_unref(L, (int)(INT_PTR)f);
	}
}

ScriptAnyValue CScriptSystem::CloneAny(const ScriptAnyValue& any)
{
	ScriptAnyValue result(any.type);

	switch (any.type)
	{
	case ANY_ANY:
	case ANY_TNIL:
	case ANY_TBOOLEAN:
		result.b = any.b;
		break;
	case ANY_TNUMBER:
		result.number = any.number;
		break;
	case ANY_THANDLE:
		result.ptr = any.ptr;
		break;
	case ANY_TSTRING:
		result.str = any.str;
		break;
	case ANY_TTABLE:
		if (any.table)
		{
			result.table = any.table;
			result.table->Release();
		}
		break;
	case ANY_TFUNCTION:
		lua_getref(L, (int)(INT_PTR)any.function);
		assert(lua_type(L, -1) == LUA_TFUNCTION);
		result.function = (HSCRIPTFUNCTION)(size_t)lua_ref(L, 1);
		break;
	case ANY_TUSERDATA:
		result.ud.ptr = any.ud.ptr;
		lua_getref(L, any.ud.nRef);
		result.ud.nRef = lua_ref(L, 1);
		break;
	case ANY_TVECTOR:
		result.vec3 = any.vec3;
		break;
	default:
		assert(0);
	}

	return result;
}

void CScriptSystem::ReleaseAny(const ScriptAnyValue& any)
{
	switch (any.type)
	{
	case ANY_ANY:
	case ANY_TNIL:
	case ANY_TBOOLEAN:
	case ANY_TNUMBER:
	case ANY_THANDLE:
	case ANY_TSTRING:
	case ANY_TVECTOR:
		break;
	case ANY_TTABLE:
	case ANY_TFUNCTION:
		// will be freed on delete
		break;
	case ANY_TUSERDATA:
		lua_unref(L, any.ud.nRef);
		break;
	default:
		assert(0);
	}
}

//////////////////////////////////////////////////////////////////////////
void CScriptSystem::PushTable(IScriptTable* pTable)
{
	static_cast<ScriptTable*>(pTable)->PushRef();
};

//////////////////////////////////////////////////////////////////////////
void CScriptSystem::AttachTable(IScriptTable* pTable)
{
	static_cast<ScriptTable*>(pTable)->Attach();
}

//////////////////////////////////////////////////////////////////////////
void CScriptSystem::PushVec3(const Vec3& vec)
{
	lua_newtable(L);
	lua_pushlstring(L, "x", 1);
	lua_pushnumber(L, vec.x);
	lua_settable(L, -3);
	lua_pushlstring(L, "y", 1);
	lua_pushnumber(L, vec.y);
	lua_settable(L, -3);
	lua_pushlstring(L, "z", 1);
	lua_pushnumber(L, vec.z);
	lua_settable(L, -3);
}

//////////////////////////////////////////////////////////////////////////
bool CScriptSystem::ToVec3(Vec3& vec, int tableIndex)
{
	if (tableIndex < 0)
	{
		tableIndex = lua_gettop(L) + tableIndex + 1;
	}

	if (lua_type(L, tableIndex) != LUA_TTABLE)
	{
		return false;
	}

	//int num = luaL_getn(L,index);
	//if (num != 3)
	//return false;

	float x, y, z;
	lua_pushlstring(L, "x", 1);
	lua_gettable(L, tableIndex);
	if (!lua_isnumber(L, -1))
	{
		lua_pop(L, 1); // pop x value.
		//////////////////////////////////////////////////////////////////////////
		// Try an indexed table.
		lua_pushnumber(L, 1);
		lua_gettable(L, tableIndex);
		if (!lua_isnumber(L, -1))
		{
			lua_pop(L, 1); // pop value.
			return false;
		}
		x = lua_tonumber(L, -1);
		lua_pushnumber(L, 2);
		lua_gettable(L, tableIndex);
		if (!lua_isnumber(L, -1))
		{
			lua_pop(L, 2); // pop value.
			return false;
		}
		y = lua_tonumber(L, -1);
		lua_pushnumber(L, 3);
		lua_gettable(L, tableIndex);
		if (!lua_isnumber(L, -1))
		{
			lua_pop(L, 3); // pop value.
			return false;
		}
		z = lua_tonumber(L, -1);
		lua_pop(L, 3); // pop value.

		vec.x = x;
		vec.y = y;
		vec.z = z;
		return true;
		//////////////////////////////////////////////////////////////////////////
	}
	x = lua_tonumber(L, -1);
	lua_pop(L, 1); // pop value.

	lua_pushlstring(L, "y", 1);
	lua_gettable(L, tableIndex);
	if (!lua_isnumber(L, -1))
	{
		lua_pop(L, 1); // pop table.
		return false;
	}
	y = lua_tonumber(L, -1);
	lua_pop(L, 1); // pop value.

	lua_pushlstring(L, "z", 1);
	lua_gettable(L, tableIndex);
	if (!lua_isnumber(L, -1))
	{
		lua_pop(L, 1); // pop table.
		return false;
	}
	z = lua_tonumber(L, -1);
	lua_pop(L, 1); // pop value.

	vec.x = x;
	vec.y = y;
	vec.z = z;

	return true;
}

//////////////////////////////////////////////////////////////////////////
void CScriptSystem::ShowDebugger(const char* pszSourceFile, int iLine, const char* pszReason)
{
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CScriptSystem::Update()
{
	FUNCTION_PROFILER(m_pSystem, PROFILE_SCRIPT);

	//CryGetScr
	//L->l_G->totalbytes =
	ITimer* pTimer = gEnv->pTimer;

	// Might need to check for new lua code needing hooks

	float currTime = pTimer->GetCurrTime();
	float frameTime = pTimer->GetFrameTime();

	IScriptSystem* pScriptSystem = m_pSystem->GetIScriptSystem();

	// Set global time variable into the script.
	pScriptSystem->SetGlobalValue("_time", currTime);
	pScriptSystem->SetGlobalValue("_frametime", frameTime);

	{
		int aiTicks = 0;

		IAISystem* pAISystem = gEnv->pAISystem;

		if (pAISystem)
			aiTicks = pAISystem->GetAITickCount();
		pScriptSystem->SetGlobalValue("_aitick", aiTicks);
	}

	//TRACE("GC DELTA %d",m_pScriptSystem->GetCGCount()-nStartGC);
	//int nStartGC = pScriptSystem->GetCGCount();

	bool bKickIn = false;                             // Invoke Gargabe Collector

	if (currTime - m_lastGCTime > m_fGCFreq)  // g_GC_Frequence->GetIVal())
		bKickIn = true;

	int nGCCount = pScriptSystem->GetCGCount();

	bool bNoLuaGC = false;

	if (nGCCount - m_nLastGCCount > 2000 && !bNoLuaGC)   //
		bKickIn = true;

	//if(bKickIn)
	{
		FRAME_PROFILER("Lua GC", m_pSystem, PROFILE_SCRIPT);

		//CryLog( "Lua GC=%d",GetCGCount() );

		//float fTimeBefore=pTimer->GetAsyncCurTime()*1000;

		// Do a full garbage collection cycle.
		//pScriptSystem->ForceGarbageCollection();

		// Do incremental Garbage Collection
		lua_gc(L, LUA_GCSTEP, (PER_FRAME_LUA_GC_STEP));

		m_nLastGCCount = pScriptSystem->GetCGCount();
		m_lastGCTime = currTime;
		//float fTimeAfter=pTimer->GetAsyncCurTime()*1000;
		//CryLog("--[after coll]GC DELTA %d ",pScriptSystem->GetCGCount()-nGCCount);
		//TRACE("--[after coll]GC DELTA %d [time =%f]",m_pScriptSystem->GetCGCount()-nStartGC,fTimeAfter-fTimeBefore);
	}

	m_pScriptTimerMgr->Update();
}

//////////////////////////////////////////////////////////////////////////
void CScriptSystem::SetGCFrequency(const float fRate)
{
	if (fRate >= 0)
	{
		m_fGCFreq = fRate;
	}
}

void CScriptSystem::SetEnvironment(HSCRIPTFUNCTION scriptFunction, IScriptTable* pEnv)
{
	lua_getref(L, (int)(INT_PTR)scriptFunction);
	if (!lua_isfunction(L, -1))
	{
		CryLogWarning("[CScriptSystem::SetEnvironment] Function %d not found", scriptFunction);
	}

	PushTable(pEnv);
	lua_setfenv(L, -2);
	lua_pop(L, 1);
}

IScriptTable* CScriptSystem::GetEnvironment(HSCRIPTFUNCTION scriptFunction)
{
	lua_getref(L, (int)(INT_PTR)scriptFunction);

	if (!lua_isfunction(L, -1))
	{
		CryLogWarning("[CScriptSystem::SetEnvironment] Function %d not found", scriptFunction);
	}
	IScriptTable* env = 0;
	lua_getfenv(L, -1);
	if (lua_istable(L, -1))
	{
		// The method AttachTable will cause an element to be poped from the lua stack.
		// Therefore, we need to pop only one element instead of two if we enter this
		// block.
		AttachTable(env = CreateTable(true));
		lua_pop(L, 1);
	}
	else
	{
		lua_pop(L, 2);
	}
	return env;
}

//////////////////////////////////////////////////////////////////////////
void CScriptSystem::RaiseError(const char* format, ...)
{
	va_list arglist;
	char sBuf[2048];
	int nCurrentLine = 0;
	const char* sSourceFile = "undefined";

	va_start(arglist, format);
	vsprintf(sBuf, format, arglist);
	va_end(arglist);

	CryLogWarning("[Lua Error] %s", sBuf);

	TraceScriptError(sSourceFile, nCurrentLine, sBuf);
}

void CScriptSystem::LoadScriptedSurfaceTypes(const char* sFolder, bool bReload)
{
	// not used in Crysis
}

//////////////////////////////////////////////////////////////////////////
void* CScriptSystem::Allocate(size_t sz)
{
	// TODO: allocator
	return malloc(sz);
}

size_t CScriptSystem::Deallocate(void* ptr)
{
	// TODO: allocator
	free(ptr);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
int CScriptSystem::GetStackSize()
{
	return lua_gettop(L);
}

//////////////////////////////////////////////////////////////////////////
uint32 CScriptSystem::GetScriptAllocSize()
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////
void CScriptSystem::GetMemoryStatistics(ICrySizer* pSizer)
{
}

//////////////////////////////////////////////////////////////////////////
void CScriptSystem::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	switch (event)
	{
/*
		case ESYSTEM_EVENT_LEVEL_POST_UNLOAD:
		{
			ForceGarbageCollection();
			break;
		}
*/
		case ESYSTEM_EVENT_LEVEL_LOAD_START:
		{
			ForceGarbageCollection();
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
struct IRecursiveLuaDump
{
	virtual ~IRecursiveLuaDump(){}
	virtual void OnElement(int nLevel, const char* sKey, int nKey, ScriptAnyValue& value) = 0;
	virtual void OnBeginTable(int nLevel, const char* sKey, int nKey) = 0;
	virtual void OnEndTable(int nLevel) = 0;
};

struct SRecursiveLuaDumpToFile : public IRecursiveLuaDump
{
	FILE* file;
	char  sLevelOffset[1024];
	char  sKeyStr[32];
	int   nSize;

	const char* GetOffsetStr(int nLevel)
	{
		if (nLevel > sizeof(sLevelOffset) - 1)
			nLevel = sizeof(sLevelOffset) - 1;
		memset(sLevelOffset, '\t', nLevel);
		sLevelOffset[nLevel] = 0;
		return sLevelOffset;
	}
	const char* GetKeyStr(const char* sKey, int nKey)
	{
		if (sKey)
			return sKey;
		sprintf(sKeyStr, "[%02d]", nKey);
		return sKeyStr;
	}
	SRecursiveLuaDumpToFile(const char* filename)
		: nSize(0)
	{
		file = fxopen(filename, "wt");
		memset(sLevelOffset, 0, sizeof sLevelOffset / sizeof sLevelOffset[0]);
		memset(sKeyStr, 0, sizeof sKeyStr / sizeof sKeyStr[0]);
	}
	~SRecursiveLuaDumpToFile()
	{
		if (file)
			fclose(file);
	}
	virtual void OnElement(int nLevel, const char* sKey, int nKey, ScriptAnyValue& value)
	{
		nSize += sizeof(Node);
		if (sKey)
			nSize += strlen(sKey) + 1;
		else
			nSize += sizeof(int);
		switch (value.type)
		{
		case ANY_TBOOLEAN:
			if (value.b)
				fprintf(file, "[%6d] %s %s=true\n", nSize, GetOffsetStr(nLevel), GetKeyStr(sKey, nKey));
			else
				fprintf(file, "[%6d] %s %s=false\n", nSize, GetOffsetStr(nLevel), GetKeyStr(sKey, nKey));
			break;
		case ANY_THANDLE:
			fprintf(file, "[%6d] %s %s=%p\n", nSize, GetOffsetStr(nLevel), GetKeyStr(sKey, nKey), value.ptr);
			break;
		case ANY_TNUMBER:
			fprintf(file, "[%6d] %s %s=%g\n", nSize, GetOffsetStr(nLevel), GetKeyStr(sKey, nKey), value.number);
			break;
		case ANY_TSTRING:
			fprintf(file, "[%6d] %s %s=%s\n", nSize, GetOffsetStr(nLevel), GetKeyStr(sKey, nKey), value.str);
			nSize += strlen(value.str) + 1;
			break;
		//case ANY_TTABLE:
		case ANY_TFUNCTION:
			fprintf(file, "[%6d] %s %s()\n", nSize, GetOffsetStr(nLevel), GetKeyStr(sKey, nKey));
			break;
		case ANY_TUSERDATA:
			fprintf(file, "[%6d] %s [userdata] %s\n", nSize, GetOffsetStr(nLevel), GetKeyStr(sKey, nKey));
			break;
		case ANY_TVECTOR:
			fprintf(file, "[%6d] %s %s=%g,%g,%g\n", nSize, GetOffsetStr(nLevel), GetKeyStr(sKey, nKey), value.vec3.x, value.vec3.y, value.vec3.z);
			nSize += sizeof(Vec3);
			break;
		}
	}
	virtual void OnBeginTable(int nLevel, const char* sKey, int nKey)
	{
		nSize += sizeof(Node);
		nSize += sizeof(Table);
		fprintf(file, "[%6d] %s %s = {\n", nSize, GetOffsetStr(nLevel), GetKeyStr(sKey, nKey));
	}
	virtual void OnEndTable(int nLevel)
	{
		fprintf(file, "[%6d] %s }\n", nSize, GetOffsetStr(nLevel));
	}
};

//////////////////////////////////////////////////////////////////////////
static void RecursiveTableDump(CScriptSystem* pSS, lua_State* L, int idx, int nLevel, IRecursiveLuaDump* sink, std::set<void*>& tables)
{
	const char* sKey = 0;
	int nKey = 0;

	void* pTable = (void*)lua_topointer(L, idx);
	if (tables.find(pTable) != tables.end())
	{
		// This table was already dumped.
		return;
	}
	tables.insert(pTable);

	lua_pushnil(L);
	while (lua_next(L, idx) != 0)
	{
		// `key' is at index -2 and `value' at index -1
		if (lua_type(L, -2) == LUA_TSTRING)
			sKey = lua_tostring(L, -2);
		else
		{
			sKey = 0;
			nKey = (int)lua_tonumber(L, -2); // key index.
		}
		int type = lua_type(L, -1);
		switch (type)
		{
		case LUA_TNIL:
			break;
		case LUA_TTABLE:
			{
				if (!(sKey != 0 && nLevel == 0 && strcmp(sKey, "_G") == 0))
				{
					sink->OnBeginTable(nLevel, sKey, nKey);
					RecursiveTableDump(pSS, L, lua_gettop(L), nLevel + 1, sink, tables);
					sink->OnEndTable(nLevel);
				}
			}
			break;
		default:
			{
				ScriptAnyValue any;
				pSS->ToAny(any, -1);
				sink->OnElement(nLevel, sKey, nKey, any);
			}
			break;
		}
		lua_pop(L, 1);
	}
}

//////////////////////////////////////////////////////////////////////////
void CScriptSystem::DumpStateToFile(const char* filename)
{
	SRecursiveLuaDumpToFile sink(filename);
	if (sink.file)
	{
		std::set<void*> tables;
		RecursiveTableDump(this, L, LUA_GLOBALSINDEX, 0, &sink, tables);
	}
}

//////////////////////////////////////////////////////////////////////////
void CScriptSystem::SerializeTimers(ISerialize* pSer)
{
	TSerialize ser(pSer);
	m_pScriptTimerMgr->Serialize(ser);
}

//////////////////////////////////////////////////////////////////////////
void CScriptSystem::ResetTimers()
{
	m_pScriptTimerMgr->Reset();
}

//////////////////////////////////////////////////////////////////////////
HSCRIPTFUNCTION CScriptSystem::CompileBuffer(const char* sBuffer, size_t nSize, const char* sBufferDesc)
{
	const char* sBufferDescription = "Pre Compiled Code";
	if (sBufferDesc)
		sBufferDescription = sBufferDesc;

	int status = luaL_loadbuffer(L, sBuffer, nSize, sBufferDescription);

	if (status == 0)
	{
		return (HSCRIPTFUNCTION)(INT_PTR)lua_ref(L, 1);
	}
	else
	{
		const char* sErr = lua_tostring(L, -2);
		GetISystem()->Warning(VALIDATOR_MODULE_SCRIPTSYSTEM, VALIDATOR_WARNING, VALIDATOR_FLAG_FILE | VALIDATOR_FLAG_SCRIPT, sBufferDescription,
		                      "[Lua Error] Failed to compile code [%s]: %s", sBuffer, sErr);

		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////
int CScriptSystem::PreCacheBuffer(const char* sBuffer, size_t nSize, const char* sBufferDesc)
{
	const char* sBufferDescription = "Pre Cached Code";
	if (sBufferDesc)
		sBufferDescription = sBufferDesc;

	int iIndex = -1;
	if (HSCRIPTFUNCTION scriptFunction = CompileBuffer(sBuffer, nSize, sBufferDesc))
	{
		iIndex = m_vecPreCached.size();
		m_pPreCacheBufferTable->SetAt(iIndex, scriptFunction);
		m_vecPreCached.push_back(sBuffer);
	}

	return iIndex;
}

//////////////////////////////////////////////////////////////////////////
int CScriptSystem::BeginPreCachedBuffer(int iIndex)
{
	assert(iIndex >= 0);

	int iRet = 0;

	if (iIndex >= 0 && iIndex < (int)m_vecPreCached.size())
	{
		ScriptAnyValue ScriptFunction;
		m_pPreCacheBufferTable->GetAtAny(iIndex, ScriptFunction);

		if (ScriptFunction.GetVarType() == svtFunction)
		{
			iRet = BeginCall(ScriptFunction.function);
		}

		if (iRet == 0)
		{
			GetISystem()->Warning(VALIDATOR_MODULE_SCRIPTSYSTEM, VALIDATOR_WARNING, VALIDATOR_FLAG_FILE | VALIDATOR_FLAG_SCRIPT, "Precached",
			                      "[Lua] Error executing: [%s]", m_vecPreCached[iIndex].c_str());
		}
	}
	else
	{
		GetISystem()->Warning(VALIDATOR_MODULE_SCRIPTSYSTEM, VALIDATOR_WARNING, VALIDATOR_FLAG_FILE | VALIDATOR_FLAG_SCRIPT, "Precached",
		                      "[Lua] Precached buffer %d does not exist (last valid is %d)", iIndex, m_vecPreCached.size());
	}

	return (iRet);
}

//////////////////////////////////////////////////////////////////////////
void CScriptSystem::ClearPreCachedBuffer()
{
	m_pPreCacheBufferTable->Clear();
	m_vecPreCached.clear();
}

/*
extern "C" DLL_EXPORT IScriptSystem *CreateScriptSystem(ISystem *pSystem, bool bStdLibs)
{
	ModuleInitISystem(pSystem);

	CScriptSystem *pScriptSystem = new CScriptSystem;

	if (!pScriptSystem->Init(pSystem, bStdLibs, 1024))
	{
		pScriptSystem->Release();
		return nullptr;
	}

	return pScriptSystem;
}
*/

ScriptVarType CScriptSystem::LuaTypeToScriptVarType(int type)
{
	switch (type)
	{
		case LUA_TNIL:           return svtNull;
		case LUA_TBOOLEAN:       return svtBool;
		case LUA_TLIGHTUSERDATA: return svtPointer;
		case LUA_TNUMBER:        return svtNumber;
		case LUA_TSTRING:        return svtString;
		case LUA_TTABLE:         return svtObject;
		case LUA_TFUNCTION:      return svtFunction;
		case LUA_TUSERDATA:      return svtUserData;
	}

	return svtNull;
}

const char *CScriptSystem::ScriptAnyTypeToString(ScriptAnyType type)
{
	switch (type)
	{
		case ANY_ANY:       return "Any";
		case ANY_TNIL:      return "Null";
		case ANY_TBOOLEAN:  return "Boolean";
		case ANY_TSTRING:   return "String";
		case ANY_TNUMBER:   return "Number";
		case ANY_TFUNCTION: return "Function";
		case ANY_TTABLE:    return "Table";
		case ANY_TUSERDATA: return "UserData";
		case ANY_THANDLE:   return "Pointer";
		case ANY_TVECTOR:   return "Vec3";
	}

	return "Unknown";
}

const char *CScriptSystem::ScriptVarTypeToString(ScriptVarType type)
{
	switch (type)
	{
		case svtNull:     return "Null";
		case svtBool:     return "Boolean";
		case svtString:   return "String";
		case svtNumber:   return "Number";
		case svtFunction: return "Function";
		case svtObject:   return "Table";
		case svtUserData: return "UserData";
		case svtPointer:  return "Pointer";
	}

	return "Unknown";
}
