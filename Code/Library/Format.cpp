#include <stdio.h>

#include "Format.h"

std::string Format(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	std::string result = FormatV(format, args);
	va_end(args);

	return result;
}

std::string FormatV(const char *format, va_list args)
{
	std::string result;

	if (format)
	{
		char buffer[1024];

		va_list argsCopy;
		va_copy(argsCopy, args);

		int status = vsnprintf(buffer, sizeof buffer, format, args);
		if (status > 0)
		{
			size_t length = status;

			// make sure the resulting string is not truncated
			if (length < sizeof buffer)
			{
				result.assign(buffer, length);
			}
			else
			{
				result.resize(length);

				// format string again with proper buffer size
				status = vsnprintf(result.data(), result.length() + 1, format, argsCopy);

				length = (status < 0) ? 0 : status;

				if (length < result.length())
				{
					result.resize(length);
				}
			}
		}

		va_end(argsCopy);
	}

	return result;
}
