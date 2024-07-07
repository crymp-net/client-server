#include "CrySystem/CryLog.h"

#include "ZipPak.h"
#include "ZipPakFile.h"

std::unique_ptr<ZipPak> ZipPak::TryOpen(const std::string& adjustedName)
{
	std::unique_ptr<ZipPak> pak = std::make_unique<ZipPak>();

	if (!mz_zip_reader_init_file(&pak->m_zip, adjustedName.c_str(), MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY))
	{
		pak->LogError(__FUNCTION__);
		return {};
	}

	return pak;
}

////////////////////////////////////////////////////////////////////////////////
// IPak
////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<IPakFile> ZipPak::OpenFile(std::uint32_t index, bool isBinary)
{
	std::size_t file_size = 0;
	void* file_data = mz_zip_reader_extract_to_heap(&m_zip, index, &file_size, 0);
	if (!file_data)
	{
		this->LogError(__FUNCTION__);
		return {};
	}

	mz_zip_archive_file_stat stat = {};
	if (mz_zip_reader_file_stat(&m_zip, index, &stat) != MZ_TRUE)
	{
		this->LogError(__FUNCTION__);
		return {};
	}

	const std::uint64_t modificationTime = stat.m_time;

	return std::make_unique<ZipPakFile>(file_data, file_size, modificationTime, isBinary);
}

std::uint32_t ZipPak::GetEntryCount()
{
	return mz_zip_reader_get_num_files(&m_zip);
}

bool ZipPak::IsDirectoryEntry(std::uint32_t index)
{
	return mz_zip_reader_is_file_a_directory(&m_zip, index) == MZ_TRUE;
}

bool ZipPak::GetEntryPath(std::uint32_t index, std::string& path)
{
	char buffer[256];
	unsigned int byteCount = mz_zip_reader_get_filename(&m_zip, index, buffer, sizeof(buffer));
	if (byteCount == 0)
	{
		return false;
	}

	path.assign(buffer, byteCount - 1);

	return true;
}

bool ZipPak::GetEntrySize(std::uint32_t index, std::uint64_t& size)
{
	mz_zip_archive_file_stat stat = {};
	if (mz_zip_reader_file_stat(&m_zip, index, &stat) != MZ_TRUE)
	{
		return false;
	}

	size = stat.m_uncomp_size;

	return true;
}

////////////////////////////////////////////////////////////////////////////////

void ZipPak::LogError(const char* function)
{
	const char* error = mz_zip_get_error_string(mz_zip_peek_last_error(&m_zip));

	CryLogAlways("%s: %s", function, error);
}
