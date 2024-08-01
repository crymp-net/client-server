#pragma once

#include <algorithm>
#include <cstdint>
#include <limits>
#include <vector>

using SlotVectorIndex = std::uint16_t;

class SlotVectorSerial
{
	std::uint16_t m_value = 0;

public:
	SlotVectorSerial() = default;

	explicit SlotVectorSerial(std::uint16_t value) : m_value(value)
	{
	}

	std::uint16_t GetValue() const
	{
		return m_value;
	}

	void Increment()
	{
		// a valid serial is never zero to make sure a valid handle is never zero as well
		// zero handle is used as an invalid handle
		m_value += 1;
		m_value += (m_value == 0);
	}

	friend bool operator==(const SlotVectorSerial& a, const SlotVectorSerial& b)
	{
		return a.m_value == b.m_value;
	}
};

class SlotVectorHandle
{
	std::uint32_t m_value = 0;

public:
	SlotVectorHandle() = default;

	explicit SlotVectorHandle(std::uint32_t value) : m_value(value)
	{
	}

	explicit SlotVectorHandle(SlotVectorIndex index, SlotVectorSerial serial)
	: m_value((static_cast<std::uint32_t>(index) << 16) | serial.GetValue())
	{
	}

	SlotVectorIndex GetIndex() const
	{
		return static_cast<SlotVectorIndex>(m_value >> 16);
	}

	SlotVectorSerial GetSerial() const
	{
		return SlotVectorSerial(static_cast<std::uint16_t>(m_value & 0xffff));
	}

	std::uint32_t GetValue() const
	{
		return m_value;
	}

	bool IsValid() const
	{
		return m_value != 0;
	}

	explicit operator bool() const
	{
		return this->IsValid();
	}

	friend bool operator==(const SlotVectorHandle& a, const SlotVectorHandle& b)
	{
		return a.m_value == b.m_value;
	}
};

template<class T>
class SlotVector
{
	std::vector<T> m_slots;

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
			if (m_slots.size() >= std::numeric_limits<SlotVectorIndex>::max())
			{
				return nullptr;
			}

			slot = &m_slots.emplace_back();
		}
		else
		{
			slot = &(*it);
		}

		slot->serial.Increment();

		return slot;
	}

	SlotVectorHandle SlotToHandle(T* slot)
	{
		if (!slot)
		{
			return {};
		}

		return SlotVectorHandle(static_cast<SlotVectorIndex>(slot - &m_slots[0]), slot->serial);
	}

	T* HandleToSlot(SlotVectorHandle handle)
	{
		const SlotVectorIndex index = handle.GetIndex();

		if (index >= m_slots.size())
		{
			return nullptr;
		}

		T* slot = &m_slots[index];

		if (slot->empty() || slot->serial != handle.GetSerial())
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

	SlotVectorIndex GetActiveCount()
	{
		return static_cast<SlotVectorIndex>(
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
