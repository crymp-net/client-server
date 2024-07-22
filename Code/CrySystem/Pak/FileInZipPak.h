#pragma once

#include <cstdlib>
#include <memory>

#include "IFileInPak.h"

struct FileInZipPak final : public IFileInPak
{
	struct DataReleaser
	{
		void operator()(void* data) const
		{
			// release memory allocated by mz_zip_reader_extract_to_heap
			std::free(data);
		}
	};

	using Data = std::unique_ptr<void, DataReleaser>;

	Data data;
	std::size_t size = 0;
	std::int64_t pos = 0;
	std::uint64_t modificationTime = 0;
	bool isBinary = false;

	std::byte* GetBytes()
	{
		return static_cast<std::byte*>(this->data.get());
	}

	////////////////////////////////////////////////////////////////////////////////
	// IFileInPak
	////////////////////////////////////////////////////////////////////////////////

	std::size_t FRead(void* buffer, std::size_t elementSize, std::size_t elementCount) override;
	std::size_t FWrite(const void* buffer, std::size_t elementSize, std::size_t elementCount) override;

	int VFPrintF(const char* format, va_list args) override;

	char* FGetS(char* buffer, int bufferSize) override;
	int FGetC() override;
	int FUnGetC(int ch) override;

	int FSeek(std::int64_t offset, int mode) override;
	std::int64_t FTell() const override;
	std::int64_t GetSize() const override;

	int FFlush() override;

	int FEof() const override;
	int FError() const override;

	void* GetCachedFileData(std::size_t& fileSize) override;

	std::uint64_t GetModificationTime() override;

	std::FILE* GetHandle() override;
	std::size_t GetCachedDataSize() override;

	////////////////////////////////////////////////////////////////////////////////
};
