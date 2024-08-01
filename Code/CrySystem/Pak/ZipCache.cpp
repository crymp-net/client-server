#include <algorithm>

#include "ZipCache.h"

ZipCache::DecompressedEntry* ZipCache::AddAndAcquire(DecompressedEntry&& entry)
{
	DecompressedEntry& newEntry = m_entries.emplace_back(std::move(entry));
	newEntry.refCount = 1;

	return &newEntry;
}

ZipCache::DecompressedEntry* ZipCache::Acquire(std::uint32_t index)
{
	const auto it = this->FindEntry(index);
	if (it == m_entries.end())
	{
		return nullptr;
	}

	if (it->refCount <= 0)
	{
		// reactive
		std::erase(m_inactiveIndexes, index);
		it->refCount = 1;
	}
	else
	{
		it->refCount++;
	}

	return &(*it);
}

void ZipCache::Release(std::uint32_t index)
{
	const auto it = this->FindEntry(index);
	if (it == m_entries.end())
	{
		return;
	}

	if (it->refCount <= 0)
	{
		// already released
		return;
	}

	it->refCount--;

	if (it->refCount > 0)
	{
		// still active
		return;
	}

	if (m_inactiveIndexes.size() >= MAX_INACTIVE_COUNT && !m_inactiveIndexes.empty())
	{
		this->DropEntry(m_inactiveIndexes.back());
		m_inactiveIndexes.pop_back();
	}

	m_inactiveIndexes.insert(m_inactiveIndexes.begin(), index);
}

std::size_t ZipCache::GetTotalEntrySize() const
{
	std::size_t totalSize = 0;

	for (const auto& entry : m_entries)
	{
		totalSize += entry.size;
	}

	return totalSize;
}

std::vector<ZipCache::DecompressedEntry>::iterator ZipCache::FindEntry(std::uint32_t index)
{
	return std::find_if(m_entries.begin(), m_entries.end(), [index](const auto& x) { return x.index == index; });
}

void ZipCache::DropEntry(std::uint32_t index)
{
	const auto it = this->FindEntry(index);
	if (it == m_entries.end())
	{
		return;
	}

	if (std::distance(it, m_entries.end()) > 1)
	{
		*it = std::move(m_entries.back());
	}

	m_entries.pop_back();
}
