#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "CrySystem/CryLog.h"

#include "ZipPakFile.h"

ZipPakFile::~ZipPakFile()
{
	std::free(m_data);
}

////////////////////////////////////////////////////////////////////////////////
// IPakFile
////////////////////////////////////////////////////////////////////////////////

std::size_t ZipPakFile::FRead(void* buffer, std::size_t elementSize, std::size_t elementCount)
{
	const std::size_t remainingBytes = m_size - static_cast<std::size_t>(m_pos);

	std::size_t totalBytes = elementSize * elementCount;
	if (totalBytes > remainingBytes)
	{
		totalBytes = remainingBytes;
		if (totalBytes < elementSize)
		{
			return 0;
		}

		totalBytes -= totalBytes % elementSize;
	}

	if (m_isBinary)
	{
		std::memcpy(buffer, &m_data[m_pos], totalBytes);
	}
	else
	{
		std::byte* dest = static_cast<std::byte*>(buffer);
		for (std::size_t i = 0; i < totalBytes; i++)
		{
			const std::byte ch = m_data[m_pos + i];
			if (std::to_integer<char>(ch) != '\r')
			{
				*(dest++) = ch;
			}
		}
	}

	m_pos += totalBytes;

	return totalBytes / elementSize;
}

std::size_t ZipPakFile::FWrite(const void* buffer, std::size_t elementSize, std::size_t elementCount)
{
	CryLogAlways("%s: NOT IMPLEMENTED!", __FUNCTION__);
	return 0;
}

int ZipPakFile::VFPrintF(const char* format, va_list args)
{
	CryLogAlways("%s: NOT IMPLEMENTED!", __FUNCTION__);
	return -1;
}

char* ZipPakFile::FGetS(char* buffer, int bufferSize)
{
	if (buffer == nullptr || bufferSize == 0 || static_cast<std::size_t>(m_pos) >= m_size)
	{
		return nullptr;
	}

	const int maxSize = bufferSize - 1;

	int i = 0;
	while (i < maxSize)
	{
		if (static_cast<std::size_t>(m_pos) >= m_size)
		{
			break;
		}

		const char ch = std::to_integer<char>(m_data[m_pos++]);

		if (ch != '\r' || m_isBinary)
		{
			buffer[i++] = ch;
		}

		if (ch == '\n')
		{
			break;
		}
	}

	buffer[i] = '\0';

	return buffer;
}

int ZipPakFile::FGetC()
{
	int ch = EOF;

	do
	{
		if (static_cast<std::size_t>(m_pos) >= m_size)
		{
			return EOF;
		}

		ch = std::to_integer<int>(m_data[m_pos++]);
	}
	while (ch == '\r' || m_isBinary);

	return ch;
}

int ZipPakFile::FUnGetC(int ch)
{
	CryLogAlways("%s: NOT IMPLEMENTED!", __FUNCTION__);
	return EOF;
}

int ZipPakFile::FSeek(std::int64_t offset, int mode)
{
	if (mode == SEEK_SET)
	{
		m_pos = offset;
	}
	else if (mode == SEEK_CUR)
	{
		m_pos += offset;
	}
	else if (mode == SEEK_END)
	{
		m_pos = m_size - offset;
	}
	else
	{
		return -1;
	}

	if (m_pos < 0)
	{
		m_pos = 0;
	}
	else if (static_cast<std::size_t>(m_pos) >= m_size)
	{
		m_pos = m_size;
	}

	return 0;
}

std::int64_t ZipPakFile::FTell() const
{
	return m_pos;
}

std::int64_t ZipPakFile::GetSize() const
{
	return m_size;
}

int ZipPakFile::FFlush()
{
	return 0;
}

int ZipPakFile::FEof() const
{
	return static_cast<std::size_t>(m_pos) >= m_size;
}

int ZipPakFile::FError() const
{
	return 0;
}

void* ZipPakFile::GetCachedFileData(std::size_t& fileSize)
{
	fileSize = m_size;

	return m_data;
}

std::uint64_t ZipPakFile::GetModificationTime()
{
	return m_modificationTime;
}

////////////////////////////////////////////////////////////////////////////////
