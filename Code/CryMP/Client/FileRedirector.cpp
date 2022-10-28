#include <ctype.h>
#include <algorithm>

#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CrySystem/ICryPak.h"
#include "Library/WinAPI.h"

#include "FileRedirector.h"

namespace
{
	FileRedirector *g_self;
}

////////////////////////////////////
// FileRedirector::DownloadedMaps //
////////////////////////////////////

void FileRedirector::DownloadedMaps::Add(const std::string_view & map)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_maps.emplace(g_self->SanitizePath(map));
}

void FileRedirector::DownloadedMaps::SetDownloadPath(const std::string_view & path)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_downloadPath = g_self->AdjustPath(path, ICryPak::FLAGS_ADD_TRAILING_SLASH);
}

bool FileRedirector::DownloadedMaps::Redirect(const std::string_view & path, std::string & result)
{
	constexpr std::string_view PATH_PREFIX = "game\\levels\\";

	if (!Util::StartsWith(path, PATH_PREFIX))
	{
		// not a map path
		return false;
	}

	const std::string_view mapPath = Util::RemovePrefix(path, PATH_PREFIX.length());

	std::lock_guard<std::mutex> lock(m_mutex);

	for (size_t pos = mapPath.find('\\'); pos != std::string_view::npos; pos = mapPath.find('\\', pos + 1))
	{
		const std::string_view mapName = Util::RemoveSuffix(mapPath, mapPath.length() - pos);

		if (m_maps.find(mapName) != m_maps.end())
		{
			result.reserve(m_downloadPath.length() + mapPath.length());

			result = m_downloadPath;
			result += mapPath;

			return true;
		}
	}

	// not a downloaded map
	return false;
}

////////////////////
// FileRedirector //
////////////////////

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
	if (m_downloadedMaps.Redirect(oldPath, newPath))
	{
		CryLog("[CryMP] [FileRedirector] Redirecting '%s' to '%s'", path, newPath.c_str());

		path = Util::CopyToBuffer(buffer, bufferSize, newPath);
	}

	return path;
}

FileRedirector::FileRedirector()
{
	if (!g_self)
	{
		HackCryPak();
	}

	g_self = this;
}

FileRedirector::~FileRedirector()
{
}

std::string FileRedirector::SanitizePath(const std::string_view & path)
{
	std::string result(path);

	std::transform(result.begin(), result.end(), result.begin(), [](char ch) -> char
	{
		if (ch == '/')
			return '\\';
		else
			return tolower(ch);
	});

	return result;
}

std::string FileRedirector::AdjustPath(const std::string_view & path, unsigned int flags)
{
	std::string result(path);

	CryPak_Hook *pCryPak = reinterpret_cast<CryPak_Hook*>(gEnv->pCryPak);

	char buffer[512];
	if ((pCryPak->*m_pOriginalAdjustFileName)(result.c_str(), buffer, flags, nullptr) != result.c_str())
	{
		result = buffer;
	}

	return result;
}
