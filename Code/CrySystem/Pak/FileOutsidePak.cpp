#include <chrono>

#include "FileOutsidePak.h"

////////////////////////////////////////////////////////////////////////////////
// IFileInPak
////////////////////////////////////////////////////////////////////////////////

std::size_t FileOutsidePak::FRead(void* buffer, std::size_t elementSize, std::size_t elementCount)
{
	return std::fread(buffer, elementSize, elementCount, this->handle.get());
}

std::size_t FileOutsidePak::FWrite(const void* buffer, std::size_t elementSize, std::size_t elementCount)
{
	return std::fwrite(buffer, elementSize, elementCount, this->handle.get());
}

int FileOutsidePak::VFPrintF(const char* format, va_list args)
{
	return std::vfprintf(this->handle.get(), format, args);
}

char* FileOutsidePak::FGetS(char* buffer, int bufferSize)
{
	return std::fgets(buffer, bufferSize, this->handle.get());
}

int FileOutsidePak::FGetC()
{
	return std::fgetc(this->handle.get());
}

int FileOutsidePak::FUnGetC(int ch)
{
	return std::ungetc(ch, this->handle.get());
}

int FileOutsidePak::FSeek(std::int64_t offset, int mode)
{
	return std::fseek(this->handle.get(), static_cast<long>(offset), mode);
}

std::int64_t FileOutsidePak::FTell() const
{
	return std::ftell(this->handle.get());
}

std::int64_t FileOutsidePak::GetSize() const
{
	const long pos = std::ftell(this->handle.get());
	std::fseek(this->handle.get(), 0, SEEK_END);
	const long size = std::ftell(this->handle.get());
	std::fseek(this->handle.get(), pos, SEEK_SET);

	return size;
}

int FileOutsidePak::FFlush()
{
	return std::fflush(this->handle.get());
}

int FileOutsidePak::FEof() const
{
	return std::feof(this->handle.get());
}

int FileOutsidePak::FError() const
{
	return std::ferror(this->handle.get());
}

void* FileOutsidePak::GetCachedFileData(std::size_t& fileSize)
{
	if (!this->data)
	{
		this->dataSize = static_cast<std::size_t>(this->GetSize());
		this->data = std::make_unique<std::byte[]>(this->dataSize);

		const long pos = std::ftell(this->handle.get());
		std::fseek(this->handle.get(), 0, SEEK_SET);
		std::fread(this->data.get(), this->dataSize, 1, this->handle.get());
		std::fseek(this->handle.get(), pos, SEEK_SET);
	}

	fileSize = this->dataSize;

	return this->data.get();
}

std::uint64_t FileOutsidePak::GetModificationTime()
{
	std::error_code ec;
	auto lastWriteTime = std::filesystem::last_write_time(this->path, ec);
	auto systemLastWriteTime = std::chrono::clock_cast<std::chrono::system_clock>(lastWriteTime);

	return std::chrono::system_clock::to_time_t(systemLastWriteTime);
}

std::FILE* FileOutsidePak::GetHandle()
{
	return this->handle.get();
}

std::size_t FileOutsidePak::GetCachedDataSize()
{
	return this->dataSize;
}

////////////////////////////////////////////////////////////////////////////////
