#include <windows.h>
#include <dbghelp.h>

#include <ctype.h>
#include <string>
#include <vector>
#include <algorithm>

#include "CrySystem/Logger.h"

#include "Library/StringTools.h"
#include "Library/WinAPI.h"

#include "CrashLogger.h"
#include "Launcher.h"

#include "config.h"

struct CallStackEntry
{
	size_t address = 0;
	std::string name;
	std::string moduleName;
	std::string sourceFile;
	unsigned int sourceLine = 0;
	unsigned int baseOffset = 0;

	CallStackEntry() = default;

	std::string ToString() const
	{
		std::string result;

	#ifdef BUILD_64BIT
		StringTools::FormatTo(result, "%016zX: ", address);
	#else
		StringTools::FormatTo(result, "%08X: ", address);
	#endif

		if (name.empty())
		{
			result += "??";
		}
		else
		{
			result += name;

			if (baseOffset)
			{
				StringTools::FormatTo(result, " + 0x%X", baseOffset);
			}
		}

		result += " (";

		if (!sourceFile.empty())
		{
			result += sourceFile;

			if (sourceLine)
			{
				StringTools::FormatTo(result, ":%u", sourceLine);
			}
		}

		result += ") in ";
		result += moduleName.empty() ? "?" : moduleName;

		return result;
	}
};

struct Module
{
	std::string name;
	size_t address = 0;
	size_t size = 0;

	Module() = default;

	bool operator<(const Module & other) const
	{
		return address < other.address;
	}

	std::string ToString() const
	{
	#ifdef BUILD_64BIT
		return StringTools::Format("%016zX - %016zX %s", address, address + size, name.c_str());
	#else
		return StringTools::Format("%08zX - %08zX %s", address, address + size, name.c_str());
	#endif
	}
};

static const char *BaseName(const char *name)
{
	size_t offset = 0;

	for (size_t i = 0; name[i]; i++)
	{
		if (name[i] == '/' || name[i] == '\\')
		{
			offset = i + 1;
		}
	}

	return name + offset;
}

static bool IsCrysisDLL(const IMAGEHLP_MODULE & moduleInfo)
{
	std::string name = moduleInfo.ModuleName;

	// convert DLL name to lowercase
	std::transform(name.begin(), name.end(), name.begin(), tolower);

	return name == "cry3dengine"
	    || name == "cryaction"
	    || name == "cryaisystem"
	    || name == "cryanimation"
	    || name == "cryentitysystem"
	    || name == "cryfont"
	    || name == "crygame"
	    || name == "cryinput"
	    || name == "crymovie"
	    || name == "crynetwork"
	    || name == "cryphysics"
	    || name == "cryrenderd3d10"
	    || name == "cryrenderd3d9"
	    || name == "cryrendernull"
	    || name == "cryscriptsystem"
	    || name == "crysoundsystem"
	    || name == "crysystem";
}

#ifdef BUILD_64BIT
static BOOL CALLBACK EnumerateModulesCallback(PCSTR name, DWORD64 address, ULONG size, PVOID context)
#else
static BOOL CALLBACK EnumerateModulesCallback(PCSTR name, ULONG address, ULONG size, PVOID context)
#endif
{
	std::vector<Module> *modules = static_cast<std::vector<Module>*>(context);

	modules->resize(modules->size() + 1);

	Module & result = modules->back();
	result.name = name;
	result.address = address;
	result.size = size;

	return TRUE;
}

class DebugHelper
{
	using TSymInitialize = BOOL (__stdcall *)(HANDLE process, const char *userSearchPath, BOOL invadeProcess);
	using TSymSetOptions = BOOL (__stdcall *)(DWORD options);
	using TSymCleanup = BOOL (__stdcall *)(HANDLE process);
	using TSymFromAddr = BOOL (__stdcall *)(HANDLE process, DWORD64 address, DWORD64 *offset, SYMBOL_INFO *symbol);
	using TSymGetLineFromAddr = BOOL (__stdcall *)(HANDLE process, size_t address, size_t *offset, IMAGEHLP_LINE *line);
	using TSymGetModuleInfo = BOOL (__stdcall *)(HANDLE process, size_t address, IMAGEHLP_MODULE *moduleInfo);
	using TEnumerateLoadedModules = BOOL (__stdcall *)(HANDLE process, PENUMLOADED_MODULES_CALLBACK callback, void *p);
	using TStackWalk = BOOL (__stdcall *)(DWORD machine, HANDLE process, HANDLE thread, STACKFRAME *frame,
	                                      void *contextRecord, PREAD_PROCESS_MEMORY_ROUTINE pReadMemoryRoutine,
	                                      PFUNCTION_TABLE_ACCESS_ROUTINE pFunctionTableAccessRoutine,
	                                      PGET_MODULE_BASE_ROUTINE pGetModuleBaseRoutine,
	                                      PTRANSLATE_ADDRESS_ROUTINE pTranslateAddressRoutine);

