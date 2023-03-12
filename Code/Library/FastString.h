#pragma once

#include <concepts>
#include <cstddef>
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
	using difference_type = std::ptrdiff_t;

	using pointer = value_type*;
	using reference = value_type&;
	using const_pointer = const value_type*;
	using const_reference = const value_type&;

	using iterator = pointer;
	using const_iterator = const_pointer;

	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
	static constexpr std::size_t LOCAL_CAPACITY = 95;

	T* m_data = nullptr;
	std::size_t m_length = 0;

	union
	{
		T m_local_buffer[LOCAL_CAPACITY + 1];
		std::size_t m_heap_capacity;
	};

public:
	BasicFastString() : m_data(m_local_buffer), m_heap_capacity(0)  // fast null-termination of m_local_buffer
	{
	}

	explicit BasicFastString(std::basic_string_view<T> view) : m_length(view.length())
	{
		if (view.length() <= LOCAL_CAPACITY)
		{
			m_data = m_local_buffer;
		}
		else
		{
			const std::size_t new_capacity = this->round_up_capacity(view.length());

			m_data = this->allocate(new_capacity);
			m_heap_capacity = new_capacity;
		}

		std::memcpy(m_data, view.data(), view.length() * sizeof(T));
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
			std::memcpy(m_data, other.m_data, (other.m_length + 1) * sizeof(T));
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

	BasicFastString& operator=(std::basic_string_view<T> view)
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

	std::size_t capacity() const
	{
		return (m_data == m_local_buffer) ? LOCAL_CAPACITY : m_heap_capacity;
	}

	std::size_t length() const
	{
		return m_length;
	}

	std::size_t size() const
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

	T* data()
	{
		return m_data;
	}

	const T* data() const
	{
		return m_data;
	}

	const T* c_str() const
	{
		return m_data;
	}

	T& front()
	{
		return m_data[0];
	}

	const T& front() const
	{
		return m_data[0];
	}

	T& back()
	{
		return m_data[m_length - 1];
	}

	const T& back() const
	{
		return m_data[m_length - 1];
	}

	T& operator[](std::size_t index)
	{
		return m_data[index];
	}

	const T& operator[](std::size_t index) const
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

	operator std::basic_string_view<T>() const
	{
		return { m_data, m_length };
	}

	////////////////////////////////////////////////////////////////////////////////
	// Heap allocation
	////////////////////////////////////////////////////////////////////////////////

private:
	[[nodiscard]] std::size_t round_up_capacity(std::size_t required_capacity)
	{
		return required_capacity | 0x1ff;
	}

	[[nodiscard]] T* allocate(std::size_t rounded_up_capacity)
	{
		return new T[rounded_up_capacity + 1];
	}

	void deallocate(T* data)
	{
		delete[] data;
	}

	FAST_STRING_NOINLINE void reallocate(std::size_t new_capacity)
	{
		new_capacity = this->round_up_capacity(new_capacity);

		T* new_data = this->allocate(new_capacity);
		std::memcpy(new_data, m_data, (m_length + 1) * sizeof(T));

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

	void reserve(std::size_t new_capacity)
	{
		if (new_capacity > this->capacity())
		{
			this->reallocate(new_capacity);
		}
	}

	void pop_back()
	{
		const std::size_t new_length = m_length - 1;

		m_data[new_length] = 0;
		m_length = new_length;
	}

	void push_back(T ch)
	{
		const std::size_t old_length = m_length;
		const std::size_t new_length = m_length + 1;

		this->reserve(new_length);

		m_data[old_length] = ch;
		m_data[new_length] = 0;
		m_length = new_length;
	}

	BasicFastString& append(std::basic_string_view<T> view)
	{
		const std::size_t new_length = m_length + view.length();

		this->reserve(new_length);

		std::memcpy(m_data + m_length, view.data(), view.length() * sizeof(T));
		m_data[new_length] = 0;
		m_length = new_length;

		return *this;
	}

	void assign(std::basic_string_view<T> view)
	{
		if (view.length() > this->capacity())
		{
			const std::size_t new_capacity = this->round_up_capacity(view.length());

			// allocation before deallocation to provide strong exception safety
			T* new_data = this->allocate(new_capacity);

			if (m_data != m_local_buffer)
			{
				this->deallocate(m_data);
			}

			m_data = new_data;
			m_heap_capacity = new_capacity;
		}

		// memmove instead of memcpy to support self-assignment
		std::memmove(m_data, view.data(), view.length() * sizeof(T));
		m_data[view.length()] = 0;
		m_length = view.length();
	}

	////////////////////////////////////////////////////////////////////////////////
	// Formatting
	////////////////////////////////////////////////////////////////////////////////

	template<typename... Args>
	BasicFastString& fappend(fmt::format_string<Args...> format, Args&&... args)
	{
		const std::size_t free_space = this->capacity() - m_length;

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
		const std::size_t free_space = this->capacity();

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

	// TODO: find
	// TODO: compare + operators
	// TODO: to_lower
	// TODO: to_upper
	// TODO: trim
};

using FastString = BasicFastString<char>;
