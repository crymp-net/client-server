#pragma once

#include <cstdint>
#include <memory>
#include <string>

struct IFileInPak;

struct IPak
{
	virtual ~IPak() = default;

	virtual std::unique_ptr<IFileInPak> OpenFile(std::uint32_t index, bool isBinary) = 0;

	virtual std::uint32_t GetEntryCount() = 0;
	virtual bool IsDirectoryEntry(std::uint32_t index) = 0;
	virtual bool GetEntryPath(std::uint32_t index, std::string& path) = 0;
	virtual bool GetEntrySize(std::uint32_t index, std::uint64_t& size) = 0;
};
