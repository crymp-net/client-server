#pragma once

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <string>
#include <new>

template<size_t DefaultSize>
class StringBuffer
{
	char m_stackBuffer[DefaultSize];
	char *m_buffer;
	size_t m_bufferSize;
	size_t m_pos;

public:
	StringBuffer()
	{
		m_stackBuffer[0] = '\0';
		m_buffer = m_stackBuffer;
		m_bufferSize = DefaultSize;
		m_pos = 0;
	}

	StringBuffer(const StringBuffer &) = delete;
	StringBuffer & operator=(const StringBuffer &) = delete;

	~StringBuffer()
	{
		if (isHeap())
		{
			delete [] m_buffer;
		}
	}

	bool isHeap() const
	{
		return m_buffer != m_stackBuffer;
	}

	bool isEmpty() const
	{
		return m_pos == 0;
	}

	const char *get() const
	{
		return m_buffer;
	}

	size_t getLength() const
	{
		return m_pos;
	}

	size_t getAvailableSpace() const
	{
		return m_bufferSize - m_pos - 1;
	}

	size_t getCapacity() const
	{
		return m_bufferSize;
	}

	char operator[](size_t index) const
	{
		return m_buffer[index];
	}

	char getLast() const
	{
		return (m_pos > 0) ? m_buffer[m_pos-1] : m_buffer[0];
	}

	std::string toString() const
	{
		return std::string(m_buffer, m_pos);
	}

	void clear()
	{
		m_pos = 0;
		m_buffer[0] = '\0';
	}

	void pop(size_t length = 1)
	{
		m_pos -= (length < m_pos) ? length : m_pos;
		m_buffer[m_pos] = '\0';
	}

	void append(char ch)
	{
		makeSpaceFor(1);

		m_buffer[m_pos] = ch;
		m_pos += 1;
		m_buffer[m_pos] = '\0';
	}

	void append(const char *string)
	{
		if (!string)
		{
			return;
		}

		append(string, std::strlen(string));
	}

	void append(const char *string, size_t length)
	{
		if (!string || length == 0)
		{
			return;
		}

		makeSpaceFor(length);

		std::memcpy(m_buffer + m_pos, string, length);
		m_pos += length;
		m_buffer[m_pos] = '\0';
	}

	void append(const std::string & string)
	{
		append(string.c_str(), string.length());
	}

	int append_f(const char *format, ...)
	{
		if (!format)
		{
			return 0;
		}

		va_list args;
		va_start(args, format);

		va_list argsCopy;
		va_copy(argsCopy, args);

		int status = std::vsnprintf(m_buffer + m_pos, m_bufferSize - m_pos, format, args);

		va_end(args);

		if (status > 0)
		{
			size_t length = status;

			// check if the resulting string was truncated
			if (length > getAvailableSpace())
			{
				makeSpaceFor(length);

				// do again with proper buffer size
				m_pos += std::vsnprintf(m_buffer + m_pos, m_bufferSize - m_pos, format, argsCopy);
			}
			else
			{
				m_pos += length;
			}
		}

		va_end(argsCopy);

		return status;
	}

	int append_vf(const char *format, va_list args)
	{
		if (!format)
		{
			return 0;
		}

		va_list argsCopy;
		va_copy(argsCopy, args);

		int status = std::vsnprintf(m_buffer + m_pos, m_bufferSize - m_pos, format, args);

		if (status > 0)
		{
			size_t length = status;

			// check if the resulting string was truncated
			if (length > getAvailableSpace())
			{
				makeSpaceFor(length);

				// do again with proper buffer size
				m_pos += std::vsnprintf(m_buffer + m_pos, m_bufferSize - m_pos, format, argsCopy);
			}
			else
			{
				m_pos += length;
			}
		}

		va_end(argsCopy);

		return status;
	}

	void makeSpaceFor(size_t length)
	{
		if (length > getAvailableSpace())
		{
			size_t requiredSize = getLength() + length + 1;
			size_t minimalSize = getCapacity() * 2;
			size_t newSize = (requiredSize > minimalSize) ? requiredSize : minimalSize;

			resize(newSize);
		}
	}

	void resize(size_t size)
	{
		if (size == 0 || size == m_bufferSize)
		{
			return;
		}

		char *oldBuffer = m_buffer;
		char *newBuffer = (size > DefaultSize) ? new char[size] : m_stackBuffer;

		if (newBuffer == oldBuffer)
		{
			return;
		}

		if (m_pos >= size)
		{
			// truncate buffer content
			m_pos = size - 1;
			oldBuffer[m_pos] = '\0';
		}

		// copy content to the new buffer
		std::memcpy(newBuffer, oldBuffer, m_pos + 1);

		if (isHeap())
		{
			delete [] oldBuffer;
		}

		m_buffer = newBuffer;
		m_bufferSize = size;
	}
};

template<size_t T>
inline StringBuffer<T> & operator+=(StringBuffer<T> & buffer, char ch)
{
	buffer.append(ch);
	return buffer;
}

template<size_t T>
inline StringBuffer<T> & operator+=(StringBuffer<T> & buffer, const char *string)
{
	buffer.append(string);
	return buffer;
}

template<size_t T>
inline StringBuffer<T> & operator+=(StringBuffer<T> & buffer, const std::string & string)
{
	buffer.append(string);
	return buffer;
}
