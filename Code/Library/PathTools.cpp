#include "PathTools.h"

bool PathTools::IsAbsolutePath(std::string_view path)
{
	if (path.length() > 0 && IsSlash(path[0]))
	{
		// also handles UNC paths
		return true;
	}

	if (path.length() > 1 && path[1] == ':')
	{
		const char drive = path[0];

		if ((drive >= 'A' && drive <= 'Z') || (drive >= 'a' && drive <= 'z'))
		{
			return true;
		}
	}

	return false;
}

std::string_view PathTools::RemoveLeadingSlashes(std::string_view path)
{
	while (!path.empty() && IsSlash(path.front()))
	{
		path.remove_prefix(1);
	}

	return path;
}

std::string_view PathTools::RemoveTrailingSlashes(std::string_view path)
{
	while (!path.empty() && IsSlash(path.back()))
	{
		path.remove_suffix(1);
	}

	return path;
}

std::string_view PathTools::DirPath(std::string_view path)
{
	path = RemoveTrailingSlashes(path);

	while (!path.empty() && !IsSlash(path.back()))
	{
		path.remove_suffix(1);
	}

	return RemoveTrailingSlashes(path);
}

std::string_view PathTools::FileName(std::string_view path)
{
	path = RemoveTrailingSlashes(path);

	const auto pos = path.find_last_of("/\\");
	if (pos != std::string_view::npos)
	{
		path.remove_prefix(pos + 1);
	}

	return path;
}

std::string_view PathTools::PathStem(std::string_view path)
{
	path = FileName(path);

	const auto pos = path.rfind('.');
	if (pos != std::string_view::npos && pos != 0)
	{
		path.remove_suffix(path.length() - pos);
	}

	return path;
}

std::string_view PathTools::PathExtension(std::string_view path)
{
	path = FileName(path);

	const auto pos = path.rfind('.');
	if (pos != std::string_view::npos && pos != 0)
	{
		path.remove_prefix(pos);
	}
	else
	{
		path.remove_prefix(path.length());
	}

	return path;
}

std::tuple<std::string_view, std::string_view> PathTools::SplitFirstPathComponent(std::string_view path)
{
	path = RemoveLeadingSlashes(path);

	std::string_view remaining;

	const auto pos = path.find_first_of("/\\");
	if (pos != std::string_view::npos)
	{
		remaining = path;
		remaining.remove_prefix(pos + 1);
		path.remove_suffix(path.length() - pos);
	}

	return {path, remaining};
}

std::tuple<std::string_view, std::string_view> PathTools::SplitPathIntoDirAndFile(std::string_view path)
{
	std::string_view dirPath = RemoveTrailingSlashes(path);
	std::string_view fileName = dirPath;

	while (!dirPath.empty() && !IsSlash(dirPath.back()))
	{
		dirPath.remove_suffix(1);
	}

	fileName.remove_prefix(dirPath.length());

	dirPath = RemoveTrailingSlashes(dirPath);

	return {dirPath, fileName};
}

std::tuple<std::string_view, std::string_view> PathTools::SplitNameIntoStemAndExtension(std::string_view name)
{
	std::string_view stem = name;
	std::string_view extension;

	const auto pos = stem.rfind('.');
	if (pos != std::string_view::npos && pos != 0)
	{
		extension = stem;
		extension.remove_prefix(pos);
		stem.remove_suffix(stem.length() - pos);
	}

	return {stem, extension};
}

std::tuple<std::string_view, std::string_view> PathTools::SplitPathIntoStemAndExtension(std::string_view path)
{
	return SplitNameIntoStemAndExtension(FileName(path));
}

void PathTools::AddTrailingSlash(std::string& path)
{
	if (!path.empty() && !IsSlash(path.back()))
	{
		path += '/';
	}
}
