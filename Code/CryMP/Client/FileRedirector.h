#pragma once

#include <stddef.h>
#include <string>
#include <string_view>
#include <set>
#include <mutex>

#include "Library/Util.h"

class FileRedirector
{
public:
	class DownloadedMaps
	{
		std::set<std::string, Util::TransparentStringCompare> m_maps;
		std::string m_downloadPath;
		std::mutex m_mutex;

	public:
		void Add(const std::string_view & map);
		void SetDownloadPath(const std::string_view & path);
		bool Redirect(const std::string_view & path, std::string & result);
	};

private:
	struct CryPak_Hook
	{
		const char *AdjustFileName(const char *src, char *dst, unsigned int flags, bool *pFoundInPak);
	};

	using TAdjustFileName = decltype(&CryPak_Hook::AdjustFileName);

	TAdjustFileName m_pOriginalAdjustFileName = nullptr;
	DownloadedMaps m_downloadedMaps;

	void HackCryPak();
	const char *RedirectPath(const char *path, char *buffer, size_t bufferSize);

public:
	FileRedirector();
	~FileRedirector();

	std::string SanitizePath(const std::string_view & path);
	std::string AdjustPath(const std::string_view & path, unsigned int flags = 0);

	DownloadedMaps & GetDownloadedMaps()
	{
		return m_downloadedMaps;
	}
};
