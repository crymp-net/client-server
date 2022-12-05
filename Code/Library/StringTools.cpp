#include <cstdio>

#include "StringTools.h"

extern "C" __declspec(dllimport) unsigned long __stdcall GetLastError();

std::string StringTools::Format(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	std::string result = FormatV(format, args);
	va_end(args);

	return result;
}

std::string StringTools::FormatV(const char* format, va_list args)
{
	std::string result;
	FormatToV(result, format, args);

	return result;
}

std::size_t StringTools::FormatTo(std::string& result, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	std::size_t length = FormatToV(result, format, args);
	va_end(args);

	return length;
}

std::size_t StringTools::FormatToV(std::string& result, const char* format, va_list args)
{
	va_list argsCopy;
	va_copy(argsCopy, args);

	char buffer[512];
	std::size_t length = FormatToV(buffer, sizeof buffer, format, args);

	// make sure the resulting string is not truncated
	if (length < sizeof buffer)
	{
		// do not overwrite the existing content
		result.append(buffer, length);
	}
	else
	{
		const std::size_t existingLength = result.length();

		result.resize(existingLength + length);

		// format string again with proper buffer size
		length = FormatToV(result.data() + existingLength, length + 1, format, argsCopy);

		if (result.length() > length)
		{
			result.resize(length);
		}
	}

	va_end(argsCopy);

	return length;
}

std::size_t StringTools::FormatTo(char* buffer, std::size_t bufferSize, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	std::size_t length = FormatToV(buffer, bufferSize, format, args);
	va_end(args);

	return length;
}

std::size_t StringTools::FormatToV(char* buffer, std::size_t bufferSize, const char* format, va_list args)
{
	if (!buffer || !bufferSize)
	{
		buffer = NULL;
		bufferSize = 0;
	}
	else
	{
		buffer[0] = '\0';
	}

	std::size_t length = 0;

	if (format)
	{
		const int status = std::vsnprintf(buffer, bufferSize, format, args);

		if (status > 0)
		{
			length = static_cast<std::size_t>(status);

			if (length >= bufferSize && bufferSize > 0)
			{
				// make sure the result is always null-terminated
				buffer[bufferSize - 1] = '\0';
			}
		}
	}

	return length;
}

std::runtime_error StringTools::ErrorFormat(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	std::runtime_error error = ErrorFormatV(format, args);
	va_end(args);

	return error;
}

std::runtime_error StringTools::ErrorFormatV(const char* format, va_list args)
{
	const std::string message = FormatV(format, args);

	return std::runtime_error(message);
}

std::system_error StringTools::SysErrorFormat(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	std::system_error error = SysErrorFormatV(format, args);
	va_end(args);

	return error;
}

std::system_error StringTools::SysErrorFormatV(const char* format, va_list args)
{
	const int code = static_cast<int>(GetLastError());

	const std::string message = FormatV(format, args);

	return std::system_error(code, std::system_category(), message);
}
