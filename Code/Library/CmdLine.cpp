#include <ctype.h>
#include <stdlib.h>  // atoi

#include "CmdLine.h"
#include "WinAPI.h"

namespace
{
	const char *GetCmdLineWithoutAppName()
	{
		const char *result = WinAPI::GetCmdLine();

		// skip program name
		if (*result == '\"')
		{
			for (result++; *result && *result != '\"'; result++);
		}
		else if (*result == '\'')
		{
			for (result++; *result && *result != '\''; result++);
		}
		else
		{
			for (result++; *result && !isspace(*result); result++);
		}

		// skip spaces after program name
		while (isspace(*result))
		{
			result++;
		}

		return result;
	}

	const char *GetArgValueBegin(const char *arg)
	{
		const char *result = GetCmdLineWithoutAppName();

		// find the argument
		for (; *result; result++)
		{
			if (tolower(*result) == tolower(*arg))
			{
				const char *name = arg + 1;  // the first letter already matches

				for (result++; *result && *name; result++, name++)
				{
					if (tolower(*result) != tolower(*name))
					{
						// skip the rest of argument name
						while (*result && !isspace(*result))
						{
							result++;
						}

						// and try again with other one
						break;
					}
				}

				// make sure the argument name really matches
				if (!*name && (!*result || isspace(*result)))
				{
					// skip spaces before argument value
					while (isspace(*result))
					{
						result++;
					}

					return result;
				}
			}
		}

		return nullptr;
	}
}

bool CmdLine::HasArg(const char *arg)
{
	return GetArgValueBegin(arg) != nullptr;
}

std::string CmdLine::GetArgValue(const char *arg, const char *defaultValue)
{
	const char *value = GetArgValueBegin(arg);

	if (value && *value)
	{
		const char *end = value;

		if (*value == '\"')
		{
			value++;
			end++;

			while (*end && *end != '\"')
			{
				end++;
			}
		}
		else if (*value == '\'')
		{
			value++;
			end++;

			while (*end && *end != '\'')
			{
				end++;
			}
		}
		else
		{
			while (*end && !isspace(*end))
			{
				end++;
			}
		}

		return std::string(value, end-value);
	}

	return (defaultValue) ? std::string(defaultValue) : std::string();
}

int CmdLine::GetArgValueInt(const char *arg, int defaultValue)
{
	const std::string value = GetArgValue(arg);

	return value.empty() ? defaultValue : atoi(value.c_str());
}
