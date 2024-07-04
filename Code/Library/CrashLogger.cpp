#include <csignal>
#include <cstring>
#include <mutex>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winternl.h>
#include <dbghelp.h>
#include <intrin.h>

#include "config.h"

#include "CrashLogger.h"

#ifdef BUILD_64BIT
#define ADDR_FMT "%016I64X"
#else
#define ADDR_FMT "%08X"
#endif

static std::mutex g_mutex;
static CrashLogger::LogFileProvider g_logFileProvider;
static CrashLogger::HeapInfoProvider g_heapInfoProvider;

static void* ByteOffset(void* base, std::size_t offset)
{
	return static_cast<unsigned char*>(base) + offset;
}

static const char* BaseName(const char* name)
{
	const char* result = name;

	for (; *name; name++)
	{
		if (*name == '/' || *name == '\\')
		{
			result = name + 1;
		}
	}

	return result;
}

static const char* ExceptionCodeToName(unsigned int code)
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

static void DumpExceptionInfo(std::FILE* file, const EXCEPTION_RECORD* info)
{
	const unsigned int code = info->ExceptionCode;
	const std::size_t address = reinterpret_cast<std::size_t>(info->ExceptionAddress);

	std::fprintf(file, "%s exception (0x%08X) at 0x" ADDR_FMT "\n", ExceptionCodeToName(code), code, address);

	if (code == EXCEPTION_ACCESS_VIOLATION || code == EXCEPTION_IN_PAGE_ERROR)
	{
		const std::size_t dataAddress = info->ExceptionInformation[1];

		switch (info->ExceptionInformation[0])
		{
			case 0: std::fprintf(file, "Read from 0x"  ADDR_FMT " failed\n", dataAddress); break;
			case 1: std::fprintf(file, "Write to 0x"   ADDR_FMT " failed\n", dataAddress); break;
			case 8: std::fprintf(file, "Execute at 0x" ADDR_FMT " failed\n", dataAddress); break;
		}

		if (g_heapInfoProvider)
		{
			g_heapInfoProvider(file, reinterpret_cast<void*>(dataAddress));
		}
	}
	else if (g_heapInfoProvider)
	{
		// let debug allocator log its error message
		g_heapInfoProvider(file, nullptr);
	}

	std::fflush(file);
}

static void DumpMemoryUsage(std::FILE* file)
{
	MEMORYSTATUSEX status = {};
	status.dwLength = sizeof status;

	if (GlobalMemoryStatusEx(&status))
	{
		std::fprintf(file, "Physical memory = %.1f MiB (%.1f MiB available, %.1f%% used)\n",
			static_cast<double>(status.ullTotalPhys) / (1024 * 1024),
			static_cast<double>(status.ullAvailPhys) / (1024 * 1024),
			(100.0 * (status.ullTotalPhys - status.ullAvailPhys)) / status.ullTotalPhys
		);

		std::fprintf(file, "Virtual memory = %.1f MiB (%.1f MiB available, %.1f%% used)\n",
			static_cast<double>(status.ullTotalVirtual) / (1024 * 1024),
			static_cast<double>(status.ullAvailVirtual) / (1024 * 1024),
			(100.0 * (status.ullTotalVirtual - status.ullAvailVirtual)) / status.ullTotalVirtual
		);
	}
	else
	{
		std::fprintf(file, "GlobalMemoryStatusEx failed with error code %u\n", GetLastError());
	}

	std::fflush(file);
}

static void DumpRegisters(std::FILE* file, const CONTEXT* context)
{
	std::fprintf(file, "Registers:\n"
#ifdef BUILD_64BIT
	  "RIP: %016I64X RSP: %016I64X RBP: %016I64X EFLAGS: %08X\n"
	  "RAX: %016I64X RBX: %016I64X RCX: %016I64X RDX: %016I64X\n"
	  "RSI: %016I64X RDI: %016I64X R8:  %016I64X R9:  %016I64X\n"
	  "R10: %016I64X R11: %016I64X R12: %016I64X R13: %016I64X\n"
	  "R14: %016I64X R15: %016I64X\n",
	  context->Rip, context->Rsp, context->Rbp, context->EFlags,
	  context->Rax, context->Rbx, context->Rcx, context->Rdx,
	  context->Rsi, context->Rdi, context->R8,  context->R9,
	  context->R10, context->R11, context->R12, context->R13,
	  context->R14, context->R15
#else
	  "EIP: %08X ESP: %08X EBP: %08X EFLAGS: %08X\n"
	  "EAX: %08X EBX: %08X ECX: %08X EDX: %08X\n"
	  "ESI: %08X EDI: %08X\n",
	  context->Eip, context->Esp, context->Ebp, context->EFlags,
	  context->Eax, context->Ebx, context->Ecx, context->Edx,
	  context->Esi, context->Edi
#endif
	);

	std::fflush(file);
}

