#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>

template<class T>
class SlotVector
{
	std::vector<T> m_slots;

	void IncrementSerial(std::uint16_t& serial)
	{
		// serial is never zero to make sure a valid handle is never zero as well
		serial += 1;
		serial += (serial == 0);
	}

	T* SkipEmptySlots(T* slot)
	{
		T* end = m_slots.data() + m_slots.size();

		while (slot != end)
		{
			if (!slot->empty())
			{
				return slot;
			}

			slot++;
		}

		return nullptr;
	}

public:
	T* GetFreeSlot()
	{
		T* slot = nullptr;

		const auto it = std::find_if(m_slots.begin(), m_slots.end(), [](const T& x) { return x.empty(); });

		if (it == m_slots.end())
		{
			if (m_slots.size() >= 0xffff)
			{
				return nullptr;
			}

			slot = &m_slots.emplace_back();
		}
		else
		{
			slot = &(*it);
		}

		this->IncrementSerial(slot->serial);

		return slot;
	}

	std::uint32_t SlotToHandle(T* slot)
	{
		const std::uint32_t index = static_cast<std::uint32_t>(slot - &m_slots[0]);

		return (index << 16) | slot->serial;
	}

	T* HandleToSlot(std::uint32_t handle)
	{
		const std::uint32_t index = handle >> 16;

		if (index >= m_slots.size())
		{
			return nullptr;
		}

		T* slot = &m_slots[index];

		if (slot->empty() || slot->serial != (handle & 0xffff))
		{
			return nullptr;
		}

		return slot;
	}

	template<class UnaryPredicate>
	T* Find(UnaryPredicate predicate)
	{
		const auto it = std::find_if(m_slots.begin(), m_slots.end(), [&predicate](const T& x) {
			return !x.empty() && predicate(x);
		});

		if (it == m_slots.end())
		{
			return nullptr;
		}

		return &(*it);
	}

	std::uint32_t GetActiveCount()
	{
		return std::count_if(m_slots.begin(), m_slots.end(), [](const T& x) { return !x.empty(); });
	}

	T* GetFirstActive()
	{
		return this->SkipEmptySlots(m_slots.data());
	}

	T* GetNextActive(T* slot)
	{
		slot++;
		return this->SkipEmptySlots(slot);
	}
};
