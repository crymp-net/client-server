#pragma once

#include <cstdio>
#include <filesystem>
#include <memory>
#include <string>

#include "IPakFile.h"

class FileOutsidePak final : public IPakFile
{
	struct FileReleaser
	{
		void operator()(std::FILE* file) const
		{
			std::fclose(file);
		}
	};

	using FileHandle = std::unique_ptr<std::FILE, FileReleaser>;

	FileHandle m_handle;
	std::filesystem::path m_path;

	std::unique_ptr<std::byte[]> m_data;
	std::size_t m_dataSize = 0;

public:
	explicit FileOutsidePak(FileHandle&& handle, std::filesystem::path&& path)
	: m_handle(std::move(handle)), m_path(std::move(path))
	{
	}

	static std::unique_ptr<FileOutsidePak> TryOpen(
		const std::string& adjustedName,
		const std::string& mode,
		bool isWriting);

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
