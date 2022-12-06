#pragma once

#include <cstdio>

namespace CrashLogger
{
	using Handler = std::FILE* (*)();

	void OnEngineError(const char* format, ...);

	void Enable(Handler handler);
}
