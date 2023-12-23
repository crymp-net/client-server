#include <algorithm>

#include "Library/StringTools.h"

#include "CryLog.h"
#include "CryPak.h"

CryPak::CryPak()
{
}

CryPak::~CryPak()
{
}

////////////////////////////////////////////////////////////////////////////////
// ICryPak
////////////////////////////////////////////////////////////////////////////////

const char* CryPak::AdjustFileName(const char* src, char* dst, unsigned int flags, bool* foundInPak)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

bool CryPak::Init(const char* basePath)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

void CryPak::Release()
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
}

bool CryPak::OpenPack(const char* name, unsigned int flags)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

bool CryPak::OpenPack(const char* bindingRoot, const char* name, unsigned int flags)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

bool CryPak::ClosePack(const char* name, unsigned int flags)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

bool CryPak::OpenPacks(const char* wildcard, unsigned int flags)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

bool CryPak::OpenPacks(const char* bindingRoot, const char* wildcard, unsigned int flags)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

bool CryPak::ClosePacks(const char* wildcard, unsigned int flags)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

void CryPak::AddMod(const char* mod)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
}

void CryPak::RemoveMod(const char* mod)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
}

void CryPak::ParseAliases(const char* commandLine)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
}

void CryPak::SetAlias(const char* name, const char* alias, bool add)
{
	const std::string_view nameView = StringTools::SafeView(name);
	if (nameView.empty())
	{
		return;
	}

	const auto nameComparator = [nameView](const Alias& alias) -> bool
	{
		return StringTools::IsEqualNoCase(alias.name, nameView);
	};

	if (!add)
	{
		std::erase_if(m_aliases, nameComparator);
		return;
	}

	const std::string_view aliasView = StringTools::SafeView(alias);

	const auto it = std::find_if(m_aliases.begin(), m_aliases.end(), nameComparator);
	if (it != m_aliases.end())
	{
		it->value = aliasView;
		return;
	}

	Alias& a = m_aliases.emplace_back();
	a.name = nameView;
	a.value = aliasView;
}

const char* CryPak::GetAlias(const char* name, bool returnSame)
{
	const std::string_view nameView = StringTools::SafeView(name);

	const auto nameComparator = [nameView](const Alias& alias) -> bool
	{
		return StringTools::IsEqualNoCase(alias.name, nameView);
	};

	const auto it = std::find_if(m_aliases.begin(), m_aliases.end(), nameComparator);
	if (it != m_aliases.end())
	{
		return it->value.c_str();
	}

	return returnSame ? name : nullptr;
}

ICryPak::PakInfo* CryPak::GetPakInfo()
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

void CryPak::FreePakInfo(PakInfo* pakInfo)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
}

FILE* CryPak::FOpen(const char* name, const char* mode, unsigned int flags)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

FILE* CryPak::FOpen(const char* name, const char* mode, char* fileGamePath, int length)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

size_t CryPak::FReadRaw(void* data, size_t length, size_t elementCount, FILE* handle)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

size_t CryPak::FReadRawAll(void* data, size_t fileSize, FILE* handle)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

void* CryPak::FGetCachedFileData(FILE* handle, size_t& fileSize)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

size_t CryPak::FWrite(const void* data, size_t length, size_t elementCount, FILE* handle)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

int CryPak::FPrintf(FILE* handle, const char* format, ...)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

char* CryPak::FGets(char* buffer, int bufferSize, FILE* handle)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

int CryPak::Getc(FILE* handle)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

size_t CryPak::FGetSize(FILE* handle)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

int CryPak::Ungetc(int ch, FILE* handle)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

bool CryPak::IsInPak(FILE* handle)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

bool CryPak::RemoveFile(const char* name)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

bool CryPak::RemoveDir(const char* name, bool recurse)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

bool CryPak::IsAbsPath(const char* path)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

size_t CryPak::FSeek(FILE* handle, long seek, int mode)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

long CryPak::FTell(FILE* handle)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

int CryPak::FClose(FILE* handle)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

int CryPak::FEof(FILE* handle)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

int CryPak::FError(FILE* handle)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

int CryPak::FGetErrno()
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

int CryPak::FFlush(FILE* handle)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

void* CryPak::PoolMalloc(size_t size)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

void CryPak::PoolFree(void* pool)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
}

intptr_t CryPak::FindFirst(const char* dir, struct _finddata_t* fd, unsigned int flags)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

int CryPak::FindNext(intptr_t handle, struct _finddata_t* fd)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

int CryPak::FindClose(intptr_t handle)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

ICryPak::FileTime CryPak::GetModificationTime(FILE* handle)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

bool CryPak::IsFileExist(const char* name)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

bool CryPak::MakeDir(const char* path)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

ICryArchive* CryPak::OpenArchive(const char* path, unsigned int flags)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

const char* CryPak::GetFileArchivePath(FILE* handle)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

int CryPak::RawCompress(const void* uncompressed, unsigned long* dstSize, void* compressed, unsigned long srcSize, int level)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

int CryPak::RawUncompress(void* uncompressed, unsigned long* dstSize, const void* compressed, unsigned long srcSize)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

void CryPak::RecordFileOpen(const ERecordFileOpenList list)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
}

void CryPak::RecordFile(FILE* handle, const char* name)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
}

IResourceList* CryPak::GetRecorderdResourceList(const ERecordFileOpenList list)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

ICryPak::ERecordFileOpenList CryPak::GetRecordFileOpenList()
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

void CryPak::Notify(ENotifyEvent event)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
}

std::uint32_t CryPak::ComputeCRC(const char* path)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

bool CryPak::ComputeMD5(const char* path, unsigned char* md5)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

void CryPak::RegisterFileAccessSink(ICryPakFileAcesssSink* pSink)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
}

void CryPak::UnregisterFileAccessSink(ICryPakFileAcesssSink* pSink)
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
}

bool CryPak::GetLvlResStatus() const
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

const char* CryPak::GetModDir() const
{
	CryLogErrorAlways("[%s] NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

////////////////////////////////////////////////////////////////////////////////
