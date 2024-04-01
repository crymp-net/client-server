#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <stack>
#include <tuple>

#include "CryLog.h"
#include "CryPak.h"
#include "Pak/FileOutsidePak.h"
#include "ResourceList.h"

// TODO: CryLog and std::string_view::data()

static bool IsSlash(char ch)
{
	return ch == '/' || ch == '\\';
}

static bool IsAbsolutePath(std::string_view path)
{
	if (path.length() > 0 && IsSlash(path[0]))
	{
		// also handles UNC paths
		return true;
	}

	if (path.length() > 1 && path[1] == ':')
	{
		const char drive = path[0];

		if ((drive >= 'A' && drive <= 'Z') || (drive >= 'a' && drive <= 'z'))
		{
			return true;
		}
	}

	return false;
}

static std::string_view RemoveLeadingSlashes(std::string_view path)
{
	while (!path.empty() && IsSlash(path.front()))
	{
		path.remove_prefix(1);
	}

	return path;
}

static std::string_view RemoveTrailingSlashes(std::string_view path)
{
	while (!path.empty() && IsSlash(path.back()))
	{
		path.remove_suffix(1);
	}

	return path;
}

static std::string_view DirPath(std::string_view path)
{
	path = RemoveTrailingSlashes(path);

	while (!path.empty() && !IsSlash(path.back()))
	{
		path.remove_suffix(1);
	}

	return RemoveTrailingSlashes(path);
}

static std::string_view FileName(std::string_view path)
{
	path = RemoveTrailingSlashes(path);

	const auto pos = path.find_last_of("/\\");
	if (pos != std::string_view::npos)
	{
		path.remove_prefix(pos + 1);
	}

	return path;
}

static std::tuple<std::string_view, std::string_view> SplitFirstPathComponent(std::string_view path)
{
	path = RemoveLeadingSlashes(path);

	std::string_view remaining;

	const auto pos = path.find_first_of("/\\");
	if (pos != std::string_view::npos)
	{
		remaining = path;
		remaining.remove_prefix(pos + 1);
		path.remove_suffix(path.length() - pos);
	}

	return {path, remaining};
}

static std::tuple<std::string_view, std::string_view> SplitPathIntoDirAndFile(std::string_view path)
{
	std::string_view dirPath = RemoveTrailingSlashes(path);
	std::string_view fileName = dirPath;

	while (!dirPath.empty() && !IsSlash(dirPath.back()))
	{
		dirPath.remove_suffix(1);
	}

	fileName.remove_prefix(dirPath.length());

	dirPath = RemoveTrailingSlashes(dirPath);

	return {dirPath, fileName};
}

static std::string_view PathStem(std::string_view path)
{
	path = FileName(path);

	const auto pos = path.rfind('.');
	if (pos != std::string_view::npos && pos != 0)
	{
		path.remove_suffix(path.length() - pos);
	}

	return path;
}

static std::string_view PathExtension(std::string_view path)
{
	path = FileName(path);

	const auto pos = path.rfind('.');
	if (pos != std::string_view::npos && pos != 0)
	{
		path.remove_prefix(pos);
	}
	else
	{
		path.remove_prefix(path.length());
	}

	return path;
}

static std::tuple<std::string_view, std::string_view> SplitNameIntoStemAndExtension(std::string_view name)
{
	std::string_view stem = name;
	std::string_view extension;

	const auto pos = stem.rfind('.');
	if (pos != std::string_view::npos && pos != 0)
	{
		extension = stem;
		extension.remove_prefix(pos);
		stem.remove_suffix(stem.length() - pos);
	}

	return {stem, extension};
}

static std::tuple<std::string_view, std::string_view> SplitPathIntoStemAndExtension(std::string_view path)
{
	return SplitNameIntoStemAndExtension(FileName(path));
}

static void AddTrailingSlash(std::string& path)
{
	if (!path.empty() && !IsSlash(path.back()))
	{
		path += '/';
	}
}

class CryPakWildcardMatcher
{
	enum class Wildcard
	{
		NONE, STEM, EXTENSION, FULL
	};

	Wildcard m_kind = Wildcard::NONE;
	std::string_view m_stem;
	std::string_view m_extension;

public:
	// | wildcard | kind      |
	// | :------: | :-------: |
	// | `xxx`    | NONE      |
	// | `*.xxx`  | STEM      |
	// | `xxx.*`  | EXTENSION |
	// | `*.*`    | FULL      |
	// | `*`      | FULL      |
	explicit CryPakWildcardMatcher(std::string_view wildcard)
	{
		std::tie(m_stem, m_extension) = SplitNameIntoStemAndExtension(wildcard);

		if (!m_extension.empty())
		{
			// remove dot
			m_extension.remove_prefix(1);
		}

		if (!m_stem.empty() && m_stem.back() == '*')
		{
			m_stem.remove_suffix(1);
			m_kind = Wildcard::STEM;
		}

		if (!m_extension.empty() && m_extension.back() == '*')
		{
			m_extension.remove_suffix(1);
			m_kind = (m_kind == Wildcard::STEM) ? Wildcard::FULL : Wildcard::EXTENSION;
		}

		if (m_stem.find_first_of("*?") != std::string_view::npos
		 || m_extension.find_first_of("*?") != std::string_view::npos)
		{
			CryLogAlways("%s(\"%.*s\"): Unsupported wildcard", __FUNCTION__,
				static_cast<int>(wildcard.length()), wildcard.data());
		}
	}

