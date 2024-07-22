#pragma once

#include <cstdio>
#include <filesystem>
#include <memory>

#include "IFileInPak.h"

struct FileOutsidePak final : public IFileInPak
{
	struct Releaser
	{
		void operator()(std::FILE* file) const
		{
			std::fclose(file);
		}
	};

	using Handle = std::unique_ptr<std::FILE, Releaser>;

	Handle handle;
	std::filesystem::path path;

	std::unique_ptr<std::byte[]> data;
	std::size_t dataSize = 0;

	explicit FileOutsidePak(Handle&& handle, std::filesystem::path&& path)
	: handle(std::move(handle)), path(std::move(path))
	{
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
