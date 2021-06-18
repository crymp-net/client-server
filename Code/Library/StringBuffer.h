#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <string>
#include <string_view>

class StringBufferBase
{
	size_t m_length = 0;

	char *m_buffer = nullptr;
	size_t m_bufferSize = 0;

	char *m_initialBuffer = nullptr;
	size_t m_initialBufferSize = 0;

	void deallocate();

protected:
	StringBufferBase(char *initialBuffer, size_t initialBufferSize)
	{
		m_buffer = initialBuffer;
		m_bufferSize = initialBufferSize;

		m_initialBuffer = initialBuffer;
		m_initialBufferSize = initialBufferSize;

		clear();
	}

	void moveBufferFrom(StringBufferBase && other);

	void appendChar(char ch);
	void appendText(const std::string_view & text);

	void appendFormat(const char *format, ...);
	void appendFormatV(const char *format, va_list args);

	void appendNumber(uint64_t number, int minWidth, char pad, int base, bool uppercase);
	void appendFloat(double number, int minWidth, int precision, bool trailingZeros);

public:
	StringBufferBase(const StringBufferBase &) = delete;
	StringBufferBase(StringBufferBase &&) = delete;

	StringBufferBase & operator=(const StringBufferBase &) = delete;
	StringBufferBase & operator=(StringBufferBase &&) = delete;

	~StringBufferBase()
	{
		if (isHeap())
		{
			deallocate();
		}
	}

	bool isHeap() const
	{
		return m_buffer != m_initialBuffer;
	}

	bool empty() const
	{
		return m_length == 0;
	}

	const char *c_str() const
	{
		return m_buffer;
	}

	size_t length() const
	{
		return m_length;
	}

	size_t capacity() const
	{
		return m_bufferSize - 1;
	}

	size_t space() const
	{
		return capacity() - length();
	}

	char & operator[](size_t index)
	{
		return m_buffer[index];
	}

	const char & operator[](size_t index) const
	{
		return m_buffer[index];
	}

	char & front()
	{
		return m_buffer[0];
	}

	const char & front() const
	{
		return m_buffer[0];
	}

	char & back()
	{
		return (m_length > 0) ? m_buffer[m_length-1] : m_buffer[0];
	}

	const char & back() const
	{
		return (m_length > 0) ? m_buffer[m_length-1] : m_buffer[0];
	}

	std::string toString() const
	{
		return std::string(m_buffer, m_length);
	}

	operator std::string_view() const
	{
		return std::string_view(m_buffer, m_length);
	}

	void pop(size_t length = 1)
	{
		m_length -= (length < m_length) ? length : m_length;
		m_buffer[m_length] = '\0';
	}

	void clear()
	{
		m_length = 0;
		m_buffer[0] = '\0';
	}

	void reserve(size_t length);
};

template<size_t Size>
class StringBuffer : public StringBufferBase
{
	static_assert(Size > 0);

	char m_storage[Size];

public:
	StringBuffer()
	: StringBufferBase(m_storage, Size)
	{
	}

	StringBuffer(const char *text)
	: StringBuffer()
	{
		appendText(text);
	}

	StringBuffer(const std::string & text)
	: StringBuffer()
	{
		appendText(text);
	}

	StringBuffer(const std::string_view & text)
	: StringBuffer()
	{
		appendText(text);
	}

	StringBuffer(const StringBuffer & other)
	: StringBuffer()
	{
		appendText(other);
	}

	StringBuffer(StringBuffer && other)
	: StringBuffer()
	{
		moveBufferFrom(std::move(other));
	}

	StringBuffer & operator=(const StringBuffer & other)
	{
		if (this != &other)
		{
			clear();
			appendText(other);
		}

		return *this;
	}

	StringBuffer & operator=(StringBuffer && other)
	{
		if (this != &other)
		{
			moveBufferFrom(std::move(other));
		}

		return *this;
	}

	StringBuffer & add(char ch)
	{
		appendChar(ch);

		return *this;
	}

	StringBuffer & add(const std::string_view & text)
	{
		appendText(text);

		return *this;
	}

	StringBuffer & addFormat(const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		appendFormatV(format, args);
		va_end(args);

		return *this;
	}

	StringBuffer & addFormatV(const char *format, va_list args)
	{
		appendFormatV(format, args);

		return *this;
	}

	StringBuffer & addUInt(uint64_t number, int minWidth = 0, char pad = '0', int base = 10, bool uppercase = true)
	{
		appendNumber(number, minWidth, pad, base, uppercase);

		return *this;
	}

	StringBuffer & addInt(int64_t number, int minWidth = 0, char pad = '0', int base = 10, bool uppercase = true)
	{
		if (number < 0)
		{
			add('-');
			number = -number;
		}

		appendNumber(number, minWidth, pad, base, uppercase);

		return *this;
	}

	StringBuffer & addFloat(double number, int minWidth = 0, int precision = 2, bool trailingZeros = false)
	{
		appendFloat(number, minWidth, precision, trailingZeros);

		return *this;
	}

	StringBuffer & operator+=(char ch)
	{
		return add(ch);
	}

	StringBuffer & operator+=(const std::string_view & text)
	{
		return add(text);
	}
};

template<size_t Size>
inline const char *begin(const StringBuffer<Size> & buffer)
{
	return buffer.c_str();
}

template<size_t Size>
inline const char *end(const StringBuffer<Size> & buffer)
{
	return buffer.c_str() + buffer.length();
}