static void DumpCallStack(std::FILE* file, const CONTEXT* context)
{
	std::fprintf(file, "Callstack:\n");

	HANDLE process = GetCurrentProcess();
	HANDLE thread = GetCurrentThread();

#ifdef BUILD_64BIT
	DWORD machine = IMAGE_FILE_MACHINE_AMD64;

	STACKFRAME frame = {};
	frame.AddrPC.Offset = context->Rip;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrFrame.Offset = context->Rbp;
	frame.AddrFrame.Mode = AddrModeFlat;
	frame.AddrStack.Offset = context->Rsp;
	frame.AddrStack.Mode = AddrModeFlat;
#else
	DWORD machine = IMAGE_FILE_MACHINE_I386;

	STACKFRAME frame = {};
	frame.AddrPC.Offset = context->Eip;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrFrame.Offset = context->Ebp;
	frame.AddrFrame.Mode = AddrModeFlat;
	frame.AddrStack.Offset = context->Esp;
	frame.AddrStack.Mode = AddrModeFlat;
#endif

	CONTEXT localContext = *context;

	DWORD options = 0;
	options |= SYMOPT_DEFERRED_LOADS;
	options |= SYMOPT_EXACT_SYMBOLS;
	options |= SYMOPT_FAIL_CRITICAL_ERRORS;
	options |= SYMOPT_LOAD_LINES;
	options |= SYMOPT_NO_PROMPTS;
	options |= SYMOPT_UNDNAME;

	SymSetOptions(options);

	if (SymInitialize(process, NULL, TRUE))
	{
		while (StackWalk(machine, process, thread, &frame, &localContext, NULL,
		                 SymFunctionTableAccess, SymGetModuleBase, NULL))
		{
			const std::size_t address = frame.AddrPC.Offset;

			std::fprintf(file, ADDR_FMT ":", address);

			IMAGEHLP_MODULE moduleInfo = {};
			moduleInfo.SizeOfStruct = sizeof moduleInfo;

			if (SymGetModuleInfo(process, address, &moduleInfo))
			{
				std::fprintf(file, " %s:", BaseName(moduleInfo.ImageName));
			}
			else
			{
				std::fprintf(file, " ?:");
			}

			unsigned char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME] = {};
			SYMBOL_INFO& symbol = *reinterpret_cast<SYMBOL_INFO*>(symbolBuffer);
			symbol.SizeOfStruct = sizeof(SYMBOL_INFO);
			symbol.MaxNameLen = MAX_SYM_NAME;
			DWORD64 symbolOffset = 0;

			if (SymFromAddr(process, address, &symbolOffset, &symbol))
			{
				std::fprintf(file, " %s", symbol.Name);
			}
			else
			{
				std::fprintf(file, " ??");
			}

			IMAGEHLP_LINE line = {};
			line.SizeOfStruct = sizeof line;
			DWORD lineOffset = 0;

			if (SymGetLineFromAddr(process, address, &lineOffset, &line))
			{
				std::fprintf(file, " [%s:%u]\n", BaseName(line.FileName), line.LineNumber);
			}
			else
			{
				std::fprintf(file, " []\n");
			}
		}

		SymCleanup(process);
	}
	else
	{
		std::fprintf(file, "SymInitialize failed with error code %u\n", GetLastError());
	}

	std::fflush(file);
}

