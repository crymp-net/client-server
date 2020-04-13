#include <cstdlib>  // std::atoi
#include <cctype>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "CmdLine.h"

static const char *GetCmdLineWithoutAppName()
{
	const char *cmdLine = GetCommandLineA();

	// skip program name
	if (*cmdLine == '\"')
	{
		for (cmdLine++; *cmdLine && *cmdLine != '\"'; cmdLine++);
	}
	else if (*cmdLine == '\'')
	{
		for (cmdLine++; *cmdLine && *cmdLine != '\''; cmdLine++);
	}
	else
	{
		for (cmdLine++; *cmdLine && !std::isspace(*cmdLine); cmdLine++);
	}

	if (*cmdLine)
	{
		cmdLine++;

		while (std::isspace(*cmdLine))
		{
			cmdLine++;
		}
	}

	return cmdLine;
}

static const char *GetArgValueBegin(const char *arg)
{
	const char *cmdLine = GetCmdLineWithoutAppName();

	// find the argument
	for (; *cmdLine; cmdLine++)
	{
		if (std::tolower(*cmdLine) == std::tolower(*arg))
		{
			const char *a = arg + 1;
			for (cmdLine++; *cmdLine && *a; cmdLine++, a++)
			{
				if (std::tolower(*cmdLine) != std::tolower(*a))
				{
					break;
				}
			}

			if (!*a)
			{
				// reached end of argument name
				if (!*cmdLine || std::isspace(*cmdLine))
				{
					return cmdLine;
				}
			}
		}
	}

	return nullptr;
}

bool CmdLine::HasArg(const char *arg)
{
	return GetArgValueBegin(arg) != nullptr;
}

std::string CmdLine::GetArgValue(const char *arg, const char *defaultValue)
{
	const char *valueBegin = GetArgValueBegin(arg);
	if (valueBegin)
	{
		while (std::isspace(*valueBegin))
		{
			valueBegin++;
		}

		if (*valueBegin)
		{
			size_t i = 0;

			if (*valueBegin == '\"')
			{
				valueBegin++;

				while (valueBegin[i] && valueBegin[i] != '\"')
				{
					i++;
				}
			}
			else if (*valueBegin == '\'')
			{
				valueBegin++;

				while (valueBegin[i] && valueBegin[i] != '\'')
				{
					i++;
				}
			}
			else
			{
				while (valueBegin[i] && !std::isspace(valueBegin[i]))
				{
					i++;
				}
			}

			return std::string(valueBegin, i);
		}
	}

	return (defaultValue) ? std::string(defaultValue) : std::string();
}

int CmdLine::GetArgValueInt(const char *arg, int defaultValue)
{
	std::string value = GetArgValue(arg);
	return (value.empty()) ? defaultValue : std::atoi(value.c_str());
}
