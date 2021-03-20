#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>

#include "Library/StringBuffer.h"

#include "CrashLogger.h"
#include "Log.h"

#include "config.h"

static CLog *g_pLog;

static void LogWrite(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	g_pLog->LogToFileV(ILog::eAlways, format, args);
	va_end(args);
}

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

static bool IsCrysisDLL(const IMAGEHLP_MODULE & module)
{
	const std::string name = module.ModuleName;

	return name == "Cry3DEngine"
	    || name == "CryAction"
	    || name == "CryAISystem"
	    || name == "CryAnimation"
	    || name == "CryEntitySystem"
	    || name == "CryFont"
	    || name == "CryGame"
	    || name == "CryInput"
	    || name == "CryMovie"
	    || name == "CryNetwork"
	    || name == "CryPhysics"
	    || name == "CryRenderD3D10"
	    || name == "CryRenderD3D9"
	    || name == "CryRenderNULL"
	    || name == "CryScriptSystem"
	    || name == "CrySoundSystem"
	    || name == "CrySystem";
}

struct CallStackEntry
{
	uintptr_t address;
	std::string name;
	std::string module;
	std::string sourceFile;
	unsigned int sourceLine;
	unsigned int baseOffset;

	CallStackEntry() = default;

	std::string toString() const
	{
		StringBuffer<1024> buffer;

		buffer.append_f("0x%p: ", reinterpret_cast<void*>(address));

		if (name.empty())
		{
			buffer += "??";
		}
		else
		{
			buffer += name;

			if (baseOffset)
			{
				buffer.append_f(" + 0x%X", baseOffset);
			}
		}

		buffer += " (";

		if (!sourceFile.empty())
		{
			buffer += sourceFile;

			if (sourceLine)
			{
				buffer.append_f(":%u", sourceLine);
			}
		}

		buffer += ") in ";

		if (module.empty())
		{
			buffer += "?";
		}
		else
		{
			buffer += module;
		}

		return buffer.toString();
	}
};

static std::vector<CallStackEntry> GetCallStack(const CONTEXT *pExceptionContext)
{
	HANDLE process = GetCurrentProcess();
	HANDLE thread = GetCurrentThread();

	if (!SymInitialize(process, NULL, TRUE))
	{
		LogWrite("GetCallStack: SymInitialize failed with error code %lu", GetLastError());
	}

	SymSetOptions(SYMOPT_UNDNAME | SYMOPT_LOAD_LINES | SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_NO_PROMPTS);

	CONTEXT context = {};
	std::memcpy(&context, pExceptionContext, sizeof (CONTEXT));

	STACKFRAME frame = {};

#ifdef BUILD_64BIT
	frame.AddrPC.Offset = context.Rip;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrFrame.Offset = context.Rbp;
	frame.AddrFrame.Mode = AddrModeFlat;
	frame.AddrStack.Offset = context.Rsp;
	frame.AddrStack.Mode = AddrModeFlat;
#else
	frame.AddrPC.Offset = context.Eip;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrFrame.Offset = context.Ebp;
	frame.AddrFrame.Mode = AddrModeFlat;
	frame.AddrStack.Offset = context.Esp;
	frame.AddrStack.Mode = AddrModeFlat;
#endif

#ifdef BUILD_64BIT
	DWORD machine = IMAGE_FILE_MACHINE_AMD64;
#else
	DWORD machine = IMAGE_FILE_MACHINE_I386;
#endif

	std::vector<CallStackEntry> result;

	while (StackWalk(machine, process, thread, &frame, &context, NULL, SymFunctionTableAccess, SymGetModuleBase, NULL))
	{
		const uintptr_t address = frame.AddrPC.Offset;

		CallStackEntry entry;
		entry.address = address;

		IMAGEHLP_MODULE module = {};
		module.SizeOfStruct = sizeof (IMAGEHLP_MODULE);

		if (SymGetModuleInfo(process, address, &module))
		{
			entry.module = BaseName(module.ImageName);
		}

		uint8_t symbolBuffer[sizeof (SYMBOL_INFO) + MAX_SYM_NAME] = {};
		SYMBOL_INFO *pSymbol = reinterpret_cast<SYMBOL_INFO*>(symbolBuffer);
		pSymbol->SizeOfStruct = sizeof (SYMBOL_INFO);
		pSymbol->MaxNameLen = MAX_SYM_NAME;

		uint64_t symbolOffset = 0;

		if (SymFromAddr(process, address, &symbolOffset, pSymbol))
		{
			// drop useless symbols obtained from export tables of Crysis DLLs
			if (!(pSymbol->Flags & SYMFLAG_EXPORT) || !IsCrysisDLL(module))
			{
				entry.name = pSymbol->Name;
				entry.baseOffset = symbolOffset;
			}
		}

		IMAGEHLP_LINE line = {};
		line.SizeOfStruct = sizeof (IMAGEHLP_LINE);

		DWORD lineOffset = 0;

		if (SymGetLineFromAddr(process, address, &lineOffset, &line))
		{
			entry.sourceFile = line.FileName;
			entry.sourceLine = line.LineNumber;
		}

		result.emplace_back(std::move(entry));
	}

	SymCleanup(process);

	return result;
}

static LONG __stdcall CrashHandler(_EXCEPTION_POINTERS *pExceptionInfo)
{
	SetUnhandledExceptionFilter(NULL);

	LogWrite("================================ CRASH DETECTED ================================");

	LogWrite(CRYMP_CLIENT_EXE_DESCRIPTION);

	// TODO: exception info, register dump, module dump, ...

	LogWrite("Callstack:");

	for (const CallStackEntry & entry : GetCallStack(pExceptionInfo->ContextRecord))
	{
		LogWrite("%s", entry.toString().c_str());
	}

	LogWrite("================================================================================");

	return EXCEPTION_CONTINUE_SEARCH;
}

void CrashLogger::Init(CLog & log)
{
	g_pLog = &log;

	SetUnhandledExceptionFilter(CrashHandler);
}