	bool operator()(std::string_view name) const
	{
		auto [stem, extension] = SplitNameIntoStemAndExtension(name);

		if (!extension.empty())
		{
			// remove dot
			extension.remove_prefix(1);
		}

		const auto check = [](std::string_view a, std::string_view b, bool prefix) -> bool
		{
			if (prefix)
			{
				return StringTools::StartsWithNoCase(a, b);
			}
			else
			{
				return StringTools::IsEqualNoCase(a, b);
			}
		};

		if (!check(stem, m_stem, m_kind == Wildcard::STEM || m_kind == Wildcard::FULL))
		{
			return false;
		}

		if (!check(extension, m_extension, m_kind == Wildcard::EXTENSION || m_kind == Wildcard::FULL))
		{
			return false;
		}

		return true;
	}
};

static std::vector<std::string> ExpandWildcardFilesystemPath(std::string_view wildcardPath)
{
	const auto [dirPath, wildcardName] = SplitPathIntoDirAndFile(wildcardPath);
	const CryPakWildcardMatcher matcher(wildcardName);

	std::vector<std::string> foundPaths;

	std::error_code ec;
	for (const auto& entry : std::filesystem::directory_iterator(dirPath, ec))
	{
		std::string path = entry.path().generic_string();
		if (matcher(FileName(path)))
		{
			// no need to adjust because generic_string uses forward slashes
			foundPaths.emplace_back(std::move(path));
		}
	}

	std::sort(foundPaths.begin(), foundPaths.end(), StringTools::ComparatorNoCase());

	return foundPaths;
}

CryPak CryPak::s_globalInstance;

CryPak::CryPak()
{
	m_resourceList_EngineStartup = ResourceList::Create();
	m_resourceList_NextLevel = ResourceList::Create();
	m_resourceList_Level = ResourceList::Create();
}

CryPak::~CryPak()
{
}

////////////////////////////////////////////////////////////////////////////////
// ICryPak
////////////////////////////////////////////////////////////////////////////////

const char* CryPak::AdjustFileName(const char* src, char* dst, unsigned int flags, bool* foundInPak)
{
	if (foundInPak)
	{
		CryLogAlways("%s(\"%s\"): Ignoring foundInPak", __FUNCTION__, src);
		*foundInPak = false;
	}

	// no need to lock m_mutex
	const std::string result = this->AdjustFileNameImpl(StringTools::SafeView(src), flags);

	// we don't know the actual size of the dst buffer
	// it's supposed to be ICryPak::g_nMaxPath, which is 2048, but it's not always the case!
	// let's hope all buffers out there are at least MAX_PATH, which is 260
	if (result.length() < 260)
	{
		std::memcpy(dst, result.c_str(), result.length() + 1);
	}
	else
	{
		CryLogAlways("%s(\"%s\"): Too long path", __FUNCTION__, src);
		dst[0] = '\0';
	}

	return dst;
}

bool CryPak::Init(const char* basePath)
{
	return true;
}

void CryPak::Release()
{
}

bool CryPak::OpenPack(const char* name, unsigned int flags)
{
	std::lock_guard lock(m_mutex);

	const std::string adjustedName = this->AdjustFileNameImpl(StringTools::SafeView(name), flags);
	const std::string_view adjustedBindingRoot = DirPath(adjustedName);

	return this->OpenPackImpl(adjustedName, adjustedBindingRoot, flags);
}

bool CryPak::OpenPack(const char* bindingRoot, const char* name, unsigned int flags)
{
	std::lock_guard lock(m_mutex);

	const std::string adjustedName = this->AdjustFileNameImpl(StringTools::SafeView(name), flags);
	const std::string adjustedBindingRoot = this->AdjustFileNameImpl(StringTools::SafeView(bindingRoot), flags);

	return this->OpenPackImpl(adjustedName, adjustedBindingRoot, flags);
}

bool CryPak::ClosePack(const char* name, unsigned int flags)
{
	std::lock_guard lock(m_mutex);

	const std::string adjustedName = this->AdjustFileNameImpl(StringTools::SafeView(name), flags);

	return this->ClosePackImpl(adjustedName, flags);
}

bool CryPak::OpenPacks(const char* wildcard, unsigned int flags)
{
	std::lock_guard lock(m_mutex);

	const std::string adjustedWildcard = this->AdjustFileNameImpl(StringTools::SafeView(wildcard), flags);
	const std::string_view adjustedBindingRoot = DirPath(adjustedWildcard);

	return this->OpenPacksImpl(adjustedWildcard, adjustedBindingRoot, flags);
}

bool CryPak::OpenPacks(const char* bindingRoot, const char* wildcard, unsigned int flags)
{
	std::lock_guard lock(m_mutex);

	const std::string adjustedWildcard = this->AdjustFileNameImpl(StringTools::SafeView(wildcard), flags);
	const std::string adjustedBindingRoot = this->AdjustFileNameImpl(StringTools::SafeView(bindingRoot), flags);

	return this->OpenPacksImpl(adjustedWildcard, adjustedBindingRoot, flags);
}

bool CryPak::ClosePacks(const char* wildcard, unsigned int flags)
{
	std::lock_guard lock(m_mutex);

	const std::string adjustedWildcard = this->AdjustFileNameImpl(StringTools::SafeView(wildcard), flags);

	return this->ClosePacksImpl(adjustedWildcard, flags);
}

void CryPak::AddMod(const char* mod)
{
	CryLogAlways("%s(\"%s\"): NOT IMPLEMENTED!", __FUNCTION__, mod);
}

void CryPak::RemoveMod(const char* mod)
{
	CryLogAlways("%s(\"%s\"): NOT IMPLEMENTED!", __FUNCTION__, mod);
}

void CryPak::ParseAliases(const char* commandLine)
{
	CryLogAlways("%s: NOT IMPLEMENTED!", __FUNCTION__);
}

