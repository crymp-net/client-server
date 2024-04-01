#pragma once

#include <cstdarg>
#include <cstddef>
#include <cstdint>

struct IPakFile
{
	virtual ~IPakFile() = default;

	virtual std::size_t FRead(void* buffer, std::size_t elementSize, std::size_t elementCount) = 0;
	virtual std::size_t FWrite(const void* buffer, std::size_t elementSize, std::size_t elementCount) = 0;

	virtual int VFPrintF(const char* format, va_list args) = 0;

	virtual char* FGetS(char* buffer, int bufferSize) = 0;
	virtual int FGetC() = 0;
	virtual int FUnGetC(int ch) = 0;

	virtual int FSeek(std::int64_t offset, int mode) = 0;
	virtual std::int64_t FTell() const = 0;
	virtual std::int64_t GetSize() const = 0;

	virtual int FFlush() = 0;

	virtual int FEof() const = 0;
	virtual int FError() const = 0;

	virtual void* GetCachedFileData(std::size_t& fileSize) = 0;

	virtual std::uint64_t GetModificationTime() = 0;
};
