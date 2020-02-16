/**
 * @file
 * @brief Implementation of virtual function table hook.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "VTable.h"

/**
 * @brief Sets value of one entry in vtable (performs vtable hook).
 * @param pEntry Pointer to the vtable entry.
 * @param pNewFunc Address of the new function.
 * @return True if no error occurred, otherwise false.
 */
static bool ReplaceVTableEntry(void **pEntry, void *pNewFunc)
{
	DWORD oldProtection;

	if (VirtualProtect(pEntry, sizeof (void*), PAGE_EXECUTE_READWRITE, &oldProtection) == 0)
		return false;

	(*pEntry) = pNewFunc;

	if (VirtualProtect(pEntry, sizeof (void*), oldProtection, &oldProtection) == 0)
		return false;

	return true;
}

bool VTableHook(void *pVTable, unsigned int index, void *pNewFunc)
{
	if (!pVTable)
	{
		return false;
	}

	void **pEntry = static_cast<void**>(pVTable) + index;

	if (!ReplaceVTableEntry(pEntry, pNewFunc))
	{
		return false;
	}

	return true;
}
