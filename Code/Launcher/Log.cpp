#include <regex>
#include <algorithm>
#include <filesystem>

#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/ICryPak.h"
#include "CryCommon/CrySystem/IConsole.h"
#include "Library/CmdLine.h"
#include "Library/Error.h"
#include "Library/StringBuffer.h"
#include "Library/WinAPI.h"

#include "Log.h"

namespace
{
	std::filesystem::path GetLogDirectoryPath()
	{
		std::filesystem::path dir;

		const std::string rootArg = CmdLine::GetArgValue("-root");
		if (!rootArg.empty())
		{
			// the root directory
			dir = rootArg;
		}
		else
		{
			// "Documents/My Games/Crysis/"
			dir = gEnv->pCryPak->GetAlias("%USER%");
		}

		// sanitize the path
		return std::filesystem::canonical(dir);
	}

	void CreateLogFileBackup(void *file, const std::filesystem::path & filePath)
	{
		// read the beginning of the existing log file
		std::string header = WinAPI::FileRead(file, 127);

		if (header.empty())
		{
			// the existing log file is empty, so no backup is needed
			return;
		}

		std::string backupFileName = filePath.stem().string();  // filename without extension

		// get the backup filename attachment from the beginning of the existing log file
		std::smatch headerMatch;
		if (std::regex_search(header, headerMatch, std::regex("BackupNameAttachment=\"([a-zA-Z0-9()-_ ]*)\"")))
		{
			backupFileName += headerMatch[1].str();
		}

		backupFileName += ".log";

		const std::filesystem::path backupDirPath = filePath.parent_path() / "LogBackups";

		// make sure the "LogBackups" directory exists
		std::filesystem::create_directories(backupDirPath);

		const std::filesystem::path backupFilePath = backupDirPath / backupFileName;

		std::filesystem::copy_file(filePath, backupFilePath);
	}

	std::string FormatLogPrefix(const std::string_view & format)
	{
		const WinAPI::DateTime dateTime = WinAPI::GetCurrentDateTimeLocal();

		StringBuffer<128> buffer;

		// true if the previous character was '%'
		bool isFormatSpecifier = false;

		for (char ch : format)
		{
			if (isFormatSpecifier)
			{
				switch (ch)
				{
					case '%':
					{
						buffer += '%';
						break;
					}
					case 't':
					{
						buffer.addUInt(WinAPI::GetCurrentThreadID(), 4, '0', 16, true);
						break;
					}
					case 'd':
					{
						buffer.addUInt(dateTime.day, 2);
						break;
					}
					case 'm':
					{
						buffer.addUInt(dateTime.month, 2);
						break;
					}
					case 'Y':
					{
						buffer.addUInt(dateTime.year, 4);
						break;
					}
					case 'F':
					{
						buffer.addUInt(dateTime.year, 4);
						buffer += '-';
						buffer.addUInt(dateTime.month, 2);
						buffer += '-';
						buffer.addUInt(dateTime.day, 2);
						break;
					}
					case 'H':
					{
						buffer.addUInt(dateTime.hour, 2);
						break;
					}
					case 'M':
					{
						buffer.addUInt(dateTime.minute, 2);
						break;
					}
					case 'S':
					{
						buffer.addUInt(dateTime.second, 2);
						break;
					}
					case 'T':
					{
						buffer.addUInt(dateTime.hour, 2);
						buffer += ':';
						buffer.addUInt(dateTime.minute, 2);
						buffer += ':';
						buffer.addUInt(dateTime.second, 2);
						break;
					}
					case 'N':
					{
						buffer.addUInt(dateTime.millisecond, 3);
						break;
					}
					case 'z':
					{
						buffer += WinAPI::GetTimeZoneOffsetString();
						break;
					}
				}

				isFormatSpecifier = false;
			}
			else
			{
				switch (ch)
				{
					case '%':
					{
						isFormatSpecifier = true;
						break;
					}
					default:
					{
						buffer += ch;
						break;
					}
				}
			}
		}

		if (!buffer.empty())
		{
			// space after the prefix
			buffer += ' ';
		}

		return buffer.toString();
	}
}

void LogMessageQueue::Push(LogMessage && message)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_queue.emplace_back(std::move(message));
}

bool LogMessageQueue::Pop(LogMessage & message)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (!m_queue.empty())
	{
		message = std::move(m_queue.front());
		m_queue.pop_front();

		return true;
	}
	else
	{
		return false;
	}
}

void CLog::OpenFile()
{
	CloseFile();

	if (m_fileName.empty())
	{
		// no file to open
		return;
	}

	const std::filesystem::path filePath = GetLogDirectoryPath() / m_fileName;

	bool created = false;

	m_file = WinAPI::FileOpen(filePath, WinAPI::FileAccess::READ_WRITE_CREATE, &created);
	if (!m_file)
	{
		throw SystemError(filePath.string());
	}

	if (!created)
	{
		CreateLogFileBackup(m_file, filePath);

		// clear the existing file
		WinAPI::FileResize(m_file, 0);
	}
}

