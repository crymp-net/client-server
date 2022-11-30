#include "CryLog.h"
#include "Logger.h"

void CryLog(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Logger::GetInstance().LogV(ILog::eMessage, format, args);
	va_end(args);
}

void CryLogWarning(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Logger::GetInstance().LogV(ILog::eWarning, format, args);
	va_end(args);
}

void CryLogError(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Logger::GetInstance().LogV(ILog::eError, format, args);
	va_end(args);
}

void CryLogAlways(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Logger::GetInstance().LogV(ILog::eAlways, format, args);
	va_end(args);
}

void CryLogWarningAlways(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Logger::GetInstance().LogV(ILog::eWarningAlways, format, args);
	va_end(args);
}

void CryLogErrorAlways(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Logger::GetInstance().LogV(ILog::eErrorAlways, format, args);
	va_end(args);
}

void CryLogComment(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Logger::GetInstance().LogV(ILog::eComment, format, args);
	va_end(args);
}
