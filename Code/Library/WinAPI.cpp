#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>

#include <ctype.h>
#include <sstream>
#include <fstream>
#include <algorithm>

#pragma comment(lib, "Shlwapi")

#include "External/picosha2.h"
#include "External/nlohmann/json.hpp"

#include "WinAPI.h"
#include "Error.h"
#include "Format.h"
#include "DLL.h"

namespace
{
	using WinAPI::RVA;

	std::string_view GetResource(void *pDLL, const char *name, const char *type)
	{
		HRSRC resourceInfo = FindResourceA(static_cast<HMODULE>(pDLL), name, type);
		if (!resourceInfo)
			return std::string_view();

		HGLOBAL resourceData = LoadResource(static_cast<HMODULE>(pDLL), resourceInfo);
		if (!resourceData)
			return std::string_view();

		const void *data = LockResource(resourceData);
		size_t length = SizeofResource(static_cast<HMODULE>(pDLL), resourceInfo);

		return std::string_view(static_cast<const char*>(data), length);
	}

	IMAGE_DATA_DIRECTORY *GetDirectoryData(void *pDLL, unsigned int directory)
	{
		if (!pDLL)
			return nullptr;

		IMAGE_DOS_HEADER *pDOSHeader = static_cast<IMAGE_DOS_HEADER*>(pDLL);
		if (pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE)
			return nullptr;

		IMAGE_NT_HEADERS *pPEHeader = static_cast<IMAGE_NT_HEADERS*>(RVA(pDLL, pDOSHeader->e_lfanew));
		if (pPEHeader->Signature != IMAGE_NT_SIGNATURE)
			return nullptr;

	#ifdef BUILD_64BIT
		const int ntOptionalHeaderMagic = IMAGE_NT_OPTIONAL_HDR64_MAGIC;
	#else
		const int ntOptionalHeaderMagic = IMAGE_NT_OPTIONAL_HDR32_MAGIC;
	#endif

		if (pPEHeader->OptionalHeader.Magic != ntOptionalHeaderMagic)
			return nullptr;

		if (pPEHeader->OptionalHeader.NumberOfRvaAndSizes <= directory)
			return nullptr;

		IMAGE_DATA_DIRECTORY *pDirectoryData = &pPEHeader->OptionalHeader.DataDirectory[directory];
		if (pDirectoryData->VirtualAddress == 0 || pDirectoryData->Size == 0)
			return nullptr;

		return pDirectoryData;
	}

	DWORD ToNativeSeek(WinAPI::SeekBase base)
	{
		switch (base)
		{
			case WinAPI::SeekBase::BEGIN:   return FILE_BEGIN;
			case WinAPI::SeekBase::CURRENT: return FILE_CURRENT;
			case WinAPI::SeekBase::END:     return FILE_END;
		}

		return 0;
	}

	WinAPI::DateTime FromNativeDateTime(const SYSTEMTIME & dateTime)
	{
		WinAPI::DateTime result;
		result.year        = dateTime.wYear;
		result.month       = dateTime.wMonth;
		result.dayOfWeek   = dateTime.wDayOfWeek;
		result.day         = dateTime.wDay;
		result.hour        = dateTime.wHour;
		result.minute      = dateTime.wMinute;
		result.second      = dateTime.wSecond;
		result.millisecond = dateTime.wMilliseconds;

		return result;
	}

	std::string GetMachineGUID()
	{
		std::string guid;

		HKEY key;

		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", 0,
		                  KEY_QUERY_VALUE | KEY_WOW64_64KEY, &key) == ERROR_SUCCESS)
		{
			char buffer[256];
			DWORD length = static_cast<DWORD>(sizeof buffer);

			if (RegQueryValueExA(key, "MachineGuid", nullptr, nullptr,
			                     reinterpret_cast<LPBYTE>(buffer), &length) == ERROR_SUCCESS && length > 0)
			{
				guid.assign(buffer, length - 1);  // skip terminating null character
			}
		}

		return guid;
	}
}

