#pragma once

#include <stddef.h>
#include <string>
#include <string_view>
#include <set>
#include <mutex>

class FileRedirector
{
	struct CryPak_Hook
	{
		const char *AdjustFileName(const char *src, char *dst, unsigned int flags, bool *pFoundInPak);
	};

	struct FastStringCompare
	{
		using is_transparent = void;

		bool operator()(const std::string_view & a, const std::string_view & b) const
		{
			return a < b;
		}
	};

	using TAdjustFileName = decltype(&CryPak_Hook::AdjustFileName);
	using TDownloadedMaps = std::set<std::string, FastStringCompare>;

	TAdjustFileName m_pOriginalAdjustFileName = nullptr;
	TDownloadedMaps m_downloadedMaps;
	std::string m_downloadedMapsPrefix;
	std::mutex m_mutex;

	void HackCryPak();
	const char *RedirectPath(const char *path, char *buffer, size_t bufferSize);
	std::string RedirectDownloadedMap(const std::string_view & path);
	std::string SanitizePath(const std::string_view & path);

public:
	FileRedirector();
	~FileRedirector();

	void AddDownloadedMap(const std::string_view & mapName);
	void SetDownloadedMapsPrefix(const std::string_view & prefix);
};
