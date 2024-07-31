#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

struct IFileInPak;

struct IPak
{
	virtual ~IPak() = default;

	virtual std::uint32_t GetEntryCount() = 0;
	virtual bool IsDirectoryEntry(std::uint32_t index) = 0;
	virtual bool GetEntryPathAndSize(std::uint32_t index, std::string& path, std::uint64_t& size) = 0;

	virtual std::unique_ptr<IFileInPak> OpenFile(std::uint32_t index, bool isBinary) = 0;
	virtual void OnFileClosed(std::uint32_t index) = 0;

	virtual std::size_t GetCachedDataSize() const = 0;
};
