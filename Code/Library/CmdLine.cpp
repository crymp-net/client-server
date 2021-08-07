#include <stdlib.h>  // atoi
#include <string_view>

#include "CmdLine.h"
#include "WinAPI.h"

namespace
{
	std::string_view TrimSpacesStart(std::string_view text)
	{
		const auto pos = text.find_first_not_of(' ');

		text.remove_prefix((pos != std::string_view::npos) ? pos : text.length());

		return text;
	}

	std::string_view GetCmdLineWithoutAppName()
	{
		std::string_view cmdLine = WinAPI::GetCmdLine();

		if (!cmdLine.empty())
		{
			auto appNameEndPos = std::string_view::npos;

			// remove program name
			if (cmdLine.front() == '\"' || cmdLine.front() == '\'')
			{
				appNameEndPos = cmdLine.find(cmdLine.front(), 1);
			}
			else
			{
				appNameEndPos = cmdLine.find(' ');
			}

			if (appNameEndPos != std::string_view::npos)
			{
				cmdLine.remove_prefix(appNameEndPos + 1);

				// remove spaces after program name
				cmdLine = TrimSpacesStart(cmdLine);
			}
			else
			{
				cmdLine.remove_prefix(cmdLine.length());
			}
		}

		return cmdLine;
	}

	std::string_view FindArg(const std::string_view & arg)
	{
		std::string_view cmdLine = GetCmdLineWithoutAppName();

		auto pos = cmdLine.find(arg);

		while (pos != std::string_view::npos)
		{
			char before = ' ';
			char after = ' ';

			if (pos > 0)
			{
				before = cmdLine[pos - 1];
			}

			if ((pos + arg.length()) < cmdLine.length())
			{
				after = cmdLine[pos + arg.length()];
			}

			// TODO: ignore arguments inside values (quotation marks)

			if (before == ' ' && after == ' ')
			{
				cmdLine.remove_prefix(pos);

				return cmdLine;
			}

			pos = cmdLine.find(arg, pos);
		}

		// not found
		return std::string_view();
	}
}

bool CmdLine::HasArg(const std::string_view & arg)
{
	return !FindArg(arg).empty();
}

std::string CmdLine::GetArgValue(const std::string_view & arg, const std::string_view & defaultValue)
{
	std::string_view cmdLine = FindArg(arg);

	if (!cmdLine.empty())
	{
		cmdLine.remove_prefix(arg.length());

		// remove spaces before argument value
		cmdLine = TrimSpacesStart(cmdLine);

		if (!cmdLine.empty())
		{
			auto valueEndPos = std::string_view::npos;

			if (cmdLine.front() == '\"' || cmdLine.front() == '\'')
			{
				const char endMark = cmdLine.front();
				cmdLine.remove_prefix(1);
				valueEndPos = cmdLine.find(endMark);
			}
			else
			{
				valueEndPos = cmdLine.find(' ');
			}

			if (valueEndPos != std::string_view::npos)
			{
				cmdLine.remove_suffix(cmdLine.length() - valueEndPos);
			}

			return std::string(cmdLine);
		}
	}

	return std::string(defaultValue);
}

int CmdLine::GetArgValueInt(const std::string_view & arg, int defaultValue)
{
	const std::string value = GetArgValue(arg);

	return value.empty() ? defaultValue : atoi(value.c_str());
}
