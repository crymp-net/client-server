/**
 * @file
 * @brief The new CryEngine log.
 */

#pragma once

#include <string>
#include <vector>

#include "CryCommon/ILog.h"

struct ICVar;
struct SSystemInitParams;

class CLog : public ILog
{
	enum ELogFlags
	{
		FILE    = (1 << 0),  //!< Log message to file.
		CONSOLE = (1 << 1),  //!< Log message to console.

		APPEND  = (1 << 2)   //!< Append message to the last line.
	};

	ICVar *m_pLogVerbosityCVar;
	ICVar *m_pLogFileVerbosityCVar;
	ICVar *m_pLogIncludeTimeCVar;
	ICVar *m_pLogPrefixCVar;
	ICVar *m_pLogPrefixFileOnlyCVar;
	int m_defaultVerbosity;
	void *m_file;
	std::string m_fileName;
	std::string m_defaultPrefix;
	std::vector<ILogCallback*> m_callbacks;

	bool openLogFile(const char *fileName, bool showError);
	void closeLogFile();
	void writeLogFile(const char *msg, size_t prefixLength, int flags);
	void writeConsole(const char *msg, size_t prefixLength, int flags);
	void write(const char *msg, size_t prefixLength, int flags);
	void doLog(ELogType msgType, const char *format, va_list args, int flags);

	friend struct LogTask;

public:
	CLog(int verbosity, const char *prefix = "")
	: m_pLogVerbosityCVar(),
	  m_pLogFileVerbosityCVar(),
	  m_pLogIncludeTimeCVar(),
	  m_pLogPrefixCVar(),
	  m_pLogPrefixFileOnlyCVar(),
	  m_defaultVerbosity(verbosity),
	  m_file(),
	  m_fileName(),
	  m_defaultPrefix(prefix),
	  m_callbacks()
	{
	}

	~CLog()
	{
		if (m_file)
		{
			closeLogFile();
		}

		UnregisterConsoleVariables();
	}

	void LogToFileV(ELogType type, const char *format, va_list args)
	{
		doLog(type, format, args, FILE);
	}

	void LogToConsoleV(ELogType type, const char *format, va_list args)
	{
		doLog(type, format, args, CONSOLE);
	}

	static bool Init(SSystemInitParams & params);

	// IMiniLog

	void LogV(ELogType type, const char *format, va_list args) override
	{
		doLog(type, format, args, FILE | CONSOLE);
	}

	void Log(const char *format, ...) override
	{
		va_list args;
		va_start(args, format);
		LogV(ILog::eMessage, format, args);
		va_end(args);
	}

	void LogWarning(const char *format, ...) override
	{
		va_list args;
		va_start(args, format);
		LogV(ILog::eWarning, format, args);
		va_end(args);
	}

	void LogError(const char *format, ...) override
	{
		va_list args;
		va_start(args, format);
		LogV(ILog::eError, format, args);
		va_end(args);
	}

	// ILog

	void Release() override;

	bool SetFileName(const char *fileName) override
	{
		return openLogFile(fileName, false);
	}

	const char *GetFileName() override
	{
		return m_fileName.c_str();
	}

	void LogPlus(const char *format, ...) override
	{
		va_list args;
		va_start(args, format);
		doLog(ILog::eMessage, format, args, FILE | CONSOLE | APPEND);
		va_end(args);
	}

	void LogToFile(const char *format, ...) override
	{
		va_list args;
		va_start(args, format);
		doLog(ILog::eMessage, format, args, FILE);
		va_end(args);
	}

	void LogToFilePlus(const char *format, ...) override
	{
		va_list args;
		va_start(args, format);
		doLog(ILog::eMessage, format, args, FILE | APPEND);
		va_end(args);
	}

	void LogToConsole(const char *format, ...) override
	{
		va_list args;
		va_start(args, format);
		doLog(ILog::eMessage, format, args, CONSOLE);
		va_end(args);
	}

	void LogToConsolePlus(const char *format, ...) override
	{
		va_list args;
		va_start(args, format);
		doLog(ILog::eMessage, format, args, CONSOLE | APPEND);
		va_end(args);
	}

	void UpdateLoadingScreen(const char *format, ...) override;

	void RegisterConsoleVariables() override;
	void UnregisterConsoleVariables() override;

	void SetVerbosity(int verbosity) override;
	int GetVerbosityLevel() override;

	void AddCallback(ILogCallback *pCallback) override;
	void RemoveCallback(ILogCallback *pCallback) override;
};
