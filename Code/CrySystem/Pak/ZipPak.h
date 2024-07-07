#pragma once

#include <cstdio>
#include <string>

#include <miniz.h>

#include "IPak.h"

class ZipPak final : public IPak
{
	mz_zip_archive m_zip = {};

public:
	ZipPak() = default;
	ZipPak(const ZipPak&) = delete;
	ZipPak& operator=(const ZipPak&) = delete;

	~ZipPak()
	{
		mz_zip_end(&m_zip);
	}

	static std::unique_ptr<ZipPak> TryOpen(const std::string& adjustedName);

	////////////////////////////////////////////////////////////////////////////////
	// IPak
	////////////////////////////////////////////////////////////////////////////////

	std::unique_ptr<IPakFile> OpenFile(std::uint32_t index, bool isBinary) override;

	std::uint32_t GetEntryCount() override;
	bool IsDirectoryEntry(std::uint32_t index) override;
	bool GetEntryPath(std::uint32_t index, std::string& path) override;
	bool GetEntrySize(std::uint32_t index, std::uint64_t& size) override;

	////////////////////////////////////////////////////////////////////////////////

protected:
	void LogError(const char* function);
};