static void DumpLoadedModules(std::FILE* file)
{
	// old Windows SDKs don't provide complete enough definitions of all required structures
#ifdef BUILD_64BIT
	const std::size_t pebOffset = 0x60;
	const std::size_t ldrOffset = 0x18;
	const std::size_t modListOffset = 0x20;  // InMemoryOrderModuleList
	const std::size_t modBaseOffset = 0x30 - 0x10;
	const std::size_t modSizeOffset = 0x40 - 0x10;
	const std::size_t modNameOffset = 0x48 - 0x10;
#else
	const std::size_t pebOffset = 0x30;
	const std::size_t ldrOffset = 0x0C;
	const std::size_t modListOffset = 0x14;  // InMemoryOrderModuleList
	const std::size_t modBaseOffset = 0x18 - 0x8;
	const std::size_t modSizeOffset = 0x20 - 0x8;
	const std::size_t modNameOffset = 0x24 - 0x8;
#endif

	void* teb = NtCurrentTeb();
	void* peb = *static_cast<void**>(ByteOffset(teb, pebOffset));
	void* ldr = *static_cast<void**>(ByteOffset(peb, ldrOffset));

	LIST_ENTRY* headMod = static_cast<LIST_ENTRY*>(ByteOffset(ldr, modListOffset));

	LIST_ENTRY* firstMod = NULL;
	std::size_t firstModBase = -1;
	unsigned int modCount = 0;

	for (LIST_ENTRY* mod = headMod->Flink; mod != headMod; mod = mod->Flink)
	{
		const std::size_t modBase = *static_cast<std::size_t*>(ByteOffset(mod, modBaseOffset));

		if (modBase < firstModBase)
		{
			firstMod = mod;
			firstModBase = modBase;
		}

		modCount++;
	}

	std::fprintf(file, "Modules (%u):\n", modCount);

	for (LIST_ENTRY* mod = firstMod; mod != NULL;)
	{
		const std::size_t base = *static_cast<std::size_t*>(ByteOffset(mod, modBaseOffset));
		const std::size_t size = *static_cast<unsigned long*>(ByteOffset(mod, modSizeOffset));
		const UNICODE_STRING* wideName = static_cast<UNICODE_STRING*>(ByteOffset(mod, modNameOffset));

		char name[512] = {};
		WideCharToMultiByte(CP_UTF8, 0, wideName->Buffer, wideName->Length, name, sizeof name, NULL, NULL);

		std::fprintf(file, ADDR_FMT " - " ADDR_FMT " %s\n", base, base + size, name);

		LIST_ENTRY* nextMod = NULL;
		std::size_t nextModBase = -1;

		for (mod = headMod->Flink; mod != headMod; mod = mod->Flink)
		{
			const std::size_t modBase = *static_cast<std::size_t*>(ByteOffset(mod, modBaseOffset));

			if (modBase > base && modBase < nextModBase)
			{
				nextMod = mod;
				nextModBase = modBase;
			}
		}

		mod = nextMod;
	}

	std::fflush(file);
}

static void DumpCommandLine(std::FILE* file)
{
	std::fprintf(file, "Command line:\n");

	const char* cmdLine = GetCommandLineA();

	// hide login name and token
	while (const char* cmd = std::strstr(cmdLine, "+secu_login"))
	{
		for (; *cmd != ' ' && *cmd; cmd++) {}

		const char* end = cmd;

		// skip name
		for (; *end == ' '; end++) {}
		for (; *end != ' ' && *end; end++) {}
		// skip token
		for (; *end == ' '; end++) {}
		for (; *end != ' ' && *end; end++) {}

		std::fprintf(file, "%.*s <hidden> <hidden>", static_cast<int>(cmd - cmdLine), cmdLine);

		cmdLine = end;
	}

	std::fprintf(file, "%s\n", cmdLine);
	std::fflush(file);
}

static void WriteDumpHeader(std::FILE* file)
{
	std::fprintf(file, "================================ CRASH DETECTED ================================\n");
	std::fprintf(file, "%s\n", "CryMP Client " CRYMP_CLIENT_VERSION_STRING " " CRYMP_CLIENT_BITS " " CRYMP_CLIENT_BUILD_TYPE);
	std::fflush(file);
}

static void WriteDumpFooter(std::FILE* file)
{
	std::fprintf(file, "================================================================================\n");
	std::fflush(file);
}

static void WriteCrashDump(std::FILE* file, EXCEPTION_POINTERS* exception)
{
	WriteDumpHeader(file);

	DumpExceptionInfo(file, exception->ExceptionRecord);
	DumpMemoryUsage(file);
	DumpRegisters(file, exception->ContextRecord);
	DumpCallStack(file, exception->ContextRecord);
	DumpLoadedModules(file);
	DumpCommandLine(file);

	WriteDumpFooter(file);
}

