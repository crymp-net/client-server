/**
 * @file
 * @brief Implementation of the new CryEngine log.
 */

#include <cstring>
#include <new>
#include <algorithm>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "CryCommon/ISystem.h"
#include "CryCommon/IConsole.h"
#include "Launcher/Launcher.h"
#include "Launcher/IExecutorTask.h"
#include "Launcher/CmdLine.h"
#include "Launcher/Util.h"
#include "Library/StringBuffer.h"

#include "Log.h"

#define LOG_DEFAULT_VERBOSITY 1
#define LOG_DEFAULT_PREFIX_FILE_ONLY 1

struct LogTask : public IExecutorTask
{
	CLog *pLog;
	std::string msg;
	size_t prefixLength;
	int flags;

	LogTask(CLog *pLog)
	: pLog(pLog),
	  msg(),
	  flags()
	{
	}

	// worker thread
	void execute() override
	{
	}

	// main thread
	void callback() override
	{
		pLog->write(msg.c_str(), prefixLength, flags);
	}
};

static bool IsMsgAllowed(ILog::ELogType msgType, int verbosity)
{
	switch (msgType)
	{
		case ILog::eAlways:
		case ILog::eWarningAlways:
		case ILog::eErrorAlways:
		case ILog::eInput:
		case ILog::eInputResponse:
		{
			return verbosity >= 0;
		}
		case ILog::eError:
		{
			return verbosity >= 1;
		}
		case ILog::eWarning:
		{
			return verbosity >= 2;
		}
		case ILog::eMessage:
		{
			return verbosity >= 3;
		}
		case ILog::eComment:
		{
			return verbosity >= 4;
		}
	}

	return false;
}

