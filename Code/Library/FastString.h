#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <string_view>

// TODO: use C++20 modules once CMake supports them
#include <fmt/format.h>

#ifdef _MSC_VER
#define FAST_STRING_NOINLINE __declspec(noinline)
#else
#define FAST_STRING_NOINLINE __attribute__((__noinline__))
#endif

template<std::integral Char>
class BasicFastString
{
public:
	using value_type = Char;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

	using view_type = std::basic_string_view<Char>;

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

	explicit BasicFastString(size_type count, value_type ch) : m_length(count)
	{
		if (count <= LOCAL_CAPACITY)
		{
			m_data = m_local_buffer;
		}
		else
		{
			const size_type new_capacity = this->round_up_capacity(count);

			m_data = this->allocate(new_capacity);
			m_heap_capacity = new_capacity;
		}

		std::fill_n(m_data, count, ch);
		m_data[count] = 0;
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
		return this->assign(view);
	}

	BasicFastString& operator=(const BasicFastString& other)
	{
		return this->assign(other);
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
		// TODO: compiler needs to assume that m_heap_capacity > LOCAL_CAPACITY to eliminate dead code

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
		return view_type(m_data, m_length);
	}

	view_type view() const
	{
		return view_type(m_data, m_length);
	}

	////////////////////////////////////////////////////////////////////////////////
	// Comparison
	////////////////////////////////////////////////////////////////////////////////

	int compare(view_type view) const
	{
		return this->view().compare(view);
	}

	bool operator==(view_type view) const
	{
		return this->view() == view;
	}

