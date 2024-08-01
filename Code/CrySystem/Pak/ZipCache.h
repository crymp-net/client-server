#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

class ZipCache
{
public:
	struct DecompressedEntry
	{
		std::uint32_t index = 0;
		std::int32_t refCount = 0;
		std::size_t size = 0;
		std::unique_ptr<std::byte[]> ptr;
		std::uint64_t modificationTime = 0;
	};

private:
	std::vector<DecompressedEntry> m_entries;
	std::vector<std::uint32_t> m_inactiveIndexes;

	static constexpr std::size_t MAX_INACTIVE_COUNT = 8;

public:
	DecompressedEntry* AddAndAcquire(DecompressedEntry&& entry);
	DecompressedEntry* Acquire(std::uint32_t index);
	void Release(std::uint32_t index);

	std::size_t GetTotalEntrySize() const;

private:
	std::vector<DecompressedEntry>::iterator FindEntry(std::uint32_t index);
	void DropEntry(std::uint32_t index);
};