const char *WinAPI::GetCmdLine()
{
	return GetCommandLineA();
}

int WinAPI::GetCurrentErrorCode()
{
	return GetLastError();
}

std::string WinAPI::GetErrorCodeDescription(int code)
{
	const DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

	char buffer[256];
	DWORD length = FormatMessageA(flags, nullptr, code, 0, buffer, sizeof buffer, nullptr);

	return std::string(buffer, length);
}

std::filesystem::path WinAPI::GetApplicationPath()
{
	constexpr DWORD BUFFER_SIZE = 1024;

	wchar_t buffer[BUFFER_SIZE];
	DWORD length = GetModuleFileNameW(nullptr, buffer, BUFFER_SIZE);
	if (length == 0)
	{
		throw SystemError("GetModuleFileNameW");
	}
	else if (length >= BUFFER_SIZE)
	{
		throw Error("Application path is too long!");
	}

	return std::filesystem::path(buffer);
}

void WinAPI::SetWorkingDirectory(const std::filesystem::path & path)
{
	if (!SetCurrentDirectoryW(path.c_str()))
	{
		throw SystemError("SetCurrentDirectoryW");
	}
}

void WinAPI::DLL_AddSearchDirectory(const std::filesystem::path & path)
{
	if (!SetDllDirectoryW(path.c_str()))
	{
		throw SystemError("SetDllDirectoryW");
	}
}

void *WinAPI::DLL_Get(const char *name)
{
	return GetModuleHandleA(name);
}

void *WinAPI::DLL_Load(const char *name)
{
	return LoadLibraryA(name);
}

void *WinAPI::DLL_GetSymbol(void *pDLL, const char *name)
{
	return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(pDLL), name));
}

void WinAPI::DLL_Unload(void *pDLL)
{
	FreeLibrary(static_cast<HMODULE>(pDLL));
}

void WinAPI::ErrorBox(const char *message)
{
	MessageBoxA(nullptr, message, "Error", MB_OK | MB_ICONERROR);
}

void WinAPI::ErrorBox(const Error & error)
{
	const int code = error.getCode();
	const char *message = error.getMessage().c_str();
	const char *description = error.getDescription().c_str();

	if (code > 0 || *description)
		ErrorBox(Format("%s\nError %d: %s", message, code, description).c_str());
	else
		ErrorBox(message);
}

std::string_view WinAPI::GetDataResource(void *pDLL, int resourceID)
{
	return GetResource(pDLL, MAKEINTRESOURCE(resourceID), RT_RCDATA);
}

/**
 * @brief Obtains game version from any Crysis DLL.
 * It parses version resource of the specified file.
 * @param pDLL Handle of any Crysis DLL.
 * @return Game build number or -1 if some error occurred.
 */
int WinAPI::GetCrysisGameBuild(void *pDLL)
{
	const void *versionRes = GetResource(pDLL, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION).data();
	if (!versionRes)
		return -1;

	if (memcmp(RVA(versionRes, 0x6), L"VS_VERSION_INFO", 0x20) != 0)
		return -1;

	const VS_FIXEDFILEINFO *pFileInfo = static_cast<const VS_FIXEDFILEINFO*>(RVA(versionRes, 0x6 + 0x20 + 0x2));
	if (pFileInfo->dwSignature != 0xFEEF04BD)
		return -1;

	return LOWORD(pFileInfo->dwFileVersionLS);
}

/**
 * @brief Fills read-only memory region with x86 NOP instruction.
 * @param address Address of the memory region.
 * @param length Size of the memory region in bytes.
 */
void WinAPI::FillNOP(void *address, size_t length)
{
	DWORD oldProtection;

	if (!VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &oldProtection))
	{
		throw SystemError("VirtualProtect");
	}

	// 0x90 is opcode of NOP instruction on both x86 and x86-64
	memset(address, '\x90', length);

	if (!VirtualProtect(address, length, oldProtection, &oldProtection))
	{
		throw SystemError("VirtualProtect");
	}
}

