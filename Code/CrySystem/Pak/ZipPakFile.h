#pragma once

#include "IPakFile.h"

class ZipPakFile final : public IPakFile
{
	std::byte* m_data = nullptr;
	std::size_t m_size = 0;
	std::int64_t m_pos = 0;
	std::uint64_t m_modificationTime = 0;
	bool m_isBinary = false;

public:
	explicit ZipPakFile(void* data, std::size_t size, std::uint64_t modificationTime, bool isBinary)
	: m_data(static_cast<std::byte*>(data)), m_size(size), m_modificationTime(modificationTime),
	  m_isBinary(isBinary) {}
	ZipPakFile(const ZipPakFile&) = delete;
	ZipPakFile& operator=(const ZipPakFile&) = delete;
	~ZipPakFile();

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
