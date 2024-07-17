#include "CrySystem/CryLog.h"

#include "FileInZipPak.h"
#include "ZipPak.h"

std::unique_ptr<ZipPak> ZipPak::OpenFileHandle(std::FILE* file)
{
	auto pak = std::make_unique<ZipPak>();

	if (!mz_zip_reader_init_cfile(&pak->zip, file, 0, MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY))
	{
		pak->LogError(__FUNCTION__);
		return {};
	}

	return pak;
}

std::unique_ptr<ZipPak> ZipPak::OpenFileName(const std::string& adjustedName)
{
	auto pak = std::make_unique<ZipPak>();

	if (!mz_zip_reader_init_file(&pak->zip, adjustedName.c_str(), MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY))
	{
		pak->LogError(__FUNCTION__);
		return {};
	}

	return pak;
}

std::unique_ptr<ZipPak> ZipPak::OpenMemory(const void* mem, std::size_t size)
{
	auto pak = std::make_unique<ZipPak>();

	if (!mz_zip_reader_init_mem(&pak->zip, mem, size, MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY))
	{
		pak->LogError(__FUNCTION__);
		return {};
	}

	return pak;
}

////////////////////////////////////////////////////////////////////////////////
// IPak
////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<IFileInPak> ZipPak::OpenFile(std::uint32_t index, bool isBinary)
{
	auto file = std::make_unique<FileInZipPak>();

	file->data = FileInZipPak::Data(mz_zip_reader_extract_to_heap(&this->zip, index, &file->size, 0));
	if (!file->data)
	{
		this->LogError(__FUNCTION__);
		return {};
	}

	mz_zip_archive_file_stat stat = {};
	if (mz_zip_reader_file_stat(&this->zip, index, &stat) != MZ_TRUE)
	{
		this->LogError(__FUNCTION__);
		return {};
	}

	file->modificationTime = stat.m_time;
	file->isBinary = isBinary;

	return file;
}

std::uint32_t ZipPak::GetEntryCount()
{
	return mz_zip_reader_get_num_files(&this->zip);
}

bool ZipPak::IsDirectoryEntry(std::uint32_t index)
{
	return mz_zip_reader_is_file_a_directory(&this->zip, index) == MZ_TRUE;
}

bool ZipPak::GetEntryPath(std::uint32_t index, std::string& path)
{
	char buffer[256];
	unsigned int byteCount = mz_zip_reader_get_filename(&this->zip, index, buffer, sizeof(buffer));
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
	if (mz_zip_reader_file_stat(&this->zip, index, &stat) != MZ_TRUE)
	{
		return false;
	}

	size = stat.m_uncomp_size;

	return true;
}

////////////////////////////////////////////////////////////////////////////////

void ZipPak::LogError(const char* function)
{
	const char* error = mz_zip_get_error_string(mz_zip_peek_last_error(&this->zip));

	CryLogAlways("%s: %s", function, error);
}