void CryPak::SetAlias(const char* name, const char* alias, bool add)
{
	std::lock_guard lock(m_mutex);

	const std::string_view aliasName = StringTools::SafeView(name);
	if (aliasName.length() < 3 || aliasName.front() != '%' || aliasName.back() != '%')
	{
		CryLogAlways("%s: Invalid alias name \"%s\"", __FUNCTION__, name);
		return;
	}

	if (add)
	{
		const auto [it, added] = m_aliases.emplace(aliasName, "");

		if (added)
		{
			CryLogAlways("%s: Adding \"%s\" -> \"%s\"", __FUNCTION__, name, alias);
		}
		else
		{
			CryLogAlways("%s: Changing \"%s\" -> \"%s\" to \"%s\"", __FUNCTION__,
				name, it->second.c_str(), alias);
		}

		it->second = StringTools::SafeView(alias);
	}
	else
	{
		const auto it = m_aliases.find(aliasName);
		if (it == m_aliases.end())
		{
			CryLogAlways("%s: Missing \"%s\" to remove", __FUNCTION__, name);
			return;
		}

		CryLogAlways("%s: Removing \"%s\" -> \"%s\"", __FUNCTION__, name, it->second.c_str());

		m_aliases.erase(it);
	}
}

const char* CryPak::GetAlias(const char* name, bool returnSame)
{
	std::lock_guard lock(m_mutex);

	const auto it = m_aliases.find(StringTools::SafeView(name));
	if (it == m_aliases.end())
	{
		return returnSame ? name : nullptr;
	}

	return it->second.c_str();
}

ICryPak::PakInfo* CryPak::GetPakInfo()
{
	std::lock_guard lock(m_mutex);

	const unsigned int pakCount = m_paks.GetActiveCount();

	PakInfo* info = static_cast<PakInfo*>(std::calloc(1, sizeof(PakInfo) + (sizeof(PakInfo::Pak) * pakCount)));
	info->numOpenPaks = pakCount;

	const auto my_strdup = [](std::string_view str) -> char*
	{
		char* res = static_cast<char*>(std::malloc(str.length() + 1));
		std::memcpy(res, str.data(), str.length());
		res[str.length()] = '\0';
		return res;
	};

	PakSlot* slot = m_paks.GetFirstActive();

	for (unsigned int i = 0; i < pakCount; ++i)
	{
		info->arrPaks[i].szFilePath = my_strdup(slot->path);
		info->arrPaks[i].szBindRoot = my_strdup(slot->bindingRoot);
		info->arrPaks[i].nUsedMem = 0;

		slot = m_paks.GetNextActive(slot);
	}

	return info;
}

void CryPak::FreePakInfo(PakInfo* pakInfo)
{
	const unsigned int pakCount = pakInfo->numOpenPaks;

	for (unsigned int i = 0; i < pakCount; ++i)
	{
		std::free(const_cast<char*>(pakInfo->arrPaks[i].szFilePath));
		std::free(const_cast<char*>(pakInfo->arrPaks[i].szBindRoot));
	}

	std::free(pakInfo);
}

FILE* CryPak::FOpen(const char* name, const char* mode, unsigned int flags)
{
	std::lock_guard lock(m_mutex);

	const std::string adjustedName = this->AdjustFileNameImpl(StringTools::SafeView(name), flags);

	// TODO: r+
	bool isWriting = false;
	bool isBinary = false;
	for (char ch : StringTools::SafeView(mode))
	{
		switch (ch)
		{
			case 'r': break;
			case 'w': isWriting = true; break;
			case 'a': isWriting = true; break;
			case 'b': isBinary = true; break;
			case 't': isBinary = false; break;
			case 'x': break;
			case '+': break;
			default:
			{
				CryLogAlways("%s(\"%s\", \"%s\", 0x%x): Unknown mode char '%c'", __FUNCTION__, name, mode, flags, ch);
				break;
			}
		}
	}

	TreeNode::FileInPak* fileInPak = this->FindFileInTree(adjustedName);
	if (fileInPak)
	{
		PakSlot* pak = m_paks.HandleToSlot(fileInPak->pakHandle);
		if (!pak)
		{
			CryLogAlways("%s(\"%s\", \"%s\", 0x%x): Invalid pak handle", __FUNCTION__, name, mode, flags);
			return nullptr;
		}

		std::unique_ptr<IPakFile> file = pak->impl->OpenFile(fileInPak->fileIndex, isBinary);
		if (!file)
		{
			CryLogAlways("%s(\"%s\", \"%s\", 0x%x): Opening a file in a pak failed", __FUNCTION__, name, mode, flags);
			return nullptr;
		}

		if (isWriting)
		{
			CryLogAlways("%s(\"%s\", \"%s\", 0x%x): Writing to paks is not supported", __FUNCTION__, name, mode, flags);
			return nullptr;
		}

		FileSlot* slot = m_files.GetFreeSlot();
		if (!slot)
		{
			CryLogAlways("%s(\"%s\", \"%s\", 0x%x): Max number of slots reached", __FUNCTION__, name, mode, flags);
			return nullptr;
		}

		slot->impl = std::move(file);
		slot->pakHandle = m_paks.SlotToHandle(pak);
		this->IncrementPakRefCount(pak);
		FILE* handle = reinterpret_cast<FILE*>(m_files.SlotToHandle(slot));
		CryLogAlways("%s(\"%s\", \"%s\", 0x%x): 0x%p Found in pak \"%s\"", __FUNCTION__, name, mode, flags, handle, pak->path.c_str());
		return handle;
	}

	const std::filesystem::path filePath(adjustedName);

	if (isWriting)
	{
		std::error_code ec;
		std::filesystem::create_directories(filePath.parent_path(), ec);
	}

	std::string newMode = StringTools::SafeString(mode);
	newMode.erase(std::remove_if(newMode.begin(), newMode.end(),
		[](char ch) -> bool
		{
			return std::string_view("rwab+").find(ch) == std::string_view::npos;
		}
	), newMode.end());

	if (newMode != StringTools::SafeView(mode))
	{
		CryLogAlways("%s(\"%s\", \"%s\", 0x%x): newMode=\"%s\"", __FUNCTION__, name, mode, flags, newMode.c_str());
	}

	std::FILE* file = std::fopen(adjustedName.c_str(), newMode.c_str());
	if (file)
	{
		FileSlot* slot = m_files.GetFreeSlot();
		if (!slot)
		{
			CryLogAlways("%s(\"%s\", \"%s\", 0x%x): Max number of slots reached", __FUNCTION__, name, mode, flags);
			return nullptr;
		}

		slot->impl = std::make_unique<FileOutsidePak>(file, filePath);
		slot->pakHandle = 0;
		FILE* handle = reinterpret_cast<FILE*>(m_files.SlotToHandle(slot));
		CryLogAlways("%s(\"%s\", \"%s\", 0x%x): 0x%p Found outside in \"%s\"", __FUNCTION__, name, mode, flags, handle, adjustedName.c_str());
		return handle;
	}

	CryLogAlways("%s(\"%s\", \"%s\", 0x%x): Not found", __FUNCTION__, name, mode, flags);
	return nullptr;
}

