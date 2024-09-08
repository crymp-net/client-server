#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <string_view>
#include <utility>
#include <filesystem>
#include <functional>
#include <map>

namespace WinAPI
{
	inline constexpr std::string_view NEWLINE = "\r\n";

	/////////////
	// Utility //
	/////////////

	inline void *RVA(void *base, size_t offset)
	{
		return static_cast<uint8_t*>(base) + offset;
	}

	inline const void *RVA(const void *base, size_t offset)
	{
		return static_cast<const uint8_t*>(base) + offset;
	}

	//////////////////
	// Command line //
	//////////////////

	namespace CmdLine
	{
		const char* GetFull();
		const char* GetOnlyArgs();

		bool HasArg(const char* arg);

		const char* GetArgValue(const char* arg, const char* defaultValue = "");
	}

	////////////
	// Errors //
	////////////

	unsigned long GetCurrentErrorCode();

	///////////
	// Paths //
	///////////

	std::filesystem::path GetApplicationPath();

	void SetWorkingDirectory(const std::filesystem::path& path);

	/////////////
	// Modules //
	/////////////

	namespace DLL
	{
		void AddSearchDirectory(const std::filesystem::path& path);

		void* Get(const char* name);
		void* Load(const char* name);
		void* GetSymbol(void* pDLL, const char* name);
		void Unload(void* pDLL);
	}

	/////////////////
	// Message box //
	/////////////////

	void ErrorBox(const char *message);

	///////////////
	// Resources //
	///////////////

	std::string_view GetDataResource(void *pDLL, int resourceID);

	int GetCrysisGameBuild(void *pDLL);

	///////////
	// Hacks //
	///////////

	void FillNOP(void *address, size_t length);
	void FillMem(void *address, const void *data, size_t length);

	void HookWithJump(void* address, void* pNewFunc);

	bool HookIATByAddress(void *pDLL, void *pFunc, void *pNewFunc);
	bool HookIATByName(void *pDLL, const char *dllName, const char *funcName, void *pNewFunc);

	///////////
	// Files //
	///////////

	enum class FileAccess
	{
		READ_ONLY,
		WRITE_ONLY,
		WRITE_ONLY_CREATE,
		READ_WRITE,
		READ_WRITE_CREATE
	};

	enum class FileSeekBase
	{
		BEGIN, CURRENT, END
	};

	void *FileOpen(const std::filesystem::path & path, FileAccess access, bool *pCreated = nullptr);

	std::string FileRead(void *handle, size_t maxLength = 0);
	void FileWrite(void *handle, const std::string_view & text);

	uint64_t FileSeek(void *handle, FileSeekBase base, int64_t offset = 0);
	void FileResize(void *handle, uint64_t size);

	void FileClose(void *handle);

	class File
	{
		void *m_handle = nullptr;

	public:
		File() = default;

		explicit File(const std::filesystem::path & path, FileAccess access, bool *pCreated = nullptr)
		{
			m_handle = FileOpen(path, access, pCreated);
		}

		File(const File &) = delete;

		File(File && other)
		{
			std::swap(m_handle, other.m_handle);
		}

		File & operator=(const File &) = delete;

		File & operator=(File && other)
		{
			if (this != &other)
			{
				Close();

				std::swap(m_handle, other.m_handle);
			}

			return *this;
		}

		~File()
		{
			Close();
		}

		bool Open(const std::filesystem::path & path, FileAccess access, bool *pCreated = nullptr)
		{
			Close();

			m_handle = FileOpen(path, access, pCreated);

			return IsOpen();
		}

		bool IsOpen() const
		{
			return m_handle != nullptr;
		}

		explicit operator bool() const
		{
			return IsOpen();
		}

		std::string Read(size_t maxLength = 0)
		{
			return FileRead(m_handle, maxLength);
		}

		void Write(const std::string_view & text)
		{
			FileWrite(m_handle, text);
		}

		uint64_t Seek(FileSeekBase base, int64_t offset = 0)
		{
			return FileSeek(m_handle, base, offset);
		}

		void Resize(uint64_t size)
		{
			FileResize(m_handle, size);
		}

		void Close()
		{
			if (m_handle)
			{
				FileClose(m_handle);
				m_handle = nullptr;
			}
		}
	};

	//////////
	// Time //
	//////////

	struct DateTime
	{
		unsigned short year        = 0;
		unsigned short month       = 0;  // January = 1, February = 2, ..., December = 12
		unsigned short dayOfWeek   = 0;  // Sunday = 0, Monday = 1, ..., Saturday = 6
		unsigned short day         = 0;
		unsigned short hour        = 0;
		unsigned short minute      = 0;
		unsigned short second      = 0;
		unsigned short millisecond = 0;

		const char *GetDayName();
		const char *GetMonthName();
	};

	DateTime GetCurrentDateTimeUTC();
	DateTime GetCurrentDateTimeLocal();

	long GetTimeZoneBias();

	/////////////
	// Strings //
	/////////////

	std::size_t RawANSIToWide(const char* string, std::size_t stringSize, wchar_t* buffer, std::size_t bufferSize);

	wchar_t WideCharToLower(wchar_t ch, int languageID = 0);
	wchar_t WideCharToUpper(wchar_t ch, int languageID = 0);

	/////////////////
	// System info //
	/////////////////

	std::string GetMachineGUID();
	std::string GetLocale();

	bool IsVistaOrLater();

	unsigned int GetLogicalProcessorCount();

	std::size_t GetSystemLanguageCode(char* buffer, std::size_t bufferSize);

	//////////
	// HTTP //
	//////////

	using HTTPRequestReader = std::function<size_t(void*,size_t)>;  // buffer, buffer size, returns data length
	using HTTPRequestCallback = std::function<void(uint64_t,const HTTPRequestReader&)>;  // content length, reader

	// blocking, returns HTTP status code, throws std::system_error
	int HTTPRequest(
		const std::string_view & method,
		const std::string_view & url,
		const std::string_view & data,
		const std::map<std::string, std::string> & headers,
		HTTPRequestCallback callback
	);

	///////////////
	// Clipboard //
	///////////////

	std::string GetClipboardText(std::size_t maxLength = std::size_t(-1));

	////////////
	// Cursor //
	////////////

	namespace Cursor
	{
		void GetPos(long& x, long& y);
		void SetPos(long x, long y);

		void Show(bool show);

		void Clip(void* window);
	}

	////////////
	// Window //
	////////////

	namespace Window
	{
		void ConvertPosToWindow(void* window, long& x, long& y);
		void ConvertPosToScreen(void* window, long& x, long& y);
	}
}