	HMODULE m_dll = nullptr;

	HANDLE m_process = nullptr;
	HANDLE m_thread = nullptr;

	TSymInitialize m_pSymInitialize = nullptr;
	TSymSetOptions m_pSymSetOptions = nullptr;
	TSymCleanup m_pSymCleanup = nullptr;
	TSymFromAddr m_pSymFromAddr = nullptr;
	TSymGetLineFromAddr m_pSymGetLineFromAddr = nullptr;
	TSymGetModuleInfo m_pSymGetModuleInfo = nullptr;
	TEnumerateLoadedModules m_pEnumerateLoadedModules = nullptr;
	TStackWalk m_pStackWalk = nullptr;

	PFUNCTION_TABLE_ACCESS_ROUTINE m_pSymFunctionTableAccess = nullptr;
	PGET_MODULE_BASE_ROUTINE m_pSymGetModuleBase = nullptr;

	bool m_isInitialized = false;

public:
	DebugHelper() = default;

	~DebugHelper()
	{
		if (m_isInitialized)
		{
			m_pSymCleanup(m_process);
			FreeLibrary(m_dll);
		}
	}

	bool Init()
	{
		if (m_isInitialized)
		{
			return true;
		}

		m_dll = LoadLibraryA("dbghelp.dll");
		if (!m_dll)
		{
			return false;
		}

		m_process = GetCurrentProcess();
		m_thread  = GetCurrentThread();

		m_pSymInitialize = reinterpret_cast<TSymInitialize>(GetProcAddress(m_dll, "SymInitialize"));
		m_pSymSetOptions = reinterpret_cast<TSymSetOptions>(GetProcAddress(m_dll, "SymSetOptions"));
		m_pSymCleanup    = reinterpret_cast<TSymCleanup>(GetProcAddress(m_dll, "SymCleanup"));
		m_pSymFromAddr   = reinterpret_cast<TSymFromAddr>(GetProcAddress(m_dll, "SymFromAddr"));

	#ifdef BUILD_64BIT
		m_pSymGetLineFromAddr     = reinterpret_cast<TSymGetLineFromAddr>(GetProcAddress(m_dll, "SymGetLineFromAddr64"));
		m_pSymGetModuleInfo       = reinterpret_cast<TSymGetModuleInfo>(GetProcAddress(m_dll, "SymGetModuleInfo64"));
		m_pEnumerateLoadedModules = reinterpret_cast<TEnumerateLoadedModules>(GetProcAddress(m_dll, "EnumerateLoadedModules64"));
		m_pStackWalk              = reinterpret_cast<TStackWalk>(GetProcAddress(m_dll, "StackWalk64"));
		m_pSymFunctionTableAccess = reinterpret_cast<PFUNCTION_TABLE_ACCESS_ROUTINE>(GetProcAddress(m_dll, "SymFunctionTableAccess64"));
		m_pSymGetModuleBase       = reinterpret_cast<PGET_MODULE_BASE_ROUTINE>(GetProcAddress(m_dll, "SymGetModuleBase64"));
	#else
		m_pSymGetLineFromAddr     = reinterpret_cast<TSymGetLineFromAddr>(GetProcAddress(m_dll, "SymGetLineFromAddr"));
		m_pSymGetModuleInfo       = reinterpret_cast<TSymGetModuleInfo>(GetProcAddress(m_dll, "SymGetModuleInfo"));
		m_pEnumerateLoadedModules = reinterpret_cast<TEnumerateLoadedModules>(GetProcAddress(m_dll, "EnumerateLoadedModules"));
		m_pStackWalk              = reinterpret_cast<TStackWalk>(GetProcAddress(m_dll, "StackWalk"));
		m_pSymFunctionTableAccess = reinterpret_cast<PFUNCTION_TABLE_ACCESS_ROUTINE>(GetProcAddress(m_dll, "SymFunctionTableAccess"));
		m_pSymGetModuleBase       = reinterpret_cast<PGET_MODULE_BASE_ROUTINE>(GetProcAddress(m_dll, "SymGetModuleBase"));
	#endif

		if (!m_pSymInitialize
		 || !m_pSymSetOptions
		 || !m_pSymCleanup
		 || !m_pSymFromAddr
		 || !m_pSymGetLineFromAddr
		 || !m_pSymGetModuleInfo
		 || !m_pEnumerateLoadedModules
		 || !m_pStackWalk
		 || !m_pSymFunctionTableAccess
		 || !m_pSymGetModuleBase)
		{
			return false;
		}

		if (!m_pSymInitialize(m_process, nullptr, TRUE))
		{
			return false;
		}

		m_pSymSetOptions(SYMOPT_UNDNAME | SYMOPT_LOAD_LINES | SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_NO_PROMPTS);

		m_isInitialized = true;

		return true;
	}

