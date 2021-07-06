#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string>
#include <string_view>

namespace Util
{
	bool EqualNoCase(const std::string_view & stringA, const std::string_view & stringB);

	bool StartsWith(const std::string_view & prefix, const std::string_view & text);
	bool StartsWithNoCase(const std::string_view & prefix, const std::string_view & text);

	std::string MakeHumanReadableBytes(uint64_t bytes);

	std::string SecondsToString(uint64_t seconds);
}