static void WriteGenericErrorDump(std::FILE* file, CONTEXT* context, const char* message)
{
	WriteDumpHeader(file);

	std::fprintf(file, "%s\n", message);
	std::fflush(file);

	DumpMemoryUsage(file);
	DumpRegisters(file, context);
	DumpCallStack(file, context);
	DumpLoadedModules(file);
	DumpCommandLine(file);

	WriteDumpFooter(file);
}

static void WriteEngineErrorDump(std::FILE* file, CONTEXT* context, const char* format, va_list args)
{
	WriteDumpHeader(file);

	std::fprintf(file, "Engine error: ");
	std::fflush(file);

	std::vfprintf(file, format, args);
	std::fputc('\n', file);
	std::fflush(file);

	DumpMemoryUsage(file);
	DumpRegisters(file, context);
	DumpCallStack(file, context);
	DumpLoadedModules(file);
	DumpCommandLine(file);

	WriteDumpFooter(file);
}

static LONG __stdcall CrashHandler(EXCEPTION_POINTERS* exception)
{
	// avoid recursive calls
	SetUnhandledExceptionFilter(NULL);

	if (g_logFileProvider)
	{
		std::lock_guard lock(g_mutex);

		std::FILE* file = g_logFileProvider();

		if (file)
		{
			WriteCrashDump(file, exception);

			std::fclose(file);
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

static void PureCallHandler()
{
	CONTEXT context = {};
	RtlCaptureContext(&context);

	if (g_logFileProvider)
	{
		std::lock_guard lock(g_mutex);

		std::FILE* file = g_logFileProvider();

		if (file)
		{
			WriteGenericErrorDump(file, &context, "Pure function call");

			std::fclose(file);
		}
	}

	__fastfail(FAST_FAIL_FATAL_APP_EXIT);
}

static void InvalidParameterHandler(const wchar_t*, const wchar_t*, const wchar_t*, unsigned int, uintptr_t)
{
	CONTEXT context = {};
	RtlCaptureContext(&context);

	if (g_logFileProvider)
	{
		std::lock_guard lock(g_mutex);

		std::FILE* file = g_logFileProvider();

		if (file)
		{
			WriteGenericErrorDump(file, &context, "Invalid parameter detected by CRT");

			std::fclose(file);
		}
	}

	__fastfail(FAST_FAIL_FATAL_APP_EXIT);
}

static void AbortHandler(int)
{
	CONTEXT context = {};
	RtlCaptureContext(&context);

	if (g_logFileProvider)
	{
		std::lock_guard lock(g_mutex);

		std::FILE* file = g_logFileProvider();

		if (file)
		{
			WriteGenericErrorDump(file, &context, "Aborted");

			std::fclose(file);
		}
	}

	__fastfail(FAST_FAIL_FATAL_APP_EXIT);
}

void CrashLogger::OnEngineError(const char* format, va_list args)
{
	CONTEXT context = {};
	RtlCaptureContext(&context);

	if (g_logFileProvider)
	{
		std::lock_guard lock(g_mutex);

		std::FILE* file = g_logFileProvider();

		if (file)
		{
			WriteEngineErrorDump(file, &context, format, args);

			std::fclose(file);
		}
	}

	__fastfail(FAST_FAIL_FATAL_APP_EXIT);
}

void CrashLogger::Enable(LogFileProvider logFileProvider, HeapInfoProvider heapInfoProvider)
{
	g_logFileProvider = logFileProvider;
	g_heapInfoProvider = heapInfoProvider;

	SetUnhandledExceptionFilter(&CrashHandler);

	signal(SIGABRT, &AbortHandler);

	// set error handlers for our MSVC runtime
	// note that engine uses VS2005 MSVC runtime, which has its own error handlers
	_set_purecall_handler(&PureCallHandler);
	_set_invalid_parameter_handler(&InvalidParameterHandler);

	HMODULE msvcr80 = GetModuleHandleA("msvcr80.dll");
	if (msvcr80)
	{
		// VS2005 _set_purecall_handler is done by each engine DLL
		// and the handler simply calls ISystem::Error, which is redirected to our CrashLogger::OnEngineError
		// so let's keep it

		void* vs2005_set_invalid_parameter_handler = GetProcAddress(msvcr80, "_set_invalid_parameter_handler");
		if (vs2005_set_invalid_parameter_handler)
		{
			static_cast<decltype(&_set_invalid_parameter_handler)>
				(vs2005_set_invalid_parameter_handler)(&InvalidParameterHandler);
		}
	}
}
