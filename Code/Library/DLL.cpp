#include "DLL.h"
#include "WinAPI.h"

void DLL::Release()
{
	WinAPI::DLL_Unload(m_handle);
}

bool DLL::Load(const char *name, int flags)
{
	Unload();

	m_flags = flags;
	m_handle = (flags & NO_LOAD) ? WinAPI::DLL_Get(name) : WinAPI::DLL_Load(name);

	return IsLoaded();
}

void *DLL::GetSymbolAddress(const char *name) const
{
	return IsLoaded() ? WinAPI::DLL_GetSymbol(m_handle, name) : nullptr;
}