FILE* CryPak::FOpen(const char* name, const char* mode, char* fileGamePath, int length)
{
	CryLogAlways("%s(\"%s\", \"%s\"): NOT IMPLEMENTED!", __FUNCTION__, name, mode);
	return nullptr;
}

size_t CryPak::FReadRaw(void* data, size_t elementSize, size_t elementCount, FILE* handle)
{
	std::lock_guard lock(m_mutex);

	FileSlot* file = m_files.HandleToSlot(reinterpret_cast<std::uint32_t>(handle));
	if (!file)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, handle);
		return 0;
	}

	return file->impl->FRead(data, elementSize, elementCount);
}

size_t CryPak::FReadRawAll(void* data, size_t fileSize, FILE* handle)
{
	std::lock_guard lock(m_mutex);

	FileSlot* file = m_files.HandleToSlot(reinterpret_cast<std::uint32_t>(handle));
	if (!file)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, handle);
		return 0;
	}

	if (file->impl->GetSize() != fileSize)
	{
		return 0;
	}

	if (file->impl->FSeek(0, SEEK_SET) < 0)
	{
		return 0;
	}

	return file->impl->FRead(data, 1, fileSize);
}

void* CryPak::FGetCachedFileData(FILE* handle, size_t& fileSize)
{
	std::lock_guard lock(m_mutex);

	FileSlot* file = m_files.HandleToSlot(reinterpret_cast<std::uint32_t>(handle));
	if (!file)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, handle);
		return nullptr;
	}

	return file->impl->GetCachedFileData(fileSize);
}

size_t CryPak::FWrite(const void* data, size_t elementSize, size_t elementCount, FILE* handle)
{
	std::lock_guard lock(m_mutex);

	FileSlot* file = m_files.HandleToSlot(reinterpret_cast<std::uint32_t>(handle));
	if (!file)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, handle);
		return 0;
	}

	return file->impl->FWrite(data, elementSize, elementCount);
}

int CryPak::FPrintf(FILE* handle, const char* format, ...)
{
	std::lock_guard lock(m_mutex);

	FileSlot* file = m_files.HandleToSlot(reinterpret_cast<std::uint32_t>(handle));
	if (!file)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, handle);
		return -1;
	}

	va_list args;
	va_start(args, format);
	const int length = file->impl->VFPrintF(format, args);
	va_end(args);

	return length;
}

char* CryPak::FGets(char* buffer, int bufferSize, FILE* handle)
{
	std::lock_guard lock(m_mutex);

	FileSlot* file = m_files.HandleToSlot(reinterpret_cast<std::uint32_t>(handle));
	if (!file)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, handle);
		return nullptr;
	}

	return file->impl->FGetS(buffer, bufferSize);
}

int CryPak::Getc(FILE* handle)
{
	std::lock_guard lock(m_mutex);

	FileSlot* file = m_files.HandleToSlot(reinterpret_cast<std::uint32_t>(handle));
	if (!file)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, handle);
		return EOF;
	}

	return file->impl->FGetC();
}

size_t CryPak::FGetSize(FILE* handle)
{
	std::lock_guard lock(m_mutex);

	FileSlot* file = m_files.HandleToSlot(reinterpret_cast<std::uint32_t>(handle));
	if (!file)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, handle);
		return 0;
	}

	return static_cast<size_t>(file->impl->GetSize());
}

int CryPak::Ungetc(int ch, FILE* handle)
{
	std::lock_guard lock(m_mutex);

	FileSlot* file = m_files.HandleToSlot(reinterpret_cast<std::uint32_t>(handle));
	if (!file)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, handle);
		return EOF;
	}

	return file->impl->FUnGetC(ch);
}

bool CryPak::IsInPak(FILE* handle)
{
	std::lock_guard lock(m_mutex);

	FileSlot* file = m_files.HandleToSlot(reinterpret_cast<std::uint32_t>(handle));
	if (!file)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, handle);
		return false;
	}

	return file->pakHandle != 0;
}

bool CryPak::RemoveFile(const char* name)
{
	// TODO: AdjustFileName
	// TODO
	CryLogAlways("%s(\"%s\"): NOT IMPLEMENTED!", __FUNCTION__, name);
	return {};
}

bool CryPak::RemoveDir(const char* name, bool recurse)
{
	// TODO: AdjustFileName
	// TODO
	CryLogAlways("%s(\"%s\", %d): NOT IMPLEMENTED!", __FUNCTION__, name, static_cast<int>(recurse));
	return {};
}

bool CryPak::IsAbsPath(const char* path)
{
	return IsAbsolutePath(StringTools::SafeView(path));
}

