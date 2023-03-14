#pragma once

#include <concepts>
#include <cstring>
#include <iterator>
#include <string_view>

// TODO: use C++20 modules once CMake supports them
#include <fmt/core.h>

#ifdef _MSC_VER
#define FAST_STRING_NOINLINE __declspec(noinline)
#else
#define FAST_STRING_NOINLINE __attribute__((__noinline__))
#endif

template<std::integral T>
class BasicFastString
{
public:
	using value_type = T;
	using size_type = std::size_t;
	using view_type = std::basic_string_view<T>;

	using pointer = value_type*;
	using reference = value_type&;
	using const_pointer = const value_type*;
	using const_reference = const value_type&;

	using iterator = pointer;
	using const_iterator = const_pointer;

	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
	static constexpr size_type LOCAL_CAPACITY = 95;

	value_type* m_data = nullptr;
	size_type m_length = 0;

	union
	{
		value_type m_local_buffer[LOCAL_CAPACITY + 1];
		size_type m_heap_capacity;
	};

public:
	BasicFastString() : m_data(m_local_buffer), m_heap_capacity(0)  // fast null-termination of m_local_buffer
	{
	}

	explicit BasicFastString(view_type view) : m_length(view.length())
	{
		if (view.length() <= LOCAL_CAPACITY)
		{
			m_data = m_local_buffer;
		}
		else
		{
			const size_type new_capacity = this->round_up_capacity(view.length());

			m_data = this->allocate(new_capacity);
			m_heap_capacity = new_capacity;
		}

		std::memcpy(m_data, view.data(), view.length() * sizeof(value_type));
		m_data[view.length()] = 0;
	}

	BasicFastString(const BasicFastString& other) : m_length(other.m_length)
	{
		if (other.m_data == other.m_local_buffer)
		{
			m_data = m_local_buffer;
			std::memcpy(m_local_buffer, other.m_local_buffer, sizeof(m_local_buffer));
		}
		else
		{
			m_data = this->allocate(other.m_heap_capacity);
			m_heap_capacity = other.m_heap_capacity;
			std::memcpy(m_data, other.m_data, (other.m_length + 1) * sizeof(value_type));
		}
	}

	BasicFastString(BasicFastString&& other) noexcept : m_length(other.m_length)
	{
		if (other.m_data == other.m_local_buffer)
		{
			m_data = m_local_buffer;
			std::memcpy(m_local_buffer, other.m_local_buffer, sizeof(m_local_buffer));
		}
		else
		{
			m_data = other.m_data;
			m_heap_capacity = other.m_heap_capacity;
		}

		other.m_data = other.m_local_buffer;
		other.m_length = 0;
		other.m_heap_capacity = 0;
	}

	BasicFastString& operator=(view_type view)
	{
		// supports self-assignment
		this->assign(view);

		return *this;
	}

	BasicFastString& operator=(const BasicFastString& other)
	{
		// supports self-assignment
		this->assign(other);

		return *this;
	}

	BasicFastString& operator=(BasicFastString&& other) noexcept
	{
		if (m_data != m_local_buffer)
		{
			this->deallocate(m_data);
			// complete initialization is required to support self-assignment
			m_data = m_local_buffer;
			m_heap_capacity = 0;
		}

		m_length = other.m_length;

		if (other.m_data == other.m_local_buffer)
		{
			// memmove instead of memcpy to support self-assignment
			std::memmove(m_local_buffer, other.m_local_buffer, sizeof(m_local_buffer));
		}
		else
		{
			m_data = other.m_data;
			m_heap_capacity = other.m_heap_capacity;
		}

		other.m_data = other.m_local_buffer;
		other.m_length = 0;
		other.m_heap_capacity = 0;

		return *this;
	}