/**
 * @brief Fills read-only memory region with custom data.
 * The memory region and the data must not overlap.
 * @param address Address of the memory region.
 * @param data The data.
 * @param length Size of the data in bytes.
 */
void WinAPI::FillMem(void *address, const void *data, size_t length)
{
	DWORD oldProtection;

	if (!VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &oldProtection))
	{
		throw SystemError("VirtualProtect");
	}

	memcpy(address, data, length);

	if (!VirtualProtect(address, length, oldProtection, &oldProtection))
	{
		throw SystemError("VirtualProtect");
	}
}

bool WinAPI::HookIATByAddress(void *pDLL, void *pFunc, void *pNewFunc)
{
	const IMAGE_DATA_DIRECTORY *pIATData = GetDirectoryData(pDLL, IMAGE_DIRECTORY_ENTRY_IAT);
	if (!pIATData)
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return false;
	}

	// import address table
	void **pIAT = static_cast<void**>(RVA(pDLL, pIATData->VirtualAddress));

	// number of functions in the table
	const size_t entryCount = pIATData->Size / sizeof (void*);

	bool found = false;

	for (size_t i = 0; i < entryCount; i++)
	{
		if (pIAT[i] == pFunc)
		{
			found = true;

			// hook the function
			FillMem(&pIAT[i], &pNewFunc, sizeof (void*));
		}
	}

	if (!found)
	{
		SetLastError(ERROR_PROC_NOT_FOUND);
		return false;
	}

	return true;
}

unsigned long WinAPI::GetCurrentThreadID()
{
	return GetCurrentThreadId();
}

void *WinAPI::OpenLogFile(const std::filesystem::path & path, bool & exists)
{
	const DWORD mode = GENERIC_READ | GENERIC_WRITE;
	const DWORD share = FILE_SHARE_READ;
	const DWORD creation = OPEN_ALWAYS;
	const DWORD attributes = FILE_ATTRIBUTE_NORMAL;

	HANDLE file = CreateFileW(path.c_str(), mode, share, nullptr, creation, attributes, nullptr);
	if (file == INVALID_HANDLE_VALUE)
	{
		return nullptr;
	}
	else if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		exists = true;
	}

	return file;
}

uint64_t WinAPI::Seek(void *handle, WinAPI::SeekBase base, int64_t offset)
{
	LARGE_INTEGER offsetValue;
	offsetValue.QuadPart = offset;

	if (!SetFilePointerEx(static_cast<HANDLE>(handle), offsetValue, &offsetValue, ToNativeSeek(base)))
	{
		throw SystemError("SetFilePointerEx");
	}

	return offsetValue.QuadPart;
}

std::string WinAPI::Read(void *handle, size_t maxLength)
{
	std::string result;
	result.resize(maxLength);

	DWORD bytesRead = 0;

	if (!ReadFile(static_cast<HANDLE>(handle), result.data(), result.length(), &bytesRead, nullptr))
	{
		throw SystemError("ReadFile");
	}

	if (bytesRead != result.length())
	{
		result.resize(bytesRead);
	}

	return result;
}

size_t WinAPI::Write(void *handle, const std::string_view & text)
{
	DWORD bytesWritten = 0;

	if (!WriteFile(static_cast<HANDLE>(handle), text.data(), text.length(), &bytesWritten, nullptr))
	{
		throw SystemError("WriteFile");
	}

	return bytesWritten;
}

void WinAPI::Resize(void *handle, uint64_t size)
{
	Seek(handle, SeekBase::BEGIN, size);  // the offset is interpreted as an unsigned value

	if (!SetEndOfFile(static_cast<HANDLE>(handle)))
	{
		throw SystemError("SetEndOfFile");
	}
}

