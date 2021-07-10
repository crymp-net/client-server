#include <ctype.h>
#include <algorithm>

#include "CryCommon/CrySystem/ISystem.h"
#include "Library/Util.h"
#include "Library/WinAPI.h"

#include "FileRedirector.h"

namespace
{
	constexpr char SEPARATOR = '\\';
	constexpr std::string_view PREFIX_MAPS = "game\\levels\\";

	FileRedirector *g_self;
}

const char *FileRedirector::CryPak_Hook::AdjustFileName(const char *src, char *dst, unsigned int flags, bool *pFoundInPak)
{
	const char *result = (this->*(g_self->m_pOriginalAdjustFileName))(src, dst, flags, pFoundInPak);

	// hopefully all path buffers are at least this size
	constexpr size_t dstSize = 260;

	return g_self->RedirectPath(result, dst, dstSize);
}

void FileRedirector::HackCryPak()
{
	void **CryPakVTable = *reinterpret_cast<void***>(gEnv->pCryPak);

	m_pOriginalAdjustFileName = reinterpret_cast<TAdjustFileName&>(CryPakVTable[0]);

	// vtable hook
	TAdjustFileName newAdjustFileName = &CryPak_Hook::AdjustFileName;
	WinAPI::FillMem(&CryPakVTable[0], &reinterpret_cast<void*&>(newAdjustFileName), sizeof (void*));
}

const char *FileRedirector::RedirectPath(const char *path, char *buffer, size_t bufferSize)
{
	const std::string_view oldPath = path;
	std::string newPath;

	if (Util::StartsWith(PREFIX_MAPS, oldPath))
	{
		newPath = RedirectDownloadedMap(Util::RemovePrefix(oldPath, PREFIX_MAPS.length()));
	}

	if (!newPath.empty())
	{
		CryLog("[CryMP] [FileRedirector] Redirecting '%s' to '%s'", path, newPath.c_str());

		path = Util::CopyToBuffer(newPath, buffer, bufferSize);
	}

	return path;
}

std::string FileRedirector::RedirectDownloadedMap(const std::string_view & path)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	for (size_t pos = path.find(SEPARATOR); pos != std::string_view::npos; pos = path.find(SEPARATOR, pos + 1))
	{
		const std::string_view mapName = Util::RemoveSuffix(path, path.length() - pos);

		if (m_downloadedMaps.count(mapName) > 0)
		{
			std::string result;
			result.reserve(m_downloadedMapsPrefix.length() + path.length());

			result += m_downloadedMapsPrefix;
			result += path;

			// redirect path
			return result;
		}
	}

	// no redirect
	return std::string();
}

std::string FileRedirector::SanitizePath(const std::string_view & path)
{
	std::string result(path);

	std::transform(result.begin(), result.end(), result.begin(), [](char ch) -> char
	{
		if (ch == '/')
			return SEPARATOR;
		else
			return tolower(ch);
	});

	return result;
}

FileRedirector::FileRedirector()
{
	g_self = this;

	HackCryPak();
}

FileRedirector::~FileRedirector()
{
}

void FileRedirector::AddDownloadedMap(const std::string_view & mapName)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_downloadedMaps.emplace(SanitizePath(mapName));
}

void FileRedirector::SetDownloadedMapsPrefix(const std::string_view & prefix)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_downloadedMapsPrefix = SanitizePath(prefix);

	// make sure the trailing slash is there
	if (!m_downloadedMapsPrefix.empty() && m_downloadedMapsPrefix.back() != SEPARATOR)
	{
		m_downloadedMapsPrefix += SEPARATOR;
	}
}
