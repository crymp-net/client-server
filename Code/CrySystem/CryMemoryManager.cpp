#include <cstring>
#include <mimalloc.h>
#include <mimalloc-new-delete.h>

// TODO: remove all malloc calls
// TODO: hook msvcr80.malloc

#include "CryCommon/CrySystem/ISystem.h"
#include "Library/WinAPI.h"

#include "CryMemoryManager.h"

static void* CryMalloc_hook(std::size_t size, std::size_t& allocatedSize)
{
	void* result = nullptr;

	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SYSTEM);

		result = mi_zalloc(size);
	}
	else
	{
		result = mi_zalloc(size);
	}

	allocatedSize = size;

	return result;
}

static void* CryRealloc_hook(void* mem, std::size_t size, std::size_t& allocatedSize)
{
	void* result = nullptr;

	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SYSTEM);

		result = mi_rezalloc(mem, size);
	}
	else
	{
		result = mi_rezalloc(mem, size);
	}

	allocatedSize = size;

	return result;
}

static std::size_t CryGetMemSize_hook(void* mem, std::size_t)
{
	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SYSTEM);

		return mi_usable_size(mem);
	}
	else
	{
		return mi_usable_size(mem);
	}
}

static std::size_t CryFree_hook(void* mem)
{
	std::size_t result = 0;

	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SYSTEM);

		result = mi_usable_size(mem);
		mi_free(mem);
	}
	else
	{
		result = mi_usable_size(mem);
		mi_free(mem);
	}

	return result;
}

static void* CrySystemCrtMalloc_hook(std::size_t size)
{
	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SYSTEM);

		return mi_zalloc(size);
	}
	else
	{
		return mi_zalloc(size);
	}
}

static void CrySystemCrtFree_hook(void* mem)
{
	if (gEnv)
	{
		FUNCTION_PROFILER(gEnv->pSystem, PROFILE_SYSTEM);

		mi_free(mem);
	}
	else
	{
		mi_free(mem);
	}
}

static void Hook(void* pFunc, void* pNewFunc)
{
	if (!pFunc)
	{
		return;
	}

#ifdef BUILD_64BIT
	unsigned char code[] = {
		0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // mov rax, 0x0
		0xFF, 0xE0                                                   // jmp rax
	};

	std::memcpy(&code[2], &pNewFunc, 8);
#else
	unsigned char code[] = {
		0xB8, 0x00, 0x00, 0x00, 0x00,  // mov eax, 0x0
		0xFF, 0xE0                     // jmp eax
	};

	std::memcpy(&code[1], &pNewFunc, 4);
#endif

	WinAPI::FillMem(pFunc, code, sizeof code);
}

void CryMemoryManager::Init(void* pCrySystem)
{
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryMalloc"), CryMalloc_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryRealloc"), CryRealloc_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryGetMemSize"), CryGetMemSize_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CryFree"), CryFree_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CrySystemCrtMalloc"), CrySystemCrtMalloc_hook);
	Hook(WinAPI::DLL::GetSymbol(pCrySystem, "CrySystemCrtFree"), CrySystemCrtFree_hook);
}