size_t CryPak::FSeek(FILE* handle, long seek, int mode)
{
	std::lock_guard lock(m_mutex);

	FileSlot* file = m_files.HandleToSlot(reinterpret_cast<std::uint32_t>(handle));
	if (!file)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, handle);
		return static_cast<size_t>(-1);
	}

	return static_cast<size_t>(file->impl->FSeek(seek, mode));
}

long CryPak::FTell(FILE* handle)
{
	std::lock_guard lock(m_mutex);

	FileSlot* file = m_files.HandleToSlot(reinterpret_cast<std::uint32_t>(handle));
	if (!file)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, handle);
		return -1;
	}

	return static_cast<long>(file->impl->FTell());
}

int CryPak::FClose(FILE* handle)
{
	std::lock_guard lock(m_mutex);

	FileSlot* file = m_files.HandleToSlot(reinterpret_cast<std::uint32_t>(handle));
	if (!file)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, handle);
		return EOF;
	}

	PakSlot* pak = m_paks.HandleToSlot(file->pakHandle);

	// close the file
	file->clear();

	if (pak)
	{
		this->DecrementPakRefCount(pak);
	}

	CryLogAlways("%s(0x%p)", __FUNCTION__, handle);

	return 0;
}

int CryPak::FEof(FILE* handle)
{
	std::lock_guard lock(m_mutex);

	FileSlot* file = m_files.HandleToSlot(reinterpret_cast<std::uint32_t>(handle));
	if (!file)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, handle);
		return 0;
	}

	return file->impl->FEof();
}

int CryPak::FError(FILE* handle)
{
	std::lock_guard lock(m_mutex);

	FileSlot* file = m_files.HandleToSlot(reinterpret_cast<std::uint32_t>(handle));
	if (!file)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, handle);
		return 0;
	}

	return file->impl->FError();
}

int CryPak::FGetErrno()
{
	return -1;
}

int CryPak::FFlush(FILE* handle)
{
	std::lock_guard lock(m_mutex);

	FileSlot* file = m_files.HandleToSlot(reinterpret_cast<std::uint32_t>(handle));
	if (!file)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, handle);
		return 0;
	}

	return file->impl->FFlush();
}

void* CryPak::PoolMalloc(size_t size)
{
	return std::malloc(size);
}

void CryPak::PoolFree(void* pool)
{
	std::free(pool);
}

intptr_t CryPak::FindFirst(const char* dir, struct _finddata_t* fd, unsigned int flags)
{
	std::lock_guard lock(m_mutex);

	const std::string wildcardPath = this->AdjustFileNameImpl(StringTools::SafeView(dir), flags);
	const auto [dirPath, wildcardName] = SplitPathIntoDirAndFile(wildcardPath);
	const CryPakWildcardMatcher matcher(wildcardName);

	std::vector<DirectorySlot::Entry> entries;

	// TODO: maybe std::map instead?
	const auto isDuplicate = [&entries](std::string_view name) -> bool
	{
		const auto it = std::find_if(entries.begin(), entries.end(),
			[name](const DirectorySlot::Entry& entry) -> bool
			{
				return StringTools::IsEqualNoCase(entry.name, name);
			}
		);

		return it != entries.end();
	};

	TreeNode::Directory* dirNode = this->FindDirectoryInTree(dirPath);
	if (dirNode)
	{
		for (auto& [name, node] : *dirNode)
		{
			if (!matcher(name) || isDuplicate(name))
			{
				continue;
			}

			DirectorySlot::Entry& e = entries.emplace_back();
			e.name = name;
			e.isInPak = true;
			e.isDirectory = std::holds_alternative<TreeNode::Directory>(node.content);
			e.size = e.isDirectory ? 0 : this->GetFileSize(node);
		}
	}

	std::error_code ec;
	for (const auto& entry : std::filesystem::directory_iterator(dirPath, ec))
	{
		const std::string path = entry.path().generic_string();
		const std::string_view fileName = FileName(path);

		if (matcher(fileName) && !isDuplicate(fileName))
		{
			DirectorySlot::Entry& e = entries.emplace_back();
			e.name = fileName;
			e.isInPak = false;
			e.isDirectory = entry.is_directory(ec);
			e.size = e.isDirectory ? 0 : entry.file_size(ec);
		}
	}

	CryLogAlways("%s(\"%s\", 0x%x): Found %zu entries", __FUNCTION__, dir, flags, entries.size());

	if (entries.empty())
	{
		return -1;
	}

	DirectorySlot* slot = m_directories.GetFreeSlot();
	if (!slot)
	{
		CryLogAlways("%s(\"%s\", 0x%x): Max number of slots reached", __FUNCTION__, dir, flags);
		return -1;
	}

	slot->entries = std::move(entries);
	slot->pos = 0;

	this->FillFindData(fd, slot->entries[0]);
	slot->pos++;

	return static_cast<intptr_t>(m_directories.SlotToHandle(slot));
}

int CryPak::FindNext(intptr_t handle, struct _finddata_t* fd)
{
	std::lock_guard lock(m_mutex);

	DirectorySlot* slot = m_directories.HandleToSlot(static_cast<std::uint32_t>(handle));
	if (!slot)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, reinterpret_cast<void*>(handle));
		return -1;
	}

	if (slot->pos >= slot->entries.size())
	{
		return -1;
	}

	this->FillFindData(fd, slot->entries[slot->pos]);
	slot->pos++;

	return 0;
}

int CryPak::FindClose(intptr_t handle)
{
	std::lock_guard lock(m_mutex);

	DirectorySlot* slot = m_directories.HandleToSlot(static_cast<std::uint32_t>(handle));
	if (!slot)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, reinterpret_cast<void*>(handle));
		return -1;
	}

	slot->clear();

	return 0;
}