void CLog::CloseFile()
{
	if (m_file)
	{
		WinAPI::FileClose(m_file);
		m_file = nullptr;
	}
}

void CLog::Write(const LogMessage & message)
{
	if (message.isFile)
		WriteToFile(message);

	if (message.isConsole)
		WriteToConsole(message);
}

void CLog::WriteToFile(const LogMessage & message)
{
	if (!m_file)
		return;

	StringBuffer<256> buffer;

	if (message.isAppend)
	{
		buffer.reserve(message.content.length() + WinAPI::NEWLINE.length());
	}
	else
	{
		buffer.reserve(message.prefix.length() + message.content.length() + WinAPI::NEWLINE.length());

		// the prefix is used only in non-append mode
		buffer += message.prefix;
	}

	// true if the previous character was '$'
	bool isColorCode = false;

	for (char ch : message.content)
	{
		if (ch < 32 || ch == 127)
		{
			// drop control characters
		}
		else if (isColorCode)
		{
			// drop color codes

			if (ch == '$')
			{
				// convert "$$" to "$"
				buffer += '$';
			}

			isColorCode = false;
		}
		else if (ch == '$')
		{
			isColorCode = true;
		}
		else
		{
			buffer += ch;
		}
	}

	buffer += WinAPI::NEWLINE;

	// workaround for CPU detection messages
	if (message.content.length() >= 2)
	{
		auto it = message.content.rbegin();

		// the message ends with 2 or more newline characters
		if (*it == '\n' && *++it == '\n')
		{
			// add empty line after the message
			buffer += WinAPI::NEWLINE;
		}
	}

	if (message.isAppend)
	{
		// move the file pointer before the last newline character
		const int64_t offset = -static_cast<int64_t>(WinAPI::NEWLINE.length());

		WinAPI::FileSeek(m_file, WinAPI::FileSeekBase::END, offset);
	}

	WinAPI::FileWrite(m_file, buffer);

	for (ILogCallback *pCallback : m_callbacks)
	{
		pCallback->OnWriteToFile(message.content.c_str(), !message.isAppend);
	}
}

void CLog::WriteToConsole(const LogMessage & message)
{
	if (!gEnv)
		return;

	IConsole *pConsole = gEnv->pConsole;

	if (!pConsole)
		return;

	if (message.isAppend)
		pConsole->PrintLinePlus(message.content.c_str());
	else
		pConsole->PrintLine(message.content.c_str());

	for (ILogCallback *pCallback : m_callbacks)
	{
		pCallback->OnWriteToConsole(message.content.c_str(), !message.isAppend);
	}
}

