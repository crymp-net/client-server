#pragma once

#include <cstdint>
#include <forward_list>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

#include "CryCommon/CrySystem/ICryPak.h"
#include "Library/PathTree.h"
#include "Library/SlotVector.h"
#include "Library/StringTools.h"

#include "Pak/IPak.h"
#include "Pak/IPakFile.h"
#include "Pak/PakLoader.h"

class CryPak final : public ICryPak
{
	struct DirectorySlot
	{
		struct Entry
		{
			std::string name;
			std::uint64_t size;
			bool isDirectory = false;
			bool isInPak = false;
		};

		std::vector<Entry> entries;
		std::vector<Entry>::size_type pos = 0;
		std::uint16_t serial = 0;

		bool empty() const
		{
			return entries.empty();
		}

		void clear()
		{
			entries.clear();
			pos = 0;
		}
	};

	struct FileSlot
	{
		std::unique_ptr<IPakFile> impl;
		std::uint32_t pakHandle = 0;
		std::uint16_t serial = 0;

		bool empty() const
		{
			return impl == nullptr;
		}

		void clear()
		{
			impl = nullptr;
			pakHandle = 0;
		}
	};

	struct PakSlot
	{
		std::string path;
		std::string bindingRoot;
		std::unique_ptr<IPak> impl;
		std::int32_t refCount = 0;
		std::uint16_t serial = 0;

		bool empty() const
		{
			return impl == nullptr;
		}

		void clear()
		{
			path.clear();
			bindingRoot.clear();
			impl = nullptr;
		}
	};

	struct FileNode
	{
		struct FileInPak
		{
			std::uint32_t pakHandle = 0;
			std::uint32_t fileIndex = 0;
		};

		FileInPak current;
		std::forward_list<FileInPak> alternatives;
	};

	using Tree = PathTree<FileNode, StringTools::ComparatorNoCase>;
	using Redirects = PathTree<std::string, StringTools::ComparatorNoCase>;

	bool m_gameFolderWritable = false;
	bool m_lvlRes = false;

	std::recursive_mutex m_mutex;
	Redirects m_redirects;
	std::map<std::string, std::string, StringTools::ComparatorNoCase> m_aliases;
	SlotVector<DirectorySlot> m_directories;
	SlotVector<FileSlot> m_files;
	SlotVector<PakSlot> m_paks;
	PakLoader m_loader;
	Tree m_tree;

	_smart_ptr<IResourceList> m_resourceList_EngineStartup;
	_smart_ptr<IResourceList> m_resourceList_NextLevel;
	_smart_ptr<IResourceList> m_resourceList_Level;

	static CryPak s_globalInstance;

public:
	CryPak();
	~CryPak();

	// to be removed once we have our own CrySystem
	static CryPak& GetInstance()
	{
		return s_globalInstance;
	}

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

	size_t FReadRaw(void* data, size_t elementSize, size_t elementCount, FILE* handle) override;
	size_t FReadRawAll(void* data, size_t fileSize, FILE* handle) override;

	void* FGetCachedFileData(FILE* handle, size_t& fileSize) override;

	size_t FWrite(const void* data, size_t elementSize, size_t elementCount, FILE* handle) override;

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

	intptr_t FindFirst(const char* wildcard, struct _finddata_t* fd, unsigned int flags = 0) override;
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

	void SetGameFolderWritable(bool writable) { m_gameFolderWritable = writable; }
	bool IsGameFolderWritable() const { return m_gameFolderWritable; }

	void AddRedirect(std::string_view path, std::string_view newPath);
	void RemoveRedirect(std::string_view path);

private:
	std::string AdjustFileNameImplWithoutRedirect(std::string_view path, unsigned int flags);
	std::string AdjustFileNameImpl(std::string_view path, unsigned int flags);

	bool OpenPackImpl(std::string_view pakPath, std::string_view bindingRoot, unsigned int flags);
	bool OpenPacksImpl(std::string_view wildcardPath, std::string_view bindingRoot, unsigned int flags);
	bool ClosePackImpl(std::string_view pakPath, unsigned int flags);
	bool ClosePacksImpl(std::string_view wildcardPath, unsigned int flags);

	PakSlot* FindLoadedPakByPath(std::string_view pakPath);

	void IncrementPakRefCount(PakSlot* pak);
	void DecrementPakRefCount(PakSlot* pak);

	void ExpandAliases(std::string& path);

	void FillFindData(struct _finddata_t* fd, const DirectorySlot::Entry& entry);

	std::uint64_t GetFileSize(FileNode& fileNode);

	void AddPakToTree(PakSlot* pak);
	void RemovePakFromTree(PakSlot* pak);
};
