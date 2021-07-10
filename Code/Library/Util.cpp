#include <ctype.h>
#include <wctype.h>
#include <string.h>
#include <array>

#include "Util.h"
#include "Format.h"
#include "StringBuffer.h"
#include "External/picosha2.h"

bool Util::EqualNoCase(const std::string_view & stringA, const std::string_view & stringB)
{
	return std::equal(stringA.begin(), stringA.end(), stringB.begin(), stringB.end(), [](char a, char b) -> bool
	{
		return tolower(a) == tolower(b);
	});
}

bool Util::EqualNoCase(const std::wstring_view & stringA, const std::wstring_view & stringB)
{
	return std::equal(stringA.begin(), stringA.end(), stringB.begin(), stringB.end(), [](wchar_t a, wchar_t b) -> bool
	{
		return towlower(a) == towlower(b);
	});
}

bool Util::StartsWith(const std::string_view & prefix, const std::string_view & text)
{
	if (prefix.length() <= text.length())
		return prefix == std::string_view(text.data(), prefix.length());
	else
		return false;
}

bool Util::StartsWith(const std::wstring_view & prefix, const std::wstring_view & text)
{
	if (prefix.length() <= text.length())
		return prefix == std::wstring_view(text.data(), prefix.length());
	else
		return false;
}

bool Util::StartsWithNoCase(const std::string_view & prefix, const std::string_view & text)
{
	if (prefix.length() <= text.length())
		return EqualNoCase(prefix, std::string_view(text.data(), prefix.length()));
	else
		return false;
}

bool Util::StartsWithNoCase(const std::wstring_view & prefix, const std::wstring_view & text)
{
	if (prefix.length() <= text.length())
		return EqualNoCase(prefix, std::wstring_view(text.data(), prefix.length()));
	else
		return false;
}

bool Util::PathStartsWith(const std::filesystem::path & prefix, const std::filesystem::path & path)
{
	auto [prefixIt, pathIt] = std::mismatch(
		prefix.begin(),
		prefix.end(),
		path.begin(),
		path.end(),
		[](const std::filesystem::path & elementA, const std::filesystem::path & elementB) -> bool
		{
			return EqualNoCase(elementA.native(), elementB.native());
		}
	);

	return prefixIt == prefix.end();
}

const char *Util::CopyToBuffer(const std::string_view & text, char *buffer, size_t bufferSize)
{
	if (buffer && bufferSize > 0)
	{
		const size_t length = std::min(text.length(), bufferSize - 1);

		memcpy(buffer, text.data(), length);
		buffer[length] = '\0';
	}

	return buffer;
}

std::string Util::MakeHumanReadableBytes(uint64_t bytes)
{
	constexpr std::array<char, 7> PREFIXES = { 'k', 'M', 'G', 'T', 'P', 'E', 'Z' };

	if (bytes == 0)
	{
		return "0 B";
	}

	if (bytes < 2000)
	{
		return std::to_string(bytes) + " B";
	}

	double value = bytes;

	// last prefix
	char prefix = PREFIXES.back();

	for (auto it = PREFIXES.begin(); it != PREFIXES.end(); ++it)
	{
		value /= 1000;

		if (value < 2000)
		{
			prefix = *it;
			break;
		}
	}

	return Format("%.1f %cB", value, prefix);
}

std::string Util::SecondsToString(uint64_t seconds)
{
	constexpr unsigned int SECONDS_PER_DAY = 60 * 60 * 24;

	StringBuffer<64> buffer;

	if (seconds >= SECONDS_PER_DAY)
	{
		const uint64_t days = seconds / SECONDS_PER_DAY;

		buffer.addUInt(days);
		buffer.add((days == 1) ? " day " : " days ");

		seconds %= SECONDS_PER_DAY;
	}

	const unsigned int hours = seconds / 3600;
	seconds %= 3600;

	const unsigned int minutes = seconds / 60;
	seconds %= 60;

	buffer.addUInt(hours, 2);
	buffer.add(':');
	buffer.addUInt(minutes, 2);
	buffer.add(':');
	buffer.addUInt(seconds, 2);

	return buffer.toString();
}

std::string Util::sha256(const std::string_view & text)
{
	// lowercase hex digits
	return picosha2::hash256_hex_string(text);
}

std::string Util::SHA256(const std::string_view & text)
{
	std::string result = sha256(text);

	// uppercase hex digits
	std::transform(result.begin(), result.end(), result.begin(), toupper);

	return result;
}
