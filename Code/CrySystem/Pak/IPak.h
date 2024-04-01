#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "IPakFile.h"

struct IPak
{
	virtual ~IPak() = default;

	virtual std::unique_ptr<IPakFile> OpenFile(std::uint32_t index, bool isBinary) = 0;

	virtual std::uint32_t GetEntryCount() = 0;
	virtual bool IsDirectoryEntry(std::uint32_t index) = 0;
	virtual bool GetEntryPath(std::uint32_t index, std::string& path) = 0;
	virtual bool GetEntrySize(std::uint32_t index, std::uint64_t& size) = 0;
};
