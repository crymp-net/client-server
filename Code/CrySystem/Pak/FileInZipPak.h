#pragma once

#include "IFileInPak.h"

struct FileInZipPak final : public IFileInPak
{
	std::byte* data = nullptr;
	std::size_t size = 0;
	std::size_t pos = 0;
	std::uint64_t modificationTime = 0;
	bool isBinary = false;

	////////////////////////////////////////////////////////////////////////////////
	// IFileInPak
	////////////////////////////////////////////////////////////////////////////////

	std::size_t FRead(void* buffer, std::size_t elementSize, std::size_t elementCount) override;
	std::size_t FWrite(const void* buffer, std::size_t elementSize, std::size_t elementCount) override;

	int VFPrintF(const char* format, va_list args) override;

	char* FGetS(char* buffer, int bufferSize) override;
	int FGetC() override;
	int FUnGetC(int ch) override;

	int FSeek(long offset, int mode) override;
	long FTell() const override;

	std::uint64_t GetSize() const override;

	int FFlush() override;

	int FEof() const override;
	int FError() const override;

	void* GetCachedFileData(std::size_t& fileSize) override;

	std::uint64_t GetModificationTime() override;

	std::FILE* GetHandle() override;

	////////////////////////////////////////////////////////////////////////////////
};
