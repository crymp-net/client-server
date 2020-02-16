/**
 * @file
 * @brief Implementation of log functions.
 */

#include "CryCommon/ISystem.h"

#include "Log.h"

void LogDebug(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	gEnv->pLog->LogV(ILog::eMessage, format, args);
	va_end(args);
}

void LogInfo(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	gEnv->pLog->LogV(ILog::eAlways, format, args);
	va_end(args);
}

void LogWarning(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	gEnv->pLog->LogV(ILog::eWarningAlways, format, args);
	va_end(args);
}

void LogError(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	gEnv->pLog->LogV(ILog::eErrorAlways, format, args);
	va_end(args);
}