	~BasicFastString()
	{
		if (m_data != m_local_buffer)
		{
			this->deallocate(m_data);
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	// Size
	////////////////////////////////////////////////////////////////////////////////

	size_type capacity() const
	{
		return (m_data == m_local_buffer) ? LOCAL_CAPACITY : m_heap_capacity;
	}

	size_type length() const
	{
		return m_length;
	}

	size_type size() const
	{
		return m_length;
	}

	bool empty() const
	{
		return m_length == 0;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Access
	////////////////////////////////////////////////////////////////////////////////

	value_type* data()
	{
		return m_data;
	}

	const value_type* data() const
	{
		return m_data;
	}

	const value_type* c_str() const
	{
		return m_data;
	}

	value_type& front()
	{
		return m_data[0];
	}

	const value_type& front() const
	{
		return m_data[0];
	}

	value_type& back()
	{
		return m_data[m_length - 1];
	}

	const value_type& back() const
	{
		return m_data[m_length - 1];
	}

	value_type& operator[](size_type index)
	{
		return m_data[index];
	}

	const value_type& operator[](size_type index) const
	{
		return m_data[index];
	}

	////////////////////////////////////////////////////////////////////////////////
	// Iterator
	////////////////////////////////////////////////////////////////////////////////

	iterator begin()
	{
		return m_data;
	}

	const_iterator begin() const
	{
		return m_data;
	}

	const_iterator cbegin() const
	{
		return m_data;
	}

	iterator end()
	{
		return m_data + m_length;
	}

	const_iterator end() const
	{
		return m_data + m_length;
	}

	const_iterator cend() const
	{
		return m_data + m_length;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Reverse iterator
	////////////////////////////////////////////////////////////////////////////////

	reverse_iterator rbegin()
	{
		return reverse_iterator(this->end());
	}

	const_reverse_iterator rbegin() const
	{
		return const_reverse_iterator(this->end());
	}

	const_reverse_iterator crbegin() const
	{
		return const_reverse_iterator(this->end());
	}

	reverse_iterator rend()
	{
		return reverse_iterator(this->begin());
	}

	const_reverse_iterator rend() const
	{
		return const_reverse_iterator(this->begin());
	}

	const_reverse_iterator crend() const
	{
		return const_reverse_iterator(this->begin());
	}

	////////////////////////////////////////////////////////////////////////////////
	// String view
	////////////////////////////////////////////////////////////////////////////////

	operator view_type() const
	{
		return { m_data, m_length };
	}

	////////////////////////////////////////////////////////////////////////////////
	// Comparison
	////////////////////////////////////////////////////////////////////////////////

	int compare(view_type view) const
	{
		return view_type(*this).compare(view);
	}

	bool operator==(view_type view) const
	{
		return view_type(*this) == view;
	}

	auto operator<=>(view_type view) const
	{
		return view_type(*this) <=> view;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Search
	////////////////////////////////////////////////////////////////////////////////

	static constexpr size_type npos = view_type::npos;

	size_type find(view_type view, size_type pos = 0) const
	{
		return view_type(*this).find(view, pos);
	}

	size_type find(value_type ch, size_type pos = 0) const
	{
		return view_type(*this).find(ch, pos);
	}

	size_type rfind(view_type view, size_type pos = npos) const
	{
		return view_type(*this).rfind(view, pos);
	}

	size_type rfind(value_type ch, size_type pos = npos) const
	{
		return view_type(*this).rfind(ch, pos);
	}

	size_type find_first_of(view_type view, size_type pos = 0) const
	{
		return view_type(*this).find_first_of(view, pos);
	}

	size_type find_first_of(value_type ch, size_type pos = 0) const
	{
		return view_type(*this).find_first_of(ch, pos);
	}

	size_type find_first_not_of(view_type view, size_type pos = 0) const
	{
		return view_type(*this).find_first_not_of(view, pos);
	}

	size_type find_first_not_of(value_type ch, size_type pos = 0) const
	{
		return view_type(*this).find_first_not_of(ch, pos);
	}

	size_type find_last_of(view_type view, size_type pos = npos) const
	{
		return view_type(*this).find_last_of(view, pos);
	}

	size_type find_last_of(value_type ch, size_type pos = npos) const
	{
		return view_type(*this).find_last_of(ch, pos);
	}

	size_type find_last_not_of(view_type view, size_type pos = npos) const
	{
		return view_type(*this).find_last_not_of(view, pos);
	}

	size_type find_last_not_of(value_type ch, size_type pos = npos) const
	{
		return view_type(*this).find_last_not_of(ch, pos);
	}

	bool starts_with(view_type view) const
	{
		return view_type(*this).starts_with(view);
	}

	bool starts_with(value_type ch) const
	{
		return view_type(*this).starts_with(ch);
	}

	bool ends_with(view_type view) const
	{
		return view_type(*this).ends_with(view);
	}

	bool ends_with(value_type ch) const
	{
		return view_type(*this).ends_with(ch);
	}

	bool contains(view_type view) const
	{
		return this->find(view) != npos;
	}

	bool contains(value_type ch) const
	{
		return this->find(ch) != npos;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Heap allocation
	////////////////////////////////////////////////////////////////////////////////

private:
	[[nodiscard]] size_type round_up_capacity(size_type required_capacity)
	{
		return required_capacity | 0x1ff;
	}

	[[nodiscard]] value_type* allocate(size_type rounded_up_capacity)
	{
		return new value_type[rounded_up_capacity + 1];
	}

	void deallocate(value_type* data)
	{
		delete[] data;
	}

	FAST_STRING_NOINLINE void reallocate(size_type new_capacity)
	{
		new_capacity = this->round_up_capacity(new_capacity);

		value_type* new_data = this->allocate(new_capacity);
		std::memcpy(new_data, m_data, (m_length + 1) * sizeof(value_type));

		if (m_data != m_local_buffer)
		{
			this->deallocate(m_data);
		}

		m_data = new_data;
		m_heap_capacity = new_capacity;
	}

public:
	////////////////////////////////////////////////////////////////////////////////
	// Modification
	////////////////////////////////////////////////////////////////////////////////

	void clear()
	{
		m_length = 0;
		m_data[0] = 0;
	}

	void reserve(size_type new_capacity)
	{
		if (new_capacity > this->capacity())
		{
			this->reallocate(new_capacity);
		}
	}

	void pop_back()
	{
		const size_type new_length = m_length - 1;

		m_data[new_length] = 0;
		m_length = new_length;
	}

	void push_back(value_type ch)
	{
		const size_type old_length = m_length;
		const size_type new_length = m_length + 1;

		this->reserve(new_length);

		m_data[old_length] = ch;
		m_data[new_length] = 0;
		m_length = new_length;
	}

	BasicFastString& append(view_type view)
	{
		const size_type new_length = m_length + view.length();

		this->reserve(new_length);

		std::memcpy(m_data + m_length, view.data(), view.length() * sizeof(value_type));
		m_data[new_length] = 0;
		m_length = new_length;

		return *this;
	}

	void assign(view_type view)
	{
		if (view.length() > this->capacity())
		{
			const size_type new_capacity = this->round_up_capacity(view.length());

			// allocation before deallocation to provide strong exception safety
			value_type* new_data = this->allocate(new_capacity);

			if (m_data != m_local_buffer)
			{
				this->deallocate(m_data);
			}

			m_data = new_data;
			m_heap_capacity = new_capacity;
		}

		// memmove instead of memcpy to support self-assignment
		std::memmove(m_data, view.data(), view.length() * sizeof(value_type));
		m_data[view.length()] = 0;
		m_length = view.length();
	}

	////////////////////////////////////////////////////////////////////////////////
	// Formatting
	////////////////////////////////////////////////////////////////////////////////

	template<typename... Args>
	BasicFastString& fappend(fmt::format_string<Args...> format, Args&&... args)
	{
		const size_type free_space = this->capacity() - m_length;

		auto result = fmt::format_to_n(this->end(), free_space, format, std::forward<Args>(args)...);
		if (result.size > free_space)
		{
			this->reallocate(m_length + result.size);

			result.out = fmt::format_to(this->end(), format, std::forward<Args>(args)...);
		}

		*result.out = 0;
		m_length = result.out - m_data;

		return *this;
	}

	template<typename... Args>
	BasicFastString& fassign(fmt::format_string<Args...> format, Args&&... args)
	{
		const size_type free_space = this->capacity();

		auto result = fmt::format_to_n(m_data, free_space, format, std::forward<Args>(args)...);
		if (result.size > free_space)
		{
			this->reallocate(result.size);

			result.out = fmt::format_to(m_data, format, std::forward<Args>(args)...);
		}

		*result.out = 0;
		m_length = result.out - m_data;

		return *this;
	}

	// TODO: constructor with pos and count
	// TODO: operator+=
	// TODO: insert
	// TODO: erase
	// TODO: replace
	// TODO: resize + resize_no_init
	// TODO: to_lower
	// TODO: to_upper
	// TODO: trim
};

using FastString = BasicFastString<char>;
