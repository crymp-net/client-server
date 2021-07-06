#include <ctype.h>
#include <array>
#include <algorithm>

#include "Util.h"
#include "Format.h"
#include "StringBuffer.h"

bool Util::EqualNoCase(const std::string_view & stringA, const std::string_view & stringB)
{
	return std::equal(stringA.begin(), stringA.end(), stringB.begin(), stringB.end(), [](char a, char b) -> bool
	{
		return tolower(a) == tolower(b);
	});
}

bool Util::StartsWith(const std::string_view & prefix, const std::string_view & text)
{
	if (prefix.length() <= text.length())
		return prefix == std::string_view(text.data(), prefix.length());
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
