#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "DLL.h"

void *DLL::getSymbolAddress(const char *name) const
{
	if (!m_handle)
	{
		return NULL;
	}

	return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(m_handle), name));
}

void DLL::unload()
{
	FreeLibrary(static_cast<HMODULE>(m_handle));

	m_handle = NULL;
}

bool DLL::load(const char *file)
{
	release();

	if (file)
	{
		m_handle = LoadLibraryA(file);
	}

	return isLoaded();
}
