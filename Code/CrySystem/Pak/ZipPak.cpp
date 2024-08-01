#include "CrySystem/CryLog.h"

#include "FileInZipPak.h"
#include "ZipPak.h"

ZipPak::~ZipPak()
{
	mz_zip_end(&m_zip);
}

std::unique_ptr<ZipPak> ZipPak::OpenFileHandle(std::FILE* file)
{
	auto pak = std::make_unique<ZipPak>();

	if (!mz_zip_reader_init_cfile(&pak->m_zip, file, 0, MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY))
	{
		CryLogErrorAlways("%s: %s", __FUNCTION__, pak->GetCurrentErrorString());
		return {};
	}

	return pak;
}

std::unique_ptr<ZipPak> ZipPak::OpenFileName(const std::string& adjustedName)
{
	auto pak = std::make_unique<ZipPak>();

	if (!mz_zip_reader_init_file(&pak->m_zip, adjustedName.c_str(), MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY))
	{
		CryLogErrorAlways("%s: %s", __FUNCTION__, pak->GetCurrentErrorString());
		return {};
	}

	return pak;
}

std::unique_ptr<ZipPak> ZipPak::OpenMemory(const void* mem, std::size_t size)
{
	auto pak = std::make_unique<ZipPak>();

	if (!mz_zip_reader_init_mem(&pak->m_zip, mem, size, MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY))
	{
		CryLogErrorAlways("%s: %s", __FUNCTION__, pak->GetCurrentErrorString());
		return {};
	}

	return pak;
}

////////////////////////////////////////////////////////////////////////////////
// IPak
////////////////////////////////////////////////////////////////////////////////

std::uint32_t ZipPak::GetEntryCount()
{
	return mz_zip_reader_get_num_files(&m_zip);
}

bool ZipPak::IsDirectoryEntry(std::uint32_t index)
{
	return mz_zip_reader_is_file_a_directory(&m_zip, index) == MZ_TRUE;
}

bool ZipPak::GetEntryPathAndSize(std::uint32_t index, std::string& path, std::uint64_t& size)
{
	mz_zip_archive_file_stat stat;
	if (!this->GetEntryStat(index, stat))
	{
		return false;
	}

	path = stat.m_filename;
	size = stat.m_uncomp_size;

	return true;
}

std::unique_ptr<IFileInPak> ZipPak::OpenFile(std::uint32_t index, bool isBinary)
{
	ZipCache::DecompressedEntry* entry = m_cache.Acquire(index);
	if (!entry)
	{
		ZipCache::DecompressedEntry newEntry;
		if (!this->DecompressEntry(index, newEntry))
		{
			return {};
		}

		entry = m_cache.AddAndAcquire(std::move(newEntry));
	}

	auto file = std::make_unique<FileInZipPak>();
	file->data = entry->ptr.get();
	file->size = entry->size;
	file->modificationTime = entry->modificationTime;
	file->isBinary = isBinary;

	return file;
}

void ZipPak::OnFileClosed(std::uint32_t index)
{
	m_cache.Release(index);
}

std::size_t ZipPak::GetCachedDataSize() const
{
	return m_cache.GetTotalEntrySize();
}

////////////////////////////////////////////////////////////////////////////////

const char* ZipPak::GetCurrentErrorString()
{
	return mz_zip_get_error_string(mz_zip_peek_last_error(&m_zip));
}

bool ZipPak::GetEntryStat(std::uint32_t index, mz_zip_archive_file_stat& stat)
{
	if (!mz_zip_reader_file_stat(&m_zip, index, &stat))
	{
		CryLogErrorAlways("%s(%u): %s", __FUNCTION__, index, this->GetCurrentErrorString());
		return false;
	}

	return true;
}

bool ZipPak::DecompressEntry(std::uint32_t index, ZipCache::DecompressedEntry& entry)
{
	mz_zip_archive_file_stat stat;
	if (!this->GetEntryStat(index, stat))
	{
		return false;
	}

	if (stat.m_uncomp_size > 0x7fffffff)
	{
		CryLogErrorAlways("%s(%u): Too big file \"%s\"", __FUNCTION__, index, stat.m_filename);
		return false;
	}

	entry.index = index;
	entry.size = static_cast<std::size_t>(stat.m_uncomp_size);
	entry.modificationTime = stat.m_time;

	entry.ptr = std::make_unique<std::byte[]>(entry.size);
	if (!mz_zip_reader_extract_to_mem(&m_zip, index, entry.ptr.get(), entry.size, 0))
	{
		CryLogErrorAlways("%s(%u): %s", __FUNCTION__, index, this->GetCurrentErrorString());
		return false;
	}

	return true;
}
