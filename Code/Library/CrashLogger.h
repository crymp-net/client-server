#pragma once

#include <cstdio>

namespace CrashLogger
{
	using Handler = std::FILE* (*)();

	void OnEngineError(const char* format, va_list args);

	void* FindLoadedModule(const wchar_t* name);

	void Enable(Handler handler);
}
