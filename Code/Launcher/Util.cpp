/**
 * @file
 * @brief Implementation of utilities.
 */

#include <cstring>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Util.h"

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

	// 0x90 is opcode of NOP instruction on both x86 and x86_64
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
 * @brief Obtains game version from any Crysis DLL.
 * It parses version resource of the specified file.
 * @param pDLL Handle to any loaded Crysis DLL.
 * @return Game build number or -1 if some error occurred.
 */
int Util::GetCrysisGameVersion(void *pDLL)
{
	HMODULE dll = static_cast<HMODULE>(pDLL);

	// VERSIONINFO resource always has ID 1
	HRSRC versionResInfo = FindResource(dll, MAKEINTRESOURCE(1), RT_VERSION);
	if (!versionResInfo)
		return -1;

	HGLOBAL versionResData = LoadResource(dll, versionResInfo);
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
