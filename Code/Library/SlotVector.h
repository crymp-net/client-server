#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>

using SlotVectorHandle = std::uint32_t;
using SlotVectorSerial = std::uint16_t;

template<class T>
class SlotVector
{
	std::vector<T> m_slots;

	SlotVectorSerial IncrementSerial(SlotVectorSerial serial)
	{
		// serial is never zero to make sure a valid handle is never zero as well
		// zero handle can be used as an invalid handle
		serial += 1;
		serial += (serial == 0);
		return serial;
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

		slot->serial = this->IncrementSerial(slot->serial);

		return slot;
	}

	SlotVectorHandle SlotToHandle(T* slot)
	{
		const SlotVectorHandle index = static_cast<SlotVectorHandle>(slot - &m_slots[0]);

		return (index << 16) | slot->serial;
	}

	T* HandleToSlot(SlotVectorHandle handle)
	{
		const SlotVectorHandle index = handle >> 16;

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

	std::uint16_t GetActiveCount()
	{
		// index is std::uint16_t
		return static_cast<std::uint16_t>(
			std::count_if(m_slots.begin(), m_slots.end(), [](const T& x) { return !x.empty(); })
		);
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
