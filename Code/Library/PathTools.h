#pragma once

#include <string>
#include <string_view>
#include <tuple>

namespace PathTools
{
	inline bool IsSlash(char ch)
	{
		return ch == '/' || ch == '\\';
	}

	bool IsAbsolutePath(std::string_view path);
	bool StartsWith(std::string_view path, std::string_view prefix);

	std::string_view RemoveLeadingSlashes(std::string_view path);
	std::string_view RemoveTrailingSlashes(std::string_view path);

	std::string_view DirPath(std::string_view path);
	std::string_view FileName(std::string_view path);

	std::string_view PathStem(std::string_view path);
	std::string_view PathExtension(std::string_view path);

	std::tuple<std::string_view, std::string_view> SplitFirstPathComponent(std::string_view path);
	std::tuple<std::string_view, std::string_view> SplitPathIntoDirAndFile(std::string_view path);
	std::tuple<std::string_view, std::string_view> SplitNameIntoStemAndExtension(std::string_view name);
	std::tuple<std::string_view, std::string_view> SplitPathIntoStemAndExtension(std::string_view path);

	std::string Join(std::string_view pathA, std::string_view pathB);

	void AddTrailingSlash(std::string& path);
	void Normalize(std::string& path);
}
