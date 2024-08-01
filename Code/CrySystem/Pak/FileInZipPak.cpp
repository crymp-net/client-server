#include <cstring>

#include "CrySystem/CryLog.h"

#include "FileInZipPak.h"

////////////////////////////////////////////////////////////////////////////////
// IFileInPak
////////////////////////////////////////////////////////////////////////////////

std::size_t FileInZipPak::FRead(void* buffer, std::size_t elementSize, std::size_t elementCount)
{
	const std::size_t remainingBytes = this->size - this->pos;

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

	if (this->isBinary)
	{
		std::memcpy(buffer, &this->data[this->pos], totalBytes);
	}
	else
	{
		std::byte* dest = static_cast<std::byte*>(buffer);
		for (std::size_t i = 0; i < totalBytes; i++)
		{
			const std::byte ch = this->data[this->pos + i];
			if (std::to_integer<char>(ch) != '\r')
			{
				*(dest++) = ch;
			}
		}
	}

	this->pos += totalBytes;

	return totalBytes / elementSize;
}

std::size_t FileInZipPak::FWrite(const void* buffer, std::size_t elementSize, std::size_t elementCount)
{
	CryLogErrorAlways("%s: Not implemented!", __FUNCTION__);
	return 0;
}

int FileInZipPak::VFPrintF(const char* format, va_list args)
{
	CryLogErrorAlways("%s: Not implemented!", __FUNCTION__);
	return -1;
}

char* FileInZipPak::FGetS(char* buffer, int bufferSize)
{
	if (buffer == nullptr || bufferSize == 0 || this->pos >= this->size)
	{
		return nullptr;
	}

	const int maxSize = bufferSize - 1;

	int i = 0;
	while (i < maxSize)
	{
		if (this->pos >= this->size)
		{
			break;
		}

		const char ch = std::to_integer<char>(this->data[this->pos++]);

		if (ch != '\r' || this->isBinary)
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

int FileInZipPak::FGetC()
{
	int ch = EOF;

	do
	{
		if (this->pos >= this->size)
		{
			return EOF;
		}

		ch = std::to_integer<int>(this->data[this->pos++]);
	}
	while (ch == '\r' || this->isBinary);

	return ch;
}

int FileInZipPak::FUnGetC(int ch)
{
	CryLogErrorAlways("%s: Not implemented!", __FUNCTION__);
	return EOF;
}

int FileInZipPak::FSeek(std::int64_t offset, int mode)
{
	std::int64_t newPos = 0;

	switch (mode)
	{
		case SEEK_SET:
		{
			newPos = offset;
			break;
		}
		case SEEK_CUR:
		{
			newPos = this->pos;
			newPos += offset;
			break;
		}
		case SEEK_END:
		{
			newPos = this->size;
			newPos += offset;
			break;
		}
		default:
		{
			return -1;
		}
	}

	if (newPos < 0)
	{
		newPos = 0;
	}
	else if (newPos > static_cast<std::int64_t>(this->size))
	{
		newPos = this->size;
	}

	this->pos = static_cast<std::size_t>(newPos);

	return 0;
}

std::int64_t FileInZipPak::FTell() const
{
	return this->pos;
}

std::uint64_t FileInZipPak::GetSize() const
{
	return this->size;
}

int FileInZipPak::FFlush()
{
	return 0;
}

int FileInZipPak::FEof() const
{
	return this->pos >= this->size;
}

int FileInZipPak::FError() const
{
	return 0;
}

void* FileInZipPak::GetCachedFileData(std::size_t& fileSize)
{
	fileSize = this->size;

	return this->data;
}

std::uint64_t FileInZipPak::GetModificationTime()
{
	return this->modificationTime;
}

std::FILE* FileInZipPak::GetHandle()
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
