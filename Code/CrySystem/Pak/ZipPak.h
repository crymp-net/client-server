#pragma once

#include <cstdio>
#include <string>

#include <miniz.h>

#include "IPak.h"

struct ZipPak final : public IPak
{
	mz_zip_archive zip = {};

	ZipPak() = default;
	ZipPak(const ZipPak&) = delete;
	ZipPak& operator=(const ZipPak&) = delete;

	~ZipPak()
	{
		mz_zip_end(&this->zip);
	}

	static std::unique_ptr<ZipPak> OpenFileHandle(std::FILE* file);
	static std::unique_ptr<ZipPak> OpenFileName(const std::string& adjustedName);
	static std::unique_ptr<ZipPak> OpenMemory(const void* mem, std::size_t size);

	////////////////////////////////////////////////////////////////////////////////
	// IPak
	////////////////////////////////////////////////////////////////////////////////

	std::unique_ptr<IFileInPak> OpenFile(std::uint32_t index, bool isBinary) override;

	std::uint32_t GetEntryCount() override;
	bool IsDirectoryEntry(std::uint32_t index) override;
	bool GetEntryPath(std::uint32_t index, std::string& path) override;
	bool GetEntrySize(std::uint32_t index, std::uint64_t& size) override;

	////////////////////////////////////////////////////////////////////////////////

private:
	void LogZipError(const char* function);
};