ICryPak::FileTime CryPak::GetModificationTime(FILE* handle)
{
	std::lock_guard lock(m_mutex);

	FileSlot* file = m_files.HandleToSlot(reinterpret_cast<std::uint32_t>(handle));
	if (!file)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, handle);
		return 0;
	}

	return file->impl->GetModificationTime();
}

bool CryPak::IsFileExist(const char* name)
{
	std::lock_guard lock(m_mutex);

	const std::string adjustedName = this->AdjustFileNameImpl(StringTools::SafeView(name), 0);

	TreeNode::FileInPak* fileInPak = this->FindFileInTree(adjustedName);
	if (fileInPak)
	{
		CryLogAlways("%s(\"%s\"): Found in a pak", __FUNCTION__, name);
		return true;
	}

	std::error_code ec;
	if (std::filesystem::exists(adjustedName, ec))
	{
		CryLogAlways("%s(\"%s\"): Found outside", __FUNCTION__, name);
		return true;
	}

	CryLogAlways("%s(\"%s\"): Not found", __FUNCTION__, name);

	return false;
}

bool CryPak::MakeDir(const char* path)
{
	const std::string adjustedPath = this->AdjustFileNameImpl(StringTools::SafeView(path), 0);

	std::error_code ec;
	const bool created = std::filesystem::create_directories(adjustedPath, ec);

	if (ec)
	{
		CryLogAlways("%s(\"%s\"): Error %d (%s)", __FUNCTION__, path, ec.value(), ec.message().c_str());
		return false;
	}
	else if (!created)
	{
		CryLogAlways("%s(\"%s\"): Already exists", __FUNCTION__, path);
		return true;
	}
	else
	{
		CryLogAlways("%s(\"%s\"): Success", __FUNCTION__, path);
		return true;
	}
}

ICryArchive* CryPak::OpenArchive(const char* path, unsigned int flags)
{
	// TODO
	CryLogAlways("%s(\"%s\", 0x%x): NOT IMPLEMENTED!", __FUNCTION__, path, flags);
	return {};
}

const char* CryPak::GetFileArchivePath(FILE* handle)
{
	std::lock_guard lock(m_mutex);

	FileSlot* file = m_files.HandleToSlot(reinterpret_cast<std::uint32_t>(handle));
	if (!file)
	{
		CryLogAlways("%s(0x%p): Invalid handle", __FUNCTION__, handle);
		return nullptr;
	}

	PakSlot* pak = m_paks.HandleToSlot(file->pakHandle);
	if (!pak)
	{
		return nullptr;
	}

	return pak->path.c_str();
}

int CryPak::RawCompress(const void* uncompressed, unsigned long* dstSize, void* compressed, unsigned long srcSize, int level)
{
	CryLogAlways("%s: NOT IMPLEMENTED!", __FUNCTION__);
	return -1;
}

int CryPak::RawUncompress(void* uncompressed, unsigned long* dstSize, const void* compressed, unsigned long srcSize)
{
	CryLogAlways("%s: NOT IMPLEMENTED!", __FUNCTION__);
	return -1;
}

void CryPak::RecordFileOpen(const ERecordFileOpenList list)
{
	// TODO
	CryLogAlways("%s: NOT IMPLEMENTED!", __FUNCTION__);
}

void CryPak::RecordFile(FILE* handle, const char* name)
{
	// TODO
	CryLogAlways("%s: NOT IMPLEMENTED!", __FUNCTION__);
}

IResourceList* CryPak::GetRecorderdResourceList(const ERecordFileOpenList list)
{
	switch (list)
	{
		case RFOM_Disabled:      break;
		case RFOM_EngineStartup: return m_resourceList_EngineStartup.get();
		case RFOM_NextLevel:     return m_resourceList_NextLevel.get();
		case RFOM_Level:         return m_resourceList_Level.get();
	}

	return nullptr;
}

ICryPak::ERecordFileOpenList CryPak::GetRecordFileOpenList()
{
	// TODO
	CryLogAlways("%s: NOT IMPLEMENTED!", __FUNCTION__);
	return {};
}

void CryPak::Notify(ENotifyEvent event)
{
}

std::uint32_t CryPak::ComputeCRC(const char* path)
{
	CryLogAlways("%s(\"%s\"): NOT IMPLEMENTED!", __FUNCTION__, path);
	return 0;
}

bool CryPak::ComputeMD5(const char* path, unsigned char* md5)
{
	CryLogAlways("%s(\"%s\"): NOT IMPLEMENTED!", __FUNCTION__, path);
	return false;
}

void CryPak::RegisterFileAccessSink(ICryPakFileAcesssSink* pSink)
{
	// TODO
	CryLogAlways("%s: NOT IMPLEMENTED!", __FUNCTION__);
}

void CryPak::UnregisterFileAccessSink(ICryPakFileAcesssSink* pSink)
{
	// TODO
	CryLogAlways("%s: NOT IMPLEMENTED!", __FUNCTION__);
}

bool CryPak::GetLvlResStatus() const
{
	// TODO: check disassembly how this is set
	return m_lvlRes;
}

const char* CryPak::GetModDir() const
{
	return "";
}

////////////////////////////////////////////////////////////////////////////////

