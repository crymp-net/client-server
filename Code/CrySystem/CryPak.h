#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "CryCommon/CrySystem/ICryPak.h"

struct IPak
{
	virtual ~IPak() = default;
};

class CryPak final : public ICryPak
{
	struct Alias
	{
		std::string name;
		std::string value;
	};

	std::vector<Alias> m_aliases;
	std::vector<std::unique_ptr<IPak>> m_paks;

public:
	CryPak();
	~CryPak();

	////////////////////////////////////////////////////////////////////////////////
	// ICryPak
	////////////////////////////////////////////////////////////////////////////////

	const char* AdjustFileName(const char* src, char* dst, unsigned int flags, bool* foundInPak = nullptr) override;

	bool Init(const char* basePath) override;
	void Release() override;

	bool OpenPack(const char* name, unsigned int flags = FLAGS_PATH_REAL) override;
	bool OpenPack(const char* bindingRoot, const char* name, unsigned int flags = FLAGS_PATH_REAL) override;
	bool ClosePack(const char* name, unsigned int flags = FLAGS_PATH_REAL) override;
	bool OpenPacks(const char* wildcard, unsigned int flags = FLAGS_PATH_REAL) override;
	bool OpenPacks(const char* bindingRoot, const char* wildcard, unsigned int flags = FLAGS_PATH_REAL) override;
	bool ClosePacks(const char* wildcard, unsigned int flags = FLAGS_PATH_REAL) override;

	void AddMod(const char* mod) override;
	void RemoveMod(const char* mod) override;

	void ParseAliases(const char* commandLine) override;
	void SetAlias(const char* name, const char* alias, bool add) override;
	const char* GetAlias(const char* name, bool returnSame = true) override;

	PakInfo* GetPakInfo() override;
	void FreePakInfo(PakInfo* pakInfo) override;

	FILE* FOpen(const char* name, const char* mode, unsigned int flags = 0) override;
	FILE* FOpen(const char* name, const char* mode, char* fileGamePath, int length) override;

	size_t FReadRaw(void* data, size_t length, size_t elementCount, FILE* handle) override;
	size_t FReadRawAll(void* data, size_t fileSize, FILE* handle) override;

	void* FGetCachedFileData(FILE* handle, size_t& fileSize) override;

	size_t FWrite(const void* data, size_t length, size_t elementCount, FILE* handle) override;

	int FPrintf(FILE* handle, const char* format, ...) override;
	char* FGets(char* buffer, int bufferSize, FILE* handle) override;
	int Getc(FILE* handle) override;
	size_t FGetSize(FILE* handle) override;
	int Ungetc(int ch, FILE* handle) override;
	bool IsInPak(FILE* handle) override;
	bool RemoveFile(const char* name) override;
	bool RemoveDir(const char* name, bool recurse) override;
	bool IsAbsPath(const char* path) override;
	size_t FSeek(FILE* handle, long seek, int mode) override;
	long FTell(FILE* handle) override;
	int FClose(FILE* handle) override;
	int FEof(FILE* handle) override;
	int FError(FILE* handle) override;
	int FGetErrno() override;
	int FFlush(FILE* handle) override;

	void* PoolMalloc(size_t size) override;
	void PoolFree(void* pool) override;

	intptr_t FindFirst(const char* dir, struct _finddata_t* fd, unsigned int flags = 0) override;
	int FindNext(intptr_t handle, struct _finddata_t* fd) override;
	int FindClose(intptr_t handle) override;

	FileTime GetModificationTime(FILE* handle) override;

	bool IsFileExist(const char* name) override;

	bool MakeDir(const char* path) override;

	ICryArchive* OpenArchive(const char* path, unsigned int flags = FLAGS_PATH_REAL) override;

	const char* GetFileArchivePath(FILE* handle) override;

	int RawCompress(const void* uncompressed, unsigned long* dstSize, void* compressed, unsigned long srcSize, int level = -1) override;
	int RawUncompress(void* uncompressed, unsigned long* dstSize, const void* compressed, unsigned long srcSize) override;

	void RecordFileOpen(const ERecordFileOpenList list) override;
	void RecordFile(FILE* handle, const char* name) override;
	IResourceList* GetRecorderdResourceList(const ERecordFileOpenList list) override;
	ERecordFileOpenList GetRecordFileOpenList() override;

	void Notify(ENotifyEvent event) override;

	std::uint32_t ComputeCRC(const char* path) override;
	bool ComputeMD5(const char* path, unsigned char* md5) override;

	void RegisterFileAccessSink(ICryPakFileAcesssSink* pSink) override;
	void UnregisterFileAccessSink(ICryPakFileAcesssSink* pSink) override;

	bool GetLvlResStatus() const override;

	const char* GetModDir() const override;

	////////////////////////////////////////////////////////////////////////////////
};