	bool IsInitialized() const
	{
		return m_isInitialized;
	}

	std::vector<CallStackEntry> GetCallStack(const CONTEXT *pExceptionContext) const
	{
		std::vector<CallStackEntry> result;

		if (!m_isInitialized)
		{
			return result;  // empty
		}

		CONTEXT context = {};
		memcpy(&context, pExceptionContext, sizeof (CONTEXT));

		STACKFRAME frame = {};
		DWORD machine = 0;

	#ifdef BUILD_64BIT
		machine = IMAGE_FILE_MACHINE_AMD64;

		frame.AddrPC.Offset = context.Rip;
		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrFrame.Offset = context.Rbp;
		frame.AddrFrame.Mode = AddrModeFlat;
		frame.AddrStack.Offset = context.Rsp;
		frame.AddrStack.Mode = AddrModeFlat;
	#else
		machine = IMAGE_FILE_MACHINE_I386;

		frame.AddrPC.Offset = context.Eip;
		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrFrame.Offset = context.Ebp;
		frame.AddrFrame.Mode = AddrModeFlat;
		frame.AddrStack.Offset = context.Esp;
		frame.AddrStack.Mode = AddrModeFlat;
	#endif

		while (m_pStackWalk(machine, m_process, m_thread, &frame, &context, nullptr,
		                    m_pSymFunctionTableAccess, m_pSymGetModuleBase, nullptr))
		{
			const size_t address = frame.AddrPC.Offset;

			result.resize(result.size() + 1);

			CallStackEntry & entry = result.back();
			entry.address = address;

			IMAGEHLP_MODULE moduleInfo = {};
			moduleInfo.SizeOfStruct = sizeof (IMAGEHLP_MODULE);

			if (m_pSymGetModuleInfo(m_process, address, &moduleInfo))
			{
				entry.moduleName = BaseName(moduleInfo.ImageName);
			}

			unsigned char symbolBuffer[sizeof (SYMBOL_INFO) + MAX_SYM_NAME] = {};
			SYMBOL_INFO *pSymbol = reinterpret_cast<SYMBOL_INFO*>(symbolBuffer);
			pSymbol->SizeOfStruct = sizeof (SYMBOL_INFO);
			pSymbol->MaxNameLen = MAX_SYM_NAME;

			DWORD64 symbolOffset = 0;

			if (m_pSymFromAddr(m_process, address, &symbolOffset, pSymbol))
			{
				if (pSymbol->Flags & SYMFLAG_EXPORT && IsCrysisDLL(moduleInfo))
				{
					// drop useless symbols obtained from export tables of Crysis DLLs
				}
				else
				{
					entry.name = pSymbol->Name;
					entry.baseOffset = symbolOffset;
				}
			}

			IMAGEHLP_LINE line = {};
			line.SizeOfStruct = sizeof (IMAGEHLP_LINE);

			size_t lineOffset = 0;

			if (m_pSymGetLineFromAddr(m_process, address, &lineOffset, &line))
			{
				entry.sourceFile = line.FileName;
				entry.sourceLine = line.LineNumber;
			}
		}

		return result;
	}

	std::vector<Module> GetLoadedModules() const
	{
		std::vector<Module> result;

		if (!m_isInitialized)
		{
			return result;  // empty
		}

		m_pEnumerateLoadedModules(m_process, EnumerateModulesCallback, &result);

		std::sort(result.begin(), result.end());

		return result;
	}
};

static const char *ExceptionCodeToString(unsigned int code)
{
	switch (code)
	{
		case EXCEPTION_ACCESS_VIOLATION:         return "Access violation";
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "Array index out of bounds";
		case EXCEPTION_BREAKPOINT:               return "Breakpoint";
		case EXCEPTION_DATATYPE_MISALIGNMENT:    return "Unaligned access";
		case EXCEPTION_FLT_DENORMAL_OPERAND:     return "Floating-point denormal operand";
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "Floating-point divide by zero";
		case EXCEPTION_FLT_INEXACT_RESULT:       return "Floating-point inexact result";
		case EXCEPTION_FLT_INVALID_OPERATION:    return "Floating-point invalid operation";
		case EXCEPTION_FLT_OVERFLOW:             return "Floating-point overflow";
		case EXCEPTION_FLT_STACK_CHECK:          return "Floating-point stack check";
		case EXCEPTION_FLT_UNDERFLOW:            return "Floating-point underflow";
		case EXCEPTION_ILLEGAL_INSTRUCTION:      return "Illegal instruction";
		case EXCEPTION_IN_PAGE_ERROR:            return "Page error";
		case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "Integer divide by zero";
		case EXCEPTION_INT_OVERFLOW:             return "Integer overflow";
		case EXCEPTION_INVALID_DISPOSITION:      return "Invalid disposition";
		case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "Noncontinuable exception";
		case EXCEPTION_PRIV_INSTRUCTION:         return "Privileged instruction";
		case EXCEPTION_SINGLE_STEP:              return "Single step";
		case EXCEPTION_STACK_OVERFLOW:           return "Stack overflow";
	}

	return "Unknown";
}