std::string CryPak::AdjustFileNameImpl(std::string_view path, unsigned int flags)
{
	std::string result;
	result.reserve(260);

	if ((flags & FLAGS_FOR_WRITING) && !m_gameFolderWritable)
	{
		if (path.empty() || (path[0] != '%' && !IsAbsolutePath(path)))
		{
			result += "%USER%/";
		}
	}

	result += path;

	this->ExpandAliases(result);

	std::replace(result.begin(), result.end(), '\\', '/');
	// TODO: remove duplicate slashes

	if (flags & FLAGS_NO_FULL_PATH)
	{
		return result;
	}

	if (!IsAbsolutePath(result))
	{
		const bool hasDotSlash = result.starts_with("./");

		if (!hasDotSlash
		 && !StringTools::StartsWithNoCase(result, "Game/")
		 && !StringTools::StartsWithNoCase(result, "Editor/")
		 && !StringTools::StartsWithNoCase(result, "Bin32/")
		 && !StringTools::StartsWithNoCase(result, "Bin64/")
		 && !StringTools::StartsWithNoCase(result, "Mods/"))
		{
			result.insert(0, "Game/");
		}
		else if (hasDotSlash)
		{
			result.erase(0, 2);
		}
	}

	// TODO: normalize the path

	if (flags & FLAGS_ADD_TRAILING_SLASH)
	{
		AddTrailingSlash(result);
	}

	return result;
}

bool CryPak::OpenPackImpl(std::string_view pakPath, std::string_view bindingRoot, unsigned int flags)
{
	// TODO: flags

	std::unique_ptr<IPak> pak = m_loader.LoadPak(pakPath);
	if (!pak)
	{
		CryLogAlways("%s(\"%s\", \"%s\", 0x%x): Failed", __FUNCTION__, pakPath.data(), bindingRoot.data(), flags);
		return false;
	}

	PakSlot* slot = m_paks.GetFreeSlot();
	if (!slot)
	{
		CryLogAlways("%s(\"%s\", \"%s\", 0x%x): Max number of slots reached", __FUNCTION__, pakPath.data(), bindingRoot.data(), flags);
		return false;
	}

	slot->path = pakPath;
	slot->bindingRoot = bindingRoot;
	slot->impl = std::move(pak);
	slot->refCount = 1;

	this->AddPakToTree(slot);

	CryLogAlways("%s(\"%s\", \"%s\", 0x%x): Success", __FUNCTION__, pakPath.data(), bindingRoot.data(), flags);

	return true;
}

bool CryPak::OpenPacksImpl(std::string_view wildcardPath, std::string_view bindingRoot, unsigned int flags)
{
	CryLogAlways("%s(\"%s\", \"%s\", 0x%x)", __FUNCTION__, wildcardPath.data(), bindingRoot.data(), flags);

	bool success = false;

	for (const std::string& path : ExpandWildcardFilesystemPath(wildcardPath))
	{
		success |= this->OpenPackImpl(path, bindingRoot, flags);
	}

	return success;
}

bool CryPak::ClosePackImpl(std::string_view pakPath, unsigned int flags)
{
	// TODO: flags

	PakSlot* pak = this->FindLoadedPakByPath(pakPath);
	if (!pak)
	{
		CryLogAlways("%s(\"%s\", 0x%x): Not found", __FUNCTION__, pakPath.data(), flags);
		return false;
	}

	this->DecrementPakRefCount(pak);

	CryLogAlways("%s(\"%s\", 0x%x): Success", __FUNCTION__, pakPath.data(), flags);

	return true;
}

bool CryPak::ClosePacksImpl(std::string_view wildcardPath, unsigned int flags)
{
	CryLogAlways("%s(\"%s\", 0x%x)", __FUNCTION__, wildcardPath.data(), flags);

	bool success = false;

	for (const std::string& path : ExpandWildcardFilesystemPath(wildcardPath))
	{
		success |= this->ClosePackImpl(path, flags);
	}

	return success;
}

CryPak::PakSlot* CryPak::FindLoadedPakByPath(std::string_view pakPath)
{
	return m_paks.Find([pakPath](const PakSlot& pak) { return StringTools::IsEqualNoCase(pak.path, pakPath); });
}

void CryPak::IncrementPakRefCount(PakSlot* slot)
{
	slot->refCount++;
}

void CryPak::DecrementPakRefCount(PakSlot* slot)
{
	slot->refCount--;

	if (slot->refCount <= 0)
	{
		this->RemovePakFromTree(slot);

		slot->clear();
	}
}

void CryPak::ExpandAliases(std::string& path)
{
	std::string::size_type pos = 0;
	while ((pos = path.find('%', pos)) != std::string::npos)
	{
		auto endPos = path.find('%', pos + 1);
		if (endPos == std::string::npos)
		{
			break;
		}

		++endPos;

		const std::string_view aliasName(path.c_str() + pos, endPos - pos);
		const auto aliasIt = m_aliases.find(aliasName);
		if (aliasIt == m_aliases.end())
		{
			// ignore unknown aliases
			pos = endPos;
		}
		else
		{
			path.replace(path.begin() + pos, path.begin() + endPos, aliasIt->second);

			pos += aliasIt->second.length();
		}
	}
}

void CryPak::FillFindData(struct _finddata_t* fd, const DirectorySlot::Entry& entry)
{
	CryLogAlways("%s: \"%s\" size=%llu directory=%d in_pak=%d", __FUNCTION__, entry.name.c_str(), entry.size,
		static_cast<int>(entry.isDirectory), static_cast<int>(entry.isInPak));

	if (!fd)
	{
		return;
	}

	*fd = {};

	fd->attrib = _A_NORMAL;
	fd->size = entry.size;
	// TODO: fd->time_create
	// TODO: fd->time_access
	// TODO: fd->time_write

	if (entry.isDirectory)
	{
		fd->attrib |= _A_SUBDIR;
	}

	if (entry.isInPak)
	{
		fd->attrib |= _A_IN_CRYPAK | _A_RDONLY;
	}

	const auto length = std::min(entry.name.length(), std::size(fd->name) - 1);

	std::memcpy(fd->name, entry.name.c_str(), length);
	fd->name[length] = '\0';
}