void WinAPI::Close(void *handle)
{
	CloseHandle(static_cast<HANDLE>(handle));
}

WinAPI::DateTime WinAPI::GetCurrentDateTimeUTC()
{
	SYSTEMTIME dateTime;
	GetSystemTime(&dateTime);

	return FromNativeDateTime(dateTime);
}

WinAPI::DateTime WinAPI::GetCurrentDateTimeLocal()
{
	SYSTEMTIME dateTime;
	GetLocalTime(&dateTime);

	return FromNativeDateTime(dateTime);
}

std::wstring WinAPI::ConvertUTF8To16(const std::string_view & text)
{
	std::wstring result;

	int count = MultiByteToWideChar(CP_UTF8, 0, text.data(), text.length(), nullptr, 0);
	if (count > 0)
	{
		result.resize(count);

		MultiByteToWideChar(CP_UTF8, 0, text.data(), text.length(), result.data(), count);
	}

	return result;
}

std::string WinAPI::ConvertUTF16To8(const std::wstring_view & text)
{
	std::string result;

	int count = WideCharToMultiByte(CP_UTF8, 0, text.data(), text.length(), nullptr, 0, nullptr, nullptr);
	if (count > 0)
	{
		result.resize(count);

		WideCharToMultiByte(CP_UTF8, 0, text.data(), text.length(), result.data(), count, nullptr, nullptr);
	}

	return result;
}

std::string WinAPI::sha256(const std::string_view & text)
{
	// lowercase hex digits
	return picosha2::hash256_hex_string(text);
}

std::string WinAPI::SHA256(const std::string_view & text)
{
	std::string result = sha256(text);

	// uppercase hex digits
	std::transform(result.begin(), result.end(), result.begin(), toupper);

	return result;
}

std::string WinAPI::GetHWID(const std::string & salt)
{
	std::string hwid = SHA256(GetMachineGUID());

	if (hwid.empty())
		return hwid;
	else
		return hwid + ":" + SHA256(hwid + salt);
}

std::string WinAPI::GetLocale()
{
	char buffer[LOCALE_NAME_MAX_LENGTH] = {};
	GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SNAME, buffer, sizeof(buffer));

	return buffer;
}

long WinAPI::GetTimeZoneBias()
{
	TIME_ZONE_INFORMATION tz;
	switch (GetTimeZoneInformation(&tz))
	{
		case TIME_ZONE_ID_UNKNOWN:
		{
			return tz.Bias;
		}
		case TIME_ZONE_ID_STANDARD:
		{
			return tz.Bias + tz.StandardBias;
		}
		case TIME_ZONE_ID_DAYLIGHT:
		{
			return tz.Bias + tz.DaylightBias;
		}
		case TIME_ZONE_ID_INVALID:
		{
			throw SystemError("GetTimeZoneInformation");
		}
	}

	return 0;
}

std::string WinAPI::GetCachePath(const std::string & path)
{
	char szPath[MAX_PATH];
	if (!SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath))) return "";
	std::string fullPath = szPath;
	fullPath += "\\sfwcl\\";
	if (!PathFileExists(fullPath.c_str())) {
		if (!CreateDirectory(fullPath.c_str(), 0)) return "";
	}
	std::string hash = SHA256(path);
	std::ifstream index(fullPath + "index");
	nlohmann::json indexDic;
	if (index) {
		std::stringstream contents;
		contents << index.rdbuf();
		index.close();
		try {
			indexDic = nlohmann::json::parse(contents.str());
		} catch (std::exception & ex) {
			indexDic["error_" + std::string(hash)] = ex.what();
			// ... well, don't really care
		}
	}
	indexDic[hash] = path;
	std::ofstream indexOut(fullPath + "index");
	if (indexOut) {
		indexOut << indexDic.dump();
		indexOut.close();
	}
	return fullPath + hash;
}
