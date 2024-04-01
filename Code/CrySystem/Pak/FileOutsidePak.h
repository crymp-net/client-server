#pragma once

#include <cstdio>
#include <filesystem>
#include <memory>

#include "IPakFile.h"

class FileOutsidePak final : public IPakFile
{
	struct FileReleaser
	{
		void operator()(std::FILE* file)
		{
			std::fclose(file);
		}
	};

	std::unique_ptr<std::FILE, FileReleaser> m_file;

	std::filesystem::path m_path;

	std::unique_ptr<std::byte[]> m_data;
	std::size_t m_dataSize = 0;

public:
	explicit FileOutsidePak(std::FILE* file, const std::filesystem::path& path)
	: m_file(file), m_path(std::move(path)) {}

	////////////////////////////////////////////////////////////////////////////////
	// IPakFile
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

	////////////////////////////////////////////////////////////////////////////////
};
