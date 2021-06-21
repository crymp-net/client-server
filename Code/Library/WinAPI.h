#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <string_view>
#include <filesystem>
#include <map>

class Error;

namespace WinAPI
{
	inline constexpr std::string_view NEWLINE = "\r\n";

	inline void *RVA(void *base, size_t offset)
	{
		return static_cast<uint8_t*>(base) + offset;
	}

	inline const void *RVA(const void *base, size_t offset)
	{
		return static_cast<const uint8_t*>(base) + offset;
	}

	const char *GetCmdLine();

	int GetCurrentErrorCode();
	std::string GetErrorCodeDescription(int code);

	std::filesystem::path GetApplicationPath();

	void SetWorkingDirectory(const std::filesystem::path & path);

	void DLL_AddSearchDirectory(const std::filesystem::path & path);

	void *DLL_Get(const char *name);
	void *DLL_Load(const char *name);
	void *DLL_GetSymbol(void *pDLL, const char *name);
	void DLL_Unload(void *pDLL);

	void ErrorBox(const char *message);
	void ErrorBox(const Error & error);

	std::string_view GetDataResource(void *pDLL, int resourceID);

	int GetCrysisGameBuild(void *pDLL);

	void FillNOP(void *address, size_t length);
	void FillMem(void *address, const void *data, size_t length);

	bool HookIATByAddress(void *pDLL, void *pFunc, void *pNewFunc);

	unsigned long GetCurrentThreadID();

	void *OpenLogFile(const std::filesystem::path & path, bool & exists);

	enum class SeekBase
	{
		BEGIN, CURRENT, END
	};

	uint64_t Seek(void *handle, SeekBase base, int64_t offset);

	std::string Read(void *handle, size_t maxLength);
	size_t Write(void *handle, const std::string_view & text);

	void Resize(void *handle, uint64_t size);

	void Close(void *handle);

	struct DateTime
	{
		unsigned short year;
		unsigned short month;
		unsigned short dayOfWeek;  // Sunday = 0, Monday = 1, ..., Saturday = 6
		unsigned short day;
		unsigned short hour;
		unsigned short minute;
		unsigned short second;
		unsigned short millisecond;
	};

	DateTime GetCurrentDateTimeUTC();
	DateTime GetCurrentDateTimeLocal();

	std::wstring ConvertUTF8To16(const std::string_view & text);
	std::string ConvertUTF16To8(const std::wstring_view & text);

	std::string sha256(const std::string_view & text);  // lowercase hex digits
	std::string SHA256(const std::string_view & text);  // uppercase hex digits

	std::string GetHWID(const std::string & salt);
	std::string GetLocale();

	long GetTimeZoneBias();

	struct HTTPResponse
	{
		int code = 0;
		std::string content;
	};

	// blocking, throws SystemError
	HTTPResponse HTTPRequest(
		const std::string_view & method,
		const std::string_view & url,
		const std::string_view & data,
		const std::map<std::string, std::string> & headers,
		int timeout
	);
}
