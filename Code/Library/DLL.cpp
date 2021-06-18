#include "DLL.h"
#include "WinAPI.h"

void DLL::release()
{
	WinAPI::DLL_Unload(m_handle);
}

bool DLL::load(const char *name, int flags)
{
	unload();

	m_flags = flags;
	m_handle = (flags & NO_LOAD) ? WinAPI::DLL_Get(name) : WinAPI::DLL_Load(name);

	return isLoaded();
}

void *DLL::getSymbolAddress(const char *name) const
{
	return isLoaded() ? WinAPI::DLL_GetSymbol(m_handle, name) : nullptr;
}
