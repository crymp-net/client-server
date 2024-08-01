#pragma once

#include <bit>
#include <bitset>
#include <cstddef>
#include <memory>

template<std::size_t ARENA_SIZE_BYTES>
class BuddyAllocator
{
	static_assert(std::has_single_bit(ARENA_SIZE_BYTES));  // make sure ARENA_SIZE_BYTES is a power of two

	static constexpr std::size_t SMALLEST_CHUNK_SIZE_BYTES = 16;

	static constexpr std::size_t GetTreeOrder()
	{
		constexpr std::size_t maxChunkCount = ARENA_SIZE_BYTES / SMALLEST_CHUNK_SIZE_BYTES;

		return (sizeof(std::size_t) * 8) - std::countl_zero(std::bit_ceil(maxChunkCount));
	}

	static constexpr std::size_t GetTreeSize()
	{
		std::size_t result = 0;
		std::size_t multiplier = 1;
		for (std::size_t order = GetTreeOrder(); order > 0; order--)
		{
			result += order * multiplier;
			multiplier *= 2;
		}

		return result;
	}

	static constexpr std::size_t GetTreeUpperPosBound()
	{
		return 1 << GetTreeOrder();
	}

	std::unique_ptr<std::byte[]> m_arena = std::make_unique<std::byte[]>(ARENA_SIZE_BYTES);
	std::bitset<GetTreeSize()> m_tree;

public:
	void* Allocate(std::size_t bytes)
	{
		// TODO
		return nullptr;
	}

	bool Deallocate(void* pointer)
	{
		std::byte* chunk = static_cast<std::byte*>(pointer);

		if (chunk < m_arena.get() || chunk >= (m_arena.get() + ARENA_SIZE_BYTES))
		{
			return false;
		}

		// TODO

		return true;
	}
};
