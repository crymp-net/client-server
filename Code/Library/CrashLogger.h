#pragma once

#include <cstdio>

namespace CrashLogger
{
	using Handler = std::FILE* (*)();

	void OnEngineError(const char* format, va_list args);

	void Enable(Handler handler);
}