void CLog::Push(ILog::ELogType type, const char *format, va_list args, bool isFile, bool isConsole, bool isAppend)
{
	if (!format)
	{
		// drop messages with null format
		return;
	}

	int requiredVerbosity = 0;

	switch (type)
	{
		case ILog::eAlways:
		case ILog::eWarningAlways:
		case ILog::eErrorAlways:
		case ILog::eInput:
		case ILog::eInputResponse:
		{
			requiredVerbosity = 0;
			break;
		}
		case ILog::eError:
		{
			requiredVerbosity = 1;
			break;
		}
		case ILog::eWarning:
		{
			requiredVerbosity = 2;
			break;
		}
		case ILog::eMessage:
		{
			requiredVerbosity = 3;
			break;
		}
		case ILog::eComment:
		{
			requiredVerbosity = 4;
			break;
		}
	}

	const int verbosity = GetVerbosityLevel();

	if (verbosity < requiredVerbosity)
	{
		// drop the message
		return;
	}

	const int fileVerbosity = (m_pFileVerbosityCVar) ? m_pFileVerbosityCVar->GetIVal() : verbosity;

	if (verbosity > fileVerbosity && fileVerbosity < requiredVerbosity)
	{
		isFile = false;
	}

	// message content
	StringBuffer<256> buffer;

	switch (type)
	{
		case ILog::eWarning:
		case ILog::eWarningAlways:
		{
			buffer += "$6[Warning] ";  // yellow
			break;
		}
		case ILog::eError:
		case ILog::eErrorAlways:
		{
			buffer += "$4[Error] ";  // red
			break;
		}
		case ILog::eComment:
		{
			buffer += "$9";  // gray
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

	buffer.addFormatV(format, args);

	LogMessage message;
	message.type = type;
	message.isFile = isFile;
	message.isConsole = isConsole;
	message.isAppend = isAppend;
	message.content = buffer.toString();

	if (m_pPrefixCVar)
	{
		const std::string_view prefix = m_pPrefixCVar->GetString();

		if (!prefix.empty() && prefix != "0")  // the prefix can be disabled by setting the cvar to zero
		{
			message.prefix = FormatLogPrefix(prefix);
		}
	}

	if (std::this_thread::get_id() == m_mainThreadID)
	{
		// messages from the main thread are processed immediately
		Write(message);
	}
	else
	{
		// messages from other threads are stored in the message queue
		// and then processed in OnUpdate called from the main thread
		m_messageQueue.Push(std::move(message));
	}
}

CLog::CLog()
{
	m_mainThreadID = std::this_thread::get_id();
}

CLog::~CLog()
{
	CloseFile();
}

void CLog::Init(const char *defaultFileName)
{
	const int defaultVerbosity = 0;

	m_fileName = CmdLine::GetArgValue("-logfile", defaultFileName);
	m_verbosity = CmdLine::GetArgValueInt("-verbosity", defaultVerbosity);

	try
	{
		OpenFile();
	}
	catch (const std::exception & ex)
	{
		throw Error(std::string("Failed to open the log file!\n\n") + ex.what());
	}

	CryLogAlways(FormatLogPrefix("BackupNameAttachment=\" Date(%Y %m %d) Time(%H %M %S)\"").c_str());
	CryLogAlways("");
}

void CLog::OnUpdate()
{
	LogMessage message;

	while (m_messageQueue.Pop(message))
	{
		Write(message);
	}
}

void CLog::LogV(ILog::ELogType type, const char *format, va_list args)
{
	Push(type, format, args, true, true, false);
}

void CLog::Release()
{
}

bool CLog::SetFileName(const char *fileName)
{
	return false;
}

const char *CLog::GetFileName()
{
	return m_fileName.c_str();
}

void CLog::Log(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	LogV(ILog::eMessage, format, args);
	va_end(args);
}

void CLog::LogWarning(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	LogV(ILog::eWarning, format, args);
	va_end(args);
}

void CLog::LogError(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	LogV(ILog::eError, format, args);
	va_end(args);
}

void CLog::LogPlus(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	Push(ILog::eMessage, format, args, true, true, true);
	va_end(args);
}

void CLog::LogToFile(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	Push(ILog::eMessage, format, args, true, false, false);
	va_end(args);
}

void CLog::LogToFilePlus(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	Push(ILog::eMessage, format, args, true, false, true);
	va_end(args);
}

void CLog::LogToConsole(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	Push(ILog::eMessage, format, args, false, true, false);
	va_end(args);
}

void CLog::LogToConsolePlus(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	Push(ILog::eMessage, format, args, false, true, true);
	va_end(args);
}

void CLog::UpdateLoadingScreen(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	LogV(ILog::eMessage, format, args);
	va_end(args);

	if (!gEnv)
		return;

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
	if (!gEnv)
		return;

	IConsole *pConsole = gEnv->pConsole;

	if (!pConsole)
		return;

	////////////////////
	// Existing CVars //
	////////////////////

	m_pVerbosityCVar = pConsole->RegisterInt("log_Verbosity", m_verbosity, VF_DUMPTODISK,
	  "Defines the verbosity level for console log messages (use log_FileVerbosity for file logging).\n"
	  "Usage: log_Verbosity [-1/0/1/2/3/4]\n"
	  " -1 = Suppress all logs (including eAlways).\n"
	  "  0 = Suppress all logs (except eAlways).\n"
	  "  1 = Additional errors.\n"
	  "  2 = Additional warnings.\n"
	  "  3 = Additional messages.\n"
	  "  4 = Additional comments."
	);

	m_pFileVerbosityCVar = pConsole->RegisterInt("log_FileVerbosity", m_verbosity, VF_DUMPTODISK,
	  "Defines the verbosity level for file log messages (if log_Verbosity is higher, this one is used).\n"
	  "Usage: log_FileVerbosity [-1/0/1/2/3/4]\n"
	  " -1 = Suppress all logs (including eAlways).\n"
	  "  0 = Suppress all logs (except eAlways).\n"
	  "  1 = Additional errors.\n"
	  "  2 = Additional warnings.\n"
	  "  3 = Additional messages.\n"
	  "  4 = Additional comments."
	);

	///////////////
	// New CVars //
	///////////////

	const std::string defaultPrefix = CmdLine::GetArgValue("-logprefix");

	m_pPrefixCVar = pConsole->RegisterString("log_Prefix", defaultPrefix.c_str(), VF_NOT_NET_SYNCED,
	  "Defines prefix of each message written to the log file.\n"
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
}

void CLog::UnregisterConsoleVariables()
{
}

void CLog::SetVerbosity(int verbosity)
{
	if (m_pVerbosityCVar)
		m_pVerbosityCVar->Set(verbosity);
	else
		m_verbosity = verbosity;
}

int CLog::GetVerbosityLevel()
{
	if (m_pVerbosityCVar)
		return m_pVerbosityCVar->GetIVal();
	else
		return m_verbosity;
}

void CLog::AddCallback(ILogCallback *pCallback)
{
	if (pCallback && std::count(m_callbacks.begin(), m_callbacks.end(), pCallback) == 0)
	{
		m_callbacks.push_back(pCallback);
	}
}

void CLog::RemoveCallback(ILogCallback *pCallback)
{
	if (pCallback)
	{
		m_callbacks.erase(std::remove(m_callbacks.begin(), m_callbacks.end(), pCallback), m_callbacks.end());
	}
}