static void Log(const char* format, ...)
{
	std::FILE* file = Logger::GetInstance().GetFileHandle();

	va_list args;
	va_start(args, format);

	if (file)
	{
		std::vfprintf(file, format, args);
		std::fflush(file);
	}

	va_end(args);
}

static void DumpExceptionInfo(const EXCEPTION_RECORD *exception)
{
	const unsigned int code = exception->ExceptionCode;

	Log("%s exception (0x%08X) at 0x%p", ExceptionCodeToString(code), code, exception->ExceptionAddress);

	if (code == EXCEPTION_ACCESS_VIOLATION || code == EXCEPTION_IN_PAGE_ERROR)
	{
		void *address = reinterpret_cast<void*>(exception->ExceptionInformation[1]);

		switch (exception->ExceptionInformation[0])
		{
			case 0:
			{
				Log("Read from 0x%p failed", address);
				break;
			}
			case 1:
			{
				Log("Write to 0x%p failed", address);
				break;
			}
			case 8:
			{
				Log("Execute at 0x%p failed", address);
				break;
			}
		}
	}
}

static void DumpRegisters(const CONTEXT *ctx)
{
	Log("Register dump:");

#ifdef BUILD_64BIT
	Log("RIP: %016llX RSP: %016llX RBP: %016llX EFLAGS: %08X", ctx->Rip, ctx->Rsp, ctx->Rbp, ctx->EFlags);
	Log("RAX: %016llX RBX: %016llX RCX: %016llX RDX: %016llX", ctx->Rax, ctx->Rbx, ctx->Rcx, ctx->Rdx);
	Log("RSI: %016llX RDI: %016llX R8:  %016llX R9:  %016llX", ctx->Rsi, ctx->Rdi, ctx->R8, ctx->R9);
	Log("R10: %016llX R11: %016llX R12: %016llX R13: %016llX", ctx->R10, ctx->R11, ctx->R12, ctx->R13);
	Log("R14: %016llX R15: %016llX", ctx->R14, ctx->R15);
#else
	Log("EIP: %08X ESP: %08X EBP: %08X EFLAGS: %08X", ctx->Eip, ctx->Esp, ctx->Ebp, ctx->EFlags);
	Log("EAX: %08X EBX: %08X ECX: %08X EDX: %08X", ctx->Eax, ctx->Ebx, ctx->Ecx, ctx->Edx);
	Log("ESI: %08X EDI: %08X", ctx->Esi, ctx->Edi);
#endif
}

static void OnCrash(_EXCEPTION_POINTERS *pExceptionInfo)
{
	Log("================================ CRASH DETECTED ================================");

	Log("CryMP Client " CRYMP_CLIENT_VERSION_STRING " " CRYMP_CLIENT_BITS);

	DumpExceptionInfo(pExceptionInfo->ExceptionRecord);
	DumpRegisters(pExceptionInfo->ContextRecord);

	DebugHelper dbghelp;

	if (dbghelp.Init())
	{
		std::vector<CallStackEntry> callstack = dbghelp.GetCallStack(pExceptionInfo->ContextRecord);

		Log("Callstack:");
		for (size_t i = 0; i < callstack.size(); i++)
		{
			Log("%s", callstack[i].ToString().c_str());
		}

		std::vector<Module> modules = dbghelp.GetLoadedModules();

		Log("Modules (%u):", modules.size());
		for (size_t i = 0; i < modules.size(); i++)
		{
			Log("%s", modules[i].ToString().c_str());
		}
	}
	else
	{
		Log("CrashLogger: DebugHelper initialization failed with error code %lu", GetLastError());
	}

	Log("Command line:");
	Log("%s", WinAPI::CmdLine::GetFull());

	Log("================================================================================");
}

static LONG __stdcall CrashHandler(_EXCEPTION_POINTERS *pExceptionInfo)
{
	// disable this crash handler to avoid recursive calls
	SetUnhandledExceptionFilter(nullptr);

	OnCrash(pExceptionInfo);

	return EXCEPTION_CONTINUE_SEARCH;
}

void CrashLogger::Init()
{
	SetUnhandledExceptionFilter(CrashHandler);
}
