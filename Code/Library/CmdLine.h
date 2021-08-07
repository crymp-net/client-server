#pragma once

#include <string>
#include <string_view>

namespace CmdLine
{
	bool HasArg(const std::string_view & arg);

	std::string GetArgValue(const std::string_view & arg, const std::string_view & defaultValue = std::string_view());
	int GetArgValueInt(const std::string_view & arg, int defaultValue = 0);
}
