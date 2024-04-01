#pragma once

#include <cstdio>
#include <stdexcept>

#include <miniz.h>

#include "IPak.h"

class ZipPak final : public IPak
{
	std::FILE* m_file = nullptr;
	mz_zip_archive m_zip = {};

public:
	explicit ZipPak(std::FILE* file);
	ZipPak(const ZipPak&) = delete;
	ZipPak& operator=(const ZipPak&) = delete;
	~ZipPak();

	////////////////////////////////////////////////////////////////////////////////
	// IPak
	////////////////////////////////////////////////////////////////////////////////

	std::unique_ptr<IPakFile> OpenFile(std::uint32_t index, bool isBinary) override;

	std::uint32_t GetEntryCount() override;
	bool IsDirectoryEntry(std::uint32_t index) override;
	bool GetEntryPath(std::uint32_t index, std::string& path) override;
	bool GetEntrySize(std::uint32_t index, std::uint64_t& size) override;

	////////////////////////////////////////////////////////////////////////////////

private:
	void LogError(const char* function);
};
