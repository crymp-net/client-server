#include "CryCommon/CrySystem/ISystem.h"
#include "Library/StringTools.h"
#include "Library/Util.h"

#include "MapExtractor.h"

const char* MapExtractor::GetErrorString()
{
	return mz_zip_get_error_string(mz_zip_peek_last_error(&m_zip));
}

std::string MapExtractor::GetFileName(unsigned int index)
{
	char buffer[512];
	if (!mz_zip_reader_get_filename(&m_zip, index, buffer, sizeof buffer))
	{
		throw StringTools::ErrorFormat("ZIP get filename: %s", GetErrorString());
	}

	// TODO: normalize the path

	return buffer;
}

void MapExtractor::ExtractFile(unsigned int index)
{
	std::filesystem::path filePath = GetFileName(index);

	// skip files outside "levels/multiplayer/ps/mymap/"
	if (!Util::PathStartsWith(filePath, m_mapPath))
	{
		CryLogAlways("$6[CryMP] [MapExtractor] Ignoring '%s'", filePath.string().c_str());
		return;
	}

	filePath = m_dirPath / filePath;

	std::filesystem::create_directories(filePath.parent_path());

	if (!mz_zip_reader_extract_to_file(&m_zip, index, filePath.string().c_str(), 0))
	{
		throw StringTools::ErrorFormat("ZIP extract: %s", GetErrorString());
	}
}

MapExtractor::MapExtractor(const std::filesystem::path & zipPath, const std::filesystem::path & mapName)
{
	mz_zip_zero_struct(&m_zip);

	if (!mz_zip_reader_init_file(&m_zip, zipPath.string().c_str(), 0))
	{
		throw StringTools::ErrorFormat("ZIP open: %s", GetErrorString());
	}

	m_dirPath = zipPath.parent_path();
	m_mapPath = "Levels" / mapName;

	const std::filesystem::path mapDirPath = m_dirPath / m_mapPath;

	if (std::filesystem::exists(mapDirPath))
	{
		std::filesystem::remove_all(mapDirPath);
	}
}

MapExtractor::~MapExtractor()
{
	mz_zip_reader_end(&m_zip);
}

void MapExtractor::Extract()
{
	const unsigned int fileCount = mz_zip_reader_get_num_files(&m_zip);

	for (unsigned int index = 0; index < fileCount; index++)
	{
		if (!mz_zip_reader_is_file_a_directory(&m_zip, index))
		{
			ExtractFile(index);
		}
	}
}
