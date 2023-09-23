#pragma once

#include <cstddef>
#include <cstdlib>
#include <memory>

template<class T>
class StlportVector
{
	T* m_begin = nullptr;
	T* m_end = nullptr;
	T* m_end_of_storage = nullptr;

public:
	StlportVector() = default;

	StlportVector(const StlportVector&) = delete;
	StlportVector(StlportVector&&) = delete;

	const StlportVector& operator=(const StlportVector&) = delete;
	const StlportVector& operator=(StlportVector&&) = delete;

	~StlportVector()
	{
		std::destroy(m_begin, m_end);
		std::free(m_begin);
	}

	bool empty() const { return m_begin == m_end; }

	std::size_t size() const { return m_end - m_begin; }

	std::size_t capacity() const { return m_end_of_storage - m_begin; }

	      T& operator[](std::size_t index)       { return m_begin[index]; }
	const T& operator[](std::size_t index) const { return m_begin[index]; }

	      T*  begin()       { return m_begin; }
	const T*  begin() const { return m_begin; }
	const T* cbegin() const { return m_begin; }

	      T*  end()       { return m_end; }
	const T*  end() const { return m_end; }
	const T* cend() const { return m_end; }

	void reserve(std::size_t new_capacity)
	{
		if (new_capacity > this->capacity())
		{
			this->reallocate(new_capacity);
		}
	}

	void push_back(const T& element)
	{
		if (m_end == m_end_of_storage)
		{
			this->reallocate((this->capacity() == 0) ? 8 : this->capacity() * 2);
		}

		std::construct_at(m_end, element);
		m_end++;
	}

private:
	void reallocate(std::size_t new_capacity)
	{
		T* new_buffer = static_cast<T*>(std::malloc(sizeof(T) * new_capacity));
		if (!new_buffer)
		{
			// STLport used by the engine does this because exceptions are disabled
			std::abort();
		}

		std::uninitialized_move(m_begin, m_end, new_buffer);
		std::destroy(m_begin, m_end);
		std::free(m_begin);

		m_end_of_storage = new_buffer + new_capacity;
		m_end = new_buffer + this->size();
		m_begin = new_buffer;
	}
};
