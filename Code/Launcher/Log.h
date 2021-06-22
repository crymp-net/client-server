#pragma once

#include <string>
#include <vector>
#include <thread>
#include <deque>
#include <mutex>

#include "CryCommon/CrySystem/ILog.h"

struct ICVar;

struct LogMessage
{
	ILog::ELogType type = ILog::eMessage;

	bool isFile = false;
	bool isConsole = false;
	bool isAppend = false;

	std::string prefix;
	std::string content;
};

class LogMessageQueue
{
	std::deque<LogMessage> m_queue;
	std::mutex m_mutex;

public:
	LogMessageQueue() = default;

	// thread-safe
	void Push(LogMessage && message);
	bool Pop(LogMessage & message);
};

class CLog : public ILog
{
	std::thread::id m_mainThreadID;
	LogMessageQueue m_messageQueue;

	ICVar *m_pVerbosityCVar = nullptr;
	ICVar *m_pFileVerbosityCVar = nullptr;
	ICVar *m_pPrefixCVar = nullptr;

	std::vector<ILogCallback*> m_callbacks;

	std::string m_fileName;
	void *m_file = nullptr;
	int m_verbosity = 0;

	void OpenFile();
	void CloseFile();

	void Write(const LogMessage & message);
	void WriteToFile(const LogMessage & message);
	void WriteToConsole(const LogMessage & message);

	void Push(ILog::ELogType type, const char *format, va_list args, bool isFile, bool isConsole, bool isAppend);

public:
	CLog();
	~CLog();

	void Init(const char *defaultFileName);
	void OnUpdate();

	void *GetFile()
	{
		return m_file;
	}

	//////////
	// ILog //
	//////////

	void LogV(ILog::ELogType type, const char *format, va_list args) override;

	void Release() override;

	bool SetFileName(const char *fileName) override;
	const char *GetFileName() override;

	void Log(const char *format, ...) override;
	void LogWarning(const char *format, ...) override;
	void LogError(const char *format, ...) override;

	void LogPlus(const char *format, ...) override;
	void LogToFile(const char *format, ...) override;
	void LogToFilePlus(const char *format, ...) override;
	void LogToConsole(const char *format, ...) override;
	void LogToConsolePlus(const char *format, ...) override;

	void UpdateLoadingScreen(const char *format, ...) override;

	void RegisterConsoleVariables() override;
	void UnregisterConsoleVariables() override;

	void SetVerbosity(int verbosity) override;
	int GetVerbosityLevel() override;

	void AddCallback(ILogCallback *pCallback) override;
	void RemoveCallback(ILogCallback *pCallback) override;
};