	auto operator<=>(view_type view) const
	{
		return this->view() <=> view;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Search
	////////////////////////////////////////////////////////////////////////////////

	static constexpr size_type npos = view_type::npos;

	size_type find(view_type view, size_type pos = 0) const
	{
		return this->view().find(view, pos);
	}

	size_type find(value_type ch, size_type pos = 0) const
	{
		return this->view().find(ch, pos);
	}

	size_type rfind(view_type view, size_type pos = npos) const
	{
		return this->view().rfind(view, pos);
	}

	size_type rfind(value_type ch, size_type pos = npos) const
	{
		return this->view().rfind(ch, pos);
	}

	size_type find_first_of(view_type view, size_type pos = 0) const
	{
		return this->view().find_first_of(view, pos);
	}

	size_type find_first_of(value_type ch, size_type pos = 0) const
	{
		return this->view().find_first_of(ch, pos);
	}

	size_type find_first_not_of(view_type view, size_type pos = 0) const
	{
		return this->view().find_first_not_of(view, pos);
	}

	size_type find_first_not_of(value_type ch, size_type pos = 0) const
	{
		return this->view().find_first_not_of(ch, pos);
	}

	size_type find_last_of(view_type view, size_type pos = npos) const
	{
		return this->view().find_last_of(view, pos);
	}

	size_type find_last_of(value_type ch, size_type pos = npos) const
	{
		return this->view().find_last_of(ch, pos);
	}

	size_type find_last_not_of(view_type view, size_type pos = npos) const
	{
		return this->view().find_last_not_of(view, pos);
	}

	size_type find_last_not_of(value_type ch, size_type pos = npos) const
	{
		return this->view().find_last_not_of(ch, pos);
	}

	bool starts_with(view_type view) const
	{
		return this->view().starts_with(view);
	}

	bool starts_with(value_type ch) const
	{
		return this->view().starts_with(ch);
	}

	bool ends_with(view_type view) const
	{
		return this->view().ends_with(view);
	}

	bool ends_with(value_type ch) const
	{
		return this->view().ends_with(ch);
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

	FAST_STRING_NOINLINE void reallocate(size_type new_capacity, bool copy_data)
	{
		new_capacity = this->round_up_capacity(new_capacity);

		value_type* new_data = this->allocate(new_capacity);

		if (copy_data)
		{
			std::memcpy(new_data, m_data, (m_length + 1) * sizeof(value_type));
		}

		if (m_data != m_local_buffer)
		{
			this->deallocate(m_data);
		}

		m_data = new_data;
		m_heap_capacity = new_capacity;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Modification
	////////////////////////////////////////////////////////////////////////////////

	void set_length(size_type new_length)
	{
		m_data[new_length] = 0;
		m_length = new_length;
	}

public:
	void clear()
	{
		this->set_length(0);
	}

	void reserve(size_type new_capacity)
	{
		if (new_capacity > this->capacity())
		{
			this->reallocate(new_capacity, true);
		}
	}

	void resize(size_type new_length, value_type ch = 0)
	{
		this->reserve(new_length);

		if (new_length > m_length)
		{
			std::fill_n(m_data + m_length, new_length - m_length, ch);
		}

		this->set_length(new_length);
	}

	template<typename Operation>
	void resize_and_overwrite(size_type max_length, Operation op)
	{
		this->reserve(max_length);

		const size_type new_length = std::move(op)(m_data, max_length);
		this->set_length(new_length);
	}

	BasicFastString& assign(view_type view)
	{
		if (view.length() > this->capacity())
		{
			this->reallocate(view.length(), false);
		}

		std::memmove(m_data, view.data(), view.length() * sizeof(value_type));
		this->set_length(view.length());

		return *this;
	}

	BasicFastString& assign(size_type count, value_type ch)
	{
		if (count > this->capacity())
		{
			this->reallocate(count, false);
		}

		std::fill_n(m_data, count, ch);
		this->set_length(count);

		return *this;
	}

	BasicFastString& append(view_type view)
	{
		this->reserve(m_length + view.length());

		std::memcpy(m_data + m_length, view.data(), view.length() * sizeof(value_type));
		this->set_length(m_length + view.length());

		return *this;
	}

	BasicFastString& append(size_type count, value_type ch)
	{
		this->reserve(m_length + count);

		std::fill_n(m_data + m_length, count, ch);
		this->set_length(m_length + count);

		return *this;
	}

	BasicFastString& pop_back()
	{
		this->set_length(m_length - 1);

		return *this;
	}

	BasicFastString& push_back(value_type ch)
	{
		this->reserve(m_length + 1);

		m_data[m_length] = ch;
		this->set_length(m_length + 1);

		return *this;
	}

	BasicFastString& operator+=(value_type ch)
	{
		return this->push_back(ch);
	}

	BasicFastString& operator+=(view_type view)
	{
		return this->append(view);
	}

private:
	value_type* prepare_insert(size_type index, size_type count)
	{
		if (index > m_length)
		{
			index = m_length;
		}

		this->reserve(m_length + count);

		value_type* const destination = m_data + index;
		const size_type move_length = m_length - index;

		std::memmove(destination + count, destination, move_length * sizeof(value_type));
		this->set_length(m_length + count);

		return destination;
	}

public:
	BasicFastString& insert(size_type index, view_type view)
	{
		value_type* const destination = this->prepare_insert(index, view.length());

		std::memmove(destination, view.data(), view.length() * sizeof(value_type));

		return *this;
	}

	BasicFastString& insert(size_type index, size_type count, value_type ch)
	{
		value_type* const destination = this->prepare_insert(index, count);

		std::fill_n(destination, count, ch);

		return *this;
	}

private:
	value_type* do_erase(size_type index, size_type count)
	{
		value_type* const destination = m_data + index;
		const size_type move_length = m_length - index - count;

		std::memmove(destination, destination + count, move_length * sizeof(value_type));
		this->set_length(m_length - count);

		return destination;
	}

public:
	BasicFastString& erase(size_type index, size_type count)
	{
		if (index > m_length)
		{
			index = m_length;
		}

		if ((index + count) > m_length)
		{
			count = m_length - index;
		}

		this->do_erase(index, count);

		return *this;
	}

	iterator erase(const_iterator pos)
	{
		return this->do_erase(pos - this->begin(), 1);
	}

	iterator erase(const_iterator first, const_iterator last)
	{
		return this->do_erase(first - this->begin(), last - first);
	}

	// TODO: replace

	////////////////////////////////////////////////////////////////////////////////
	// Formatting
	////////////////////////////////////////////////////////////////////////////////

	// TODO: formatting functions support only char/UTF-8

	FAST_STRING_NOINLINE BasicFastString& vfappend(fmt::string_view format, fmt::format_args args)
	{
		const size_type free_space = this->capacity() - m_length;

		auto result = fmt::vformat_to_n(this->end(), free_space, format, args);
		if (result.size > free_space)
		{
			this->reallocate(m_length + result.size, true);

			result.out = fmt::vformat_to(this->end(), format, args);
		}

		this->set_length(result.out - this->begin());

		return *this;
	}

	FAST_STRING_NOINLINE BasicFastString& vfassign(fmt::string_view format, fmt::format_args args)
	{
		const size_type free_space = this->capacity();

		auto result = fmt::vformat_to_n(this->begin(), free_space, format, args);
		if (result.size > free_space)
		{
			this->reallocate(result.size, false);

			result.out = fmt::vformat_to(this->begin(), format, args);
		}

		this->set_length(result.out - this->begin());

		return *this;
	}

	template<typename... Args>
	BasicFastString& fappend(fmt::format_string<Args...> format, Args&&... args)
	{
		return this->vfappend(format, fmt::make_format_args(args...));
	}

	template<typename... Args>
	BasicFastString& fassign(fmt::format_string<Args...> format, Args&&... args)
	{
		return this->vfassign(format, fmt::make_format_args(args...));
	}

	template<typename... Args>
	static BasicFastString format(fmt::format_string<Args...> format, Args&&... args)
	{
		BasicFastString result;
		result.vfassign(format, fmt::make_format_args(args...));
		return result;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Case conversion
	////////////////////////////////////////////////////////////////////////////////

	// ASCII-only
	BasicFastString& to_lower()
	{
		for (value_type& ch : *this)
		{
			ch |= (ch >= 'A' && ch <= 'Z') << 5;
		}

		return *this;
	}

	// ASCII-only
	BasicFastString& to_upper()
	{
		for (value_type& ch : *this)
		{
			ch &= ~((ch >= 'a' && ch <= 'z') << 5);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Trimming
	////////////////////////////////////////////////////////////////////////////////

	// TODO: trimming
};

////////////////////////////////////////////////////////////////////////////////
// Make it formattable
////////////////////////////////////////////////////////////////////////////////

template<typename Char>
struct fmt::formatter<BasicFastString<Char>, Char> : fmt::formatter<std::basic_string_view<Char>, Char>
{
	template<typename FormatContext>
	auto format(const BasicFastString<Char>& value, FormatContext& context) const
	{
		return fmt::formatter<std::basic_string_view<Char>, Char>::format(value.view(), context);
	}
};

////////////////////////////////////////////////////////////////////////////////
// Shorter aliases
////////////////////////////////////////////////////////////////////////////////

using FastString = BasicFastString<char>;
