#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <new>

#include "StringBuffer.h"

void StringBufferBase::deallocate()
{
	delete [] m_buffer;
}

void StringBufferBase::moveBufferFrom(StringBufferBase && other)
{
	clear();

	if (other.isHeap())
	{
		if (isHeap())
		{
			deallocate();
		}

		m_length = other.m_length;
		m_buffer = other.m_buffer;
		m_bufferSize = other.m_bufferSize;

		other.m_buffer = other.m_initialBuffer;
		other.m_bufferSize = other.m_initialBufferSize;
	}
	else
	{
		reserve(other.m_length);

		memcpy(m_buffer, other.m_buffer, other.m_length);

		m_length = other.m_length;
		m_buffer[m_length] = '\0';
	}

	other.clear();
}

void StringBufferBase::appendChar(char ch)
{
	reserve(m_length + 1);

	m_buffer[m_length++] = ch;
	m_buffer[m_length] = '\0';
}

void StringBufferBase::appendText(const std::string_view & text)
{
	if (text.empty())
	{
		return;
	}

	reserve(m_length + text.length());

	memcpy(m_buffer + m_length, text.data(), text.length());

	m_length += text.length();
	m_buffer[m_length] = '\0';
}

void StringBufferBase::appendFormat(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	appendFormatV(format, args);
	va_end(args);
}

void StringBufferBase::appendFormatV(const char *format, va_list args)
{
	if (!format)
	{
		return;
	}

	va_list argsCopy;
	va_copy(argsCopy, args);

	int status = vsnprintf(m_buffer + m_length, m_bufferSize - m_length, format, args);

	if (status > 0)
	{
		size_t length = status;

		// make sure the resulting string is not truncated
		if (length > space())
		{
			reserve(m_length + length);

			// format string again with proper buffer size
			status = vsnprintf(m_buffer + m_length, m_bufferSize - m_length, format, argsCopy);

			length = (status < 0) ? 0 : status;
			length = std::min(length, space());
		}

		m_length += length;
	}

	m_buffer[m_length] = '\0';

	va_end(argsCopy);
}

void StringBufferBase::appendNumber(uint64_t number, int minWidth, char pad, int base, bool uppercase)
{
	if (base < 2 || base > 36)
	{
		return;
	}

	const char firstLetter = (uppercase) ? 'A' : 'a';
	const size_t beginPos = m_length;

	do
	{
		const int digit = number % base;
		const char ch = (digit < 10) ? '0' + digit : firstLetter + (digit - 10);

		appendChar(ch);

		number /= base;
	}
	while (number > 0);

	for (int digitCount = m_length - beginPos; digitCount < minWidth; digitCount++)
	{
		appendChar(pad);
	}

	std::reverse(m_buffer + beginPos, m_buffer + m_length);
}

void StringBufferBase::appendFloat(double number, int minWidth, int precision, bool trailingZeros)
{
	StringBuffer<32> format;

	format.add('%');
	format.add('#');  // make sure decimal point is always there
	format.addInt(minWidth);
	format.add('.');
	format.addInt(precision);
	format.add('f');

	const size_t beginPos = m_length;

	appendFormat(format.c_str(), number);

	if (!trailingZeros && m_length > beginPos)
	{
		size_t pos = m_length - 1;

		// remove trailing zeros after decimal point
		for (; m_buffer[pos] == '0'; pos--)
		{
			if (static_cast<int>(pos - beginPos) < minWidth)
			{
				// replace with space to maintain the minimum width
				m_buffer[pos] = ' ';
			}
			else
			{
				pop();
			}
		}

		// remove trailing decimal point
		if (m_buffer[pos] == '.')
		{
			if (static_cast<int>(pos - beginPos) < minWidth)
			{
				// replace with space to maintain the minimum width
				m_buffer[pos] = ' ';
			}
			else
			{
				pop();
			}
		}
	}
}

void StringBufferBase::reserve(size_t length)
{
	if (length < m_bufferSize)
	{
		return;
	}

	const size_t newSize = std::max(length + 1, m_bufferSize * 2);

	// allocate a new buffer
	char *newBuffer = new char[newSize];

	// copy content to the new buffer
	memcpy(newBuffer, m_buffer, m_length + 1);

	if (isHeap())
	{
		// free the old buffer
		deallocate();
	}

	m_buffer = newBuffer;
	m_bufferSize = newSize;
}
