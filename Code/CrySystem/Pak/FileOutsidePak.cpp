#include <chrono>

#include "FileOutsidePak.h"

////////////////////////////////////////////////////////////////////////////////
// IPakFile
////////////////////////////////////////////////////////////////////////////////

std::size_t FileOutsidePak::FRead(void* buffer, std::size_t elementSize, std::size_t elementCount)
{
	return std::fread(buffer, elementSize, elementCount, m_file.get());
}

std::size_t FileOutsidePak::FWrite(const void* buffer, std::size_t elementSize, std::size_t elementCount)
{
	return std::fwrite(buffer, elementSize, elementCount, m_file.get());
}

int FileOutsidePak::VFPrintF(const char* format, va_list args)
{
	return std::vfprintf(m_file.get(), format, args);
}

char* FileOutsidePak::FGetS(char* buffer, int bufferSize)
{
	return std::fgets(buffer, bufferSize, m_file.get());
}

int FileOutsidePak::FGetC()
{
	return std::fgetc(m_file.get());
}

int FileOutsidePak::FUnGetC(int ch)
{
	return std::ungetc(ch, m_file.get());
}

int FileOutsidePak::FSeek(std::int64_t offset, int mode)
{
	return std::fseek(m_file.get(), static_cast<long>(offset), mode);
}

std::int64_t FileOutsidePak::FTell() const
{
	return std::ftell(m_file.get());
}

std::int64_t FileOutsidePak::GetSize() const
{
	const long pos = std::ftell(m_file.get());
	std::fseek(m_file.get(), 0, SEEK_END);
	const long size = std::ftell(m_file.get());
	std::fseek(m_file.get(), pos, SEEK_SET);

	return size;
}

int FileOutsidePak::FFlush()
{
	return std::fflush(m_file.get());
}

int FileOutsidePak::FEof() const
{
	return std::feof(m_file.get());
}

int FileOutsidePak::FError() const
{
	return std::ferror(m_file.get());
}

void* FileOutsidePak::GetCachedFileData(std::size_t& fileSize)
{
	if (!m_data)
	{
		m_dataSize = static_cast<std::size_t>(this->GetSize());
		m_data = std::make_unique<std::byte[]>(m_dataSize);

		const long pos = std::ftell(m_file.get());
		std::fseek(m_file.get(), 0, SEEK_SET);
		std::fread(m_data.get(), m_dataSize, 1, m_file.get());
		std::fseek(m_file.get(), pos, SEEK_SET);
	}

	fileSize = m_dataSize;

	return m_data.get();
}

std::uint64_t FileOutsidePak::GetModificationTime()
{
	std::error_code ec;
	auto lastWriteTime = std::filesystem::last_write_time(m_path, ec);
	auto systemLastWriteTime = std::chrono::clock_cast<std::chrono::system_clock>(lastWriteTime);

	return std::chrono::system_clock::to_time_t(systemLastWriteTime);
}

////////////////////////////////////////////////////////////////////////////////