static bool CreateLogFileBackup(HANDLE file, const char *fileName, const char *filePath, bool showError)
{
	char lineBuffer[256];
	DWORD lineLength = 0;

	// read the first line from the existing log file
	if (!ReadFile(file, lineBuffer, sizeof lineBuffer, &lineLength, NULL))
	{
		if (showError)
		{
			Util::ErrorBox("Failed to read from the existing log file!");
		}

		return false;
	}

	if (lineLength == 0)
	{
		// the existing log file is empty, so no backup is needed
		return true;
	}

	// skip any text after the first line
	for (size_t i = 0 ; i < lineLength; i++)
	{
		char ch = lineBuffer[i];

		if (ch == '\r' || ch == '\n')  // CRLF
		{
			lineBuffer[i] = '\0';
			lineLength = i;
			break;
		}
	}

	std::string backupFileName = fileName;

	// remove file extension
	backupFileName = backupFileName.substr(0, backupFileName.find_last_of('.'));

	const char *prefix = "BackupNameAttachment=";
	const size_t prefixLength = std::strlen(prefix);  // optimized out

	// parse backup name attachment stored in the existing log file
	if (std::strncmp(lineBuffer, prefix, prefixLength) == 0)
	{
		const char *nameAttachment = lineBuffer + prefixLength;
		if (*nameAttachment == '\"')
		{
			nameAttachment++;

			size_t length = 0;
			while (nameAttachment[length] != '\0' && nameAttachment[length] != '\"')
			{
				length++;
			}

			backupFileName.append(nameAttachment, length);
		}
	}

	backupFileName += ".log";

	Path backupPath = Launcher::GetRootDirectory() + "LogBackups";

	if (!CreateDirectoryA(backupPath.toString(Path::WINDOWS).c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
	{
		if (showError)
		{
			Util::ErrorBox("Failed to create LogBackups directory!");
		}

		return false;
	}

	backupPath += backupFileName;

	if (!CopyFileA(filePath, backupPath.toString(Path::WINDOWS).c_str(), FALSE))
	{
		if (showError)
		{
			Util::ErrorBox("Failed to backup the existing log file!");
		}

		return false;
	}

	return true;
}

bool CLog::openLogFile(const char *fileName, bool showError)
{
	const std::string filePath = (Launcher::GetRootDirectory() + fileName).toString(Path::WINDOWS);

	HANDLE file = CreateFileA(filePath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,
	                          NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
	{
		if (showError)
		{
			Util::ErrorBox("Failed to open log file!");
		}

		return false;
	}
	else if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		if (!CreateLogFileBackup(file, fileName, filePath.c_str(), showError))
		{
			CloseHandle(file);
			return false;
		}

		// clear the existing file
		SetFilePointer(file, 0, NULL, FILE_BEGIN);
		SetEndOfFile(file);
	}

	m_file = file;
	m_fileName = fileName;

	return true;
}

void CLog::closeLogFile()
{
	CloseHandle(m_file);

	m_file = nullptr;
	m_fileName.clear();
}

void CLog::writeLogFile(const char *msg, size_t prefixLength, int flags)
{
	if (!m_file)
	{
		return;
	}

	StringBuffer<2048> result;

	result.append(msg, prefixLength);

	// workaround for broken messages from CPU detection
	bool addEmptyLine = false;

	for (size_t i = prefixLength; msg[i]; i++)
	{
		char ch = msg[i];

		if (ch < 32 || ch == 127)  // drop control characters
		{
			if (ch == '\n' && msg[i+1] == '\n' && msg[i+2] == '\0')
			{
				// the message ends with 2 or more newline characters
				addEmptyLine = true;
				break;
			}
		}
		else if (ch == '$')  // drop color codes
		{
			if (msg[i+1] == '$')  // convert "$$" to "$"
			{
				result += '$';
			}

			i++;
		}
		else
		{
			result += ch;
		}
	}

	// add newline character
	result += "\r\n";  // CRLF

	if (addEmptyLine)
	{
		// add one more newline
		result += "\r\n";
	}

	const bool isNewLine = !(flags & APPEND);

	if (!isNewLine)
	{
		// move file pointer before the last newline character
		SetFilePointer(m_file, -2, NULL, FILE_END);  // CRLF is 2 bytes long
	}

	DWORD bytesWritten;  // required
	WriteFile(m_file, result.get(), result.getLength(), &bytesWritten, NULL);

	for (ILogCallback *pCallback : m_callbacks)
	{
		pCallback->OnWriteToFile(msg, isNewLine);
	}
}

void CLog::writeConsole(const char *msg, size_t prefixLength, int flags)
{
	if (!gEnv)
	{
		return;
	}

	if (m_pLogPrefixFileOnlyCVar && m_pLogPrefixFileOnlyCVar->GetIVal())
	{
		// skip prefix
		msg += prefixLength;
	}

	const bool isNewLine = !(flags & APPEND);

	if (isNewLine)
	{
		gEnv->pConsole->PrintLine(msg);
	}
	else
	{
		gEnv->pConsole->PrintLinePlus(msg);
	}

	for (ILogCallback *pCallback : m_callbacks)
	{
		pCallback->OnWriteToConsole(msg, isNewLine);
	}
}

void CLog::write(const char *msg, size_t prefixLength, int flags)
{
	if (flags & FILE)
	{
		writeLogFile(msg, prefixLength, flags);
	}

	if (flags & CONSOLE)
	{
		writeConsole(msg, prefixLength, flags);
	}
}

void CLog::doLog(ELogType msgType, const char *format, va_list args, int flags)
{
	// yes, Crysis really contains code that calls log with null format...
	if (!format)
	{
		return;
	}

	int verbosity = GetVerbosityLevel();

	if (flags & CONSOLE)
	{
		if (!IsMsgAllowed(msgType, verbosity))
		{
			// do not log to console
			flags &= ~(CONSOLE);
		}
	}

	if (flags & FILE)
	{
		int fileVerbosity = (m_pLogFileVerbosityCVar) ? m_pLogFileVerbosityCVar->GetIVal() : m_defaultVerbosity;

		if (fileVerbosity > verbosity)
		{
			fileVerbosity = verbosity;
		}

		if (!IsMsgAllowed(msgType, fileVerbosity))
		{
			// do not log to file
			flags &= ~(FILE);
		}
	}

	if (!(flags & FILE) && !(flags & CONSOLE))
	{
		// nothing to do
		return;
	}

	StringBuffer<2048> buffer;

	const char *prefixFormat = (m_pLogPrefixCVar) ? m_pLogPrefixCVar->GetString() : nullptr;
	if (prefixFormat)
	{
		auto addNumber = [&buffer](int number, unsigned int minWidth = 0) -> void
		{
			std::string result = std::to_string(number);

			for (unsigned int i = minWidth; i > result.length(); i--)
			{
				buffer += '0';
			}

			buffer += result;
		};

		auto addTimeZone = [&buffer, &addNumber]() -> void
		{
			TIME_ZONE_INFORMATION tz;
			if (GetTimeZoneInformation(&tz) == TIME_ZONE_ID_INVALID)
			{
				return;
			}

			long bias = tz.Bias;

			if (bias == 0)
			{
				buffer += 'Z';  // UTC
			}
			else
			{
				if (bias < 0)
				{
					bias = -bias;
					buffer += '+';
				}
				else
				{
					buffer += '-';
				}

				addNumber(bias / 60, 2);  // hours
				addNumber(bias % 60, 2);  // minutes
			}
		};

		SYSTEMTIME time;
		GetLocalTime(&time);

		for (size_t i = 0; prefixFormat[i]; i++)
		{
			char ch = prefixFormat[i];

			if (ch == '%')
			{
				switch (prefixFormat[i+1])
				{
					case '%':
					{
						buffer += '%';
						break;
					}
					case 't':
					{
						buffer.append_f("%04lX", GetCurrentThreadId());
						break;
					}
					case 'd':
					{
						addNumber(time.wDay, 2);
						break;
					}
					case 'm':
					{
						addNumber(time.wMonth, 2);
						break;
					}
					case 'Y':
					{
						addNumber(time.wYear, 4);
						break;
					}
					case 'F':
					{
						addNumber(time.wYear, 4);
						buffer += '-';
						addNumber(time.wMonth, 2);
						buffer += '-';
						addNumber(time.wDay, 2);
						break;
					}
					case 'H':
					{
						addNumber(time.wHour, 2);
						break;
					}
					case 'M':
					{
						addNumber(time.wMinute, 2);
						break;
					}
					case 'S':
					{
						addNumber(time.wSecond, 2);
						break;
					}
					case 'T':
					{
						addNumber(time.wHour, 2);
						buffer += ':';
						addNumber(time.wMinute, 2);
						buffer += ':';
						addNumber(time.wSecond, 2);
						break;
					}
					case 'N':
					{
						addNumber(time.wMilliseconds, 3);
						break;
					}
					case 'z':
					{
						addTimeZone();
						break;
					}
				}

				i++;
			}
			else
			{
				buffer += ch;
			}
		}

		if (buffer.getLength() > 0)
		{
			buffer += ' ';
		}
	}

	size_t prefixLength = buffer.getLength();

	switch (msgType)
	{
		case ILog::eWarning:
		case ILog::eWarningAlways:
		{
			buffer += "$6[Warning] ";
			break;
		}
		case ILog::eError:
		case ILog::eErrorAlways:
		{
			buffer += "$4[Error] ";
			break;
		}
		case ILog::eComment:
		{
			buffer += "$9";  // new feature
			break;
		}
		case ILog::eMessage:
		case ILog::eAlways:
		case ILog::eInput:
		case ILog::eInputResponse:
		{
			break;
		}
	}

	buffer.append_vf(format, args);

	if (Launcher::IsMainThread())
	{
		write(buffer.get(), prefixLength, flags);
	}
	else
	{
		LogTask *pTask = new LogTask(this);
		pTask->msg = buffer.toString();
		pTask->prefixLength = prefixLength;
		pTask->flags = flags;

		Launcher::GetExecutor().addTaskCallback(pTask);
	}
}

bool CLog::Init(SSystemInitParams & params)
{
	if (params.pLog)
	{
		return true;
	}

	const std::string fileName = CmdLine::GetArgValue("-logfile", params.sLogFileName);
	const std::string prefix = CmdLine::GetArgValue("-logprefix");
	const int verbosity = CmdLine::GetArgValueInt("-verbosity", LOG_DEFAULT_VERBOSITY);

	CLog *pLog = new CLog(verbosity, prefix.c_str());

	params.pLog = pLog;

	return pLog->openLogFile(fileName.c_str(), true);
}

void CLog::Release()
{
}

void CLog::UpdateLoadingScreen(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	LogV(eMessage, format, args);
	va_end(args);

	if (!gEnv || !Launcher::IsMainThread())
	{
		return;
	}

	ISystem *pSystem = gEnv->pSystem;

	if (!pSystem->IsEditor())
	{
		ISystem::ILoadingProgressListener *pLoadingProgressListener = pSystem->GetLoadingProgressListener();
		if (pLoadingProgressListener)
		{
			pLoadingProgressListener->OnLoadingProgress(0);
		}
	}
}

void CLog::RegisterConsoleVariables()
{
	IConsole *pConsole = gEnv->pConsole;

	m_pLogVerbosityCVar = pConsole->RegisterInt("log_Verbosity", m_defaultVerbosity, VF_DUMPTODISK,
	  "Defines the verbosity level for console log messages (use log_FileVerbosity for file logging).\n"
	  "Usage: log_Verbosity [-1/0/1/2/3/4]\n"
	  " -1 = Suppress all logs (including eAlways).\n"  // CE5 feature
	  "  0 = Suppress all logs (except eAlways).\n"
	  "  1 = Additional errors.\n"
	  "  2 = Additional warnings.\n"
	  "  3 = Additional messages.\n"
	  "  4 = Additional comments."
	);

	m_pLogFileVerbosityCVar = pConsole->RegisterInt("log_FileVerbosity", m_defaultVerbosity, VF_DUMPTODISK,
	  "Defines the verbosity level for file log messages (if log_Verbosity defines higher value, this one is used).\n"
	  "Usage: log_FileVerbosity [-1/0/1/2/3/4]\n"
	  " -1 = Suppress all logs (including eAlways).\n"  // CE5 feature
	  "  0 = Suppress all logs (except eAlways).\n"
	  "  1 = Additional errors.\n"
	  "  2 = Additional warnings.\n"
	  "  3 = Additional messages.\n"
	  "  4 = Additional comments."
	);

	// maintain compatibility with legacy servers
	m_pLogIncludeTimeCVar = pConsole->RegisterInt("log_IncludeTime", 0, 0, "This CVar is not used.");

	const int flags = VF_NOT_NET_SYNCED;

	m_pLogPrefixCVar = pConsole->RegisterString("log_Prefix", m_defaultPrefix.c_str(), flags,
	  "Defines prefix of each log message.\n"
	  "Usage: log_Prefix FORMAT\n"
	  "The format string consists of normal characters and the following conversion specifiers:\n"
	  "  %% = %\n"
	  "  %d = Day of the month (01..31)\n"
	  "  %m = Month (01..12)\n"
	  "  %Y = Year (e.g. 2007)\n"
	  "  %H = Hour (00..23)\n"
	  "  %M = Minute (00..59)\n"
	  "  %S = Second (00..60)\n"
	  "  %N = Millisecond (000..999)\n"
	  "  %z = Offset from UTC (time zone) in the ISO 8601 format (e.g. +0100)\n"
	  "  %F = Equivalent to \"%Y-%m-%d\" (the ISO 8601 date format)\n"
	  "  %T = Equivalent to \"%H:%M:%S\" (the ISO 8601 time format)\n"
	  "  %t = Thread ID where the message was logged"
	);

	m_pLogPrefixFileOnlyCVar = pConsole->RegisterInt("log_PrefixFileOnly", LOG_DEFAULT_PREFIX_FILE_ONLY, flags,
	  "Defines whether the log prefix is added only when writing to the log file.\n"
	  "Usage: log_PrefixFileOnly [0/1]"
	);
}

void CLog::UnregisterConsoleVariables()
{
	m_pLogVerbosityCVar = nullptr;
	m_pLogFileVerbosityCVar = nullptr;
	m_pLogIncludeTimeCVar = nullptr;
	m_pLogPrefixCVar = nullptr;
	m_pLogPrefixFileOnlyCVar = nullptr;
}

void CLog::SetVerbosity(int verbosity)
{
	if (m_pLogVerbosityCVar)
	{
		m_pLogVerbosityCVar->Set(verbosity);
	}
}

int CLog::GetVerbosityLevel()
{
	return (m_pLogVerbosityCVar) ? m_pLogVerbosityCVar->GetIVal() : m_defaultVerbosity;
}

void CLog::AddCallback(ILogCallback *pCallback)
{
	if (pCallback && Launcher::IsMainThread() && std::count(m_callbacks.begin(), m_callbacks.end(), pCallback) == 0)
	{
		m_callbacks.push_back(pCallback);
	}
}

void CLog::RemoveCallback(ILogCallback *pCallback)
{
	if (pCallback && Launcher::IsMainThread())
	{
		m_callbacks.erase(std::remove(m_callbacks.begin(), m_callbacks.end(), pCallback), m_callbacks.end());
	}
}