CryPak::TreeNode* CryPak::AddToTree(std::string_view path, TreeNode* node)
{
	if (!node)
	{
		node = &m_root;
	}

	while (!path.empty())
	{
		const auto [pathComponent, remainingPath] = SplitFirstPathComponent(path);

		TreeNode::Directory* dirNode = std::get_if<TreeNode::Directory>(&node->content);
		if (!dirNode)
		{
			return nullptr;
		}

		// a directory node is added
#ifdef __cpp_lib_associative_heterogeneous_insertion
		const auto [it, added] = dirNode->try_emplace(pathComponent);
#else
		const auto [it, added] = dirNode->try_emplace(std::string(pathComponent));
#endif

		node = &it->second;
		path = remainingPath;
	}

	return node;
}

CryPak::TreeNode* CryPak::FindInTree(std::string_view path, TreeNode* node)
{
	if (!node)
	{
		node = &m_root;
	}

	while (!path.empty())
	{
		const auto [pathComponent, remainingPath] = SplitFirstPathComponent(path);

		TreeNode::Directory* dirNode = std::get_if<TreeNode::Directory>(&node->content);
		if (!dirNode)
		{
			return nullptr;
		}

		const auto it = dirNode->find(pathComponent);
		if (it == dirNode->end())
		{
			return nullptr;
		}

		node = &it->second;
		path = remainingPath;
	}

	return node;
}

CryPak::TreeNode::FileInPak* CryPak::FindFileInTree(std::string_view path)
{
	TreeNode* node = this->FindInTree(path);
	if (!node)
	{
		return nullptr;
	}

	TreeNode::File* fileNode = std::get_if<TreeNode::File>(&node->content);
	if (!fileNode)
	{
		return nullptr;
	}

	if (fileNode->empty())
	{
		return nullptr;
	}

	return &fileNode->back();
}

CryPak::TreeNode::Directory* CryPak::FindDirectoryInTree(std::string_view path)
{
	TreeNode* node = this->FindInTree(path);
	if (!node)
	{
		return nullptr;
	}

	TreeNode::Directory* dirNode = std::get_if<TreeNode::Directory>(&node->content);
	if (!dirNode)
	{
		return nullptr;
	}

	return dirNode;
}

std::uint64_t CryPak::GetFileSize(TreeNode& node)
{
	TreeNode::File* fileNode = std::get_if<TreeNode::File>(&node.content);
	if (!fileNode)
	{
		return 0;
	}

	if (fileNode->empty())
	{
		return 0;
	}

	TreeNode::FileInPak& file = fileNode->back();

	PakSlot* pak = m_paks.HandleToSlot(file.pakHandle);
	if (!pak)
	{
		return 0;
	}

	std::uint64_t size = 0;
	if (!pak->impl->GetEntrySize(file.fileIndex, size))
	{
		return 0;
	}

	return size;
}

void CryPak::AddPakToTree(PakSlot* slot)
{
	const std::uint32_t pakHandle = m_paks.SlotToHandle(slot);

	TreeNode* baseNode = this->AddToTree(slot->bindingRoot);
	if (!baseNode)
	{
		// binding root path points to a file
		return;
	}

	const std::uint32_t entryCount = slot->impl->GetEntryCount();

	std::string path;
	for (std::uint32_t i = 0; i < entryCount; i++)
	{
		if (slot->impl->IsDirectoryEntry(i))
		{
			continue;
		}

		if (!slot->impl->GetEntryPath(i, path))
		{
			continue;
		}

		// TODO: normalize the path

		const auto [dirPath, fileName] = SplitPathIntoDirAndFile(path);

		TreeNode* node = this->AddToTree(dirPath, baseNode);
		if (!node)
		{
			continue;
		}

		TreeNode::Directory* dirNode = std::get_if<TreeNode::Directory>(&node->content);
		if (!dirNode)
		{
			continue;
		}

		// a directory node is added
#ifdef __cpp_lib_associative_heterogeneous_insertion
		const auto [it, added] = dirNode->try_emplace(fileName);
#else
		const auto [it, added] = dirNode->try_emplace(std::string(fileName));
#endif

		if (added)
		{
			it->second.content.emplace<TreeNode::File>();
		}

		TreeNode::File* fileNode = std::get_if<TreeNode::File>(&it->second.content);
		if (!fileNode)
		{
			continue;
		}

		fileNode->emplace_back(TreeNode::FileInPak{.pakHandle = pakHandle, .fileIndex = i});
	}
}

void CryPak::RemovePakFromTree(PakSlot* slot)
{
	const std::uint32_t pakHandle = m_paks.SlotToHandle(slot);

	struct Branch
	{
		TreeNode* node = nullptr;
		bool processed = false;
	};

	std::stack<Branch> stack;
	stack.emplace(&m_root);

	const auto isDirEntryEmpty = [](const TreeNode::Directory::value_type& entry) -> bool
	{
		const auto& [name, node] = entry;

		return std::visit([](const auto& x) { return x.empty(); }, node.content);
	};

	const auto isToRemove = [pakHandle](const TreeNode::FileInPak& file) -> bool
	{
		return file.pakHandle == pakHandle;
	};

	while (!stack.empty())
	{
		Branch& branch = stack.top();

		TreeNode::Directory* dirNode = std::get_if<TreeNode::Directory>(&branch.node->content);
		if (!dirNode)
		{
			stack.pop();
			continue;
		}

		if (branch.processed)
		{
			// remove empty nodes
			std::erase_if(*dirNode, isDirEntryEmpty);

			stack.pop();
			continue;
		}

		for (auto& [name, node] : *dirNode)
		{
			TreeNode::File* fileNode = std::get_if<TreeNode::File>(&node.content);
			if (fileNode)
			{
				std::erase_if(*fileNode, isToRemove);
			}
			else
			{
				stack.emplace(&node);
			}
		}

		branch.processed = true;
	}
}
