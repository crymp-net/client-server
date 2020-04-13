/**
 * @file
 * @brief Implementation of launcher utilities.
 */

#include <cctype>
#include <cstring>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Library/StringBuffer.h"

#include "Util.h"

static bool IsDirectorySeparator(char ch)
{
	return ch == '/' || ch == '\\';
}

/**
 * @brief Finds substring in string without case sensitivity.
 * @param buffer The null-terminated byte string to examine.
 * @param string The null-terminated byte substring to search for.
 * @return Pointer to the first character of the found substring or NULL.
 */
const char *Util::FindStringNoCase(const char *buffer, const char *string)
{
	if (!buffer || !string || !*string)
	{
		return buffer;
	}

	for (; *buffer; buffer++)
	{
		if (std::tolower(*buffer) != std::tolower(*string))
		{
			continue;
		}

		const char *b = buffer + 1;
		const char *s = string + 1;

		while (*b && *s)
		{
			if (std::tolower(*b) != std::tolower(*s))
			{
				break;
			}

			b++;
			s++;
		}

		if (!*s)
		{
			return buffer;
		}
	}

	return nullptr;
}

/**
 * @brief Fills read-only memory region with x86 NOP instruction.
 * @param address Address of the memory region.
 * @param length Size of the memory region in bytes.
 * @return True if no error occurred, otherwise false.
 */
bool Util::FillNOP(void *address, size_t length)
{
	DWORD oldProtection;

	if (VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &oldProtection) == 0)
		return false;

	// 0x90 is opcode of NOP instruction on both x86 and x86-64
	std::memset(address, '\x90', length);

	if (VirtualProtect(address, length, oldProtection, &oldProtection) == 0)
		return false;

	return true;
}

/**
 * @brief Fills read-only memory region with custom data.
 * The memory region and the data must not overlap.
 * @param address Address of the memory region.
 * @param data Address of the data.
 * @param length Size of the data in bytes.
 * @return True if no error occurred, otherwise false.
 */
bool Util::FillMem(void *address, const void *data, size_t length)
{
	DWORD oldProtection;

	if (VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &oldProtection) == 0)
		return false;

	std::memcpy(address, data, length);

	if (VirtualProtect(address, length, oldProtection, &oldProtection) == 0)
		return false;

	return true;
}

/**
 * @brief Checks if directory exists.
 * @param path The path to check.
 * @return True if the path points to directory, otherwise false.
 */
bool Util::DirectoryExists(const Path & path)
{
	DWORD attributes = GetFileAttributesA(path.toString(Path::WINDOWS).c_str());

	return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

/**
 * @brief Changes working directory.
 * @param path The new working directory.
 * @return True if no error occurred, otherwise false.
 */
bool Util::SetWorkingDirectory(const Path & path)
{
	return SetCurrentDirectoryA(path.toString(Path::WINDOWS).c_str()) != 0;
}

/**
 * @brief Adds a directory to the search path used when loading DLLs.
 * @param path The additional directory with DLLs.
 * @return True if no error occurred, otherwise false.
 */
bool Util::AddDLLDirectory(const Path & path)
{
	return SetDllDirectoryA(path.toString(Path::WINDOWS).c_str()) != 0;
}

/**
 * @brief Returns the current application executable file name.
 * @return Name of application executable or empty string if some error occurred.
 */
std::string Util::GetApplicationFileName()
{
	char buffer[4096];

	DWORD length = GetModuleFileNameA(NULL, buffer, sizeof buffer);
	if (length == 0 || length >= sizeof buffer)
	{
		return std::string();
	}

	const char *filename = buffer;

	// find the executable file name
	for (size_t i = length; i-- > 0;)
	{
		if (IsDirectorySeparator(buffer[i]))
		{
			filename = buffer + i + 1;
			break;
		}
	}

	return std::string(filename);
}

/**
 * @brief Returns path to directory where executable file of the current application is stored.
 * @return Path to application directory or empty path if some error occurred.
 */
Path Util::GetApplicationDirectory()
{
	char buffer[4096];

	DWORD length = GetModuleFileNameA(NULL, buffer, sizeof buffer);
	if (length == 0 || length >= sizeof buffer)
	{
		return Path();
	}

	// remove the executable file name
	for (size_t i = length; i-- > 0;)
	{
		if (IsDirectorySeparator(buffer[i]))
		{
			buffer[i] = '\0';
			length = static_cast<DWORD>(i);
			break;
		}
	}

	return Path(buffer);
}

/**
 * @brief Returns calling thread ID.
 * @return ID of the current thread.
 */
unsigned long Util::GetCurrentThreadID()
{
	return ::GetCurrentThreadId();
}

/**
 * @brief Obtains game version from any Crysis DLL.
 * It parses version resource of the specified file.
 * @param pDLL Handle to any loaded Crysis DLL.
 * @return Game build number or -1 if some error occurred.
 */
int Util::GetCrysisGameVersion(void *pDLL)
{
	HMODULE module = static_cast<HMODULE>(pDLL);
	if (!module)
		return -1;

	// VERSIONINFO resource always has ID 1
	HRSRC versionResInfo = FindResource(module, MAKEINTRESOURCE(1), RT_VERSION);
	if (!versionResInfo)
		return -1;

	HGLOBAL versionResData = LoadResource(module, versionResInfo);
	if (!versionResData)
		return -1;

	void *versionRes = LockResource(versionResData);  // this function does nothing
	if (!versionRes)
		return -1;

	if (std::memcmp(RVA(versionRes, 0x6), L"VS_VERSION_INFO", 0x20) != 0)
		return -1;

	VS_FIXEDFILEINFO *pFileInfo = static_cast<VS_FIXEDFILEINFO*>(RVA(versionRes, 0x6 + 0x20 + 0x2));
	if (pFileInfo->dwSignature != 0xFEEF04BD)
		return -1;

	return LOWORD(pFileInfo->dwFileVersionLS);
}

/**
 * @brief Shows message box with error message and blocks until user closes it.
 * It also shows current last-error code and its description.
 * @param msg The error message to show.
 */
void Util::ErrorBox(const char *msg)
{
	DWORD errorCode = GetLastError();

	StringBuffer<1024> buffer;

	buffer += msg;
	buffer += '\n';

	if (errorCode)
	{
		buffer.append_f("Error %ld", errorCode);

		char errorMsgBuffer[512];
		if (FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorCode,
		                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), errorMsgBuffer, sizeof errorMsgBuffer, NULL))
		{
			buffer += ": ";
			buffer += errorMsgBuffer;
		}
	}

	MessageBoxA(NULL, buffer.get(), "Error", MB_OK | MB_ICONERROR);
}
